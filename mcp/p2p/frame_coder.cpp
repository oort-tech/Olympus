#include "frame_coder.hpp"
using namespace mcp;
using namespace mcp::p2p;
using namespace mcp::encry;

void frame_coder_impl::set_key(bool _originated, public_key_comp const&_pub, key_pair const&_key)
{
	public_key_comp remote_pub, local_pub;
	secret_encry local_sec;
	if (!get_encry_public_key_from_sign_key(remote_pub, _pub)
		|| !get_encry_secret_key_from_sign_key(local_sec, _key.secret())
		|| !get_encry_public_key_from_sign_key(local_pub, _key.pub_comp())
		)
	{
		LOG(m_log.info) << "set_key error.";
		return;
	}

	if (_originated) //local is client
	{
		if (crypto_kx_client_session_keys(in_key.ref().data(), out_key.ref().data(),
			local_pub.ref().data(), local_sec.ref().data(), remote_pub.ref().data()) != 0)
		{
            LOG(m_log.info) << "set_client_key error.";
			return;
		}
	}
	else  //local is server
	{
		if (crypto_kx_server_session_keys(in_key.ref().data(), out_key.ref().data(),
			local_pub.ref().data(), local_sec.ref().data(), remote_pub.ref().data()) != 0)
		{
            LOG(m_log.info) << "set_server_key error.";
			return;
		}
	}
}

void frame_coder_impl::set_nonce(nonce const&_remote_nonce, nonce const&_nonce)
{
	_remote_nonce.ref().copyTo(remote_nonce.ref());
	_nonce.ref().copyTo(local_nonce.ref());
}
bool frame_coder_impl::encry(bytes& io_cipher)
{
	size_t msg_len = io_cipher.size();
	bytes cipherText(crypto_cipher_len + msg_len);

	bytes plain(std::move(io_cipher));
	io_cipher.resize(msg_len + crypto_cipher_len);
	if (encryption(io_cipher.data(), plain.data(), msg_len, local_nonce.ref().data(), out_key.ref().data()) != 0)
	{
		return false;
	}

	return true;
}
bool frame_coder_impl::dencry(bytes& io_cipher)
{
	size_t msg_len = io_cipher.size();

	bytes plain(std::move(io_cipher));

	io_cipher.resize(msg_len - crypto_cipher_len);

	if (dencryption(io_cipher.data(), plain.data(), msg_len, remote_nonce.ref().data(), in_key.ref().data()) != 0)
	{
		return false;
	}
	return true;
}

bool frame_coder_impl::dencry(bytesConstRef io, bytes& o_bytes)
{
	size_t msg_len = io.size();

	if (o_bytes.size() != msg_len - crypto_cipher_len)
	{
		o_bytes.resize(msg_len - crypto_cipher_len);
	}

	if (crypto_secretbox_open_easy(o_bytes.data(), io.data(), msg_len, remote_nonce.ref().data(), in_key.ref().data()) != 0)
	{
		return false;
	}
	return true;
}

frame_coder::~frame_coder()
{}

frame_coder::frame_coder(hankshake const& _init) :
	m_impl(new frame_coder_impl)
{
	setup(_init.m_originated, _init.m_ecdheRemote, _init.m_remoteNonce, _init.m_ecdheLocal, _init.m_nonce);
}

frame_coder::frame_coder(bool _originated, public_key_comp const& _remoteEphemeral, nonce const& _remoteNonce, key_pair const& _ecdheLocal, nonce const& _nonce) :
	m_impl(new frame_coder_impl)
{
	setup(_originated, _remoteEphemeral, _remoteNonce, _ecdheLocal, _nonce);
}

void frame_coder::setup(bool _originated, public_key_comp const& _remoteEphemeral, nonce const& _remoteNonce, key_pair const& _ecdheLocal, nonce const& _nonce)
{
	m_impl->set_key(_originated,_remoteEphemeral, _ecdheLocal);
	m_impl->set_nonce(_remoteNonce, _nonce);
}

void frame_coder::write_frame(RLPStream const& _header, bytesConstRef _payload, bytes& o_bytes)
{
	bytes header(handshake_header_size);
	bytesConstRef(&_header.out()).copyTo(bytesRef(&header));

	if (!m_impl->encry(header))
	{
        LOG(m_log.info) << "encry Header error.";
		return;
	}


	o_bytes.swap(header);

	bytes payload(_payload.size());
	_payload.copyTo(bytesRef(&payload));
	if(!m_impl->encry(payload))
	{
        LOG(m_log.info) << "encry payload error.";
		return;
	}

	o_bytes.resize(crypto_cipher_len + mcp::p2p::handshake_header_size + payload.size());
	bytesRef macRef(o_bytes.data() + crypto_cipher_len + mcp::p2p::handshake_header_size, payload.size());
	bytesConstRef(&payload).copyTo(macRef);
}

void frame_coder::write_frame(bytesConstRef _payload, bytes& o_bytes)
{
	bytes payload(_payload.size());
	_payload.copyTo(bytesRef(&payload));
	if (!m_impl->encry(payload))
	{
        LOG(m_log.info) << "encry payload error.";
		return;
	}

	if (o_bytes.size() != payload.size())
	{
		o_bytes.resize(payload.size());
	}
	
	bytesRef macRef(o_bytes.data(), payload.size());
	bytesConstRef(&payload).copyTo(macRef);
}

void frame_coder::write_single_frame_packet(bytesConstRef _packet, bytes& o_bytes)
{
	if (!m_impl)
	{
        LOG(m_log.info) << "Internal error in write_single_frame_packet: frame_coder_impl disappeared.";
		return;
	}

	RLPStream header;
	uint32_t len = (uint32_t)_packet.size() + crypto_cipher_len;
	header.appendRaw(bytes({ byte((len >> 16) & 0xff), byte((len >> 8) & 0xff), byte(len & 0xff) }));

	write_frame(header, _packet, o_bytes);
}

void frame_coder::write_frame_packet(bytesConstRef _packet, bytes& o_bytes)
{
	if (!m_impl)
	{
        LOG(m_log.info) << "Internal error in write_single_frame_packet: frame_coder_impl disappeared.";
		return;
	}

	write_frame(_packet, o_bytes);
}

bool frame_coder::auth_and_decrypt_header(bytes& io, uint32_t& len)
{
	if (!m_impl)
	{
        LOG(m_log.info) << "Internal error in auth_and_decrypt_header: frame_coder_impl disappeared.";
		return false;
	}

	if (io.size() != handshake_header_size + crypto_cipher_len)
	{
        LOG(m_log.info) << "auth Decrypt Header lenth error.";
		return false;
	}

	if (!m_impl->dencry(io))
	{
        LOG(m_log.info) << "auth Decrypt Header error.";
		return false;
	}

	len = (uint32_t)(io[2]) | (uint32_t)(io[1]) << 8 | (uint32_t)(io[0]) << 16;

	return true;
}

bool frame_coder::auth_and_decrypt_frame(bytes& io)
{
	if (!m_impl)
	{
        LOG(m_log.info) << "Internal error in auth_and_decrypt_frame: frame_coder_impl disappeared.";
		return false;
	}

	if (io.size() < crypto_cipher_len)
	{
        LOG(m_log.info) << "auth Decrypt Frame lenth error.";
		return false;
	}

	if (!m_impl->dencry(io))
	{
        LOG(m_log.info) << "auth Decrypt Frame error.";
		return false;
	}

	return true;
}

bool frame_coder::auth_and_decrypt_frame(bytesConstRef io, bytes& o_bytes)
{
	if (!m_impl)
	{
        LOG(m_log.info) << "Internal error in auth_and_decrypt_frame: frame_coder_impl disappeared.";
		return false;
	}

	if (io.size() < crypto_cipher_len)
	{
        LOG(m_log.info) << "auth Decrypt Frame lenth error.";
		return false;
	}

	if (!m_impl->dencry(io, o_bytes))
	{
        LOG(m_log.info) << "auth Decrypt Frame error.";
		return false;
	}

	return true;
}

bytes frame_coder::serialize_packet_size(uint32_t const & size)
{
	std::vector<uint8_t> data(mcp::p2p::tcp_header_size);
	data[0] = (size >> 24) & 0xff;
	data[1] = (size >> 16) & 0xff;
	data[2] = (size >> 8) & 0xff;
	data[3] = size & 0xff;
	return data;
}

uint32_t frame_coder::deserialize_packet_size(bytes const & data)
{
	uint32_t size((data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3]);
	return size;
}

void frame_coder::write_frame_packet_header(bytes& o_bytes)
{
	uint32_t packet_size(o_bytes.size());
	dev::bytes header(serialize_packet_size(packet_size));

	uint32_t header_size = header.size();

	bytes resalt(std::move(o_bytes));

	o_bytes.resize(header_size + packet_size);

	dev::bytesConstRef(header.data(), header_size).copyTo(dev::bytesRef(o_bytes.data(), header_size));
	dev::bytesConstRef(resalt.data(), packet_size).copyTo(dev::bytesRef(o_bytes.data() + header_size, packet_size));
}


