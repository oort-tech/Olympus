#pragma once

#include <mcp/node/message.hpp>
#include <mcp/consensus/ledger.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/node/chain.hpp>
#include "transaction_queue.hpp"

namespace mcp
{
	class composer
	{
	public:
		composer(mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
			mcp::ledger& ledger_a, std::shared_ptr<mcp::TransactionQueue> tq
		);
		~composer();
		std::shared_ptr<mcp::block> compose_block(mcp::account const & from_a, dev::Secret const& s);
		
	private:
		mcp::block_hash get_latest_block(mcp::db::db_transaction &  transaction_a, mcp::account const & account_a);
		void pick_parents_and_last_summary_and_wl_block(mcp::db::db_transaction &  transaction_a, mcp::block_hash const & previous_a, mcp::account const & from_a, std::vector<mcp::block_hash>& parents, h256s& links, mcp::block_hash & last_summary_block, mcp::block_hash & last_summary, mcp::block_hash & last_stable_block);

		mcp::ledger & m_ledger;
		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::shared_ptr<mcp::TransactionQueue> m_tq;
        mcp::log m_log = { mcp::log("node") };
		void stop() { m_stopped = true; }
		bool m_stopped = false;
	};
}