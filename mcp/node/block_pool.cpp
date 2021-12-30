#include <mcp/node/block_pool.hpp>
#include <mcp/node/node.hpp>

mcp::block_pool::block_pool(mcp::node & node_a) :
	m_node(node_a),
	m_store(node_a.store),
	m_cache(node_a.cache),
	m_last_request_unknown_missing_time(std::chrono::steady_clock::now())
{
}

bool mcp::block_pool::add(mcp::db::db_transaction & transaction_a, 
	std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_processor_item const & item)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	mcp::block_hash const & block_hash(item.block_hash);
	std::shared_ptr<mcp::block> block(item.joint.block);
	assert_x(block->hashables->type == mcp::block_type::light);

	mcp::db::db_transaction transaction(m_store.create_transaction());
	bool exists(cache_a->block_exists(transaction, block_hash));
	if (exists)
		return false;

	if (exists_in_light_queue(block_hash))
		return false;

	std::unordered_set<mcp::block_hash> dependences;
	std::unordered_set<mcp::block_hash> missings;

	if (exists_in_light_pending(block_hash))
	{
		if (!m_node.is_syncing())
			request_missings(item.remote_node_id());
		return false;
	}

	bool is_add_to_queue(true);

	mcp::block_hash const & previous(block->previous());
	std::shared_ptr<mcp::block> previous_block;
	if (!previous.is_zero())
	{
		previous_block = cache_a->block_get(transaction, previous);
		if (!previous_block)
		{
			std::shared_ptr<mcp::light_queue_item> previous_item(get_light_queue_item_internal(previous));
			if (previous_item)
			{
				previous_block = previous_item->item.joint.block;
			}
			else
			{
				dependences.insert(previous);
				if (!exists_in_light_pending(previous))
					missings.insert(previous);
			}
		}
	}

	if (dependences.empty())
	{
		assert_x(previous.is_zero() || previous_block);
		if (!add_to_light_queue(item, previous_block))
			return false;
	}
	else
		add_to_light_pending(item);

	update_pending_missing(block_hash, block->hashables->type, dependences, missings);

	if (!dependences.empty() && !m_node.is_syncing())
	{
		if (!missings.empty())
		{
			//to request missing block
			uint64_t now = m_node.steady_clock.now_since_epoch();
			for (mcp::block_hash const & unknown_missing : missings)
			{
				auto hash(std::make_shared<mcp::block_hash>(unknown_missing));
				mcp::joint_request_item request_item(item.remote_node_id(), hash, mcp::requesting_block_cause::new_unknown);
				m_node.sync->request_new_missing_joints(request_item, now);
			}
		}
		else
		{
			request_missings(item.remote_node_id());
		}
	}

	return true;
}

std::shared_ptr<mcp::light_queue_item> mcp::block_pool::get_light_queue_item(mcp::block_hash const & block_hash_a)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return get_light_queue_item_internal(block_hash_a);
}

std::shared_ptr<mcp::light_queue_item> mcp::block_pool::get_light_queue_item_internal(mcp::block_hash const & block_hash_a)
{
	std::shared_ptr<mcp::light_queue_item> result(nullptr);
	auto it = m_light_queue.find(block_hash_a);
	bool exists = it != m_light_queue.end();
	if (exists)
		result = it->second;
	return result;
}

std::shared_ptr<mcp::block> mcp::block_pool::get_block_by_hash(mcp::block_hash const & block_hash_a)
{
	//to do added block type

	std::lock_guard<std::mutex> lock(m_mutex);
	std::shared_ptr<mcp::block> result(nullptr);
	auto it = m_light_queue.find(block_hash_a);
	if (it != m_light_queue.end())
	{
		result = it->second->item.joint.block;
	}

	return result;
}

std::shared_ptr<std::list<mcp::block_hash>> mcp::block_pool::random_get_links(mcp::uint256_t const & min_gas_price ,size_t const & limit_a)
{
	std::list<mcp::block_hash> ordered_links;

	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_unlink.empty())
	{
		uint64_t rand = mcp::random_pool.GenerateWord32(0, m_unlink.size() - 1);
		auto it = m_unlink.begin();
		for (size_t i = 0; i < rand; i++) //random
			it++;

		mcp::account start_account(0);
		while (ordered_links.size() < limit_a)
		{
			if (it == m_unlink.end())
				it = m_unlink.begin();

			mcp::block_hash cur_hash(it->second.earliest_unlink);

			mcp::block_hash last_unlink_hash(0);
			if (start_account == it->first)  // eq start ,break
				break;
			if (start_account.is_zero())
				start_account = it->first;

			if (!cur_hash.is_zero())
			{
				while (true)
				{
					auto cur_block = m_light_queue.find(cur_hash);
					assert_x_msg(cur_block != m_light_queue.end(), "light_queue have no current hash!");
					if (cur_block->second->item.joint.block->hashables->gas_price >= min_gas_price)
					{
						last_unlink_hash = cur_hash;

						auto it_light(m_light_successor.find(cur_hash));
						if (it_light != m_light_successor.end())
						{
							cur_hash = it_light->second;
						}
						else // no successor,cur_hash is last
						{
							if (!last_unlink_hash.is_zero())
								ordered_links.push_back(last_unlink_hash);
							break;
						}
					}
					else
					{
						if (!last_unlink_hash.is_zero())
							ordered_links.push_back(last_unlink_hash);
						break;
					}
				}
			}
			it++;
		}
	}

	ordered_links.sort();
	return std::make_shared<std::list<mcp::block_hash>>(ordered_links);
}

std::shared_ptr<std::list<mcp::block_hash>> mcp::block_pool::random_get_latest_unlinks(size_t const & limit)
{
	std::list<mcp::block_hash> ordereds;

	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_unlink.empty())
	{
		uint64_t rand = mcp::random_pool.GenerateWord32(0, m_unlink.size() - 1);
		auto it = m_unlink.begin();
		for (size_t i = 0; i < rand; i++) //random
			it++;

		mcp::account start_account(0);
		while (ordereds.size() < limit)
		{
			if (it == m_unlink.end())
				it = m_unlink.begin();

			if (start_account == it->first)  // eq start ,break
				break;
			if (start_account.is_zero())
				start_account = it->first;

			if (!it->second.latest_unlink.is_zero())
				ordereds.push_back(it->second.latest_unlink);
			it++;
		}
	}

	return std::make_shared<std::list<mcp::block_hash>>(ordereds);
}

mcp::block_hash mcp::block_pool::get_latest_block(mcp::block_type const & type_a, mcp::account const & account_a)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash previous;
	switch (type_a)
	{
	case mcp::block_type::dag:
	{
		mcp::dag_account_info info;
		bool account_error(m_store.dag_account_get(transaction, account_a, info));
		previous = account_error ? 0 : info.latest_stable_block;

		break;
	}
	case mcp::block_type::light:
	{
		mcp::account_info info;
		bool account_error(m_store.account_get(transaction, account_a, info));
		previous = account_error ? 0 : info.latest_stable_block;

		break;
	}
	default:
		assert_x_msg(false, "Invalid block type");
	}

	mcp::block_hash latest_block_hash(previous);
	mcp::block_hash root(previous.is_zero() ? account_a : previous);

	//search in chain
	while (true)
	{
		mcp::block_hash successor_hash;
		bool exists(!m_store.successor_get(transaction, root, successor_hash));
		if (!exists)
			break;

		latest_block_hash = std::move(successor_hash);
		root = latest_block_hash;
	}

	if (type_a == mcp::block_type::light)
	{
		//search in block pool
		while (true)
		{
			auto it(m_light_successor.find(root));
			if (it == m_light_successor.end())
				break;

			latest_block_hash = it->second;
			root = latest_block_hash;
		}
	}

	return latest_block_hash;
}

void mcp::block_pool::erase(mcp::block_processor_item const & item_a)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	std::shared_ptr<mcp::block> block(item_a.joint.block);
	assert_x(block->hashables->type == mcp::block_type::light);

	std::shared_ptr<mcp::light_queue_item> light_item(get_light_queue_item_internal(item_a.block_hash));
	if (!light_item)
		return;

	if (light_item->item.is_successor)
		m_light_successor.erase(block->root());

	m_light_queue.erase(item_a.block_hash);
}

bool mcp::block_pool::is_full()
{
	return false;
}

void mcp::block_pool::add_to_dag_pending(mcp::block_processor_item const & item)
{
	auto r = m_dag_pending.insert(std::make_pair(item.block_hash, std::move(item)));
	assert_x(r.second);
}

bool mcp::block_pool::add_to_light_queue(mcp::block_processor_item const & item , std::shared_ptr<mcp::block> previous_block)
{
	//std::shared_ptr<mcp::block> block(item.joint.block);

	//bool is_successor(true);
	//bool has_successor_in_pool(false);
	//auto it = m_light_successor.find(block->root());
	//if (it != m_light_successor.end())
	//{
	//	is_successor = false;
	//	has_successor_in_pool = true;
	//}
	//else
	//{
	//	mcp::db::db_transaction transaction(m_store.create_transaction());
	//	mcp::block_hash successor_hash;
	//	bool exists(!m_store.successor_get(transaction, block->root(), successor_hash));
	//	if(exists)
	//		is_successor = false;
	//}

	//mcp::block_hash block_hash(item.block_hash);

	//unlink & ulk = get_unlink(block->hashables->from);
	//if ((!ulk.latest_unlink.is_zero() && block->previous() == ulk.latest_unlink)
	//	|| (ulk.latest_unlink.is_zero() && block->previous() == ulk.latest_linked))
	//{
	//	assert_x(is_successor);

	//	//update earliest_unlink
	//	if (ulk.earliest_unlink.is_zero())
	//	{
	//		ulk.latest_linked = block->previous();
	//		ulk.earliest_unlink = block_hash;
	//	}

	//	//update latest_unlink
	//	ulk.latest_unlink = block_hash;
	//}
	//else if(has_successor_in_pool)
	//{
	//	mcp::block_hash const & successor_hash(it->second);
	//	std::shared_ptr<mcp::light_queue_item> successor_in_pool(get_light_queue_item_internal(successor_hash));
	//	assert_x(successor_in_pool);
	//	std::shared_ptr<mcp::block> successor_block(successor_in_pool->item.joint.block);
	//	//replace successor with higher gas_price
	//	if (!successor_in_pool->linked 
	//		&& block->hashables->gas_price > successor_block->hashables->gas_price)
	//	{
	//		bool to_be_successor(false);
	//		mcp::block_hash unlink_hash(ulk.latest_unlink);
	//		while (true)
	//		{
	//			auto it(m_light_queue.find(unlink_hash));
	//			assert_x(it != m_light_queue.end());
	//			std::shared_ptr<light_queue_item> light_item(it->second);
	//			if (light_item->item.joint.block->root() == block->root())
	//			{
	//				to_be_successor = true;
	//				break;
	//			}

	//			if (unlink_hash == ulk.earliest_unlink)
	//				break;
	//		}

	//		if (to_be_successor)
	//		{
	//			m_light_successor.erase(block->root());
	//			is_successor = true;

	//			//update earliest_unlink
	//			if (unlink_hash == ulk.earliest_unlink)
	//			{
	//				ulk.latest_linked = block->previous();
	//				ulk.earliest_unlink = block_hash;
	//			}

	//			//update latest_unlink
	//			ulk.latest_unlink = block_hash;
	//		}
	//	}
	//}

	//auto light_item(std::make_shared<mcp::light_queue_item>(std::move(item)));
	//light_item->item.is_successor = is_successor;
	//auto r = m_light_queue.insert(std::make_pair(block_hash, light_item));
	//assert_x(r.second);
	//if (is_successor)
	//{
	//	auto r = m_light_successor.insert(std::make_pair(block->root(), block_hash));
	//	assert_x(r.second);
	//}

	return true;
}

void mcp::block_pool::add_to_light_pending(mcp::block_processor_item const & item)
{
	auto r = m_light_pending.insert(std::make_pair(item.block_hash, std::move(item)));
	assert_x(r.second);
}

void mcp::block_pool::try_get_pendings(mcp::block_hash const & missing_hash_a, std::list<mcp::block_processor_item> & pendings)
{
		mcp::block_hash const & missing_hash(missing_hash_a);

		auto it_range(m_dependences.equal_range(missing_hash));
		auto it = it_range.first;
		while (it != it_range.second)
		{
			mcp::block_hash pending_hash(it->pending);
			mcp::block_type pending_type(it->pending_type);
			//get next it
			it = m_dependences.erase(it);

			switch (pending_type)
			{
			case mcp::block_type::dag:
			{
				if (m_dependences.get<1>().find(pending_hash) == m_dependences.get<1>().end())
				{
					auto p_it = m_dag_pending.find(pending_hash);
					assert_x(p_it != m_dag_pending.end());

					mcp::block_processor_item const & pending_item(p_it->second);
					pendings.push_back(std::move(pending_item));

					m_dag_pending.erase(p_it);
				}

				break;
			}
			case mcp::block_type::light:
			{
				auto p_it = m_light_pending.find(pending_hash);
				assert_x(p_it != m_light_pending.end());
				mcp::block_processor_item const & pending_item(p_it->second);
				pendings.push_back(std::move(pending_item));

				m_light_pending.erase(p_it);

				break;
			}
			default:
				assert_x_msg(false, "Invalid pending type");
			}
		}
}

void mcp::block_pool::try_delete_pending_by_invalid_hash(mcp::block_hash const & missing_hash_a)
{
	std::queue<mcp::block_hash> dependences;
	dependences.push(missing_hash_a);
	std::unordered_set<mcp::block_hash> invalids;
	while (!dependences.empty())
	{
		mcp::block_hash missing_hash(std::move(dependences.front()));
		dependences.pop();

		auto it_range(m_dependences.equal_range(missing_hash));
		auto it = it_range.first;
		while (it != it_range.second)
		{
			mcp::block_hash pending_hash(it->pending);
			mcp::block_type pending_type(it->pending_type);
			//get next it
			it = m_dependences.erase(it);

			switch (pending_type)
			{
			case mcp::block_type::dag:
			{
				m_dependences.get<1>().erase(pending_hash);
				m_dag_pending.erase(pending_hash);
				dependences.push(pending_hash);
				break;
			}
			case mcp::block_type::light:
			{
				m_light_pending.erase(pending_hash);
				dependences.push(pending_hash);
				break;
			}
			default:
				assert_x_msg(false, "Invalid pending type");
			}
		}
	}
}

void mcp::block_pool::update_pending_missing(mcp::block_hash const & new_block_hash_a,
	mcp::block_type const & new_block_type_a,
	std::unordered_set<mcp::block_hash> const & dependences_a,
	std::unordered_set<mcp::block_hash> const & missings_a)
{
	for (mcp::block_hash const & missing : dependences_a)
		m_dependences.insert(missing_item(missing, new_block_hash_a, new_block_type_a));

	m_missings.get<1>().erase(new_block_hash_a);
	for (mcp::block_hash const & unknown_missing : missings_a)
	{
		auto r = m_missings.push_back(unknown_missing);
	}
}

bool mcp::block_pool::exists_in_dag_pending(mcp::block_hash const & block_hash_a)
{
	return m_dag_pending.find(block_hash_a) != m_dag_pending.end();
}

bool mcp::block_pool::exists_in_light_queue(mcp::block_hash const & block_hash_a)
{
	return m_light_queue.find(block_hash_a) != m_light_queue.end();
}

bool mcp::block_pool::exists_in_light_pending(mcp::block_hash const & block_hash_a)
{
	return m_light_pending.find(block_hash_a) != m_light_pending.end();
}

void mcp::block_pool::request_missings(p2p::node_id const& id)
{
	//remove block arrival
	auto now = std::chrono::steady_clock::now();
	if (now - m_last_request_unknown_missing_time >= std::chrono::seconds(1))
	{
		static const size_t limit = 500;
		std::list<mcp::block_hash> existing_unknown_missings;
		if (m_missings.size() <= limit)
		{
			for (auto it(m_missings.begin()); it != m_missings.end(); it++)
				existing_unknown_missings.push_back(*it);
		}
		else
		{
			size_t offset = mcp::random_pool.GenerateWord32(0, m_missings.size() - 1);
			auto it = m_missings.begin() + offset;
			while (existing_unknown_missings.size() < limit)
			{
				if (it == m_missings.end())
					it = m_missings.begin();
				existing_unknown_missings.push_back(*it);
			}
		}

		if (!existing_unknown_missings.empty())
		{
			uint64_t now = m_node.steady_clock.now_since_epoch();
			for (mcp::block_hash const & existing_unknown_missing : existing_unknown_missings)
			{
				m_node.block_arrival.remove(existing_unknown_missing);
				auto hash(std::make_shared<mcp::block_hash>(existing_unknown_missing));
				mcp::joint_request_item request_item(id, hash, mcp::requesting_block_cause::existing_unknown);
				m_node.sync->request_new_missing_joints(request_item, now);
			}
		}

		m_last_request_unknown_missing_time = now;
	}
}

void mcp::block_pool::on_successor_change(std::unordered_set<mcp::account> const & accounts_a)
{
	//std::lock_guard<std::mutex> lock(m_mutex);
	for (auto it = accounts_a.begin(); it != accounts_a.end(); it++)
	{
		mcp::account const & account(*it);
		mcp::unlink unlink_a;
		if (get_light_unlink(account, unlink_a))
			m_unlink[account] = unlink_a;
	}
}

bool mcp::block_pool::get_light_unlink(mcp::account const & account_a, mcp::unlink & unlink_a)
{
	bool ret = false;

	mcp::db::db_transaction transaction(m_store.create_transaction());
	
	mcp::account_info info;
	bool account_error(m_store.account_get(transaction, account_a, info));
	mcp::block_hash latest_block_hash = account_error ? 0 : info.latest_stable_block;
	mcp::block_hash root(latest_block_hash.is_zero() ? account_a : latest_block_hash);

	//search in chain
	while (true)
	{
		mcp::block_hash successor_hash;
		bool exists(!m_store.successor_get(transaction, root, successor_hash));
		if (!exists)
			break;

		latest_block_hash = std::move(successor_hash);
		root = latest_block_hash;
		unlink_a.latest_linked = latest_block_hash;
		ret = true;
	}

	//search in block pool
	while (true)
	{
		auto it(m_light_successor.find(root));
		if (it == m_light_successor.end())
			break;

		latest_block_hash = it->second;
		root = latest_block_hash;

		std::shared_ptr<mcp::light_queue_item> successor_in_pool(get_light_queue_item_internal(latest_block_hash));
		assert_x(successor_in_pool);
		std::shared_ptr<mcp::block> successor_block(successor_in_pool->item.joint.block);
		if (successor_in_pool->linked)
			unlink_a.latest_linked = latest_block_hash;
		else
		{
			if (unlink_a.earliest_unlink.is_zero())
				unlink_a.earliest_unlink = latest_block_hash;
			unlink_a.latest_unlink = latest_block_hash;
		}
		ret = true;
	}
	return ret;
}

mcp::unlink mcp::block_pool::get_unlink(mcp::account const & account_a)
{
	//std::lock_guard<std::mutex> lock(m_mutex);
	if (m_unlink.count(account_a))
	{
		return m_unlink[account_a];
	}
	else
	{
		mcp::unlink unlink_a;
		if (get_light_unlink(account_a, unlink_a))
			m_unlink[account_a] = unlink_a;
		return unlink_a;
	}
}

void mcp::block_pool::put_successor_change_account(mcp::account const& account_a)
{
	m_to_del_accounts.insert(account_a);
}

void mcp::block_pool::on_successor_changes()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_to_del_accounts.empty())
	{
		std::unordered_set<mcp::account> to_del_accounts;
		std::swap(to_del_accounts, m_to_del_accounts);
		on_successor_change(to_del_accounts);
	}
}
