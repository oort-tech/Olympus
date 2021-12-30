#pragma once
#include <chrono>
#include <mcp/p2p/peer_store.hpp>

namespace mcp
{
	namespace p2p
	{
		//class peer_store;
		class peer_manager
		{
		public:
			peer_manager(bool & error_a, boost::filesystem::path const & application_path_a);

			bool should_reconnect(node_id const& id, PeerType type, disconnect_reason & reason);

			void record_connect(node_id const& id, disconnect_reason const& reason);

			mcp::p2p::peer_store store;
		private:
			unsigned fall_back_seconds(peers_content const& _p, PeerType type) const;

			unsigned get_score_by_reason(disconnect_reason reason) const;

			void add_score(node_id const& id, disconnect_reason reason);

			void clear_disconnect(node_id const& id);
		};

	}
}