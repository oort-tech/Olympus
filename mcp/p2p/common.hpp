#pragma once

#include <mcp/core/common.hpp>
#include <mcp/common/numbers.hpp>
#include <libdevcore/Common.h>
#include <libdevcore/RLP.h>
#include <mcp/common/mcp_json.hpp>

#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>

namespace ba = boost::asio;
namespace bi = boost::asio::ip;

namespace mcp
{
	namespace p2p
	{
		static uint16_t const version(0);
		static uint16_t const default_port(30606);
		static uint16_t const default_max_peers(25);

		static size_t const tcp_header_size(4);
		static size_t const max_tcp_packet_size(128 * 1024 * 1024);
        static size_t const max_summary_items(500);
		bool isPrivateAddress(bi::address const& _addressToCheck);
		bool isLocalHostAddress(bi::address const& _addressToCheck);
		bool isPublicAddress(bi::address const& _addressToCheck);
		bool isSameNetwork(bi::address const& _address, bi::address const& _addressToCheck);

		/// The ECDHE agreement failed during RLPx handshake.
		struct ECDHEError : virtual Exception {};

		class p2p_config
		{
		public:
			p2p_config();
            void serialize_json(mcp::json &) const;
            bool deserialize_json(mcp::json const &);
            bool parse_old_version_data(mcp::json const &, uint64_t const&);

			std::string listen_ip;
			uint16_t port;
			uint32_t max_peers;
			std::vector<std::string> bootstrap_nodes;
			std::vector<std::string> exemption_nodes;
			bool nat;
		};

		enum RLP_append
		{
			stream_list,
			stream_inline
		};

		class node_endpoint
		{
		public:
			node_endpoint() {}

			node_endpoint(bi::address addr_a, uint16_t udp_a, uint16_t tcp_a) :
				address(addr_a),
				udp_port(udp_a),
				tcp_port(tcp_a)
			{
			}

			node_endpoint(dev::RLP const & r)
			{
				interpret_RLP(r);
			}

			void stream_RLP(dev::RLPStream & s, RLP_append append = RLP_append::stream_list) const
			{
				if (append == RLP_append::stream_list)
					s.appendList(3);
				if (address.is_v4())
					s << dev::bytesConstRef(&address.to_v4().to_bytes()[0], 4);
				else if (address.is_v6())
					s << dev::bytesConstRef(&address.to_v6().to_bytes()[0], 16);
				else
					s << dev::bytes();
				s << udp_port << tcp_port;
			}

			void interpret_RLP(dev::RLP const & r)
			{
				if (r[0].size() == 4)
					address = bi::address_v4(*(bi::address_v4::bytes_type*)r[0].toBytes().data());
				else if (r[0].size() == 16)
					address = bi::address_v6(*(bi::address_v6::bytes_type*)r[0].toBytes().data());
				else
					address = bi::address();
				udp_port = r[1].toInt<uint16_t>();
				tcp_port = r[2].toInt<uint16_t>();
			}

			operator bi::udp::endpoint() const { return bi::udp::endpoint(address, udp_port); }
			operator bi::tcp::endpoint() const { return bi::tcp::endpoint(address, tcp_port); }

			operator bool() const { return !address.is_unspecified() && udp_port > 0 && tcp_port > 0; }

			bool operator==(node_endpoint const& other) const {
				return address == other.address && udp_port == other.udp_port && tcp_port == other.tcp_port;
			}
			bool operator!=(node_endpoint const& other) const {
				return !operator==(other);
			}

			bi::address address;
			uint16_t udp_port = 0;
			uint16_t tcp_port = 0;
		};

		enum class PeerType
		{
			Optional,
			Required
		};

		class node_info
		{
		public:
			node_info(node_id const & node_id_a, node_endpoint const & endpoint_a, PeerType peerType_a = PeerType::Optional) :
				id(node_id_a),
				endpoint(endpoint_a),
				peer_type(peerType_a)
			{
			}

			node_info(node_info const & other) :
				id(other.id),
				endpoint(other.endpoint),
				peer_type(other.peer_type.load())
			{

			}

			node_id id;
			node_endpoint endpoint;
			std::atomic<PeerType> peer_type{ PeerType::Optional };
		};

		enum class disconnect_reason
		{
			disconnect_requested = 0,
			tcp_error = 1,
			bad_protocol = 2,
			useless_peer = 3,
			too_many_peers = 4,
			duplicate_peer = 5,
			self_connect = 6,
			client_quit = 7,
			too_large_packet_size = 8,
			network_error = 9,
			malformed = 10,
			no_disconnect = 0xffff,
		};

		enum class HandshakeFailureReason
		{
			NoFailure = 0,
			UnknownFailure,
			Timeout,
			TCPError,
			FrameDecryptionFailure,
			InternalError,
			ProtocolError,
			PunishmentPeriod,
			DisconnectRequested
		};
	}
}
