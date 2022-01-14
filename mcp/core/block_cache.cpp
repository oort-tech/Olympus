#include "block_cache.hpp"

mcp::block_cache::block_cache(mcp::block_store &store_a) :
	m_store(store_a),
	m_blocks(50000),
	m_block_states(50000),
	m_latest_account_states(50000),
	m_unlink_blocks(50000),
	m_accounts(50000),
	m_successors(50000),
	m_block_summarys(50000)
{
}

bool mcp::block_cache::block_exists(mcp::db::db_transaction &transaction_a, mcp::block_hash const &block_hash_a)
{
	std::shared_ptr<mcp::block> block = block_get(transaction_a, block_hash_a);
	bool exists = block != nullptr;
	return exists;
}

std::shared_ptr<mcp::block> mcp::block_cache::block_get(mcp::db::db_transaction &transaction_a, mcp::block_hash const &block_hash_a)
{
	std::shared_ptr<mcp::block> block;
	std::lock_guard<std::mutex> lock(m_block_mutex);
	if (!m_block_changings.count(block_hash_a))
	{
		bool exists = m_blocks.tryGet(block_hash_a, block);
		if (!exists)
			block = m_store.block_get(transaction_a, block_hash_a);

		m_blocks.insert(block_hash_a, block);
	}
	else
		block = m_store.block_get(transaction_a, block_hash_a);

	return block;
}

void mcp::block_cache::block_put(mcp::block_hash const &block_hash_a, std::shared_ptr<mcp::block> block_a)
{
	std::lock_guard<std::mutex> lock(m_block_mutex);
	m_blocks.insert(block_hash_a, block_a);
}

void mcp::block_cache::block_earse(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_block_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_blocks.remove(block_hash);
}

void mcp::block_cache::mark_block_as_changing(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_block_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_block_changings.insert(block_hash);
}

void mcp::block_cache::clear_block_changing()
{
	std::lock_guard<std::mutex> lock(m_block_mutex);
	m_block_changings.clear();
}


std::shared_ptr<mcp::block_state> mcp::block_cache::block_state_get(mcp::db::db_transaction &transaction_a, mcp::block_hash const &block_hash_a)
{
	std::shared_ptr<mcp::block_state> state;
	std::lock_guard<std::mutex> lock(m_block_state_mutex);
	if (!m_block_state_changings.count(block_hash_a))
	{
		bool exists = m_block_states.tryGet(block_hash_a, state);
		if (!exists)
		{
			state = m_store.block_state_get(transaction_a, block_hash_a);
			if (state)
				m_block_states.insert(block_hash_a, state);
		}
	}
	else
		state = m_store.block_state_get(transaction_a, block_hash_a);

	return state;
}

void mcp::block_cache::block_state_put(mcp::block_hash const &block_hash_a, std::shared_ptr<mcp::block_state> block_state_a)
{
	std::lock_guard<std::mutex> lock(m_block_state_mutex);
	m_block_states.insert(block_hash_a, block_state_a);
}

void mcp::block_cache::block_state_earse(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_block_state_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_block_states.remove(block_hash);
}

void mcp::block_cache::mark_block_state_as_changing(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_block_state_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_block_state_changings.insert(block_hash);
}

void mcp::block_cache::clear_block_state_changing()
{
	std::lock_guard<std::mutex> lock(m_block_state_mutex);
	m_block_state_changings.clear();
}


std::shared_ptr<mcp::account_state> mcp::block_cache::latest_account_state_get(mcp::db::db_transaction &transaction_a, mcp::account const &account_a)
{
	std::shared_ptr<mcp::account_state> state;
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	if (!m_latest_account_state_changings.count(account_a))
	{
		bool exists = m_latest_account_states.tryGet(account_a, state);
		if (exists)
			return state;
	}

	//get from db
	mcp::account_state_hash hash;
	bool exists = !m_store.latest_account_state_get(transaction_a, account_a, hash);
	if (exists)
	{
		state = m_store.account_state_get(transaction_a, hash);
		assert_x(state);
		m_latest_account_states.insert(account_a, state);
	}

	return state;
}

void mcp::block_cache::latest_account_state_put(mcp::account const &account_a, std::shared_ptr<mcp::account_state> account_state_a)
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	m_latest_account_states.insert(account_a, account_state_a);
}

void mcp::block_cache::latest_account_state_earse(std::unordered_set<mcp::account> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	for (mcp::account const & account : accounts_a)
		m_latest_account_states.remove(account);
}

void mcp::block_cache::mark_latest_account_state_as_changing(std::unordered_set<mcp::account> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	for (mcp::account const & account : accounts_a)
		m_latest_account_state_changings.insert(account);
}

void mcp::block_cache::clear_latest_account_state_changing()
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	m_latest_account_state_changings.clear();
}

bool mcp::block_cache::unlink_block_exists(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a)
{
	std::shared_ptr<mcp::unlink_block> unlink_block = unlink_block_get(transaction_a, block_hash_a);
	bool exists = unlink_block != nullptr;
	return exists;
}

std::shared_ptr<mcp::unlink_block> mcp::block_cache::unlink_block_get(mcp::db::db_transaction &transaction_a, mcp::block_hash const &block_hash_a)
{
	std::shared_ptr<mcp::unlink_block> unlink_block;
	std::lock_guard<std::mutex> lock(m_unlink_block_mutex);
	if (!m_unlink_block_changings.count(block_hash_a))
	{
		bool exists = m_unlink_blocks.tryGet(block_hash_a, unlink_block);
		if (!exists)
		{
			unlink_block = m_store.unlink_block_get(transaction_a, block_hash_a);
			if (unlink_block)
				m_unlink_blocks.insert(block_hash_a, unlink_block);
		}
	}
	else
		unlink_block = m_store.unlink_block_get(transaction_a, block_hash_a);

	return unlink_block;
}

void mcp::block_cache::unlink_block_put(mcp::block_hash const &block_hash_a, std::shared_ptr<mcp::unlink_block> unlink_block_a)
{
	std::lock_guard<std::mutex> lock(m_unlink_block_mutex);
	m_unlink_blocks.insert(block_hash_a, unlink_block_a);
}

void mcp::block_cache::unlink_block_earse(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_unlink_block_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_unlink_blocks.remove(block_hash);
}

void mcp::block_cache::mark_unlink_block_as_changing(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_unlink_block_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_unlink_block_changings.insert(block_hash);
}

void mcp::block_cache::clear_unlink_block_changing()
{
	std::lock_guard<std::mutex> lock(m_unlink_block_mutex);
	m_unlink_block_changings.clear();
}


std::shared_ptr<mcp::account_info> mcp::block_cache::account_get(mcp::db::db_transaction &transaction_a, mcp::account const &account_a)
{
	std::shared_ptr<mcp::account_info> account_info;
	std::lock_guard<std::mutex> lock(m_account_mutex);
	if (!m_account_changings.count(account_a))
	{
		bool exists = m_accounts.tryGet(account_a, account_info);
		if (!exists)
		{
			account_info = m_store.account_get(transaction_a, account_a);
			if (account_info)
				m_accounts.insert(account_a, account_info);
		}
	}
	else
		account_info = m_store.account_get(transaction_a, account_a);

	return account_info;
}

void mcp::block_cache::account_put(mcp::account const &account_a, std::shared_ptr<mcp::account_info> account_info_a)
{
	std::lock_guard<std::mutex> lock(m_account_mutex);
	m_accounts.insert(account_a, account_info_a);
}

void mcp::block_cache::account_earse(std::unordered_set<mcp::account> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_account_mutex);
	for (mcp::block_hash const & account : accounts_a)
		m_accounts.remove(account);
}

void mcp::block_cache::mark_account_as_changing(std::unordered_set<mcp::account> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_account_mutex);
	for (mcp::block_hash const & account : accounts_a)
		m_account_changings.insert(account);
}

void mcp::block_cache::clear_account_changing()
{
	std::lock_guard<std::mutex> lock(m_account_mutex);
	m_account_changings.clear();
}


bool mcp::block_cache::successor_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash & successor_a)
{
	bool exists;
	std::lock_guard<std::mutex> lock(m_successor_mutex);
	if (!m_successor_changings.count(root_a))
	{
		exists = m_successors.tryGet(root_a, successor_a);
		if (!exists)
		{
			exists = !m_store.successor_get(transaction_a, root_a, successor_a);
			if (exists)
				m_successors.insert(root_a, successor_a);
		}
	}
	else
		exists = !m_store.successor_get(transaction_a, root_a, successor_a);
	return !exists;
}

void mcp::block_cache::successor_put(mcp::block_hash const & root_a, mcp::block_hash const & successor_a)
{
	std::lock_guard<std::mutex> lock(m_successor_mutex);
	m_successors.insert(root_a, successor_a);
}

void mcp::block_cache::successor_earse(std::unordered_set<mcp::account> const & successors_a)
{
	std::lock_guard<std::mutex> lock(m_successor_mutex);
	for (mcp::account const & successor : successors_a)
		m_successors.remove(successor);
}

void mcp::block_cache::successor_earse(std::unordered_set<mcp::block_hash> const & successors_a)
{
	std::lock_guard<std::mutex> lock(m_successor_mutex);
	for (mcp::block_hash const & successor : successors_a)
		m_successors.remove(successor);
}

void mcp::block_cache::mark_successor_as_changing(std::unordered_set<mcp::account> const & successors_a)
{
	std::lock_guard<std::mutex> lock(m_successor_mutex);
	for (mcp::block_hash const & successor : successors_a)
		m_successor_changings.insert(successor);
}

void mcp::block_cache::clear_successor_changing()
{
	std::lock_guard<std::mutex> lock(m_successor_mutex);
	m_successor_changings.clear();
}


bool mcp::block_cache::block_summary_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash & summary_a)
{
	bool exists;
	std::lock_guard<std::mutex> lock(m_block_summary_mutex);
	if (!m_block_summary_changings.count(block_hash_a))
	{
		exists = m_block_summarys.tryGet(block_hash_a, summary_a);
		if (!exists)
		{
			exists = !m_store.block_summary_get(transaction_a, block_hash_a, summary_a);
			if (exists)
				m_block_summarys.insert(block_hash_a, summary_a);
		}
	}
	else
		exists = !m_store.block_summary_get(transaction_a, block_hash_a, summary_a);
	return !exists;
}

void mcp::block_cache::block_summary_put(mcp::block_hash const & block_hash_a, mcp::block_hash const & summary_a)
{
	std::lock_guard<std::mutex> lock(m_block_summary_mutex);
	m_block_summarys.insert(block_hash_a, summary_a);
}

void mcp::block_cache::block_summary_earse(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_block_summary_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_block_summarys.remove(block_hash);
}

void mcp::block_cache::mark_block_summary_as_changing(std::unordered_set<mcp::block_hash> const & block_hashs_a)
{
	std::lock_guard<std::mutex> lock(m_block_summary_mutex);
	for (mcp::block_hash const & block_hash : block_hashs_a)
		m_block_summary_changings.insert(block_hash);
}

void mcp::block_cache::clear_block_summary_changing()
{
	std::lock_guard<std::mutex> lock(m_block_summary_mutex);
	m_block_summary_changings.clear();
}


std::string mcp::block_cache::report_cache_size()
{
	std::stringstream s;
	s << "m_blocks:" << m_blocks.size()
		<< " , m_block_states:" << m_block_states.size()
		<< " , m_latest_account_states:" << m_latest_account_states.size()
		<< " , m_unlink_blocks:" << m_unlink_blocks.size()
		<< " , m_accounts:" << m_accounts.size()
		<< " , m_successors:" << m_successors.size()
		<< " , m_block_summarys:" << m_block_summarys.size();

	return s.str();
}