#pragma once
#include <libdevcore/CommonJS.h>

#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <mcp/core/transaction_receipt.hpp>
#include <mcp/core/approve_receipt.hpp>
#include <mcp/core/LogFilter.hpp>
#include <mcp/p2p/common.hpp>
#include "exceptions.hpp"
#include "json.hpp"

namespace mcp
{
	const static char* BadHexFormat = "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length.";
	const static char* AddressNotExist = "no key for given address or file.";
	const static char* AddressPwdError = "could not decrypt key with given passphrase.";

	TransactionSkeleton toTransactionSkeletonForEth(mcp::json const& _json);

	mcp::json toJson(Address const& _address);

	mcp::json toJson(bi::tcp::endpoint const& _ep);

	template <class T>
	mcp::json toJson(std::vector<T> const& _es)
	{
		mcp::json res = mcp::json::array();
		for (auto const& e : _es)
			res.push_back(toJson(e));
		return res;
	}

	mcp::LogFilter toLogFilter(mcp::json const& _json);

	mcp::BlockNumberOrHash toBlockNumberOrHash(mcp::json const& _json);

	inline Address jsToAddress(std::string const& _s) { return jsToFixed<20>(_s); }

	inline h256 jsToHash(std::string const& _s) { return jsToFixed<32>(_s);  }

	inline Signature jsToSignature(std::string const& _s) { return jsToFixed<65>(_s); }

	inline uint64_t jsToULl(std::string const & _s, std::string const & _errorMsg = "")
	{
		try
		{
			if (_s.empty())
				throw std::invalid_argument("");

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
			std::string _e = "cannot wrap string value as a json-rpc type; params \"" + _errorMsg + "\" cannot be converted to uint64.";
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(_e.c_str()));
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
		else if (_js == "finalized" || _js == "safe")
			return LatestBlock;
		else
			return (BlockNumber)jsToULl(_js, _js);
	}

	mcp::json toJson(Transaction const& _t);

	mcp::json toJson(LocalisedTransaction const& _t);

	mcp::json toJson(dev::eth::LocalisedTransactionReceipt const& _t);

	mcp::json toJson(mcp::localised_log_entries const& _e);

	mcp::json toJson(mcp::localised_log_entry const& _e);

	mcp::json toJson(mcp::log_entry const& _e);

	mcp::json toJson(mcp::block & _b);

	mcp::json toJson(mcp::LocalisedBlock& _b, bool is_full = false);


	mcp::json toJson(mcp::block_state & _b);

	mcp::json toJson(dev::ApproveReceipt const& _a);

	std::string newRevertError(mcp::ExecutionResult const& result);

	mcp::json toJson(WitnessList& _wl);

	mcp::json toJson(mcp::Approves const& _a);

	mcp::json toJson(mcp::approve const& _a);
}