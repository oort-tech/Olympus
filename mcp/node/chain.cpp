#include "chain.hpp"
#include <mcp/core/genesis.hpp>
#include <mcp/common/stopwatch.hpp>
#include <mcp/common/Exceptions.h>
#include <mcp/node/debug.hpp>
#include <mcp/node/evm/Executive.hpp>

#include <queue>

mcp::chain::chain(mcp::block_store& store_a, mcp::ledger& ledger_a) :
	m_store(store_a),
	m_ledger(ledger_a),
	m_stopped(false)
{
}

mcp::chain::~chain()
{
}

void mcp::chain::init(bool & error_a, mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	mcp::db::db_transaction & transaction(timeout_tx_a.get_transaction());

	bool first_time;
	try
	{
		first_time = mcp::genesis::try_initialize(transaction, m_store);
	}
	catch (const std::exception & e)
	{
		LOG(m_log.error) << boost::str(boost::format("Init genesis error: %1%") % e.what());
		transaction.rollback();
		error_a = true;
		return;
	}

	LOG(m_log.info) << "Genesis block:" << mcp::genesis::block_hash.to_string();

	// Init precompiled contract account
	if (first_time)
	{
		AccountMap precompiled_accounts;
		for (unsigned i = 1; i <= 8; ++i)
		{
			mcp::account acc(i);
			precompiled_accounts[acc] = std::make_shared<mcp::account_state>(acc, 0, 0, 0, 0);
		}

		mcp::overlay_db db(transaction, m_store);
		mcp::commit(transaction, precompiled_accounts, &db, cache_a, m_store, 0);
	}

	// Setup default precompiled contracts as equal to genesis of Frontier.
	m_precompiled.insert(std::make_pair(mcp::account(1), dev::eth::PrecompiledContract(3000, 0, dev::eth::PrecompiledRegistrar::executor("ecrecover"))));
	m_precompiled.insert(std::make_pair(mcp::account(2), dev::eth::PrecompiledContract(60, 12, dev::eth::PrecompiledRegistrar::executor("sha256"))));
	m_precompiled.insert(std::make_pair(mcp::account(3), dev::eth::PrecompiledContract(600, 120, dev::eth::PrecompiledRegistrar::executor("ripemd160"))));
	m_precompiled.insert(std::make_pair(mcp::account(4), dev::eth::PrecompiledContract(15, 3, dev::eth::PrecompiledRegistrar::executor("identity"))));
	m_precompiled.insert(std::make_pair(mcp::account(5), dev::eth::PrecompiledContract(dev::eth::PrecompiledRegistrar::pricer("modexp"), dev::eth::PrecompiledRegistrar::executor("modexp"))));
	m_precompiled.insert(std::make_pair(mcp::account(6), dev::eth::PrecompiledContract(500, 0, dev::eth::PrecompiledRegistrar::executor("alt_bn128_G1_add"))));
	m_precompiled.insert(std::make_pair(mcp::account(7), dev::eth::PrecompiledContract(40000, 0, dev::eth::PrecompiledRegistrar::executor("alt_bn128_G1_mul"))));
	m_precompiled.insert(std::make_pair(mcp::account(8), dev::eth::PrecompiledContract(dev::eth::PrecompiledRegistrar::pricer("alt_bn128_pairing_product"), dev::eth::PrecompiledRegistrar::executor("alt_bn128_pairing_product"))));

	//get init data
	m_last_mci_internal = m_store.last_mci_get(transaction);
	m_last_stable_mci_internal = m_store.last_stable_mci_get(transaction);

	m_min_retrievable_mci_internal = 0;
	if (m_last_stable_mci_internal > 0)
	{
		mcp::block_hash mc_stable_hash;
		bool mc_stable_hash_error(m_store.main_chain_get(transaction, m_last_stable_mci_internal, mc_stable_hash));
		assert_x(!mc_stable_hash_error);
		auto mc_stable_block = m_store.block_get(transaction, mc_stable_hash);
		assert_x(mc_stable_block);
		std::shared_ptr<mcp::block_state> min_retrievable_state(m_store.block_state_get(transaction, mc_stable_block->hashables->last_stable_block));
		assert_x(min_retrievable_state);
		assert_x(min_retrievable_state->main_chain_index);
		m_min_retrievable_mci_internal = *min_retrievable_state->main_chain_index;
	}

	m_last_stable_index_internal = m_store.last_stable_index_get(transaction);

	m_advance_info = m_store.advance_info_get(transaction);

	update_cache();
}

void mcp::chain::stop()
{
	m_stopped = true;
}

void mcp::chain::save_light_to_pool(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block_processor_item> item_a)
{
	std::shared_ptr<mcp::block> block(item_a->joint.block);
	mcp::block_hash block_hash(item_a->block_hash);
	bool is_successor(true);
	bool successor_unlink(false);

	mcp::uint64_union now(static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now()).time_since_epoch()).count()));

	mcp::block_hash successor_hash(0);
	std::shared_ptr<mcp::unlink_block> successor_block = nullptr;
	if (!cache_a->successor_get(timeout_tx_a.get_transaction(), block->root(), successor_hash))
	{
		is_successor = false;
		successor_block = cache_a->unlink_block_get(timeout_tx_a.get_transaction(), successor_hash);
		if (nullptr != successor_block)
			successor_unlink = true;
	}

	std::shared_ptr<mcp::unlink_info> ulk = m_store.unlink_info_get(timeout_tx_a.get_transaction(), block->hashables->from);
	
	if (is_successor)
	{
		std::shared_ptr<mcp::account_info> info(cache_a->account_get(timeout_tx_a.get_transaction(), block->hashables->from));

		if (nullptr == ulk)
		{
			if (!info || block->previous() == info->latest_linked)
			{
				ulk = std::make_shared<mcp::unlink_info>();
				ulk->earliest_unlink = block_hash;
				ulk->latest_unlink = block_hash;
			}
		}
		else if (block->previous() == ulk->latest_unlink)
		{
			//update latest_unlink
			ulk->latest_unlink = block_hash;
		}
		if (nullptr != ulk)
			m_store.unlink_info_put(timeout_tx_a.get_transaction(), block->hashables->from, *ulk);
		cache_a->successor_put(timeout_tx_a.get_transaction(), block->root(), block_hash);
	}
	else if (successor_unlink)
	{
		//replace successor with higher gas_price
		if (block->hashables->gas_price > successor_block->block->hashables->gas_price && ulk)
		{
			bool to_be_successor(false);
			mcp::block_hash unlink_hash(ulk->latest_unlink);
			while (true)
			{
				std::shared_ptr<mcp::unlink_block> light_block = cache_a->unlink_block_get(timeout_tx_a.get_transaction(), unlink_hash);
				assert_x(nullptr != light_block)
				if (light_block->block->root() == block->root())
				{
					to_be_successor = true;
					break;
				}

				if (unlink_hash == ulk->earliest_unlink)
					break;
				unlink_hash = light_block->block->previous();
			}

			if (to_be_successor)
			{
				cache_a->successor_put(timeout_tx_a.get_transaction(), block->root(), block_hash);
				//update earliest_unlink
				if (unlink_hash == ulk->earliest_unlink)
				{
					ulk->earliest_unlink = block_hash;
				}

				//update latest_unlink
				ulk->latest_unlink = block_hash;
				m_store.unlink_info_put(timeout_tx_a.get_transaction(), block->hashables->from, *ulk);
			}
		}
	}

	//head unlink or next unlink
	if (block->previous().is_zero())
	{
		mcp::head_unlink head(now, block_hash);
		m_store.head_unlink_put(timeout_tx_a.get_transaction(), head);
	}
	else
	{
		std::shared_ptr<mcp::unlink_block> previous_block = cache_a->unlink_block_get(timeout_tx_a.get_transaction(), block->previous());
		if (nullptr == previous_block)//is linked
		{
			mcp::head_unlink head(now, block_hash);
			m_store.head_unlink_put(timeout_tx_a.get_transaction(), head);
		}
	}

	mcp::next_unlink next(block->root(), block_hash);
	m_store.next_unlink_put(timeout_tx_a.get_transaction(), next);
	//LOG(m_log.debug) << "save_light_to_pool next create:" << block->previous().to_string() << " ,block_hash:" << block_hash.to_string();
	
	std::shared_ptr<mcp::unlink_block> u_block(std::make_shared<mcp::unlink_block>(now, block));
	cache_a->unlink_block_put(timeout_tx_a.get_transaction(), block_hash, u_block);

	timeout_tx_a.commit_if_timeout();
}

void mcp::chain::save_dag_block(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a)
{
	if (m_stopped)
		return;

	{
		//mcp::stopwatch_guard sw("process:save_block:not commit");

		mcp::db::db_transaction & transaction(timeout_tx_a.get_transaction());
		try
		{
			{
				//mcp::stopwatch_guard sw("save_block:write_block");

				write_dag_block(transaction, cache_a, block_a);
			}
			assert_x(block_a->hashables->type == mcp::block_type::dag);

			mcp::block_hash best_free_block_hash;
			{
				//mcp::stopwatch_guard sw("save_block:best_free");

				//search best free block by witnessed_level desc, level asc, block hash asc
				mcp::db::forward_iterator free_iter(m_store.dag_free_begin(transaction));
				assert_x(free_iter.valid());
				mcp::free_key free_key(free_iter.key());
				best_free_block_hash = free_key.hash_asc;

				if (best_free_block_hash == mcp::genesis::block_hash) //genesis block
					return;
			}

			bool is_mci_retreat;
			uint64_t retreat_mci;
			uint64_t retreat_level;
			std::list<mcp::block_hash> new_mc_block_hashs;
			{
				//mcp::stopwatch_guard sw("save_block:find_main_chain_changes");

				find_main_chain_changes(transaction, cache_a, block_a, best_free_block_hash, is_mci_retreat, retreat_mci, retreat_level, new_mc_block_hashs);
			}

			{
				//mcp::stopwatch_guard sw("save_block:update_mci");

				update_mci(transaction, cache_a, block_a, retreat_mci, new_mc_block_hashs);
			}

			{
				//mcp::stopwatch_guard sw("save_block:update_latest_included_mci");

				update_latest_included_mci(transaction, cache_a, block_a, is_mci_retreat, retreat_mci, retreat_level);
			}

			mcp::block_hash b_hash(block_a->hash());
			std::shared_ptr<mcp::block_state> last_stable_block_state(cache_a->block_state_get(transaction, block_a->hashables->last_stable_block));
			assert_x(last_stable_block_state->is_on_main_chain);
			assert_x(last_stable_block_state->main_chain_index);
			uint64_t last_stable_block_mci = *last_stable_block_state->main_chain_index;
			if (m_last_stable_mci_internal < last_stable_block_mci)
			{
				m_advance_info = mcp::advance_info(last_stable_block_mci, b_hash);
				m_store.advance_info_put(transaction, m_advance_info);
			}

			m_new_blocks.push(block_a);

			timeout_tx_a.commit_if_timeout();
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain save block error: " << e.what();
			timeout_tx_a.rollback();
			throw;
		}
	}
}

void mcp::chain::save_light_block(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::unlink_block> block_a)
{
	if (m_stopped)
		return;

	{
		//mcp::stopwatch_guard sw("process:save_block:not commit");

		mcp::db::db_transaction & transaction(timeout_tx_a.get_transaction());
		try
		{
			{
				//mcp::stopwatch_guard sw("save_block:write_block");

				write_light_block(transaction, cache_a, block_a);
			}

			m_new_blocks.push(block_a->block);

			timeout_tx_a.commit_if_timeout();
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain save block error: " << e.what();
			timeout_tx_a.rollback();
			throw;
		}
	}
}

void mcp::chain::try_advance(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	while (!m_stopped && m_advance_info.mci.number() > m_last_stable_mci_internal)
	{
		m_last_stable_mci_internal++;
		advance_stable_mci(timeout_tx_a, cache_a, m_last_stable_mci_internal, m_advance_info.witness_block);

		//update last stable mci
		mcp::db::db_transaction & transaction(timeout_tx_a.get_transaction());
		try
		{
			m_store.last_stable_mci_put(transaction, m_last_stable_mci_internal);
			mcp::block_hash mc_stable_hash;
			bool mc_stable_hash_error(m_store.main_chain_get(transaction, m_last_stable_mci_internal, mc_stable_hash));
			assert_x(!mc_stable_hash_error);
			auto mc_stable_block = cache_a->block_get(transaction, mc_stable_hash);
			assert_x(mc_stable_block);
			auto min_retrievable_state = cache_a->block_state_get(transaction, mc_stable_block->hashables->last_stable_block);
			assert_x(min_retrievable_state);
			assert_x(min_retrievable_state->main_chain_index);
			m_min_retrievable_mci_internal = *min_retrievable_state->main_chain_index;

			m_stable_mcis.push(m_last_stable_mci_internal);

			timeout_tx_a.commit_if_timeout();
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain update last stable mci error: " << e.what();
			timeout_tx_a.rollback();
			throw;
		}

	}
}


void mcp::chain::write_dag_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a)
{
	mcp::block_hash const &block_hash = block_a->hash();

	//save block, need put first
	cache_a->block_put(transaction_a, block_hash, block_a);

	mcp::block_hash const & previous(block_a->previous());
	mcp::block_hash const & root(block_a->root());

	assert_x(block_a->hashables->type == mcp::block_type::dag);
	uint64_t level;
	
	mcp::block_hash successor;
	bool successor_exists(!cache_a->successor_get(transaction_a, root, successor));
	if (!successor_exists)
		cache_a->successor_put(transaction_a, root, block_hash);

	for (mcp::block_hash const & pblock_hash : block_a->parents())
	{
		std::shared_ptr<mcp::block_state> pblock_state(cache_a->block_state_get(transaction_a, pblock_hash));
		assert_x(pblock_state);

		if (pblock_state->is_free)
		{
			//make a copy for not changing value in cache
			std::shared_ptr<mcp::block_state> pblock_state_copy(std::make_shared<mcp::block_state>(*pblock_state));
			pblock_state_copy->is_free = false;
			cache_a->block_state_put(transaction_a, pblock_hash, pblock_state_copy);

			//remove parent block from dag free
			mcp::free_key f_key(pblock_state->witnessed_level, pblock_state->level, pblock_hash);
			m_store.dag_free_del(transaction_a, f_key);
		}

		{
			//save child
			m_store.block_child_put(transaction_a, mcp::block_child_key(pblock_hash, block_hash));
		}
	}

	std::shared_ptr<mcp::block_state> last_summary_block_state(cache_a->block_state_get(transaction_a, block_a->hashables->last_summary_block));
	assert_x(last_summary_block_state
		&& last_summary_block_state->is_stable
		&& last_summary_block_state->is_on_main_chain
		&& last_summary_block_state->main_chain_index);
	uint64_t const & last_summary_mci(*last_summary_block_state->main_chain_index);
	mcp::witness_param const & w_param(mcp::param::witness_param(last_summary_mci));

	//best parent
	mcp::block_hash best_pblock_hash(m_ledger.determine_best_parent(transaction_a, cache_a, block_a->parents()));
	//level
	level = m_ledger.calc_level(transaction_a, cache_a, best_pblock_hash);
	//witnessed level
	uint64_t witnessed_level(m_ledger.calc_witnessed_level(w_param, level));

	std::shared_ptr<mcp::block_state> state(std::make_shared<mcp::block_state>());
	state->block_type = block_a->hashables->type;
	state->status = mcp::block_status::unknown;
	state->is_free = true;
	state->best_parent = best_pblock_hash;
	state->level = level;
	state->witnessed_level = witnessed_level;
	cache_a->block_state_put(transaction_a, block_hash, state);

	m_store.dag_free_put(transaction_a, mcp::free_key(witnessed_level, level, block_hash));
}

void mcp::chain::write_light_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::unlink_block> block_a)
{
	mcp::block_hash const &block_hash = block_a->block->hash();

	if (cache_a->block_exists(transaction_a, block_hash))
	{
		assert_x_msg(false, "block exist do not added count,hash:" + block_hash.to_string());
	}

	//save block, need put first
	cache_a->block_put(transaction_a, block_hash, block_a->block);

	mcp::block_hash const & previous(block_a->block->previous());
	mcp::block_hash const & root(block_a->block->root());

	assert_x(block_a->block->hashables->type == mcp::block_type::light);

	uint64_t level;
	std::shared_ptr<mcp::block_state> previous_state;
	if (!previous.is_zero())
		previous_state = cache_a->block_state_get(transaction_a, previous);

	if (previous.is_zero())
		level = 0;
	else
	{
		assert_x_msg(previous_state,"previous hash:" + previous.to_string() + " ,block_hash:" + block_hash.to_string());
		level = previous_state->level + 1;
	}

	std::shared_ptr<mcp::block_state> state(std::make_shared<mcp::block_state>());
	state->block_type = block_a->block->hashables->type;
	state->status = mcp::block_status::unknown;
	state->level = level;
	cache_a->block_state_put(transaction_a, block_hash, state);

	m_store.light_unstable_count_add(transaction_a);

	//update unlink info
	std::shared_ptr<mcp::unlink_info> unlink = m_store.unlink_info_get(transaction_a, block_a->block->hashables->from);
	if (nullptr != unlink && block_hash == unlink->earliest_unlink)
	{
		if (block_hash != unlink->latest_unlink)
		{
			mcp::block_hash successor_hash;
			bool successor_exists(!cache_a->successor_get(transaction_a, block_hash, successor_hash));
			assert_x(successor_exists);
			unlink->earliest_unlink = successor_hash;
			m_store.unlink_info_put(transaction_a, block_a->block->hashables->from, *unlink);
		}
		else
		{
			m_store.unlink_info_del(transaction_a, block_a->block->hashables->from);
		}

		std::shared_ptr<mcp::account_info> info(cache_a->account_get(transaction_a, block_a->block->hashables->from));
		std::shared_ptr<mcp::account_info> info_copy(info ? std::make_shared<mcp::account_info>(*info) : std::make_shared<mcp::account_info>());
		info_copy->latest_linked = block_hash;
		cache_a->account_put(transaction_a, block_a->block->hashables->from, info_copy);
	}

	//update head unlink
	m_store.head_unlink_del(transaction_a, mcp::head_unlink(block_a->time, block_hash));
	m_store.next_unlink_del(transaction_a, mcp::next_unlink(root, block_hash));

	std::list<mcp::next_unlink> li;
	m_store.next_unlink_get(transaction_a, block_hash, li);
	for (auto it = li.begin(); it != li.end(); it++)
	{
		std::shared_ptr<mcp::unlink_block> next_unlink_block = cache_a->unlink_block_get(transaction_a, it->next);
		m_store.head_unlink_put(transaction_a, mcp::head_unlink(next_unlink_block->time, it->next));
	}

	cache_a->unlink_block_del(transaction_a, block_hash);
}

void mcp::chain::find_main_chain_changes(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, mcp::block_hash const &best_free_block_hash,
	bool & is_mci_retreat, uint64_t &retreat_mci, uint64_t &retreat_level, std::list<mcp::block_hash> &new_mc_block_hashs)
{
	uint64_t old_last_mci(m_last_mci_internal);
	mcp::block_hash prev_mc_block_hash(best_free_block_hash);
	std::shared_ptr<mcp::block_state> prev_mc_block_state(cache_a->block_state_get(transaction_a, prev_mc_block_hash));
	assert_x(prev_mc_block_state);

	while (!prev_mc_block_state->is_on_main_chain)
	{
		new_mc_block_hashs.push_front(prev_mc_block_hash);

		//get previous best parent block
		prev_mc_block_hash = prev_mc_block_state->best_parent;
		prev_mc_block_state = cache_a->block_state_get(transaction_a, prev_mc_block_hash);
		assert_x(prev_mc_block_state);
	}
	assert_x(prev_mc_block_state->main_chain_index);

	retreat_mci = *prev_mc_block_state->main_chain_index;
	retreat_level = prev_mc_block_state->level;
	is_mci_retreat = retreat_mci < old_last_mci;

	//check stable mci not retreat
	if (retreat_mci < m_last_stable_mci_internal)
	{
		std::string msg(boost::str(boost::format("stable mci retreat, last added block: %1%, retreat mci: %2%, last stable mci: %3%") 
			% block_a->hash().to_string() % retreat_mci % m_last_stable_mci_internal));
		LOG(m_log.info) << msg;
		throw std::runtime_error(msg);
	}
}

void mcp::chain::update_mci(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, uint64_t const &retreat_mci, std::list<mcp::block_hash> const &new_mc_block_hashs)
{
#pragma region delete old main chain block whose main chain index larger than retreat_mci

	//mcp::stopwatch_guard sw("update_mci1");
	uint64_t old_mci(m_last_mci_internal);
	assert_x(old_mci >= retreat_mci);
	while (true)
	{
		if (old_mci == retreat_mci)
			break;

		mcp::block_hash old_last_mc_block_hash;
		bool mc_exists(!m_store.main_chain_get(transaction_a, old_mci, old_last_mc_block_hash));
		assert_x(mc_exists);

		std::shared_ptr<mcp::block_state> old_mci_block_state_in_cache(cache_a->block_state_get(transaction_a, old_last_mc_block_hash));
		assert_x(old_mci_block_state_in_cache);
		//make a copy for not changing value in cache
		std::shared_ptr<mcp::block_state> old_mci_block_state_copy(std::make_shared<mcp::block_state>(*old_mci_block_state_in_cache));
		assert_x(!old_mci_block_state_copy->is_stable);
		if (old_mci_block_state_copy->is_on_main_chain)
			old_mci_block_state_copy->is_on_main_chain = false;
		old_mci_block_state_copy->main_chain_index = boost::none;
		cache_a->block_state_put(transaction_a, old_last_mc_block_hash, old_mci_block_state_copy);

		//delete old main chian block
		m_store.main_chain_del(transaction_a, old_mci);

		old_mci--;
	}

	assert_x(old_mci == retreat_mci);

#pragma endregion

	//mcp::stopwatch_guard sw("update_mci3");

#pragma region update main chain index

	std::unordered_set<mcp::block_hash> updated_hashs;
	uint64_t new_mci(retreat_mci);
	for (auto iter(new_mc_block_hashs.begin()); iter != new_mc_block_hashs.end(); iter++)
	{
		new_mci++;
		mcp::block_hash new_mc_block_hash(*iter);

		std::shared_ptr<mcp::block_state> new_mc_block_state_in_cache(cache_a->block_state_get(transaction_a, new_mc_block_hash));
		assert_x(new_mc_block_state_in_cache);
		//make a copy for not changing value in cache
		std::shared_ptr<mcp::block_state> new_mc_block_state_copy(std::make_shared<mcp::block_state>(*new_mc_block_state_in_cache));
		new_mc_block_state_copy->is_on_main_chain = true;
		new_mc_block_state_copy->main_chain_index = new_mci;
		cache_a->block_state_put(transaction_a, new_mc_block_hash, new_mc_block_state_copy);

		m_store.main_chain_put(transaction_a, new_mci, new_mc_block_hash);
	}

#pragma endregion

	m_last_mci_internal = new_mci;
	m_store.last_mci_put(transaction_a, m_last_mci_internal);

	//LOG(m_log.debug) << "Retreat mci to " << retreat_mci << ", new mci is " << new_mci;
}

void mcp::chain::update_latest_included_mci(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, bool const &is_mci_retreat, uint64_t const &retreat_mci, uint64_t const &retreat_level)
{
	bool is_curr_block_set(false);
	mcp::block_hash block_hash(block_a->hash());

	std::map<uint64_t, std::unordered_set<mcp::block_hash>> to_update_hashs;
	if (is_mci_retreat)
	{
		{
			//mcp::stopwatch_guard sw2("update_latest_included_mci1");

			mcp::block_hash retreat_mci_block_hash;
			bool error(m_store.main_chain_get(transaction_a, retreat_mci, retreat_mci_block_hash));
			assert_x(!error);
			std::queue<mcp::block_hash> to_search_child_hashs;
			to_search_child_hashs.push(retreat_mci_block_hash);

			while (!to_search_child_hashs.empty())
			{
				mcp::block_hash p_hash(to_search_child_hashs.front());
				to_search_child_hashs.pop();

				std::shared_ptr<std::list<mcp::block_hash>> child_hashs(std::make_shared<std::list<mcp::block_hash>>());
				m_store.block_children_get(transaction_a, p_hash, *child_hashs);
				for (auto it(child_hashs->begin()); it != child_hashs->end(); it++)
				{
					mcp::block_hash const &child_hash(*it);
					std::shared_ptr<mcp::block> child_block(cache_a->block_get(transaction_a, child_hash));
					assert_x(child_block);
					assert_x(child_block->hashables->type == mcp::block_type::dag);

					std::shared_ptr<mcp::block_state> child_state(cache_a->block_state_get(transaction_a, child_hash));
					assert_x(child_state);

					auto r = to_update_hashs[child_state->level].insert(child_hash);
					if (r.second)
						to_search_child_hashs.push(child_hash);
				}
			}
		}
	}

	std::shared_ptr<mcp::block_state> block_state(cache_a->block_state_get(transaction_a, block_hash));
	assert_x(block_state);
	if (to_update_hashs.empty())
	{
		to_update_hashs[block_state->level].insert(block_hash);
	}
	else
	{
		assert_x(to_update_hashs[block_state->level].count(block_hash));
	}

	{
		//mcp::stopwatch_guard sw2("update_latest_included_mci2");

		//get from unstable blocks where main_chain_index > last_main_chain_index or main_chain_index == null
		auto end = to_update_hashs.end();
		for (auto u_it(to_update_hashs.begin()); u_it != end; u_it++)
		{
			//mcp::stopwatch_guard sw3("update_latest_included_mci3");
			std::unordered_set<mcp::block_hash> & u_hashs(u_it->second);
			for (auto it(u_hashs.begin()); it != u_hashs.end(); it++)
			{
				mcp::block_hash const &u_block_hash(*it);
				if (u_block_hash == block_hash)
					is_curr_block_set = true;

				std::shared_ptr<mcp::block_state> u_block_state_in_cache(cache_a->block_state_get(transaction_a, u_block_hash));
				assert_x(u_block_state_in_cache);
				//make a copy for not changing value in cache
				std::shared_ptr<mcp::block_state> u_block_state_copy(std::make_shared<mcp::block_state>(*u_block_state_in_cache));

				if (!u_block_state_copy->main_chain_index || (*u_block_state_copy->main_chain_index) > retreat_mci)
				{
					std::shared_ptr<mcp::block> u_block = cache_a->block_get(transaction_a, u_block_hash);
					assert_x(u_block != nullptr);

					boost::optional<uint64_t> min_limci;
					boost::optional<uint64_t> max_limci;
					boost::optional<uint64_t> bp_limci;
					auto const &u_pblock_hashs(u_block->parents());
					for (mcp::block_hash const &u_pblock_hash : u_pblock_hashs)
					{
						std::shared_ptr<mcp::block_state> u_pblock_state(cache_a->block_state_get(transaction_a, u_pblock_hash));
						assert_x(u_pblock_state);

						if (u_pblock_state->is_on_main_chain)
						{
							assert_x(u_pblock_state->main_chain_index);

							if (!min_limci || *min_limci > *u_pblock_state->main_chain_index)
								min_limci = u_pblock_state->main_chain_index;

							if (!max_limci || *max_limci < *u_pblock_state->main_chain_index)
								max_limci = u_pblock_state->main_chain_index;
						}
						else
						{
							assert_x(u_pblock_state->latest_included_mc_index);

							if (!min_limci || *min_limci > *u_pblock_state->earliest_included_mc_index)
								min_limci = u_pblock_state->earliest_included_mc_index;

							if (!max_limci || *max_limci < *u_pblock_state->latest_included_mc_index)
								max_limci = u_pblock_state->latest_included_mc_index;
						}

						//best parent limci
						if (u_pblock_hash == u_block_state_copy->best_parent)
						{
							if (u_pblock_state->is_on_main_chain)
							{
								assert_x(u_pblock_state->main_chain_index);
								bp_limci = u_pblock_state->main_chain_index;
							}
							else
							{
								assert_x(u_pblock_state->bp_included_mc_index);
								bp_limci = u_pblock_state->bp_included_mc_index;
							}
						}
					}
					assert_x(min_limci);
					assert_x(max_limci);
					assert_x(bp_limci);

					boost::optional<uint64_t> min_bp_limci = bp_limci;
					boost::optional<uint64_t> max_bp_limci = bp_limci;
					for (mcp::block_hash const &u_pblock_hash : u_pblock_hashs)
					{
						std::shared_ptr<mcp::block_state> u_pblock_state(cache_a->block_state_get(transaction_a, u_pblock_hash));
						assert_x(u_pblock_state);

						if (!u_pblock_state->is_on_main_chain)
						{
							assert_x(u_pblock_state->latest_bp_included_mc_index);
							//max_bp_limci
							if (*max_bp_limci < *u_pblock_state->latest_bp_included_mc_index)
								max_bp_limci = u_pblock_state->latest_bp_included_mc_index;

							assert_x(u_pblock_state->earliest_bp_included_mc_index);
							//min_bp_limci
							if (*min_bp_limci > *u_pblock_state->earliest_bp_included_mc_index)
								min_bp_limci = u_pblock_state->earliest_bp_included_mc_index;
						}
					}

					assert_x(min_bp_limci);
					assert_x(max_bp_limci);

					u_block_state_copy->earliest_included_mc_index = min_limci;
					u_block_state_copy->latest_included_mc_index = max_limci;
					u_block_state_copy->bp_included_mc_index = bp_limci;
					u_block_state_copy->earliest_bp_included_mc_index = min_bp_limci;
					u_block_state_copy->latest_bp_included_mc_index = max_bp_limci;
					cache_a->block_state_put(transaction_a, u_block_hash, u_block_state_copy);
				}
			}
		}
	}

	//LOG(m_log.debug) << "update limci:" << to_update_hashs.size();
}

void mcp::chain::advance_stable_mci(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, uint64_t const &mci, mcp::block_hash const & block_hash_a)
{
	mcp::db::db_transaction & transaction_a(timeout_tx_a.get_transaction());

	mcp::block_hash mc_stable_hash;
	bool mc_stable_hash_error(m_store.main_chain_get(transaction_a, mci, mc_stable_hash));
	assert_x(!mc_stable_hash_error);

	std::map<uint64_t, std::set<mcp::block_hash>> dag_stable_block_hashs; //order by block level and hash
	search_stable_block(transaction_a, cache_a, mc_stable_hash, mci, dag_stable_block_hashs);

	std::shared_ptr<mcp::block> mc_stable_block = cache_a->block_get(transaction_a, mc_stable_hash);
	assert_x(mc_stable_block != nullptr);

	std::shared_ptr<mcp::block_state> last_summary_state(cache_a->block_state_get(transaction_a, mc_stable_block->hashables->last_summary_block));
	assert_x(last_summary_state);
	assert_x(last_summary_state->is_stable);
	assert_x(last_summary_state->is_on_main_chain);
	assert_x(last_summary_state->main_chain_index);
	uint64_t const & mc_last_summary_mci = *last_summary_state->main_chain_index;

	auto block_to_advance = cache_a->block_get(transaction_a, block_hash_a);
	uint64_t const & stable_timestamp = block_to_advance->hashables->exec_timestamp;
	uint64_t const & mc_timestamp = mc_stable_block->hashables->exec_timestamp;

	for (auto iter_p(dag_stable_block_hashs.begin()); iter_p != dag_stable_block_hashs.end(); iter_p++)
	{
		std::set<mcp::block_hash> const & hashs(iter_p->second);
		for (auto iter(hashs.begin()); iter != hashs.end(); iter++)
		{
			mcp::block_hash const & dag_stable_block_hash(*iter);
			std::map<block_hash, std::list<mcp::block_hash>> light_stable_block_hashs;
			{
				//mcp::stopwatch_guard sw("advance_stable_mci2_1");

				//handle dag stable block 
				std::shared_ptr<mcp::block> dag_stable_block = cache_a->block_get(transaction_a, dag_stable_block_hash);
				assert_x(dag_stable_block);

				//handle light stable block 
				//account A : b2, b3, b4, b5
				//account B : b1, b2, b3
				//account c : b2, b3
				std::shared_ptr<std::list<mcp::block_hash>> links(dag_stable_block->links());
				for (auto it(links->begin()); it != links->end(); it++)
				{
					mcp::block_hash const & link_hash(*it);
					mcp::block_hash light_stable_block_hash(link_hash);
					while (true)
					{
						std::shared_ptr<mcp::block_state> light_block_state(cache_a->block_state_get(transaction_a, light_stable_block_hash));
						assert_x(light_block_state);
						if (light_block_state->is_stable)
							break;
						light_stable_block_hashs[link_hash].push_front(light_stable_block_hash);

						std::shared_ptr<mcp::block> light_block(cache_a->block_get(transaction_a, light_stable_block_hash));
						assert_x(light_block);
						if (light_block->previous().is_zero())
							break;
						light_stable_block_hash = light_block->previous();
					}
				}
			}

			{
				//mcp::stopwatch_guard sw("advance_stable_mci2_2");

				for (auto it_p(light_stable_block_hashs.begin()); it_p != light_stable_block_hashs.end(); it_p++)
				{
					std::list<mcp::block_hash> const & light_hashs(it_p->second);
					for (auto it(light_hashs.begin()); it != light_hashs.end(); it++)
					{
						m_last_stable_index_internal++;
						mcp::block_hash const & light_stable_block_hash(*it);
						set_block_stable(timeout_tx_a, cache_a, light_stable_block_hash, mci, mc_timestamp, mc_last_summary_mci, stable_timestamp, m_last_stable_index_internal);
					}
				}

				m_last_stable_index_internal++;
				set_block_stable(timeout_tx_a, cache_a, dag_stable_block_hash, mci, mc_timestamp, mc_last_summary_mci, stable_timestamp, m_last_stable_index_internal);
			}
		}

		//LOG(m_log.debug) << "Mci: " << mci << ", stable_index: " << stable_index;
	}
}

void mcp::chain::set_block_stable(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & stable_block_hash, 
	uint64_t const & mci, uint64_t const & mc_timestamp, uint64_t const & mc_last_summary_mci, 
	uint64_t const & stable_timestamp, uint64_t const & stable_index)
{
	mcp::db::db_transaction & transaction_a(timeout_tx_a.get_transaction());
	try
	{
		std::shared_ptr<mcp::block> stable_block(cache_a->block_get(transaction_a, stable_block_hash));
		assert_x(stable_block);

		std::shared_ptr<mcp::block_state> stable_block_state_in_cache(cache_a->block_state_get(transaction_a, stable_block_hash));
		assert_x(stable_block_state_in_cache);
		//make a copy for not changing value in cache
		std::shared_ptr<mcp::block_state> stable_block_state_copy(std::make_shared<mcp::block_state>(*stable_block_state_in_cache));

		//has set
		if (stable_block_state_copy->is_stable)
		{
			assert_x(false);
		}

		assert_x(stable_block_state_copy->status == mcp::block_status::unknown);

		stable_block_state_copy->status = mcp::block_status::ok;

#pragma region check fork
		mcp::block_hash previous_hash(stable_block->previous());
		std::shared_ptr<mcp::block_state> previous_state;
		if (!previous_hash.is_zero())
		{
			previous_state = cache_a->block_state_get(transaction_a, previous_hash);
			assert_x(previous_state);
		}

		if (!previous_hash.is_zero() && previous_state->status == mcp::block_status::fork)
			stable_block_state_copy->status = mcp::block_status::fork;
		else
		{
			mcp::block_hash old_successor_hash;
			bool old_successor_exists(!cache_a->successor_get(transaction_a, stable_block->root(), old_successor_hash));
			assert_x_msg(old_successor_exists, "hash: " + stable_block->root().to_string());
			if (old_successor_hash != stable_block_hash)
			{
				std::shared_ptr<mcp::block_state> old_successor_state(cache_a->block_state_get(transaction_a, old_successor_hash));
				if (old_successor_state && old_successor_state->is_stable)
					stable_block_state_copy->status = mcp::block_status::fork;
				else
				{
					cache_a->successor_put(transaction_a, stable_block->root(), stable_block_hash);
					if (stable_block->hashables->type == mcp::block_type::light)
					{
						rebuild_unlink(transaction_a, cache_a, stable_block->hashables->from);
					}
				}
			}
		}
#pragma endregion

		if (stable_block_state_copy->status == mcp::block_status::ok)
		{
			mcp::account const & account(stable_block->hashables->from);
			switch (stable_block->hashables->type)
			{
			case mcp::block_type::dag:
			{
				mcp::dag_account_info info;
				m_store.dag_account_get(transaction_a, account, info);
				info.latest_stable_block = stable_block_hash;
				m_store.dag_account_put(transaction_a, account, info);
				break;
			}
			case mcp::block_type::light:
			{
				std::shared_ptr<mcp::account_info> info(cache_a->account_get(transaction_a, account));
				assert_x(info);
				std::shared_ptr<mcp::account_info> info_copy(std::make_shared<mcp::account_info>(*info));
				info_copy->latest_stable_block = stable_block_hash;
				cache_a->account_put(transaction_a, account, info_copy);
				break;
			}
			default:
			{
				assert_x_msg(false, "invalid block type");
				throw;
			}
			}
		}

		//only light block will change account state, for example balance
		if (stable_block->hashables->type == mcp::block_type::light)
		{
			//mcp::stopwatch_guard sw("set_block_stable2");

#pragma region update account state

			bool is_invalid(false);
			bool is_fail(false);
			if (stable_block_state_copy->status != mcp::block_status::fork)
			{
				try
				{
					dev::eth::McInfo mc_info(mci, mc_timestamp, mc_last_summary_mci);
					//mcp::stopwatch_guard sw("set_block_stable2_1");
					std::pair<ExecutionResult, dev::eth::TransactionReceipt> result = execute(transaction_a, cache_a, stable_block, mc_info, Permanence::Committed, dev::eth::OnOpFunc());

					// check if the execution is successful
					if (result.second.statusCode() == 0)
					{
						//std::cerr << "TransactionException: " << static_cast<std::underlying_type<mcp::TransactionException>::type>(result.first.excepted) << " "
						//	<< "Output: " << toHex(result.first.output) << std::endl;
						is_fail = true;
					}

					// transaction receipt
					// the account states were committed in Executive::go()
					std::shared_ptr<mcp::account_state> from_state(cache_a->latest_account_state_get(transaction_a, stable_block->hashables->from));
					assert_x(from_state);

					std::set<mcp::account_state_hash> to_states;
					for (auto account : result.first.modified_accounts)
					{
						if (account != stable_block->hashables->from)
						{
							std::shared_ptr<mcp::account_state> to_state(cache_a->latest_account_state_get(transaction_a, account));
							to_states.insert(to_state->hash());
						}
					}

					stable_block_state_copy->receipt = transaction_receipt(from_state->hash(), to_states,
						result.second.cumulativeGasUsed(), result.second.log());
				}
				catch (dev::eth::NotEnoughCash const& _e)
				{
					std::cout << "NotEnoughCash" << std::endl;
					is_invalid = true;
				}
				catch (Exception const& _e)
				{
					cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
						<< diagnostic_information(_e);
					exit(1);
				}
				catch (std::exception const& _e)
				{
					std::cerr << _e.what() << std::endl;
					throw;
				}
			}

			if ((stable_block_state_copy->status == mcp::block_status::fork) || is_invalid)
			{
				//m_store.block_data_del(transaction_a, stable_block->hashables->data_hash);
			}

			//save block state
			if (is_fail)
			{
				stable_block_state_copy->status = mcp::block_status::fail;
			}
			else if (is_invalid)
			{
				stable_block_state_copy->status = mcp::block_status::invalid;
			}
			else
			{
				;
			}
#pragma endregion

		}

		{
			//mcp::stopwatch_guard sw("set_block_stable3");

			if (!stable_block_state_copy->main_chain_index || *stable_block_state_copy->main_chain_index != mci)
				stable_block_state_copy->main_chain_index = mci;
			stable_block_state_copy->mc_timestamp = mc_timestamp;
			stable_block_state_copy->stable_timestamp = stable_timestamp;
			stable_block_state_copy->is_stable = true;
			stable_block_state_copy->stable_index = stable_index;
			cache_a->block_state_put(transaction_a, stable_block_hash, stable_block_state_copy);

			m_store.stable_block_put(transaction_a, stable_index, stable_block_hash);
			m_store.last_stable_index_put(transaction_a, stable_index);

			if (stable_block->hashables->type == mcp::block_type::light)
				m_store.light_unstable_count_reduce(transaction_a);

#pragma region summary

			//previous summary hash
			mcp::summary_hash previous_summary_hash(0);
			if (!stable_block->previous().is_zero())
			{
				bool previous_summary_hash_error(cache_a->block_summary_get(transaction_a, stable_block->previous(), previous_summary_hash));
				assert_x(!previous_summary_hash_error);
			}

			//parent summary hashs
			std::list<mcp::summary_hash> p_summary_hashs;
			for (mcp::block_hash const & pblock_hash : stable_block->parents())
			{
				mcp::summary_hash p_summary_hash;
				bool p_summary_hash_error(cache_a->block_summary_get(transaction_a, pblock_hash, p_summary_hash));
				assert_x(!p_summary_hash_error);

				p_summary_hashs.push_back(p_summary_hash);
			}

			//link summary hashs
			std::shared_ptr<std::list<mcp::block_hash>> links(stable_block->links());
			std::list<mcp::summary_hash> l_summary_hashs;
			for (auto it(links->begin()); it != links->end(); it++)
			{
				mcp::block_hash const & link_hash(*it);
				mcp::summary_hash l_summary_hash;
				bool link_summary_hash_error(cache_a->block_summary_get(transaction_a, link_hash, l_summary_hash));
				assert_x(!link_summary_hash_error);

				l_summary_hashs.push_back(l_summary_hash);
			}

			//skip list
			std::set<mcp::block_hash> block_skiplist;
			std::set<mcp::summary_hash> summary_skiplist;
			if (stable_block_state_copy->is_on_main_chain)
			{
				assert_x(stable_block_state_copy->main_chain_index);
				std::vector<uint64_t> skip_list_mcis = cal_skip_list_mcis(*stable_block_state_copy->main_chain_index);
				for (uint64_t & sk_mci : skip_list_mcis)
				{
					mcp::block_hash sl_block_hash;
					bool sl_block_hash_error(m_store.main_chain_get(transaction_a, sk_mci, sl_block_hash));
					assert_x(!sl_block_hash_error);
					block_skiplist.insert(sl_block_hash);

					mcp::summary_hash sl_summary_hash;
					bool sl_summary_hash_error(cache_a->block_summary_get(transaction_a, sl_block_hash, sl_summary_hash));
					assert_x(!sl_summary_hash_error);
					summary_skiplist.insert(sl_summary_hash);
				}

				if (!block_skiplist.empty())
					m_store.skiplist_put(transaction_a, stable_block_hash, mcp::skiplist_info(block_skiplist));
			}

			mcp::summary_hash summary_hash = mcp::summary::gen_summary_hash(stable_block_hash, previous_summary_hash, p_summary_hashs, l_summary_hashs, summary_skiplist,
				stable_block_state_copy->status, stable_block_state_copy->stable_index, stable_block_state_copy->mc_timestamp, stable_block_state_copy->receipt);

			cache_a->block_summary_put(transaction_a, stable_block_hash, summary_hash);
			m_store.summary_block_put(transaction_a, summary_hash, stable_block_hash);

#pragma endregion

			//add hash tree summary to delete list
			m_complete_store_notice(summary_hash);
		}

		m_stable_blocks.push(stable_block);

		timeout_tx_a.commit_if_timeout();
	}
	catch (std::exception const & e)
	{
		LOG(m_log.error) << "Chain Set block stable error: " << e.what();
		timeout_tx_a.rollback();
		throw;
	}
}

void mcp::chain::search_stable_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const &block_hash_a, uint64_t const &mci, std::map<uint64_t, std::set<mcp::block_hash>> &stable_block_level_and_hashs)
{
	std::queue<mcp::block_hash> queue;
	queue.push(block_hash_a);

	while (!queue.empty())
	{
		mcp::block_hash block_hash(queue.front());
		queue.pop();

		if (block_hash == mcp::genesis::block_hash)
			continue;

		std::shared_ptr<mcp::block_state> state(cache_a->block_state_get(transaction_a, block_hash));
		assert_x(state);

		if (state->is_stable)
			continue;

		auto r = stable_block_level_and_hashs[state->level].insert(block_hash);
		if (!r.second)
			continue;

		std::shared_ptr<mcp::block> block(cache_a->block_get(transaction_a, block_hash));
		for (mcp::block_hash const &pblock_hash : block->parents())
		{
			queue.push(pblock_hash);
		}
	}
}

std::vector<uint64_t> mcp::chain::cal_skip_list_mcis(uint64_t const &mci)
{
	std::vector<uint64_t> skip_list_mcis;
	uint64_t divisor = mcp::skiplist_divisor;
	while (true)
	{
		if (mci % divisor == 0)
		{
			skip_list_mcis.push_back(mci - divisor);
			divisor *= mcp::skiplist_divisor;
		}
		else
			return skip_list_mcis;
	}
}

void mcp::chain::rebuild_unlink(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::account const & account_a)
{
	std::shared_ptr<mcp::account_info> info(cache_a->account_get(transaction_a, account_a));
	std::shared_ptr<mcp::account_info> info_copy(info ? std::make_shared<mcp::account_info>(*info) : std::make_shared<mcp::account_info>());
	mcp::block_hash latest_block_hash = info ? info_copy->latest_stable_block : 0;
	mcp::block_hash root(latest_block_hash.is_zero() ? account_a.number() : latest_block_hash);
	mcp::unlink_info unlink;
	bool rebuild = false;

	//search in chain
	while (true)
	{
		mcp::block_hash successor_hash;
		bool exists(!cache_a->successor_get(transaction_a, root, successor_hash));
		if (!exists)
			break;

		latest_block_hash = std::move(successor_hash);
		root = latest_block_hash;
		if (cache_a->block_get(transaction_a, latest_block_hash))
		{
			info_copy->latest_linked = latest_block_hash;
		}
		else
		{
			if (unlink.earliest_unlink.is_zero())
				unlink.earliest_unlink = latest_block_hash;
			unlink.latest_unlink = latest_block_hash;
			rebuild = true;

			//LOG(m_log.info) << "rebuild unlink info, account: " << account_a.to_string()
			//	<< " ,hash:" << latest_block_hash.to_string();
		}
	}

	if (rebuild)
	{
		m_store.unlink_info_put(transaction_a, account_a, unlink);
	}
	else
	{
		m_store.unlink_info_del(transaction_a, account_a);
	}
	
	cache_a->account_put(transaction_a, account_a, info_copy);
}

void mcp::chain::update_cache()
{
	m_last_mci = m_last_mci_internal;
	m_last_stable_mci = m_last_stable_mci_internal;
	m_min_retrievable_mci = m_min_retrievable_mci_internal;
	m_last_stable_index = m_last_stable_index_internal;
}

uint64_t mcp::chain::last_mci()
{
	return m_last_mci;
}

uint64_t mcp::chain::last_stable_mci()
{
	return m_last_stable_mci;
}

uint64_t mcp::chain::min_retrievable_mci()
{
	return m_min_retrievable_mci;
}

uint64_t mcp::chain::last_stable_index()
{
	return m_last_stable_index;
}

bool mcp::chain::get_mc_info_from_block_hash(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash hash_a, dev::eth::McInfo & mc_info_a)
{
	std::shared_ptr<mcp::block_state> block_state(cache_a->block_state_get(transaction_a, hash_a));

	if (!block_state || !block_state->is_stable || !block_state->main_chain_index)
		return false;

	mcp::block_hash mc_hash;
	bool exists(!m_store.main_chain_get(transaction_a, *block_state->main_chain_index, mc_hash));
	assert_x(exists);
	std::shared_ptr<mcp::block_state> mc_state(cache_a->block_state_get(transaction_a, mc_hash));
	assert_x(mc_state);
	assert_x(mc_state->is_stable);
	assert_x(mc_state->main_chain_index);
	assert_x(mc_state->mc_timestamp > 0);

	uint64_t last_summary_mci(0);
	if (mc_hash != mcp::genesis::block_hash)
	{
		std::shared_ptr<mcp::block> mc_block(cache_a->block_get(transaction_a, mc_hash));
		assert_x(mc_block);
		std::shared_ptr<mcp::block_state> last_summary_state(cache_a->block_state_get(transaction_a, mc_block->hashables->last_summary_block));
		assert_x(last_summary_state);
		assert_x(last_summary_state->is_stable);
		assert_x(last_summary_state->is_on_main_chain);
		assert_x(last_summary_state->main_chain_index);
		last_summary_mci = *last_summary_state->main_chain_index;

		mc_info_a = dev::eth::McInfo(*mc_state->main_chain_index, mc_state->mc_timestamp, last_summary_mci);
		return true;
	}
	else
		return false;
}

void mcp::chain::notify_observers()
{
	while (!m_new_blocks.empty())
	{
		for (auto it = m_new_block_observer.begin(); it != m_new_block_observer.end(); it++)
		{
			(*it)(m_new_blocks.front());
		}
		m_new_blocks.pop();
	}


	while (!m_stable_blocks.empty())
	{
		for (auto it = m_stable_block_observer.begin(); it != m_stable_block_observer.end(); it++)
		{
			(*it)(m_stable_blocks.front());
		}
		m_stable_blocks.pop();
	}

	while (!m_stable_mcis.empty())
	{
		for (auto it = m_stable_mci_observer.begin(); it != m_stable_mci_observer.end(); it++)
		{
			(*it)(m_stable_mcis.front());
		}
		m_stable_mcis.pop();
	}
}

std::pair<u256, bool> mcp::chain::estimate_gas(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a,
	mcp::account const& _from, u256 _value, mcp::account _to, bytes const& _data, int64_t _maxGas, u256 _gasPrice, dev::eth::McInfo const & mc_info_a, GasEstimationCallback const& _callback)
{
	try
    {
		if (_maxGas == 0)
			_maxGas = mcp::block_max_gas;

		int64_t lowerBound = mcp::block::baseGasRequired(_to.is_zero(), &_data, dev::eth::EVMSchedule());
		if(_maxGas < lowerBound)
			return std::make_pair(u256(), false);

        int64_t upperBound = _maxGas;
        if (upperBound == Invalid256 || upperBound > mcp::block_max_gas)
            upperBound = mcp::block_max_gas;

		// sichaoy: default gas price to be defined
        // u256 gasPrice = _gasPrice == Invalid256 ? gasBidPrice() : _gasPrice;
		u256 gasPrice = _gasPrice;
        ExecutionResult er;
        ExecutionResult lastGood;
        bool good = false;

		do
		{
			int64_t mid = (lowerBound + upperBound) / 2;
			std::shared_ptr<mcp::block> block = std::make_shared<mcp::block>(
				mcp::block_type::light,             //mcp::block_type type_a,
				_from,                              // mcp::account const & from_a,
				_to,                                // mcp::account const & to_a,
				_value,                             // mcp::amount const & amount_a,
				0,                                  // mcp::block_hash const & previous_a,
				std::vector<mcp::block_hash>{},     // std::vector<mcp::block_hash> const & parents_a,
				std::make_shared<std::list<mcp::block_hash>>(std::list<mcp::block_hash>{}), // std::shared_ptr<std::list<mcp::block_hash>> links_a,
				0,                                  // mcp::summary_hash const & last_summary_a,
				0,                                  // mcp::block_hash const & last_summary_block_a,
				0,                                  // mcp::block_hash const & last_stable_block_a,
				mid,  	                            // uint256_t gas_a,
				_gasPrice,                          // uint256_t gas_price_a,
				mcp::block::data_hash(_data),       // mcp::data_hash const & data_hash_a,
				_data,                              // std::vector<uint8_t> const & data_a,
				0,                                  // uint64_t const & exec_timestamp_a,
				mcp::uint64_union(0)                // mcp::uint64_union const & work_a
				);

			dev::eth::EnvInfo env(transaction_a, m_store, cache_a, mc_info_a);
			auto chain_ptr(shared_from_this());
			chain_state c_state(transaction_a, 0, m_store, chain_ptr, cache_a, block);
			c_state.addBalance(_from, mid * _gasPrice + _value);
			std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = c_state.execute(env, Permanence::Reverted, dev::eth::OnOpFunc());
			er = result.first;
			if (er.excepted == TransactionException::OutOfGas ||
				er.excepted == TransactionException::OutOfGasBase ||
				er.excepted == TransactionException::OutOfGasIntrinsic ||
				er.codeDeposit == CodeDeposit::Failed ||
				er.excepted == TransactionException::BadJumpDestination)
				lowerBound = lowerBound == mid ? upperBound : mid;
			else
			{
				lastGood = er;
				upperBound = upperBound == mid ? lowerBound : mid;
				good = true;
			}
		} 
		while (upperBound != lowerBound);

        if (_callback)
            _callback(GasEstimationProgress { lowerBound, upperBound });
        return std::make_pair(upperBound, good);
    }
    catch (std::exception const & e)
    {
		LOG(m_log.error) << "estimate_gas error:" << e.what();
        return std::make_pair(u256(), false);
    }
	catch (...)
	{
		LOG(m_log.error) << "estimate_gas unknown error";
		// TODO: Some sort of notification of failure.
		return std::make_pair(u256(), false);
	}
}

// This is the top function to be called by js call(). The reason to have this extra wrapper is to have this function
// be called other methods except chain::set_block_stable
std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> mcp::chain::execute(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, const std::shared_ptr<mcp::block> block, dev::eth::McInfo const & mc_info_a ,Permanence _p, dev::eth::OnOpFunc const& _onOp)
{
	dev::eth::EnvInfo env(transaction_a, m_store, cache_a, mc_info_a);
	// sichaoy: startNonce = 0
	auto chain_ptr(shared_from_this());
	chain_state c_state(transaction_a, 0, m_store, chain_ptr, cache_a, block);

	//mcp::stopwatch_guard sw("advance_mc_stable_block3_1_1");
	return c_state.execute(env, _p, _onOp);
}

mcp::json mcp::chain::traceTransaction(Executive& _e, mcp::json const& _json)
{
	StandardTrace st;
	st.setShowMnemonics();
	st.setOptions(debugOptions(_json));
	_e.initialize();
	if (!_e.execute())
		_e.go(st.onOp());
	_e.finalize();
	return st.jsonValue();
}
