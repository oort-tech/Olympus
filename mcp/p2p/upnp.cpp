#include <mcp/p2p/upnp.hpp>
using namespace mcp::p2p;

upnp::upnp()
{
	m_urls = std::make_shared<UPNPUrls>();
	m_data = std::make_shared<IGDdatas>();
	memset(m_urls.get(), 0, sizeof(struct UPNPUrls));
	memset(m_data.get(), 0, sizeof(struct IGDdatas));

	m_ok = false;

	struct UPNPDev* devlist;
	struct UPNPDev* dev;
	char* descXML;
	int descXMLsize = 0;
	int upnperror = 0;
#if MINIUPNPC_API_VERSION >= 14
	devlist = upnpDiscover(2000, NULL/*multicast interface*/, NULL/*minissdpd socket path*/, 0/*sameport*/, 0/*ipv6*/, 2/*ttl*/, &upnperror);
#else
	devlist = upnpDiscover(2000, NULL/*multicast interface*/, NULL/*minissdpd socket path*/, 0/*sameport*/, 0/*ipv6*/, &upnperror);
#endif
	if (devlist)
	{
		dev = devlist;
		while (dev)
		{
			if (strstr(dev->st, "InternetGatewayDevice"))
				break;
			dev = dev->pNext;
		}
		if (!dev)
			dev = devlist; /* defaulting to first device */

		//LOG(m_log.info) << "UPnP device:" << dev->descURL << "[st:" << dev->st << "]";
#if MINIUPNPC_API_VERSION >= 16
		int responsecode = 200;
		descXML = (char*)miniwget(dev->descURL, &descXMLsize, 0, &responsecode);
#elif MINIUPNPC_API_VERSION >= 9
		descXML = (char*)miniwget(dev->descURL, &descXMLsize, 0);
#else
		descXML = (char*)miniwget(dev->descURL, &descXMLsize);
#endif
		if (descXML)
		{
			parserootdesc(descXML, descXMLsize, m_data.get());
			free(descXML);
#if MINIUPNPC_API_VERSION >= 9
			GetUPNPUrls(m_urls.get(), m_data.get(), dev->descURL, 0);
#else
			GetUPNPUrls(m_urls.get(), m_data.get(), dev->descURL);
#endif
			m_ok = true;
		}
		freeUPNPDevlist(devlist);
	}
}

upnp::~upnp()
{
	if (m_port > 0)
		remove_port_mapping();
}

std::string upnp::externalIP()
{
	char addr[16];
	if (!UPNP_GetExternalIPAddress(m_urls->controlURL, m_data->first.servicetype, addr))
	{
		if (0 == strlen(addr))
		{
			return "0.0.0.0";
		}
		return addr;
	}
		
	return "0.0.0.0";
}

int upnp::port_mapping(char const* _addr,int ex_port_a)
{
	(void)_addr;
	(void)ex_port_a;
	if (m_urls->controlURL[0] == '\0')
	{
        LOG(m_log.info) << "UPnP::port_mapping() called without proper initialisation?";
		return -1;
	}

	char ext_port[16];
	sprintf(ext_port, "%d", ex_port_a);
	int ret = UPNP_AddPortMapping(m_urls->controlURL, m_data->first.servicetype, ext_port, ext_port, _addr, "mcp", "TCP", nullptr, nullptr);
	UPNP_AddPortMapping(m_urls->controlURL, m_data->first.servicetype, ext_port, ext_port, _addr, "mcp", "UDP", nullptr, nullptr);
	if (ret != UPNPCOMMAND_SUCCESS)
	{
        LOG(m_log.info) << "UPNP_AddPortMapping fail.";
		return -2;
	}
	m_port = ex_port_a;
	return 0;
}

void upnp::remove_port_mapping()
{
	char port_str[16];
	if (m_urls->controlURL[0] == '\0')
	{
        LOG(m_log.info) << "TB : the init was not done !\n";
		return;
	}
	sprintf(port_str, "%d", m_port);
	UPNP_DeletePortMapping(m_urls->controlURL, m_data->first.servicetype, port_str, "TCP", NULL);
	UPNP_DeletePortMapping(m_urls->controlURL, m_data->first.servicetype, port_str, "UDP", NULL);
}


std::set<bi::address> upnp::getInterfaceAddresses()
{
	std::set<bi::address> addresses;

#if defined(_WIN32)
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
		BOOST_THROW_EXCEPTION(dev::NoNetworking());

	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
	{
		BOOST_LOG_TRIVIAL(info) << "Error " << WSAGetLastError() << " when getting local host name.";
		WSACleanup();
		BOOST_THROW_EXCEPTION(dev::NoNetworking());
	}

	struct hostent* phe = gethostbyname(ac);
	if (phe == 0)
	{
		BOOST_LOG_TRIVIAL(info) << "Bad host lookup.";
		WSACleanup();
		BOOST_THROW_EXCEPTION(dev::NoNetworking());
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		char *addrStr = inet_ntoa(addr);
		bi::address address(bi::address::from_string(addrStr));
		if (!isLocalHostAddress(address))
			addresses.insert(address.to_v4());
	}

	WSACleanup();
#else
	ifaddrs* ifaddr;
	if (getifaddrs(&ifaddr) == -1)
		BOOST_THROW_EXCEPTION(dev::NoNetworking());

	for (auto ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (!ifa->ifa_addr || std::string(ifa->ifa_name) == "lo0" || !(ifa->ifa_flags & IFF_UP))
			continue;

		if (ifa->ifa_addr->sa_family == AF_INET)
		{
			in_addr addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			boost::asio::ip::address_v4 address(boost::asio::detail::socket_ops::network_to_host_long(addr.s_addr));
			if (!isLocalHostAddress(address))
				addresses.insert(address);
		}
		else if (ifa->ifa_addr->sa_family == AF_INET6)
		{
			sockaddr_in6* sockaddr = ((struct sockaddr_in6 *)ifa->ifa_addr);
			in6_addr addr = sockaddr->sin6_addr;
			boost::asio::ip::address_v6::bytes_type bytes;
			memcpy(&bytes[0], addr.s6_addr, 16);
			boost::asio::ip::address_v6 address(bytes, sockaddr->sin6_scope_id);
			if (!isLocalHostAddress(address))
				addresses.insert(address);
		}
	}

	if (ifaddr != NULL)
		freeifaddrs(ifaddr);

#endif

	return addresses;
}

void upnp::traverseNAT(std::set<bi::address> const& _ifAddresses, unsigned short Port)
{
	if (Port == 0)
	{
        LOG(m_log.info) << "UPnP::traverseNAT() port can not be 0";
		return;
	}

	auto eIP = externalIP();
	bi::address eIPAddr(bi::address::from_string(eIP));
	if (isPrivateAddress(eIPAddr))
	{
        LOG(m_log.info) << "UPnP::traverseNAT() ip address can not be private network.";
		return;
	}

	if (eIP == std::string("0.0.0.0"))
	{
        LOG(m_log.info) << "Couldn't punch through NAT (or no NAT in place)";
		return;
	}

	bi::address pAddr;
	bool is_maped = false;
	for (auto const& addr : _ifAddresses)
	{
		pAddr = addr;
		if (addr.is_v4() && isPrivateAddress(addr) && (port_mapping(addr.to_string().c_str(), Port) == 0))
		{
			is_maped = true;
			break;
		}
	}

	if (pAddr.is_v4() && isPublicAddress(pAddr))
	{
        LOG(m_log.info) << "Is already the public network address:" << pAddr;
		return;
	}

	if (is_maped && eIP != std::string("0.0.0.0") && !isPrivateAddress(eIPAddr))
	{
        LOG(m_log.info) << "Punched through NAT and mapped local port:" << Port <<  ",  External addr: " << eIP << ":" << Port << ".";
	}
	else
        LOG(m_log.info) << "Couldn't punch through NAT (or no NAT in place).";
}

