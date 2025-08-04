#include "database.hpp"
#include <mcp/common/assert.hpp>
#include <rocksdb/sst_file_manager.h>
#include <rocksdb/filter_policy.h>

using namespace mcp::db;

std::shared_ptr<rocksdb::BlockBasedTableOptions> mcp::db::defaultBlockBasedTableOptions(uint64_t size)
{
	rocksdb::BlockBasedTableOptions table_options;

	table_options.block_cache = rocksdb::NewLRUCache(size * 1024 * 1024);

	//Partitioned Index Filters
	if (database_config::cache_filter)
	{
		table_options.index_type = rocksdb::BlockBasedTableOptions::IndexType::kTwoLevelIndexSearch;
		table_options.partition_filters = true;
		table_options.metadata_block_size = 4 * 1024;
	}

	table_options.data_block_index_type = rocksdb::BlockBasedTableOptions::DataBlockIndexType::kDataBlockBinaryAndHash;
	table_options.filter_policy.reset(rocksdb::NewBloomFilterPolicy(10, false));
	table_options.format_version = 4;
	table_options.block_size = 16 * 1024;
	return std::make_shared<rocksdb::BlockBasedTableOptions>(table_options);
}

std::shared_ptr<rocksdb::Options> mcp::db::defaultDBOptions(std::shared_ptr<rocksdb::BlockBasedTableOptions> base)
{
	//see https://github.com/facebook/rocksdb/wiki/Setup-Options-and-Basic-Tuning for detail
	rocksdb::Options options;
	options.create_if_missing = true;
	options.create_missing_column_families = true;
	options.db_write_buffer_size = 512 * 1024 * 1024;
	options.max_background_jobs = 8;
	options.max_subcompactions = 4;
	options.compaction_pri = rocksdb::kMinOverlappingRatio;
	options.sst_file_manager = std::shared_ptr<rocksdb::SstFileManager>(rocksdb::NewSstFileManager(rocksdb::Env::Default(), nullptr, "", 0));
	options.atomic_flush = true;
	options.max_total_wal_size = 512 * 1024 * 1024;

	options.compression = rocksdb::CompressionType::kLZ4Compression;
	options.write_buffer_size = 128 * 1024 * 1024;
	options.max_bytes_for_level_base = 1024 * 1024 * 1024;
	options.level_compaction_dynamic_level_bytes = true;
	options.target_file_size_base = 128 * 1024 * 1024;
	options.table_factory.reset(NewBlockBasedTableFactory(*base));

	return std::make_shared<rocksdb::Options>(options);
}

mcp::db::database::database(boost::filesystem::path const& path_a, std::shared_ptr<rocksdb::Options> Ops) :
	m_db(nullptr),
	m_read_options(std::move(default_read_options())),
	m_write_options(std::move(default_write_options()))
{
	rocksdb::Status status = rocksdb::TransactionDB::Open(
		*Ops,
		rocksdb::TransactionDBOptions(),
		path_a.string(),
		&m_db);

	mcp::db::check_status(status);
}

mcp::db::database::~database()
{
	if (nullptr != m_db)
		delete m_db;
}

namespace mcp
{
	class RocksDBWriteBatch : public WriteBatchFace
	{
	public:
		void insert(Slice _key, Slice _value) override;
		void kill(Slice _key) override;

		rocksdb::WriteBatch const& writeBatch() const { return m_writeBatch; }
		rocksdb::WriteBatch& writeBatch() { return m_writeBatch; }

	private:
		rocksdb::WriteBatch m_writeBatch;
	};

	void RocksDBWriteBatch::insert(Slice _key, Slice _value)
	{
		auto const status = m_writeBatch.Put(
			rocksdb::Slice(_key.data(), _key.size()),
			rocksdb::Slice(_value.data(), _value.size())
		);
		db::check_status(status);
	}

	void RocksDBWriteBatch::kill(Slice _key)
	{
		auto const status = m_writeBatch.Delete(rocksdb::Slice(_key.data(), _key.size()));
		db::check_status(status);
	}
}

std::string mcp::db::database::lookup(Slice _key) const
{
	rocksdb::Slice const key(_key.data(), _key.size());
	std::string value;
	auto const status = m_db->Get(*m_read_options, key, &value);
	if (status.IsNotFound())
		return std::string();

	mcp::db::check_status(status);
	return value;
}

bool mcp::db::database::exists(Slice _key) const
{
	std::string value;
	rocksdb::Slice const key(_key.data(), _key.size());
	if (!m_db->KeyMayExist(*m_read_options, key, &value, (bool*)nullptr))
		return false;

	auto const status = m_db->Get(*m_read_options, key, &value);
	if (status.IsNotFound())
		return false;

	mcp::db::check_status(status);
	return true;
}

void mcp::db::database::insert(Slice _key, Slice _value)
{
	rocksdb::Slice const key(_key.data(), _key.size());
	rocksdb::Slice const value(_value.data(), _value.size());
	auto const status = m_db->Put(*m_write_options, key, value);
	mcp::db::check_status(status);
}

void mcp::db::database::kill(Slice _key)
{
	rocksdb::Slice const key(_key.data(), _key.size());
	auto const status = m_db->Delete(*m_write_options, key);
	//checkStatus(status);
	mcp::db::check_status(status);
}

std::unique_ptr<WriteBatchFace> mcp::db::database::createWriteBatch() const
{
	return std::unique_ptr<WriteBatchFace>(new RocksDBWriteBatch());
}

void mcp::db::database::commit(std::unique_ptr<WriteBatchFace> _batch)
{
	auto a = DatabaseError() << dev::errinfo_comment("Cannot commit null batch");
	if (!_batch)
		BOOST_THROW_EXCEPTION(DatabaseError() << dev::errinfo_comment("Cannot commit null batch"));

	auto* batchPtr = dynamic_cast<RocksDBWriteBatch*>(_batch.get());
	if (!batchPtr)
		BOOST_THROW_EXCEPTION(DatabaseError() << dev::errinfo_comment("Invalid batch type passed to rocksdb::commit"));

	auto const status = m_db->Write(*m_write_options, &batchPtr->writeBatch());
	//checkStatus(status);
	mcp::db::check_status(status);
}

void mcp::db::database::forEach(std::function<bool(Slice, Slice)> f) const
{
	std::unique_ptr<rocksdb::Iterator> itr(m_db->NewIterator(*m_read_options));
	if (itr == nullptr)
		BOOST_THROW_EXCEPTION(DatabaseError() << dev::errinfo_comment("null iterator"));

	auto keepIterating = true;
	for (itr->SeekToFirst(); keepIterating && itr->Valid(); itr->Next())
	{
		auto const dbKey = itr->key();
		auto const dbValue = itr->value();
		Slice const key(dbKey.data(), dbKey.size());
		Slice const value(dbValue.data(), dbValue.size());
		keepIterating = f(key, value);
	}
}

///start include , end not include
void mcp::db::database::del_range(uint8_t const& _prefix, dev::Slice const& start, dev::Slice const& end,
	std::shared_ptr<rocksdb::WriteOptions> write_ops_a)
{
	dev::Slicebytes prefix = dev::Slicebytes(1, _prefix);
	dev::Slicebytes key_start = prefix + start;
	dev::Slicebytes key_end = prefix + end;
	std::shared_ptr<rocksdb::WriteOptions> write_ops(write_ops_a);
	if (nullptr == write_ops)
		write_ops = m_write_options;

	rocksdb::Status status = m_db->GetBaseDB()->DeleteRange(
		*write_ops,
		m_db->GetBaseDB()->DefaultColumnFamily(),
		rocksdb::Slice(key_start.data(), key_start.size()),
		rocksdb::Slice(key_end.data(), key_end.size())
	);
	check_status(status);
}

/*return a db_transaction*/
mcp::db::db_transaction mcp::db::database::create_transaction(std::shared_ptr<rocksdb::WriteOptions> write_options_a, std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a)
{
	return db_transaction(*this, write_options_a, txn_ops_a);
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

std::string mcp::db::database::get_rocksdb_state(uint64_t limit)
{
	std::string str = "Rocksdb ";
	std::string block_cache_usage;
	m_db->GetProperty(rocksdb::DB::Properties::kBlockCacheUsage, &block_cache_usage);
	str = str + "block-cache-usage:" + block_cache_usage + ";";

	std::string block_cache_pinned_usage;
	m_db->GetProperty(rocksdb::DB::Properties::kBlockCachePinnedUsage, &block_cache_pinned_usage);
	str = str + "block-cache-pinned-usage:" + block_cache_pinned_usage + ";";

	std::string table_readers_mem;
	m_db->GetProperty(rocksdb::DB::Properties::kEstimateTableReadersMem, &table_readers_mem);
	str = str + "estimate-table-readers-mem:" + table_readers_mem + ";";

	std::string memtable_size;
	m_db->GetProperty(rocksdb::DB::Properties::kCurSizeAllMemTables, &memtable_size);
	str = str + "cur-size-all-mem-tables:" + memtable_size + ";";

	std::string sst_size;
	m_db->GetProperty(rocksdb::DB::Properties::kTotalSstFilesSize, &sst_size);
	str = str + "total-sst-files-size:" + sst_size + ";";

	std::string num_keys;
	m_db->GetProperty(rocksdb::DB::Properties::kEstimateNumKeys, &num_keys);
	str = str + "estimate-num-keys:" + num_keys + ";";

	std::string compact_pending;
	m_db->GetProperty(rocksdb::DB::Properties::kCompactionPending, &compact_pending);
	str = str + "compaction-pending:" + compact_pending + ";";

	std::string num_immutable_mem_table;
	m_db->GetProperty(rocksdb::DB::Properties::kNumImmutableMemTable, &num_immutable_mem_table);
	str = str + "num-immutable-mem-table:" + num_immutable_mem_table + ";";

	std::string num_running_compactions;
	m_db->GetProperty(rocksdb::DB::Properties::kNumRunningCompactions, &num_running_compactions);
	str = str + "num-running-compactions:" + num_running_compactions + ";";

	std::string stats;
	m_db->GetProperty(rocksdb::DB::Properties::kStats, &stats);
	str = str + "stats:" + stats + ";";

	return str;
}

