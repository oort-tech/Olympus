#include "common.hpp"

std::string mcp::uint64_to_hex (uint64_t value_a)
{
	std::stringstream stream;
	stream << std::hex << std::uppercase << std::noshowbase << std::setw (16) << std::setfill ('0');
	stream << value_a;
	return stream.str ();
}

bool mcp::hex_to_uint64(std::string const & value_a, uint64_t & target_a)
{
	auto error(value_a.empty());
	if (!error)
	{
		error = value_a.size() > 16;
		if (!error)
		{
			std::stringstream stream(value_a);
			stream << std::hex << std::noshowbase;
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

std::string mcp::bytes_to_hex(dev::bytes const & b)
{
	static char const* hexdigits = "0123456789ABCDEF";
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

mcp::key_pair::key_pair(mcp::seed_key const & seed)
{
	// create ed-25519(sign) key pair,encryption need trasnsfer to curve-25519
	// disabled by michael at 1/5
	// int ret = crypto_sign_seed_keypair(m_public.ref().data(), m_secret.ref().data(), seed.ref().data());

	// if (ret == 0)
	// 	flag = true;

	seed.ref().copyTo(m_secret.ref());
	if (mcp::encry::generate_public_from_secret(m_secret, m_public)) {
		flag = true;
	}
}

mcp::key_pair::~key_pair()
{
	m_secret.clear();
	m_public.clear();
}

mcp::key_pair mcp::key_pair::create()
{
	while (true)
	{
		mcp::seed_key seed;
		mcp::random_pool.GenerateBlock(seed.ref().data(), seed.ref().size());

		key_pair m_key_pair(seed);
		if (m_key_pair.flag)
			return m_key_pair;
	}
}

int mcp::encry::encryption(unsigned char * c, const unsigned char * m, unsigned long long mlen, const unsigned char * n, const unsigned char * pk, const unsigned char * sk)
{
	return crypto_box_easy(c, m, mlen, n, pk, sk);
}

int mcp::encry::dencryption(unsigned char * m, const unsigned char * c, unsigned long long clen, const unsigned char * n, const unsigned char * pk, const unsigned char * sk)
{
	return crypto_box_open_easy(m, c, clen, n, pk, sk);
}

int mcp::encry::encryption(unsigned char * c, const unsigned char * m, unsigned long long mlen, const unsigned char * n, const unsigned char * k)
{
	return crypto_secretbox_easy(c, m, mlen, n, k);
}

int mcp::encry::dencryption(unsigned char * m, const unsigned char * c, unsigned long long clen, const unsigned char * n, const unsigned char * k)
{
	return crypto_secretbox_open_easy(m, c, clen, n, k);
}

bool mcp::encry::sign(secret_key const& _k, dev::bytesConstRef _hash, mcp::signature& sig)
{
	//TODO: sign _hash
	unsigned long long signed_message_len;
	int ret = crypto_sign_detached(sig.ref().data(), &signed_message_len,
		_hash.data(), _hash.size(), _k.ref().data());

	if (ret != 0)
		return false;

	return true;
}

//secret and public key detached
bool mcp::encry::sign(private_key const& _k, public_key const& _pk, dev::bytesConstRef _hash, mcp::signature& sig)
{
	dev::bytes key(secret_key::size);
	_k.ref().copyTo(dev::bytesRef(&key[0], private_key::size));
	_pk.ref().copyTo(dev::bytesRef(&key[private_key::size], public_key::size));

	unsigned long long signed_message_len;
	int ret = crypto_sign_detached(sig.ref().data(), &signed_message_len,
		_hash.data(), _hash.size(), key.data());

	if (ret != 0)
		return false;

	return true;
}

///input : _s = signature + original data
bool mcp::encry::verify(public_key const& _p, dev::bytesConstRef const& _s)
{
	// TODO: Verify w/o recovery (if faster).

	public_key msg;
	unsigned long long msg_len;
	if (crypto_sign_open(msg.bytes.data(), &msg_len,
		_s.data(), _s.size(), _p.ref().data()) == 0) {
		return true;
	}
	else
		return false;
}

///input : _s = signature , _o = original data
bool mcp::encry::verify(public_key const& _k, mcp::signature const& _s, dev::bytesConstRef const& _o)
{
	// TODO: Verify w/o recovery (if faster).
	if (crypto_sign_verify_detached(_s.ref().data(), _o.data(), _o.size(), _k.ref().data()) == 0) {
		return true;
	}
	else
		return false;
}

//ed25519 secret key to curve25519 secret key
bool mcp::encry::get_encry_secret_key_from_sign_key(secret_encry & curve, secret_key const & ed25519)
{
	return crypto_sign_ed25519_sk_to_curve25519(curve.ref().data(), ed25519.ref().data()) == 0;
}

//ed25519 public key to curve25519 public key
bool mcp::encry::get_encry_public_key_from_sign_key(public_key & curve, public_key const & ed25519)
{
	return crypto_sign_ed25519_pk_to_curve25519(curve.ref().data(), ed25519.ref().data()) == 0;
}

bool mcp::encry::get_encry_public_key_from_sign_key(public_key & curve, dev::bytesConstRef ed25519)
{
	return crypto_sign_ed25519_pk_to_curve25519(curve.ref().data(), ed25519.data()) == 0;
}

// modified by michael at 1/5
bool mcp::encry::generate_public_from_secret(secret_key const& _sk, public_key& _pk)
{
	// commented by michael at 1/5
	// secret_key sec;
	// return crypto_sign_seed_keypair(_pk.ref().data(), sec.ref().data(), _sk.ref().data());

	auto* ctx = get_secp256k1_ctx();
	secp256k1_pubkey rawPubkey;
	if (!secp256k1_ec_pubkey_create(ctx, &rawPubkey, _sk.ref().data()))
        return false;
	
	std::array<byte, 65> serializedPubkey;
    auto serializedPubkeySize = serializedPubkey.size();
    secp256k1_ec_pubkey_serialize(
        ctx,
		serializedPubkey.data(),
		&serializedPubkeySize,
		&rawPubkey,
		SECP256K1_EC_UNCOMPRESSED
	);

	if (serializedPubkeySize == serializedPubkey.size() && serializedPubkey[0] == 0x04) {
		dev::bytesRef(&serializedPubkey[1], 64).copyTo(_pk.ref());
		return true;
	}

	return false;
}

// added by michael at 1/5
secp256k1_context const* mcp::encry::get_secp256k1_ctx()
{
    static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
        secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
        &secp256k1_context_destroy
    };
    return s_ctx.get();
}
