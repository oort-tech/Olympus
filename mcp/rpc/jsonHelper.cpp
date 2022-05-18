#include <boost/throw_exception.hpp>
#include <libdevcore/CommonJS.h>
#include "jsonHelper.hpp"

namespace mcp
{
	TransactionSkeleton mcp::toTransactionSkeleton(mcp::json const& json)
	{
		TransactionSkeleton ret;

		if (!json.is_object() || json.empty())
			return ret;

		if (json.count("from") && !json["from"].empty() && json["from"].is_string())
			ret.from = jsToAddress(json["from"]);
		if (json.count("to") && !json["to"].empty() && json["to"].is_string())
			ret.to = jsToAddress(json["to"]);

		if (json.count("value") && !json["value"].empty() && json["value"].is_string())
			ret.value = jsToU256(json["value"]);

		if (json.count("gas") && !json["gas"].empty() && json["gas"].is_string())
			ret.gas = jsToU256(json["gas"]);

		if (json.count("gasPrice") && !json["gasPrice"].empty() && json["gasPrice"].is_string())
			ret.gasPrice = jsToU256(json["gasPrice"]);

		if (json.count("data") && !json["data"].empty() && json["data"].is_string())
			ret.data = jsToBytes(json["data"], OnFailed::Throw);

		//if (!json["code"].empty() && json["gasPrice"].is_string())
		//	ret.data = jsToBytes(json["code"].asString(), OnFailed::Throw);

		if (json.count("nonce") && !json["nonce"].empty() && json["nonce"].is_string())
			ret.nonce = jsToU256(json["nonce"]);
		return ret;
	}

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


	mcp::json toJson(transaction const& _t)
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