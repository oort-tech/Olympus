/*
 * public class for sync,processor,capability.decoupling
 */
#include "common.hpp"

namespace mcp
{
	capability_metrics CapMetricsRecieved = capability_metrics();
	capability_metrics CapMetricsSend = capability_metrics();

	sync_request_hash  gen_sync_request_hash(p2p::node_id const & id, uint64_t random, mcp::sub_packet_type & request_type_a)
	{
		dev::RLPStream s;
		s.appendList(4);
		s << id << random << (uint32_t)request_type_a << mcp::random_pool.GenerateWord32();

		return dev::sha3(s.out());
	}

}