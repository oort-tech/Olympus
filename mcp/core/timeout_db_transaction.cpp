#include "timeout_db_transaction.hpp"

mcp::timeout_db_transaction::timeout_db_transaction(mcp::block_store & store_a, uint32_t const & timeout_a, 
	std::shared_ptr<rocksdb::WriteOptions> write_options_a,
	std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a,
	std::function<void()> const & before_commit_event_a,
	std::function<void()> const & after_commit_event_a):
	m_store(store_a),
	m_write_options(write_options_a),
	m_txn_ops(txn_ops_a),
	m_transaction(m_store.create_transaction(m_write_options, m_txn_ops)),
	m_timeout(timeout_a),
	m_last_create_time(std::chrono::steady_clock::now()),
	m_before_commit_event(before_commit_event_a),
	m_after_commit_event(after_commit_event_a)
{
}

mcp::db::db_transaction & mcp::timeout_db_transaction::get_transaction()
{
	return m_transaction;
}

void mcp::timeout_db_transaction::commit_if_timeout()
{
	if (std::chrono::steady_clock::now() - m_last_create_time >= m_timeout)
	{
		commit_and_continue();
	}
}

void mcp::timeout_db_transaction::commit_and_continue()
{
	commit();

	m_transaction = m_store.create_transaction(m_write_options, m_txn_ops);
	m_last_create_time = std::chrono::steady_clock::now();
}

void mcp::timeout_db_transaction::commit()
{
	if (m_before_commit_event)
		m_before_commit_event();
	m_transaction.commit();
	if (m_after_commit_event)
		m_after_commit_event();
}

void mcp::timeout_db_transaction::rollback()
{
	m_transaction.rollback();
}

