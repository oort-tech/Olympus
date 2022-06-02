#pragma once
#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <mcp/core/transaction_receipt.hpp>

namespace mcp
{
	TransactionSkeleton toTransactionSkeleton(mcp::json const& _json);

	inline Address jsToAddress(std::string const& _s) { return mcp::toAddress(_s); }

	enum class OnFailed { InterpretRaw, Empty, Throw };
	bytes jsToBytes(std::string const& _s, OnFailed _f = OnFailed::Empty);


	//response
	mcp::json toJson(Transaction const& _t);

	mcp::json toJson(LocalisedTransaction const& _t);

	mcp::json toJson(dev::eth::LocalisedTransactionReceipt const& _t);

	mcp::json toJson(mcp::localised_log_entries const& _e);

	mcp::json toJson(mcp::log_entry const& _e);

	mcp::json toJson(mcp::block & _b);
}