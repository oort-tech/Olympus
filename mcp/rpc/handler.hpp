#pragma once

#include "rpc.hpp"
#include "json.hpp"
#include <mcp/node/tracers/Tracer.hpp>

namespace mcp
{
	template <class I> using AbstractRPCMethodPointer = void(I::*)(mcp::json &, bool &);

	class rpc_handler : public std::enable_shared_from_this<mcp::rpc_handler>
	{
		using RPCMethodPointer = AbstractRPCMethodPointer<rpc_handler>;

	public:
		rpc_handler(mcp::rpc &, std::string const &, std::function<void(mcp::json const &)>const &);
		void process_request();

		void account_remove(mcp::json & j_response, bool & async);
		void account_import(mcp::json & j_response, bool & async);
		void accounts_balances(mcp::json & j_response, bool & async);

		void block(mcp::json & j_response, bool & async);
		void block_state(mcp::json & j_response, bool & async);
		void block_states(mcp::json & j_response, bool & async);
		void stable_blocks(mcp::json & j_response, bool & async);

		void block_summary(mcp::json & j_response, bool & async);

		void version(mcp::json & j_response, bool & async);
		void status(mcp::json & j_response, bool & async);
		void peers(mcp::json & j_response, bool & async);
		void nodes(mcp::json & j_response, bool & async);
		void witness_list(mcp::json & j_response, bool & async);

		dev::h256 get_eth_signed_msg(dev::bytes & data);

		void web3_clientVersion(mcp::json & j_response, bool & async);
		void web3_sha3(mcp::json & j_response, bool & async);

		void eth_blockNumber(mcp::json & j_response, bool & async);
		void eth_getTransactionCount(mcp::json & j_response, bool & async);
		void eth_chainId(mcp::json & j_response, bool & async);
		void eth_gasPrice(mcp::json & j_response, bool & async);
		void eth_estimateGas(mcp::json & j_response, bool & async);
		void eth_getBlockByNumber(mcp::json & j_response, bool & async);
		void eth_getBlockTransactionCountByHash(mcp::json & j_response, bool & async);
		void eth_getBlockTransactionCountByNumber(mcp::json & j_response, bool & async);
		void eth_sendRawTransaction(mcp::json & j_response, bool & async);
		void eth_sendTransaction(mcp::json & j_response, bool & async);
		void eth_call(mcp::json & j_response, bool & async);
		void net_version(mcp::json & j_response, bool & async);
		void net_listening(mcp::json & j_response, bool & async);
		void net_peerCount(mcp::json & j_response, bool & async);
		void eth_protocolVersion(mcp::json & j_response, bool & async);
		void eth_syncing(mcp::json & j_response, bool & async);
		void eth_getLogs(mcp::json & j_response, bool & async);
		// related to the upgrades
		void eth_getCode(mcp::json & j_response, bool & async);
		void eth_getStorageAt(mcp::json & j_response, bool & async);
		void eth_getStorageRoot(mcp::json& j_response, bool& async);
		void eth_getTransactionByHash(mcp::json & j_response, bool & async);
		//
		void eth_getTransactionByBlockHashAndIndex(mcp::json & j_response, bool & async);
		void eth_getTransactionByBlockNumberAndIndex(mcp::json & j_response, bool & async);
		void eth_getTransactionReceipt(mcp::json & j_response, bool & async);
		void eth_getBalance(mcp::json & j_response, bool & async);
		// related to the metamask
		void eth_getBlockByHash(mcp::json & j_response, bool & async);
		void eth_accounts(mcp::json & j_response, bool & async);
		void eth_sign(mcp::json & j_response, bool & async);
		void eth_signTransaction(mcp::json & j_response, bool & async);
		void debug_traceTransaction(mcp::json & j_response, bool & async);
		// related to personal
		void personal_importRawKey(mcp::json & j_response, bool & async);
		void personal_listAccounts(mcp::json & j_response, bool & async);
		void personal_lockAccount(mcp::json & j_response, bool & async);
		void personal_newAccount(mcp::json & j_response, bool & async);
		void personal_unlockAccount(mcp::json & j_response, bool & async);
		void personal_sendTransaction(mcp::json & j_response, bool & async);
		void personal_sign(mcp::json & j_response, bool & async);
		void personal_ecRecover(mcp::json & j_response, bool & async);
		void epoch_approves(mcp::json &j_response, bool &);
		void epoch_work_transaction(mcp::json &j_response, bool &);
		void approve_receipt(mcp::json &j_response, bool &);

		//////////
		void debug_accountRange(mcp::json& j_response, bool& async);

		std::string body;
		mcp::rpc & rpc;

		static const uint32_t list_max_limit = 100;

		mcp::json params;
		std::function<void(mcp::json const&)> response;

	private:
		void handleBatch(mcp::jsonrpcMessages const& req);
		void handleMsg(mcp::jsonrpcMessage const& req);
		mcp::json handleCallMsg(mcp::jsonrpcMessage const& req, bool& async);
		std::shared_ptr<mcp::Client> client() { return m_client; }
		void traceTransaction(mcp::Executive& _e, mcp::Transaction const& _t);
		mcp::chain_state stateAt(std::string const& _blockHashOrNumber, int _txIndex) const;

		std::shared_ptr<mcp::key_manager> m_key_manager;
		std::shared_ptr<mcp::wallet> m_wallet;
		std::shared_ptr<mcp::Client> m_client;
		std::map<std::string, RPCMethodPointer> m_ethRpcMethods;

		mcp::log m_log = { mcp::log("rpc") };
	};
}