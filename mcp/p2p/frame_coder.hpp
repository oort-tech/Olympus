#pragma once

#include <mcp/p2p/common.hpp>
#include <mcp/p2p/handshake.hpp>
#include <mcp/common/log.hpp>

using namespace dev;

namespace mcp
{
	namespace p2p
	{
		static size_t const handshake_header_size(3);

		class frame_coder_impl
		{
		public:
			/// set key and nonce.
			void set_key(bool _originated, public_key_comp const&_pub, key_pair const&_key);
			void set_nonce(nonce const&_remote_nonce, nonce const&_nonce);

			//encry
			bool encry(bytes& io_cipher);

			//dencry input:io_cipher, output:io_cipher
			bool dencry(bytes& io_cipher);

			//dencry input:io, output:o_bytes
			bool dencry(bytesConstRef io, bytes& o_bytes);

		private:
			//Mutex x_macEnc;  ///< Mutex.
			secret_encry in_key, out_key;
			nonce local_nonce;
			nonce remote_nonce;
            mcp::log m_log = { mcp::log("p2p") };
		};

		class hankshake;

		/**
		* @brief Encoder/decoder transport for RLPx connection established by hankshake.
		*/
		class frame_coder
		{
		public:
			frame_coder(hankshake const& _init);

			/// Construct with external key material.
			frame_coder(bool _originated, public_key_comp const& _remoteEphemeral, nonce const& _remoteNonce, key_pair const& _ecdheLocal, nonce const& _nonce);

			~frame_coder();

			/// Establish shared secrets and setup AES and MAC states.
			void setup(bool _originated, public_key_comp const& _remoteEphemeral, nonce const& _remoteNonce, key_pair const& _ecdheLocal, nonce const& _nonce);

			/// Legacy. Encrypt _packet header and frame.
			void write_single_frame_packet(bytesConstRef _packet, bytes& o_bytes);

			void write_frame_packet(bytesConstRef _packet, bytes& o_bytes);

			/// Authenticate and decrypt header in-place.
			bool auth_and_decrypt_header(bytes& io,uint32_t& len);

			/// Authenticate and decrypt frame in-place.
			bool auth_and_decrypt_frame(bytes& io);

			bool auth_and_decrypt_frame(bytesConstRef io, bytes& o_bytes);

			bytes serialize_packet_size(uint32_t const & size);
			uint32_t deserialize_packet_size(bytes const & data);

			//add packet header(lenth)
			void write_frame_packet_header(bytes& o_bytes);

		protected:
			void write_frame(RLPStream const& _header, bytesConstRef _payload, bytes& o_bytes);
			void write_frame(bytesConstRef _payload, bytes& o_bytes);

		private:
			std::unique_ptr<class frame_coder_impl> m_impl;
            mcp::log m_log = { mcp::log("p2p") };
		};
	}
}