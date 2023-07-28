#pragma once

#include <boost/asio.hpp>
#include <mcp/common/mcp_json.hpp>

namespace mcp
{
	class rpc_config
	{
	public:
		rpc_config();
		void serialize_json(mcp::json &) const;
		bool deserialize_json(mcp::json const &);
		bool parse_old_version_data(mcp::json const &, uint64_t const&);

		boost::asio::ip::address address;
		uint16_t port;
		bool rpc_enable;
	};
}
