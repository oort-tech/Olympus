#pragma once

#include <mcp/core/block_store.hpp>
#include <mcp/common/utility.hpp>
#include <set>

namespace mcp
{
	class genesis
	{
	public:
		static bool try_initialize(mcp::db::db_transaction & transaction_a, mcp::block_store & store_a);
		
		static mcp::block_hash block_hash;
	};
}
