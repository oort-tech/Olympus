#pragma once
#include <mcp/p2p/common.hpp>

namespace mcp
{
	namespace p2p
	{
		class peers_content
		{
		public:
			peers_content();

			peers_content(dev::Slice const & val_a);

			dev::Slice val() const;

			std::chrono::system_clock::time_point m_last_connected;
			std::chrono::system_clock::time_point m_last_attempted;
			int m_fail_attempts;
			disconnect_reason m_last_disconnect = disconnect_reason::no_disconnect;
			HandshakeFailureReason m_lastHandshakeFailure = HandshakeFailureReason::NoFailure;  ///< Reason for most recent handshake failure
		};

		class peer_store
		{
		public:
			peer_store(bool & error_a, boost::filesystem::path const& _path);
			bool peer_get(mcp::db::db_transaction & transaction, node_id const & node_id_a, peers_content & content_a);
			void peer_put(mcp::db::db_transaction & transaction, node_id const & node_id_a, peers_content const & content_a);
			void peer_del(mcp::db::db_transaction & transaction, node_id const & node_id_a);

			bool node_get(mcp::db::db_transaction & transaction, std::shared_ptr<node_info> nf_a, node_id & id_a);
			void node_put(mcp::db::db_transaction & transaction, std::shared_ptr<node_info> nf_a);
			void node_del(mcp::db::db_transaction & transaction, std::shared_ptr<node_info> nf_a);
			mcp::db::forward_iterator node_begin(mcp::db::db_transaction& transaction);

			mcp::db::db_transaction create_transaction() { return m_database->create_transaction(); };
		private:
			std::shared_ptr<mcp::db::database> m_database;
			int m_peers;
			int m_nodes;
		};
	}
}