#include "common.hpp"

#include <secp256k1-vrf.h>
#include <secp256k1_ecdh.h>
#include <secp256k1_recovery.h>

#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

mcp::key_pair::key_pair(dev::Secret const & seed)
{
	try {
		m_secret = seed;
		m_public = dev::toPublic(m_secret);
		m_public_comp = dev::toPublicCompressed(m_secret);
		flag = true;
	}
	catch (...) {
		flag = false;
	}
}

mcp::key_pair::~key_pair()
{
	m_secret.clear();
	m_public.clear();
	m_public_comp.clear();
	m_account.clear();
}

mcp::key_pair mcp::key_pair::create()
{
	while (true)
	{
		dev::Secret seed;
		mcp::random_pool.GenerateBlock((byte*)seed.data(), seed.size);

		key_pair m_key_pair(seed);
		if (m_key_pair.flag)
			return m_key_pair;
	}
}

secp256k1_context const* mcp::encry::get_secp256k1_ctx()
{
	static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
		secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
		&secp256k1_context_destroy
	};
	return s_ctx.get();
}

int mcp::encry::get_encryption_key(dev::Secret &key, const unsigned char* pk, const size_t pkLen, const dev::Secret &sk)
{
	auto* ctx = get_secp256k1_ctx();

	secp256k1_pubkey rawPubKey;
	if (!secp256k1_ec_pubkey_parse(ctx, &rawPubKey, pk, pkLen)) {
		return 1;
	}
	if (!secp256k1_ec_pubkey_tweak_mul(ctx, &rawPubKey, sk.data())) {
		return 1;
	}

	std::array<byte, 65> serializedPubkey;
	auto serializedPubkeySize = serializedPubkey.size();
	secp256k1_ec_pubkey_serialize(
		ctx,
		serializedPubkey.data(),
		&serializedPubkeySize,
		&rawPubKey,
		SECP256K1_EC_UNCOMPRESSED
	);

	if (serializedPubkeySize != serializedPubkey.size() ||
		serializedPubkey[0] != 0x04
		) {
		return 1;
	}

	CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
	hkdf.DeriveKey((byte*)key.data(), key.size, &serializedPubkey[1], 64, NULL, 0, NULL, 0);

	return 0;
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

bool mcp::encry::verify(dev::h256 const &pkSlice, dev::Signature const &sig, dev::h256 const &hash)
{
	dev::PublicCompressed pkCompressed;
	pkSlice.ref().copyTo(pkCompressed.ref().cropped(1));

	pkCompressed[0] = 0x02;
	if (dev::verify(pkCompressed, dev::h512(sig.ref(), dev::h512::AlignLeft), hash)) {
		return true;
	}

	pkCompressed[0] = 0x03;
	if (dev::verify(pkCompressed, dev::h512(sig.ref(), dev::h512::AlignLeft), hash)) {
		return true;
	}

	return false;
}
