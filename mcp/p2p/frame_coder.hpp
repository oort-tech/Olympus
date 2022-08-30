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
		class hankshake;

		/**
		* @brief Encapsulation of Frame
		* @todo coder integration; padding derived from coder
		*/
		struct RLPXFrameInfo
		{
			/// Constructor. frame-size || protocol-type, [sequence-id[, total-packet-size]]
			explicit RLPXFrameInfo(bytesConstRef _frameHeader);

			uint32_t const length;  ///< Size of frame (excludes padding). Max: 2**24
			uint8_t const padding;  ///< Length of padding which follows @length.

			bytes const data;  ///< Bytes of Header.
			RLP const header;  ///< Header RLP.

			uint16_t const protocolId;   ///< Protocol ID as negotiated by handshake.
			bool const multiFrame;       ///< If this frame is part of a sequence
			uint16_t const sequenceId;   ///< Sequence ID of frame
			uint32_t const totalLength;  ///< Total length of packet in first frame of multiframe packet.
		};

		/**
		* @brief Encoder/decoder transport for RLPx connection established by RLPXHandshake.
		*
		* @todo rename to RLPXTranscoder
		* @todo Remove 'Frame' nomenclature and expect caller to provide RLPXFrame
		* @todo Remove handshake as friend, remove handshake-based constructor
		*
		* Thread Safety
		* Distinct Objects: Unsafe.
		* Shared objects: Unsafe.
		*/
		class RLPXFrameCoder
		{
			friend class Session;

		public:
			/// Construct; requires instance of RLPXHandshake which has encrypted ECDH key exchange
			/// (first two phases of handshake).
			explicit RLPXFrameCoder(hankshake const& _init);

			/// Construct with external key material.
			RLPXFrameCoder(bool _originated, h512 const& _remoteEphemeral, h256 const& _remoteNonce,
				KeyPair const& _ecdheLocal, h256 const& _nonce, bytesConstRef _ackCipher,
				bytesConstRef _authCipher);

			~RLPXFrameCoder();

			/// Establish shared secrets and setup AES and MAC states.
			void setup(bool _originated, h512 const& _remoteEphemeral, h256 const& _remoteNonce,
				KeyPair const& _ecdheLocal, h256 const& _nonce, bytesConstRef _ackCipher,
				bytesConstRef _authCipher);

			/// Write single-frame payload of packet(s).
			void writeFrame(uint16_t _protocolType, bytesConstRef _payload, bytes& o_bytes);

			/// Write continuation frame of segmented payload.
			void writeFrame(
				uint16_t _protocolType, uint16_t _seqId, bytesConstRef _payload, bytes& o_bytes);

			/// Write first frame of segmented or sequence-tagged payload.
			void writeFrame(uint16_t _protocolType, uint16_t _seqId, uint32_t _totalSize,
				bytesConstRef _payload, bytes& o_bytes);

			/// Legacy. Encrypt _packet as ill-defined legacy RLPx frame.
			void writeSingleFramePacket(bytesConstRef _packet, bytes& o_bytes);

			/// Authenticate and decrypt header in-place.
			bool authAndDecryptHeader(bytesRef io_cipherWithMac);

			/// Authenticate and decrypt frame in-place.
			bool authAndDecryptFrame(bytesRef io_cipherWithMac);

			/// Return first 16 bytes of current digest from egress mac.
			h128 egressDigest();

			/// Return first 16 bytes of current digest from ingress mac.
			h128 ingressDigest();

			/// add packet header(lenth)
			void writeFramePacketHeader(bytes& o_bytes);

			bytes serializePacketSize(uint32_t const & size);
			uint32_t deserializePacketSize(bytes const & data);

		protected:
			void writeFrame(RLPStream const& _header, bytesConstRef _payload, bytes& o_bytes);

			/// Update state of egress MAC with frame header.
			void updateEgressMACWithHeader(bytesConstRef _headerCipher);

			/// Update state of egress MAC with frame.
			void updateEgressMACWithFrame(bytesConstRef _cipher);

			/// Update state of ingress MAC with frame header.
			void updateIngressMACWithHeader(bytesConstRef _headerCipher);

			/// Update state of ingress MAC with frame.
			void updateIngressMACWithFrame(bytesConstRef _cipher);

		private:
			std::unique_ptr<class RLPXFrameCoderImpl> m_impl;
		};
	}
}