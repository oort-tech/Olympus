#pragma once

#include <mcp/p2p/common.hpp>

namespace mcp
{
	namespace p2p
	{
		class capability_desc
		{
		public:
			capability_desc(std::string const & name_a, uint32_t const & version_a);
			capability_desc(dev::RLP const & r);
			void stream_RLP(dev::RLPStream & s);

			bool operator==(capability_desc const & other_a) const;
			bool operator<(capability_desc const & other_a) const;

			std::string name;
			uint32_t version;
		};

		class peer;

		class icapability
		{
		public:
			icapability(capability_desc const & desc_a, unsigned const & packet_count);
			virtual void on_connect(std::shared_ptr<p2p::peer> peer_a, unsigned const & offset) = 0;
			virtual void on_disconnect(std::shared_ptr<peer> peer_a) = 0;
			virtual bool read_packet(std::shared_ptr<peer> peer_a, unsigned const & type, std::shared_ptr<dev::RLP> r) = 0;
			unsigned packet_count() const;

			capability_desc desc;
		private:
			unsigned _packet_count;
		};

		class peer_capability
		{
		public:
			peer_capability(unsigned const & offset_a, std::shared_ptr<icapability> const & cap_a);
			unsigned offset;
			std::shared_ptr<icapability> cap;
		};
	}
}