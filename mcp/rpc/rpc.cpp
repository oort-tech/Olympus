#include "rpc.hpp"
#include "connection.hpp"

mcp::rpc::rpc(mcp::block_store &store_a, std::shared_ptr<mcp::chain> chain_a,
			  std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
			  std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
			  std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::composer> composer_a,
			  boost::asio::io_service &service_a, mcp::rpc_config const &config_a) : m_store(store_a),
																					 m_chain(chain_a),
																					 m_cache(cache_a),
																					 m_key_manager(key_manager_a),
																					 m_wallet(wallet_a),
																					 m_host(host_a),
																					 m_background(background_a),
																					 m_composer(composer_a),
																					 io_service(service_a),
																					 acceptor(service_a),
																					 config(config_a)
{
}

void mcp::rpc::start()
{
	auto endpoint(bi::tcp::endpoint(config.address, config.port));
	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

	boost::system::error_code ec;
	acceptor.bind(endpoint, ec);
	if (ec)
	{
		LOG(m_log.error) << boost::str(boost::format("Error while binding for HTTP RPC on port %1%: %2%") % endpoint.port() % ec.message());
		throw std::runtime_error(ec.message());
	}

	acceptor.listen();

	LOG(m_log.info) << "HTTP RPC started, http://" << endpoint;

	accept();
}

void mcp::rpc::accept()
{
	auto connection(std::make_shared<mcp::rpc_connection>(*this));
	acceptor.async_accept(connection->socket, [this, connection](boost::system::error_code const &ec)
	{
		if (!ec)
		{
			accept();
			connection->parse_connection();
		}
		else
		{
            LOG(this->m_log.error) << "Error accepting HTTP RPC connections:" << ec.message();
		} 
	});
}

void mcp::rpc::stop()
{
	acceptor.close();
}

std::shared_ptr<mcp::rpc> mcp::get_rpc(mcp::block_store &store_a, std::shared_ptr<mcp::chain> chain_a,
									   std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
									   std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
									   std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::composer> composer_a,
									   boost::asio::io_service &service_a, mcp::rpc_config const &config_a)
{
	std::shared_ptr<rpc> impl(new rpc(store_a, chain_a, cache_a, key_manager_a, wallet_a, host_a, background_a, composer_a, service_a, config_a));
	return impl;
}
