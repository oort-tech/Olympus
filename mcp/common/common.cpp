#include "common.hpp"

#include <secp256k1-vrf.h>
#include <secp256k1_ecdh.h>
#include <secp256k1_recovery.h>

#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

namespace mcp
{
	mcp::mru_list<mcp::block_hash> InvalidBlockCache(1000);
	fast_steady_clock SteadyClock;
}

int mcp::encry::aesCTRXOR(dev::h256& _result, dev::h128 const& _k, dev::h128 const& _iv, dev::bytesConstRef& _sec) {
	CryptoPP::AES::Encryption alg(_k.data(), _k.size);
	CryptoPP::CTR_Mode_ExternalCipher::Encryption enc(alg, _iv.data());
	enc.ProcessData(_result.data(), _sec.data(), _sec.size());

	return 0;
}
