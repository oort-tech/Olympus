#include "block_cache.hpp"

mcp::block_cache::block_cache(mcp::block_store &store_a) :
	m_store(store_a),
	m_blocks(1000),
	m_block_states(1000),
	m_latest_account_states(50000),
	m_transactions(50000),
	m_account_nonces(30000),
	m_transaction_address(50000),
	m_successors(1000),
	m_block_summarys(1000),
	m_block_numbers(1000),
	m_number_blocks(1000),
	m_transaction_receipts(50000)
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

std::shared_ptr<mcp::block> mcp::block_cache::block_get(mcp::db::db_transaction &transaction_a, uint64_t const & index_a)
{
	mcp::block_hash bh;
	if (block_number_get(transaction_a, index_a, bh))/// not exist
		return nullptr;

	std::shared_ptr<mcp::block> block = nullptr;
	std::lock_guard<std::mutex> lock(m_block_mutex);
	if (!m_block_changings.count(bh))
	{
		bool exists = m_blocks.tryGet(bh, block);
		if (!exists)
			block = m_store.block_get(transaction_a, bh);

		m_blocks.insert(bh, block);
	}
	else
		block = m_store.block_get(transaction_a, bh);

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


std::shared_ptr<mcp::account_state> mcp::block_cache::latest_account_state_get(mcp::db::db_transaction &transaction_a, Address const &account_a)
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
	h256 hash;
	bool exists = !m_store.latest_account_state_get(transaction_a, account_a, hash);
	if (exists)
	{
		state = m_store.account_state_get(transaction_a, hash);
		assert_x(state);
		m_latest_account_states.insert(account_a, state);
	}

	return state;
}

void mcp::block_cache::latest_account_state_put(Address const &account_a, std::shared_ptr<mcp::account_state> account_state_a)
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	m_latest_account_states.insert(account_a, account_state_a);
}

void mcp::block_cache::latest_account_state_earse(std::unordered_set<Address> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	for (auto const & account : accounts_a)
		m_latest_account_states.remove(account);
}

void mcp::block_cache::mark_latest_account_state_as_changing(std::unordered_set<Address> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	for (auto const & account : accounts_a)
		m_latest_account_state_changings.insert(account);
}

void mcp::block_cache::clear_latest_account_state_changing()
{
	std::lock_guard<std::mutex> lock(m_latest_account_state_mutex);
	m_latest_account_state_changings.clear();
}


bool mcp::block_cache::transaction_exists(mcp::db::db_transaction & transaction_a, h256 const & hash)
{
	auto t = transaction_get(transaction_a, hash);
	return t != nullptr;
}

std::shared_ptr<mcp::Transaction> mcp::block_cache::transaction_get(mcp::db::db_transaction &transaction_a, h256 const &hash)
{
	std::shared_ptr<mcp::Transaction> t = nullptr;
	std::lock_guard<std::mutex> lock(m_transaction_mutex);
	if (!m_transaction_changings.count(hash))
	{
		bool exists = m_transactions.tryGet(hash, t);
		if (!exists)
		{
			t = m_store.transaction_get(transaction_a, hash);
			if (t)
				m_transactions.insert(hash, t);
		}
	}
	else
		t = m_store.transaction_get(transaction_a, hash);

	return t;
}

void mcp::block_cache::transaction_put(h256 const &hash, std::shared_ptr<mcp::Transaction> const & t)
{
	std::lock_guard<std::mutex> lock(m_transaction_mutex);
	m_transactions.insert(hash, t);
}

void mcp::block_cache::transaction_earse(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_transaction_mutex);
	for (auto const & block_hash : hashs)
		m_transactions.remove(block_hash);
}

void mcp::block_cache::mark_transaction_as_changing(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_transaction_mutex);
	for (auto const & block_hash : hashs)
		m_transaction_changings.insert(block_hash);
}

void mcp::block_cache::clear_transaction_changing()
{
	std::lock_guard<std::mutex> lock(m_transaction_mutex);
	m_transaction_changings.clear();
}


bool mcp::block_cache::approve_exists(mcp::db::db_transaction & transaction_a, h256 const & hash)
{
	auto t = approve_get(transaction_a, hash);
	return t != nullptr;
}

std::shared_ptr<mcp::approve> mcp::block_cache::approve_get(mcp::db::db_transaction &transaction_a, h256 const &hash)
{
	std::shared_ptr<mcp::approve> t = nullptr;
	std::lock_guard<std::mutex> lock(m_approve_mutex);
	if (!m_approve_changings.count(hash))
	{
		bool exists = m_approves.tryGet(hash, t);
		if (!exists)
		{
			t = m_store.approve_get(transaction_a, hash);
			if (t)
				m_approves.insert(hash, t);
		}
	}
	else
		t = m_store.approve_get(transaction_a, hash);

	return t;
}

void mcp::block_cache::approve_put(h256 const &hash, std::shared_ptr<mcp::approve> const & t)
{
	std::lock_guard<std::mutex> lock(m_approve_mutex);
	m_approves.insert(hash, t);
}

void mcp::block_cache::approve_earse(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_approve_mutex);
	for (auto const & block_hash : hashs)
		m_approves.remove(block_hash);
}

void mcp::block_cache::mark_approve_as_changing(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_approve_mutex);
	for (auto const & block_hash : hashs)
		m_approve_changings.insert(block_hash);
}

void mcp::block_cache::clear_approve_changing()
{
	std::lock_guard<std::mutex> lock(m_approve_mutex);
	m_approve_changings.clear();
}



bool mcp::block_cache::account_nonce_get(mcp::db::db_transaction & transaction_a, Address const & account_a, u256 & nonce_a)
{
	std::lock_guard<std::mutex> lock(m_account_nonce_mutex);
	bool exists = m_account_nonces.tryGet(account_a, nonce_a);
	if (!exists)
	{
		exists = m_store.account_nonce_get(transaction_a, account_a, nonce_a);
		if (exists)
			m_account_nonces.insert(account_a, nonce_a);
	}
	return exists;
}

void mcp::block_cache::account_nonce_put(Address const & account_a, u256 const & nonce_a)
{
	std::lock_guard<std::mutex> lock(m_account_nonce_mutex);
	m_account_nonces.insert(account_a, nonce_a);
}

void mcp::block_cache::account_nonce_earse(std::unordered_set<Address> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_account_nonce_mutex);
	for (Address const & accou : accounts_a)
		m_account_nonces.remove(accou);
}

void mcp::block_cache::mark_account_nonce_as_changing(std::unordered_set<Address> const & accounts_a)
{
	std::lock_guard<std::mutex> lock(m_account_nonce_mutex);
	for (Address const & acco : accounts_a)
		m_account_nonce_changings.insert(acco);
}

void mcp::block_cache::clear_account_nonce_changing()
{
	std::lock_guard<std::mutex> lock(m_account_nonce_mutex);
	m_account_nonce_changings.clear();
}


std::shared_ptr<mcp::TransactionAddress> mcp::block_cache::transaction_address_get(mcp::db::db_transaction & transaction_a, h256 const & hash)
{
	std::shared_ptr<mcp::TransactionAddress> td = nullptr;
	std::lock_guard<std::mutex> lock(m_transaction_address_mutex);
	bool exists = m_transaction_address.tryGet(hash, td);
	if (!exists)
	{
		td = m_store.transaction_address_get(transaction_a, hash);
		if (td)
			m_transaction_address.insert(hash, td);
	}
	return td;
}

void mcp::block_cache::transaction_address_put(h256 const & hash, std::shared_ptr<mcp::TransactionAddress> const& td)
{
	std::lock_guard<std::mutex> lock(m_transaction_address_mutex);
	m_transaction_address.insert(hash, td);
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

void mcp::block_cache::successor_earse(std::unordered_set<mcp::block_hash> const & successors_a)
{
	std::lock_guard<std::mutex> lock(m_successor_mutex);
	for (mcp::block_hash const & successor : successors_a)
		m_successors.remove(successor);
}

void mcp::block_cache::mark_successor_as_changing(std::unordered_set<mcp::block_hash> const & successors_a)
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

bool mcp::block_cache::block_number_get(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::block_hash & hash_a)
{
	std::lock_guard<std::mutex> lock(m_block_number_mutex);
	bool exists = m_block_numbers.tryGet(index_a, hash_a);
	if (!exists)
	{
		exists = !m_store.stable_block_get(transaction_a, index_a, hash_a);
		if (exists)
			m_block_numbers.insert(index_a, hash_a);
	}
	return !exists;
}

bool mcp::block_cache::block_number_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a, uint64_t & index_a)
{
	std::lock_guard<std::mutex> lock(m_block_number_mutex);
	bool exists = m_number_blocks.tryGet(hash_a,index_a);
	if (!exists)
	{
		exists = !m_store.stable_block_get(transaction_a, hash_a, index_a);
		if (exists)
			m_number_blocks.insert(hash_a,index_a);
	}
	return !exists;
}

void mcp::block_cache::block_number_put(uint64_t const & index_a, mcp::block_hash const & hash_a)
{
	std::lock_guard<std::mutex> lock(m_block_number_mutex);
	m_block_numbers.insert(index_a, hash_a);
	m_number_blocks.insert(hash_a, index_a);
}

bool mcp::block_cache::transaction_receipt_exists(mcp::db::db_transaction & transaction_a, h256 const & hash)
{
	auto t = transaction_receipt_get(transaction_a, hash);
	return t != nullptr;
}

std::shared_ptr<dev::eth::TransactionReceipt> mcp::block_cache::transaction_receipt_get(mcp::db::db_transaction &transaction_a, h256 const &hash)
{
	std::shared_ptr<dev::eth::TransactionReceipt> t = nullptr;
	std::lock_guard<std::mutex> lock(m_transaction_receipt_mutex);
	if (!m_transaction_receipt_changings.count(hash))
	{
		bool exists = m_transaction_receipts.tryGet(hash, t);
		if (!exists)
		{
			t = m_store.transaction_receipt_get(transaction_a, hash);
			if (t)
				m_transaction_receipts.insert(hash, t);
		}
	}
	else
		t = m_store.transaction_receipt_get(transaction_a, hash);

	return t;
}

void mcp::block_cache::transaction_receipt_put(h256 const &hash, std::shared_ptr<dev::eth::TransactionReceipt> const & t)
{
	std::lock_guard<std::mutex> lock(m_transaction_receipt_mutex);
	m_transaction_receipts.insert(hash, t);
}

void mcp::block_cache::transaction_receipt_earse(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_transaction_receipt_mutex);
	for (auto const & block_hash : hashs)
		m_transaction_receipts.remove(block_hash);
}

void mcp::block_cache::mark_transaction_receipt_as_changing(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_transaction_receipt_mutex);
	for (auto const & block_hash : hashs)
		m_transaction_receipt_changings.insert(block_hash);
}

void mcp::block_cache::clear_transaction_receipt_changing()
{
	std::lock_guard<std::mutex> lock(m_transaction_receipt_mutex);
	m_transaction_receipt_changings.clear();
}

bool mcp::block_cache::approve_receipt_exists(mcp::db::db_transaction & transaction_a, h256 const & hash)
{
	auto t = approve_receipt_get(transaction_a, hash);
	return t != nullptr;
}

std::shared_ptr<dev::ApproveReceipt> mcp::block_cache::approve_receipt_get(mcp::db::db_transaction &transaction_a, h256 const &hash)
{
	std::shared_ptr<dev::ApproveReceipt> t = nullptr;
	std::lock_guard<std::mutex> lock(m_approve_receipt_mutex);
	if (!m_approve_receipt_changings.count(hash))
	{
		bool exists = m_approve_receipts.tryGet(hash, t);
		if (!exists)
		{
			t = m_store.approve_receipt_get(transaction_a, hash);
			if (t)
				m_approve_receipts.insert(hash, t);
		}
	}
	else
		t = m_store.approve_receipt_get(transaction_a, hash);

	return t;
}

void mcp::block_cache::approve_receipt_put(h256 const &hash, std::shared_ptr<dev::ApproveReceipt> const & t)
{
	std::lock_guard<std::mutex> lock(m_approve_receipt_mutex);
	m_approve_receipts.insert(hash, t);
}

void mcp::block_cache::approve_receipt_earse(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_approve_receipt_mutex);
	for (auto const & block_hash : hashs)
		m_approve_receipts.remove(block_hash);
}

void mcp::block_cache::mark_approve_receipt_as_changing(std::unordered_set<h256> const & hashs)
{
	std::lock_guard<std::mutex> lock(m_approve_receipt_mutex);
	for (auto const & block_hash : hashs)
		m_approve_receipt_changings.insert(block_hash);
}

void mcp::block_cache::clear_approve_receipt_changing()
{
	std::lock_guard<std::mutex> lock(m_approve_receipt_mutex);
	m_approve_receipt_changings.clear();
}

std::string mcp::block_cache::report_cache_size()
{
	std::stringstream s;
	s << "m_blocks:" << m_blocks.size()
		<< " , m_block_states:" << m_block_states.size()
		<< " , m_latest_account_states:" << m_latest_account_states.size()
		<< " , m_successors:" << m_successors.size()
		<< " , m_block_summarys:" << m_block_summarys.size();

	return s.str();
}