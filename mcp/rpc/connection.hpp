#pragma once

#include "rpc.hpp"
#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <mcp/common/log.hpp>

namespace mcp
{
	class rpc_connection : public std::enable_shared_from_this<mcp::rpc_connection>
	{
	public:
		rpc_connection(mcp::rpc &);
		virtual void parse_connection();
		virtual void read();
		boost::asio::ip::tcp::socket socket;
	private:
		virtual void write_result(std::string body, unsigned version, boost::beast::http::status status);
		void response(std::string const & body, unsigned version, boost::beast::http::status status);
		mcp::rpc & rpc;
		boost::beast::flat_buffer buffer;
		boost::beast::http::request<boost::beast::http::string_body> request;
		boost::beast::http::response<boost::beast::http::string_body> res;
		std::atomic_flag responded;
		mcp::log m_log = { mcp::log("rpc") };
	};
}


