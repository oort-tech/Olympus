#pragma once
#include <libdevcore/CommonJS.h>

#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <mcp/core/transaction_receipt.hpp>
#include <mcp/core/approve_receipt.hpp>
#include "exceptions.hpp"

namespace mcp
{
	TransactionSkeleton toTransactionSkeletonForEth(mcp::json const& _json);

	inline Address jsToAddress(std::string const& _s) { return jsToFixed<20>(_s); }

	inline h256 jsToHash(std::string const& _s) { return jsToFixed<32>(_s);  }

	inline Signature jsToSignature(std::string const& _s) { return jsToFixed<65>(_s); }

	inline uint64_t jsToULl(std::string const & _s, std::string const & _errorMsg = "")
	{
		try
		{
			if (_s.substr(0, 2) == "0x")
			{
				// Hex
				dev::bytes in = fromHex(_s.substr(2), WhenError::Throw);
				return (uint64_t)fromBigEndian<boost::multiprecision::number<boost::multiprecision::cpp_int_backend<8 * 8, 8 * 8, boost::multiprecision::unsigned_magnitude, boost::multiprecision::checked, void>>>(in);
			}

			// Decimal
			return (uint64_t)boost::multiprecision::number<boost::multiprecision::cpp_int_backend<8 * 8, 8 * 8, boost::multiprecision::unsigned_magnitude, boost::multiprecision::checked, void>>(_s);

		}
		catch (const std::exception&)
		{
			std::string _e = "Cannot wrap string value as a json-rpc type; params \"" + _errorMsg + "\" cannot be converted to uint64.";
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(_e.c_str()));
		}
	}

	inline BlockNumber jsToBlockNumber(std::string const& _js)
	{
		if (_js == "latest")
			return LatestBlock;
		else if (_js == "earliest")
			return 0;
		else if (_js == "pending")
			return PendingBlock;
		else
			return (BlockNumber)jsToULl(_js, _js);
	}

	mcp::json toJson(Transaction const& _t);

	mcp::json toJson(LocalisedTransaction const& _t);

	mcp::json toJson(dev::eth::LocalisedTransactionReceipt const& _t);

	mcp::json toJson(mcp::localised_log_entries const& _e);

	mcp::json toJson(mcp::localised_log_entry const& _e);

	mcp::json toJson(mcp::log_entry const& _e);

	mcp::json toJson(mcp::block & _b, bool is_eth = false);

	mcp::json toJson(mcp::block_state & _b);

	mcp::json toJson(dev::ApproveReceipt const& _a);
}