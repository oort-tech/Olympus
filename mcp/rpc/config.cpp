#include "config.hpp"

mcp::rpc_config::rpc_config() : address(boost::asio::ip::address_v4::loopback()),
													 port(8766),
													 rpc_enable(false)
{
}

void mcp::rpc_config::serialize_json(mcp::json &json_a) const
{
	json_a["rpc"] = rpc_enable ? "true" : "false";
	json_a["rpc_addr"] = address.to_string();
	json_a["rpc_port"] = port;
}

bool mcp::rpc_config::deserialize_json(mcp::json const &json_a)
{
	auto error(false);
	try
	{
		if (!error)
		{
			if (json_a.count("rpc") && json_a["rpc"].is_string())
			{
				rpc_enable = (json_a["rpc"].get<std::string>() == "true" ? true : false);
			}
			else
			{
				error = true;
			}

			if (json_a.count("rpc_addr") && json_a["rpc_addr"].is_string())
			{
				std::string address_text = json_a["rpc_addr"].get<std::string>();
				boost::system::error_code ec;
				address = boost::asio::ip::address::from_string(address_text, ec);
				if (ec)
				{
					error = true;
				}
			}
			else
			{
				error = true;
			}

			if (json_a.count("rpc_port") && json_a["rpc_port"].is_number_unsigned())
			{
				uint64_t port_l = json_a["rpc_port"].get<uint64_t>();
				if (port_l <= std::numeric_limits<uint16_t>::max())
				{
					port = port_l;
				}
				else
				{
					error = true;
				}
			}
			else
			{
				error = true;
			}
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

bool mcp::rpc_config::parse_old_version_data(mcp::json const &json_a, uint64_t const &version)
{
	auto error(false);
	try
	{
		/// parse json used low version
		switch (version)
		{
			//case 0:
			//{
			//	/// parse
			//	break;
			//}
			//case 1:
			//{
			//	/// parse
			//	break;
			//}
		default:
			error |= deserialize_json(json_a);
			break;
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

