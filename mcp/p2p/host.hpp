#pragma once

#include <mcp/p2p/common.hpp>
#include <mcp/p2p/capability.hpp>
#include <mcp/p2p/handshake.hpp>
#include <mcp/p2p/node_table.hpp>
#include <mcp/p2p/frame_coder.hpp>
#include <mcp/p2p/peer.hpp>
#include <mcp/p2p/peer_manager.hpp>
#include <mcp/p2p/upnp.hpp>

#include <unordered_map>


namespace mcp
{
    namespace p2p
    {
        class peers_content;
        class peer_metrics;

		class peer_outbound
		{
			struct connect_info
			{
				int attempts;
				int need_attempts;
			};
		public:
			peer_outbound() :max_times(15), dynameter(2) {}
			bool should_connect(node_id const& id);
			void record(node_id const& id);
			void attempt();
		
		private:
			int dynameter;
			int max_times;	//total retry time = max_times * dynameter * try_connect_interval
			std::map<node_id, connect_info> outs;
		};

		class frame_coder;
        class host : public std::enable_shared_from_this<host>
        {
        public:
            host(bool & error_a, p2p_config const & config_a, boost::asio::io_service & io_service_a, mcp::seed_key const & node_key,
				 boost::filesystem::path const & application_path_a);
			~host() { stop(); }
            void start();
            void stop();
            void register_capability(std::shared_ptr<icapability> cap);
            void on_node_table_event(node_id const & node_id_a, node_table_event_type const & type_a);
            std::unordered_map<node_id, bi::tcp::endpoint> peers() const;
            std::list<node_info> nodes() const;

			node_id id() const { return alias.public_key_comp(); }

			std::list<capability_desc> caps() const { std::list<capability_desc> ret; for (auto const& i : capabilities) ret.push_back(i.first); return ret; }
			void start_peer(mcp::public_key const& _id, dev::RLP const& _hello, std::unique_ptr<mcp::p2p::frame_coder>&& _io, std::shared_ptr<bi::tcp::socket> const & socket);

			mcp::key_pair alias;
			
            std::map<std::string,uint64_t> get_peers_write_queue_size();
            std::map<std::string, std::shared_ptr<mcp::p2p::peer_metrics> > get_peers_metrics();

			std::mutex pending_conns_mutex;
			std::unordered_set<node_id> pending_conns;
			peer_outbound attempt_outs;

			void replace_bootstrap(node_id const& old_a, node_id new_a);
        private:
            enum class peer_type
            {
                egress = 0,
                ingress = 1
            };

            void run();
            bool resolve_host(std::string const & addr, bi::tcp::endpoint & ep);
            void connect(std::shared_ptr<node_info> const & ne);
            size_t avaliable_peer_count(peer_type const & type, bool b = true);
            uint32_t max_peer_size(peer_type const & type);
            void keep_alive_peers();
            void try_connect_nodes();
            void start_listen(bi::address const & listen_ip, uint16_t const & port);
            void accept_loop();

            void map_public(bi::address const & listen_ip, uint16_t port);

            node_info node_info_from_node_table(node_id const& node_id) const;

            p2p_config const & config;
            boost::asio::io_service & io_service;
            std::map<capability_desc, std::shared_ptr<icapability>> capabilities;

            std::unique_ptr<bi::tcp::acceptor> acceptor;
            std::unordered_map<node_id, std::weak_ptr<peer>> m_peers;
            mutable std::mutex m_peers_mutex;

            std::shared_ptr<node_table> m_node_table;

            dev::bytes restore_network_bytes;

            std::atomic<bool> is_run;
            std::unique_ptr<boost::asio::deadline_timer> run_timer;
            const boost::posix_time::milliseconds run_interval = boost::posix_time::milliseconds(100);

            std::chrono::seconds const keep_alive_interval = std::chrono::seconds(30);
            std::chrono::steady_clock::time_point last_ping;

            std::chrono::seconds const try_connect_interval = std::chrono::seconds(3);
			std::chrono::seconds const try_connect_interval_exemption = std::chrono::seconds(30);
            std::chrono::steady_clock::time_point last_try_connect;
			std::chrono::steady_clock::time_point last_try_connect_exemption;
            std::chrono::seconds node_fallback_interval = std::chrono::seconds(20);

            std::chrono::steady_clock::time_point start_time;
            std::vector<std::shared_ptr<node_info>> bootstrap_nodes;
            std::vector<std::shared_ptr<node_info>> exemption_nodes;

            boost::posix_time::milliseconds const handshake_timeout = boost::posix_time::milliseconds(5000);

			std::shared_ptr<peer_manager> m_peer_manager;
            std::unique_ptr<upnp> up;
            mcp::log m_log = { mcp::log("p2p") };
        };

        class host_node_table_event_handler : public node_table_event_handler
        {
        public:
            host_node_table_event_handler(host & host_a)
                :m_host(host_a)
            {
            }

            virtual void process_event(node_id const & node_id_a, node_table_event_type const & type_a)
            {
                m_host.on_node_table_event(node_id_a, type_a);
            }

            host & m_host;
        };
    }
}