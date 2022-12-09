#pragma once

#include <mcp/node/message.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/core/graph.hpp>
#include <mcp/core/transaction_queue.hpp>
#include <mcp/core/iapprove_queue.hpp>

#include <set>
#include <unordered_set>


namespace mcp
{
	enum class base_validate_result_codes
	{
		ok,
		old,
		invalid_signature,
		invalid_block,
		known_invalid_block
	};

	class base_validate_result
	{
	public:
		mcp::base_validate_result_codes code;
		std::string err_msg;
	};

	enum class validate_result_codes
	{
		ok,
		old,
		missing_parents_and_previous,
		invalid_block,
		known_invalid_block,
		parents_and_previous_include_invalid_block,
	};

	class validate_result
	{
	public:
		mcp::validate_result_codes code;
		std::string err_msg;
		std::unordered_set<mcp::block_hash> missing_parents_and_previous;
		h256Hash missing_links;
		h256Hash missing_approves;
	};


	class validation
	{
	public:
		validation(
			mcp::block_store& store_a,
			std::shared_ptr<mcp::block_cache> cache_a,
			std::shared_ptr<mcp::iTransactionQueue> tq,
			std::shared_ptr<mcp::iApproveQueue> aq
		);
		~validation();

		mcp::base_validate_result base_validate(mcp::db::db_transaction& transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, std::shared_ptr<mcp::block_processor_item> item_a);
		mcp::validate_result dag_validate(mcp::db::db_transaction &  transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::joint_message const & message);

		mcp::block_store & m_store;
		mcp::graph m_graph;
		std::shared_ptr<mcp::block_cache> m_cache;
		std::shared_ptr<mcp::iTransactionQueue> m_tq;
		std::shared_ptr<mcp::iApproveQueue> m_aq;
	};
}