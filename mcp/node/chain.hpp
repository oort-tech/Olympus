#pragma once

#include <mcp/node/message.hpp>
#include <mcp/node/process_block_cache.hpp>
#include <mcp/core/Precompiled.h>
#include <mcp/core/SealEngine.h>
#include <memory>
#include <set>
#include <queue>
#include <mcp/node/chain_state.hpp>
#include <mcp/core/approve_receipt.hpp>
#include "Block.hpp"

namespace mcp
{
	std::vector<uint64_t> cal_skip_list_mcis(uint64_t const& mci);

	class Statistics
	{
		struct Details
		{
			int OnMci = 0;
			int NotOnMci = 0;
		};
	public:
		void Insert(dev::Address const& _a, bool const& _onMci)
		{
			if (!m.count(_a))
				m[_a] = Details();

			if (_onMci)
				m[_a].OnMci++;
			else
				m[_a].NotOnMci++;
		}
		std::map<dev::Address, Details> values() const
		{
			return m;
		}

		void clear()
		{
			m.clear();
		}
	private:
		std::map<dev::Address, Details> m;
	};

	struct ImportBlockResult
	{
		h256	receiptsRoot;
		h256	stateRoot;
		log_bloom	logBloom;
	};

	class witness;
	class ApproveQueue;
	class chain : public std::enable_shared_from_this<mcp::chain>
	{
	public:
		chain(mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a);
		~chain();
		void init(bool & error_a, mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a/*, std::shared_ptr<mcp::block_cache> block_cache_a*/);
		void stop();

		void set_TQ(std::shared_ptr<mcp::TransactionQueue> tq) { m_tq = tq; }

		SealEngineFace* sealEngine() const { return m_sealEngine.get(); }

		void save_dag_block(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a);
		void save_transaction(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::Transaction> t_a);
		void save_approve(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::approve> t_a);
		void try_advance(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a);
		bool IsStakingList(mcp::db::db_transaction & _transaction, Epoch const& _epoch, dev::Address const& _address) const;

		void update_cache();
		uint64_t last_mci();
		uint64_t last_stable_mci();
		uint64_t min_retrievable_mci();
		uint64_t last_stable_index();

		/// Register a handler that will be called once mci stabled
		void onMciStable(std::function<void(uint64_t const&)> const& _t) { m_onMciStable.add(_t); }

		Epoch last_epoch();
		Epoch last_stable_epoch();

		/// Get the hash for a given block's number.
		h256 numberHash(unsigned _i) const { h256 _h; auto _t = m_store.create_transaction(); m_cache->block_number_get(_t, _i, _h); return _h; }

		uint64_t number() const { return m_last_stable_index; }

		h256 lastStateRoot() { return m_lastStateRoot; }
		/// Returns the LatestBlock.
		Block postSeal() const { /*ReadGuard l(x_postSeal);*/ return m_postSeal; }
	private:
		/// Get a pre-made genesis State object.
		void genesisBlock(mcp::timeout_db_transaction& timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a);
		void write_dag_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a);
		void find_main_chain_changes(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, mcp::block_hash const & best_free_block_hash, bool & is_mci_retreat, uint64_t & retreat_mci, uint64_t &retreat_level, std::list<mcp::block_hash>& new_mc_block_hashs);
		void update_mci(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, uint64_t const & retreat_mci, std::list<mcp::block_hash> const & new_mc_block_hashs);
		void update_latest_included_mci(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, std::shared_ptr<mcp::block> block_a, bool const &is_mci_retreat, uint64_t const & retreat_mci, uint64_t const &retreat_level);
		void advance_stable_mci(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, uint64_t const & mci, mcp::block_hash const & block_hash_a);
		dev::bytes epochRewardsData(mcp::db::db_transaction& transaction_a, Epoch const& epoch, MainInfo const& _mInfo) const;
		ImportBlockResult import(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, VerifiedBlockRef& _block, dev::eth::McInfo const& _mc, bool _epochFinalized);
		
		void set_block_stable(mcp::timeout_db_transaction & timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & stable_block_hash, uint64_t const & mci, uint64_t const & mc_timestamp, uint64_t const & mc_last_summary_mci, uint64_t const & stable_timestamp, uint64_t const & stable_index, ImportBlockResult const& importResult);
		void search_stable_block(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const & block_hash, uint64_t const & mci, std::map<uint64_t, std::set<mcp::block_hash>>& stable_block_hashs);
		void UpdateCommittee(mcp::db::db_transaction& transaction_a, Epoch const& epoch);
		void init_vrf_outputs(mcp::db::db_transaction & transaction_a);
		void InitWork(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::process_block_cache> cache_a);
		void EpochFinalize(mcp::db::db_transaction& transaction_a, Epoch _epoch);
		bool IsEpochFinalized(uint64_t const& mci) const;

		dev::OverlayDB m_stateDB;
		mcp::block_store m_store;
		std::shared_ptr<mcp::block_cache> m_cache;
		std::shared_ptr<mcp::TransactionQueue> m_tq;
		std::shared_ptr<mcp::SealEngineFace> m_sealEngine;   // consider shared_ptr.

		bool m_stopped;
		uint64_t m_last_mci = 0;
		uint64_t m_last_mci_internal = 0;
		uint64_t m_last_stable_mci = 0;
		uint64_t m_last_stable_mci_internal = 0;
		uint64_t m_min_retrievable_mci = 0;
		uint64_t m_min_retrievable_mci_internal = 0;
		uint64_t m_last_stable_index = 0;
		uint64_t m_last_stable_index_internal = 0;
		Epoch m_last_stable_epoch;
		
		mcp::advance_info m_advance_info;

		std::map<Epoch, std::map<h256, dev::ApproveReceipt>> vrf_outputs;
		Signal<uint64_t const&> m_onMciStable; ///<  Called when a subsequent call to import transactions and ready.

		Statistics m_statistics; ///Statistical witness block

		h256 m_lastStateRoot;
		Block m_postSeal;                       ///< The state of the client which we're sealing (i.e. it'll have all the rewards added).

        mcp::log m_log = { mcp::log("node") };
	};
}