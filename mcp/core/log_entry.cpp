#include "log_entry.hpp"

#include <libdevcore/RLP.h>
#include <libdevcore/SHA3.h>
#include <mcp/common/common.hpp>

namespace mcp
{

log_entry::log_entry(RLP const& _r)
{
	assert(_r.itemCount() == 3);
	acct = (mcp::account)_r[0];
	topics = _r[1].toVector<h256>();
	data = _r[2].toBytes();
}

void log_entry::streamRLP(RLPStream& _s) const
{
	_s.appendList(3) << acct << topics << data;
}

void log_entry::serialize_json(mcp::json & json_a) const
{
	json_a["account"] = acct.to_account();
	json_a["data"] = mcp::bytes_to_hex(data);

    mcp::json topics_l = mcp::json::array();
	for (auto t: topics)
	{
		topics_l.push_back(t.hex());
	}
	json_a["topics"] = topics_l;
}

void log_entry::hash(blake2b_state & hash_a) const
{
	blake2b_update(&hash_a, acct.bytes.data(), sizeof(acct.bytes));
	for (auto t : topics)
	{
		mcp::uint256_union topic_u(t);
		blake2b_update(&hash_a, topic_u.bytes.data(), sizeof(topic_u.bytes));
	}
	blake2b_update(&hash_a, data.data(), data.size());
}

log_bloom log_entry::bloom() const
{
	log_bloom ret;
	ret.shiftBloom<3>(sha3(acct.ref()));
	for (auto t: topics)
		ret.shiftBloom<3>(sha3(t.ref()));
	return ret;
}

}
