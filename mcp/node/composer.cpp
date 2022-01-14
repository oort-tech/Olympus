#include <mcp/node/composer.hpp>
#include <mcp/core/genesis.hpp>
#include <mcp/common/stopwatch.hpp>

#include <unordered_set>

mcp::compose_result::compose_result(mcp::compose_result_codes const &code_a, std::shared_ptr<mcp::joint_message> message_a) : code(code_a),
                                                                                                                              joint(message_a)
{
}

mcp::composer::composer(
	mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
	mcp::ledger& ledger_a, std::shared_ptr<mcp::chain> chain_a,
	uint256_t const& gas_price_a
) :
	m_ledger(ledger_a),
	m_store(store_a),
	m_cache(cache_a),
	m_chain(chain_a),
	m_gas_price(gas_price_a)
{
}

mcp::composer::~composer()
{
}

mcp::compose_result mcp::composer::compose_joint(mcp::db::db_transaction & transaction_a, mcp::block_type const & type_a, 
													boost::optional<mcp::block_hash> const & previous_a, mcp::account const &from_a, mcp::account const &to_a,
													mcp::amount const &amount_a, uint256_t const & gas_a, uint256_t const & gas_price_a, std::vector<uint8_t> const &data_a,
													mcp::raw_key const &prv_a, mcp::public_key const &pub_a, bool generate_work_a)
{
    std::shared_ptr<mcp::block> block;
    mcp::compose_result_codes code(compose_block(transaction_a, type_a, previous_a, from_a, to_a, amount_a, gas_a, gas_price_a, data_a, block));
    if (code != mcp::compose_result_codes::ok)
        return mcp::compose_result(code, nullptr);

    mcp::signature sig(mcp::sign_message(prv_a, pub_a, block->hash()));

    return sign_and_compose_joint( block, sig, generate_work_a);
}

mcp::compose_result_codes mcp::composer::compose_block(mcp::db::db_transaction & transaction_a, mcp::block_type const &type_a, 
														boost::optional<mcp::block_hash> const & previous_a, mcp::account const &from_a, mcp::account const &to_a,
														mcp::amount const &amount_a, uint256_t const & gas_a, uint256_t const & gas_price_a,
														std::vector<uint8_t> const &data_a, std::shared_ptr<mcp::block> &block_a)
{
	mcp::stopwatch_guard sw("compose:compose_block");

    if (data_a.size() > mcp::max_data_size)
        return mcp::compose_result_codes::data_size_too_large;

    //previous
	mcp::block_hash previous = previous_a ? *previous_a : get_latest_block(transaction_a, type_a, from_a);

	mcp::uint64_union work_l(0);

    //pick parents and last summary
    std::vector<mcp::block_hash> parents;
	std::shared_ptr<std::list<mcp::block_hash>> links(std::make_shared<std::list<mcp::block_hash>>());
    mcp::block_hash last_summary_block;
    mcp::block_hash last_summary;
	mcp::block_hash last_stable_block;
    mcp::error_message err_msg;
    pick_parents_and_last_summary_and_wl_block(err_msg, transaction_a, type_a, previous, from_a, parents, links, last_summary_block, last_summary, last_stable_block, gas_a, gas_price_a);
    
	if (type_a == mcp::block_type::dag)
	{
		size_t light_unstable_count(m_store.light_unstable_count(transaction_a));
		if (light_unstable_count == 0 && links->empty())
			return mcp::compose_result_codes::dag_no_links;
	}
	
	if (err_msg.error)
    {
        LOG(m_log.error) << err_msg.message;
        return mcp::compose_result_codes::error;
    }

	uint64_t exec_timestamp(type_a == mcp::block_type::light ? 0 : mcp::seconds_since_epoch());
	//check balance
	mcp::chain_state c_state(transaction_a, 0, m_store, m_chain, m_cache);
    mcp::amount balance(c_state.balance(from_a));
	mcp::amount fee = gas_a * gas_price_a;
    if (balance < amount_a + fee)
        return mcp::compose_result_codes::insufficient_balance;

	//sichaoy: real gasprice and gas
    block_a = std::make_shared<mcp::block>(type_a, from_a, to_a, amount_a, previous, parents, links, 
		last_summary, last_summary_block, last_stable_block, gas_a, gas_price_a, mcp::block::data_hash(data_a), data_a, exec_timestamp, work_l);
    return mcp::compose_result_codes::ok;
}

mcp::compose_result mcp::composer::sign_and_compose_joint(std::shared_ptr<mcp::block> block_a, mcp::signature const &signature_a, bool generate_work_a)
{
	mcp::stopwatch_guard sw("compose:sign_and_compose_joint");

    //sign
    block_a->set_signature(signature_a);

    std::shared_ptr<mcp::joint_message> message(new mcp::joint_message(block_a));

    return mcp::compose_result(mcp::compose_result_codes::ok, message);
}

void mcp::composer::pick_parents_and_last_summary_and_wl_block(mcp::error_message & err_msg, mcp::db::db_transaction & transaction_a,
	mcp::block_type const & block_type_a, mcp::block_hash const & previous_a, mcp::account const & from_a,
	std::vector<mcp::block_hash> & parents, std::shared_ptr<std::list<mcp::block_hash>> links, mcp::block_hash & last_summary_block, mcp::summary_hash & last_summary,
	mcp::block_hash & last_stable_block, uint256_t const& gas_a, uint256_t const& gas_price_a)
{
	mcp::stopwatch_guard sw("compose:pick_parents");

	switch (block_type_a)
	{
	case mcp::block_type::dag:
	{
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
			last_summary_block = bp_block->hashables->last_stable_block;
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
				err_msg.error = true;
				err_msg.message = "compose error:composer stopped";
				return;
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

			std::shared_ptr<std::list<mcp::block_hash>> random_links = random_get_links(transaction_a, b_param.max_link_size);
			for (mcp::block_hash const & link : *random_links)
				links->push_back(std::move(link));

			assert_x(links->size() <= b_param.max_link_size);
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
				err_msg.error = true;
				err_msg.message = "compose error: check majority different of witnesses failed";
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

				std::shared_ptr<mcp::block_state> p_last_stable_block_state(m_cache->block_state_get(transaction_a, pblock->hashables->last_stable_block));
				assert_x(p_last_stable_block_state);
				assert_x(p_last_stable_block_state->main_chain_index);

				if (max_p_last_stable_block_mci < *p_last_stable_block_state->main_chain_index)
				{
					max_p_last_stable_block_mci = *p_last_stable_block_state->main_chain_index;
					max_p_last_stable_block_hash = pblock->hashables->last_stable_block;
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

		break;
	}
	case mcp::block_type::light:
	{
		last_summary_block = 0;
		last_summary = 0;
		last_stable_block = 0;
		break;
	}
	default:
	{
		assert_x_msg(false, "invalid block type");
		break;
	}
	}
}

mcp::block_hash mcp::composer::get_latest_block(mcp::db::db_transaction &  transaction_a, mcp::block_type const & type_a, mcp::account const & account_a)
{
	mcp::block_hash previous;
	switch (type_a)
	{
	case mcp::block_type::dag:
	{
		mcp::dag_account_info info;
		bool account_error(m_store.dag_account_get(transaction_a, account_a, info));
		previous = account_error ? 0 : info.latest_stable_block;

		mcp::block_hash latest_block_hash(previous);
		mcp::block_hash root(previous.is_zero() ? account_a.number() : previous);

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

		break;
	}
	case mcp::block_type::light:
	{
		std::shared_ptr<mcp::unlink_info> ulk = m_store.unlink_info_get(transaction_a, account_a);
		if (nullptr != ulk)
		{
			assert_x_msg(!ulk->latest_unlink.is_zero(), "get_latest_block unlink info error.");
			return ulk->latest_unlink;
		}
		else
		{
			std::shared_ptr<mcp::account_info> info(m_cache->account_get(transaction_a, account_a));
			previous = info ? info->latest_linked : 0;
			return previous;
		}
		break;
	}
	default:
		assert_x_msg(false, "Invalid block type");
	}
}

std::shared_ptr<std::list<mcp::block_hash>> mcp::composer::random_get_links(mcp::db::db_transaction &  transaction_a, size_t const & limit_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	std::list<mcp::block_hash> ordered_links;

	if (nullptr == snapshot_a)
		snapshot_a = m_store.create_snapshot();

	auto it = m_store.unlink_info_begin(transaction_a, snapshot_a);
	if (it.valid())
	{
		mcp::account rand_account;
		mcp::random_pool.GenerateBlock(rand_account.bytes.data(), rand_account.bytes.size());
		mcp::db::forward_iterator rand_it = m_store.unlink_info_begin(transaction_a, rand_account, snapshot_a);
		if (rand_it.valid())
		{
			it = std::move(rand_it);
		}

		uint64_t links_block_size = 0;
		mcp::account start_account(0);
		while (ordered_links.size() < limit_a && links_block_size < mcp::max_link_block_size)
		{
			if (!it.valid())
				it = m_store.unlink_info_begin(transaction_a, snapshot_a);

			mcp::account current_account(mcp::slice_to_uint256(it.key()).number());
			mcp::unlink_info unlink(it.value());

			if (start_account == current_account)  // eq start ,break
				break;
			if (start_account.is_zero())
				start_account = current_account;

			mcp::block_hash cur_hash(unlink.earliest_unlink);
			mcp::block_hash last_unlink_hash(0);

			if (!cur_hash.is_zero())
			{
				uint64_t account_blocks = 0;
				while (true)
				{
					std::shared_ptr<mcp::unlink_block> cur_block = m_store.unlink_block_get(transaction_a, cur_hash, snapshot_a);
					assert_x_msg(nullptr != cur_block, "unlink_block have no current hash, account:" + current_account.to_account()
						+ " ,hash: " + cur_hash.to_string());
					account_blocks++;

					if (cur_block->block->hashables->gas_price >= m_gas_price
						&& cur_block->block->hashables->light_version > 0) //not link legacy light block
					{
						last_unlink_hash = cur_hash;

						mcp::block_hash successor_hash(0);
						auto successor_exist(!m_store.successor_get(transaction_a, cur_hash, successor_hash, snapshot_a));
						if (successor_exist)
						{
							cur_hash = successor_hash;
						}
						else // no successor,cur_hash is last
						{
							if (!last_unlink_hash.is_zero())
							{
								links_block_size += account_blocks;
								ordered_links.push_back(last_unlink_hash);
							}
							break;
						}
					}
					else
					{
						if (!last_unlink_hash.is_zero())
						{
							links_block_size += account_blocks;
							ordered_links.push_back(last_unlink_hash);
						}
						break;
					}
				}
			}
			++it;
		}
	}

	ordered_links.sort();
	return std::make_shared<std::list<mcp::block_hash>>(ordered_links);
}
