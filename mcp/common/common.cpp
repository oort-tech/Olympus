#include "common.hpp"

#include <secp256k1.h>
#include <secp256k1_ecdh.h>
#include <secp256k1_recovery.h>
#include <secp256k1_sha256.h>

#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

std::string mcp::uint64_to_hex (uint64_t value_a)
{
	std::stringstream stream;
	stream << std::hex << std::noshowbase << std::setw (16) << std::setfill ('0');
	stream << value_a;
	
	std::string s = stream.str();
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

std::string mcp::uint64_to_hex_nofill(uint64_t value_a)
{
	std::stringstream stream;
	stream << "0x" << std::hex;
	stream << value_a;

	std::string s = stream.str();
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

std::string mcp::uint256_to_hex_nofill(uint256_t value_a)
{
	std::stringstream stream;
	stream << "0x" << std::hex;
	stream << value_a;
	
	std::string s = stream.str();
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

bool mcp::hex_to_uint64(std::string const & value_a, uint64_t & target_a, bool show_base)
{
	auto error(value_a.empty());
	if (!error)
	{
		error = value_a.size() > (show_base ? 18 : 16);
		if (!error)
		{
			std::stringstream stream(value_a);
			stream << std::hex << (show_base ? std::showbase : std::noshowbase);
			try
			{
				uint64_t number_l;
				stream >> number_l;
				target_a = number_l;
				if (!stream.eof())
				{
					error = true;
				}
			}
			catch (std::runtime_error &)
			{
				error = true;
			}
		}
	}
	return error;
}

bool mcp::hex_to_uint256(std::string const & value_a, uint256_t & target_a, bool show_base)
{
	auto error(value_a.empty());
	if (!error)
	{
		error = value_a.size() > (show_base ? 66 : 64);
		if (!error)
		{
			std::stringstream stream(value_a);
			stream << std::hex << (show_base ? std::showbase : std::noshowbase);
			try
			{
				uint256_t number_l;
				stream >> number_l;
				target_a = number_l;
				if (!stream.eof())
				{
					error = true;
				}
			}
			catch (std::runtime_error &)
			{
				error = true;
			}
		}
	}
	return error;
}

std::string mcp::bytes_to_hex(dev::bytes const & b)
{
	static char const* hexdigits = "0123456789abcdef";
	std::string hex(b.size() * 2, '0');
	int off = 0;
	for (auto it(b.begin()); it != b.end(); it++)
	{
		hex[off++] = hexdigits[(*it >> 4) & 0x0f];
		hex[off++] = hexdigits[*it & 0x0f];
	}
	return hex;
}

int mcp::from_hex_char(char c) noexcept
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return -1;
}

bool mcp::hex_to_bytes(std::string const & str , dev::bytes & out)
{
	bool error(str.size() % 2 != 0);
	if (error)
		return error;

	unsigned s = (str.size() >= 2 && str[0] == '0' && str[1] == 'x') ? 2 : 0;
	out.reserve((str.size() - s + 1) / 2);
	for (unsigned i = s; i < str.size(); i += 2)
	{
		int h = from_hex_char(str[i]);
		int l = from_hex_char(str[i + 1]);
		if (h != -1 && l != -1)
			out.push_back((byte)(h * 16 + l));
		else
		{
			error = true;
			break;
		}
	}
	return error;
}

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

secp256k1_context const* get_secp256k1_ctx()
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
