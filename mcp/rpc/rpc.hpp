#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <unordered_map>
#include <libevm/ExtVMFace.h>

#include <mcp/common/utility.hpp>
#include <mcp/common/log.hpp>
#include <mcp/common/mcp_json.hpp>
#include <mcp/db/database.hpp>
#include <mcp/node/chain.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/wallet/wallet.hpp>

namespace mcp
{

class rpc_config
{
public:
	rpc_config ();
    void serialize_json(mcp::json &) const;
    bool deserialize_json(mcp::json const &);
    bool parse_old_version_data(mcp::json const &, uint64_t const&);

	boost::asio::ip::address address;
	uint16_t port;
    bool rpc_enable;
};

class rpc
{
public:
	rpc (mcp::block_store & store_a, std::shared_ptr<mcp::chain> chain_a,
		std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
		std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
		std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::composer> composer_a,
		boost::asio::io_service & service_a, mcp::rpc_config const& config_a);
	void start ();
	virtual void accept ();
	void stop ();
	boost::asio::io_service &io_service;
	boost::asio::ip::tcp::acceptor acceptor;
	std::mutex mutex;
	mcp::rpc_config config;
	std::shared_ptr<mcp::chain> m_chain;
	std::shared_ptr<mcp::block_cache> m_cache;
	std::shared_ptr<mcp::key_manager> m_key_manager;
	std::shared_ptr<mcp::wallet> m_wallet;
	std::shared_ptr<mcp::p2p::host> m_host;
	std::shared_ptr<mcp::async_task> m_background;
	std::shared_ptr<mcp::composer> m_composer;
	mcp::block_store m_store;
    mcp::log m_log = { mcp::log("rpc") };
};

class rpc_connection : public std::enable_shared_from_this<mcp::rpc_connection>
{
public:
	rpc_connection (mcp::rpc &);
	virtual void parse_connection ();
	virtual void read ();
	virtual void write_result (std::string body, unsigned version);
	mcp::rpc & rpc;
	boost::asio::ip::tcp::socket socket;
	boost::beast::flat_buffer buffer;
	boost::beast::http::request<boost::beast::http::string_body> request;
	boost::beast::http::response<boost::beast::http::string_body> res;
	std::atomic_flag responded;
    mcp::log m_log = { mcp::log("rpc") };
};

template <class I> using AbstractRPCMethodPointer = void(I::*)(mcp::json &, bool &);

class rpc_handler : public std::enable_shared_from_this<mcp::rpc_handler>
{
	using RPCMethodPointer = AbstractRPCMethodPointer<rpc_handler>;

public:
    rpc_handler(mcp::rpc &, std::string const &, std::function<void(mcp::json const &)>const & ,int m_cap);
    void process_request();

	void account_remove(mcp::json & j_response, bool & async);
	void account_export(mcp::json & j_response, bool & async);
	void account_import(mcp::json & j_response, bool & async);
	void accounts_balances(mcp::json & j_response, bool & async);

	void block(mcp::json & j_response, bool & async);
	void block_state(mcp::json & j_response, bool & async);
	void block_states(mcp::json & j_response, bool & async);
	void block_traces(mcp::json & j_response, bool & async);
	void stable_blocks(mcp::json & j_response, bool & async);

	void block_summary(mcp::json & j_response, bool & async);

	void version(mcp::json & j_response, bool & async);
	void status(mcp::json & j_response, bool & async);
	void peers(mcp::json & j_response, bool & async);
	void nodes(mcp::json & j_response, bool & async);
	void witness_list(mcp::json & j_response, bool & async);

	void debug_storage_range_at(mcp::json & j_response, bool & async);

	void get_eth_signed_msg(dev::bytes & data, dev::h256 & hash);
	bool try_get_mc_info(dev::eth::McInfo &mc_info_a, uint64_t &block_number);

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

	std::string body;
	mcp::rpc & rpc;

	static const uint32_t list_max_limit = 100;

    mcp::json request;
    std::function<void(mcp::json const& )> response;

private:
	std::shared_ptr<mcp::chain> m_chain;
	std::shared_ptr<mcp::block_cache> m_cache;
	std::shared_ptr<mcp::key_manager> m_key_manager;
	std::shared_ptr<mcp::wallet> m_wallet;
	std::shared_ptr<mcp::p2p::host> m_host;
	std::shared_ptr<mcp::composer> m_composer;
	std::shared_ptr<mcp::async_task> m_background;
	mcp::block_store m_store;

	std::map<std::string, RPCMethodPointer> m_mcpRpcMethods;
	std::map<std::string, RPCMethodPointer> m_ethRpcMethods;

    mcp::log m_log = { mcp::log("rpc") };
};
/** Returns the correct RPC implementation based on TLS configuration */
std::shared_ptr<mcp::rpc> get_rpc (mcp::block_store & store_a, std::shared_ptr<mcp::chain> chain_a,
	std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
	std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
	std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::composer> composer_a,
	boost::asio::io_service & service_a, mcp::rpc_config const& config_a);
}
