#include "chain.hpp"
#include "Block.hpp"
#include <mcp/core/genesis.hpp>
#include <mcp/core/param.hpp>
#include <mcp/common/stopwatch.hpp>
#include <mcp/common/Exceptions.h>
#include <mcp/node/evm/Executive.hpp>
#include <libdevcore/TrieHash.h>
#include <libdevcore/CommonJS.h>
#include <mcp/core/config.hpp>
#include <mcp/core/contract.hpp>
#include <mcp/node/approve_queue.hpp>
#include <mcp/consensus/ledger.hpp>

#include <queue>

mcp::chain::chain(mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a) :
	m_stateDB(store_a.db()),
	m_store(store_a),
	m_cache(cache_a),
	m_stopped(false),
	m_postSeal()
{
}

mcp::chain::~chain()
{
}

void mcp::chain::init(bool & error_a, mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	m_sealEngine.reset(mcp::param::createSealEngine());
	try
	{
		genesisBlock(timeout_tx_a, cache_a);
	}
	catch (const std::exception & e)
	{
		LOG(m_log.error) << boost::str(boost::format("Init genesis error: %1%") % e.what());
		timeout_tx_a.rollback();
		error_a = true;
		return;
	}
	LOG(m_log.info) << "Genesis block:" << mcp::genesis::block_hash.hex();

	mcp::db::db_transaction& transaction(timeout_tx_a.get_transaction());
	///get init data
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
	init_vrf_outputs(transaction);
	InitWork(transaction, cache_a);
	m_last_stable_epoch = mcp::epoch(m_last_stable_mci_internal);

	{
		h256 _h;
		assert_x(!m_store.stable_block_get(transaction, m_last_stable_index_internal, _h));
		std::shared_ptr<mcp::block_state> state(m_store.block_state_get(transaction, _h));
		assert_x(state && state->is_stable && state->is_on_main_chain && state->main_chain_index);
		m_lastStateRoot = state->m_stateRoot;

		auto _block = m_store.block_get(transaction, _h);
		assert_x(_block);
		dev::eth::McInfo mc_info(state->stable_index,
			*state->main_chain_index,
			state->mc_timestamp,
			_block->from());

		m_postSeal = Block(*this, m_stateDB, m_lastStateRoot, mc_info);
	}

	update_cache();
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

				///search best free block by witnessed_level desc, level asc, block hash asc
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
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain save block error: " << e.what();
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
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain save transaction error: " << e.what();
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
				m_store.epoch_approves_put(transaction, mcp::epoch_approves_key(t_a->epoch(), t_a->sha3()));
				m_store.approve_unstable_count_add(transaction);
				m_store.approve_count_add(transaction);
				cache_a->approve_del_from_queue(hash);///mark as clear,It will be really cleaned up after commit event
			}
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain save approve error: " << e.what();
			throw;
		}
	}
}

void mcp::chain::UpdateCommittee(mcp::db::db_transaction& transaction_a, Epoch const& epoch){
	///send approve at #0, this approve stable at #1, used witness at #2
	Epoch vrfepoch = epoch - 1;
	Epoch useepoch = epoch + 1;
	mcp::witness_param p_param = mcp::param::witness_param(transaction_a, epoch);
	assert_x(p_param.witness_list.size() && p_param.witness_count);///must be existed

	if (!vrf_outputs.count(vrfepoch))/// used the previous epoch
		mcp::param::add_witness_param(transaction_a, useepoch, p_param);

	/// witness redeemed,but approve has been send. Abstain from election.
	for (auto it = vrf_outputs[vrfepoch].begin(); it != vrf_outputs[vrfepoch].end();)
	{
		if (!IsStakingList(transaction_a, epoch, it->second.from()))
			vrf_outputs[vrfepoch].erase(it++);
		else
			it++;
	}

	if (vrf_outputs[vrfepoch].size() < p_param.witness_count)
	{
		if (vrf_outputs.count(vrfepoch))
			LOG(m_log.debug) << "Not switch witness_list because elector's number is too short: " << vrf_outputs[vrfepoch].size() << " ,epoch:" << vrfepoch;
		else
			LOG(m_log.debug) << "Not switch witness_list because elector not found.epoch:" << vrfepoch;
		mcp::param::add_witness_param(transaction_a, useepoch, p_param);
	}
	else
	{
		p_param.witness_list.clear();
		auto it = vrf_outputs[vrfepoch].rbegin();
		for (int i = 0; i<p_param.witness_count; i++) {
			p_param.witness_list.insert(it->second.from());
			it++;
		}
		assert_x(p_param.witness_list.size() == p_param.witness_count);
	}
	mcp::param::add_witness_param(transaction_a, useepoch, p_param);
	vrf_outputs.erase(vrfepoch);
}

void mcp::chain::init_vrf_outputs(mcp::db::db_transaction & transaction_a)
{
	std::list<h256> hashs;
	auto epoch = mcp::epoch(m_last_stable_mci_internal);
	m_store.epoch_approves_get(transaction_a, epoch, hashs);
	for(auto hash : hashs)
	{
		auto approve_receipt = m_store.approve_receipt_get(transaction_a, hash);
		if (approve_receipt && approve_receipt->statusCode()) ///approve is not the driving force of mci, so maybe exist approve is linked but not stable.
		{
			vrf_outputs[epoch].insert(std::make_pair(approve_receipt->output(), *approve_receipt));
			LOG(m_log.debug) << "[init_vrf_outputs] ap epoch=" << epoch 
				<< ",address:" << approve_receipt->from().hexPrefixed()
				<< ",outputs:" << approve_receipt->output().hexPrefixed();
		}
	}
}

void mcp::chain::InitWork(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	uint64_t start = mcp::epoch(m_last_stable_mci_internal) * mcp::epoch_period + 1;///include start

	mcp::block_hash mc_stable_hash;
	bool mc_stable_hash_error(m_store.main_chain_get(transaction_a, m_last_stable_mci_internal, mc_stable_hash));
	assert_x(!mc_stable_hash_error);
	h256Hash hashes;
	std::queue<mcp::block_hash> queue;
	queue.push(mc_stable_hash);
	while (!queue.empty())
	{
		mcp::block_hash block_hash(queue.front());
		queue.pop();

		if (block_hash == mcp::genesis::block_hash)
			continue;
		std::shared_ptr<mcp::block_state> state(cache_a->block_state_get(transaction_a, block_hash));
		assert_x(state && state->is_stable);
		if (state->main_chain_index < start)///previous epoch
			continue;

		std::shared_ptr<mcp::block> block(cache_a->block_get(transaction_a, block_hash));
		m_statistics.Insert(block->from(), state->is_on_main_chain);
		for (mcp::block_hash const &pblock_hash : block->parents())
		{
			if (hashes.insert(pblock_hash).second)///Filter duplicate.
				queue.push(pblock_hash);
		}
	}
}

dev::bytes mcp::chain::epochRewardsData(mcp::db::db_transaction& transaction_a, Epoch const& epoch, MainInfo const& _mInfo) const
{
	uint32_t total = 0;
	std::map<dev::Address, uint32_t> details;
	for (auto const& it : m_statistics.values())
	{
		///Collect statistics about the epoch. Ignore the address of the last epoch.
		if (!mcp::param::is_witness(transaction_a, epoch - 1, it.first))
		{
			continue;
		}

		uint32_t t = (it.second.OnMci * _mInfo.onMci) + (it.second.NotOnMci * _mInfo.notOnMci);
		details[it.first] = t;
		total += t;
	}
	std::map<dev::Address, u256> _v;
	u256 precision = (u256)1e13;
	u256 _precisionAmount = _mInfo.amount / precision;

	for (auto const& it : details)
	{
		u256 a = it.second * _precisionAmount / total;
		_v.insert(std::make_pair(it.first, a * precision));
	}
	return MainCaller.PackDistributeRewards(_v);
}

void mcp::chain::EpochFinalize(mcp::db::db_transaction& transaction_a, Epoch _epoch)
{
	UpdateCommittee(transaction_a, _epoch);
	m_statistics.clear();
	m_last_stable_epoch = _epoch;
}

bool mcp::chain::IsEpochFinalized(uint64_t const & mci) const
{
	Epoch epoch = mcp::epoch(mci);
	return epoch && epoch != m_last_stable_epoch;///epoch completed
}

void mcp::chain::try_advance(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	while (!m_stopped && ((dev::h64::Arith) m_advance_info.mci).convert_to<uint64_t>() > m_last_stable_mci_internal)
	{
		m_last_stable_mci_internal++;
		advance_stable_mci(timeout_tx_a, cache_a, m_last_stable_mci_internal, m_advance_info.witness_block);
		LOG(m_log.debug) << "[try_advance] m_last_stable_mci_internal=" << m_last_stable_mci_internal;

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

			/// send approve if witness
			m_onMciStable(m_last_stable_mci_internal);
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Chain update last stable mci error: " << e.what();
			throw;
		}

	}
}

bool mcp::chain::IsStakingList(mcp::db::db_transaction & _transaction, Epoch const& _epoch, dev::Address const & _address) const
{
	auto sl = m_cache->GetStakingList(_transaction, _epoch);
	return sl.count(_address);
}


void mcp::chain::genesisBlock(mcp::timeout_db_transaction& timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a)
{
	mcp::db::db_transaction& transaction(timeout_tx_a.get_transaction());
	std::pair<mcp::block, mcp::Transactions> gnesis = mcp::genesis::try_initialize();
	/// 0: genesis transaction.
	/// 1: transfer to the account that deployed system contract.
	/// 2: Admin contract transaction.
	/// 3: Deposit contract transaction.
	/// 4: Proxy contract transaction.
	/// 5: staking init witness transaction.
	mcp::block block = gnesis.first;

	mcp::block_hash genesis_hash;
	bool exists(!m_store.genesis_hash_get(transaction, genesis_hash));
	if (exists)
	{
		if (genesis_hash != mcp::genesis::block_hash)
			throw std::runtime_error("genesis block changed");

		return;	///initialized
	}

	m_store.genesis_hash_put(transaction, mcp::genesis::block_hash);
	m_store.block_put(transaction, mcp::genesis::block_hash, block);

	//block state
	mcp::block_state block_state;

	block_state.is_free = true;
	block_state.level = 0;
	block_state.is_stable = true;
	block_state.status = mcp::block_status::ok;
	block_state.main_chain_index = 0;
	block_state.mc_timestamp = block.exec_timestamp();
	block_state.stable_timestamp = block.exec_timestamp();

	block_state.is_on_main_chain = true;
	block_state.witnessed_level = block_state.level;
	block_state.best_parent.clear();
	block_state.earliest_included_mc_index = boost::none;
	block_state.latest_included_mc_index = boost::none;
	block_state.bp_included_mc_index = boost::none;
	block_state.earliest_bp_included_mc_index = boost::none;
	block_state.latest_bp_included_mc_index = boost::none;
	m_store.block_state_put(transaction, mcp::genesis::block_hash, block_state);

	//mci
	m_store.main_chain_put(transaction, *block_state.main_chain_index, mcp::genesis::block_hash);

	//stable index
	m_store.stable_block_put(transaction, 0, mcp::genesis::block_hash);
	m_store.last_stable_index_put(transaction, 0);

	//free
	m_store.dag_free_put(transaction, mcp::free_key(block_state.witnessed_level, block_state.level, mcp::genesis::block_hash));

	//add dag account info
	mcp::dag_account_info info;
	m_store.dag_account_get(transaction, block.from(), info);
	info.latest_stable_block = mcp::genesis::block_hash;
	m_store.dag_account_put(transaction, block.from(), info);
	//genesis maybe is witness too.
	m_store.successor_put(transaction, mcp::block_hash(block.from()), info.latest_stable_block);

	//genesis account state
	Transaction ts = gnesis.second[0];
	mcp::account_state to_state(0, ts.value());
	to_state.incNonce();//nonce + 1 Stored for the next nonce
	m_store.account_nonce_put(transaction, ts.sender(), ts.nonce());
	m_store.transaction_put(transaction, ts.sha3(), ts);
	dev::eth::LocalTransactionReceipt const _lreceipt(dev::eth::TransactionReceipt(true, 0, mcp::log_entries()),
		mcp::genesis::block_hash, 0, 0);
	m_store.transaction_receipt_put(transaction, ts.sha3(), _lreceipt);

	h256 receiptsRoot = dev::orderedTrieRoot(std::vector<bytes>{_lreceipt.rlp()});
	mcp::summary_hash summary_hash = mcp::summary::gen_summary_hash(mcp::genesis::block_hash, dev::h256(), std::list<mcp::summary_hash>(), receiptsRoot, h256Set(),
		block_state.status, block_state.stable_index, block_state.mc_timestamp);

	LOG(m_log.info) << "Genesis Summary:" << summary_hash.hex();

	m_store.block_summary_put(transaction, mcp::genesis::block_hash, summary_hash);
	m_store.summary_block_put(transaction, summary_hash, mcp::genesis::block_hash);

	/// Init precompiled contract account
	AccountMap precompiled_accounts;
	precompiled_accounts[ts.sender()] = std::make_shared<mcp::account_state>(to_state);
	for (unsigned i = 1; i <= 8; ++i)
	{
		precompiled_accounts[Address(i)] = std::make_shared<mcp::account_state>(0, 0);
	}

	dev::eth::McInfo _mc(0, 0, block_state.stable_timestamp, dev::ZeroAddress);
	Block ret(*this, m_stateDB, _mc, BaseState::Empty);
	mcp::commit(precompiled_accounts, ret.mutableState().m_state);        // bit horrible. maybe consider a better way of constructing it?

	///init system contract
	dev::eth::EnvInfo env(_mc, mcp::chainID());
	///init staking
	for (size_t i = 1; i < gnesis.second.size(); i++)
	{
		auto _t(gnesis.second[i]);
		std::pair<ExecutionResult, dev::eth::TransactionReceipt> result = ret.mutableState().execute(env, *m_sealEngine, Permanence::Committed, _t);
		assert_x(result.second.statusCode());
		cache_a->transaction_put(transaction, std::make_shared<Transaction>(_t));
		cache_a->account_nonce_put(transaction, _t.sender(), _t.nonce());
		auto _lreceipt = std::make_shared<dev::eth::LocalTransactionReceipt>(result.second, 
			mcp::genesis::block_hash, i, i);
		cache_a->transaction_receipt_put(transaction, _t.sha3(), _lreceipt);
	}
	block_state.m_receiptsRoot = receiptsRoot;
	block_state.m_stateRoot = ret.mutableState().rootHash();
	m_lastStateRoot = block_state.m_stateRoot;
	m_store.block_state_put(transaction, mcp::genesis::block_hash, block_state);

	/// set genesis epoch staking list
	StakingList _sl = ret.getStakingList();
	m_cache->PutStakingList(transaction, 0, _sl);

	ret.mutableState().db().commit();
	ret.cleanup();
	m_postSeal = ret;
	timeout_tx_a.commit_and_continue();///GetWitnesses use new database transaction
}

void mcp::chain::write_dag_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a)
{
	mcp::block_hash const &block_hash = block_a->hash();

	///save block, need put first
	cache_a->block_put(transaction_a, block_hash, block_a);

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
			///make a copy for not changing value in cache
			std::shared_ptr<mcp::block_state> pblock_state_copy(std::make_shared<mcp::block_state>(*pblock_state));
			pblock_state_copy->is_free = false;
			cache_a->block_state_put(transaction_a, pblock_hash, pblock_state_copy);

			///remove parent block from dag free
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
	mcp::witness_param const & w_param(mcp::param::witness_param(transaction_a,mcp::epoch(last_summary_mci)));

	///best parent
	mcp::block_hash best_pblock_hash(Ledger.determine_best_parent(transaction_a, cache_a, block_a->parents()));
	///level
	level = Ledger.calc_level(transaction_a, cache_a, best_pblock_hash);
	///witnessed level
	uint64_t witnessed_level(Ledger.calc_witnessed_level(w_param, level));

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

		///get previous best parent block
		prev_mc_block_hash = prev_mc_block_state->best_parent;
		prev_mc_block_state = cache_a->block_state_get(transaction_a, prev_mc_block_hash);
		assert_x(prev_mc_block_state);
	}
	assert_x(prev_mc_block_state->main_chain_index);

	retreat_mci = *prev_mc_block_state->main_chain_index;
	retreat_level = prev_mc_block_state->level;
	is_mci_retreat = retreat_mci < old_last_mci;

	///check stable mci not retreat
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
		///make a copy for not changing value in cache
		std::shared_ptr<mcp::block_state> old_mci_block_state_copy(std::make_shared<mcp::block_state>(*old_mci_block_state_in_cache));
		assert_x(!old_mci_block_state_copy->is_stable);
		if (old_mci_block_state_copy->is_on_main_chain)
			old_mci_block_state_copy->is_on_main_chain = false;
		old_mci_block_state_copy->main_chain_index = boost::none;
		cache_a->block_state_put(transaction_a, old_last_mc_block_hash, old_mci_block_state_copy);

		///delete old main chian block
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
		///make a copy for not changing value in cache
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

		///get from unstable blocks where main_chain_index > last_main_chain_index or main_chain_index == null
		auto end = to_update_hashs.end();
		for (auto u_it(to_update_hashs.begin()); u_it != end; u_it++)
		{
			///mcp::stopwatch_guard sw3("update_latest_included_mci3");
			std::unordered_set<mcp::block_hash> & u_hashs(u_it->second);
			for (auto it(u_hashs.begin()); it != u_hashs.end(); it++)
			{
				mcp::block_hash const &u_block_hash(*it);
				if (u_block_hash == block_hash)
					is_curr_block_set = true;

				std::shared_ptr<mcp::block_state> u_block_state_in_cache(cache_a->block_state_get(transaction_a, u_block_hash));
				assert_x(u_block_state_in_cache);
				///make a copy for not changing value in cache
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

						///best parent limci
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
							///max_bp_limci
							if (*max_bp_limci < *u_pblock_state->latest_bp_included_mc_index)
								max_bp_limci = u_pblock_state->latest_bp_included_mc_index;

							assert_x(u_pblock_state->earliest_bp_included_mc_index);
							///min_bp_limci
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

	std::shared_ptr<mcp::block_state> last_summary_state(cache_a->block_state_get(transaction_a, mc_stable_block->last_summary_block()));
	assert_x(last_summary_state);
	assert_x(last_summary_state->is_stable);
	assert_x(last_summary_state->is_on_main_chain);
	assert_x(last_summary_state->main_chain_index);
	uint64_t const & mc_last_summary_mci = *last_summary_state->main_chain_index;

	auto block_to_advance = cache_a->block_get(transaction_a, block_hash_a);
	uint64_t const & stable_timestamp = block_to_advance->exec_timestamp();
	uint64_t const & mc_timestamp = mc_stable_block->exec_timestamp();

	bool finalized = IsEpochFinalized(mci);
	size_t _dagCount = 0;
	for (auto iter_p(dag_stable_block_hashs.begin()); iter_p != dag_stable_block_hashs.end(); iter_p++)
	{
		_dagCount++;
		size_t _hashsCount = 0;
		std::set<mcp::block_hash> const & hashs(iter_p->second);
		for (auto iter(hashs.begin()); iter != hashs.end(); iter++)
		{
			_hashsCount++;
			bool tmpFinalized = finalized && _dagCount == dag_stable_block_hashs.size() && _hashsCount == hashs.size();
			
			mcp::block_hash const & dag_stable_block_hash(*iter);
			m_last_stable_index_internal++;
			///handle dag stable block 
			std::shared_ptr<mcp::block> dag_stable_block = cache_a->block_get(transaction_a, dag_stable_block_hash);
			assert_x(dag_stable_block);
			VerifiedBlockRef _block{ dag_stable_block };
			dev::eth::McInfo mc_info(m_last_stable_index_internal, mci, mc_timestamp, dag_stable_block->from());
			Block s(*this, m_stateDB, m_lastStateRoot, mc_info);
			auto _receiptRoot = import(s, transaction_a, cache_a, _block, mci, tmpFinalized);
			/// set block stable
			set_block_stable(timeout_tx_a, cache_a, dag_stable_block_hash, mci, mc_timestamp, mc_last_summary_mci, stable_timestamp, m_last_stable_index_internal,
				_receiptRoot, s.rootHash(), s.logBloom());
			m_lastStateRoot = s.rootHash();
			s.cleanup();
			m_postSeal = s;
		}
	}
	if (finalized)
		EpochFinalize(transaction_a, mcp::epoch(mci));
}

dev::h256 mcp::chain::import(Block& _s, mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, VerifiedBlockRef& _block, uint64_t const& _mci, bool _epochFinalized)
{
	///handle light stable block 
	///account A : b2, b3, b4, b5
	///account B : b1, b2, b3
	///account c : b2, b3
	std::vector<std::shared_ptr<dev::eth::TransactionReceipt>> _local;
	h256s const& _links = _block.info->links();
	for (h256 const& _th : _links)
	{
		auto receipt = cache_a->transaction_receipt_get(transaction_a, _th);
		_local.push_back(receipt);
		if (receipt)/// transaction maybe processed yet,but summary need used receipt even if it has been processed.
			continue;

		auto _t = cache_a->transaction_get(transaction_a, _th);
		_block.transactions.push_back(_t);
	}
	
	auto tdIncrease = _s.enactOn(_block, *this);
	std::vector<bytes> receipts;
	unsigned _execIndex = 0;
	for (unsigned i = 0; i < _links.size(); ++i)
	{
		if (_local[i])
			receipts.push_back(_local[i]->rlp());
		else
		{
			auto _lre = std::make_shared<dev::eth::LocalTransactionReceipt>(_s.receipt(_execIndex),
				_block.info->hash(), i, _execIndex);
			_execIndex++;
			/// commit transaction receipt
			cache_a->transaction_receipt_put(transaction_a, _links[i], _lre);
			receipts.push_back(_lre->rlp());
		}
	}
	/// exec transaction can reduce, if two or more block linked a transaction,reduce once.
	if (_execIndex)
		m_store.transaction_unstable_count_reduce(transaction_a, _execIndex);

	/// applying the earned rewards transaction.
	if (_epochFinalized)
	{
		Epoch _epoch = mcp::epoch(_mci);
		StakingList _sl = _s.getStakingList();
		m_cache->PutStakingList(transaction_a, _epoch, _sl);
		mcp::MainInfo _mi = _s.getMainInfo();
		m_statistics.Insert(_block.info->from(), true);
		dev::bytes _data = epochRewardsData(transaction_a, _epoch, _mi);
		auto ret = _s.ApplyWorkTransaction(_data);
		cache_a->transaction_put(transaction_a, std::make_shared<Transaction>(ret.first));
		cache_a->account_nonce_put(transaction_a, ret.first.sender(), ret.first.nonce());
		///receipt
		auto _lre = std::make_shared<dev::eth::LocalTransactionReceipt>(ret.second,
			_block.info->hash(), _links.size(), _execIndex);
		cache_a->transaction_receipt_put(transaction_a, ret.first.sha3(), _lre);
		m_store.epoch_work_transaction_put(transaction_a, _epoch - 1, ret.first.sha3());
		m_tq->makeQueue(std::make_shared<Transaction>(ret.first));///may be transactions

		if (mcp::param::get()->IsOIP6(_mci))
		{
			receipts.push_back(ret.second.rlp());
		}
	}

	///handle approve stable block 
	for (h256 const& _ah : _block.info->approves())
	{
		auto receipt = cache_a->approve_receipt_get(transaction_a, _ah);
		if (receipt)/// approve maybe processed yet,but summary need used receipt even if it has been processed.
		{
			receipts.push_back(receipt->rlp());
			continue;
		}

		auto ap = cache_a->approve_get(transaction_a, _ah);
		assert_x(ap);
		/// exec approves
		try 
		{
			/// exec approve can reduce, if two or more block linked a approve,reduce once.
			m_store.approve_unstable_count_reduce(transaction_a);

			if (ap->outputs() == h256(0))/// reboot system. approve read from db,but not cache outputs
			{
				mcp::block_hash hash;
				if (ap->epoch() <= 1) {
					hash = mcp::genesis::block_hash;
				}
				else {
					bool exists(!m_store.main_chain_get(transaction_a, (ap->epoch() - 1) * epoch_period, hash));
					assert_x(exists);
				}
				ap->vrf_verify(hash);///cached outputs.must successed.
			}
			bool apStatus = false;
			if (IsStakingList(transaction_a, ap->epoch(), ap->sender()))///staking completed.
				apStatus = true;
			std::shared_ptr<dev::ApproveReceipt> preceipt = std::make_shared<dev::ApproveReceipt>(apStatus, ap->sender(), ap->outputs());
			cache_a->approve_receipt_put(transaction_a, _ah, preceipt);

			///the approve which is smaller than the current epoch, is not eligible for election.
			///Bigger than the present is problematic
			if (ap->epoch() == epoch(_mci) && apStatus)
			{
				vrf_outputs[ap->epoch()].insert(std::make_pair(ap->outputs(), *preceipt));
			}

			receipts.push_back(preceipt->rlp());
		}
		catch (std::exception const& _e)
		{
			std::cerr << _e.what() << std::endl;
			throw;
		}
	}

	return dev::orderedTrieRoot(receipts);
}

void mcp::chain::set_block_stable(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & stable_block_hash, 
	uint64_t const & mci, uint64_t const & mc_timestamp, uint64_t const & mc_last_summary_mci, 
	uint64_t const & stable_timestamp, uint64_t const & stable_index, 
	h256 const& _receiptsRoot, h256 const& _stateRoot, log_bloom const& _logBloom)
{
	mcp::db::db_transaction & transaction_a(timeout_tx_a.get_transaction());
	try
	{
		std::shared_ptr<mcp::block> stable_block(cache_a->block_get(transaction_a, stable_block_hash));
		assert_x(stable_block);

		std::shared_ptr<mcp::block_state> stable_block_state_in_cache(cache_a->block_state_get(transaction_a, stable_block_hash));
		assert_x(stable_block_state_in_cache);
		///make a copy for not changing value in cache
		std::shared_ptr<mcp::block_state> stable_block_state_copy(std::make_shared<mcp::block_state>(*stable_block_state_in_cache));

		///has set
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
			stable_block_state_copy->m_receiptsRoot = _receiptsRoot;
			stable_block_state_copy->m_stateRoot = _stateRoot;
			stable_block_state_copy->m_logBloom = _logBloom;
			cache_a->block_state_put(transaction_a, stable_block_hash, stable_block_state_copy);

			//m_store.stable_block_put(transaction_a, stable_index, stable_block_hash);
			cache_a->block_number_put(transaction_a, stable_index, stable_block_hash);
			m_store.last_stable_index_put(transaction_a, stable_index);

#pragma region summary

			///previous summary hash
			mcp::summary_hash previous_summary_hash(0);
			if (stable_block->previous() != mcp::block_hash(0))
			{
				bool previous_summary_hash_error(cache_a->block_summary_get(transaction_a, stable_block->previous(), previous_summary_hash));
				assert_x(!previous_summary_hash_error);
			}

			///parent summary hashs
			std::list<mcp::summary_hash> p_summary_hashs;
			for (mcp::block_hash const & pblock_hash : stable_block->parents())
			{
				mcp::summary_hash p_summary_hash;
				bool p_summary_hash_error(cache_a->block_summary_get(transaction_a, pblock_hash, p_summary_hash));
				assert_x(!p_summary_hash_error);

				p_summary_hashs.push_back(p_summary_hash);
			}

			///skip list
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

			mcp::summary_hash summary_hash;
			if (mcp::param::get()->IsOIP6(mci))
				summary_hash = mcp::summary::gen_summary_hash(stable_block_hash, previous_summary_hash, p_summary_hashs, _receiptsRoot, summary_skiplist,
					stable_block_state_copy->status, stable_block_state_copy->stable_index, stable_block_state_copy->mc_timestamp,
					mci, _stateRoot, _logBloom);
			else
				summary_hash = mcp::summary::gen_summary_hash(stable_block_hash, previous_summary_hash, p_summary_hashs, _receiptsRoot, summary_skiplist,
					stable_block_state_copy->status, stable_block_state_copy->stable_index, stable_block_state_copy->mc_timestamp);

			cache_a->block_summary_put(transaction_a, stable_block_hash, summary_hash);
			m_store.summary_block_put(transaction_a, summary_hash, stable_block_hash);

#pragma endregion

			///Statistical witness block
			///if fork, this block was sent much later than the other nodes, invalid.
			m_statistics.Insert(stable_block->from(), stable_block_state_copy->is_on_main_chain);
		}
	}
	catch (std::exception const & e)
	{
		LOG(m_log.error) << "Chain Set block stable error: " << e.what();
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

mcp::Epoch mcp::chain::last_epoch()
{
	return mcp::epoch(m_last_mci);
}

mcp::Epoch mcp::chain::last_stable_epoch()
{
	return mcp::epoch(m_last_stable_mci);
}

std::vector<uint64_t> mcp::cal_skip_list_mcis(uint64_t const& mci)
{
	std::vector<uint64_t> skip_list_mcis;
	if (mci == 0) ///return empty
		return skip_list_mcis;
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
