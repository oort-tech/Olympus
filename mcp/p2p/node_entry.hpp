#pragma once

#include <mcp/p2p/common.hpp>

namespace mcp
{
	namespace p2p
	{
		struct node_entry : public node_info
		{
		public:
			node_entry(h256 const& _hostNodeIDHash, node_id const & remote_node_id, node_endpoint const & endpoint_a, std::chrono::steady_clock::time_point _pongReceivedTime, PeerType type = PeerType::Optional) :
				node_info(remote_node_id, endpoint_a, type),
				nodeIDHash(sha3(remote_node_id)),
				distance(node_entry::calc_distance(_hostNodeIDHash, nodeIDHash)),
				lastPongReceivedTime(_pongReceivedTime),
				pending(true)
			{
			}

			bool hasValidEndpointProof(std::chrono::steady_clock::time_point now) const
			{
				return now <
					lastPongReceivedTime + std::chrono::hours(12);
			}

			h256 const nodeIDHash;
			unsigned distance;	//< Node's distance (xor of _src as integer).
			bool pending;		//< Node will be ignored until Pong is received
			std::chrono::steady_clock::time_point lastPongReceivedTime;

			static int calc_distance(h256 const& _a, h256 const& _b)
			{
				u256 d = _a ^ _b;
				unsigned ret = 0;
				while (d >>= 1)
					++ret;
				return ret;
			}
		};
	}
}