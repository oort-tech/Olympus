#include "common.hpp"

using namespace mcp::p2p;

bool mcp::p2p::isPublicAddress(bi::address const & _addressToCheck)
{
	return !(isPrivateAddress(_addressToCheck) || isLocalHostAddress(_addressToCheck));
}

bool mcp::p2p::isPrivateAddress(bi::address const & _addressToCheck)
{
	if (_addressToCheck.is_v4())
	{
		bi::address_v4 v4Address = _addressToCheck.to_v4();
		bi::address_v4::bytes_type bytesToCheck = v4Address.to_bytes();
		if (bytesToCheck[0] == 10 || bytesToCheck[0] == 127)
			return true;
		if (bytesToCheck[0] == 172 && (bytesToCheck[1] >= 16 && bytesToCheck[1] <= 31))
			return true;
		if (bytesToCheck[0] == 192 && bytesToCheck[1] == 168)
			return true;
	}
	else if (_addressToCheck.is_v6())
	{
		bi::address_v6 v6Address = _addressToCheck.to_v6();
		bi::address_v6::bytes_type bytesToCheck = v6Address.to_bytes();
		if (bytesToCheck[0] == 0xfd && bytesToCheck[1] == 0)
			return true;
		if (!bytesToCheck[0] && !bytesToCheck[1] && !bytesToCheck[2] && !bytesToCheck[3] && !bytesToCheck[4] && !bytesToCheck[5] && !bytesToCheck[6] && !bytesToCheck[7]
			&& !bytesToCheck[8] && !bytesToCheck[9] && !bytesToCheck[10] && !bytesToCheck[11] && !bytesToCheck[12] && !bytesToCheck[13] && !bytesToCheck[14] && (bytesToCheck[15] == 0 || bytesToCheck[15] == 1))
			return true;
	}
	return false;
}

bool mcp::p2p::isSameNetwork(bi::address const& _address, bi::address const& _addressToCheck)
{
	if (_address.is_v4() && _addressToCheck.is_v4())
	{
		bi::address_v4 Address = _address.to_v4();
		bi::address_v4::bytes_type bytes = Address.to_bytes();
		bi::address_v4 AddressToCheck = _addressToCheck.to_v4();
		bi::address_v4::bytes_type bytesToCheck = AddressToCheck.to_bytes();
		if (bytes[0] == bytesToCheck[0])
			return true;
		else
			return false;
	}
	else if (_address.is_v6() && _addressToCheck.is_v6())
	{
		bi::address_v6 Address = _address.to_v6();
		bi::address_v6::bytes_type bytes = Address.to_bytes();
		bi::address_v6 AddressToCheck = _addressToCheck.to_v6();
		bi::address_v6::bytes_type bytesToCheck = AddressToCheck.to_bytes();
		if (bytes[0] == bytesToCheck[0])
			return true;
		else
			return false;
	}
	return false;
}

bool mcp::p2p::isLocalHostAddress(bi::address const & _addressToCheck)
{
	// @todo: ivp6 link-local adresses (macos), ex: fe80::1%lo0
	static const std::set<bi::address> c_rejectAddresses = {
		{ bi::address_v4::from_string("127.0.0.1") },
		{ bi::address_v4::from_string("0.0.0.0") },
		{ bi::address_v6::from_string("::1") },
		{ bi::address_v6::from_string("::") }
	};

	return std::find(c_rejectAddresses.begin(), c_rejectAddresses.end(), _addressToCheck) != c_rejectAddresses.end();
}

p2p_config::p2p_config() :
	port(mcp::p2p::default_port),
	max_peers(mcp::p2p::default_max_peers),
	nat(false)
{
}

void p2p_config::serialize_json(mcp::json & json_a) const
{
    json_a["host"] = listen_ip;
    json_a["port"] = port;
    json_a["max_peers"] = max_peers;

    mcp::json j_bootstarp_nodes = mcp::json::array();
    for (auto i: bootstrap_nodes)
    {
        j_bootstarp_nodes.push_back(i);
    }
    json_a["bootstrap_nodes"] = j_bootstarp_nodes;

    mcp::json j_exemption_nodes = mcp::json::array();
    for (auto i : exemption_nodes)
    {
        j_exemption_nodes.push_back(i);
    }
    json_a["exemption_nodes"] = j_exemption_nodes;

    json_a["nat"] = nat ? "true":"false";
}

bool p2p_config::deserialize_json(mcp::json const & json_a)
{
    auto error(false);
    try
    {
        if (json_a.count("host") && json_a["host"].is_string())
        {
            listen_ip = json_a["host"].get<std::string>();
        }
        else
        {
            error = true;
        }
        if (json_a.count("port") && json_a["port"].is_number_unsigned())
        {
            uint64_t port_l = json_a["port"].get<uint64_t>();
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
 
        if (json_a.count("max_peers") && json_a["max_peers"].is_number_unsigned())
        {
            max_peers = json_a["max_peers"].get<uint16_t>();
        }
        else
        {
            error = true;
        }

        if (json_a.count("bootstrap_nodes"))
        {
            if (json_a["bootstrap_nodes"].is_array())
            {
                mcp::json j_boot = json_a["bootstrap_nodes"];
                bootstrap_nodes.clear();
                for (auto i : j_boot)
                {
                    bootstrap_nodes.push_back(i);
                }
            }

        }
        else
        {
            error = true;
        }

        if (json_a.count("exemption_nodes"))
        {
            if (json_a["exemption_nodes"].is_array())
            {
                mcp::json j_exemp = json_a["exemption_nodes"];
                for (auto i : j_exemp)
                {
                    exemption_nodes.push_back(i);
                }
            }
        }
        else
        {
            error = true;
        }

        if (json_a.count("nat") && json_a["nat"].is_string())
        {
            nat = (json_a["nat"].get<std::string>() == "true" ? true : false);
        }
        else
        {
            error = true;
        }

    }
    catch (std::runtime_error const &)
    {
        error = true;
    }
    return error;
}

bool mcp::p2p::p2p_config::parse_old_version_data(mcp::json const & json_a, uint64_t const& version)
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

