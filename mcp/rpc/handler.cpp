#include "handler.hpp"
#include "exceptions.hpp"
#include "jsonHelper.hpp"
#include <mcp/core/genesis.hpp>
#include <mcp/core/param.hpp>
#include <mcp/common/pwd.hpp>
#include <mcp/node/evm/Executive.hpp>

mcp::rpc_handler::rpc_handler(mcp::rpc &rpc_a, std::string const &body_a, std::function<void(mcp::json const &)> const &response_a/*, int m_cap*/) : 
	body(body_a),
	rpc(rpc_a),
	response(response_a),
	m_key_manager(rpc_a.m_key_manager),
	m_wallet(rpc_a.m_wallet),
	m_client(rpc_a.m_client)
{
	m_ethRpcMethods["account_remove"] = &mcp::rpc_handler::account_remove;
	m_ethRpcMethods["account_import"] = &mcp::rpc_handler::account_import;
	m_ethRpcMethods["accounts_balances"] = &mcp::rpc_handler::accounts_balances;
	m_ethRpcMethods["block"] = &mcp::rpc_handler::block;
	m_ethRpcMethods["block_state"] = &mcp::rpc_handler::block_state;
	m_ethRpcMethods["block_states"] = &mcp::rpc_handler::block_states;
	m_ethRpcMethods["stable_blocks"] = &mcp::rpc_handler::stable_blocks;
	m_ethRpcMethods["block_summary"] = &mcp::rpc_handler::block_summary;
	m_ethRpcMethods["version"] = &mcp::rpc_handler::version;
	m_ethRpcMethods["status"] = &mcp::rpc_handler::status;
	m_ethRpcMethods["peers"] = &mcp::rpc_handler::peers;
	m_ethRpcMethods["nodes"] = &mcp::rpc_handler::nodes;
	m_ethRpcMethods["witness_list"] = &mcp::rpc_handler::witness_list;

	m_ethRpcMethods["epoch_approves"] = &mcp::rpc_handler::epoch_approves;
	m_ethRpcMethods["approve_receipt"] = &mcp::rpc_handler::approve_receipt;
	m_ethRpcMethods["epoch_work_transaction"] = &mcp::rpc_handler::epoch_work_transaction;

	m_ethRpcMethods["net_version"] = &mcp::rpc_handler::net_version;
	m_ethRpcMethods["net_listening"] = &mcp::rpc_handler::net_listening;
	m_ethRpcMethods["net_peerCount"] = &mcp::rpc_handler::net_peerCount;
	m_ethRpcMethods["web3_clientVersion"] = &mcp::rpc_handler::web3_clientVersion;
	m_ethRpcMethods["web3_sha3"] = &mcp::rpc_handler::web3_sha3;

	m_ethRpcMethods["eth_blockNumber"] = &mcp::rpc_handler::eth_blockNumber;
	m_ethRpcMethods["eth_getTransactionCount"] = &mcp::rpc_handler::eth_getTransactionCount;
	m_ethRpcMethods["eth_chainId"] = &mcp::rpc_handler::eth_chainId;
	m_ethRpcMethods["eth_gasPrice"] = &mcp::rpc_handler::eth_gasPrice;
	m_ethRpcMethods["eth_estimateGas"] = &mcp::rpc_handler::eth_estimateGas;
	m_ethRpcMethods["eth_getBlockByNumber"] = &mcp::rpc_handler::eth_getBlockByNumber;
	m_ethRpcMethods["eth_getBlockTransactionCountByHash"] = &mcp::rpc_handler::eth_getBlockTransactionCountByHash;
	m_ethRpcMethods["eth_getBlockTransactionCountByNumber"] = &mcp::rpc_handler::eth_getBlockTransactionCountByNumber;
	m_ethRpcMethods["eth_sendRawTransaction"] = &mcp::rpc_handler::eth_sendRawTransaction;
	m_ethRpcMethods["eth_sendTransaction"] = &mcp::rpc_handler::eth_sendTransaction;
	m_ethRpcMethods["eth_call"] = &mcp::rpc_handler::eth_call;

	m_ethRpcMethods["eth_protocolVersion"] = &mcp::rpc_handler::eth_protocolVersion;
	m_ethRpcMethods["eth_syncing"] = &mcp::rpc_handler::eth_syncing;
	m_ethRpcMethods["eth_getLogs"] = &mcp::rpc_handler::eth_getLogs;
	m_ethRpcMethods["eth_getCode"] = &mcp::rpc_handler::eth_getCode;
	m_ethRpcMethods["eth_getStorageAt"] = &mcp::rpc_handler::eth_getStorageAt;
	m_ethRpcMethods["eth_getStorageRoot"] = &mcp::rpc_handler::eth_getStorageRoot;
	m_ethRpcMethods["eth_getTransactionByHash"] = &mcp::rpc_handler::eth_getTransactionByHash;
	m_ethRpcMethods["eth_getTransactionByBlockHashAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex;
	m_ethRpcMethods["eth_getTransactionByBlockNumberAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex;
	m_ethRpcMethods["eth_getTransactionReceipt"] = &mcp::rpc_handler::eth_getTransactionReceipt;
	m_ethRpcMethods["eth_getBalance"] = &mcp::rpc_handler::eth_getBalance;
	m_ethRpcMethods["eth_getBlockByHash"] = &mcp::rpc_handler::eth_getBlockByHash;
	m_ethRpcMethods["eth_accounts"] = &mcp::rpc_handler::eth_accounts;
	m_ethRpcMethods["eth_sign"] = &mcp::rpc_handler::eth_sign;
	m_ethRpcMethods["eth_signTransaction"] = &mcp::rpc_handler::eth_signTransaction;
	m_ethRpcMethods["debug_traceTransaction"] = &mcp::rpc_handler::debug_traceTransaction;
	m_ethRpcMethods["debug_accountRange"] = &mcp::rpc_handler::debug_accountRange;

	m_ethRpcMethods["personal_importRawKey"] = &mcp::rpc_handler::personal_importRawKey;
	m_ethRpcMethods["personal_listAccounts"] = &mcp::rpc_handler::personal_listAccounts;
	m_ethRpcMethods["personal_lockAccount"] = &mcp::rpc_handler::personal_lockAccount;
	m_ethRpcMethods["personal_newAccount"] = &mcp::rpc_handler::personal_newAccount;
	m_ethRpcMethods["personal_unlockAccount"] = &mcp::rpc_handler::personal_unlockAccount;
	m_ethRpcMethods["personal_sendTransaction"] = &mcp::rpc_handler::personal_sendTransaction;
	m_ethRpcMethods["personal_sign"] = &mcp::rpc_handler::personal_sign;
	m_ethRpcMethods["personal_ecRecover"] = &mcp::rpc_handler::personal_ecRecover;

	m_ethRpcMethods["eth_requestAccounts"] = &mcp::rpc_handler::eth_accounts;
}

void mcp::rpc_handler::account_remove(mcp::json &j_response, bool &)
{
	//0: account, 1: password
	std::string account_text = params[0];
	if (!mcp::isAddress(account_text))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::Address account(account_text);
	if (!m_key_manager->exists(account))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(AddressNotExist));

	if (!m_key_manager->remove(account, params[1]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(AddressPwdError));

	j_response["result"] = true;
}

void mcp::rpc_handler::account_import(mcp::json &j_response, bool &)
{
	std::string json_text = params[0];
	mcp::json js = mcp::json::parse(json_text);
	mcp::key_content kc;
	if (!m_key_manager->import(js, kc))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("Cannot wrap string value as a json-rpc type; only the v3 keystore file."));

	j_response["result"] = kc.address.hexPrefixed();
}

void mcp::rpc_handler::accounts_balances(mcp::json &j_response, bool &)
{
	mcp::json j_balances = mcp::json::array();
	
	if (!params.is_array() || params.size() < 1)
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("Cannot wrap string value as a json-rpc type; not array type or incorrect number of arguments."));

	auto _block = client()->blockByNumber(LatestBlock);
	for (mcp::json const &j_account : params)
	{
		std::string account_text = j_account;
		if (!mcp::isAddress(account_text))
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

		mcp::json acc_balance;
		acc_balance[account_text] = _block.balance(jsToAddress(account_text)).str();
		j_balances.push_back(acc_balance);
	}

	j_response["result"] = j_balances;
}

void mcp::rpc_handler::block(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	auto _block = client()->blockInfo(jsToHash(params[0]));
	if (_block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	j_response["result"] = toJson(*_block);
}

void mcp::rpc_handler::block_state(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	auto _state = client()->blockState(jsToHash(params[0]));
	if (_state == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	j_response["result"] = toJson(*_state);
}

void mcp::rpc_handler::block_states(mcp::json &j_response, bool &)
{
	if (!params.is_array() || params.size() < 1)
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("Cannot wrap string value as a json-rpc type; not array type or incorrect number of arguments."));

	mcp::json states_l = mcp::json::array();
	for (mcp::json const &_p : params)
	{
		std::string _blockHash = _p;
		if (!mcp::isH256(_blockHash))
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

		auto _state = client()->blockState(jsToHash(_blockHash));
		mcp::json _tmp;
		if (_state == nullptr)
			_tmp[_blockHash] = nullptr;
		else
			_tmp[_blockHash] = toJson(*_state);
		states_l.push_back(_tmp);
	}
	j_response["result"] = states_l;
}

void mcp::rpc_handler::stable_blocks(mcp::json &j_response, bool &)
{
	//0: index, 1: limit
	uint64_t index = jsToULl(params[0], "index");
	uint64_t limit_l = jsToULl(params[1], "limit");
	if (limit_l > list_max_limit || !limit_l)///too big or zero.
		BOOST_THROW_EXCEPTION(RPC_Error_TooLargeSearchRange("query returned more than 100 results or limit zero."));

	uint64_t last_stable_index(client()->number());
	if (index > last_stable_index)///invalid index,bigger than stable index.
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("index bigger than max block number."));

	mcp::json block_list_l = mcp::json::array();
	int blocks_count(0);
	for (uint64_t stable_index = index; stable_index <= last_stable_index; stable_index++)
	{
		auto _block = client()->blockInfo(stable_index);
		block_list_l.push_back(toJson(*_block));
		blocks_count++;
		if (blocks_count == limit_l)
			break;
	}
	mcp::json result;

	result["blocks"] = block_list_l;

	uint64_t next_index = index + limit_l;
	if (next_index <= last_stable_index)
		result["next_index"] = next_index;
	else
		result["next_index"] = nullptr;
	j_response["result"] = result;
}

void mcp::rpc_handler::block_summary(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	dev::h256 hash = jsToHash(params[0]);
	auto _summary = client()->blockSummary(hash);
	if (!_summary)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	json result;
	result["summeries"] = toJS(*_summary);
	auto block(client()->blockInfo(hash));
	assert_x(block);
	auto block_state(client()->blockState(hash));
	assert_x(block_state);

	// previous summary hash
	mcp::summary_hash previous_summary_hash(0);
	if (block->previous() != dev::h256(0))
	{
		auto tmp = client()->blockSummary(block->previous());
		assert_x(tmp);
		previous_summary_hash = *tmp;
	}
	result["previous_summary"] = toJS(previous_summary_hash);

	// parent summary hashs
	mcp::json parent_summaries_l = mcp::json::array();
	for (mcp::block_hash const &pblock_hash : block->parents())
	{
		auto tmp = client()->blockSummary(pblock_hash);
		assert_x(tmp);
		parent_summaries_l.push_back(toJS(*tmp));
	}
	result["parent_summaries"] = parent_summaries_l;

	// skip list
	mcp::json skiplist_summaries_l = mcp::json::array();
	if (block_state->is_on_main_chain)
	{
		std::set<mcp::summary_hash> summary_skiplist;
		std::vector<uint64_t> skip_list_mcis = mcp::cal_skip_list_mcis(*block_state->main_chain_index);
		for (uint64_t &mci : skip_list_mcis)
		{
			auto _mciHash = client()->mciHash(mci);
			assert_x(_mciHash);
			mcp::block_hash sl_block_hash = *_mciHash;

			auto tmp = client()->blockSummary(sl_block_hash);
			assert_x(tmp);
			summary_skiplist.insert(*tmp);
		}

		for (mcp::summary_hash const& s : summary_skiplist)
			skiplist_summaries_l.push_back(toJS(s));
	}
	result["skiplist_summaries"] = skiplist_summaries_l;

	result["receiptsRoot"] = block_state->m_receiptsRoot.hexPrefixed();
	result["stateRoot"] = block_state->m_stateRoot.hexPrefixed();
	result["logBloom"] = block_state->m_logBloom.hexPrefixed();
	result["stableIndex"] = block_state->stable_index;
	result["mcTimestamp"] = block_state->mc_timestamp;
	result["mci"] = *block_state->main_chain_index;
	result["status"] = (uint64_t)block_state->status;
	j_response["result"] = result;
}

void mcp::rpc_handler::version(mcp::json &j_response, bool &)
{
	json result;
	result["version"] = STR(MCP_VERSION);
	result["rpc_version"] = "1";
	result["store_version"] = std::to_string(client()->storeVersion());
	j_response["result"] = result;
}

void mcp::rpc_handler::status(mcp::json &j_response, bool &)
{
	json result;
	result["syncing"] = mcp::node_sync::is_syncing() ? 1 : 0;
	result["last_stable_mci"] = client()->lastStableMci();
	result["last_mci"] = client()->lastMci();
	result["last_stable_block_index"] = client()->number();
	result["epoch"] = client()->lastEpoch();
	result["epoch_period"] = mcp::epoch_period;
	j_response["result"] = result;
}

void mcp::rpc_handler::peers(mcp::json &j_response, bool &)
{
	mcp::json peers_l = mcp::json::array();
	auto peers = client()->peers();
	for (auto const& i : peers)
	{
		mcp::json peer_l;
		peer_l["id"] = toJS(i.first);
		peer_l["endpoint"] = toJson(i.second);
		peers_l.push_back(peer_l);
	}
	j_response["result"] = peers_l;
}

void mcp::rpc_handler::nodes(mcp::json &j_response, bool &)
{
	mcp::json nodes_l = mcp::json::array();
	auto nodes(client()->nodes());
	for (auto const& node : nodes)
	{
		mcp::json node_l;
		node_l["id"] = toJS(node.id);
		node_l["endpoint"] = toJson((bi::tcp::endpoint)node.endpoint);
		nodes_l.push_back(node_l);
	}
	j_response["result"] = nodes_l;
}

void mcp::rpc_handler::witness_list(mcp::json &j_response, bool &)
{
	Epoch epoch = (uint64_t)jsToULl(params[0],"epoch");

	if (epoch > client()->lastEpoch())
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("The epoch has not yet completed."));
	
	auto _wl = client()->witnessList(epoch);
	j_response["result"] = toJson(_wl);
}

void mcp::rpc_handler::process_request()
{
    try
    {
		LOG(m_log.debug) << "REQUEST: " << body;
		std::pair<jsonrpcMessages, bool> batch = readBatch(body);
		if (batch.second)///batch
		{
			handleBatch(batch.first);
		}
		else
		{
			handleMsg(batch.first[0]);
		}		
    }
    catch (...)
    {
		mcp::json j_response;
		SetResponse(j_response);
		RPC_Error_JsonParseError("parse error").toJson(j_response);
		response(j_response);
    }
}

// handleBatch executes all messages in a batch and returns the responses.
void mcp::rpc_handler::handleBatch(mcp::jsonrpcMessages const& req)
{
	bool async = false;
	// Emit error response for empty batches:
	if (req.size() == 0)
	{
		mcp::json _res;
		SetResponse(_res);///set response rpc version.
		RPC_Error_InvalidRequest("empty batch").toJson(_res);
		response(_res);
		return;
	}

	mcp::json resp = mcp::json::array();
	for (const auto& req : req)
	{
		mcp::json answer = handleCallMsg(req, async);
		resp.push_back(answer);
	}
	response(resp);
}

// handleMsg handles a single message.
void mcp::rpc_handler::handleMsg(mcp::jsonrpcMessage const& req)
{
	bool async = true;
	mcp::json answer = handleCallMsg(req, async);
	if (async)
		response(answer);
}

// handleCallMsg executes a call message and returns the answer.
mcp::json mcp::rpc_handler::handleCallMsg(mcp::jsonrpcMessage const& req, bool& async)
{
	mcp::json _res;
	try
	{
		if (req.isCall())
		{
			req.SetResponse(_res);///set response rpc version and id.
			auto pointer = m_ethRpcMethods.find(req.Method);
			if (pointer == m_ethRpcMethods.end())
			{
				std::string _msg = "The method " + req.Method + " does not exist/is not available";
				BOOST_THROW_EXCEPTION(RPC_Error_MethodNotFound(_msg.c_str()));
			}

			params = req.Params;
			(this->*(pointer->second))(_res, async);
		}
		else if (req.hasValidID())///with id
		{
			req.SetResponse(_res);///set response rpc version and id.
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidRequest("invalid request"));
		}
		else
		{
			SetResponse(_res);///set response rpc version.
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidRequest("invalid request"));
		}
	}
	catch (mcp::RPC_Error_NoResult const& e)
	{
		_res["result"] = nullptr;
	}
	catch (mcp::RpcException const& e)
	{
		e.toJson(_res);
	}
	catch (std::exception const& e)
	{
		toRpcExceptionEthJson(e, _res);
	}
	catch (...)
	{
		RPC_Error_InternalError("internal error.").toJson(_res);
	}
	return _res;
}

void mcp::rpc_handler::eth_blockNumber(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(client()->number());
}

void mcp::rpc_handler::eth_getTransactionCount(mcp::json &j_response, bool &)
{
	if(!isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	BlockNumber blockTag = jsToBlockNumber(params[1]);
	if (blockTag == PendingBlock || blockTag == LatestBlock)
		j_response["result"] = toJS(m_wallet->getTransactionCount(jsToAddress(params[0]), blockTag));
	else
		j_response["result"] = toJS(client()->countAt(jsToAddress(params[0]), blockTag));
}

void mcp::rpc_handler::eth_chainId(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(mcp::chain_id);
}

void mcp::rpc_handler::eth_gasPrice(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(mcp::gas_price);
}

void mcp::rpc_handler::eth_estimateGas(mcp::json &j_response, bool &)
{
	TransactionSkeleton ts = mcp::toTransactionSkeletonForEth(params[0]);

	std::pair<u256, mcp::ExecutionResult> result = client()->estimateGas(ts.from,ts.value,ts.to,ts.data, static_cast<int64_t>(ts.gas),ts.gasPrice, PendingBlock);
	if (result.second.Failed())///execution failed
	{
		if (result.second.Revert().size())///revert
		{
			std::string msg = newRevertError(result.second);
			std::string data = toJS(result.second.Revert());
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(msg.c_str(), data.c_str()));
		}
		BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(result.second.ErrorMsg().c_str()));
	}

	j_response["result"] = toJS(result.first);
}

void mcp::rpc_handler::eth_getBlockByNumber(mcp::json &j_response, bool &)
{
	bool _includeTransactions = params[1].is_null() ? false : (bool)params[1];
	auto _block = client()->localisedBlock(jsToBlockNumber(params[0]));
	if (_block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	j_response["result"] = toJson(*_block, _includeTransactions);
}

void mcp::rpc_handler::eth_getBlockByHash(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	bool _includeTransactions = params[1].is_null() ? false : (bool)params[1];
	auto _block = client()->localisedBlock(jsToHash(params[0]));
	if (_block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	j_response["result"] = toJson(*_block, _includeTransactions);
}

void mcp::rpc_handler::eth_sendRawTransaction(mcp::json &j_response, bool &)
{
	Transaction t(jsToBytes(params[0], OnFailed::Throw), CheckTransaction::None);
	j_response["result"] = toJS(m_wallet->importTransaction(t));
}

void mcp::rpc_handler::eth_sendTransaction(mcp::json &j_response, bool &async)
{
	TransactionSkeleton t = mcp::toTransactionSkeletonForEth(params[0]);
	if (!async)///synchronize
	{
		try {
			h256 h = m_wallet->send_action(t, boost::none);
			j_response["result"] = toJS(h);
		}
		catch (dev::Exception const& e) {
			toRpcExceptionEthJson(e, j_response);
		}
		return;
	}

	auto rpc_l(shared_from_this());
	auto fun = [rpc_l, j_response, this](h256 &h, boost::optional<dev::Exception const &> e)
	{
		mcp::json j_resp = j_response;
		if (!e)
			j_resp["result"] = toJS(h);
		else
			toRpcExceptionEthJson(*e, j_resp);
		response(j_resp);
	};

	async = false;
	m_wallet->send_async(t, fun);
}

void mcp::rpc_handler::eth_call(mcp::json &j_response, bool &)
{
	TransactionSkeleton ts = mcp::toTransactionSkeletonForEth(params[0]);
	BlockNumberOrHash _b = toBlockNumberOrHash(params[1]);
	if (!_b.Number() && !_b.Hash())
		BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied("invalid arguments; neither block nor hash specified"));
	
	BlockNumber _bn;
	if (_b.Number())
	{
		_bn = *_b.Number();
		if (_bn != LatestBlock
			&& _bn != PendingBlock
			&& _bn > client()->number())
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied("header not found"));
	}
	else
	{
		auto _bs = client()->blockState(*_b.Hash());
		if (_bs == nullptr)
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied("header for hash not found"));
		_bn = _bs->stable_index;
	}
	
	mcp::ExecutionResult const& er = client()->call(ts.from, ts.value, ts.to, ts.data, ts.gas, ts.gasPrice, _bn);
	if (er.Failed())///execution failed
	{
		if (er.Revert().size())///revert
		{
			std::string msg = newRevertError(er);
			std::string data = toJS(er.Revert());
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(msg.c_str(), data.c_str()));
		}
		BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(er.ErrorMsg().c_str()));
	}

	j_response["result"] = toJS(er.output);
}

void mcp::rpc_handler::net_version(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(mcp::chain_id);
}

void mcp::rpc_handler::net_listening(mcp::json &j_response, bool &)
{
	j_response["result"] = client()->netListening();
}

void mcp::rpc_handler::net_peerCount(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(client()->peersCount());
}

void mcp::rpc_handler::web3_clientVersion(mcp::json &j_response, bool &)
{
	j_response["result"] = STR(MCP_VERSION);
}

void mcp::rpc_handler::web3_sha3(mcp::json &j_response, bool &)
{
	dev::bytes msg = jsToBytes(params[0], OnFailed::Throw);
	j_response["result"] = toJS(dev::sha3(msg));
}

void mcp::rpc_handler::eth_getCode(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	j_response["result"] = toJS(client()->codeAt(jsToAddress(params[0]), jsToBlockNumber(params[1])));
}

void mcp::rpc_handler::eth_getStorageAt(mcp::json &j_response, bool &)
{
	if(!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	j_response["result"] = toJS(toCompactBigEndian(client()->stateAt(jsToAddress(params[0]), jsToU256(params[1]), jsToBlockNumber(params[2])), 32));
}

void mcp::rpc_handler::eth_getStorageRoot(mcp::json& j_response, bool& async)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	j_response["result"] = toJS(client()->stateRootAt(jsToAddress(params[0]), jsToBlockNumber(params[1])));

}

void mcp::rpc_handler::eth_getTransactionByHash(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	try
	{
		j_response["result"] = toJson(client()->localisedTransaction(jsToHash(params[0])));
	}
	catch (TransactionNotFound)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	}
}

void mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	uint64_t index = jsToULl(params[1], "index");

	try
	{
		j_response["result"] = toJson(client()->localisedTransaction(jsToHash(params[0]), index));
	}
	catch (TransactionNotFound)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	}
}

void mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex(mcp::json &j_response, bool &)
{
	uint64_t index = jsToULl(params[1], "index");

	try
	{
		j_response["result"] = toJson(client()->localisedTransaction(jsToBlockNumber(params[0]), index));
	}
	catch (TransactionNotFound)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	}
}

void mcp::rpc_handler::eth_getTransactionReceipt(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	try
	{
		j_response["result"] = toJson(client()->localisedTransactionReceipt(jsToHash(params[0])));
	}
	catch (TransactionNotFound)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	}
}

void mcp::rpc_handler::eth_getBlockTransactionCountByHash(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	j_response["result"] = toJS(client()->transactionCount(jsToHash(params[0])));
}

void mcp::rpc_handler::eth_getBlockTransactionCountByNumber(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(client()->transactionCount(jsToBlockNumber(params[0])));
}

void mcp::rpc_handler::eth_getBalance(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	j_response["result"] = toJS(client()->balanceAt(jsToAddress(params[0]), jsToBlockNumber(params[1])));
}

void mcp::rpc_handler::eth_accounts(mcp::json &j_response, bool &)
{
	j_response["result"] = toJson(m_key_manager->list());
}

void mcp::rpc_handler::eth_sign(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::bytes data = jsToBytes(params[1], OnFailed::Throw);
	if (data.size() > mcp::max_data_size)
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("exceeds block data limit."));

	/// throw exception if locked or unknown.
	std::pair<bool, Secret> ar = m_key_manager->authenticate(jsToAddress(params[0]));
	j_response["result"] = toJS(dev::sign(ar.second, get_eth_signed_msg(data)));
}

void mcp::rpc_handler::eth_signTransaction(mcp::json &j_response, bool &)
{
	TransactionSkeleton ts = mcp::toTransactionSkeletonForEth(params[0]);
	/// throw exception if locked or unknown.
	std::pair<bool, Secret> ar = m_key_manager->authenticate(ts.from);
	m_wallet->populateTransactionWithDefaults(ts);

	Transaction t(ts, ar.second);
	j_response["result"] = toJS(t.rlp());
}

void mcp::rpc_handler::eth_protocolVersion(mcp::json &j_response, bool &)
{
	j_response["result"] = STR(MCP_VERSION);
}

void mcp::rpc_handler::eth_syncing(mcp::json &j_response, bool &)
{
	if (!mcp::node_sync::is_syncing())
	{
		j_response["result"] = false;
		return;
	}

	mcp::json result;
	result["startingBlock"] = toJS(client()->lastStableMci());
	result["currentBlock"] = toJS(client()->lastMci());
	result["highestBlock"] = toJS(client()->number());

	j_response["result"] = result;
}

void mcp::rpc_handler::eth_getLogs(mcp::json &j_response, bool &)
{
	try
	{
		j_response["result"] = toJson(client()->logs(toLogFilter(params[0])));
	}
	catch (QueryRangeTooLarge)
	{
		//-32005 query returned more than 10000 results
		BOOST_THROW_EXCEPTION(RPC_Error_TooLargeSearchRange("Query Returned More Than 2000 Results"));
	}
}

void mcp::rpc_handler::personal_importRawKey(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::Secret prv = dev::Secret(params[0].get<std::string>());
	std::string password = params[1];
	if (password.empty() ||
		!mcp::validatePasswordSize(password) ||
		!mcp::validatePassword(password))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("the password needs 8 to 100 characters and cannot contain invalid characters."));
	}

	mcp::key_content kc = m_key_manager->importRawKey(prv, password);
	j_response["result"] = toJS(kc.address);
}

void mcp::rpc_handler::personal_listAccounts(mcp::json &j_response, bool &)
{
	j_response["result"] = toJson(m_key_manager->list());
}

void mcp::rpc_handler::personal_lockAccount(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::Address account = jsToAddress(params[0]);
	if (!m_key_manager->exists(account))
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(AddressNotExist));

	m_key_manager->lock(account);
	j_response["result"] = true;
}

void mcp::rpc_handler::personal_newAccount(mcp::json &j_response, bool &)
{
	std::string password = params[0];
	if (password.empty() ||
		!mcp::validatePasswordSize(password) ||
		!mcp::validatePassword(password))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("the password needs 8 to 100 characters and cannot contain invalid characters."));
	}

	dev::Address account = m_key_manager->create(password);
	j_response["result"] = toJS(account);
}

void mcp::rpc_handler::personal_unlockAccount(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::Address account = jsToAddress(params[0]);
	if (!m_key_manager->exists(account))
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(AddressNotExist));

	j_response["result"] = m_key_manager->unlock(account, params[1]);
}

void mcp::rpc_handler::personal_sendTransaction(mcp::json &j_response, bool &async)
{
	TransactionSkeleton t = mcp::toTransactionSkeletonForEth(params[0]);
	std::string password = params[1];

	if (!async)///synchronize
	{
		try {
			h256 h = m_wallet->send_action(t, password);
			j_response["result"] = toJS(h);
		}
		catch (dev::Exception const& e) {
			toRpcExceptionEthJson(e, j_response);
		}
		return;
	}

	auto rpc_l(shared_from_this());
	auto fun = [rpc_l, j_response, this](h256 &h, boost::optional<dev::Exception const &> e)
	{
		mcp::json j_resp = j_response;
		if (!e)
			j_resp["result"] = toJS(h);
		else
			toRpcExceptionEthJson(*e, j_resp);
		response(j_resp);
	};

	async = false;
	m_wallet->send_async(t, fun, password);
}

void mcp::rpc_handler::personal_sign(mcp::json &j_response, bool &)
{
	dev::bytes data = jsToBytes(params[0], OnFailed::Throw);
	if (data.size() > mcp::max_data_size)
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("exceeds block data limit."));

	if (!mcp::isAddress(params[1]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	std::pair<bool, Secret> _k = m_key_manager->DecryptKey(jsToAddress(params[1]), params[2]);
	if (!_k.first)
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("authentication needed: passphrase or unlock."));

	dev::Signature signature = dev::sign(_k.second, get_eth_signed_msg(data));
	j_response["result"] = toJS(signature);
}

void mcp::rpc_handler::personal_ecRecover(mcp::json &j_response, bool &)
{
	dev::bytes data = jsToBytes(params[0], OnFailed::Throw);
	if (data.size() > mcp::max_data_size)
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Data"));

	dev::Signature sig(0);
	try
	{
		sig = jsToSignature(params[1]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Signiture"));
	}

	dev::Address from = dev::toAddress(dev::recover(sig, get_eth_signed_msg(data)));
	j_response["result"] = toJS(from);
}

dev::h256 mcp::rpc_handler::get_eth_signed_msg(dev::bytes &data)
{
	dev::bytes msg;
	std::string prefix = "Ethereum Signed Message:\n" + std::to_string(data.size());
	msg.resize(prefix.size() + data.size() + 1);

	msg[0] = 0x19;
	dev::bytesRef((unsigned char *)prefix.data(), prefix.size()).copyTo(dev::bytesRef(msg.data() + 1, prefix.size()));
	dev::bytesRef(data.data(), data.size()).copyTo(dev::bytesRef(msg.data() + prefix.size() + 1, data.size()));

	return dev::sha3(msg);
}

void mcp::rpc_handler::epoch_approves(mcp::json &j_response, bool &)
{
	Epoch epoch = (uint64_t)jsToULl(params[0], "epoch");

	if (epoch > client()->lastEpoch())
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("The epoch has not yet completed."));

	j_response["result"] = toJson(client()->epochApproves(epoch));
}

void mcp::rpc_handler::epoch_work_transaction(mcp::json &j_response, bool &)
{
	Epoch epoch = (uint64_t)jsToULl(params[0], "epoch");

	auto _h = client()->workTransactionHash(epoch);
	if (_h)
		j_response["result"] = toJS(*_h);
	else
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("The epoch has not yet completed."));
}

void mcp::rpc_handler::approve_receipt(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	auto _ar = client()->approveReceipt(jsToHash(params[0]));
	if (nullptr == _ar)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	j_response["result"] = toJson(*_ar);
}


void mcp::rpc_handler::debug_traceTransaction(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	dev::h256 _hash(jsToHash(params[0]));

	try
	{
		LocalisedTransaction t = client()->localisedTransaction(_hash);
		if (t.blockHash() == dev::h256())
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied("transaction not found"));
		if (!t.blockNumber())//genesis
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied("genesis is not traceable"));

		Block block = client()->blockByHash(t.blockHash(), true);
		chain_state s(chain_state::Null);
		mcp::ExecutionResult er;
		std::shared_ptr<Tracer> _tracer = NewTracer(params[1], er);
		Executive e(s, block, t.transactionExecIndex(), client()->blockChain(), _tracer);
		e.setResultRecipient(er);
		traceTransaction(e, t);

		j_response["result"] = _tracer->GetResult();
	}
	catch (TransactionNotFound)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied("transaction not found"));
	}
}

void mcp::rpc_handler::traceTransaction(mcp::Executive& _e, mcp::Transaction const& _t)
{
	_e.initialize(_t);
	if (!_e.execute())
		_e.go();
	_e.finalize();
}

mcp::chain_state mcp::rpc_handler::stateAt(std::string const& _blockHashOrNumber, int _txIndex) const
{
	if (_txIndex > 4096)
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidRequest("Negative index"));

	mcp::Block _block;
	if (mcp::isH256(_blockHashOrNumber))
		_block = m_client->blockByHash(h256(_blockHashOrNumber), true);
	else
		_block = m_client->blockByNumber(jsToBlockNumber(_blockHashOrNumber), true);

	auto const txCount = _block.pending().size();
	mcp::chain_state state(mcp::chain_state::Null);

	if (static_cast<size_t>(_txIndex) < txCount)
		createIntermediateState(state, _block, _txIndex, m_client->blockChain());
	else if (static_cast<size_t>(_txIndex) == txCount)
		// the final state of block (after applying rewards)
		state = _block.state();
	else
	{
		auto msg = "Transaction index " + toString(_txIndex) + " out of range (" + toString(txCount) + ") for block " + _blockHashOrNumber;
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidRequest(msg.c_str()));
	}

	return state;
}

void mcp::rpc_handler::debug_accountRange(mcp::json& j_response, bool& async)
{
	auto ret = mcp::json::object();

	//auto _maxResults = jsToULl(params[3]);
	//if (_maxResults <= 0)
	//	BOOST_THROW_EXCEPTION(RPC_Error_TooLargeSearchRange("Nonpositive maxResults"));

	mcp::chain_state const state = stateAt(params[0], jsToULl(params[1]));
	auto const addressMap = state.addresses(jsToHash(params[2]), jsToULl(params[3]));

	auto addressList = mcp::json::object();
	for (auto const& record : addressMap.first)
		addressList[toHexPrefixed(record.first)] = toHexPrefixed(record.second);

	ret["addressMap"] = addressList;
	ret["nextKey"] = toHexPrefixed(addressMap.second);

	j_response["result"] = ret;
}
