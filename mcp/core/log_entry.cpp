#include "log_entry.hpp"

#include <libdevcore/RLP.h>
#include <libdevcore/SHA3.h>
#include <mcp/common/common.hpp>

namespace mcp
{

log_entry::log_entry(RLP const& _r)
{
	assert(_r.itemCount() == 3);
	address = (Address)_r[0];
	topics = _r[1].toVector<h256>();
	data = _r[2].toBytes();
}

void log_entry::streamRLP(RLPStream& _s) const
{
	_s.appendList(3) << address << topics << data;
}

log_bloom log_entry::bloom() const
{
	log_bloom ret;
	ret.shiftBloom<3>(sha3(address.ref()));
	for (auto t: topics)
		ret.shiftBloom<3>(sha3(t.ref()));
	return ret;
}

}
