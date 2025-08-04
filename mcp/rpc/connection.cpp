#include "connection.hpp"
#include "handler.hpp"
#include "exceptions.hpp"

const std::size_t maxRequestContentLength = 1024 * 1024 * 5;
std::unordered_set<std::string> acceptedContentTypes = { "application/json", "application/json-rpc", "application/jsonrequest" };

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
				auto version(this_l->request.version());
                auto response_handler([this_l, version](mcp::json const & js)
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

				// Permit dumb empty requests for remote health-checks
				if (this_l->request.method() == boost::beast::http::verb::get &&
					getContentLength(this_l->request) == 0 &&
					this_l->request.target() == "/")
				{
					this_l->response("", version, boost::beast::http::status::ok);
					return;
				}

				auto validateCode = validateRequest(this_l->request);
				if (validateCode.first != boost::beast::http::status::ok)
				{
					this_l->response(validateCode.second, version, validateCode.first);
					return;
				}
				
				auto handler(std::make_shared<mcp::rpc_handler>(this_l->rpc, this_l->request.body(), response_handler/*, 0*/));
				handler->process_request();
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
	auto this_l(shared_from_this());
	try
	{
		write_result(body, version, status);
		boost::beast::http::async_write(this_l->socket, this_l->res, [this_l](boost::system::error_code const & e, size_t size)
		{
		});
	}
	catch (std::exception const & e)
	{
		LOG(m_log.error) << "rpc http write error:" << e.what() << "," << boost::stacktrace::stacktrace();
		throw "";
	}

	
}


std::pair<boost::beast::http::status, std::string> mcp::validateRequest(boost::beast::http::request<boost::beast::http::string_body>const& request)
{
	auto _handler = [](boost::beast::http::status const& _s) -> 
		std::pair<boost::beast::http::status, std::string> 
	{
		return std::make_pair(_s, boost::beast::http::obsolete_reason(_s));
	};

	if (request.method() != boost::beast::http::verb::post)
		return _handler(boost::beast::http::status::method_not_allowed);

	if (getContentLength(request) > maxRequestContentLength)
		return _handler(boost::beast::http::status::payload_too_large);

	/// Check content-type
	if (request.find(boost::beast::http::field::content_type) != request.end()) {
		std::string contentType = request[boost::beast::http::field::content_type];
		if (acceptedContentTypes.count(contentType))
		{
			return _handler(boost::beast::http::status::ok);
		}
	}
	/// Invalid content-type
	std::string _msg = "invalid content type, only application/json is supported";
	return std::make_pair(boost::beast::http::status::unsupported_media_type, _msg);
}

std::size_t mcp::getContentLength(boost::beast::http::request<boost::beast::http::string_body> const& request)
{
	std::size_t contentLength = 0;
	//if (request.find(boost::beast::http::field::content_length) != request.end())
	if (request.has_content_length())
	{
		try
		{
			std::string contentLengthStr = request[boost::beast::http::field::content_length];
			contentLength = std::stoull(contentLengthStr);
		}
		catch (const std::exception&)
		{
		}
	}
	return contentLength;
}
