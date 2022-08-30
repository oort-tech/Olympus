#pragma once

#include <mcp/p2p/common.hpp>
#include <mcp/p2p/node_entry.hpp>

namespace mcp
{
	namespace p2p
	{
		enum class discover_packet_type
		{
			ping = 1,
			pong = 2,
			find_node = 3,
			neighbours = 4
		};

		class discover_packet
		{
		public:
			discover_packet(node_id const & node_id_a) :
				source_id(node_id_a),
				timestamp(future_from_epoch(std::chrono::seconds(60)))
			{
			}
			virtual ~discover_packet() {};

			static uint32_t future_from_epoch(std::chrono::seconds sec)
			{
				return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() + sec).time_since_epoch()).count());
			}

			static uint32_t seconds_since_epoch()
			{
				return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now()).time_since_epoch()).count());
			}

			bool is_expired() const
			{
				return seconds_since_epoch() > timestamp;
			}

			virtual discover_packet_type packet_type() const = 0;
			virtual void stream_RLP(dev::RLPStream & s) const = 0;
			virtual void interpret_RLP(dev::bytesConstRef bytes) = 0;

			node_id source_id;
			uint32_t timestamp;
		};

		class ping_packet : public discover_packet
		{
		public:
			ping_packet(node_id const & node_id_a) :
				version(mcp::p2p::version),
				discover_packet(node_id_a)
			{
			}

			ping_packet(node_id const & node_id_a, uint16_t const & tcp_port_a) :
				version(mcp::p2p::version),
				discover_packet(node_id_a),
				tcp_port(tcp_port_a)
			{
			}

			discover_packet_type packet_type() const { return discover_packet_type::ping; };

			void stream_RLP(dev::RLPStream & s) const
			{
				s.appendList(3);
				s << mcp::p2p::version;
				s << tcp_port;
				s << timestamp;
			}

			void interpret_RLP(dev::bytesConstRef _bytes)
			{
				dev::RLP r(_bytes, dev::RLP::AllowNonCanon | dev::RLP::ThrowOnFail);
				version = r[0].toInt<uint16_t>();
				tcp_port = r[1].toInt<uint16_t>();
				timestamp = r[2].toInt<uint32_t>();
			}

			uint16_t version;
			uint64_t tcp_port = 0;
		};

		class pong_packet : public discover_packet
		{
		public:
			pong_packet(node_id const & node_id_a) :
				discover_packet(node_id_a)
			{

			}

			discover_packet_type packet_type() const { return discover_packet_type::pong; };

			void stream_RLP(dev::RLPStream & s) const
			{
				s.appendList(1);
				s << timestamp;
			}

			void interpret_RLP(dev::bytesConstRef _bytes)
			{
				dev::RLP r(_bytes, dev::RLP::AllowNonCanon | dev::RLP::ThrowOnFail);
				timestamp = r[0].toInt<uint32_t>();
			}

			node_endpoint destination;
		};

		class find_node_packet : public discover_packet
		{
		public:
			find_node_packet(node_id const & node_id_a) :
				discover_packet(node_id_a)
			{

			}

			// Daniel, update the target_a's type into node_id from h256
			find_node_packet(node_id const & node_id_a, node_id const & target_a) :
				discover_packet(node_id_a),
				target(target_a)
			{
			}

			discover_packet_type packet_type() const { return discover_packet_type::find_node; };

			void stream_RLP(dev::RLPStream & s) const
			{
				s.appendList(2); s << target << timestamp;
			}
			void interpret_RLP(dev::bytesConstRef bytes)
			{
				dev::RLP r(bytes, dev::RLP::AllowNonCanon | dev::RLP::ThrowOnFail);
				target = (node_id)r[0];
				timestamp = r[1].toInt<uint32_t>();
			}

			node_id target;
		};

		class neighbour
		{
		public:
			neighbour(node_entry const & ne) :
				endpoint(ne.endpoint),
				id(ne.id)
			{

			}

			neighbour(dev::RLP const & r)
			{
				endpoint.interpret_RLP(r);
				id = (node_id)r[3];
			}

			node_endpoint endpoint;
			node_id id;

			void stream_RLP(dev::RLPStream & s) const
			{
				s.appendList(4);
				endpoint.stream_RLP(s, RLP_append::stream_inline);
				s << id;
			}

			static size_t const max_size = 57;
		};

		class neighbours_packet : public discover_packet
		{
		public:
			neighbours_packet(node_id const & node_id_a) :
				discover_packet(node_id_a)
			{
			}

			neighbours_packet(node_id const & node_id_a, std::vector<std::shared_ptr<node_entry>> const& nearest_a, unsigned offset_a = 0, unsigned limit_a = 0) :
				discover_packet(node_id_a)
			{
				auto limit = limit_a ? std::min(nearest_a.size(), (size_t)(offset_a + limit_a)) : nearest_a.size();
				for (auto i = offset_a; i < limit; i++)
					neighbours.push_back(neighbour(*nearest_a[i]));
			}

			discover_packet_type packet_type() const { return discover_packet_type::neighbours; };

			void stream_RLP(dev::RLPStream& _s) const
			{
				_s.appendList(2);
				_s.appendList(neighbours.size());
				for (auto const& n : neighbours)
					n.stream_RLP(_s);
				_s << timestamp;
			}

			void interpret_RLP(dev::bytesConstRef bytes)
			{
				dev::RLP r(bytes, dev::RLP::AllowNonCanon | dev::RLP::ThrowOnFail);
				for (auto const & n : r[0])
					neighbours.emplace_back(n);
				timestamp = r[1].toInt<uint32_t>();
			}

			std::vector<neighbour> neighbours;
		};
	}
}