#pragma once
#include <libdevcore/CommonJS.h>

#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <mcp/core/transaction_receipt.hpp>

namespace mcp
{
	TransactionSkeleton toTransactionSkeletonForEth(mcp::json const& _json);

	TransactionSkeleton toTransactionSkeletonForMcp(mcp::json const& _json);

	inline Address jsToAddress(std::string const& _s) { return jsToFixed<20>(_s); }

	inline h256 jsToHash(std::string const& _s) { return jsToFixed<32>(_s);  }

	inline Signature jsToSignature(std::string const& _s) { return jsToFixed<65>(_s); }

	inline u128 jsToU128(std::string const& _s) { return jsToInt<16>(_s); }

	inline u64 jsToU64(std::string const& _s) { return jsToInt<8>(_s); }

	//enum class OnFailed { InterpretRaw, Empty, Throw };
	//bytes jsToBytes(std::string const& _s, OnFailed _f = OnFailed::Empty);

	//response
	mcp::json toJson(Transaction const& _t);

	mcp::json toJson(LocalisedTransaction const& _t);

	mcp::json toJson(dev::eth::LocalisedTransactionReceipt const& _t);

	mcp::json toJson(mcp::localised_log_entries const& _e);

	mcp::json toJson(mcp::log_entry const& _e);

	mcp::json toJson(mcp::block & _b);

	mcp::json toJson(mcp::block & _b, Transaction const& _t);
}