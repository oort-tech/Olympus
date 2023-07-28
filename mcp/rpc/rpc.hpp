#pragma once

#include "config.hpp"
#include <mcp/wallet/key_manager.hpp>
#include <mcp/wallet/wallet.hpp>

namespace mcp
{


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

/** Returns the correct RPC implementation based on TLS configuration */
std::shared_ptr<mcp::rpc> get_rpc (mcp::block_store & store_a, std::shared_ptr<mcp::chain> chain_a,
	std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
	std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
	std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::composer> composer_a,
	boost::asio::io_service & service_a, mcp::rpc_config const& config_a);
}
