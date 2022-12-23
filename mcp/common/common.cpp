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

int mcp::encry::encryption(unsigned char *c, const unsigned char *m,
	unsigned long long mlen, const unsigned char *n,
	const unsigned char *ek) {
	CryptoPP::AES::Encryption alg(ek, 32);
	CryptoPP::CTR_Mode_ExternalCipher::Encryption enc(alg, n);
	enc.ProcessData(c, m, mlen);
	return 0;
}

int mcp::encry::dencryption(unsigned char *m, const unsigned char *c,
	unsigned long long clen, const unsigned char *n,
	const unsigned char *ek) {
	CryptoPP::AES::Encryption alg(ek, 32);
	CryptoPP::CTR_Mode_ExternalCipher::Decryption dec(alg, n);
	dec.ProcessData(m, c, clen);
	return 0;
}
