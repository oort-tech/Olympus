#include "process_block_cache.hpp"
#include "transaction_queue.hpp"
#include "approve_queue.hpp"

mcp::process_block_cache::process_block_cache(std::shared_ptr<mcp::block_cache> cache_a, mcp::block_store &store_a, std::shared_ptr<mcp::TransactionQueue> tq, std::shared_ptr<ApproveQueue> aq) :
	m_cache(cache_a),
	m_store(store_a),
	m_tq(tq),
	m_aq(aq)
{
}

bool mcp::process_block_cache::block_exists(mcp::db::db_transaction & transaction_a, mcp::block_hash const &block_hash_a)
{
	std::shared_ptr<mcp::block> block = block_get(transaction_a, block_hash_a);
	bool exists = block != nullptr;
	return exists;
}

std::shared_ptr<mcp::block> mcp::process_block_cache::block_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const &block_hash_a)
{
	std::shared_ptr<mcp::block> block;
	auto it(m_block_puts.get<1>().find(block_hash_a));
	if (it != m_block_puts.get<1>().end())
	{
		block = it->value;
	}
	else
	{
		if (m_block_puts_flushed.count(block_hash_a))
			block = m_store.block_get(transaction_a, block_hash_a);
		else
			block = m_cache->block_get(transaction_a, block_hash_a);
	}
	return block;
}

void mcp::process_block_cache::block_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::shared_ptr<mcp::block> block_a)
{
	m_store.block_put(transaction_a, block_hash_a, *block_a);
	auto r = m_block_puts.push_back(put_item<mcp::block_hash, std::shared_ptr<mcp::block>>(block_hash_a, block_a));
	assert_x(r.second);

	if (m_block_puts.size() >= m_max_block_puts_size)
	{
		while (m_block_puts.size() > m_max_block_puts_size / 2)
		{
			put_item<mcp::block_hash, std::shared_ptr<block>> const & item(m_block_puts.front());
			m_block_puts_flushed.insert(std::move(item.key));
			m_block_puts.pop_front();
		}
	}
}


std::shared_ptr<mcp::block_state> mcp::process_block_cache::block_state_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a)
{
	std::shared_ptr<mcp::block_state> block_state;
	auto it(m_block_state_puts.get<1>().find(block_hash_a));
	if (it != m_block_state_puts.get<1>().end())
	{
		block_state = it->value;
	}
	else
	{
		if (m_block_state_puts_flushed.count(block_hash_a))
			block_state = m_store.block_state_get(transaction_a, block_hash_a);
		else
			block_state = m_cache->block_state_get(transaction_a, block_hash_a);
	}
	return block_state;
}

void mcp::process_block_cache::block_state_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::shared_ptr<mcp::block_state> block_state_a)
{
	m_store.block_state_put(transaction_a, block_hash_a, *block_state_a);
	auto it(m_block_state_puts.get<1>().find(block_hash_a));
	if (it != m_block_state_puts.get<1>().end())
	{
		m_block_state_puts.get<1>().modify(it, [block_state_a](put_item<mcp::block_hash, std::shared_ptr<mcp::block_state>> & item_a)
		{
			item_a.value = block_state_a;
		});
	}
	else
	{
		auto r = m_block_state_puts.push_back(put_item<mcp::block_hash, std::shared_ptr<mcp::block_state>>(block_hash_a, block_state_a));
		assert_x(r.second);
		if (m_block_state_puts.size() >= m_max_block_state_puts_size)
		{
			while (m_block_state_puts.size() >= m_max_block_state_puts_size / 2)
			{
				put_item<mcp::block_hash, std::shared_ptr<block_state>> const & item(m_block_state_puts.front());
				m_block_state_puts_flushed.insert(std::move(item.key));
				m_block_state_puts.pop_front();
			}
		}
	}
}


std::shared_ptr<mcp::account_state> mcp::process_block_cache::latest_account_state_get(mcp::db::db_transaction & transaction_a, Address const & account_a)
{
	std::shared_ptr<mcp::account_state> state;
	auto it(m_latest_account_state_puts.get<1>().find(account_a));
	if (it != m_latest_account_state_puts.get<1>().end())
	{
		state = it->value;
	}
	else
	{
		if (m_latest_account_state_puts_flushed.count(account_a))
		{
			h256 hash;
			bool exists = !m_store.latest_account_state_get(transaction_a, account_a, hash);
			if (exists)
			{
				state = m_store.account_state_get(transaction_a, hash);
				assert_x(state);
			}
		}
		else
			state = m_cache->latest_account_state_get(transaction_a, account_a);
	}
	return state;
}

void mcp::process_block_cache::latest_account_state_put(mcp::db::db_transaction & transaction_a, Address const & account_a, std::shared_ptr<mcp::account_state> account_state_a)
{
	h256 acc_hash(account_state_a->hash());
	m_store.account_state_put(transaction_a, acc_hash, *account_state_a);
	m_store.latest_account_state_put(transaction_a, account_a, acc_hash);
	auto it(m_latest_account_state_puts.get<1>().find(account_a));
	if (it != m_latest_account_state_puts.get<1>().end())
	{
		m_latest_account_state_puts.get<1>().modify(it, [account_state_a](put_item<Address, std::shared_ptr<mcp::account_state>> & item_a)
		{
			item_a.value = account_state_a;
		});
	}
	else
	{
		auto r = m_latest_account_state_puts.push_back(put_item<Address, std::shared_ptr<mcp::account_state>>(account_a, account_state_a));
		assert_x(r.second);
		if (m_latest_account_state_puts.size() >= m_max_latest_account_state_puts_size)
		{
			while (m_latest_account_state_puts.size() >= m_max_latest_account_state_puts_size / 2)
			{
				put_item<Address, std::shared_ptr<account_state>> const & item(m_latest_account_state_puts.front());
				m_latest_account_state_puts_flushed.insert(std::move(item.key));
				m_latest_account_state_puts.pop_front();
			}
		}
	}
}


bool mcp::process_block_cache::transaction_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	auto t = transaction_get(transaction_a, _hash);
	return t != nullptr;
}

std::shared_ptr<mcp::Transaction> mcp::process_block_cache::transaction_get(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	std::shared_ptr<mcp::Transaction> t = nullptr;

	auto it(m_transaction_puts.get<1>().find(_hash));
	if (it != m_transaction_puts.get<1>().end())
	{
		t = it->value;
	}
	else
	{
		if (m_transaction_puts_flushed.count(_hash))
			t = m_store.transaction_get(transaction_a, _hash);
		else
			t = m_cache->transaction_get(transaction_a, _hash);
	}
	return t;
}

void mcp::process_block_cache::transaction_put(mcp::db::db_transaction & transaction_a, std::shared_ptr<Transaction> _t)
{
	auto h = _t->sha3();
	m_store.transaction_put(transaction_a, h, *_t);
	auto r = m_transaction_puts.push_back(put_item<h256, std::shared_ptr<mcp::Transaction>>(h, _t));
	assert_x(r.second);

	if (m_transaction_puts.size() >= m_max_transaction_puts_size)
	{
		while (m_transaction_puts.size() >= m_max_transaction_puts_size / 2)
		{
			put_item<h256, std::shared_ptr<mcp::Transaction>> const & item(m_transaction_puts.front());
			m_transaction_puts_flushed.insert(std::move(item.key));
			m_transaction_puts.pop_front();
		}
	}
}

void mcp::process_block_cache::transaction_del_from_queue(h256 const& _hash)
{
	m_transaction_dels.push_back(_hash);
}

void mcp::process_block_cache::approve_del_from_queue(h256 const& _hash)
{
	m_approve_dels.push_back(_hash);
}


bool mcp::process_block_cache::approve_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	auto t = approve_get(transaction_a, _hash);
	return t != nullptr;
}

std::shared_ptr<mcp::approve> mcp::process_block_cache::approve_get(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	return m_cache->approve_get(transaction_a, _hash);
}

void mcp::process_block_cache::approve_put(mcp::db::db_transaction & transaction_a, std::shared_ptr<approve> _t)
{
	auto h = _t->sha3();
	m_store.approve_put(transaction_a, h, *_t);
	m_cache->approve_put(h, _t);
}


bool mcp::process_block_cache::account_nonce_get(mcp::db::db_transaction & transaction_a, Address const & account_a, u256 & nonce_a)
{
	auto it(m_account_nonce_puts.get<1>().find(account_a));
	bool exists(it != m_account_nonce_puts.get<1>().end());
	if (exists)
	{
		nonce_a = it->value;
	}
	else
	{
		if (m_account_nonce_puts_flushed.count(account_a))
			exists = m_store.account_nonce_get(transaction_a, account_a, nonce_a);
		else
			exists = m_cache->account_nonce_get(transaction_a, account_a, nonce_a);
	}
	return exists;
}

void mcp::process_block_cache::account_nonce_put(mcp::db::db_transaction & transaction_a, Address const & account_a, u256 const & nonce_a)
{
	m_store.account_nonce_put(transaction_a, account_a, nonce_a);
	auto it(m_account_nonce_puts.get<1>().find(account_a));
	if (it != m_account_nonce_puts.get<1>().end())
	{
		m_account_nonce_puts.get<1>().modify(it, [nonce_a](put_item<Address, u256> & item_a)
		{
			item_a.value = nonce_a;
		});
	}
	else
	{
		auto r = m_account_nonce_puts.push_back(put_item<Address, u256>(account_a, nonce_a));
		assert_x(r.second);
		if (m_account_nonce_puts.size() >= m_max_account_nonce_puts_size)
		{
			while (m_account_nonce_puts.size() >= m_max_account_nonce_puts_size / 2)
			{
				put_item<Address, u256> const & item(m_account_nonce_puts.front());
				m_account_nonce_puts_flushed.insert(std::move(item.key));
				m_account_nonce_puts.pop_front();
			}
		}
	}
}


void mcp::process_block_cache::transaction_address_put(mcp::db::db_transaction & transaction_a, h256 const & hash, std::shared_ptr<mcp::TransactionAddress> const& td)
{
	m_store.transaction_address_put(transaction_a, hash, *td);
	m_cache->transaction_address_put(hash, td);
}


bool mcp::process_block_cache::successor_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash & successor_a)
{
	bool exists;

	if (m_successor_dels.count(root_a))
		exists = false;
	else
	{
		auto it(m_successor_puts.get<1>().find(root_a));
		exists = (it != m_successor_puts.get<1>().end());
		if (exists)
		{
			successor_a = it->value;
		}
		else
		{
			if (m_successor_puts_flushed.count(root_a))
				exists = !m_store.successor_get(transaction_a, root_a, successor_a);
			else
				exists = !m_cache->successor_get(transaction_a, root_a, successor_a);
		}
	}
	
	return !exists;
}

void mcp::process_block_cache::successor_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash const & successor_a)
{
	m_store.successor_put(transaction_a, root_a, successor_a);
	m_successor_dels.erase(root_a);
	auto it(m_successor_puts.get<1>().find(root_a));
	if (it != m_successor_puts.get<1>().end())
	{
		m_successor_puts.get<1>().modify(it, [successor_a](put_item<mcp::block_hash, mcp::block_hash> & item_a)
		{
			item_a.value = successor_a;
		});
	}
	else
	{
		auto r = m_successor_puts.push_back(put_item<mcp::block_hash, mcp::block_hash>(root_a, successor_a));
		assert_x(r.second);
		if (m_successor_puts.size() >= m_max_successor_puts_size)
		{
			while (m_successor_puts.size() >= m_max_successor_puts_size / 2)
			{
				put_item<mcp::block_hash, mcp::block_hash> const & item(m_successor_puts.front());
				m_successor_puts_flushed.insert(std::move(item.key));
				m_successor_puts.pop_front();
			}
		}
	}
}

void mcp::process_block_cache::successor_del(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a)
{
	m_store.successor_del(transaction_a, root_a);
	auto r(m_successor_dels.insert(root_a));
	assert_x(r.second);
	m_successor_puts.get<1>().erase(root_a);
	m_successor_puts_flushed.erase(root_a);
}


bool mcp::process_block_cache::block_summary_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash & summary_a)
{
	auto it(m_block_summary_puts.get<1>().find(block_hash_a));
	bool exists(it != m_block_summary_puts.get<1>().end());
	if (exists)
	{
		summary_a = it->value;
	}
	else
	{
		if (m_block_summary_puts_flushed.count(block_hash_a))
			exists = !m_store.block_summary_get(transaction_a, block_hash_a, summary_a);
		else
			exists = !m_cache->block_summary_get(transaction_a, block_hash_a, summary_a);
	}
	return !exists;
}

void mcp::process_block_cache::block_summary_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::block_hash const & summary_a)
{
	m_store.block_summary_put(transaction_a, block_hash_a, summary_a);
	auto r = m_block_summary_puts.push_back(put_item<mcp::block_hash, mcp::block_hash>(block_hash_a, summary_a));
	assert_x(r.second);
	if (m_block_summary_puts.size() >= m_max_block_summary_puts_size)
	{
		while (m_block_summary_puts.size() >= m_max_block_summary_puts_size / 2)
		{
			put_item<mcp::block_hash, mcp::block_hash> const & item(m_block_summary_puts.front());
			m_block_summary_puts_flushed.insert(std::move(item.key));
			m_block_summary_puts.pop_front();
		}
	}
}

void mcp::process_block_cache::block_number_put(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::block_hash const & hash_a)
{
	m_store.stable_block_put(transaction_a, index_a, hash_a);
	m_cache->block_number_put(index_a, hash_a);
}

bool mcp::process_block_cache::transaction_receipt_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	auto t = transaction_receipt_get(transaction_a, _hash);
	return t != nullptr;
}

std::shared_ptr<dev::eth::TransactionReceipt> mcp::process_block_cache::transaction_receipt_get(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	std::shared_ptr<dev::eth::TransactionReceipt> t = nullptr;

	auto it(m_transaction_receipt_puts.get<1>().find(_hash));
	if (it != m_transaction_receipt_puts.get<1>().end())
	{
		t = it->value;
	}
	else
	{
		if (m_transaction_receipt_puts_flushed.count(_hash))
			t = m_store.transaction_receipt_get(transaction_a, _hash);
		else
			t = m_cache->transaction_receipt_get(transaction_a, _hash);
	}
	return t;
}

void mcp::process_block_cache::transaction_receipt_put(mcp::db::db_transaction & transaction_a, h256 const& _hash, std::shared_ptr<dev::eth::TransactionReceipt> _t)
{
	m_store.transaction_receipt_put(transaction_a, _hash, *_t);
	auto r = m_transaction_receipt_puts.push_back(put_item<h256, std::shared_ptr<dev::eth::TransactionReceipt>>(_hash, _t));
	assert_x(r.second);

	if (m_transaction_receipt_puts.size() >= m_max_transaction_receipt_puts_size)
	{
		while (m_transaction_receipt_puts.size() >= m_max_transaction_receipt_puts_size / 2)
		{
			put_item<h256, std::shared_ptr<dev::eth::TransactionReceipt>> const & item(m_transaction_receipt_puts.front());
			m_transaction_receipt_puts_flushed.insert(std::move(item.key));
			m_transaction_receipt_puts.pop_front();
		}
	}
}

bool mcp::process_block_cache::approve_receipt_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	auto t = approve_receipt_get(transaction_a, _hash);
	return t != nullptr;
}

std::shared_ptr<dev::ApproveReceipt> mcp::process_block_cache::approve_receipt_get(mcp::db::db_transaction & transaction_a, h256 const& _hash)
{
	return m_cache->approve_receipt_get(transaction_a, _hash);
}

void mcp::process_block_cache::approve_receipt_put(mcp::db::db_transaction & transaction_a, h256 const& _hash, std::shared_ptr<dev::ApproveReceipt> _t)
{
	m_store.approve_receipt_put(transaction_a, _hash, *_t);
	m_cache->approve_receipt_put(_hash, _t);
}


void mcp::process_block_cache::mark_as_changing()
{
	//mark as changing
	//block
	std::unordered_set<mcp::block_hash> block_changings(m_block_puts_flushed);
	for (put_item<mcp::block_hash, std::shared_ptr<block>> const & item : m_block_puts)
		block_changings.insert(item.key);
	m_cache->mark_block_as_changing(block_changings);

	//block state
	std::unordered_set<mcp::block_hash> block_state_changings(m_block_state_puts_flushed);
	for (put_item<mcp::block_hash, std::shared_ptr<block_state>> const & item : m_block_state_puts)
		block_state_changings.insert(item.key);
	m_cache->mark_block_state_as_changing(block_state_changings);

	//latest account state
	std::unordered_set<Address> latest_account_state_changings(m_latest_account_state_puts_flushed);
	for (put_item<Address, std::shared_ptr<account_state>> const & item : m_latest_account_state_puts)
		latest_account_state_changings.insert(item.key);
	m_cache->mark_latest_account_state_as_changing(latest_account_state_changings);

	//transactions
	std::unordered_set<h256> transaction_changings(m_transaction_puts_flushed);
	for (put_item<h256, std::shared_ptr<mcp::Transaction>> const & item : m_transaction_puts)
		transaction_changings.insert(item.key);
	m_cache->mark_transaction_as_changing(transaction_changings);

	//account nonce
	std::unordered_set<Address> account_nonce_changings(m_account_nonce_puts_flushed);
	for (put_item<Address, u256> const & item : m_account_nonce_puts)
		account_nonce_changings.insert(item.key);
	m_cache->mark_account_nonce_as_changing(account_nonce_changings);

	//successor
	std::unordered_set<mcp::block_hash> successor_changings(m_successor_puts_flushed);
	for (put_item<mcp::block_hash, mcp::block_hash> const & item : m_successor_puts)
		successor_changings.insert(item.key);
	for (mcp::block_hash const & hash : m_successor_dels)
		successor_changings.insert(hash);
	m_cache->mark_successor_as_changing(successor_changings);

	//block summary
	std::unordered_set<mcp::block_hash> block_summary_changings(m_block_summary_puts_flushed);
	for (put_item<mcp::block_hash, mcp::summary_hash> const & item : m_block_summary_puts)
		block_summary_changings.insert(item.key);
	m_cache->mark_block_summary_as_changing(block_summary_changings);

	//transaction receipt
	std::unordered_set<h256> receipt_changings(m_transaction_receipt_puts_flushed);
	for (put_item<h256, std::shared_ptr<dev::eth::TransactionReceipt>> const & item : m_transaction_receipt_puts)
		receipt_changings.insert(item.key);
	m_cache->mark_transaction_receipt_as_changing(receipt_changings);
}

void mcp::process_block_cache::commit_and_clear_changing()
{
	//modify block cache
	m_cache->block_earse(m_block_puts_flushed);
	for (put_item<mcp::block_hash, std::shared_ptr<block>> const & item : m_block_puts)
		m_cache->block_put(item.key, item.value);
	m_block_puts.clear();
	m_block_puts_flushed.clear();

	//modify block state cache
	m_cache->block_state_earse(m_block_state_puts_flushed);
	for (put_item<mcp::block_hash, std::shared_ptr<block_state>> const & item : m_block_state_puts)
		m_cache->block_state_put(item.key, item.value);
	m_block_state_puts.clear();
	m_block_state_puts_flushed.clear();

	//modify latest account state cache
	m_cache->latest_account_state_earse(m_latest_account_state_puts_flushed);
	for (put_item<Address, std::shared_ptr<account_state>> const & item : m_latest_account_state_puts)
		m_cache->latest_account_state_put(item.key, item.value);
	m_latest_account_state_puts.clear();
	m_latest_account_state_puts_flushed.clear();

	/// The global cache must be updated before the drop transaction.else getTransactionCount maybe error,because transaction drop from queue but nonce not update.
	//modify account nonce cache
	m_cache->account_nonce_earse(m_account_nonce_puts_flushed);
	for (put_item<Address, u256> const & item : m_account_nonce_puts)
		m_cache->account_nonce_put(item.key, item.value);
	m_account_nonce_puts.clear();
	m_account_nonce_puts_flushed.clear();


	//modify transaction cache
	m_cache->transaction_earse(m_transaction_puts_flushed);
	for (put_item<h256, std::shared_ptr<mcp::Transaction>> const & item : m_transaction_puts)
		m_cache->transaction_put(item.key, item.value);
	m_tq->drop(m_transaction_dels);
	m_transaction_puts.clear();
	m_transaction_puts_flushed.clear();
	m_transaction_dels.clear();

	//modify approve cache
	m_aq->drop(m_approve_dels);
	m_approve_dels.clear();

	//modify successor cache
	m_cache->successor_earse(m_successor_puts_flushed);
	m_cache->successor_earse(m_successor_dels);
	for (put_item<mcp::block_hash, mcp::block_hash> const & item : m_successor_puts)
		m_cache->successor_put(item.key, item.value);
	m_successor_puts.clear();
	m_successor_puts_flushed.clear();
	m_successor_dels.clear();

	//modify block summary cache
	m_cache->block_summary_earse(m_block_summary_puts_flushed);
	for (put_item<mcp::block_hash, mcp::summary_hash> const & item : m_block_summary_puts)
		m_cache->block_summary_put(item.key, item.value);
	m_block_summary_puts.clear();
	m_block_summary_puts_flushed.clear();

	//modify transaction receipt cache
	m_cache->transaction_receipt_earse(m_transaction_receipt_puts_flushed);
	for (put_item<h256, std::shared_ptr<dev::eth::TransactionReceipt>> const & item : m_transaction_receipt_puts)
		m_cache->transaction_receipt_put(item.key, item.value);
	m_transaction_receipt_puts.clear();
	m_transaction_receipt_puts_flushed.clear();

	//clear changing
	m_cache->clear_block_changing();
	m_cache->clear_block_state_changing();
	m_cache->clear_latest_account_state_changing();
	m_cache->clear_transaction_changing();
	m_cache->clear_account_nonce_changing();
	m_cache->clear_successor_changing();
	m_cache->clear_block_summary_changing();
	m_cache->clear_transaction_receipt_changing();
}

