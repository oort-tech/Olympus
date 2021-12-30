#pragma once

#include <mcp/core/block_store.hpp>
#include <mcp/db/db_transaction.hpp>

namespace mcp
{
	class timeout_db_transaction
	{
	public:
		timeout_db_transaction(mcp::block_store & store_a, uint32_t const & timeout_a, 
			std::shared_ptr<rocksdb::WriteOptions> write_options_a = nullptr,
			std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a = nullptr,
			std::function<void()> const & brefore_commit_event_a = nullptr,
			std::function<void()> const & after_commit_event_a= nullptr);
		mcp::db::db_transaction & get_transaction();
		void commit_if_timeout();
		void commit_and_continue();
		void commit();
		void rollback();

	private:
		mcp::block_store & m_store;
		std::shared_ptr<rocksdb::WriteOptions> m_write_options;
		std::shared_ptr<rocksdb::TransactionOptions> m_txn_ops;
		mcp::db::db_transaction m_transaction;
		std::chrono::milliseconds m_timeout;
		std::chrono::steady_clock::time_point m_last_create_time;
		std::function<void()> m_before_commit_event;
		std::function<void()> m_after_commit_event;
	};

} // namespace mcp
