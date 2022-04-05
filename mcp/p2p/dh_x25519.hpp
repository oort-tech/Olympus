#pragma once

#include <mcp/common/common.hpp>
#include <mcp/common/log.hpp>

namespace mcp
{
	namespace p2p
	{
		void encrypt_dh(mcp::public_key_comp2 const& _k, dev::bytesConstRef _plain, dev::bytes& o_cipher);
		bool dencrypt_dh(mcp::secret_key const& _k, dev::bytesConstRef _cipher, dev::bytes& o_plaintext);
		/**
		* DH x25519.
		*/
		class dh_x25519
		{
		public:
			static dh_x25519* get();

			/// Encrypts text (replace input). curve25519
			void encrypt_x25519(mcp::public_key_comp2 const& _k, dev::bytes& io_cipher);

			/// Encrypts text (replace input). curve25519
			void encrypt_x25519(mcp::public_key_comp2 const& _k, dev::bytesConstRef _sharedMacData, dev::bytes& io_cipher);

			/// Decrypts text (replace input). curve25519
			bool decrypt_x25519(mcp::secret_key const& _k, dev::bytes& io_text);

			/// Decrypts text (replace input). curve25519
			bool decrypt_x25519(mcp::secret_encry const& _k, dev::bytesConstRef _sharedMacData, dev::bytes& io_text);

		private:
			dh_x25519() = default;
            mcp::log m_log = { mcp::log("p2p") };
		};

	}
}
