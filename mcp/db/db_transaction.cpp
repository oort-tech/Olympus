#include "db_transaction.hpp"
#include <boost/endian/conversion.hpp>

mcp::db::db_transaction::db_transaction(mcp::db::database& m_db_a,
	std::shared_ptr<rocksdb::WriteOptions> write_options_a,
	std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a
	) :
	m_db(m_db_a),
	m_commited_or_rollbacked(false),
	m_read_only(true)
{
	std::shared_ptr<rocksdb::WriteOptions> write_ops(write_options_a);
	std::shared_ptr<rocksdb::TransactionOptions> txn_ops(txn_ops_a);
	
	if (nullptr == write_ops)
		write_ops = m_db.default_write_options();
	if (nullptr == txn_ops)
		txn_ops = default_trans_options();

	m_txn = m_db_a.get_db()->BeginTransaction(*write_ops, *txn_ops);
}

mcp::db::db_transaction::db_transaction(mcp::db::db_transaction && other_a):
	m_db(other_a.m_db)
{
	m_txn = other_a.m_txn;
	other_a.m_txn = nullptr;
	m_commited_or_rollbacked = other_a.m_commited_or_rollbacked;
	m_read_only = other_a.m_read_only;
}

mcp::db::db_transaction::~db_transaction()
{
	if (!m_commited_or_rollbacked)
		commit();
	if (m_txn)
		delete m_txn;
}

std::shared_ptr<rocksdb::TransactionOptions> mcp::db::db_transaction::default_trans_options()
{
	return std::make_shared<rocksdb::TransactionOptions>(rocksdb::TransactionOptions());
}

void mcp::db::db_transaction::put(int const& index, dev::Slice const& _k, dev::Slice const& _v)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);
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

	rocksdb::Status status = m_txn->Put(
		handle,
		rocksdb::Slice(key.data(), key.size()),
		rocksdb::Slice(_v.data(), _v.size())
	);
	m_read_only = false;

	check_status(status);
}

/*	input:	_k is key, _v is value
*	return
*	true :	key exist , _v is data
*  false:	key is not exist, _v is null
*/
bool mcp::db::db_transaction::get(int const& index, dev::Slice const& _k, std::string& _v, 
	std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);
	
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

	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = mcp::db::database::default_read_options();

	if (snapshot_a)
		read_ops->snapshot = snapshot_a->snapshot();

	rocksdb::Status status = m_txn->Get(
		*read_ops,
		handle,
		rocksdb::Slice(key.data(), key.size()),
		&_v
	);

	if (status.ok())
		return true;

	if (status.IsNotFound())
		return false;

	check_status(status);
	return false;
}

void mcp::db::db_transaction::del(int const& index, dev::Slice const& _k)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);
	
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

	rocksdb::Status status = m_txn->Delete(
		handle,
		rocksdb::Slice(key.data(), key.size())
	);
	m_read_only = false;

	check_status(status);
}

bool mcp::db::db_transaction::exists(int const& index, dev::Slice const & _k, 
	std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);
	
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

	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = mcp::db::database::default_read_options();

	if (snapshot_a)
		read_ops->snapshot = snapshot_a->snapshot();

	std::string value;
	rocksdb::Status status = m_txn->Get(
		*read_ops,
		handle,
		rocksdb::Slice(key.data(), key.size()),
		&value
	);

	if (status.ok())
		return true;
	return false;
}

void mcp::db::db_transaction::count_add(std::string const& _k, uint32_t const& _v)
{
	uint64_t ori_value = count_get(_k);
	ori_value += _v;
	uint64_t big_value = boost::endian::native_to_big(ori_value);

	auto handle = m_db.get_column_family_handle(m_db.m_count);
	rocksdb::Status status = m_txn->Put(
		handle,
		_k,
		rocksdb::Slice((char*)&big_value, sizeof(big_value))
	);
	check_status(status);
	m_read_only = false;
}

void mcp::db::db_transaction::count_reduce(std::string const& _k, uint32_t const& _v)
{
	uint64_t ori_value = count_get(_k);
	assert_x(ori_value >= _v);
	ori_value -= _v;
	uint64_t big_value = boost::endian::native_to_big(ori_value);

	auto handle = m_db.get_column_family_handle(m_db.m_count);
	rocksdb::Status status = m_txn->Put(
		handle,
		_k,
		rocksdb::Slice((char*)&big_value, sizeof(big_value))
	);
	check_status(status);
	m_read_only = false;
}

void mcp::db::db_transaction::count_del(std::string const& _k)
{
	auto handle = m_db.get_column_family_handle(m_db.m_count);
	rocksdb::Status status = m_txn->Delete(
		handle,
		rocksdb::Slice(_k.data(), _k.size())
	);
	check_status(status);
	m_read_only = false;
}

uint64_t mcp::db::db_transaction::count_get(std::string const& _k, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	auto handle = m_db.get_column_family_handle(m_db.m_count);
	std::shared_ptr<rocksdb::ReadOptions> read_ops = mcp::db::database::default_read_options();
	if (snapshot_a)
		read_ops->snapshot = snapshot_a->snapshot();
	std::string value = "";

	rocksdb::Status status = m_txn->Get(
		*read_ops,
		handle,
		rocksdb::Slice(_k.data(), _k.size()),
		&value
	);

	uint64_t i_value = 0;
	if (status.ok())
	{
		uint64_t* orig_value = (uint64_t*)value.data();
		i_value = boost::endian::big_to_native(*orig_value);
	}
	else if (status.IsNotFound())
	{ }
	else
		check_status(status);

	return i_value;
}

void mcp::db::db_transaction::commit()
{
	if (m_commited_or_rollbacked)
		return;

	m_commited_or_rollbacked = true;
	if (!m_read_only)
	{
		rocksdb::Status status = m_txn->Commit();
		check_status(status);
	}
}

void mcp::db::db_transaction::rollback()
{
	if (m_commited_or_rollbacked)
		return;

	m_commited_or_rollbacked = true;
	rocksdb::Status status = m_txn->Rollback();
	check_status(status);
}

mcp::db::forward_iterator mcp::db::db_transaction::begin(int const& index, 
	std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);
	if (info->shared)//prefix begin
	{
		return begin(index, dev::Slice(), snapshot_a, read_ops_a);
	}

	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = mcp::db::database::default_read_options();

	read_ops->fill_cache = false;
	if (snapshot_a != nullptr)
		read_ops->snapshot = snapshot_a->snapshot();

	auto it = m_txn->GetIterator(*read_ops, handle);
	return forward_iterator(it);
}

mcp::db::forward_iterator mcp::db::db_transaction::begin(int const& index, 
	dev::Slice const & _k, 
	std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);

	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = mcp::db::database::default_read_options();

	read_ops->fill_cache = false;
	if (snapshot_a != nullptr)
		read_ops->snapshot = snapshot_a->snapshot();
	
	dev::Slicebytes key;
	if (info->shared)
	{
		key.resize(info->prefix.size() + _k.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key));
		_k.copyTo(dev::SliceRef(&key).cropped(info->prefix.size()));
		read_ops->prefix_same_as_start = true;
	}
	else
	{
		key.resize(_k.size());
		_k.copyTo(dev::SliceRef(&key));
	}

	auto it = m_txn->GetIterator(*read_ops, handle);
	return forward_iterator(it, rocksdb::Slice(key.data(),key.size()), info->prefix);
}

mcp::db::backward_iterator mcp::db::db_transaction::rbegin(int const& index, 
	std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);
	if (info->shared)//prefix begin
	{
		//todo size depended on key size
		dev::Slicebytes key(48,0xFF);
		return rbegin(index, dev::Slice(key.data(), key.size()), snapshot_a, read_ops_a);
	}

	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = mcp::db::database::default_read_options();

	read_ops->fill_cache = false;
	if (snapshot_a != nullptr)
		read_ops->snapshot = snapshot_a->snapshot();

	auto it = m_txn->GetIterator(*read_ops, handle);
	return backward_iterator(it);
}

mcp::db::backward_iterator mcp::db::db_transaction::rbegin(int const& index, 
	dev::Slice const & _k, 
	std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a,
	std::shared_ptr<rocksdb::ReadOptions> read_ops_a)
{
	std::shared_ptr<mcp::db::index_info> info = std::make_shared<mcp::db::index_info>();
	auto handle = m_db.get_column_family_handle(index, info);

	std::shared_ptr<rocksdb::ReadOptions> read_ops(read_ops_a);
	if (nullptr == read_ops)
		read_ops = mcp::db::database::default_read_options();

	read_ops->fill_cache = false;
	if (snapshot_a != nullptr)
		read_ops->snapshot = snapshot_a->snapshot();

	dev::Slicebytes key;
	if (info->shared)
	{
		key.resize(info->prefix.size() + _k.size());
		dev::Slice(&info->prefix).copyTo(dev::SliceRef(&key));
		_k.copyTo(dev::SliceRef(&key).cropped(info->prefix.size()));
		read_ops->prefix_same_as_start = true;
	}
	else
	{
		key.resize(_k.size());
		_k.copyTo(dev::SliceRef(&key));
	}

	auto it = m_txn->GetIterator(*read_ops, handle);
	return backward_iterator(it, rocksdb::Slice(key.data(), key.size()), info->prefix);
}

bool mcp::db::db_transaction::merge(int const& index, std::string const& _k, dev::Slice const& _v)
{
	m_read_only = false;

	auto handle = m_db.get_column_family_handle(index);
	rocksdb::Status status = m_txn->Merge(
		handle,
		_k,
		rocksdb::Slice(_v.data(), _v.size())
	);
	if (status.ok())
		return true;
	check_status(status);
}

mcp::db::db_transaction & mcp::db::db_transaction::operator= (mcp::db::db_transaction && other_a)
{
	m_db = other_a.m_db;
	if (m_txn != nullptr)
	{
		delete m_txn;
	}
	m_txn = other_a.m_txn;
	other_a.m_txn = nullptr;

	m_commited_or_rollbacked = other_a.m_commited_or_rollbacked;
	m_read_only = other_a.m_read_only;
	return *this;
}

