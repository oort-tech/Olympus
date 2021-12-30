#pragma once

#include <mcp/core/block_store.hpp>
#include <mcp/core/block_cache.hpp>
#include <mcp/common/log.hpp>

namespace mcp
{
	enum class graph_compare_result
	{
		non_related = 0,			//hash1 and hash2 are not related
		equal = 1,					//hash1 == hash2
		hash1_included_by_hash2,	//hash1 is included by hash2
		hash2_included_by_hash1,	//hash2 is included by hash1
	};

	class graph
	{
	public:
		graph(mcp::block_store &);
		mcp::graph_compare_result compare(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash hash1, mcp::block_hash hash2);
		bool determine_if_included(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & earlier_hash, std::vector<mcp::block_hash> const & later_hashs, bool const & is_trace = false);
		bool determine_if_included_or_equal(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & earlier_hash, std::vector<mcp::block_hash> const & later_hashs, bool const & is_trace = false);
		void test_determine_if_included_or_equal();

	private:
		bool go_up_check_included(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & earlier_hash, std::vector<mcp::block_hash> const & later_hashs, bool const & is_trace = false);
		bool go_down_check_included(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & later_hash, std::vector<mcp::block_hash> const & earlier_hashs);


		mcp::block_store & m_store;
        mcp::log m_log = { mcp::log("node") };
	};
};
