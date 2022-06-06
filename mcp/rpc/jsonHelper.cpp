#include <boost/throw_exception.hpp>

#include <libdevcore/CommonJS.h>
#include <mcp/core/config.hpp>
#include "jsonHelper.hpp"
#include "exceptions.hpp"

namespace mcp
{
	TransactionSkeleton toTransactionSkeletonForEth(mcp::json const& _json)
	{
		TransactionSkeleton ret;
		ret.from.clear();
		ret.to.clear();

		if (!_json.is_object() || _json.empty())
			return ret;

		if (_json.count("from") && !_json["from"].empty() && _json["from"].is_string()) {
			try {
				ret.from = jsToAddress(_json["from"]);
			}
			catch (...) {
				BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidAccountFrom());
			}
		}

		if (_json.count("to") && !_json["to"].empty() && _json["to"].is_string()) {
			try {
				ret.to = jsToAddress(_json["to"]);
			}
			catch (...) {
				BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidAccountTo());
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
				BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidData());
			}
		}

		//if (!_json["code"].empty() && _json["gasPrice"].is_string())
		//	ret.data = jsToBytes(_json["code"].asString(), OnFailed::Throw);

		if (_json.count("nonce") && !_json["nonce"].empty() && _json["nonce"].is_string())
			ret.nonce = jsToU256(_json["nonce"]);

		return ret;
	}

	TransactionSkeleton toTransactionSkeletonForMcp(mcp::json const& _json)
	{
		TransactionSkeleton ret;
		ret.from.clear();
		ret.to.clear();

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

		if (_json.count("amount") && !_json["amount"].empty() && _json["amount"].is_string()) {
			ret.value = jsToU256(_json["amount"]);
		}

		if (_json.count("gas") && !_json["gas"].empty() && _json["gas"].is_string())
			ret.gas = jsToU256(_json["gas"]);

		if (_json.count("gas_price") && !_json["gas_price"].empty() && _json["gas_price"].is_string())
			ret.gasPrice = jsToU256(_json["gas_price"]);

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
			if (_t.isCreation()) {
				res["to"] = nullptr;
			}
			else {
				res["to"] = toJS(_t.receiveAddress());
			}
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

	mcp::json toJson(LocalisedTransaction const& _t)
	{
		mcp::json res;
		if (_t)
		{
			res["hash"] = toJS(_t.sha3());
			res["input"] = toJS(_t.data());
			if (_t.isCreation()) {
				res["to"] = nullptr;
			}
			else {
				res["to"] = toJS(_t.receiveAddress());
			}
			res["from"] = toJS(_t.safeSender());
			res["gas"] = toJS(_t.gas());
			res["gasPrice"] = toJS(_t.gasPrice());
			res["nonce"] = toJS(_t.nonce());
			res["value"] = toJS(_t.value());
			if (_t.blockHash() == mcp::block_hash(0)) {
				res["blockHash"] = nullptr;
				res["transactionIndex"] = nullptr;
				res["blockNumber"] = nullptr;
			}
			else {
				res["blockHash"] = _t.blockHash().hexPrefixed();
				res["transactionIndex"] = toJS(_t.transactionIndex());
				res["blockNumber"] = toJS(_t.blockNumber());
			}
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
		res["transactionIndex"] = toJS(_t.transactionIndex());
		res["blockHash"] = _t.blockHash().hexPrefixed();
		res["blockNumber"] = toJS(_t.blockNumber());
		res["from"] = toJS(_t.from());
		if (_t.to() == dev::Address(0)) {
			res["to"] = nullptr;
			res["contractAddress"] = toJS(_t.contractAddress());
		}
		else {
			res["to"] = toJS(_t.to());
		}
		res["cumulativeGasUsed"] = toJS(_t.cumulativeGasUsed());
		res["gasUsed"] = toJS(_t.gasUsed());
		res["logs"] = toJson(_t.localisedLogs());
		res["logsBloom"] = toJS(_t.bloom());
		res["status"] = toJS(_t.statusCode());
		//res["type"] = "0x2";//for metamask.golang
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
				rs["blockNumber"] = toJS(r.blockNumber);
				rs["blockHash"] = r.blockHash.hexPrefixed();
				rs["logIndex"] = r.logIndex;
				rs["transactionHash"] = r.transactionHash.hexPrefixed();
				rs["transactionIndex"] = toJS(r.transactionIndex);
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

	mcp::json toJson(mcp::block & _b, bool is_eth)
	{
		mcp::json res;

		if (is_eth) {
			res["number"] = nullptr;
			res["hash"] = _b.hash().hexPrefixed();
			res["parentHash"] = _b.previous().hexPrefixed();
			res["gasUsed"] = 0;
			res["minGasPrice"] = 0;
			res["gasLimit"] = toJS(mcp::block_max_gas);
			res["timestamp"] = toJS(_b.exec_timestamp());
			res["transactions"] = mcp::json::array();
		}
		else {
			res["hash"] = _b.hash().hex();
			res["from"] = _b.from().hexPrefixed();
			res["previous"] = _b.previous().hex();

			mcp::json j_parents = mcp::json::array();
			for (auto & p : _b.parents())
			{
				j_parents.push_back(p.hex());
			}
			res["parents"] = j_parents;

			mcp::json j_links = mcp::json::array();
			for (auto & l : _b.links())
				j_links.push_back(toJS(l));
			res["links"] = j_links;

			res["last_summary"] = _b.last_summary().hex();
			res["last_summary_block"] = _b.last_summary_block().hex();
			res["last_stable_block"] = _b.last_stable_block().hex();
			res["timestamp"] = _b.exec_timestamp();
			res["gasLimit"] = toJS(mcp::block_max_gas);
			res["signature"] = ((Signature)_b.signature()).hex();
		}

		return res;
	}
}