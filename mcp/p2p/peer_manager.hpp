#pragma once
#include <chrono>
#include <mcp/p2p/peer_store.hpp>

namespace mcp
{
	namespace p2p
	{
		class peer_manager
		{
		public:
			peer_manager(bool & error_a, boost::filesystem::path const & application_path_a);

			bool should_reconnect(node_id const& id, PeerType type);

			void record_connect(node_id const& id, disconnect_reason const& reason);

			// Set a handshake failure reason for a peer
			void onHandshakeFailed(node_id const& _n, HandshakeFailureReason _r);

			mcp::p2p::peer_store store;
		private:
			unsigned fall_back_seconds(peers_content const& _p, PeerType type) const;

			void add_score(node_id const& id, disconnect_reason reason);

			void clear_disconnect(node_id const& id);
		};

	}
}