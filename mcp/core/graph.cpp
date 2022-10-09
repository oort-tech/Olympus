#include "graph.hpp"
#include <mcp/core/genesis.hpp>

#include <queue>
#include <unordered_set>

mcp::graph::graph(mcp::block_store & store_a):
	m_store(store_a)
{
}

mcp::graph_compare_result mcp::graph::compare(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a,  mcp::block_hash hash1, mcp::block_hash hash2)
{
	if (hash1 == hash2)
		return graph_compare_result::equal;

	std::shared_ptr<mcp::block_state> state1(cache_a->block_state_get(transaction_a, hash1));
	assert_x(state1);

	std::shared_ptr<mcp::block_state> state2(cache_a->block_state_get(transaction_a, hash2));
	assert_x(state2);

	if (state1->level == state2->level)
		return graph_compare_result::non_related;
	if(state1->is_free && state2->is_free)
		return graph_compare_result::non_related;

	// genesis
	if (!state1->latest_included_mc_index)
		return graph_compare_result::hash1_included_by_hash2;
	if (!state2->latest_included_mc_index)
		return graph_compare_result::hash2_included_by_hash1;

	if (state2->main_chain_index && *state1->latest_included_mc_index >= *state2->main_chain_index)
		return graph_compare_result::hash2_included_by_hash1;
	if (state1->main_chain_index && *state2->latest_included_mc_index >= *state1->main_chain_index)
		return graph_compare_result::hash1_included_by_hash2;

	if ((state1->level <= state2->level
		&& *state1->latest_included_mc_index <= *state2->latest_included_mc_index
		&& (!state1->main_chain_index
			|| !state2->main_chain_index
			|| (*state1->main_chain_index <= *state2->main_chain_index)))
		||
		(state1->level >= state2->level
			&& *state1->latest_included_mc_index >= *state2->latest_included_mc_index
			&& (!state1->main_chain_index
				|| !state2->main_chain_index
				|| (*state1->main_chain_index >= *state2->main_chain_index))))
	{
	}
	else
		return graph_compare_result::non_related;

	mcp::block_hash earlier_hash(state1->level < state2->level ? hash1 : hash2);
	std::shared_ptr<mcp::block_state> earlier_state(state1->level < state2->level ? state1 : state2);
	mcp::block_hash later_hash(state1->level < state2->level ? hash2 : hash1);
	std::shared_ptr<mcp::block_state> later_state(state1->level < state2->level ? state2 : state1);
	mcp::graph_compare_result result_if_found = state1->level < state2->level ? graph_compare_result::hash1_included_by_hash2 
																		: graph_compare_result::hash2_included_by_hash1;

	uint64_t earlier_delta((earlier_state->main_chain_index ? *earlier_state->main_chain_index : 0) - *earlier_state->latest_included_mc_index);
	uint64_t later_delta((later_state->main_chain_index ? *later_state->main_chain_index : 0) - *later_state->latest_included_mc_index);

	if (later_delta > earlier_delta)
	{
		std::vector<mcp::block_hash> later_hashs;
		later_hashs.push_back(later_hash);

		if (go_up_check_included(transaction_a, cache_a, earlier_hash, later_hashs))
			return result_if_found;
	}
	else
	{
		std::vector<mcp::block_hash> earlier_hashs;
		earlier_hashs.push_back(earlier_hash);

		if (go_down_check_included(transaction_a, cache_a, later_hash, earlier_hashs))
			return result_if_found;
	}

	return graph_compare_result::non_related;
}

bool mcp::graph::determine_if_included(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a,  mcp::block_hash const & earlier_hash, std::vector<mcp::block_hash> const & later_hashs, bool const & is_trace)
{
	if (earlier_hash == mcp::genesis::block_hash)
		return true;
	std::shared_ptr<mcp::block_state> earlier_state(cache_a->block_state_get(transaction_a, earlier_hash));
	assert_x(earlier_state);

	if (is_trace)
	{
        LOG(m_log.info) << "earlier_hash: " << earlier_hash.hex()
			<< ", is_stable: " << earlier_state->is_stable
			<< ", is_on_mc: " << earlier_state->is_on_main_chain 
			<< ", mci: " << (earlier_state->main_chain_index ? *earlier_state->main_chain_index : 0)
			<< ", limci: " << earlier_state->latest_included_mc_index ? *earlier_state->latest_included_mc_index : 0;
	}

	if (earlier_state->is_free)
		return false;

	uint64_t max_later_limci(0);
	uint64_t max_later_level(0);
	for (mcp::block_hash const & later_hash : later_hashs)
	{
		std::shared_ptr<mcp::block_state> later_state(cache_a->block_state_get(transaction_a, later_hash));
		assert_x(later_state);

		if (is_trace)
		{
			uint64_t mc_index = later_state->latest_included_mc_index ? *later_state->latest_included_mc_index : 0;
            LOG(m_log.info) << "later_hash: " << later_hash.hex()
				<< ", is_stable: " << later_state->is_stable
				<< ", is_on_mc: " << later_state->is_on_main_chain
				<< ", mci: " << (later_state->main_chain_index ? *later_state->main_chain_index : 0)
				<< ", limci: " << mc_index;
		}

		if (later_state->latest_included_mc_index
			&& *later_state->latest_included_mc_index > max_later_limci)
			max_later_limci = *later_state->latest_included_mc_index;

		if (later_state->level > max_later_level)
			max_later_level = later_state->level;
	}

	if (earlier_state->main_chain_index
		&& max_later_limci >= *earlier_state->main_chain_index)
		return true;

	if (max_later_level < earlier_state->level)
		return false;

	return go_up_check_included(transaction_a, cache_a, earlier_hash, later_hashs, is_trace);
}

bool mcp::graph::determine_if_included_or_equal(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & earlier_hash, std::vector<mcp::block_hash> const & later_hashs, bool const & is_trace)
{
	if (std::find(later_hashs.begin(), later_hashs.end(), earlier_hash) != later_hashs.end())
		return true;
	return determine_if_included(transaction_a, cache_a, earlier_hash, later_hashs, is_trace);
}

bool mcp::graph::go_up_check_included(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & earlier_hash, std::vector<mcp::block_hash> const &  later_hashs, bool const & is_trace)
{
	std::shared_ptr<mcp::block_state> earlier_state(cache_a->block_state_get(transaction_a, earlier_hash));
	assert_x(earlier_state);
	assert_x(earlier_state->earliest_included_mc_index);
	assert_x(earlier_state->latest_included_mc_index);

	std::unordered_set<mcp::block_hash> searched_hashs;
	std::queue<mcp::block_hash> search_hashs;
	for (mcp::block_hash const & hash : later_hashs)
		search_hashs.push(hash);

	int search_count(0);
	while (search_hashs.size() > 0)
	{
		search_count++;
		mcp::block_hash hash(std::move(search_hashs.front()));
		search_hashs.pop();

		std::shared_ptr<mcp::block> block(cache_a->block_get(transaction_a, hash));
		assert_x(block != nullptr);

		for (mcp::block_hash const & p_hash : block->parents())
		{
			auto r = searched_hashs.insert(p_hash);
			if (!r.second)
				continue;

			if (p_hash == earlier_hash)
			{
				//LOG(m_log.trace) << "go_up_check_included search count: " << search_count;
				return true;
			}

			std::shared_ptr<mcp::block_state> p_state(cache_a->block_state_get(transaction_a, p_hash));
			assert_x(p_state);

			if (is_trace)
			{
                LOG(m_log.info) << "p_hash: " << p_hash.hex()
					<< ", is_stable: " << p_state->is_stable
					<< ", is_on_mc: " << p_state->is_on_main_chain
					<< ", mci: " << (p_state->main_chain_index ? *p_state->main_chain_index : 0)
					<< ", limci: " << p_state->latest_included_mc_index ? *p_state->latest_included_mc_index : 0;
			}

			if (p_state->is_on_main_chain)
			{
				if (earlier_state->main_chain_index
					&& *p_state->main_chain_index >= *earlier_state->main_chain_index)
				{
					//LOG(m_log.trace) << "go_up_check_included search count: " << search_count;
					return true;
				}
			}
			else
			{
				assert_x(p_state->earliest_included_mc_index);
				assert_x(p_state->latest_included_mc_index);
				if (*p_state->earliest_included_mc_index > *earlier_state->latest_included_mc_index
					|| *p_state->latest_included_mc_index < *earlier_state->earliest_included_mc_index)
					continue;

				if (p_state->level > earlier_state->level)
					search_hashs.push(p_hash);
			}
		}
	}

	//LOG(m_log.trace) << "go_up_check_included search count: " << search_count;
	return false;
}

bool mcp::graph::go_down_check_included(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & later_hash, std::vector<mcp::block_hash> const &  earlier_hashs)
{
	std::shared_ptr<mcp::block_state> later_state(cache_a->block_state_get(transaction_a, later_hash));
	assert_x(later_state);
	assert_x(later_state->earliest_included_mc_index);
	assert_x(later_state->latest_included_mc_index);

	std::unordered_set<mcp::block_hash> searched_hashs;
	std::queue<mcp::block_hash> search_hashs;
	for (mcp::block_hash const & hash : earlier_hashs)
		search_hashs.push(hash);
	std::queue<mcp::block_hash> next_search_hashs;

	while (search_hashs.size() > 0)
	{
		mcp::block_hash hash(std::move(search_hashs.front()));
		search_hashs.pop();

		//get children
		std::shared_ptr<std::list<mcp::block_hash>> child_hashs(std::make_shared<std::list<mcp::block_hash>>());
		m_store.block_children_get(transaction_a, hash, *child_hashs);
		for (auto it(child_hashs->begin()); it != child_hashs->end(); it++)
		{
			mcp::block_hash const & c_hash(*it);

			auto r = searched_hashs.insert(c_hash);
			if (!r.second)
				continue;

			if (hash == later_hash)
				return true;

			std::shared_ptr<mcp::block_state> p_state(cache_a->block_state_get(transaction_a, c_hash));
			assert_x(p_state);

			if (later_state->is_on_main_chain)
			{
				if (p_state->main_chain_index
					&& *later_state->main_chain_index >= *p_state->main_chain_index)
					return true;
			}
			else
			{
				assert_x(p_state->earliest_included_mc_index);
				assert_x(p_state->latest_included_mc_index);
				if (*later_state->earliest_included_mc_index > *p_state->latest_included_mc_index
					|| *later_state->latest_included_mc_index < *p_state->earliest_included_mc_index)
					continue;

				if (later_state->level < p_state->level)
					search_hashs.push(c_hash);
			}
		}
	}

	return false;
}


void  mcp::graph::test_determine_if_included_or_equal()
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::shared_ptr<mcp::block_cache> cache(std::make_shared<mcp::block_cache>(m_store));

	mcp::block_hash previous("0563E2B56907BADECDE03F55DFED61B98E4BEFC22243C7860744A765F4A3F7A8");

	std::vector<mcp::block_hash> parents;
	std::vector<std::string> parent_strs;
	parent_strs.push_back("5B9ECD8D3B2049FAF6D797D5BE4832C13D9E3B4D9E6E9C0A7881565ECF28C016");
	for (std::string const & p_str : parent_strs)
	{
		parents.push_back(mcp::block_hash(p_str));
	}

    LOG(m_log.info) << "determine_if_included_or_equal1 start";

	bool r = determine_if_included_or_equal(transaction, cache, previous, parents);

    LOG(m_log.info) << "determine_if_included_or_equal1:" << r;

    LOG(m_log.info) << "determine_if_included_or_equal2 start";
	int count(0);
	while (count < 100000)
	{
		r = determine_if_included_or_equal(transaction, cache, previous, parents);
		count++;
	}
    LOG(m_log.info) << "determine_if_included_or_equal2:" << r;
}