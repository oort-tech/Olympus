#include "dh_x25519.hpp"
using namespace mcp;
using namespace mcp::p2p;
using namespace mcp::encry;
using namespace dev;

void mcp::p2p::encrypt_dh(public_key_comp const & _k, dev::bytesConstRef _plain, dev::bytes & o_cipher)
{
	bytes io = _plain.toBytes();
	dh_x25519::get()->encrypt_x25519(_k, io);
	o_cipher = std::move(io);
}

bool mcp::p2p::dencrypt_dh(secret_key const & _k, dev::bytesConstRef _cipher, dev::bytes & o_plaintext)
{
	bytes io = _cipher.toBytes();
	if (!dh_x25519::get()->decrypt_x25519(_k, io))
		return false;
	o_plaintext = std::move(io);
	return true;
}


dh_x25519* dh_x25519::get()
{
	static dh_x25519 s_this;
	return &s_this;
}

void dh_x25519::encrypt_x25519(public_key_comp const& _k, bytes& io_cipher)
{
	////get encry public key
	public_key_comp public_encry;
	if (!get_encry_public_key_from_sign_key(public_encry, _k))
	{
		LOG(m_log.info) << "encrypt get encry public key error.";
		return;
	}

	encrypt_x25519(public_encry, bytesConstRef(), io_cipher);
}

void dh_x25519::encrypt_x25519(public_key_comp const& _k, bytesConstRef _sharedMacData, bytes& io_cipher)
{
	// create key pair
	auto r = key_pair::create();

	//random nonce
	auto iv = nonce::get();

	//get encry secret key
	secret_encry encry_secret;
	if (!get_encry_secret_key_from_sign_key(encry_secret, r.secret()))
	{
        LOG(m_log.info) << "encrypt get encry secret key error.";
		return;
	}

	size_t msg_len = io_cipher.size();
	bytes cipherText(crypto_cipher_len + msg_len);
	if (encryption(cipherText.data(), io_cipher.data(), msg_len, iv.ref().data(), _k.ref().data(), encry_secret.ref().data()) != 0)
	{
        LOG(m_log.info) << "encryption error.";
		return;
	}

	bytes msg(1 + public_key::size + nonce::size + cipherText.size());
	msg[0] = 0x04;

	auto pub = bytesConstRef(r.pub().bytes.data(), 32);
	pub.copyTo(bytesRef(&msg).cropped(1, public_key::size));

	iv.ref().copyTo(bytesRef(&msg).cropped(1 + public_key::size, nonce::size));
	bytesRef msgCipherRef = bytesRef(&msg).cropped(1 + public_key::size + nonce::size, cipherText.size());
	bytesConstRef(&cipherText).copyTo(msgCipherRef);

	io_cipher.resize(msg.size());
	io_cipher.swap(msg);
}

bool dh_x25519::decrypt_x25519(secret_key const& _k, bytes& io_text)
{
	//get encry secret key
	secret_encry encry_secret;
	if (!get_encry_secret_key_from_sign_key(encry_secret, _k))
	{
        LOG(m_log.info) << "decrypt_x25519 get decrypt secret key error.";
		return false;
	}

	return decrypt_x25519(encry_secret, bytesConstRef(), io_text);
}

bool dh_x25519::decrypt_x25519(secret_encry const& _k, bytesConstRef _sharedMacData, bytes& io_text)
{

	// interop w/go ecies implementation

	// io_cipher[0] must be 2, 3, or 4, else invalidpublickey
	if (io_text.empty() || io_text[0] < 2 || io_text[0] > 4)
		// invalid message: publickey
		return false;

	if (io_text.size() < (1 + public_key::size + nonce::size))
		// invalid message: length
		return false;

	bytesConstRef public_key(io_text.data() + 1, public_key::size);


	////get encry public key
	mcp::public_key_comp encry_pub;
	if (!get_encry_public_key_from_sign_key(encry_pub, public_key))
	{
        LOG(m_log.info) << "decrypt_x25519 to get public key error.";
		return false;
	}

	size_t cipherLen = io_text.size() - 1 - public_key::size - nonce::size;
	bytesConstRef cipherWithIV(io_text.data() + 1 + public_key::size, nonce::size + cipherLen);
	bytesConstRef cipherIV = cipherWithIV.cropped(0, nonce::size);
	bytesConstRef cipherNoIV = cipherWithIV.cropped(nonce::size, cipherLen);
	nonce iv(cipherIV.toBytes());
	
	bytes plain(cipherNoIV.size() - crypto_cipher_len);
	if (dencryption(plain.data(), cipherNoIV.toBytes().data(), cipherLen, iv.ref().data(),
		encry_pub.ref().data(), _k.ref().data()) != 0) {
		return false;
	}

	io_text.resize(plain.size());
	io_text.swap(plain);

	return true;
}


