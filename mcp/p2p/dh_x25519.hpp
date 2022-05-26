#pragma once

#include <mcp/common/common.hpp>
#include <mcp/common/log.hpp>

namespace mcp
{
	namespace p2p
	{
		void encrypt_dh(dev::PublicCompressed const& _k, dev::bytesConstRef _plain, dev::bytes& o_cipher);
		bool dencrypt_dh(dev::Secret const& _k, dev::bytesConstRef _cipher, dev::bytes& o_plaintext);
		
		class dh_x25519
		{
		public:
			static dh_x25519* get();

			void encrypt_x25519(dev::PublicCompressed const& _k, dev::bytes& io_cipher);

			void encrypt_x25519(dev::PublicCompressed const& _k, dev::bytesConstRef _sharedMacData, dev::bytes& io_cipher);

			bool decrypt_x25519(dev::Secret const& _k, dev::bytes& io_text);

			bool decrypt_x25519(dev::Secret const& _k, dev::bytesConstRef _sharedMacData, dev::bytes& io_text);

		private:
			dh_x25519() = default;
            mcp::log m_log = { mcp::log("p2p") };
		};

	}
}
