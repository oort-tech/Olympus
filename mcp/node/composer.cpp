#include <mcp/node/composer.hpp>
#include <mcp/core/genesis.hpp>
#include <mcp/common/stopwatch.hpp>

#include <unordered_set>

mcp::composer::composer(
	mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
	mcp::ledger& ledger_a, std::shared_ptr<mcp::TransactionQueue> tq
) :
	m_ledger(ledger_a),
	m_store(store_a),
	m_cache(cache_a),
	m_tq(tq)
{
}

mcp::composer::~composer()
{
}

std::shared_ptr<mcp::block> mcp::composer::compose_block(dev::Address const & from_a, dev::Secret const& s)
{
	mcp::stopwatch_guard sw("compose:compose_block");

	mcp::db::db_transaction transaction(m_store.create_transaction());
    //previous
	mcp::block_hash previous = get_latest_block(transaction, from_a);

    //pick parents and last summary
    std::vector<mcp::block_hash> parents;
	h256s links;
	mcp::block_hash last_summary_block;
	mcp::block_hash last_summary;
	mcp::block_hash last_stable_block;
    pick_parents_and_last_summary_and_wl_block(transaction, previous, from_a, parents, links, last_summary_block, last_summary, last_stable_block);
    
	size_t transaction_unstable_count(m_store.transaction_unstable_count(transaction));
	if (transaction_unstable_count == 0 && links.empty())//todo throw exception
		BOOST_THROW_EXCEPTION(BadComposeBlock()
			<< errinfo_comment("compose error:block no links"));

	uint64_t exec_timestamp(mcp::seconds_since_epoch());

    return std::make_shared<mcp::block>(from_a, previous, parents, links, 
		last_summary, last_summary_block, last_stable_block, exec_timestamp,s);
}

void mcp::composer::pick_parents_and_last_summary_and_wl_block(mcp::db::db_transaction &  transaction_a, mcp::block_hash const & previous_a, dev::Address const & from_a, std::vector<mcp::block_hash>& parents, h256s& links, mcp::block_hash & last_summary_block, mcp::block_hash & last_summary, mcp::block_hash & last_stable_block)
{
	mcp::stopwatch_guard sw("compose:pick_parents");

	auto snapshot = m_store.create_snapshot();
	mcp::db::forward_iterator best_dag_free_it(m_store.dag_free_begin(transaction_a, snapshot));
	mcp::free_key best_dag_free_key(best_dag_free_it.key());
	mcp::block_hash const & best_pblock_hash(best_dag_free_key.hash_asc);

	//last summary
	if (best_pblock_hash == mcp::genesis::block_hash)
		last_summary_block = mcp::genesis::block_hash;
	else
	{
		std::shared_ptr<mcp::block> bp_block(m_cache->block_get(transaction_a, best_pblock_hash));
		assert_x(bp_block);
		last_summary_block = bp_block->last_stable_block();
	}

	int count = 0;
	std::shared_ptr<mcp::block_state> last_summary_block_state;
	do
	{
		//make sure last summary block is stable
		bool last_summary_exist(!m_cache->block_summary_get(transaction_a, last_summary_block, last_summary));
		if (last_summary_exist)
		{
			do
			{
				//todo: why block stable is not stable when block_summary exists in cache?
				last_summary_block_state = m_cache->block_state_get(transaction_a, last_summary_block);
				if (last_summary_block_state && last_summary_block_state->is_stable)
					break;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			} while (true);

			break;
		}

		count++;
		if (count % 10 == 0)
			LOG(m_log.warning) << "composer: last summary not exists, check count:" << count;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (m_stopped)
		{
			BOOST_THROW_EXCEPTION(BadComposeBlock()
				<< errinfo_comment("compose error:composer stopped"));
		}
	} while (true);

	assert_x(last_summary_block_state->is_on_main_chain);
	assert_x(last_summary_block_state->main_chain_index);

	uint64_t const & last_summary_mci(*last_summary_block_state->main_chain_index);

	mcp::block_param const & b_param(mcp::param::block_param(last_summary_mci));

	{
		mcp::stopwatch_guard sw("compose:pick_parents1");
		std::set<mcp::block_hash> ordered_parents;
		ordered_parents.insert(best_pblock_hash);
		bool has_previous(!previous_a.is_zero());
		if (has_previous && previous_a != best_pblock_hash)
		{
			ordered_parents.insert(previous_a);
		}

		//rand dag free
		mcp::db::forward_iterator dag_free_it;
		mcp::db::backward_iterator last_dag_free_it(m_store.dag_free_rbegin(transaction_a, snapshot));
		mcp::free_key last_dag_free_key(last_dag_free_it.key());

		uint64_t rand_witnessed_level = mcp::random_pool.GenerateWord32(last_dag_free_key.witnessed_level_desc, best_dag_free_key.witnessed_level_desc);

		uint64_t min_level = best_dag_free_key.level_desc < last_dag_free_key.level_desc ? best_dag_free_key.level_desc : last_dag_free_key.level_desc;
		uint64_t max_level = best_dag_free_key.level_desc > last_dag_free_key.level_desc ? best_dag_free_key.level_desc : last_dag_free_key.level_desc;
		uint64_t rand_level = mcp::random_pool.GenerateWord32(min_level, max_level);

		mcp::block_hash rand_hash;
		mcp::random_pool.GenerateBlock(rand_hash.bytes.data(), rand_hash.bytes.size());

		mcp::free_key rand_key(rand_witnessed_level, rand_level, rand_hash);
		mcp::db::forward_iterator rand_it = m_store.dag_free_begin(transaction_a, rand_key, snapshot);
		if (rand_it.valid())
			dag_free_it = std::move(rand_it);


		std::unordered_set<mcp::block_hash> ordered_tmp;
		while (ordered_parents.size() < b_param.max_parent_size)
		{
			if (!dag_free_it.valid())
			{
				dag_free_it = m_store.dag_free_begin(transaction_a, snapshot);
				assert_x_msg(dag_free_it.valid(), "dag free is null");
			}

			mcp::free_key key(dag_free_it.key());
			mcp::block_hash const & free_hash(key.hash_asc);

			auto ar = ordered_tmp.insert(free_hash);
			if (!ar.second)
				break;

			if (free_hash != best_pblock_hash && free_hash != previous_a)
			{
				auto r = ordered_parents.insert(free_hash);
				//exists
				if (!r.second)
					break;
			}

			++dag_free_it;
		}

		for (mcp::block_hash const & parent : ordered_parents)
			parents.push_back(std::move(parent));

		assert_x(parents.size() <= b_param.max_parent_size);
	}

	{
		mcp::stopwatch_guard sw("compose:pick_parents2");

		links = m_tq->topTransactions(b_param.max_link_size);
	}

	//release snapshot
	snapshot.reset();

	{
		mcp::stopwatch_guard sw("compose:pick_parents3");

		assert_x(best_pblock_hash == m_ledger.determine_best_parent(transaction_a, m_cache, parents));

		mcp::witness_param const & w_param(mcp::param::witness_param(last_summary_mci));

		//check majority different of witnesses
		bool is_diff_majority(m_ledger.check_majority_witness(transaction_a, m_cache, best_pblock_hash, from_a, w_param));
		if (!is_diff_majority)
		{
			BOOST_THROW_EXCEPTION(BadComposeBlock()
				<< errinfo_comment("compose error: check majority different of witnesses failed"));
			return;
		}

		//last stable block
		mcp::block_hash max_p_last_stable_block_hash = mcp::genesis::block_hash;
		uint64_t max_p_last_stable_block_mci(0);
		for (mcp::block_hash const & pblock_hash : parents)
		{
			if (pblock_hash == mcp::genesis::block_hash)
				continue;

			std::shared_ptr<mcp::block> pblock(m_cache->block_get(transaction_a, pblock_hash));
			assert_x(pblock);

			std::shared_ptr<mcp::block_state> p_last_stable_block_state(m_cache->block_state_get(transaction_a, pblock->last_stable_block()));
			assert_x(p_last_stable_block_state);
			assert_x(p_last_stable_block_state->main_chain_index);

			if (max_p_last_stable_block_mci < *p_last_stable_block_state->main_chain_index)
			{
				max_p_last_stable_block_mci = *p_last_stable_block_state->main_chain_index;
				max_p_last_stable_block_hash = pblock->last_stable_block();
			}
		}

		last_stable_block = max_p_last_stable_block_hash;
		uint64_t next_check_mci = max_p_last_stable_block_mci + 1;
		mcp::block_hash next_mc_hash;
		bool next_mc_exists(!m_store.main_chain_get(transaction_a, next_check_mci, next_mc_hash));
		if (next_mc_exists)
		{
			bool is_next_mc_stable(m_ledger.check_stable(transaction_a, m_cache, next_mc_hash, best_pblock_hash, parents, from_a, last_stable_block, w_param));
			if (is_next_mc_stable)
			{
				last_stable_block = next_mc_hash;

				uint64_t static const max_skip_step = 1024;
				uint64_t skip_step = max_skip_step;
				next_check_mci += skip_step;
				while (true)
				{
					mcp::block_hash skip_mc_hash;
					bool skip_mc_exists(!m_store.main_chain_get(transaction_a, next_check_mci, skip_mc_hash));
					if (!skip_mc_exists)
					{
						skip_step /= 2;
						if (skip_step == 0)
							break;

						next_check_mci -= skip_step;
						continue;
					}

					bool is_stable = m_ledger.check_stable(transaction_a, m_cache, skip_mc_hash, best_pblock_hash, parents, from_a, last_stable_block, w_param);
					if (is_stable)
						last_stable_block = skip_mc_hash;

					if (skip_step == 1)
						break;

					if (skip_step == max_skip_step
						&& is_stable)
					{
						next_check_mci += skip_step;
					}
					else
					{
						skip_step /= 2;
						if (is_stable)
							next_check_mci += skip_step;
						else
							next_check_mci -= skip_step;
					}
				}
			}
		}

		std::shared_ptr<mcp::block_state> last_stable_state(m_cache->block_state_get(transaction_a, last_stable_block));
		assert_x(last_stable_state);
		assert_x(last_stable_state->is_on_main_chain);
		uint64_t & last_stable_block_mci = *last_stable_state->main_chain_index;
	}
}

mcp::block_hash mcp::composer::get_latest_block(mcp::db::db_transaction &  transaction_a, dev::Address const & account_a)
{
	mcp::block_hash previous;

	mcp::dag_account_info info;
	bool account_error(m_store.dag_account_get(transaction_a, account_a, info));
	previous = account_error ? 0 : info.latest_stable_block;

	mcp::block_hash latest_block_hash(previous);
	mcp::block_hash root(previous.is_zero() ? ((dev::Address::Arith)account_a).convert_to<unsigned>() : previous);

	//search in chain
	while (true)
	{
		mcp::block_hash successor_hash;
		bool exists(!m_cache->successor_get(transaction_a, root, successor_hash));
		if (!exists)
			break;

		latest_block_hash = std::move(successor_hash);
		root = latest_block_hash;
	}

	return latest_block_hash;
}


