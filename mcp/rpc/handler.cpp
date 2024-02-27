#include "handler.hpp"
#include "exceptions.hpp"
#include "jsonHelper.hpp"
#include <mcp/core/genesis.hpp>
#include <mcp/core/param.hpp>
#include <mcp/common/pwd.hpp>
#include <mcp/node/evm/Executive.hpp>

mcp::rpc_handler::rpc_handler(mcp::rpc &rpc_a, std::string const &body_a, std::function<void(mcp::json const &)> const &response_a, int m_cap) : body(body_a),
																																				 rpc(rpc_a),
																																				 response(response_a),
																																				 m_chain(rpc_a.m_chain),
																																				 m_cache(rpc_a.m_cache),
																																				 m_key_manager(rpc_a.m_key_manager),
																																				 m_wallet(rpc_a.m_wallet),
																																				 m_host(rpc_a.m_host),
																																				 m_composer(rpc_a.m_composer),
																																				 m_background(rpc_a.m_background),
																																				 m_store(rpc.m_store)
{
	m_ethRpcMethods["account_remove"] = &mcp::rpc_handler::account_remove;
	m_ethRpcMethods["account_import"] = &mcp::rpc_handler::account_import;
	m_ethRpcMethods["accounts_balances"] = &mcp::rpc_handler::accounts_balances;
	m_ethRpcMethods["block"] = &mcp::rpc_handler::block;
	m_ethRpcMethods["block_state"] = &mcp::rpc_handler::block_state;
	m_ethRpcMethods["block_states"] = &mcp::rpc_handler::block_states;
	m_ethRpcMethods["block_traces"] = &mcp::rpc_handler::block_traces;
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
	m_ethRpcMethods["eth_getTransactionByHash"] = &mcp::rpc_handler::eth_getTransactionByHash;
	m_ethRpcMethods["eth_getTransactionByBlockHashAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex;
	m_ethRpcMethods["eth_getTransactionByBlockNumberAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex;
	m_ethRpcMethods["eth_getTransactionReceipt"] = &mcp::rpc_handler::eth_getTransactionReceipt;
	m_ethRpcMethods["eth_getBalance"] = &mcp::rpc_handler::eth_getBalance;
	m_ethRpcMethods["eth_getBlockByHash"] = &mcp::rpc_handler::eth_getBlockByHash;
	m_ethRpcMethods["eth_accounts"] = &mcp::rpc_handler::eth_accounts;
	m_ethRpcMethods["eth_sign"] = &mcp::rpc_handler::eth_sign;
	m_ethRpcMethods["eth_signTransaction"] = &mcp::rpc_handler::eth_signTransaction;
	//m_ethRpcMethods["debug_traceTransaction"] = &mcp::rpc_handler::debug_traceTransaction;
	//m_ethRpcMethods["debug_storageRangeAt"] = &mcp::rpc_handler::debug_storageRangeAt;

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


bool mcp::rpc_handler::try_get_mc_info(dev::eth::McInfo &mc_info_a, uint64_t &block_number)
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash block_hash;
	bool exists(!m_cache->block_number_get(transaction, block_number, block_hash));
	if (!exists)
		return false;

	std::shared_ptr<mcp::block_state> mc_state(m_cache->block_state_get(transaction, block_hash));
	assert_x(mc_state);
	assert_x(mc_state->is_stable);
	assert_x(mc_state->main_chain_index);
	assert_x(mc_state->mc_timestamp > 0);

	uint64_t last_summary_mci(0);
	if (block_hash != mcp::genesis::block_hash)
	{
		std::shared_ptr<mcp::block> mc_block(m_cache->block_get(transaction, block_hash));
		assert_x(mc_block);
		std::shared_ptr<mcp::block_state> last_summary_state(m_cache->block_state_get(transaction, mc_block->last_summary_block()));
		assert_x(last_summary_state);
		assert_x(last_summary_state->is_stable);
		assert_x(last_summary_state->is_on_main_chain);
		assert_x(last_summary_state->main_chain_index);
		last_summary_mci = *last_summary_state->main_chain_index;
	}

	mc_info_a = dev::eth::McInfo(mc_state->stable_index, *mc_state->main_chain_index, mc_state->mc_timestamp, last_summary_mci);

	return true;
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

	//0: account list
	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
	for (mcp::json const &j_account : params)
	{
		std::string account_text = j_account;
		if (!mcp::isAddress(account_text))
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

		dev::Address account(account_text);
		auto balance(c_state.balance(account));
		mcp::json acc_balance;
		acc_balance[account_text] = balance.str();
		j_balances.push_back(acc_balance);
	}

	j_response["result"] = j_balances;
}

void mcp::rpc_handler::block(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::h256 block_hash = jsToHash(params[0]);
	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, block_hash));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	j_response["result"] = toJson(*block);
}

void mcp::rpc_handler::block_state(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::h256 block_hash = jsToHash(params[0]);
	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::shared_ptr<mcp::block_state> state(m_store.block_state_get(transaction, block_hash));
	if (state == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	j_response["result"] = toJson(*state);
}

void mcp::rpc_handler::block_states(mcp::json &j_response, bool &)
{
	if (!params.is_array() || params.size() < 1)
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("Cannot wrap string value as a json-rpc type; not array type or incorrect number of arguments."));

	mcp::json states_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	for (mcp::json const &_p : params)
	{
		std::string _blockHash = _p;
		if (!mcp::isH256(_blockHash))
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

		dev::h256 block_hash = jsToHash(_blockHash);
		std::shared_ptr<mcp::block_state> state(m_store.block_state_get(transaction, block_hash));
		mcp::json _tmp;
		if (state == nullptr)
			_tmp[_blockHash] = nullptr;
		else
			_tmp[_blockHash] = toJson(*state);
		states_l.push_back(_tmp);
	}
	j_response["result"] = states_l;
}

void mcp::rpc_handler::block_traces(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::h256 block_hash = jsToHash(params[0]);
	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::list<std::shared_ptr<mcp::trace>> traces;
	m_store.traces_get(transaction, block_hash, traces);

	mcp::json traces_l = mcp::json::array();
	std::deque<uint32_t> trace_address;
	for (auto it(traces.begin()); it != traces.end(); it++)
	{
		std::shared_ptr<mcp::trace> trace(*it);
		mcp::json trace_l;
		trace->serialize_json(trace_l);

		uint32_t const &depth(trace->depth);

		uint32_t sub_traces(0);
		auto it_temp = it;
		while (++it_temp != traces.end())
		{
			std::shared_ptr<mcp::trace> t(*it_temp);
			if (t->depth <= depth)
				break;

			if (t->depth == depth + 1)
				sub_traces++;
		}
		trace_l["subtraces"] = sub_traces;

		if (depth > 0)
		{
			if (trace_address.size() < depth)
			{
				assert_x(trace_address.size() == depth - 1);
				trace_address.push_back(0);
			}
			else
			{
				trace_address[depth - 1] += 1;
				while (trace_address.size() > depth)
					trace_address.pop_back();
			}
		}
		trace_l["trace_address"] = trace_address;

		traces_l.push_back(trace_l);
	}

	j_response["result"] = traces_l;
}

void mcp::rpc_handler::stable_blocks(mcp::json &j_response, bool &)
{
	//0: index, 1: limit
	uint64_t index = jsToULl(params[0], "index");
	uint64_t limit_l = jsToULl(params[1], "limit");
	if (limit_l > list_max_limit || !limit_l)///too big or zero.
		BOOST_THROW_EXCEPTION(RPC_Error_TooLargeSearchRange("query returned more than 100 results or limit zero."));

	uint64_t last_stable_index(m_chain->last_stable_index());
	if (index > last_stable_index)///invalid index,bigger than stable index.
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("index bigger than max block number."));

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::json block_list_l = mcp::json::array();
	int blocks_count(0);
	for (uint64_t stable_index = index; stable_index <= last_stable_index; stable_index++)
	{
		mcp::block_hash block_hash_l;
		assert_x(!m_store.stable_block_get(transaction, stable_index, block_hash_l));

		auto block = m_cache->block_get(transaction, block_hash_l);
		assert_x(block);
		block_list_l.push_back(toJson(*block));
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
	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::summary_hash summary;
	if (m_cache->block_summary_get(transaction, hash, summary))
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	
	json result;
	result["summeries"] = toJS(summary);
	auto block(m_cache->block_get(transaction, hash));
	assert_x(block);
	auto block_state(m_cache->block_state_get(transaction, hash));
	assert_x(block_state);

	// previous summary hash
	mcp::summary_hash previous_summary_hash(0);
	if (block->previous() != dev::h256(0))
	{
		bool previous_summary_hash_error(m_cache->block_summary_get(transaction, block->previous(), previous_summary_hash));
		assert_x(!previous_summary_hash_error);
	}
	result["previous_summary"] = toJS(previous_summary_hash);

	// parent summary hashs
	mcp::json parent_summaries_l = mcp::json::array();
	for (mcp::block_hash const &pblock_hash : block->parents())
	{
		mcp::summary_hash p_summary_hash;
		bool p_summary_hash_error(m_cache->block_summary_get(transaction, pblock_hash, p_summary_hash));
		assert_x(!p_summary_hash_error);

		parent_summaries_l.push_back(toJS(p_summary_hash));
	}
	result["parent_summaries"] = parent_summaries_l;

	///High performance overhead.
	/*/// receiptsRoot hash
	std::vector<bytes> receipts;
	for (auto _h : block->links())
	{
	auto receipt = m_cache->transaction_receipt_get(transaction, _h);
	if (receipt)/// transaction maybe processed yet,but summary need used receipt even if it has been processed.
	{
	RLPStream receiptRLP;
	receipt->streamRLP(receiptRLP);
	receipts.push_back(receiptRLP.out());
	}
	}
	for (auto _h : block->links())
	{
	auto receipt = m_cache->approve_receipt_get(transaction, _h);
	if (receipt)
	{
	RLPStream receiptRLP;
	receipt->streamRLP(receiptRLP);
	receipts.push_back(receiptRLP.out());
	}
	}
	h256 receiptsRoot = dev::orderedTrieRoot(receipts);
	j_response["receiptsRoot"] = receiptsRoot.hexPrefixed();*/

	// skip list
	mcp::json skiplist_summaries_l = mcp::json::array();
	if (block_state->is_on_main_chain)
	{
		std::set<mcp::summary_hash> summary_skiplist;
		std::vector<uint64_t> skip_list_mcis = m_chain->cal_skip_list_mcis(*block_state->main_chain_index);
		for (uint64_t &mci : skip_list_mcis)
		{
			mcp::block_hash sl_block_hash;
			bool sl_block_hash_error(m_store.main_chain_get(transaction, mci, sl_block_hash));
			assert_x(!sl_block_hash_error);

			mcp::summary_hash sl_summary_hash;
			bool sl_summary_hash_error(m_cache->block_summary_get(transaction, sl_block_hash, sl_summary_hash));
			assert_x(!sl_summary_hash_error);
			summary_skiplist.insert(sl_summary_hash);
		}

		for (mcp::summary_hash s : summary_skiplist)
			skiplist_summaries_l.push_back(toJS(s));
	}
	result["skiplist_summaries"] = skiplist_summaries_l;

	result["status"] = (uint64_t)block_state->status;
	j_response["result"] = result;
}

void mcp::rpc_handler::version(mcp::json &j_response, bool &)
{
	json result;
	result["version"] = STR(MCP_VERSION);
	result["rpc_version"] = "1";
	result["store_version"] = std::to_string(m_store.version_get());
	j_response["result"] = result;
}

void mcp::rpc_handler::status(mcp::json &j_response, bool &)
{
	// stable_mci
	uint64_t last_stable_mci(m_chain->last_stable_mci());
	uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());

	json result;
	result["syncing"] = mcp::node_sync::is_syncing() ? 1 : 0;
	result["last_stable_mci"] = last_stable_mci;
	result["last_mci"] = last_mci;
	result["last_stable_block_index"] = last_stable_index;
	result["epoch"] = m_chain->last_epoch();
	result["epoch_period"] = mcp::epoch_period;
	j_response["result"] = result;
}

void mcp::rpc_handler::peers(mcp::json &j_response, bool &)
{
	mcp::json peers_l = mcp::json::array();
	std::unordered_map<p2p::node_id, bi::tcp::endpoint> peers(m_host->peers());
	for (auto i : peers)
	{
		p2p::node_id id(i.first);
		bi::tcp::endpoint endpoint(i.second);

		mcp::json peer_l;
		peer_l["id"] = toJS(id);
		std::stringstream ss_endpoint;
		ss_endpoint << endpoint;
		peer_l["endpoint"] = ss_endpoint.str();
		peers_l.push_back(peer_l);
	}
	j_response["result"] = peers_l;
}

void mcp::rpc_handler::nodes(mcp::json &j_response, bool &)
{
	mcp::json nodes_l = mcp::json::array();
	std::list<p2p::node_info> nodes(m_host->nodes());
	for (p2p::node_info node : nodes)
	{
		mcp::json node_l;
		node_l["id"] = toJS(node.id);
		std::stringstream ss_endpoint;
		ss_endpoint << (bi::tcp::endpoint)node.endpoint;
		node_l["endpoint"] = ss_endpoint.str();
		nodes_l.push_back(node_l);
	}
	j_response["result"] = nodes_l;
}

void mcp::rpc_handler::witness_list(mcp::json &j_response, bool &)
{
	Epoch epoch = (uint64_t)jsToULl(params[0],"epoch");

	if (epoch > m_chain->last_epoch())
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("The epoch has not yet completed."));

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::witness_param const &w_param(mcp::param::witness_param(transaction, epoch));
	mcp::json witness_list_l = mcp::json::array();
	for (auto i : w_param.witness_list)
	{
		witness_list_l.push_back(i.hexPrefixed());
	}
	j_response["result"] = witness_list_l;
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
	j_response["result"] = toJS(m_chain->last_stable_index());
}

void mcp::rpc_handler::eth_getTransactionCount(mcp::json &j_response, bool &)
{
	if(!isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	BlockNumber blockTag = LatestBlock;
	if (params.size() >= 2)
		blockTag = jsToBlockNumber(params[1]);

	j_response["result"] = toJS(m_wallet->getTransactionCount(jsToAddress(params[0]), blockTag));
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

	dev::eth::McInfo mc_info;
	uint64_t block_number = m_chain->last_stable_index();
	if (!try_get_mc_info(mc_info, block_number))
		BOOST_THROW_EXCEPTION(RPC_Error_InternalError("internal error: block number not stable."));

	mc_info.mc_timestamp = mcp::seconds_since_epoch();

	mcp::db::db_transaction transaction(m_store.create_transaction());

	std::pair<u256, mcp::ExecutionResult> result = m_chain->estimate_gas(
		transaction,
		m_cache,
		ts.from,
		ts.value,
		ts.to,
		ts.data,
		static_cast<int64_t>(ts.gas),
		ts.gasPrice,
		mc_info);
	
	mcp::ExecutionResult executionResult = result.second;
	if (executionResult.Failed())///execution failed
	{
		if (executionResult.Revert().size())///revert
		{
			std::string msg = newRevertError(executionResult);
			std::string data = toJS(executionResult.Revert());
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(msg.c_str(), data.c_str()));
		}
		BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(executionResult.ErrorMsg().c_str()));
	}

	j_response["result"] = toJS(result.first);
}

void mcp::rpc_handler::eth_getBlockByNumber(mcp::json &j_response, bool &)
{
	BlockNumber block_number = jsToBlockNumber(params[0]);
	if (block_number == LatestBlock || block_number == PendingBlock)
		block_number = m_chain->last_stable_index();

	bool is_full = params[1].is_null() ? false : (bool)params[1];

	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, block_number));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto state = m_cache->block_state_get(transaction, block->hash());
	mcp::Transactions txs;
	int index = 0;
	for (auto& th : block->links())
	{
		auto t = m_cache->transaction_get(transaction, th);
		txs.push_back(*t);
	}

	mcp::summary_hash stateRoot;/// stateRoot
	m_cache->block_summary_get(transaction, block->hash(), stateRoot);
	dev::h256 receiptsRoot;/// receiptsRoot
	m_store.GetBlockReceiptsRoot(transaction, block->hash(), receiptsRoot);

	mcp::LocalisedBlock lb = mcp::LocalisedBlock(*block,
		block_number,
		txs,
		stateRoot,
		receiptsRoot,
		state->best_parent
	);

	j_response["result"] = toJson(lb, is_full);
}

void mcp::rpc_handler::eth_getBlockByHash(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	mcp::block_hash block_hash = jsToHash(params[0]);
	bool is_full = params[1].is_null() ? false : (bool)params[1];

	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block = m_cache->block_get(transaction, block_hash);
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	auto state = m_cache->block_state_get(transaction, block_hash);
	BlockNumber block_number = *state->main_chain_index;

	mcp::Transactions txs;
	int index = 0;
	for (auto &th : block->links())
	{
		auto t = m_cache->transaction_get(transaction, th);
		txs.push_back(*t);
	}

	mcp::summary_hash stateRoot;/// stateRoot
	m_cache->block_summary_get(transaction, block->hash(), stateRoot);
	dev::h256 receiptsRoot;/// receiptsRoot
	m_store.GetBlockReceiptsRoot(transaction, block->hash(), receiptsRoot);

	mcp::LocalisedBlock lb = mcp::LocalisedBlock(*block, 
		block_number, 
		txs,
		stateRoot,
		receiptsRoot,
		state->best_parent
	);

	j_response["result"] = toJson(lb, is_full);
}

void mcp::rpc_handler::eth_sendRawTransaction(mcp::json &j_response, bool &)
{
	Transaction t(jsToBytes(params[0], OnFailed::Throw), CheckTransaction::None);

	//LOG(m_log.info) << "m_nonce:" << t.nonce() 
	//	<< " ,m_value:" << t.value() 
	//	<< " ,m_receiveAddress:" << t.receiveAddress().hex()
	//	<< " ,m_gasPrice:" << t.gasPrice()
	//	<< " ,m_gas:" << t.gas()
	//	<< " ,m_data:" << toHex(t.data())
	//	<< " ,m_vrs v:" << (int)t.signature().v
	//	<< " ,m_vrs r:" << t.signature().r.hex()
	//	<< " ,m_vrs s:" << t.signature().s.hex()
	//	<< " ,m_chainId:" << t.chainID();

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
	ts.gasPrice = 0;
	ts.gas = mcp::tx_max_gas;
	if (ts.nonce == Invalid256)
		ts.nonce = m_wallet->getTransactionCount(ts.from);

	Transaction t(ts);
	t.setSignature(h256(0), h256(0), 0);

	BlockNumber block_number = jsToBlockNumber(params[1]);
	if (block_number == LatestBlock || block_number == PendingBlock)
		block_number = m_chain->last_stable_index();

	dev::eth::McInfo mc_info;
	if (!try_get_mc_info(mc_info, block_number))
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("block not found."));

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = m_chain->execute(
		transaction,
		m_cache,
		t,
		mc_info,
		Permanence::Uncommitted,
		dev::eth::OnOpFunc());

	mcp::ExecutionResult executionResult = result.first;
	if (executionResult.Failed())///execution failed
	{
		if (executionResult.Revert().size())///revert
		{
			std::string msg = newRevertError(executionResult);
			std::string data = toJS(executionResult.Revert());
			BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(msg.c_str(), data.c_str()));
		}
		BOOST_THROW_EXCEPTION(RPC_Error_RequestDenied(executionResult.ErrorMsg().c_str()));
	}

	j_response["result"] = toJS(result.first.output);
}

void mcp::rpc_handler::net_version(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(mcp::chain_id);
}

void mcp::rpc_handler::net_listening(mcp::json &j_response, bool &)
{
	j_response["result"] = m_host->is_started();
}

void mcp::rpc_handler::net_peerCount(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(m_host->get_peers_count());
}

void mcp::rpc_handler::web3_clientVersion(mcp::json &j_response, bool &)
{
	j_response["result"] = STR(MCP_VERSION);
}

void mcp::rpc_handler::web3_sha3(mcp::json &j_response, bool &)
{
	try {
		dev::bytes msg = jsToBytes(params[0], OnFailed::Throw);
		j_response["result"] = toJS(dev::sha3(msg));
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("cannot wrap string value as a json-rpc type; the \"input\" contains invalid hex character."));
	}
}

void mcp::rpc_handler::eth_getCode(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
	j_response["result"] = toJS(c_state.code(jsToAddress(params[0])));
}

void mcp::rpc_handler::eth_getStorageAt(mcp::json &j_response, bool &)
{
	if(!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	dev::Address account = jsToAddress(params[0]);
	uint256_t position = jsToU256(params[1]);

	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
	j_response["result"] = toJS(c_state.storage(account, position));
}

void mcp::rpc_handler::eth_getTransactionByHash(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	h256 hash = jsToHash(params[0]);

	auto transaction = m_store.create_transaction();
	auto t = m_cache->transaction_get(transaction, hash);
	if (t == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto lt = LocalisedTransaction(*t, mcp::block_hash(0), 0, 0);
	mcp::json j_transaction = toJson(lt);

	auto td = m_cache->transaction_address_get(transaction, hash);
	if (td == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	j_transaction["blockHash"] = toJS(td->blockHash);
	j_transaction["transactionIndex"] = toJS(td->index);

	uint64_t block_number = 0;
	if (!m_cache->block_number_get(transaction, td->blockHash, block_number))
		j_transaction["blockNumber"] = toJS(block_number);

	j_response["result"] = j_transaction;
}

void mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	mcp::block_hash block_hash = jsToHash(params[0]);
	uint64_t index = jsToULl(params[1], "index");

	auto transaction = m_store.create_transaction();
	auto block(m_cache->block_get(transaction, block_hash));
	uint64_t block_number;
	if (block == nullptr ||
		m_cache->block_number_get(transaction, block_hash, block_number) ||
		index >= block->links().size())
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	dev::h256 hash = block->links().at(index);
	auto t = m_cache->transaction_get(transaction, hash);
	if (t == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto lt = LocalisedTransaction(*t, block->hash(), index, block_number);
	j_response["result"] = toJson(lt);
}

void mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex(mcp::json &j_response, bool &)
{
	BlockNumber block_number = jsToBlockNumber(params[0]);
	if (block_number == LatestBlock || block_number == PendingBlock)
		block_number = m_chain->last_stable_index();

	uint64_t index = jsToULl(params[1], "index");
	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash block_hash;
	if (m_cache->block_number_get(transaction, block_number, block_hash))
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto block(m_cache->block_get(transaction, block_hash));
	if (block == nullptr || index >= block->links().size())
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	dev::h256 hash = block->links().at(index);
	auto t = m_cache->transaction_get(transaction, hash);
	if (t == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto lt = LocalisedTransaction(*t, block_hash, index, block_number);
	j_response["result"] = toJson(lt);
}

void mcp::rpc_handler::eth_getTransactionReceipt(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	h256 hash = jsToHash(params[0]);
	auto transaction = m_store.create_transaction();
	auto t = m_cache->transaction_get(transaction, hash);
	auto tr = m_store.transaction_receipt_get(transaction, hash);
	auto td = m_cache->transaction_address_get(transaction, hash);

	if (t == nullptr || tr == nullptr || td == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	uint64_t block_number = 0;
	if (m_cache->block_number_get(transaction, td->blockHash, block_number))
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto lt = dev::eth::LocalisedTransactionReceipt(
		*tr,
		t->sha3(),
		td->blockHash,
		block_number,
		t->from(),
		t->to(),
		td->index,
		toAddress(t->from(), t->nonce()));

	j_response["result"] = toJson(lt);
}

void mcp::rpc_handler::eth_getBlockTransactionCountByHash(mcp::json &j_response, bool &)
{
	if(!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	mcp::block_hash block_hash = jsToHash(params[0]);

	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, block_hash));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	j_response["result"] = toJS(block->links().size());
}

void mcp::rpc_handler::eth_getBlockTransactionCountByNumber(mcp::json &j_response, bool &)
{
	BlockNumber block_number = jsToBlockNumber(params[0]);
	if (block_number == LatestBlock || block_number == PendingBlock)
		block_number = m_chain->last_stable_index();

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash block_hash;
	if (m_cache->block_number_get(transaction, block_number, block_hash))
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto block(m_cache->block_get(transaction, block_hash));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	j_response["result"] = toJS(block->links().size());
}

void mcp::rpc_handler::eth_getBalance(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
	j_response["result"] = toJS(c_state.balance(jsToAddress(params[0])));
}

void mcp::rpc_handler::eth_accounts(mcp::json &j_response, bool &)
{
	mcp::json j_accounts = mcp::json::array();
	Addresses account_list(m_key_manager->list());
	for (auto account : account_list)
		j_accounts.push_back(toJS(account));

	j_response["result"] = j_accounts;
}

void mcp::rpc_handler::eth_sign(mcp::json &j_response, bool &)
{
	if (!mcp::isAddress(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	dev::Address account = jsToAddress(params[0]);
	if (!m_key_manager->exists(account))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(AddressNotExist));

	dev::bytes data = jsToBytes(params[1]);
	if (data.size() > mcp::max_data_size)
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("exceeds block data limit."));

	/// throw exception if locked or unknown.
	std::pair<bool, Secret> ar = m_key_manager->authenticate(account);

	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Signature signature = dev::sign(ar.second, hash);
	j_response["result"] = toJS(signature);
}

void mcp::rpc_handler::eth_signTransaction(mcp::json &j_response, bool &)
{
	TransactionSkeleton ts = mcp::toTransactionSkeletonForEth(params[0]);
	if (!m_key_manager->exists(ts.from))
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(AddressNotExist));
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

	uint64_t last_stable_mci(m_chain->last_stable_mci());
	uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());

	mcp::json result;
	result["startingBlock"] = toJS(last_stable_mci);
	result["currentBlock"] = toJS(last_mci);
	result["highestBlock"] = toJS(last_stable_index);

	j_response["result"] = result;
}

void mcp::rpc_handler::eth_getLogs(mcp::json &j_response, bool &)
{
	params = params[0];
	std::unordered_set<dev::Address> search_address;
	if (params.count("address"))///is_null() or empty() invalid for string of json.
	{
		if (params["address"].is_string())
		{
			if(!mcp::isAddress(params["address"]))
				BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
			dev::Address _a = jsToAddress(params["address"]);
			if (_a != dev::ZeroAddress)
				search_address.insert(_a);
		}
		else if (params["address"].is_array())
		{
			std::vector<std::string> address_l = params["address"];
			for (std::string const &address_text : address_l)
			{
				if(!mcp::isAddress(address_text))
					BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
				dev::Address _a = jsToAddress(address_text);
				if (_a != dev::ZeroAddress)
					search_address.insert(_a);
			}
		}
		else
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	}

	std::unordered_set<dev::h256> search_topics;
	if (params.count("topics"))
	{
		if (!params["topics"].is_array())
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError("cannot wrap string value as a json-rpc type; topics needs array type."));

		std::vector<std::string> topics_l = params["topics"];
		for (std::string const &topic_text : topics_l)
		{
			if(!mcp::isH256(topic_text))
				BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
			search_topics.insert(jsToHash(topic_text));
		}
	}

	mcp::json logs_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());
	/// If we're doing singleton block filtering, execute and return
	if (params.count("blockhash"))///is_null() or empty() invalid for string of json.
	{
		if (!mcp::isH256(params["blockhash"]))
			BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

		mcp::block_hash block_hash = jsToHash(params["blockhash"]);
		auto state = m_cache->block_state_get(transaction, block_hash);
		if (!state || !state->is_stable)
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("block not exist or not stable."));

		auto block = m_cache->block_get(transaction, block_hash);
		for (auto &th : block->links())
		{
			auto t = m_cache->transaction_get(transaction, th);
			auto tr = m_store.transaction_receipt_get(transaction, th);
			auto td = m_cache->transaction_address_get(transaction, th);
			if (t == nullptr || tr == nullptr || td == nullptr)
				continue;
			if (td->blockHash != block_hash) {///not first linked, ignore.
				continue;
			}

			auto lt = dev::eth::LocalisedTransactionReceipt(
				*tr,
				t->sha3(),
				block_hash,
				state->stable_index,
				t->from(),
				t->to(),
				td->index,
				toAddress(t->from(), t->nonce()),
				search_address,
				search_topics);
			for (auto localisedLog : lt.localisedLogs()) {
				mcp::json log = toJson(localisedLog);
				logs_l.push_back(log);
			}
		}
		j_response["result"] = logs_l;
		return;
	}

	BlockNumber fromBlock;
	if (params.count("fromBlock"))
	{
		fromBlock = jsToBlockNumber(params["fromBlock"]);
		if (fromBlock == LatestBlock || fromBlock == PendingBlock)
			fromBlock = m_chain->last_stable_index();
	}

	BlockNumber toBlock;
	if (params.count("toBlock"))
	{
		toBlock = jsToBlockNumber(params["toBlock"]);
		if (toBlock == LatestBlock || toBlock == PendingBlock)
			toBlock = m_chain->last_stable_index();
	}

	if (toBlock - fromBlock + 1 > 200)///max 200
		BOOST_THROW_EXCEPTION(RPC_Error_TooLargeSearchRange("Query Returned More Than 200 Results"));//-32005 query returned more than 10000 results

	for (uint64_t i(fromBlock); i <= toBlock; i++)
	{
		mcp::block_hash block_hash(0);
		if (m_store.stable_block_get(transaction, i, block_hash))
			break;

		auto block_state = m_cache->block_state_get(transaction, block_hash);
		if (!block_state || !block_state->is_stable)
			break;

		auto block = m_cache->block_get(transaction, block_hash);
		for (auto &th : block->links())
		{
			auto t = m_cache->transaction_get(transaction, th);
			auto tr = m_store.transaction_receipt_get(transaction, th);
			auto td = m_cache->transaction_address_get(transaction, th);
			if (t == nullptr || tr == nullptr || td == nullptr)
				continue;
			if (td->blockHash != block_hash) {///not first linked, ignore.
				continue;
			}

			auto lt = dev::eth::LocalisedTransactionReceipt(
				*tr,
				t->sha3(),
				block_hash,
				i,
				t->from(),
				t->to(),
				td->index,
				toAddress(t->from(), t->nonce()),
				search_address,
				search_topics);
			for (auto localisedLog : lt.localisedLogs()) {
				mcp::json log = toJson(localisedLog);
				logs_l.push_back(log);
			}
		}
	}

	j_response["result"] = logs_l;
}

//void mcp::rpc_handler::debug_traceTransaction(mcp::json &j_response, bool &)
//{
//	
//	std::string hash_text = params[0];
//	dev::h256 hash;
//	hash = jsToHash(hash_text);
//
//	mcp::db::db_transaction transaction(m_store.create_transaction());
//	auto _t = m_cache->transaction_get(transaction, hash);
//	auto td = m_cache->transaction_address_get(transaction, hash);
//
//	if (_t == nullptr || td == nullptr)
//	{
//		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Hash"));
//	}
//
//	dev::eth::McInfo mc_info;
//	if (!m_chain->get_mc_info_from_block_hash(transaction, m_cache, td->blockHash, mc_info))
//	{
//		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Mci"));
//	}
//	mcp::json options;
//	options["disableStorage"] = true;
//	options["disableMemory"] = false;
//	options["disableStack"] = false;
//	options["full_storage"] = false;
//
//	mcp::json options_json = params[1];
//	if (options_json.count("disableStorage"))
//		options["disableStorage"] = options_json["disableStorage"];
//	if (options_json.count("disableMemory"))
//		options["disableMemory"] = options_json["disableMemory"];
//	if (options_json.count("disableStack"))
//		options["disableStack"] = options_json["disableStack"];
//	if (options_json.count("full_storage"))
//		options["full_storage"] = options_json["full_storage"];
//
//	try
//	{
//		dev::eth::EnvInfo env(transaction, m_store, m_cache, mc_info, mcp::chain_id);
//		auto block(m_cache->block_get(transaction, td->blockHash));
//		assert_x(block);
//		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
//		std::vector<h256> accout_state_hashs;
//		if(!m_store.transaction_previous_account_state_get(transaction, hash, accout_state_hashs))
//		{
//			BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Hash"));
//		}
//		c_state.ts = *_t;
//		c_state.set_defalut_account_state(accout_state_hashs);
//
//		//c_state should be used after set_defalut_account_state. Otherwise, account_state will be abnormal.
//		if (!_t->isCreation() && !c_state.addressHasCode(_t->receiveAddress()))
//		{
//			j_response["return_value"] = "Only contract transcation can debug.";
//			return;
//		}
//		mcp::ExecutionResult er;
//		std::list<std::shared_ptr<mcp::trace>> traces;
//		mcp::Executive e(c_state, env, traces);
//		e.setResultRecipient(er);
//
//		mcp::json trace = m_chain->traceTransaction(e, *_t, options);
//		j_response["return_value"] = toHexPrefixed(er.output);
//		j_response["struct_logs"] = trace;
//	}
//	catch (Exception const &_e)
//	{
//		BOOST_THROW_EXCEPTION(RPC_Error_InternalError("Unexpected exception in VM. There may be a bug in this implementation."));
//	}
//	catch (std::exception const &_e)
//	{
//		BOOST_THROW_EXCEPTION(RPC_Error_InternalError("Unknown Error"));
//	}
//}
//
//void mcp::rpc_handler::debug_storageRangeAt(mcp::json &j_response, bool &)
//{
//	//this should be a json object, not an array
//	dev::Address acct(0);
//	if (!params.count("account") || !params["account"].is_string())
//	{
//		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Account"));
//	}
//	std::string account_text = params["account"];
//	if (!mcp::isAddress(params["account"]))
//	{
//		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Account"));
//	}
//	acct = dev::Address(account_text);
//
//	h256 begin;
//	begin = jsToHash(params["begin"]);
//	BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Begin"));
//
//	uint64_t max_results(0);
//	if (!params.count("max_results"))
//	{
//		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("Invalid Begin"));
//	}
//	max_results = jsToULl(params["max_results"]);
//
//	j_response["result"] = mcp::json::object();
//
//	try
//	{
//		mcp::db::db_transaction transaction(m_store.create_transaction());
//		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
//
//		std::map<h256, std::pair<u256, u256>> const storage(c_state.storage(acct));
//
//		// begin is inclusive
//		auto itBegin = storage.lower_bound(begin);
//		for (auto it = itBegin; it != storage.end(); ++it)
//		{
//			if (j_response["result"].size() == static_cast<unsigned>(max_results))
//			{
//				j_response["next_key"] = toCompactHexPrefixed(it->first, 32);
//				break;
//			}
//
//			mcp::json keyValue = mcp::json::object();
//			std::string hashedKey = toCompactHexPrefixed(it->first, 32);
//			keyValue["key"] = toCompactHexPrefixed(it->second.first, 32);
//			keyValue["value"] = toCompactHexPrefixed(it->second.second, 32);
//
//			j_response["result"][hashedKey] = keyValue;
//		}
//	}
//	catch (Exception const &_e)
//	{
//		cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
//			<< diagnostic_information(_e);
//		exit(1);
//	}
//}

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
	mcp::json j_accounts = mcp::json::array();
	Addresses account_list(m_key_manager->list());
	for (auto account : account_list)
		j_accounts.push_back(toJS(account));

	j_response["result"] = j_accounts;
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
	dev::bytes data = jsToBytes(params[0]);
	if (data.size() > mcp::max_data_size)
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("exceeds block data limit."));

	if (!mcp::isAddress(params[1]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));
	dev::Address account = jsToAddress(params[1]);
	if (!m_key_manager->exists(account))
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams(AddressNotExist));

	std::pair<bool, Secret> _k = m_key_manager->DecryptKey(account, params[2]);
	if (!_k.first)
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("authentication needed: passphrase or unlock."));

	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Signature signature = dev::sign(_k.second, hash);
	j_response["result"] = toJS(signature);
}

void mcp::rpc_handler::personal_ecRecover(mcp::json &j_response, bool &)
{
	dev::bytes data = jsToBytes(params[0]);
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

	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Address from = dev::toAddress(dev::recover(sig, hash));
	j_response["result"] = toJS(from);
}

void mcp::rpc_handler::get_eth_signed_msg(dev::bytes &data, dev::h256 &hash)
{
	dev::bytes msg;
	std::string prefix = "Ethereum Signed Message:\n" + std::to_string(data.size());
	msg.resize(prefix.size() + data.size() + 1);

	msg[0] = 0x19;
	dev::bytesRef((unsigned char *)prefix.data(), prefix.size()).copyTo(dev::bytesRef(msg.data() + 1, prefix.size()));
	dev::bytesRef(data.data(), data.size()).copyTo(dev::bytesRef(msg.data() + prefix.size() + 1, data.size()));

	hash = dev::sha3(msg);
}

void mcp::rpc_handler::epoch_approves(mcp::json &j_response, bool &)
{
	Epoch epoch = (uint64_t)jsToULl(params[0], "epoch");

	if (epoch > m_chain->last_epoch())
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("The epoch has not yet completed."));

	mcp::json approves_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::list<h256> hashs;
	m_store.epoch_approves_get(transaction, epoch, hashs);

	for (auto hash : hashs)
	{
		auto approve = m_cache->approve_get(transaction, hash);
		if (approve) {
			mcp::json approve_l;
			approve_l["hash"] = toJS(approve->sha3());
			approve_l["from"] = toJS(approve->sender());
			approve_l["proof"] = toJS(approve->proof());
			approves_l.push_back(approve_l);
		}
	}
	j_response["result"] = approves_l;
}

void mcp::rpc_handler::epoch_work_transaction(mcp::json &j_response, bool &)
{
	Epoch epoch = (uint64_t)jsToULl(params[0], "epoch");

	if (epoch >= m_chain->last_epoch())
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidParams("The epoch has not yet completed."));

	mcp::db::db_transaction transaction(m_store.create_transaction());
	h256 _h;
	m_store.epoch_work_transaction_get(transaction, epoch, _h);

	j_response["result"] = toJS(_h);
}

void mcp::rpc_handler::approve_receipt(mcp::json &j_response, bool &)
{
	if (!mcp::isH256(params[0]))
		BOOST_THROW_EXCEPTION(RPC_Error_JsonParseError(BadHexFormat));

	dev::h256 hash = jsToHash(params[0]);

	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto _a = m_cache->approve_receipt_get(transaction, hash);
	if (_a == nullptr)
		BOOST_THROW_EXCEPTION(RPC_Error_NoResult());
	j_response["result"] = toJson(*_a);
}
