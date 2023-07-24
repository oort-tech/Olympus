#include <boost/throw_exception.hpp>

#include <libdevcore/CommonJS.h>
#include <mcp/core/config.hpp>
#include "jsonHelper.hpp"

namespace mcp
{
	inline std::string TransactionSkeletonField(mcp::json const& _json)
	{
		if (!_json.is_string())
			BOOST_THROW_EXCEPTION(NEW_RPC_Eth_Error_InvalidParams("Invalid Params"));
		std::string _a = _json;
		return _a;
	}

	TransactionSkeleton toTransactionSkeletonForEth(mcp::json const& _json)
	{
		TransactionSkeleton ret;
		ret.from.clear();
		ret.to.clear();

		if (!_json.is_object() || _json.empty())
			return ret;

		if (_json.count("from"))
		{
			std::string _from = TransactionSkeletonField(_json["from"]);
			try {
				ret.from = jsToAddress(_from);
			}
			catch (...) {
				BOOST_THROW_EXCEPTION(NEW_RPC_Eth_Error_InvalidParams("Invalid Account From"));
			}
		}

		if (_json.count("to")) 
		{
			if (!_json["to"].is_null())///null is create contract.
			{
				std::string _to = TransactionSkeletonField(_json["to"]);
				try {
					ret.to = jsToAddress(_to);
				}
				catch (...) {
					BOOST_THROW_EXCEPTION(NEW_RPC_Eth_Error_InvalidParams("Invalid Account To"));
				}
			}
		}

		if (_json.count("value")) 
			ret.value = jsToU256(TransactionSkeletonField(_json["value"]));

		if (_json.count("gas"))
			ret.gas = jsToU256(TransactionSkeletonField(_json["gas"]));

		if (_json.count("gasPrice"))
			ret.gasPrice = jsToU256(TransactionSkeletonField(_json["gasPrice"]));

		if (_json.count("data")) 
		{
			std::string _data = TransactionSkeletonField(_json["data"]);
			try {
				ret.data = jsToBytes(_data, OnFailed::Throw);
			}
			catch (...) {
				BOOST_THROW_EXCEPTION(NEW_RPC_Eth_Error_InvalidParams("Invalid Data"));
			}
		}

		if (_json.count("nonce"))
			ret.nonce = jsToU256(TransactionSkeletonField(_json["nonce"]));

		return ret;
	}

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
				res["blockHash"] = toJS(_t.blockHash());
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
		res["blockHash"] = toJS(_t.blockHash());
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
				rs["blockHash"] = toJS(r.blockHash);
				rs["logIndex"] = toJS(r.logIndex);
				rs["transactionHash"] = toJS(r.transactionHash);
				rs["transactionIndex"] = toJS(r.transactionIndex);
			}
			res.push_back(rs);
		}
		
		return res;
	}

	mcp::json toJson(mcp::localised_log_entry const& _e)
	{
			mcp::json res;
			if (_e.isSpecial)
				res = toJS(_e.special);
			else
			{
				res = toJson(static_cast<mcp::log_entry const&>(_e));
				res["type"] = "mined";
				res["blockNumber"] = toJS(_e.blockNumber);
				res["blockHash"] = toJS(_e.blockHash);
				res["logIndex"] = toJS(_e.logIndex);
				res["transactionHash"] = toJS(_e.transactionHash);
				res["transactionIndex"] = toJS(_e.transactionIndex);
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
			res["nonce"] = nullptr;
			res["extraData"] = "0x00";
			res["hash"] = _b.hash().hexPrefixed();
			res["parentHash"] = toJS(_b.previous());
			res["gasUsed"] = 0;
			res["minGasPrice"] = 0;
			res["gasLimit"] = toJS(mcp::tx_max_gas);
			res["timestamp"] = toJS(_b.exec_timestamp());
			res["transactions"] = mcp::json::array();
			res["miner"] = _b.from().hexPrefixed();
		}
		else {
			res["hash"] = toJS(_b.hash());
			res["from"] = toJS(_b.from());
			res["previous"] = toJS(_b.previous());

			mcp::json j_parents = mcp::json::array();
			for (auto & p : _b.parents())
			{
				j_parents.push_back(toJS(p));
			}
			res["parents"] = j_parents;

			mcp::json j_links = mcp::json::array();
			for (auto & l : _b.links())
				j_links.push_back(toJS(l));
			res["links"] = j_links;

			mcp::json j_approves = mcp::json::array();
			for (auto & l : _b.approves())
				j_approves.push_back(toJS(l));
			res["approves"] = j_approves;

			res["last_summary"] = toJS(_b.last_summary());
			res["last_summary_block"] = toJS(_b.last_summary_block());
			res["last_stable_block"] = toJS(_b.last_stable_block());
			res["timestamp"] = _b.exec_timestamp();
			res["gasLimit"] = toJS(mcp::tx_max_gas);
			res["signature"] = toJS((Signature)_b.signature());
		}

		return res;
	}

	mcp::json toJson(dev::ApproveReceipt const& _a)
	{
		mcp::json res;

		res["from"] = toJS(_a.from());
		res["output"] = toJS(_a.output());
		res["status"] = toJS(_a.statusCode());

		return res;
	}
}
