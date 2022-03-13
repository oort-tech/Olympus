#include "ledger.hpp"
#include <mcp/core/genesis.hpp>
#include <mcp/common/stopwatch.hpp>

#include <queue>
#include <unordered_set>

mcp::ledger::ledger()
{
}

//best parent:compatible parent, witnessed_level DESC, level ASC, block hash ASC
mcp::block_hash mcp::ledger::determine_best_parent(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, 
	std::vector<mcp::block_hash> const &pblock_hashs)
{
	mcp::block_hash best_pblock_hash(0);
	std::shared_ptr<mcp::block_state> best_pblock_state;
	for (mcp::block_hash const &pblock_hash : pblock_hashs)
	{
		std::shared_ptr<mcp::block> pblock(cache_a->block_get(transaction_a, pblock_hash));
		std::shared_ptr<mcp::block_state> pblock_state(cache_a->block_state_get(transaction_a, pblock_hash));
		assert_x(pblock_state);

		if (best_pblock_state == nullptr
			|| (pblock_state->witnessed_level > best_pblock_state->witnessed_level)
			|| (pblock_state->witnessed_level == best_pblock_state->witnessed_level
				&& pblock_state->level > best_pblock_state->level)
			|| (pblock_state->witnessed_level == best_pblock_state->witnessed_level
				&& pblock_state->level == best_pblock_state->level
				&& pblock_hash < best_pblock_hash))
		{
			best_pblock_hash = pblock_hash;
			best_pblock_state = pblock_state;
		}
	}

	return best_pblock_hash;
}

//level = best parent level + 1
uint64_t mcp::ledger::calc_level(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & best_pblock_hash)
{
	std::shared_ptr<mcp::block_state> bp_block_state(cache_a->block_state_get(transaction_a, best_pblock_hash));
	uint64_t level(bp_block_state->level + 1);
	return level;
}

//witness level = level - majority + 1
uint64_t mcp::ledger::calc_witnessed_level(mcp::witness_param const & witness_param_a, uint64_t const & level)
{
	assert_x(witness_param_a.majority_of_witnesses > 0);
	uint64_t wl_diff(witness_param_a.majority_of_witnesses - 1);
	uint64_t witnessed_level(level > wl_diff ? level - wl_diff : 0);
	return witnessed_level;
}

//witnessed level: search up along best parents, if meet majority of witnesses, the level is witnessed level
bool mcp::ledger::check_majority_witness(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a,
	mcp::block_hash const &best_pblock_hash, mcp::account const & block_from, mcp::witness_param const & witness_param_a)
{
	uint64_t const & majority_of_witnesses(witness_param_a.majority_of_witnesses);
	assert_x(majority_of_witnesses > 0);

	if(majority_of_witnesses == 1)
		return true;

	std::unordered_set<mcp::account> collected_witnesses;
	collected_witnesses.insert(block_from);
	mcp::block_hash mc_block_hash(best_pblock_hash);
	while (true)
	{
		if (mc_block_hash == mcp::genesis::block_hash)
			return true;

		std::shared_ptr<mcp::block> mc_block = cache_a->block_get(transaction_a, mc_block_hash);
		assert_x(mc_block);
		if (collected_witnesses.count(mc_block->hashables->from))
			break;
		else
		{
			collected_witnesses.insert(mc_block->hashables->from);
			assert_x(collected_witnesses.size() <= majority_of_witnesses);
			if (collected_witnesses.size() == majority_of_witnesses)
				return true;
		}

		std::shared_ptr<mcp::block_state> mc_block_state(cache_a->block_state_get(transaction_a, mc_block_hash));
		assert_x(mc_block_state);
		mc_block_hash = mc_block_state->best_parent;
	}

	return false;
}


std::shared_ptr<mcp::min_wl_result> mcp::ledger::find_mc_min_wl(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, uint64_t const & witnessed_level_a, mcp::block_hash const &best_parent_block_hash_a, mcp::account const & block_from)
{
	{
		//mcp::stopwatch_guard sw("find_mc_min_wl");

		uint64_t const & mc_end_level(witnessed_level_a);
		std::shared_ptr<mcp::min_wl_result> min_wl_result(std::make_shared<mcp::min_wl_result>());
		min_wl_result->min_wl = witnessed_level_a;
		mcp::block_hash best_pblock_hash(best_parent_block_hash_a);

		min_wl_result->witnesses.insert(block_from);

		while (true)
		{
			if (best_pblock_hash == mcp::genesis::block_hash)
				break;

			std::shared_ptr<mcp::block_state> mc_block_state(cache_a->block_state_get(transaction_a, best_pblock_hash));
			assert_x(mc_block_state);

			if (mc_block_state->level < mc_end_level)
				break;

			std::shared_ptr<mcp::block> mc_block(cache_a->block_get(transaction_a, best_pblock_hash));
			assert_x(mc_block);

			if (!min_wl_result->witnesses.count(mc_block->hashables->from))
			{
				// std::cerr << "min_wl witness:" << mc_block->hashables->from.to_account() << std::endl;
				// std::cerr << "min_wl witness block:" << best_pblock_hash.to_string() << std::endl;

				min_wl_result->witnesses.insert(mc_block->hashables->from);
				if (mc_block_state->witnessed_level < min_wl_result->min_wl)
				{
					min_wl_result->min_wl = mc_block_state->witnessed_level;
				}
			}

			best_pblock_hash = mc_block_state->best_parent;
		}

		return min_wl_result;
	}
}

bool mcp::ledger::check_stable(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a,
	mcp::block_hash const &earlier_hash, mcp::block_hash const & bp_block_hash, std::vector<mcp::block_hash> const &parents_a,
	mcp::account const & block_from_a, mcp::block_hash const & checked_stable_block_hash, mcp::witness_param const & witness_param_a)
{
	//mcp::stopwatch_guard sw("check_stable");

	//genesis
	if (earlier_hash == mcp::genesis::block_hash)
	{
		//LOG(m_log.debug) << "check stable return true1";
		return true;
	}

	std::shared_ptr<mcp::block_state> earlier_block_state;
	std::shared_ptr<mcp::block_state> checked_stable_block_state;
	{
		//mcp::stopwatch_guard sw("check_stable0");

		earlier_block_state = cache_a->block_state_get(transaction_a, earlier_hash);
		assert_x(earlier_block_state);
		if (!earlier_block_state->is_on_main_chain)
			return false;

		//if (earlier_block_state->is_free)
		//	return false;

		checked_stable_block_state = cache_a->block_state_get(transaction_a, checked_stable_block_hash);
		assert_x(checked_stable_block_state);
		assert_x(checked_stable_block_state->is_on_main_chain);
		assert_x(checked_stable_block_state->main_chain_index);
	}

	uint64_t const & earlier_mci(*earlier_block_state->main_chain_index);
	uint64_t const & checked_stable_mci(*checked_stable_block_state->main_chain_index);

	//earlier block is stable in view of best parent
	if (earlier_mci <= checked_stable_mci)
	{
		//LOG(m_log.debug) << "check stable return true2";
		return true;
	}

	std::shared_ptr<mcp::min_wl_result> min_wl_result;
	{
		//mcp::stopwatch_guard sw("check_stable1");

		uint64_t level(calc_level(transaction_a, cache_a, bp_block_hash));
		uint64_t witnessed_level(calc_witnessed_level(witness_param_a, level));
		min_wl_result = find_mc_min_wl(transaction_a, cache_a, witnessed_level, bp_block_hash, block_from_a);

		//check min_wl = level - 2 * majority + 2
		uint64_t wl_diff(witness_param_a.majority_of_witnesses - 1);
		uint64_t min_wl_check(level > 2 * wl_diff ? level - 2 * wl_diff : 0);
		assert_x(min_wl_result->min_wl == min_wl_check);

		if (min_wl_result->min_wl < earlier_block_state->level)
			return false;
	}

	std::unordered_set<mcp::block_hash> searched_hashs;
	std::queue<mcp::block_hash> to_search_hashs;

	for (mcp::block_hash const & pblock_hash : parents_a)
	{
		searched_hashs.insert(pblock_hash);
		to_search_hashs.push(pblock_hash);
	}

	std::unordered_set<mcp::block_hash> mc_hashs;
	{
		//mcp::stopwatch_guard sw("check_stable2");
		mcp::block_hash next_mc_hash(bp_block_hash);
		while (true)
		{
			mc_hashs.insert(next_mc_hash);

			if (next_mc_hash == earlier_hash)
				break;

			if (next_mc_hash == mcp::genesis::block_hash)
				return false;

			auto mc_block_state(cache_a->block_state_get(transaction_a, next_mc_hash));
			//best later block does not include earlier block
			if (mc_block_state->level <= earlier_block_state->level)
				return false;

			next_mc_hash = mc_block_state->best_parent;
		}
	}

	//int search_count(0);
	std::unordered_set<mcp::block_hash> handled_branch_hashs;
	while (!to_search_hashs.empty())
	{
		//search_count++;
		//mcp::stopwatch_guard sw("check_stable3");

		mcp::block_hash block_hash(std::move(to_search_hashs.front()));
		to_search_hashs.pop();

		auto block_state(cache_a->block_state_get(transaction_a, block_hash));
		assert_x(block_state);
		if (block_state->level < min_wl_result->min_wl)
			continue;

		std::shared_ptr<mcp::block> block;
		if(!mc_hashs.count(block_hash))
		{
			assert_x(block_state->latest_bp_included_mc_index);
			if (*block_state->latest_bp_included_mc_index < checked_stable_mci)
				continue;

			assert_x(block_state->earliest_bp_included_mc_index);
			if (*block_state->earliest_bp_included_mc_index >= earlier_mci)
				continue;

			block = cache_a->block_get(transaction_a, block_hash);
			assert_x(block);

			if (min_wl_result->witnesses.count(block->hashables->from)
				&& *block_state->bp_included_mc_index < earlier_mci
				&& *block_state->bp_included_mc_index >= checked_stable_mci)
			{
				//mcp::stopwatch_guard sw("check_stable3_1");				

				if (!handled_branch_hashs.count(block_hash))
				{
					size_t const unknown_branch_witness_count((witness_param_a.witness_count - witness_param_a.majority_of_witnesses) * 2);
					assert_x(witness_param_a.majority_of_witnesses > unknown_branch_witness_count);
					size_t const branch_witness_check_count(witness_param_a.majority_of_witnesses - unknown_branch_witness_count);
					assert_x(branch_witness_check_count > 0);
					if (branch_witness_check_count == 1)
						return false;

					// std::unordered_set<mcp::block_hash> branch_witnesses; changed by Daniel
					std::unordered_set<mcp::account> branch_witnesses;
					branch_witnesses.insert(block->hashables->from);

					//check block does not include earlier block along best parents
					mcp::block_hash branch_bp_block_hash(block_state->best_parent);

					while (true)
					{
						handled_branch_hashs.insert(branch_bp_block_hash);

						//mcp::stopwatch_guard sw("check_stable3_1_1");

						auto branch_bp_block_state(cache_a->block_state_get(transaction_a, branch_bp_block_hash));
						assert_x(branch_bp_block_state);

						if (branch_witnesses.size() < branch_witness_check_count)
						{
							if (branch_bp_block_state->level < min_wl_result->min_wl)
								break;

							auto branch_bp_block(cache_a->block_get(transaction_a, branch_bp_block_hash));
							if (min_wl_result->witnesses.count(branch_bp_block->hashables->from)
								&& !branch_witnesses.count(branch_bp_block->hashables->from))
							{
								branch_witnesses.insert(branch_bp_block->hashables->from);

								if (branch_witnesses.size() == branch_witness_check_count)
								{
									//if (search_count > 1000)
									//	LOG(m_log.debug) << "check stable return false, search count:" << search_count;
									return false;
								}
							}
						}

						if (branch_bp_block_state->is_on_main_chain)
							break;

						branch_bp_block_hash = branch_bp_block_state->best_parent;
					}
				}
			}
		}
		else
		{
			block = cache_a->block_get(transaction_a, block_hash);
			assert_x(block);
		}

		for (mcp::block_hash const & pblock_hash : block->parents())
		{
			auto r = searched_hashs.insert(pblock_hash);
			if (r.second)
				to_search_hashs.push(pblock_hash);
		}
	}

	//if (search_count > 1000)
	//	LOG(m_log.debug) << "check stable return true3, search count:" << search_count;
	return true;
}
