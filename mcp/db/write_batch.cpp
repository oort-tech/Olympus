//#include "write_batch.hpp"
//
//void mcp::db::write_batch::insert(int const& index, dev::Slice const& _k, dev::Slice const& _v)
//{
//	auto handle = m_db.get_column_family_handle(index);
//	rocksdb::Status status = m_write_batch.Put(
//		handle,
//		rocksdb::Slice(_k.data(), _k.size()),
//		rocksdb::Slice(_v.data(), _v.size())
//	);
//
//	check_status(status);
//}
//
//void mcp::db::write_batch::kill(int const& index, dev::Slice const& _k)
//{
//	auto handle = m_db.get_column_family_handle(index);
//	rocksdb::Slice const key(_k.data(), _k.size());
//
//	rocksdb::Status status = m_write_batch.Delete(
//		handle,
//		key
//	);
//
//	check_status(status);
//}
