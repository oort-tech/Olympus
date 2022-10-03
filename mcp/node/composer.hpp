#pragma once

#include <mcp/node/message.hpp>
#include <mcp/consensus/ledger.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/node/chain.hpp>
#include "transaction_queue.hpp"
#include "approve_queue.hpp"

namespace mcp
{
	class composer
	{
	public:
		composer(mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
			mcp::ledger& ledger_a, std::shared_ptr<mcp::TransactionQueue> tq, std::shared_ptr<mcp::ApproveQueue> aq
		);
		~composer();
		std::shared_ptr<mcp::block> compose_block(dev::Address const & from_a, dev::Secret const& s);
		uint64_t get_new_last_summary_mci(mcp::db::db_transaction &  transaction_a);

	private:
		mcp::block_hash get_latest_block(mcp::db::db_transaction &  transaction_a, dev::Address const & account_a);
		void pick_parents_and_last_summary_and_wl_block(mcp::db::db_transaction &  transaction_a, mcp::block_hash const & previous_a, dev::Address const & from_a, std::vector<mcp::block_hash>& parents, h256s& links,  h256s& approves, mcp::block_hash & last_stable_block);

		mcp::ledger & m_ledger;
		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::shared_ptr<mcp::TransactionQueue> m_tq;
		std::shared_ptr<mcp::ApproveQueue> m_aq;
        mcp::log m_log = { mcp::log("node") };
		void stop() { m_stopped = true; }
		bool m_stopped = false;

		std::shared_ptr<rocksdb::ManagedSnapshot> m_snapshot;
		std::shared_ptr<mcp::free_key> m_best_dag_free_key;
		mcp::block_hash m_best_pblock_hash;
		uint64_t m_new_last_summary_mci;
		mcp::block_hash m_last_summary_block;
		mcp::block_hash m_last_summary;
	};
}