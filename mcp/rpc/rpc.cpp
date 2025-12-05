#include "rpc.hpp"
#include "connection.hpp"

mcp::rpc::rpc(std::shared_ptr<mcp::key_manager> key_manager_a,
			  std::shared_ptr<mcp::wallet> wallet_a,
			  std::shared_ptr<mcp::async_task> background_a,
			  std::shared_ptr<mcp::Client> client_a,
			  boost::asio::io_service &service_a, mcp::rpc_config const &config_a) : 
	m_key_manager(key_manager_a),
	m_wallet(wallet_a),
	m_background(background_a),
	m_client(client_a),
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
			// Retry accepting to prevent service stoppage
			accept();
		} 
	});
}

void mcp::rpc::stop()
{
	acceptor.close();
}

std::shared_ptr<mcp::rpc> mcp::get_rpc(std::shared_ptr<mcp::key_manager> key_manager_a,
									   std::shared_ptr<mcp::wallet> wallet_a,
									   std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::Client> client_a,
									   boost::asio::io_service &service_a, mcp::rpc_config const &config_a)
{
	std::shared_ptr<rpc> impl(new rpc(key_manager_a, wallet_a, background_a, client_a, service_a, config_a));
	return impl;
}
