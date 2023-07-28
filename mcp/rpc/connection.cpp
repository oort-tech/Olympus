#include "connection.hpp"
#include "handler.hpp"
#include "exceptions.hpp"
#include <iostream>

mcp::rpc_connection::rpc_connection(mcp::rpc &rpc_a) : rpc(rpc_a), socket(rpc_a.io_service)
{
	responded.clear();
}

void mcp::rpc_connection::parse_connection()
{
	read();
}

void mcp::rpc_connection::write_result(std::string body, unsigned version, boost::beast::http::status status)
{
	if (!responded.test_and_set())
	{
		res.set("Content-Type", "application/json");
		res.set("Access-Control-Allow-Origin", "*");
		res.set("Access-Control-Allow-Headers", "Accept, Accept-Language, Content-Language, Content-Type");
		res.set("Connection", "close");
		res.result(status);
		res.body() = body;
		res.version(version);
		res.prepare_payload();
	}
	else
	{
		assert_x(false && "HTTP RPC already responded and should only respond once");
		// Guards `res' from being clobbered while async_write is being serviced
	}
}

void mcp::rpc_connection::read()
{
	auto this_l(shared_from_this());
	boost::beast::http::async_read(socket, buffer, request, [this_l](boost::system::error_code const &ec, size_t bytes_transferred)
	{
		if (!ec)
		{
			this_l->rpc.m_background->sync_async([this_l]() {
				//auto start(std::chrono::steady_clock::now());
				auto version(this_l->request.version());
                auto response_handler([this_l, version/*, start*/](mcp::json const & js)
				{
					try
					{
                        std::string body = js.dump();
						LOG(this_l->m_log.debug) << "RESPONSE:" << body;
						this_l->write_result(body, version, boost::beast::http::status::ok);
						boost::beast::http::async_write(this_l->socket, this_l->res, [this_l](boost::system::error_code const & e, size_t size)
						{
						});
					}
					catch (std::exception const & e)
					{
                        LOG(this_l->m_log.error) << "rpc http write error:" << e.what() << "," << boost::stacktrace::stacktrace();
						throw "";
					}
				});

				if (this_l->request.method() == boost::beast::http::verb::post)
				{
					try
					{
						auto handler(std::make_shared<mcp::rpc_handler>(this_l->rpc, this_l->request.body(), response_handler, 0));
						handler->process_request();
					}
					catch (mcp::RpcHttpException const & e)
					{
						std::string str = std::to_string((unsigned)e.status()) + " " + 
							boost::beast::http::obsolete_reason(e.status()).to_string();						
						if (std::strlen(e.what()))
							str = str + ": " + e.what();
						this_l->response(str, version, e.status());
					}
				}
				else
				{
					boost::beast::http::status code = boost::beast::http::status::not_found;
					std::string str = std::to_string((unsigned)code) + " " + boost::beast::http::obsolete_reason(code).to_string();
					this_l->response(str, version, boost::beast::http::status::not_found);
				}
			});
		}
		else
		{
            LOG(this_l->m_log.error) << "HTTP RPC read error: " << ec.message();
		} 
	});
}

void mcp::rpc_connection::response(std::string const & body, unsigned version, boost::beast::http::status status)
{
	try
	{
		write_result(body, version, status);
		boost::beast::http::async_write(socket, res, [](boost::system::error_code const & e, size_t size)
		{
		});
	}
	catch (std::exception const & e)
	{
		LOG(m_log.error) << "rpc http write error:" << e.what() << "," << boost::stacktrace::stacktrace();
		throw "";
	}

	
}



