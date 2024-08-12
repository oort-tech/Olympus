#include <boost/throw_exception.hpp>

#include <libdevcore/CommonJS.h>
#include <mcp/core/config.hpp>
#include <account/abi.hpp>
#include "jsonHelper.hpp"

namespace mcp
{
	inline std::string TransactionSkeletonField(mcp::json const& _json)
	{
		if (!_json.is_string())
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("Cannot wrap input as a json-rpc type."));
		std::string _a = _json;
		return _a;
	}

	TransactionSkeleton toTransactionSkeletonForEth(mcp::json const& _json)
	{
		TransactionSkeleton ret;
		ret.from.clear();
		ret.to.clear();

		if (!_json.is_object() || _json.empty())
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("CreateTransaction called on non-object."));

		if (_json.count("from"))
		{
			std::string _from = TransactionSkeletonField(_json["from"]);
			if (!isAddress(_from))
				BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

			ret.from = jsToAddress(_from);
		}

		if (_json.count("to")) 
		{
			if (!_json["to"].is_null())///null is create contract.
			{
				std::string _to = TransactionSkeletonField(_json["to"]);
				if (!isAddress(_to))
					BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

				ret.to = jsToAddress(_to);
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
			catch (const std::exception&) {
				BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("cannot wrap string value as a json-rpc type; the \"data\" contains invalid hex character."));
			}
		}

		if (_json.count("nonce"))
			ret.nonce = jsToU256(TransactionSkeletonField(_json["nonce"]));

		return ret;
	}

	mcp::LogFilter toLogFilter(mcp::json const& _json)
	{
		mcp::LogFilter filter;
		if (!_json.is_object() || _json.empty())
			return filter;

		bool isRange = false;
		if (_json.count("fromBlock") && !_json["fromBlock"].is_null())
		{
			filter.withFrom(jsToBlockNumber(_json["fromBlock"]));
			isRange = true;
		}

		if (_json.count("toBlock") && !_json["toBlock"].is_null())
		{
			filter.withTo(jsToBlockNumber(_json["toBlock"]));
			isRange = true;
		}

		if (_json.count("blockhash") && !_json["blockhash"].is_null())
		{
			if (!mcp::isH256(_json["blockhash"]))
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(BadHexFormat));
			if (isRange)// BlockHash is mutually exclusive with FromBlock/ToBlock criteria
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("invalid argument 0: cannot specify both BlockHash and FromBlock/ToBlock, choose one or the other"));
			filter.withBlockHash(jsToHash(_json["blockhash"]));
		}

		if (_json.count("address") && !_json["address"].is_null())
		{
			if (_json["address"].is_array())
			{
				for (std::string const& i : _json["address"])
				{
					if (!mcp::isAddress(i))
						BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(BadHexFormat));
					filter.withAddress(jsToAddress(i));
				}
			}
			else
			{
				if (!mcp::isAddress(_json["address"]))
					BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(BadHexFormat));
				filter.withAddress(jsToAddress(_json["address"]));
			}
		}

		if (_json.count("topics") && !_json["topics"].is_null())
		{
			if (!_json["topics"].is_array())
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("cannot wrap string value as a json-rpc type; topics needs array type."));
			int index = 0;
			for (auto const& i : _json["topics"])
			{
				if (i.is_array())
				{
					for (auto const& t : i)
					{
						if (!t.empty() && !t.is_null())
						{
							if (!mcp::isH256(t))
								BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(BadHexFormat));
							filter.withTopic(index, jsToHash(t));
						}
					}
				}
				else if (!i.empty() && !i.is_null())
				{
					if (!mcp::isH256(i))
						BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(BadHexFormat));
					filter.withTopic(index, jsToHash(i));
				}
				index++;
			}
		}

		return filter;
	}

	mcp::BlockNumberOrHash toBlockNumberOrHash(mcp::json const& _json)
	{
		mcp::BlockNumberOrHash _ret;
		if (_json.is_object())
		{
			if (_json.count("blockNumber") && !_json["blockNumber"].is_null())
				_ret._blockNumber = jsToBlockNumber(_json["blockNumber"]);

			if (_json.count("blockHash") && !_json["blockHash"].is_null())
			{
				if (!mcp::isH256(_json["blockHash"]))
					BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(BadHexFormat));
				_ret._blockHash = jsToHash(_json["blockHash"]);
			}
			if (_ret._blockNumber && _ret._blockHash)
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("cannot specify both BlockHash and BlockNumber, choose one or the other"));
		}
		else if(!_json.empty() && !_json.is_null())///default string
		{
			if (mcp::isH256(_json))///block hash
				_ret._blockHash = jsToHash(_json);
			else///block number
				_ret._blockNumber = jsToBlockNumber(_json);
		}

		return _ret;
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

		for (auto const& r : _e)
		{
			res.push_back(toJson(r));
		}
		
		return res;
	}

	mcp::json toJson(mcp::localised_log_entry const& _e)
	{
		mcp::json res;
		res = toJson(static_cast<mcp::log_entry const&>(_e));
		res["type"] = "mined";
		res["blockNumber"] = toJS(_e.blockNumber);
		res["blockHash"] = toJS(_e.blockHash);
		res["logIndex"] = toJS(_e.logIndex);
		res["transactionHash"] = toJS(_e.transactionHash);
		res["transactionIndex"] = toJS(_e.transactionIndex);
		res["removed"] = _e.Removed;

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

	mcp::json toJson(mcp::block & _b)
	{
		mcp::json res;

		res["hash"] = toJS(_b.hash());
		res["from"] = toJS(_b.from());
		res["previous"] = toJS(_b.previous());

		mcp::json j_parents = mcp::json::array();
		for (auto& p : _b.parents())
		{
			j_parents.push_back(toJS(p));
		}
		res["parents"] = j_parents;

		mcp::json j_links = mcp::json::array();
		for (auto& l : _b.links())
			j_links.push_back(toJS(l));
		res["links"] = j_links;

		mcp::json j_approves = mcp::json::array();
		for (auto& l : _b.approves())
			j_approves.push_back(toJS(l));
		res["approves"] = j_approves;

		res["last_summary"] = toJS(_b.last_summary());
		res["last_summary_block"] = toJS(_b.last_summary_block());
		res["last_stable_block"] = toJS(_b.last_stable_block());
		res["timestamp"] = _b.exec_timestamp();
		res["gasLimit"] = toJS(mcp::tx_max_gas);
		res["signature"] = toJS((Signature)_b.signature());

		return res;
	}

	mcp::json toJson(mcp::LocalisedBlock& _b, bool is_full)
	{
		mcp::json res;

		res["number"] = toJS(_b.blockNumber());
		res["hash"] = toJS(_b.hash());
		res["parentHash"] = toJS(_b.parent());
		res["nonce"] = nullptr;
		res["miner"] = toJS(_b.from());
		res["extraData"] = "0x";
		res["difficulty"] = "0x";
		res["minGasPrice"] = toJS(_b.minGasPrice());
		res["gasLimit"] = toJS(mcp::tx_max_gas);
		res["gasUsed"] = toJS(_b.gasUsed());
		res["timestamp"] = toJS(_b.exec_timestamp());

		res["transactions"] = mcp::json::array();
		auto _ts = _b.transactions();
		for (size_t i = 0; i < _ts.size(); i++)
		{
			if (is_full)
			{
				res["transactions"].push_back(toJson(LocalisedTransaction(_ts[i], _b.hash(), i, _b.blockNumber())));
			}
			else
				res["transactions"].push_back(toJS(_ts[i].sha3()));
		}
		
		res["sha3Uncles"] = toJS(_b.sha3Uncles());
		res["transactionsRoot"] = toJS(_b.transactionsRoot());
		res["stateRoot"] = toJS(_b.stateRoot());
		res["receiptsRoot"] = toJS(_b.receiptsRoot());
		res["size"] = toJS(_b.size());
		res["logsBloom"] = toJS(_b.bloom());

		res["uncles"] = mcp::json::array();
		for (auto const& _u : _b.uncles())
			res["uncles"].push_back(toJS(_u));

		return res;
	}

	mcp::json toJson(mcp::block_state & _b)
	{
		mcp::json res;

		mcp::json content = mcp::json::object();
		content["level"] = _b.level;
		content["witnessed_level"] = _b.witnessed_level;
		content["best_parent"] = toJS(_b.best_parent);
		res["content"] = content;

		res["is_stable"] = _b.is_stable ? 1 : 0;
		if (_b.is_stable)
		{
			mcp::json stable_content = mcp::json::object();
			stable_content["status"] = (uint8_t)_b.status;
			stable_content["stable_index"] = _b.stable_index;
			stable_content["stable_timestamp"] = _b.stable_timestamp;

			if (_b.main_chain_index)
				stable_content["mci"] = *_b.main_chain_index;
			else
				stable_content["mci"] = nullptr;

			stable_content["mc_timestamp"] = _b.mc_timestamp;
			stable_content["is_on_mc"] = _b.is_on_main_chain ? 1 : 0;
			stable_content["is_free"] = _b.is_free ? 1 : 0;

			res["stable_content"] = stable_content;
		}
		else
			res["stable_content"] = nullptr;

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

	std::string newRevertError(mcp::ExecutionResult const& result)
	{
		std::string reason;
		bool unpack = dev::UnpackRevert(result.Revert(), reason);
		std::string err = result.ErrorMsg();
		if (unpack)
			err = err + ": " + reason;

		return err;
	}
}
