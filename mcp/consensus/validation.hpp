#pragma once

#include <mcp/node/message.hpp>
#include <mcp/consensus/ledger.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/core/graph.hpp>
// added by michael at 1/14
#include <mcp/wallet/key_manager.hpp>

#include <set>
#include <unordered_set>


namespace mcp
{
	class validation
	{
	public:
		validation(
			mcp::block_store& store_a, mcp::ledger& ledger_a,
			mcp::mru_list<mcp::block_hash>& invalid_block_cache_a,
			std::shared_ptr<mcp::block_cache> cache_a,
			// added by michael at 1/14
			std::shared_ptr<mcp::key_manager> key_manager
		);
		~validation();

		mcp::base_validate_result base_validate(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, std::shared_ptr<mcp::block_processor_item> item_a);
		mcp::validate_result dag_validate(mcp::db::db_transaction &  transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::joint_message const & message);
		mcp::light_validate_result light_validate(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, std::shared_ptr<mcp::block> block);

		mcp::block_store & m_store;
		mcp::ledger & m_ledger;
		mcp::graph m_graph;
		mcp::mru_list<mcp::block_hash> m_invalid_block_cache;
		std::shared_ptr<mcp::block_cache> m_cache;

		// added by michael at 1/14
		std::shared_ptr<mcp::key_manager> m_key_manager;
	};
}