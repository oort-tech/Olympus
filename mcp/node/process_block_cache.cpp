#include "process_block_cache.hpp"

mcp::process_block_cache::process_block_cache(std::shared_ptr<mcp::block_cache> cache_a, mcp::block_store &store_a) :
	m_cache(cache_a),
	m_store(store_a)
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


std::shared_ptr<mcp::account_state> mcp::process_block_cache::latest_account_state_get(mcp::db::db_transaction & transaction_a, mcp::account const & account_a)
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
			mcp::account_state_hash hash;
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

void mcp::process_block_cache::latest_account_state_put(mcp::db::db_transaction & transaction_a, mcp::account const & account_a, std::shared_ptr<mcp::account_state> account_state_a)
{
	mcp::account_state_hash acc_hash(account_state_a->hash());
	m_store.account_state_put(transaction_a, acc_hash, *account_state_a);
	m_store.latest_account_state_put(transaction_a, account_a, acc_hash);
	auto it(m_latest_account_state_puts.get<1>().find(account_a));
	if (it != m_latest_account_state_puts.get<1>().end())
	{
		m_latest_account_state_puts.get<1>().modify(it, [account_state_a](put_item<mcp::account, std::shared_ptr<mcp::account_state>> & item_a)
		{
			item_a.value = account_state_a;
		});
	}
	else
	{
		auto r = m_latest_account_state_puts.push_back(put_item<mcp::account, std::shared_ptr<mcp::account_state>>(account_a, account_state_a));
		assert_x(r.second);
		if (m_latest_account_state_puts.size() >= m_max_latest_account_state_puts_size)
		{
			while (m_latest_account_state_puts.size() >= m_max_latest_account_state_puts_size / 2)
			{
				put_item<mcp::account, std::shared_ptr<account_state>> const & item(m_latest_account_state_puts.front());
				m_latest_account_state_puts_flushed.insert(std::move(item.key));
				m_latest_account_state_puts.pop_front();
			}
		}
	}
}


bool mcp::process_block_cache::unlink_block_exists(mcp::db::db_transaction & transaction_a, mcp::block_hash const &block_hash_a)
{
	std::shared_ptr<mcp::unlink_block> unlink_block = unlink_block_get(transaction_a, block_hash_a);
	bool exists = unlink_block != nullptr;
	return exists;
}

std::shared_ptr<mcp::unlink_block> mcp::process_block_cache::unlink_block_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a)
{
	std::shared_ptr<mcp::unlink_block> unlink_block;
	if (m_unlink_block_dels.count(block_hash_a))
		return nullptr;

	auto it(m_unlink_block_puts.get<1>().find(block_hash_a));
	if (it != m_unlink_block_puts.get<1>().end())
	{
		unlink_block = it->value;
	}
	else
	{
		if (m_unlink_block_puts_flushed.count(block_hash_a))
			unlink_block = m_store.unlink_block_get(transaction_a, block_hash_a);
		else
			unlink_block = m_cache->unlink_block_get(transaction_a, block_hash_a);
	}
	return unlink_block;
}

void mcp::process_block_cache::unlink_block_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::shared_ptr<mcp::unlink_block> unlink_block_a)
{
	m_store.unlink_block_put(transaction_a, block_hash_a, *unlink_block_a);
	auto r = m_unlink_block_puts.push_back(put_item<mcp::block_hash, std::shared_ptr<mcp::unlink_block>>(block_hash_a, unlink_block_a));
	assert_x(r.second);
	m_unlink_block_dels.erase(block_hash_a);

	if (m_unlink_block_puts.size() >= m_max_unlink_block_puts_size)
	{
		while (m_unlink_block_puts.size() >= m_max_unlink_block_puts_size / 2)
		{
			put_item<mcp::block_hash, std::shared_ptr<unlink_block>> const & item(m_unlink_block_puts.front());
			m_unlink_block_puts_flushed.insert(std::move(item.key));
			m_unlink_block_puts.pop_front();
		}
	}
}

void mcp::process_block_cache::unlink_block_del(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a)
{
	m_store.unlink_block_del(transaction_a, block_hash_a);
	auto r(m_unlink_block_dels.insert(block_hash_a));
	assert_x(r.second);
	m_unlink_block_puts.get<1>().erase(block_hash_a);
	m_unlink_block_puts_flushed.erase(block_hash_a);
}


std::shared_ptr<mcp::account_info> mcp::process_block_cache::account_get(mcp::db::db_transaction & transaction_a, mcp::account const & account_a)
{
	std::shared_ptr<mcp::account_info> account_info;
	auto it(m_account_puts.get<1>().find(account_a));
	if (it != m_account_puts.get<1>().end())
	{
		account_info = it->value;
	}
	else
	{
		if (m_account_puts_flushed.count(account_a))
			account_info = m_store.account_get(transaction_a, account_a);
		else
			account_info = m_cache->account_get(transaction_a, account_a);
	}
	return account_info;
}

void mcp::process_block_cache::account_put(mcp::db::db_transaction & transaction_a, mcp::account const & account_a, std::shared_ptr<mcp::account_info> account_info_a)
{
	m_store.account_put(transaction_a, account_a, *account_info_a);
	auto it(m_account_puts.get<1>().find(account_a));
	if (it != m_account_puts.get<1>().end())
	{
		m_account_puts.get<1>().modify(it, [account_info_a](put_item<mcp::account, std::shared_ptr<mcp::account_info>> & item_a)
		{
			item_a.value = account_info_a;
		});
	}
	else
	{
		auto r = m_account_puts.push_back(put_item<mcp::account, std::shared_ptr<mcp::account_info>>(account_a, account_info_a));
		assert_x(r.second);
		if (m_account_puts.size() >= m_max_account_puts_size)
		{
			while (m_account_puts.size() >= m_max_account_puts_size / 2)
			{
				put_item<mcp::account, std::shared_ptr<account_info>> const & item(m_account_puts.front());
				m_account_puts_flushed.insert(std::move(item.key));
				m_account_puts.pop_front();
			}
		}
	}
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
	std::unordered_set<mcp::account> latest_account_state_changings(m_latest_account_state_puts_flushed);
	for (put_item<mcp::account, std::shared_ptr<account_state>> const & item : m_latest_account_state_puts)
		latest_account_state_changings.insert(item.key);
	m_cache->mark_latest_account_state_as_changing(latest_account_state_changings);

	//unlink block
	std::unordered_set<mcp::block_hash> unlink_block_changings(m_unlink_block_puts_flushed);
	for (put_item<mcp::block_hash, std::shared_ptr<unlink_block>> const & item : m_unlink_block_puts)
		unlink_block_changings.insert(item.key);
	for (mcp::block_hash const & hash : m_unlink_block_dels)
		unlink_block_changings.insert(hash);
	m_cache->mark_unlink_block_as_changing(unlink_block_changings);

	//account info
	std::unordered_set<mcp::account> account_changings(m_account_puts_flushed);
	for (put_item<mcp::account, std::shared_ptr<account_info>> const & item : m_account_puts)
		account_changings.insert(item.key);
	m_cache->mark_account_as_changing(account_changings);

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
	for (put_item<mcp::account, std::shared_ptr<account_state>> const & item : m_latest_account_state_puts)
		m_cache->latest_account_state_put(item.key, item.value);
	m_latest_account_state_puts.clear();
	m_latest_account_state_puts_flushed.clear();

	//modify unlink block cache
	m_cache->unlink_block_earse(m_unlink_block_puts_flushed);
	m_cache->unlink_block_earse(m_unlink_block_dels);
	for (put_item<mcp::block_hash, std::shared_ptr<unlink_block>> const & item : m_unlink_block_puts)
		m_cache->unlink_block_put(item.key, item.value);
	m_unlink_block_puts.clear();
	m_unlink_block_puts_flushed.clear();
	m_unlink_block_dels.clear();

	//modify account cache
	m_cache->account_earse(m_account_puts_flushed);
	for (put_item<mcp::account, std::shared_ptr<mcp::account_info>> const & item : m_account_puts)
		m_cache->account_put(item.key, item.value);
	m_account_puts.clear();
	m_account_puts_flushed.clear();

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

	//clear changing
	m_cache->clear_block_changing();
	m_cache->clear_block_state_changing();
	m_cache->clear_latest_account_state_changing();
	m_cache->clear_unlink_block_changing();
	m_cache->clear_account_changing();
	m_cache->clear_successor_changing();
	m_cache->clear_block_summary_changing();
}

