#include "unhandle.hpp"
#include "arrival.hpp"
#include <queue>

constexpr size_t c_maxBlockPendingSize = 1000;

/// hash_a must be block hash
mcp::unhandle_add_result mcp::unhandle_cache::add(std::shared_ptr<mcp::block_processor_item> item_a,
	h256Hash const &bks,
	h256Hash const &transactions,
	h256Hash const &approves)
{
	auto hash_a = item_a->block_hash;
	std::lock_guard<std::mutex> lock (m_mutux);
	assert_x(!bks.count(hash_a));

	/// Determine if a transaction exists
	/// validation maybe missing,but transaction maybe arrived before add unhandle.
	h256Hash _txs;
	for (h256 const &h : transactions)
	{
		if (!m_tq->exist(h))
			_txs.insert(h);
	}
	/// Determine if a approve exists
	/// validation maybe missing,but approve maybe arrived before add unhandle.
	h256Hash _aps;
	for (h256 const &h : approves)
	{
		if (!m_aq->exist(h))
			_aps.insert(h);
	}
	if (bks.empty() && _txs.empty() && _aps.empty())
		return unhandle_add_result::Retry;

    bool exists(m_pending.count(hash_a));
    if (exists)
    {
		unhandle_exist_count++;
        return unhandle_add_result::Exist;
    }

	/// only accept unknown dependencies when size reach at half of capacity
	if (is_full() && item_a->is_broadCast())
	{
		unhandle_full_count++;
        return unhandle_add_result::Exist;
	}
	
	add_unhandle_ok_count++;

	mcp::unhandle_item u_item(item_a, bks, _txs, _aps);
	m_pending[hash_a] = u_item;

	///delete unknown dependency. 3<-4<-5, 3,4 in missing, received 4
	if (m_missings.count(hash_a))
		m_missings.erase(hash_a);

	bool allExist = true;
	for (mcp::block_hash const &bk : u_item.bks)
	{
		m_dependencies[bk].insert(hash_a);

		/// add unknown dependency
		if (!m_pending.count(bk))
		{
			m_missings.insert(bk);
			allExist = false;
		}
	}

	for (h256 const &h : u_item.txs)
	{
		m_transactions[h].insert(hash_a);
		/// add unknown dependency
		m_light_missings.insert(h);/// maybe failed because exist,unimportance.
	}

	for (h256 const &h : u_item.aps)
	{
		m_approves[h].insert(hash_a);
		/// add unknown dependency
		m_approve_missings.insert(h);/// maybe failed because exist,unimportance.
	}

	if (m_pending.size() > c_maxBlockPendingSize)
	{
		m_pending.clear();
		m_dependencies.clear();
		m_transactions.clear();
		m_approves.clear();
		m_missings.clear();
		m_light_missings.clear();
		m_approve_missings.clear();
		return unhandle_add_result::Nothing;
	}

	///dependice block,transaction,approves exist,need request existed missings.
	if (allExist && !u_item.txs.size()&& !u_item.aps.size())
	{
		return unhandle_add_result::Exist;
	}
    return unhandle_add_result::Success;
}

std::unordered_set<std::shared_ptr<mcp::block_processor_item>> mcp::unhandle_cache::release_dependency(mcp::block_hash const &dependency_hash_a)
{
	std::lock_guard<std::mutex> lock(m_mutux);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> result;

	if (m_dependencies.count(dependency_hash_a))
	{
		h256Hash pending_hashs = m_dependencies[dependency_hash_a];
		for (auto _h : pending_hashs)
		{
			assert_x(m_pending.count(_h));
			auto &unhandle = m_pending[_h];
			unhandle.bks.erase(dependency_hash_a);
			if (unhandle.ready())
			{
				result.insert(unhandle.item);
				m_pending.erase(_h);
			}
		}
        
		//delete dependency
		m_dependencies.erase(dependency_hash_a);
		//delete unknown dependencies
		m_missings.erase(dependency_hash_a);
	}

	return result;
}

std::unordered_set<std::shared_ptr<mcp::block_processor_item>> mcp::unhandle_cache::release_transaction_dependency(h256Hash const &hashs)
{
	std::lock_guard<std::mutex> lock(m_mutux);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> result;
	for (auto h : hashs)
	{
		if (!m_transactions.count(h))
			continue;

		h256Hash pending_hashs = m_transactions[h];
		for (auto _h : pending_hashs)
		{
			assert_x(m_pending.count(_h));
			auto &unhandle = m_pending[_h];
			unhandle.txs.erase(h);
			if (unhandle.ready())
			{
				result.insert(unhandle.item);
				m_pending.erase(_h);
			}
		}

		//delete dependency
		m_transactions.erase(h);
		//delete unknown dependencies
		m_light_missings.erase(h);
	}

	return result;
}

std::unordered_set<std::shared_ptr<mcp::block_processor_item>> mcp::unhandle_cache::release_approve_dependency(h256Hash const &hashs)
{
	std::lock_guard<std::mutex> lock(m_mutux);

	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> result;

	for (auto h : hashs)
	{
		if (!m_approves.count(h))
			return result;

		h256Hash pending_hashs = m_approves[h];
		for (auto _h : pending_hashs)
		{
			assert_x(m_pending.count(_h));
			auto &unhandle = m_pending[_h];
			unhandle.aps.erase(h);
			if (unhandle.ready())
			{
				result.insert(unhandle.item);
				m_pending.erase(_h);
			}
		}

		//delete dependency
		m_approves.erase(h);
		//delete unknown dependencies
		m_approve_missings.erase(h);
	}

	return result;
}

void mcp::unhandle_cache::get_missings(size_t const & missing_limit_a, std::vector<mcp::block_hash> & missings_a, std::vector<h256> & light_missings_a, std::vector<h256>& approve_missings_a)
{
	std::lock_guard<std::mutex> lock(m_mutux);

	if (m_missings.size() == 0 && m_pending.size() > 0)
	{
		auto it = m_pending.begin();
		size_t offset = mcp::random_pool.GenerateWord32(0, m_pending.size() - 1);
		for (; offset > 0; offset--)
			it++;
		mcp::block_hash start(0);
		while (missings_a.size() < 50 && start != it->first)
		{
			missings_a.push_back(it->first);
			if (start == mcp::block_hash(0))
				start = it->first;
			it++;
			if (it == m_pending.end())
				it = m_pending.begin();
		}
	}
	else
	{
		if (m_missings.size() <= missing_limit_a)
		{
			for (auto const &d : m_missings)
			{
				if (!m_pending.count(d))
					missings_a.push_back(d);
			}
		}
		else
		{
			auto it = m_missings.begin();
			size_t offset = mcp::random_pool.GenerateWord32(0, m_missings.size() - 1);
			for (; offset > 0; offset--)
				it++;
			mcp::block_hash start(0);
			while (missings_a.size() < missing_limit_a && start != *it)
			{
				if (!m_pending.count(*it))
					missings_a.push_back(*it);
				if (start == mcp::block_hash(0))
					start = *it;
				it++;
				if (it == m_missings.end())
					it = m_missings.begin();
			}
		}
	}

	size_t light_missing_limit = missing_limit_a - missings_a.size();

	h256Hash knowns;
	{
		/// will locked transaction queue
		knowns = m_tq->knownTransactions();
	}
	if (m_light_missings.size() <= light_missing_limit)
	{
		for (auto const &d : m_light_missings)
		{
			if (!knowns.count(d))
				light_missings_a.push_back(d);
		}
	}
	else
	{
		auto it = m_light_missings.begin();
		size_t offset = mcp::random_pool.GenerateWord32(0, m_light_missings.size() - 1);
		for (; offset > 0; offset--)
		{
			it++;
		}

		h256 start(0);
		while (light_missings_a.size() < light_missing_limit && start != *it)
		{
			if (!knowns.count(*it))
				light_missings_a.push_back(*it);
			if (start == h256(0))
				start = *it;
			it++;
			if (it == m_light_missings.end())
				it = m_light_missings.begin();
		}
	}

	size_t approve_missing_limit = missing_limit_a / 4;

	if (m_approve_missings.size() <= approve_missing_limit)
	{
		for (auto const &d : m_approve_missings)
		{
			approve_missings_a.push_back(d);
		}
	}
	else
	{
		auto it = m_approve_missings.begin();
		size_t offset = mcp::random_pool.GenerateWord32(0, m_approve_missings.size() - 1);
		for (; offset > 0; offset--)
		{
			it++;
		}

		h256 start(0);
		while (approve_missings_a.size() < approve_missing_limit && start != *it)
		{
			approve_missings_a.push_back(*it);
			if (start == h256(0))
				start = *it;
			it++;
			if (it == m_approve_missings.end())
				it = m_approve_missings.begin();
		}
	}
}

bool mcp::unhandle_cache::exists(mcp::block_hash const & block_hash_a)
{
	std::lock_guard<std::mutex> lock(m_mutux);

	auto it(m_pending.find(block_hash_a));
	return it != m_pending.end();
}

bool mcp::unhandle_cache::is_full()
{
	return m_pending.size() >= (c_maxBlockPendingSize / 2);
}

std::string mcp::unhandle_cache::getInfo()
{
	std::string str = "pending size:" + std::to_string(m_pending.size())
		+ " ,dependency size:" + std::to_string(m_dependencies.size())
		+ " ,missing size:" + std::to_string(m_missings.size())
		+ " ,txs :" + std::to_string(m_light_missings.size())
		+ " ,apx :" + std::to_string(m_approve_missings.size())
		+ " ,ok:" + std::to_string(add_unhandle_ok_count)
		+ " ,full:" + std::to_string(unhandle_full_count)
		+ " ,exist:" + std::to_string(unhandle_exist_count)
		;

	return str;
}

