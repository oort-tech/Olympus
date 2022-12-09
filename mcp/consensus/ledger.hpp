#pragma once

#include <mcp/core/block_cache.hpp>
#include <mcp/common/log.hpp>
#include <mcp/core/config.hpp>

namespace mcp
{
	class ledger
	{
	public:
		ledger();

		mcp::block_hash determine_best_parent(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, std::vector<mcp::block_hash> const & pblock_hashs);
		uint64_t calc_level(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & best_pblock_hash);
		uint64_t calc_witnessed_level(mcp::witness_param const & witness_param_a, uint64_t const & level);
		// Updated block_from to account, Daniel
		bool check_majority_witness(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & best_pblock_hash, dev::Address const & block_from, mcp::witness_param const & witness_param_a);
		std::shared_ptr<mcp::min_wl_result> find_mc_min_wl(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, uint64_t const &witnessed_level_a, mcp::block_hash const &best_parent_block_hash_a, dev::Address const & block_from);
		bool check_stable(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_hash const & earlier_hash, mcp::block_hash const & bp_block_hash, std::vector<mcp::block_hash> const & parents_a, dev::Address const & block_from_a, mcp::block_hash const & checked_stable_block_hash, mcp::witness_param const & witness_param_a);

	private:
        mcp::log m_log = { mcp::log("node") };
	};

	extern ledger Ledger;
}; // namespace mcp
