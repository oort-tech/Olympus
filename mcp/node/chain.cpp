#include "chain.hpp"
#include <mcp/core/genesis.hpp>
#include <mcp/common/stopwatch.hpp>
#include <mcp/common/Exceptions.h>
#include <mcp/node/debug.hpp>
#include <mcp/node/evm/Executive.hpp>
#include <libdevcore/TrieHash.h>
#include <libdevcore/CommonJS.h>
#include <mcp/core/config.hpp>

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

void mcp::chain::init(bool & error_a, mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block_cache> block_cache_a)
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

	LOG(m_log.info) << "Genesis block:" << mcp::genesis::block_hash.hex();

	// Init precompiled contract account
	if (first_time)
	{
		AccountMap precompiled_accounts;
		for (unsigned i = 1; i <= 8; ++i)
		{
			Address acc(i);
			precompiled_accounts[acc] = std::make_shared<mcp::account_state>(acc, h256(0), h256(0), 0, 0);
		}

		mcp::overlay_db db(transaction, m_store);
		mcp::commit(transaction, precompiled_accounts, &db, cache_a, m_store,h256(0));
	}

	// Setup default precompiled contracts as equal to genesis of Frontier.
	m_precompiled.insert(std::make_pair(Address(1), dev::eth::PrecompiledContract(3000, 0, dev::eth::PrecompiledRegistrar::executor("ecrecover"))));
	m_precompiled.insert(std::make_pair(Address(2), dev::eth::PrecompiledContract(60, 12, dev::eth::PrecompiledRegistrar::executor("sha256"))));
	m_precompiled.insert(std::make_pair(Address(3), dev::eth::PrecompiledContract(600, 120, dev::eth::PrecompiledRegistrar::executor("ripemd160"))));
	m_precompiled.insert(std::make_pair(Address(4), dev::eth::PrecompiledContract(15, 3, dev::eth::PrecompiledRegistrar::executor("identity"))));
	m_precompiled.insert(std::make_pair(Address(5), dev::eth::PrecompiledContract(dev::eth::PrecompiledRegistrar::pricer("modexp"), dev::eth::PrecompiledRegistrar::executor("modexp"))));
	m_precompiled.insert(std::make_pair(Address(6), dev::eth::PrecompiledContract(500, 0, dev::eth::PrecompiledRegistrar::executor("alt_bn128_G1_add"))));
	m_precompiled.insert(std::make_pair(Address(7), dev::eth::PrecompiledContract(40000, 0, dev::eth::PrecompiledRegistrar::executor("alt_bn128_G1_mul"))));
	m_precompiled.insert(std::make_pair(Address(8), dev::eth::PrecompiledContract(dev::eth::PrecompiledRegistrar::pricer("alt_bn128_pairing_product"), dev::eth::PrecompiledRegistrar::executor("alt_bn128_pairing_product"))));

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
		std::shared_ptr<mcp::block_state> min_retrievable_state(m_store.block_state_get(transaction, mc_stable_block->last_stable_block()));
		assert_x(min_retrievable_state);
		assert_x(min_retrievable_state->main_chain_index);
		m_min_retrievable_mci_internal = *min_retrievable_state->main_chain_index;
	}

	m_last_stable_index_internal = m_store.last_stable_index_get(transaction);

	m_advance_info = m_store.advance_info_get(transaction);
	m_last_epoch = m_store.last_epoch_get(transaction);
	m_last_summary_mci = get_last_summary_mci(transaction, cache_a, block_cache_a, m_last_mci_internal);
	LOG(m_log.debug) << "m_last_mci_internal: " << m_last_mci_internal << " m_last_stable_mci_internal: " << m_last_stable_mci_internal;
	LOG(m_log.debug) << "m_last_epoch: " << m_last_epoch << " m_last_summary_mci: " << m_last_summary_mci;

	update_cache();
	init_witness(transaction, cache_a);
	init_vrf_outputs(transaction, cache_a);
}

void mcp::chain::stop()
{
	m_stopped = true;
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
			std::shared_ptr<mcp::block_state> last_stable_block_state(cache_a->block_state_get(transaction, block_a->last_stable_block()));
			assert_x(last_stable_block_state->is_on_main_chain);
			assert_x(last_stable_block_state->main_chain_index);
			uint64_t last_stable_block_mci = *last_stable_block_state->main_chain_index;
			if (m_last_stable_mci_internal < last_stable_block_mci)
			{
				m_advance_info = mcp::advance_info(last_stable_block_mci, b_hash);
				m_store.advance_info_put(transaction, m_advance_info);
			}

			//m_new_blocks.push(block_a);

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

void mcp::chain::save_transaction(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::Transaction> t_a)
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
				auto const& hash = t_a->sha3();
				if (cache_a->transaction_exists(transaction, hash))
				{
					assert_x_msg(false, "block exist do not added count,hash:" + hash.hex());
				}

				//save transaction, need put first
				cache_a->transaction_put(transaction, t_a);
				u256 _n = 0;
				if (!(cache_a->account_nonce_get(transaction, t_a->sender(), _n) && t_a->nonce() < _n))
					cache_a->account_nonce_put(transaction, t_a->sender(), t_a->nonce());
				m_store.transaction_unstable_count_add(transaction);
				m_store.transaction_count_add(transaction);
				cache_a->transaction_del_from_queue(hash);///mark as clear,It will be really cleaned up after commit event
			}

			//m_new_blocks.push(block_a->block);

			//timeout_tx_a.commit_if_timeout();
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain save transaction error: " << e.what();
			timeout_tx_a.rollback();
			throw;
		}
	}
}

void mcp::chain::save_approve(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::approve> t_a)
{
	if (m_stopped)
		return;

	{
		mcp::db::db_transaction & transaction(timeout_tx_a.get_transaction());
		try
		{
			{
				//mcp::stopwatch_guard sw("save_block:write_block");
				auto const& hash = t_a->sha3();
				if (cache_a->approve_exists(transaction, hash))
				{
					assert_x_msg(false, "block exist do not added count,hash:" + hash.hex());
				}

				//save approve, need put first
				cache_a->approve_put(transaction, t_a);
				m_store.epoch_approves_put(transaction, mcp::epoch_approves_key(t_a->m_epoch, t_a->sha3()));
				m_store.approve_unstable_count_add(transaction);
				LOG(m_log.debug) << "approve_unstable: add " << m_store.approve_unstable_count(transaction);
				m_store.approve_count_add(transaction);
				cache_a->approve_del_from_queue(hash, t_a->m_epoch);///mark as clear,It will be really cleaned up after commit event
			}

			//m_new_blocks.push(block_a->block);

			//timeout_tx_a.commit_if_timeout();
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain save approve error: " << e.what();
			timeout_tx_a.rollback();
			throw;
		}
	}
}

void mcp::chain::add_new_witness_list(mcp::db::db_transaction & transaction_a, uint64_t mc_last_summary_mci){
	static uint64_t old_summary_mci = 0;
	static uint64_t old_elected_epoch = 0;
	uint64_t elected_epoch = mcp::approve::calc_elect_epoch(mc_last_summary_mci) - 1;
	if(old_summary_mci == mc_last_summary_mci) return;
	else old_summary_mci = mc_last_summary_mci;

	if(m_restart_not_need_switch_witness){
		old_elected_epoch = elected_epoch;
		m_restart_not_need_switch_witness = false;
		return;
	}
	if(old_elected_epoch == elected_epoch) return;
	else old_elected_epoch = elected_epoch;

	mcp::witness_param w_param = mcp::param::witness_param(m_last_epoch);
	LOG(m_log.debug) << "[add_new_witness_list] in last_summary_mci = " << mc_last_summary_mci << " elected_epoch = " << elected_epoch;
	
	epoch_elected_list elected_list;
	if(vrf_outputs.find(elected_epoch) == vrf_outputs.end()) return;
	if(vrf_outputs[elected_epoch].size() < w_param.witness_count)
	{
		LOG(m_log.debug) << "Not switch witness_list because elector's number is too short: " << vrf_outputs.size();
		vrf_outputs[elected_epoch].clear();
		return;
	}
	std::vector<std::string> test_witness;
	auto it = vrf_outputs[elected_epoch].rbegin();
	for(int i=0; i<w_param.witness_count; i++){
		Address a = it->second.from();
		uint32_t output = it->first;
		test_witness.emplace_back(std::string("0x")+a.hex());
		elected_list.hashs.emplace_back(it->second.approve_hash());
		LOG(m_log.debug) << "elect " << a.hexPrefixed() << " output:" << output << " hash:" << it->second.approve_hash().hexPrefixed();
		it++;
	}
	w_param.witness_list.clear();
	w_param.witness_list = mcp::param::to_witness_list(test_witness);
	
	assert_x(w_param.witness_list.size() == w_param.witness_count);

	mcp::param::add_witness_param(elected_epoch, w_param);
	//The elected_list corresponds to next epoch.
	m_store.epoch_elected_approve_receipts_put(transaction_a, elected_epoch, elected_list);
	vrf_outputs[elected_epoch].clear();
	LOG(m_log.debug) << "elect to epoch " << elected_epoch;
}

void mcp::chain::init_vrf_outputs(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	std::list<h256> hashs;
	auto elect_epoch = mcp::approve::calc_elect_epoch(m_last_summary_mci);
	m_store.epoch_approve_receipts_get(transaction_a, elect_epoch, hashs);
	for(auto hash : hashs)
	{
		auto approve_receipt = cache_a->approve_receipt_get(transaction_a, hash);
		assert_x(approve_receipt);
		vrf_outputs[elect_epoch].insert(std::make_pair(*(uint32_t*)approve_receipt->output().data(), *approve_receipt));
		LOG(m_log.debug) << "[init_vrf_outputs] add output: sender=" << approve_receipt->from().hexPrefixed() << " output="<<*(uint32_t*)approve_receipt->output().data() << " epoch="<<approve_receipt->epoch();
	}
}

void mcp::chain::init_witness(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	mcp::witness_param w_param = mcp::param::witness_param(m_last_epoch);
	for(uint64_t i=1; i<=m_last_epoch + 2; i++){
		mcp::epoch_elected_list list;
		if(m_store.epoch_elected_approve_receipts_get(transaction_a, i, list)){
			continue;;
		}

		std::vector<std::string> test_witness;
		for(auto hash : list.hashs)
		{
			std::shared_ptr<dev::ApproveReceipt> receipt = cache_a->approve_receipt_get(transaction_a, hash);
			assert_x(receipt);

			test_witness.emplace_back(receipt->from().hexPrefixed());
			LOG(m_log.debug) << "[init_witness] epoch" << i << "\'s receipts from=" << receipt->from().hexPrefixed();
		}
		w_param.witness_list.clear();
		w_param.witness_list = mcp::param::to_witness_list(test_witness);		
		assert_x(w_param.witness_list.size() == w_param.witness_count);
		mcp::param::add_witness_param(i, w_param);
	}
}

void mcp::chain::try_advance(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	while (!m_stopped && ((dev::h64::Arith) m_advance_info.mci).convert_to<uint64_t>() > m_last_stable_mci_internal)
	{
		uint64_t last_summary_mci_stable;
		m_last_stable_mci_internal++;
		advance_stable_mci(timeout_tx_a, cache_a, m_last_stable_mci_internal, m_advance_info.witness_block, last_summary_mci_stable);
		LOG(m_log.debug) << "[try_advance] last_summary_mci_stable=" << last_summary_mci_stable;

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
			auto min_retrievable_state = cache_a->block_state_get(transaction, mc_stable_block->last_stable_block());
			assert_x(min_retrievable_state);
			assert_x(min_retrievable_state->main_chain_index);
			m_min_retrievable_mci_internal = *min_retrievable_state->main_chain_index;

			//m_stable_mcis.push(m_last_stable_mci_internal);

			//timeout_tx_a.commit_if_timeout();
			
			add_new_witness_list(transaction, last_summary_mci_stable);
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

	std::shared_ptr<mcp::block_state> last_summary_block_state(cache_a->block_state_get(transaction_a, block_a->last_summary_block()));
	assert_x(last_summary_block_state
		&& last_summary_block_state->is_stable
		&& last_summary_block_state->is_on_main_chain
		&& last_summary_block_state->main_chain_index);
	uint64_t const & last_summary_mci(*last_summary_block_state->main_chain_index);
	mcp::witness_param const & w_param(mcp::param::witness_param(mcp::approve::calc_curr_epoch(last_summary_mci)));

	//best parent
	mcp::block_hash best_pblock_hash(m_ledger.determine_best_parent(transaction_a, cache_a, block_a->parents()));
	//level
	level = m_ledger.calc_level(transaction_a, cache_a, best_pblock_hash);
	//witnessed level
	uint64_t witnessed_level(m_ledger.calc_witnessed_level(w_param, level));

	std::shared_ptr<mcp::block_state> state(std::make_shared<mcp::block_state>());
	state->status = mcp::block_status::unknown;
	state->is_free = true;
	state->best_parent = best_pblock_hash;
	state->level = level;
	state->witnessed_level = witnessed_level;
	cache_a->block_state_put(transaction_a, block_hash, state);

	m_store.dag_free_put(transaction_a, mcp::free_key(witnessed_level, level, block_hash));
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
			% block_a->hash().hex() % retreat_mci % m_last_stable_mci_internal));
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

void mcp::chain::advance_stable_mci(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, uint64_t const &mci, mcp::block_hash const & block_hash_a, uint64_t & mc_last_summary_mci)
{
	mcp::db::db_transaction & transaction_a(timeout_tx_a.get_transaction());

	mcp::block_hash mc_stable_hash;
	bool mc_stable_hash_error(m_store.main_chain_get(transaction_a, mci, mc_stable_hash));
	assert_x(!mc_stable_hash_error);

	std::map<uint64_t, std::set<mcp::block_hash>> dag_stable_block_hashs; //order by block level and hash
	search_stable_block(transaction_a, cache_a, mc_stable_hash, mci, dag_stable_block_hashs);

	std::shared_ptr<mcp::block> mc_stable_block = cache_a->block_get(transaction_a, mc_stable_hash);
	assert_x(mc_stable_block != nullptr);

	std::shared_ptr<mcp::block_state> last_summary_state(cache_a->block_state_get(transaction_a, mc_stable_block->last_summary_block()));
	assert_x(last_summary_state);
	assert_x(last_summary_state->is_stable);
	assert_x(last_summary_state->is_on_main_chain);
	assert_x(last_summary_state->main_chain_index);
	mc_last_summary_mci = *last_summary_state->main_chain_index;

	auto block_to_advance = cache_a->block_get(transaction_a, block_hash_a);
	uint64_t const & stable_timestamp = block_to_advance->exec_timestamp();
	uint64_t const & mc_timestamp = mc_stable_block->exec_timestamp();

	for (auto iter_p(dag_stable_block_hashs.begin()); iter_p != dag_stable_block_hashs.end(); iter_p++)
	{
		std::set<mcp::block_hash> const & hashs(iter_p->second);
		for (auto iter(hashs.begin()); iter != hashs.end(); iter++)
		{
			mcp::block_hash const & dag_stable_block_hash(*iter);
			m_last_stable_index_internal++;
			std::vector<bytes> receipts;
			{
				//mcp::stopwatch_guard sw("advance_stable_mci2_1");

				//handle dag stable block 
				std::shared_ptr<mcp::block> dag_stable_block = cache_a->block_get(transaction_a, dag_stable_block_hash);
				assert_x(dag_stable_block);

				///handle light stable block 
				///account A : b2, b3, b4, b5
				///account B : b1, b2, b3
				///account c : b2, b3
				auto links(dag_stable_block->links());
				unsigned index = 0;
				for (auto i = 0; i < links.size(); i++)
				{
					h256 const& link_hash = links[i];
					auto receipt = cache_a->transaction_receipt_get(transaction_a, link_hash);
					if (receipt)/// transaction maybe processed yet,but summary need used receipt even if it has been processed.
					{
						RLPStream receiptRLP;
						receipt->streamRLP(receiptRLP);
						receipts.push_back(receiptRLP.out());

						index++;
						continue;
					}
					auto _t = cache_a->transaction_get(transaction_a, link_hash);
					/// exec transactions
					bool invalid = false;
					try
					{
						dev::eth::McInfo mc_info(m_last_stable_index_internal, mci, mc_timestamp, mc_last_summary_mci);
						//mcp::stopwatch_guard sw("set_block_stable2_1");
						std::pair<ExecutionResult, dev::eth::TransactionReceipt> result = execute(transaction_a, cache_a, *_t, mc_info, Permanence::Committed, dev::eth::OnOpFunc());

						//// check if the execution is successful
						//if (result.second.statusCode() == 0)
						//{
						//	//std::cerr << "TransactionException: " << static_cast<std::underlying_type<mcp::TransactionException>::type>(result.first.excepted) << " "
						//	//	<< "Output: " << toHex(result.first.output) << std::endl;
						//	is_fail = true;
						//}

						/// commit transaction receipt
						/// the account states were committed in Executive::go()
						cache_a->transaction_receipt_put(transaction_a, link_hash, std::make_shared<dev::eth::TransactionReceipt>(result.second));
						RLPStream receiptRLP;
						result.second.streamRLP(receiptRLP);
						receipts.push_back(receiptRLP.out());
					}
					catch (dev::eth::NotEnoughCash const& _e)
					{
						LOG(m_log.info) << "transaction exec not enough cash,hash: " << _t->sha3().hex()
							<< ", from: " << dev::toJS(_t->sender())
							<< ", to: " << dev::toJS(_t->to())
							<< ", value: " << _t->value();
						invalid = true;
					}
					catch (dev::eth::InvalidNonce const& _e)
					{
						LOG(m_log.info) << "transaction exec not expect nonce,hash: " << _t->sha3().hex()
							<< ", from: " << dev::toJS(_t->sender())
							<< ", to: " << dev::toJS(_t->to())
							<< ", value: " << _t->value();
						invalid = true;
						assert(false);
					}
					//catch (Exception const& _e)
					//{
					//	cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
					//		<< diagnostic_information(_e);
					//	exit(1);
					//}
					catch (std::exception const& _e)
					{
						std::cerr << _e.what() << std::endl;
						throw;
					}

					LOG(m_log.info) << "exec transaction,hash: " << link_hash.hex() << " ,nonce:" << _t->nonce();

					if (invalid)
					{
						TransactionReceipt const receipt = TransactionReceipt(0, 0, mcp::log_entries());
						cache_a->transaction_receipt_put(transaction_a, link_hash, std::make_shared<dev::eth::TransactionReceipt>(receipt));
						RLPStream receiptRLP;
						receipt.streamRLP(receiptRLP);
						receipts.push_back(receiptRLP.out());
					}

					std::shared_ptr<mcp::TransactionAddress> td(std::make_shared<mcp::TransactionAddress>(dag_stable_block_hash, index));
					cache_a->transaction_address_put(transaction_a, link_hash, td);
					/// exec transaction can reduce, if two or more block linked a transaction,reduce once.
					m_store.transaction_unstable_count_reduce(transaction_a);
				}

				///handle approve stable block 
				auto approves(dag_stable_block->approves());
				for (auto i = 0; i < approves.size(); i++)
				{
					h256 const& approve_hash = approves[i];
					
					auto receipt = cache_a->approve_receipt_get(transaction_a, approve_hash);
					if (receipt)/// transaction maybe processed yet,but summary need used receipt even if it has been processed.
					{
						RLPStream receiptRLP;
						receipt->streamRLP(receiptRLP);
						receipts.push_back(receiptRLP.out());

						index++;
						continue;
					}

					auto ap = cache_a->approve_get(transaction_a, approve_hash);
					assert_x(ap);
					/// exec approves
					try{
						mcp::block_hash hash;
						if(ap->m_epoch <= 2){
							hash = mcp::block_hash(0);
						}
						else{
							bool exists(!m_store.stable_block_get(transaction_a, (ap->m_epoch-2)*epoch_period, hash));
							assert_x(exists);
						}
						
						std::vector<uint8_t> output;
						if(ap->m_epoch != mcp::approve::calc_elect_epoch(mc_last_summary_mci))
						{
							LOG(m_log.info) << "epoch " << ap->m_epoch << " mismatch last_summary_mci " << mc_last_summary_mci;
							continue;
						}
						ap->vrf_verify(output, hash.hex());

						/// exec approve can reduce, if two or more block linked a approve,reduce once.
						m_store.approve_unstable_count_reduce(transaction_a);
						//LOG(m_log.info) << "approve_unstable: reduce " << m_store.approve_unstable_count(transaction_a);

						std::shared_ptr<dev::ApproveReceipt> preceipt = std::make_shared<dev::ApproveReceipt>(ap->sender(), ap->m_epoch, output, approve_hash);
						cache_a->approve_receipt_put(transaction_a, approve_hash, preceipt);
						m_store.epoch_approve_receipts_put(transaction_a, mcp::epoch_approves_key(ap->m_epoch, approve_hash));
					
						vrf_outputs[ap->m_epoch].insert(std::make_pair(*(uint32_t*)output.data(), *preceipt));
						//LOG(m_log.info) << "add vrf output: sender=" << preceipt->from().hexPrefixed() << " output="<<*(uint32_t*)output.data() << " epoch="<<preceipt->epoch()
						//	<< " mc_last_summary_mci="<<mc_last_summary_mci;
						

						RLPStream receiptRLP;
						preceipt->streamRLP(receiptRLP);
						receipts.push_back(receiptRLP.out());
					}
					catch (std::exception const& _e)
					{
						std::cerr << _e.what() << std::endl;
						throw;
					}
				}
			}

			// set block stable
			{
				h256 receiptsRoot = dev::orderedTrieRoot(receipts);
				//mcp::stopwatch_guard sw("advance_stable_mci2_2");
				//m_last_stable_index_internal++;
				set_block_stable(timeout_tx_a, cache_a, dag_stable_block_hash, mci, mc_timestamp, mc_last_summary_mci, stable_timestamp, m_last_stable_index_internal, receiptsRoot);
			}
		}

		//LOG(m_log.debug) << "Mci: " << mci << ", stable_index: " << stable_index;
	}
}

void mcp::chain::set_block_stable(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & stable_block_hash, 
	uint64_t const & mci, uint64_t const & mc_timestamp, uint64_t const & mc_last_summary_mci, 
	uint64_t const & stable_timestamp, uint64_t const & stable_index, h256 receiptsRoot)
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
		if (previous_hash != mcp::block_hash(0))
		{
			previous_state = cache_a->block_state_get(transaction_a, previous_hash);
			assert_x(previous_state);
		}

		if (previous_hash != mcp::block_hash(0) && previous_state->status == mcp::block_status::fork)
			stable_block_state_copy->status = mcp::block_status::fork;
		else
		{
			mcp::block_hash old_successor_hash;
			bool old_successor_exists(!cache_a->successor_get(transaction_a, stable_block->root(), old_successor_hash));
			assert_x_msg(old_successor_exists, "hash: " + stable_block->root().hex());
			if (old_successor_hash != stable_block_hash)
			{
				std::shared_ptr<mcp::block_state> old_successor_state(cache_a->block_state_get(transaction_a, old_successor_hash));
				if (old_successor_state && old_successor_state->is_stable)
					stable_block_state_copy->status = mcp::block_status::fork;
				else
				{
					cache_a->successor_put(transaction_a, stable_block->root(), stable_block_hash);
				}
			}
		}
#pragma endregion

		if (stable_block_state_copy->status == mcp::block_status::ok)
		{
			dev::Address const & account(stable_block->from());
			mcp::dag_account_info info;
			m_store.dag_account_get(transaction_a, account, info);
			info.latest_stable_block = stable_block_hash;
			m_store.dag_account_put(transaction_a, account, info);
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

			//m_store.stable_block_put(transaction_a, stable_index, stable_block_hash);
			cache_a->block_number_put(transaction_a, stable_index, stable_block_hash);
			m_store.last_stable_index_put(transaction_a, stable_index);

#pragma region summary

			//previous summary hash
			mcp::summary_hash previous_summary_hash(0);
			if (stable_block->previous() != mcp::block_hash(0))
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

			mcp::summary_hash summary_hash = mcp::summary::gen_summary_hash(stable_block_hash, previous_summary_hash, p_summary_hashs, receiptsRoot, summary_skiplist,
				stable_block_state_copy->status, stable_block_state_copy->stable_index, stable_block_state_copy->mc_timestamp);

			cache_a->block_summary_put(transaction_a, stable_block_hash, summary_hash);
			m_store.summary_block_put(transaction_a, summary_hash, stable_block_hash);

#pragma endregion

			//add hash tree summary to delete list
			m_complete_store_notice(summary_hash);
		}

		//m_stable_blocks.push(stable_block);

		//timeout_tx_a.commit_if_timeout();
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

void mcp::chain::search_already_stable_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const &block_hash_a, uint64_t const &mci, std::map<uint64_t, std::set<mcp::block_hash>> &stable_block_level_and_hashs)
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

		if (!state->is_stable)
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

uint64_t mcp::chain::last_epoch()
{
	return m_last_epoch;
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
		std::shared_ptr<mcp::block_state> last_summary_state(cache_a->block_state_get(transaction_a, mc_block->last_summary_block()));
		assert_x(last_summary_state);
		assert_x(last_summary_state->is_stable);
		assert_x(last_summary_state->is_on_main_chain);
		assert_x(last_summary_state->main_chain_index);
		last_summary_mci = *last_summary_state->main_chain_index;

		mc_info_a = dev::eth::McInfo(mc_state->stable_index, *mc_state->main_chain_index, mc_state->mc_timestamp, last_summary_mci);
		return true;
	}
	else
		return false;
}

//void mcp::chain::notify_observers()
//{
//	while (!m_new_blocks.empty())
//	{
//		for (auto it = m_new_block_observer.begin(); it != m_new_block_observer.end(); it++)
//		{
//			(*it)(m_new_blocks.front());
//		}
//		m_new_blocks.pop();
//	}
//
//
//	while (!m_stable_blocks.empty())
//	{
//		for (auto it = m_stable_block_observer.begin(); it != m_stable_block_observer.end(); it++)
//		{
//			(*it)(m_stable_blocks.front());
//		}
//		m_stable_blocks.pop();
//	}
//
//	while (!m_stable_mcis.empty())
//	{
//		for (auto it = m_stable_mci_observer.begin(); it != m_stable_mci_observer.end(); it++)
//		{
//			(*it)(m_stable_mcis.front());
//		}
//		m_stable_mcis.pop();
//	}
//}

std::pair<u256, bool> mcp::chain::estimate_gas(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a,
	Address const& _from, u256 const& _value, Address const& _dest, bytes const& _data, int64_t const& _maxGas, u256 const& _gasPrice, dev::eth::McInfo const & mc_info_a, GasEstimationCallback const& _callback)
{
	try
    {
		int64_t upperBound = _maxGas;
		if (upperBound == Invalid256 || upperBound > mcp::block_max_gas)
			upperBound = mcp::block_max_gas;
		//if (_maxGas == 0)
		//	_maxGas = mcp::block_max_gas;

		int64_t lowerBound = Transaction::baseGasRequired(!_dest, &_data, dev::eth::EVMSchedule());

		///// if gas need used 50000,but input _maxGas ,it return zero ?
		//if(_maxGas < lowerBound)
		//	return std::make_pair(u256(), false);

        //int64_t upperBound = _maxGas;
        //if (upperBound == Invalid256 || upperBound > mcp::block_max_gas)
        //    upperBound = mcp::block_max_gas;

		// sichaoy: default gas price to be defined
         u256 gasPrice = _gasPrice == Invalid256 ? mcp::gas_price : _gasPrice;
        ExecutionResult er;
        ExecutionResult lastGood;
        bool good = false;

		do
		{
			int64_t mid = (lowerBound + upperBound) / 2;
			dev::eth::EnvInfo env(transaction_a, m_store, cache_a, mc_info_a, mcp::chainID());
			auto chain_ptr(shared_from_this());
			chain_state c_state(transaction_a, 0, m_store, chain_ptr, cache_a);
			u256 n = c_state.getNonce(_from);
			Transaction t;
			if (_dest)
				t = Transaction(_value, gasPrice, mid, _dest, _data, n);
			else
				t = Transaction(_value, gasPrice, mid, _data, n);
			t.setSignature(h256(0), h256(0), 0);
			t.forceSender(_from);
			c_state.ts = t;
			c_state.addBalance(_from, mid * _gasPrice + _value);
			std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = c_state.execute(env, Permanence::Reverted, t, dev::eth::OnOpFunc());
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

			if (_callback)
				_callback(GasEstimationProgress{ lowerBound, upperBound });
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
std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> mcp::chain::execute(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, Transaction const& _t, dev::eth::McInfo const & mc_info_a, Permanence _p, dev::eth::OnOpFunc const& _onOp)
{
	dev::eth::EnvInfo env(transaction_a, m_store, cache_a, mc_info_a, mcp::chainID());
	// sichaoy: startNonce = 0
	auto chain_ptr(shared_from_this());
	chain_state c_state(transaction_a, 0, m_store, chain_ptr, cache_a);

	//mcp::stopwatch_guard sw("advance_mc_stable_block3_1_1");
	return c_state.execute(env, _p, _t, _onOp);
}

mcp::json mcp::chain::traceTransaction(Executive& _e, Transaction const& _t, mcp::json const& _json)
{
	StandardTrace st;
	st.setShowMnemonics();
	st.setOptions(debugOptions(_json));
	_e.initialize(_t);
	if (!_e.execute())
		_e.go(st.onOp());
	_e.finalize();
	return st.jsonValue();
}

uint64_t mcp::chain::get_last_summary_mci(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block_cache> block_cache_a, uint64_t const & mci)
{
	mcp::block_hash mc_block_hash;
	bool exists(!m_store.main_chain_get(transaction_a, mci, mc_block_hash));
	assert_x(exists)

	uint64_t last_summary_mci(0);
	if (mc_block_hash != mcp::genesis::block_hash)
	{
		std::shared_ptr<mcp::block> mc_block(cache_a->block_get(transaction_a, mc_block_hash));
		LOG(m_log.info) << "[get_last_summary_mci] mci=" << mci << "mc_block_hash:" << mc_block_hash.hexPrefixed() << "last_summary_block: " << mc_block->last_summary_block().hexPrefixed();
		std::shared_ptr<mcp::block_state> last_summary_block_state(cache_a->block_state_get(transaction_a, mc_block->last_summary_block()));
		assert_x(last_summary_block_state
			&& last_summary_block_state->is_stable
			&& last_summary_block_state->is_on_main_chain
			&& last_summary_block_state->main_chain_index);

		last_summary_mci = *last_summary_block_state->main_chain_index;
	}
	else
	{
		last_summary_mci = 0;
	}
	return last_summary_mci;
}

bool mcp::chain::restart_not_need_send_approve(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::block_cache> block_cache_a, dev::Address account_a)
{
	bool ret = false;
	std::list<h256> hashs;
	m_store.epoch_approves_get(transaction_a, mcp::approve::calc_elect_epoch(m_last_summary_mci), hashs);
	for(auto hash : hashs)
	{
		std::shared_ptr<mcp::approve> ap = block_cache_a->approve_get(transaction_a, hash);
		if(ap->sender() == account_a){
			ret = true;
			break;
		}
	}
    LOG(m_log.info) << "[restart_not_need_send_approve] not need send = " << ret;
	return ret;
}

uint64_t mcp::chain::last_summary_mci()
{
	return m_last_summary_mci;
}

void mcp::chain::set_last_summary_mci(mcp::db::db_transaction & transaction_a, uint64_t const& mci)
{
	m_last_summary_mci = mci;
	uint64_t new_epoch = mcp::approve::calc_curr_epoch(m_last_summary_mci);
	if(m_last_epoch < new_epoch){
		m_last_epoch = new_epoch;
		m_store.last_epoch_put(transaction_a, m_last_epoch);
	}
}
