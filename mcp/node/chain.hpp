#pragma once

#include <mcp/node/message.hpp>
#include <mcp/core/timeout_db_transaction.hpp>
#include <mcp/node/process_block_cache.hpp>
#include <mcp/consensus/ledger.hpp>
#include <mcp/node/evm/Precompiled.h>
#include <memory>
#include <set>
#include <queue>
#include <mcp/node/chain_state.hpp>
#include <mcp/node/sync.hpp>
#include <mcp/node/approve_queue.hpp>
#include <mcp/core/approve_receipt.hpp>

namespace mcp
{
	struct GasEstimationProgress
	{
		u256 lowerBound;
		u256 upperBound;
	};
	using GasEstimationCallback = std::function<void(GasEstimationProgress const&)>;

	class chain : public std::enable_shared_from_this<mcp::chain>
	{
	public:
		chain(mcp::block_store& store_a, mcp::ledger& ledger_a);
		~chain();
		void init(bool & error_a, mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block_cache> block_cache_a);
		void stop();

		std::pair<u256, bool> estimate_gas(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a,
			Address const& _from, u256 const& _value, Address const& _dest, bytes const& _data, int64_t const& _maxGas, u256 const& _gasPrice, dev::eth::McInfo const & mc_info, GasEstimationCallback const& _callback = GasEstimationCallback());
		std::pair<ExecutionResult, dev::eth::TransactionReceipt> execute(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, Transaction const& _t, dev::eth::McInfo const & mc_info_a, Permanence _p, dev::eth::OnOpFunc const& _onOp);
		mcp::json traceTransaction(Executive& _e, Transaction const& _t, mcp::json const& _json);

		void save_dag_block(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a);
		void save_transaction(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::Transaction> t_a);
		void save_approve(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::approve> t_a);
		void try_advance(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a);

		void update_cache();
		uint64_t last_mci();
		uint64_t last_stable_mci();
		uint64_t min_retrievable_mci();
		uint64_t last_stable_index();
		uint64_t last_epoch();
		uint64_t last_summary_mci();

		bool get_mc_info_from_block_hash(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash hash_a, dev::eth::McInfo & mc_info_a);

		bool is_precompiled(Address const& account_a, uint64_t const& last_summary_mci_a) const
		{
			return m_precompiled.count(account_a) != 0 && last_summary_mci_a >= m_precompiled.at(account_a).startingMci();
		}
		bigint cost_of_precompiled(Address const& account_a, bytesConstRef in_a) const
		{ 
			return m_precompiled.at(account_a).cost(in_a);
		}
		std::pair<bool, bytes> execute_precompiled(Address const& account_a, bytesConstRef in_a) const
		{ 
			return m_precompiled.at(account_a).execute(in_a);
		}

		//void notify_observers();

		std::vector<uint64_t> cal_skip_list_mcis(uint64_t const &);

		//void set_ws_new_block_func(std::function<void(std::shared_ptr<mcp::block>)> new_block_observer_a)
		//{
		//	m_new_block_observer.push_back(new_block_observer_a);
		//}
		//void set_ws_stable_block_func(std::function<void(std::shared_ptr<mcp::block>)> stable_block_observer_a)
		//{
		//	m_stable_block_observer.push_back(stable_block_observer_a);
		//}
		//void set_ws_stable_mci_func(std::function<void(uint64_t const&)> stable_mci_observer_a)
		//{
		//	m_stable_mci_observer.push_back(stable_mci_observer_a);
		//}
		void set_complete_store_notice_func(std::function<void(mcp::block_hash)> func_a)
		{
			m_complete_store_notice = func_a;
		}
		uint64_t get_last_summary_mci(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block_cache> block_cache_a, uint64_t const & mci);
		
	private:
		void write_dag_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a);
		void find_main_chain_changes(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, mcp::block_hash const & best_free_block_hash, bool & is_mci_retreat, uint64_t & retreat_mci, uint64_t &retreat_level, std::list<mcp::block_hash>& new_mc_block_hashs);
		void update_mci(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, uint64_t const & retreat_mci, std::list<mcp::block_hash> const & new_mc_block_hashs);
		void update_latest_included_mci(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, bool const &is_mci_retreat, uint64_t const & retreat_mci, uint64_t const &retreat_level);
		void advance_stable_mci(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, uint64_t const & mci, mcp::block_hash const & block_hash_a, uint64_t & mc_last_summary_mci);
		void set_block_stable(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & stable_block_hash, uint64_t const & mci, uint64_t const & mc_timestamp, uint64_t const & mc_last_summary_mci, uint64_t const & stable_timestamp, uint64_t const & stable_index, h256 receiptsRoot);
		void search_stable_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & block_hash, uint64_t const & mci, std::map<uint64_t, std::set<mcp::block_hash>>& stable_block_hashs);
		void search_already_stable_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & block_hash, uint64_t const & mci, std::map<uint64_t, std::set<mcp::block_hash>>& stable_block_hashs);
		void switch_witness(mcp::db::db_transaction & transaction_a, uint64_t mc_last_summary_mci);
		void init_vrf_outputs(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a);
		void init_witness(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a);
		mcp::block_store m_store;
		mcp::ledger m_ledger;
		//std::list<std::function<void(std::shared_ptr<mcp::block>)> > m_new_block_observer;
		//std::queue<std::shared_ptr<mcp::block>> m_new_blocks;
		//std::list<std::function<void(std::shared_ptr<mcp::block>)> > m_stable_block_observer;
		//std::queue<std::shared_ptr<mcp::block>> m_stable_blocks;
		//std::list<std::function<void(uint64_t const&)> > m_stable_mci_observer;
		//std::queue<uint64_t> m_stable_mcis;

		//completed write to store notice sync,if need
		std::function<void(mcp::block_hash)> m_complete_store_notice;

		bool m_stopped;
		uint64_t m_last_mci = 0;
		uint64_t m_last_mci_internal = 0;
		uint64_t m_last_stable_mci = 0;
		uint64_t m_last_stable_mci_internal = 0;
		uint64_t m_last_summary_mci = 0;
		uint64_t m_last_summary_mci_internal = 0;
		uint64_t m_min_retrievable_mci = 0;
		uint64_t m_min_retrievable_mci_internal = 0;
		uint64_t m_last_stable_index = 0;
		uint64_t m_last_stable_index_internal = 0;
		uint64_t m_last_epoch = 0;
		
		mcp::advance_info m_advance_info;

		std::unordered_map<Address, dev::eth::PrecompiledContract> m_precompiled;

		std::map<uint32_t, dev::ApproveReceipt> vrf_outputs;

        mcp::log m_log = { mcp::log("node") };
	};
}