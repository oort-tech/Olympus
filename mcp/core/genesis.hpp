#pragma once

#include <mcp/core/block_store.hpp>
#include <mcp/common/utility.hpp>
#include <set>

namespace mcp
{
	class genesis
	{
	public:
		static std::pair<mcp::block, Transactions> try_initialize(/*mcp::db::db_transaction & transaction_a, mcp::block_store & store_a*/);
		
		static std::pair<bool, dev::Address> isGenesisTransaction(mcp::block_hash const& _h);

		static mcp::block_hash block_hash;

		static dev::Address GenesisAddress;
	};
}
