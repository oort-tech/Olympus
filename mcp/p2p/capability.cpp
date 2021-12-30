#include "capability.hpp"

using namespace mcp::p2p;

capability_desc::capability_desc(std::string const & name_a, uint32_t const & version_a) :
	name(name_a),
	version(version_a)
{
}

capability_desc::capability_desc(dev::RLP const & r)
{
	if (r.itemCount() != 2)
		throw std::runtime_error("invalid capability_desc rlp format");
	name = r[0].toString();
	version = r[1].toInt<uint32_t>();
}

void capability_desc::stream_RLP(dev::RLPStream & s)
{
	s.appendList(2) << name << version;
}

bool capability_desc::operator==(capability_desc const & other_a) const
{
	return name == other_a.name && version == other_a.version;
}

bool capability_desc::operator<(capability_desc const & other_a) const
{
	return name < other_a.name ||
		(name == other_a.name && version < other_a.version);
}

icapability::icapability(capability_desc const & desc_a, unsigned const & packet_count_a):
	desc(desc_a),
	_packet_count(packet_count_a)
{
}

unsigned icapability::packet_count() const
{
	return _packet_count;
}

peer_capability::peer_capability(unsigned const & offset_a, std::shared_ptr<icapability> const & cap_a) :
	offset(offset_a),
	cap(cap_a)
{
}
