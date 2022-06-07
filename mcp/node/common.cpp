/*
 * public class for sync,processor,capability.decoupling
 */
#pragma once

#include "common.hpp"
#include <blake2/blake2.h>

namespace mcp
{
	capability_metrics CapMetricsRecieved = capability_metrics();
	capability_metrics CapMetricsSend = capability_metrics();

	sync_request_hash  gen_sync_request_hash(p2p::node_id const & id, uint64_t random, mcp::sub_packet_type & request_type_a)
	{
		mcp::sync_request_hash result;
		blake2b_state hash_l;
		auto status(blake2b_init(&hash_l, sizeof(result.bytes)));

		assert_x(status == 0);
		size_t random_l = mcp::random_pool.GenerateWord32();
		blake2b_update(&hash_l, id.bytes.data(), sizeof(id.bytes));
		blake2b_update(&hash_l, &random, sizeof(random));
		blake2b_update(&hash_l, &request_type_a, sizeof(request_type_a));
		blake2b_update(&hash_l, &random_l, sizeof(random_l));


		status = blake2b_final(&hash_l, result.bytes.data(), sizeof(result.bytes));
		assert_x(status == 0);

		return result;
	}

}