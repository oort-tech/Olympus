#pragma once
#include <mcp/p2p/common.hpp>
#include <mcp/p2p/capability.hpp>
#include <mcp/p2p/frame_coder.hpp>
#include <mcp/p2p/peer_manager.hpp>
#include "lz4.h"
#include <libdevcore/RLP.h>

#define SEND_BUFFER_LIMIT	(100 * 1024 * 1024)
#define GROUP_BUFFER_SIZE_LIMIT	32768

namespace mcp
{
    namespace p2p
    {
        enum class packet_type
        {
            handshake = 0,
            ack = 1,
            disconect = 2,
            ping = 3,
            pong = 4,

            user_packet = 0x10
        };

        class peer_manager;
		class frame_coder;
        class peer_metrics
        {
        public:
            peer_metrics() = default;
            uint64_t  send_size = 0;
            uint64_t  send_count = 0;
            uint64_t  write_write_queue_size = 0;
			uint64_t  read_read_queue_size = 0;
			uint64_t  write_queue_buffer_size = 0;
        };

        class peer : public std::enable_shared_from_this<peer>
        {
        public:
            //peer(std::shared_ptr<bi::tcp::socket> const & socket_a, node_id const & node_id_a);
			peer(std::shared_ptr<bi::tcp::socket> const & socket_a, node_id const & node_id_a, std::shared_ptr<peer_manager> peer_manager_a, std::unique_ptr<frame_coder>&& _io);
            ~peer();
            void register_capability(std::shared_ptr<peer_capability> const & cap);
            void start();
            void ping();
            dev::RLPStream & prep(dev::RLPStream & s, unsigned const & type, unsigned const & size = 0);
            void send(dev::RLPStream & s);
            bool is_connected();
            void disconnect(disconnect_reason const & reason);
            std::chrono::steady_clock::time_point last_received();
            node_id remote_node_id() const;
            bi::tcp::endpoint remote_endpoint() const;
            uint64_t get_write_queue_size() { return write_queue.size(); }
            std::shared_ptr<mcp::p2p::peer_metrics> get_peer_metrics();
        private:
            void read_loop();
            bool check_packet(dev::bytesConstRef msg);
            bool read_packet(unsigned const & type, std::shared_ptr<dev::RLP> r);
            void do_write();
			void do_read();
            void drop(disconnect_reason const & reason);
			/// Check error code after reading and drop peer if error code.
			bool checkRead(std::size_t _expected, boost::system::error_code _ec, std::size_t _length);
            std::string reason_of(disconnect_reason reason)
            {
                switch (reason)
                {
                case disconnect_reason::disconnect_requested: return "Disconnect was requested.";
                case disconnect_reason::tcp_error: return "Low-level TCP communication error.";
                case disconnect_reason::bad_protocol: return "Data format error.";
                case disconnect_reason::useless_peer: return "Peer had no use for this node.";
                case disconnect_reason::too_many_peers: return "Peer had too many connections.";
                case disconnect_reason::duplicate_peer: return "Peer was already connected.";
                case disconnect_reason::client_quit: return "Peer is exiting.";
                case disconnect_reason::self_connect: return "Connected to ourselves.";
                case disconnect_reason::too_large_packet_size: return "Too large packet size.";
                case disconnect_reason::no_disconnect: return "(No disconnect has happened.)";
                default: return "Unknown reason.";
                }
            }

            node_id m_node_id;
			ba::io_service & m_io_service;
			std::shared_ptr<peer_manager> m_peer_manager;
            std::shared_ptr<bi::tcp::socket> socket;
            std::list<std::shared_ptr<peer_capability>> capabilities;
			std::unique_ptr<frame_coder> m_io;	///< Transport over which packets are sent.
            dev::bytes read_buffer;
			dev::bytes read_header_buffer;
            std::deque<dev::bytes> write_queue;
            std::mutex write_queue_mutex;
			std::deque<dev::bytes> read_queue;
			std::mutex read_queue_mutex;
            std::chrono::steady_clock::time_point _last_received;
			std::atomic<bool> is_dropped;
            std::shared_ptr <mcp::p2p::peer_metrics> m_pmetrics;
			dev::bytes write_bufs;
			bool bprintf = false;
			uint32_t surplus_size = 0;

			//int send_size = 0;
			//std::chrono::time_point<std::chrono::system_clock> send_start = std::chrono::system_clock::now();
			//std::chrono::time_point<std::chrono::system_clock> one_min_start = send_start;
			//std::map<std::chrono::time_point<std::chrono::system_clock>, int> send_map;

			//int read_size = 0;
			//std::chrono::time_point<std::chrono::system_clock> read_start = std::chrono::system_clock::now();
			//std::chrono::time_point<std::chrono::system_clock> read_one_min_start = read_start;
			//std::map<std::chrono::time_point<std::chrono::system_clock>, int> read_map;

			//int deal_size = 0;
			//std::chrono::time_point<std::chrono::system_clock> deal_start = std::chrono::system_clock::now();
			//std::chrono::time_point<std::chrono::system_clock> deal_one_min_start = deal_start;
			//std::map<std::chrono::time_point<std::chrono::system_clock>, int> deal_map;
            mcp::log m_log = { mcp::log("p2p") };
        };
    }
}
