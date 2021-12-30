#include "write_batch.hpp"

mcp::db::write_batch::write_batch(mcp::db::database& m_db_a) :
	m_db(m_db_a)
{
}

mcp::db::write_batch::~write_batch()
{
	commit();
}

void mcp::db::write_batch::put(int const& index, dev::Slice const& _k, dev::Slice const& _v)
{
	auto handle = m_db.get_column_family_handle(index);
	rocksdb::Status status = m_write_batch.Put(
		handle,
		rocksdb::Slice(_k.data(), _k.size()),
		rocksdb::Slice(_v.data(), _v.size())
	);

	check_status(status);
}

void mcp::db::write_batch::del(int const& index, dev::Slice const& _k)
{
	auto handle = m_db.get_column_family_handle(index);
	rocksdb::Slice const key(_k.data(), _k.size());

	rocksdb::Status status = m_write_batch.Delete(
		handle,
		key
	);

	check_status(status);
}

void mcp::db::write_batch::commit()
{
	auto const status = m_db.get_db()->Write(*m_db.default_write_options(), &m_write_batch);
	check_status(status);
}