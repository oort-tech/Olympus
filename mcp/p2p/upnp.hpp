#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/miniwget.h>
#include <boost/log/trivial.hpp>
#include <mcp/p2p/common.hpp>
#include <libdevcore/Exceptions.h>
#include <mcp/common/log.hpp>
#ifndef _WIN32
#include <ifaddrs.h>
#endif

namespace mcp
{
	namespace p2p
	{
		class upnp
		{
		public:
			upnp();

			~upnp();

			bool isValid() const { return m_ok; }

			static std::set<bi::address> getInterfaceAddresses();

			void traverseNAT(std::set<bi::address> const& _ifAddresses, unsigned short _listenPort);

			template <class _T>
			inline std::string toString(_T const& _t)
			{
				std::ostringstream o;
				o << _t;
				return o.str();
			}
		private:
			std::string externalIP();

			int port_mapping(char const* _addr, int ex_port_a);

			void remove_port_mapping();

			std::shared_ptr<UPNPUrls> m_urls;
			std::shared_ptr<IGDdatas> m_data;
			int m_port = 0;
			bool m_ok;
            mcp::log m_log = { mcp::log("p2p") };
		};

	}
}

