#pragma once

#include <mcp/p2p/common.hpp>
#include <mcp/p2p/node_entry.hpp>
#include <mcp/p2p/discover_packet.hpp>
#include <mcp/common/common.hpp>
#include <blake2/blake2.h>
#include <mcp/common/log.hpp>
#include <mcp/p2p/peer_store.hpp>
#include <mcp/common/utility.hpp>
#include <mcp/core/config.hpp>

namespace mcp
{
	namespace p2p
	{
		class node_bucket
		{
		public:
			unsigned distance;
			std::list<std::weak_ptr<node_entry>> nodes;
		};

		class send_udp_datagram
		{
		public:
			send_udp_datagram(bi::udp::endpoint const & endpoint_a) :
				endpoint(endpoint_a)
			{
			}

			h256 add_packet_and_sign(dev::Secret const & prv_a, discover_packet const & packet_a)
			{
				assert_x((byte)packet_a.packet_type());
				//rlp: network type || packet type || packet
				dev::bytes rlp;
				{
					dev::RLPStream s;
					s.appendRaw(dev::bytes(1, (byte)mcp::mcp_network));
					s.appendRaw(dev::bytes(1, (byte)packet_a.packet_type()));
					packet_a.stream_RLP(s);

					//BOOST_LOG_TRIVIAL(info) << "send  add_packet_and_sign time" << packet_a.timestamp;

					s.swapOut(rlp);
				}
				dev::bytesConstRef rlp_cref(&rlp);

				//rlp hash : H(packet type || packet)
				h256 rlp_hash(dev::sha3(rlp_cref));

				//rlp sig : S(H(packet type || packet))
				dev::Signature rlp_sig = dev::sign(prv_a, rlp_hash);

				//BOOST_LOG_TRIVIAL(debug) << boost::str(boost::format("send packet sig, node id:%1%, hash:%2%, sig:%3%") 
				//	% packet_a.source_id.to_string() % rlp_hash.to_string() % rlp_sig.to_string());

				//data:  H( rlp sig || rlp ) || rlp sig || rlp 
				data.resize(h256::size + dev::Signature::size + rlp.size());
				dev::bytesRef data_hash_ref(&data[0], h256::size);
				//dev::bytesRef data_node_id_ref(&data[h256::size], node_id::size);
				dev::bytesRef data_sig_ref(&data[h256::size], dev::Signature::size);
				dev::bytesRef data_rlp_ref(&data[h256::size + dev::Signature::size], rlp_cref.size());

				//dev::bytesConstRef node_id_cref(packet_a.source_id.data(), packet_a.source_id.size);
				//node_id_cref.copyTo(data_node_id_ref);

				rlp_sig.ref().copyTo(data_sig_ref);
				rlp_cref.copyTo(data_rlp_ref);

				dev::bytesConstRef bytes_to_hash(&data[h256::size], data.size() - h256::size);
				h256 hash(dev::sha3(bytes_to_hash));
				hash.ref().copyTo(data_hash_ref);

				return rlp_hash;
			}

			bi::udp::endpoint endpoint;
			dev::bytes data;
		};

		struct eviction_entry
		{
			node_id new_node_id;
			std::chrono::steady_clock::time_point evicted_time;
		};

		enum class node_table_event_type
		{
			node_entry_added = 0,
			node_entry_dropped = 1
		};

		class node_table_event_handler
		{
		public:
			node_table_event_handler() = default;
			virtual ~node_table_event_handler() {};

			virtual void process_event(node_id const & node_id_a, node_table_event_type const & typea_a) = 0;

			void process_events()
			{
				std::list<std::pair<node_id, node_table_event_type>> temp_events;
				{
					std::lock_guard<std::mutex> lock(events_mutex);
					if (!node_event_handler.size())
						return;
					node_event_handler.unique();
					for (auto const & n : node_event_handler)
						temp_events.push_back(std::make_pair(n, events[n]));
					node_event_handler.clear();
					events.clear();
				}
				for (auto const & e : temp_events)
					process_event(e.first, e.second);
			}

			void append_event(node_id const & node_id_a, node_table_event_type const & type_a)
			{
				std::lock_guard<std::mutex> lock(events_mutex);
				node_event_handler.push_back(node_id_a);
				events[node_id_a] = type_a;
			}

			std::list<node_id> node_event_handler;
			std::mutex events_mutex;
			std::unordered_map<node_id, node_table_event_type> events;
		};

		class node_table : public std::enable_shared_from_this<node_table>
		{
		public:
			node_table(mcp::p2p::peer_store& store_a, KeyPair const & alias_a, node_endpoint const & endpoint_a);
			~node_table();

			void start();
			void add_node(node_info const & node_indo_a);
			std::shared_ptr<node_entry> get_node(node_id node_id_a);
			std::list<std::shared_ptr<node_info>> get_random_nodes(size_t const & max_size) const;
			std::list<std::shared_ptr<node_info>> snapshot(unsigned& index) const;
			void process_events();
			void set_event_handler(node_table_event_handler* handler);
			std::list<node_info> nodes() const;

            mcp::log m_log = { mcp::log("p2p") };

		private:
			// Constants for Kademlia, derived from address space.
			static unsigned const s_address_byte_size = node_id::size;							//< Size of address type in bytes.
			static unsigned const s_bits = 8 * s_address_byte_size;					//< Denoted by n in [Kademlia].
			static unsigned const s_bins = s_bits - 1;								//< Size of buckets (excludes root, which is us).

			static unsigned const s_bucket_size = 16;								//< Denoted by k in [Kademlia]. Number of nodes stored in each bucket.
			static unsigned const s_alpha = 3;										//< Denoted by \alpha in [Kademlia]. Number of concurrent FindNode requests.

			//Max iterations of discover. (discover)
			static unsigned const max_discover_rounds = boost::static_log2<s_bits>::value;

			boost::posix_time::milliseconds const discover_interval = boost::posix_time::milliseconds(7200);
			boost::posix_time::milliseconds const eviction_check_interval = boost::posix_time::milliseconds(75);
			//How long to wait for requests (evict, find iterations).
			std::chrono::milliseconds const req_timeout = std::chrono::milliseconds(300);

			static size_t const max_udp_packet_size = 1028;

			void discover_loop();
			void do_discover(node_id const & rand_node_id, unsigned const & round = 0, std::shared_ptr<std::set<std::shared_ptr<node_entry>>> tried = nullptr);

			void receive_loop();
			void handle_receive(bi::udp::endpoint const & recv_endpoint_a, dev::bytesConstRef const & data);
			std::unique_ptr<discover_packet> interpret_packet(bi::udp::endpoint const & from, dev::bytesConstRef data);

			void send(bi::udp::endpoint const & to_endpoint, discover_packet const & packet);
			void do_write();

			void ping(node_endpoint const & to);

			void evict(std::shared_ptr<node_entry> const & node_to_evict, std::shared_ptr<node_entry> const & new_node);
			void do_check_evictions();

			node_bucket & bucket_UNSAFE(node_entry const * node_a);
			std::vector<std::shared_ptr<node_entry>> nearest_node_entries(node_id const& target_a, node_id const& source_a = node_id(0), std::shared_ptr<bi::udp::endpoint> from = nullptr);
			void drop_node(std::shared_ptr<node_entry> _n);
			void note_active_node(node_id const & node_id, bi::udp::endpoint const & _endpoint);

			void process_write_node_info();
			boost::posix_time::minutes const write_info_interval = boost::posix_time::minutes(10);
			std::unique_ptr<ba::deadline_timer> write_info_timer;

			ba::io_service io_service;
			node_info my_node_info;
			dev::Secret secret;
			node_endpoint my_endpoint;

			std::unique_ptr<bi::udp::socket> socket;

			bi::udp::endpoint recv_endpoint;
			std::array<byte, max_udp_packet_size> recv_buffer;

			std::deque<send_udp_datagram> send_queue;
			std::mutex send_queue_mutex;

			std::unique_ptr<ba::deadline_timer> discover_timer;

			//Known Node Endpoints
			std::unordered_map<node_id, std::shared_ptr<node_entry>> m_nodes;
			mutable std::mutex m_nodes_mutex;

			//State of p2p node network
			std::array<node_bucket, s_bins> states;
			mutable std::mutex states_mutex;

			std::unique_ptr<ba::deadline_timer> eviction_check_timer;
			std::unordered_map<node_id, eviction_entry> evictions;
			std::mutex evictions_mutex;

			//Timeouts for FindNode requests.
			std::unordered_map<node_id, std::chrono::steady_clock::time_point> find_node_timeouts;
			std::mutex  find_node_timeouts_mutex;

			std::unique_ptr<node_table_event_handler> node_event_handler;

			bool is_cancel;
			std::thread io_service_thread;
			mcp::p2p::peer_store& m_store;
		};
	}
}