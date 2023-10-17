#include "database.hpp"

using namespace mcp::db;

std::shared_ptr<rocksdb::Cache> mcp::db::database::table_cache = nullptr;
std::shared_ptr<rocksdb::SstFileManager> mcp::db::database::rocksdb_sst_file_manager = std::shared_ptr<rocksdb::SstFileManager>(rocksdb::NewSstFileManager(rocksdb::Env::Default(), nullptr, "", 0));
uint64_t mcp::db::database_config::write_buffer_size = 1024;
bool mcp::db::database_config::cache_filter = true;
//check return status
void mcp::db::check_status(rocksdb::Status const& _status)
{
	if (_status.ok())
		return;

	if (!_status.ok())
	{
		assert_x_msg(_status.ok(), "rocksDB operate error, msg:" + _status.ToString());
	}
}


void mcp::db::database_config::serialize_json(mcp::json &json_a) const
{
	json_a["cache"] = cache_size;
	json_a["write_buffer"] = write_buffer_size;
	json_a["cache_filter"] = cache_filter ? "true" : "false";
}

bool mcp::db::database_config::deserialize_json(mcp::json const & json_a)
{
	auto error(false);
	try
	{
		if (json_a.count("cache") && json_a["cache"].is_number_unsigned())
			cache_size = json_a["cache"].get<std::uint64_t>();
		if (json_a.count("write_buffer") && json_a["write_buffer"].is_number_unsigned())
			write_buffer_size = json_a["write_buffer"].get<std::uint64_t>();
		if (json_a.count("cache_filter") && json_a["cache_filter"].is_string())
			cache_filter = (json_a["cache_filter"].get<std::string>() == "true" ? true : false);
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

bool mcp::db::database_config::parse_old_version_data(mcp::json const & json_a, uint64_t const& version)
{
	auto error(false);
	try
	{
		/// parse json used low version
		switch (version)
		{
			//case 0:
			//{
			//	/// parse
			//	break;
			//}
			//case 1:
			//{
			//	/// parse
			//	break;
			//}
		default:
			error |= deserialize_json(json_a);
			break;
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

mcp::db::database::database(boost::filesystem::path const& path_a) :
	m_db(nullptr),
	m_path(path_a.string()),
	m_column(std::make_shared<db_column>()),
	m_read_options(std::move(default_read_options())),
	m_write_options(std::move(default_write_options()))
{
	auto tbops = mcp::db::db_column::default_table_options(mcp::db::database::get_table_cache());
	auto cfops = mcp::db::db_column::default_column_family_options(tbops);
	cfops->merge_operator = std::make_shared<count_merge_operator>();
	cfops->max_successive_merges = 1000;
	m_count = set_column_family(create_column_family("count", cfops));
}

mcp::db::database::~database()
{
	for (auto handle : m_column->m_handles)
	{
		delete handle;
	}

	if (nullptr != m_db)
		delete m_db;
}

bool mcp::db::database::open()
{
	rocksdb::Status status = open_rocksdb(m_path);
	if (status.ok())
	{
		//m_column->preserve_index();
	}
	else
	{
		LOG(m_log.error) << "open database error, msg:" << status.ToString();
		return false;
	}

	return true;
}


void mcp::db::database::put(int const& _index, dev::Slice const& _k, dev::Slice const& _v,
	std::shared_ptr<rocksdb::WriteOptions> write_ops_a)
{
	std::shared_ptr<rocksdb::WriteOptions> write_ops(write_ops_a);
	if (nullptr == write_ops)
		write_ops = m_write_options;

	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = get_column_family_handle(_index, info);

	dev::Slicebytes key;
	if (info->shared)
	{
		key.resize(info->prefix.size() + _k.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key));
		_k.copyTo(dev::SliceRef(&key).cropped(info->prefix.size()));
	}
	else
	{
		key.resize(_k.size());
		_k.copyTo(dev::SliceRef(&key));
	}

	rocksdb::Status status = m_db->Put(
		*write_ops,
		handle,
		rocksdb::Slice(key.data(), key.size()),
		rocksdb::Slice(_v.data(), _v.size())
	);

	check_status(status);
}

bool mcp::db::database::get(int const& _index, dev::Slice const& _k, std::string& _v,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = get_column_family_handle(_index, info);

	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = m_read_options;

	dev::Slicebytes key;
	if (info->shared)
	{
		key.resize(info->prefix.size() + _k.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key));
		_k.copyTo(dev::SliceRef(&key).cropped(info->prefix.size()));
	}
	else
	{
		key.resize(_k.size());
		_k.copyTo(dev::SliceRef(&key));
	}

	rocksdb::Status status = m_db->Get(
		*read_ops,
		handle,
		rocksdb::Slice(key.data(), key.size()),
		&_v
	);

	if (status.ok())
	{
		return true;
	}
	else if (status.IsNotFound())
	{
		return false;
	}
	check_status(status);
}

void mcp::db::database::del(int const& _index, dev::Slice const& _k,
	std::shared_ptr<rocksdb::WriteOptions> write_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = get_column_family_handle(_index, info);

	dev::Slicebytes key;
	if (info->shared)
	{
		key.resize(info->prefix.size() + _k.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key));
		_k.copyTo(dev::SliceRef(&key).cropped(info->prefix.size()));
	}
	else
	{
		key.resize(_k.size());
		_k.copyTo(dev::SliceRef(&key));
	}

	std::shared_ptr<rocksdb::WriteOptions> write_ops(write_ops_a);
	if (nullptr == write_ops)
		write_ops = m_write_options;

	rocksdb::Status status = m_db->Delete(
		*write_ops,
		handle,
		rocksdb::Slice(key.data(), key.size()));
	check_status(status);
}

///start include , end not include
void mcp::db::database::del_range(int const& index, dev::Slice const& start, dev::Slice const& end,
	std::shared_ptr<rocksdb::WriteOptions> write_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = get_column_family_handle(index, info);

	dev::Slicebytes key_start, key_end;
	if (info->shared)
	{
		key_start.resize(info->prefix.size() + start.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key_start));
		start.copyTo(dev::SliceRef(&key_start).cropped(info->prefix.size()));

		key_end.resize(info->prefix.size() + end.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key_end));
		end.copyTo(dev::SliceRef(&key_end).cropped(info->prefix.size()));
	}
	else
	{
		key_start.resize(start.size());
		start.copyTo(dev::SliceRef(&key_start));

		key_end.resize(end.size());
		end.copyTo(dev::SliceRef(&key_end));
	}

	std::shared_ptr<rocksdb::WriteOptions> write_ops(write_ops_a);
	if (nullptr == write_ops)
		write_ops = m_write_options;

	rocksdb::Status status = m_db->GetBaseDB()->DeleteRange(
		*write_ops,
		handle,
		rocksdb::Slice(key_start.data(), key_start.size()),
		rocksdb::Slice(key_end.data(), key_end.size())
	);
	check_status(status);
}

bool mcp::db::database::exists(int const& _index, dev::Slice const & _k,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = m_read_options;

	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = get_column_family_handle(_index, info);

	dev::Slicebytes key;
	if (info->shared)
	{
		key.resize(info->prefix.size() + _k.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key));
		_k.copyTo(dev::SliceRef(&key).cropped(info->prefix.size()));
	}
	else
	{
		key.resize(_k.size());
		_k.copyTo(dev::SliceRef(&key));
	}

	std::string value;
	rocksdb::Status status = m_db->Get(
		*read_ops,
		handle,
		rocksdb::Slice(key.data(), key.size()),
		&value);
	if (status.ok())
		return true;
	return false;
}

//bool mcp::db::database::merge(int const& _index, dev::Slice const & _k, dev::Slice const & _v,
//	std::shared_ptr<rocksdb::WriteOptions> write_ops_a)
//{
//	std::shared_ptr<rocksdb::WriteOptions> write_ops(write_ops_a);
//	if (nullptr == write_ops)
//		write_ops = m_write_options;
//
//	auto handle = get_column_family_handle(_index);
//	rocksdb::Status status = m_db->Merge(
//		*write_ops,
//		handle,
//		rocksdb::Slice(_k.data(), _k.size()),
//		rocksdb::Slice(_v.data(), _v.size())
//	);
//	if (status.ok())
//		return true;
//	check_status(status);
//}

/*return a db_transaction*/
mcp::db::db_transaction mcp::db::database::create_transaction(std::shared_ptr<rocksdb::WriteOptions> write_options_a, std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a)
{
	return db_transaction(*this, write_options_a, txn_ops_a);
}

int mcp::db::database::create_column_family(std::string const & name_a, std::shared_ptr<rocksdb::ColumnFamilyOptions> cfops)
{
	return m_column->insert_column_families(name_a, cfops);
}

int mcp::db::database::set_column_family(int index_a, std::string const & name_a)
{
	if (m_column->m_column_families.size() <= index_a)//not insert, need call create colume first 
		assert_x_msg(false, "use column family need call create first");

	int index = m_index.size();

	index_info info;
	info.col_index = index_a;
	if (name_a.size() > 0)
	{
		info.prefix = name_a;
		info.shared = true;
	}

	m_index.insert(std::make_pair(index, info));
	return index;
}

std::shared_ptr<rocksdb::ManagedSnapshot> mcp::db::database::create_snapshot()
{ 
	return std::make_shared<rocksdb::ManagedSnapshot>(m_db, m_db->GetSnapshot());
}


std::shared_ptr<rocksdb::ReadOptions> mcp::db::database::default_read_options()
{
	return std::make_shared<rocksdb::ReadOptions>(rocksdb::ReadOptions());
}

std::shared_ptr<rocksdb::WriteOptions> mcp::db::database::default_write_options()
{
	return std::make_shared<rocksdb::WriteOptions>(rocksdb::WriteOptions());
}

rocksdb::Options mcp::db::database::default_DB_options()
{
	//see https://github.com/facebook/rocksdb/wiki/Setup-Options-and-Basic-Tuning for detail
	rocksdb::Options options;
	//options.max_open_files = 128;
	options.create_if_missing = true;
	options.create_missing_column_families = true;
	//options.max_file_opening_threads = 4;
	options.db_write_buffer_size = 512 * 1024 * 1024;
	options.max_background_jobs = 8;
	options.max_subcompactions = 4;
	//options.table_cache_numshardbits = 6;
	//options.allow_mmap_reads = true;
	//options.bytes_per_sync = 1 * 1024 * 1024;
	options.compaction_pri = rocksdb::kMinOverlappingRatio;
	options.sst_file_manager= rocksdb_sst_file_manager;
	options.atomic_flush = true;
	options.max_total_wal_size = 512 * 1024 * 1024;
	//options.wal_bytes_per_sync = 8 * 1024 * 1024;;
	//options.enable_pipelined_write = true;
	//options.new_table_reader_for_compaction_inputs = true;
	//options.table_factory.reset(NewBlockBasedTableFactory(m_column->default_table_options()));
	//options.row_cache = rocksdb::NewLRUCache(6 * 1024 * 1024 * 1024);

	//options.rate_limiter.reset(rocksdb::NewGenericRateLimiter(10 * 1024 * 1024));
	//options.statistics = rocksdb::CreateDBStatistics();
	//options.prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(3));
	return options;
}

rocksdb::TransactionDBOptions mcp::db::database::default_trans_DB_options()
{
	return rocksdb::TransactionDBOptions();
}

rocksdb::Status mcp::db::database::open_rocksdb(std::string path_a)
{
	rocksdb::Status status = rocksdb::TransactionDB::Open(
		default_DB_options(),
		default_trans_DB_options(),
		path_a,
		m_column->m_column_families,
		&m_column->m_handles,
		&m_db);

	return status;
}

//void mcp::db::database::create_column()
//{
//	int index = 1;
//	auto it = m_column->m_column_families.begin();
//	it++;	//first is default
//	for (; it != m_column->m_column_families.end(); it++)
//	{
//		rocksdb::ColumnFamilyHandle* cf;
//		rocksdb::Status s = m_db->CreateColumnFamily(it->options, it->name, &cf);
//		int ret = cf->GetID();
//		delete cf;
//		check_status(s);
//		m_column->insert_index(index++, ret);
//	}
//}

rocksdb::ColumnFamilyHandle* mcp::db::database::get_column_family_handle(int index, std::shared_ptr<index_info> info)
{
	auto it = m_index.find(index);
	if (it == m_index.end())
		return nullptr;

	if (info)
	{
		info->col_index = it->second.col_index;
		info->prefix = it->second.prefix;
		info->shared = it->second.shared;
	}

	return m_column->get_column_family_handle(it->second.col_index);
}

std::string mcp::db::database::get_rocksdb_state(uint64_t limit)
{
	size_t block_cache_usage = table_cache->GetUsage();
	size_t block_pinned_usage = table_cache->GetPinnedUsage();

	std::string str = "";
	str = "Rocksdb chche: block cache usage:" + std::to_string(block_cache_usage)
		+ " , block_pinned_usage:" + std::to_string(block_pinned_usage) + " , ";

	std::string size = "";
	uint64_t total = 0;

	for (auto it : m_column->m_handles)
	{
		std::string indexes_filter_size;
		m_db->GetProperty(it, "rocksdb.estimate-table-readers-mem", &indexes_filter_size);

		std::string memtable_size;
		m_db->GetProperty(it, "rocksdb.cur-size-all-mem-tables", &memtable_size);

		uint64_t filter_size = boost::lexical_cast<uint64_t>(indexes_filter_size);
		uint64_t table_size = boost::lexical_cast<uint64_t>(memtable_size);
		total = total + filter_size + table_size;

		if (filter_size + table_size > limit)
			size = size + it->GetName() + ":[filter:" + indexes_filter_size + " , memtable:" + memtable_size + "] \n";
	}

	str = str + "filter and memtable size:" + std::to_string(total) + " \n " + size;
	return str;
}

