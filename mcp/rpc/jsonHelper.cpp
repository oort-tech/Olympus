#include <boost/throw_exception.hpp>

#include "jsonHelper.hpp"
#include "exceptions.hpp"

namespace mcp
{
	TransactionSkeleton toTransactionSkeleton(mcp::json const& _json)
	{
		TransactionSkeleton ret;

		if (!_json.is_object() || _json.empty())
			return ret;

		if (_json.count("from") && !_json["from"].empty() && _json["from"].is_string()) {
			try {
				ret.from = jsToAddress(_json["from"]);
			}
			catch (...) {
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccountFrom());
			}
		}

		if (_json.count("to") && !_json["to"].empty() && _json["to"].is_string()) {
			try {
				ret.to = jsToAddress(_json["to"]);
			}
			catch (...) {
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccountTo());
			}
		}

		if (_json.count("value") && !_json["value"].empty() && _json["value"].is_string()) {
			ret.value = jsToU256(_json["value"]);
		}

		if (_json.count("gas") && !_json["gas"].empty() && _json["gas"].is_string())
			ret.gas = jsToU256(_json["gas"]);

		if (_json.count("gasPrice") && !_json["gasPrice"].empty() && _json["gasPrice"].is_string())
			ret.gasPrice = jsToU256(_json["gasPrice"]);

		if (_json.count("data") && !_json["data"].empty() && _json["data"].is_string()) {
			try {
				ret.data = jsToBytes(_json["data"], OnFailed::Throw);
			}
			catch (...) {
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidData());
			}
		}

		//if (!_json["code"].empty() && _json["gasPrice"].is_string())
		//	ret.data = jsToBytes(_json["code"].asString(), OnFailed::Throw);

		if (_json.count("nonce") && !_json["nonce"].empty() && _json["nonce"].is_string())
			ret.nonce = jsToU256(_json["nonce"]);

		return ret;
	}

	/*
	bytes jsToBytes(std::string const & _s, OnFailed _f)
	{
		try
		{
			return fromHex(_s, WhenError::Throw);
		}
		catch (...)
		{
			if (_f == OnFailed::InterpretRaw)
				return asBytes(_s);
			else if (_f == OnFailed::Throw)
				BOOST_THROW_EXCEPTION(InvalidData());
		}
		return bytes();
	}
	*/

	mcp::json toJson(Transaction const& _t)
	{
		mcp::json res;
		if (_t)
		{
			res["hash"] = toJS(_t.sha3());
			res["input"] = toJS(_t.data());
			res["to"] = _t.isCreation() ? "" : toJS(_t.receiveAddress());
			res["from"] = toJS(_t.safeSender());
			res["gas"] = toJS(_t.gas());
			res["gasPrice"] = toJS(_t.gasPrice());
			res["nonce"] = toJS(_t.nonce());
			res["value"] = toJS(_t.value());
			//res["blockHash"] = toJS(_t.blockHash());
			//res["transactionIndex"] = toJS(_t.transactionIndex());
			//res["blockNumber"] = toJS(_t.blockNumber());
			res["r"] = toJS(_t.signature().r);
			res["s"] = toJS(_t.signature().s);
			res["v"] = toJS(_t.rawV());
		}
		return res;
	}

	mcp::json toJson(dev::eth::LocalisedTransactionReceipt const& _t)
	{
		mcp::json res;
		res["transactionHash"] = toJS(_t.hash());
		res["transactionIndex"] = _t.transactionIndex();
		res["blockHash"] = toJS(_t.blockHash());
		res["blockNumber"] = _t.blockNumber();
		res["from"] = toJS(_t.from());
		res["to"] = toJS(_t.to());
		res["cumulativeGasUsed"] = toJS(_t.cumulativeGasUsed());
		res["gasUsed"] = toJS(_t.gasUsed());
		res["contractAddress"] = toJS(_t.contractAddress());
		res["logs"] = toJson(_t.localisedLogs());
		res["logsBloom"] = toJS(_t.bloom());
		res["status"] = toJS(_t.statusCode());
		res["type"] = "0x2";//for metamask.golang
		return res;
	}

	mcp::json toJson(mcp::localised_log_entries const& _e)
	{
		mcp::json res = mcp::json::array();

		for (auto r : _e)
		{
			mcp::json rs;
			if (r.isSpecial)
				rs = toJS(r.special);
			else
			{
				rs = toJson(static_cast<mcp::log_entry const&>(r));
				rs["type"] = "mined";
				rs["blockNumber"] = r.blockNumber;
				rs["blockHash"] = toJS(r.blockHash);
				rs["logIndex"] = r.logIndex;
				rs["transactionHash"] = toJS(r.transactionHash);
				rs["transactionIndex"] = r.transactionIndex;
			}
			res.push_back(rs);
		}
		
		return res;
	}

	mcp::json toJson(mcp::log_entry const& _e)
	{
		mcp::json res;
		res["data"] = toJS(_e.data);
		res["address"] = toJS(_e.address);
		mcp::json topics = mcp::json::array();
		for (auto const& t : _e.topics)
			topics.push_back(toJS(t));
		res["topics"] = topics;
		
		return res;
	}

}