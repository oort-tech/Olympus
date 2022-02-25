#include "handshake.hpp"
#include <mcp/p2p/dh_x25519.hpp>
using namespace mcp;
using namespace mcp::p2p;
using namespace dev;

mcp::p2p::hankshake_msg::hankshake_msg(node_id const & node_id_a, uint16_t const & version_a, mcp::mcp_networks const & network_a, std::list<capability_desc> const & cap_descs_a) : 
	id(node_id_a),
	version(version_a),
	network(network_a),
	cap_descs(cap_descs_a)
{
}

mcp::p2p::hankshake_msg::hankshake_msg(dev::RLP const & r)
{
	if (r.itemCount() != 4)
		throw std::runtime_error("invalid handshake_message rlp format");

	id = r[0].toHash<mcp::public_key_comp>();
	version = (uint16_t)r[1];
	network = (mcp::mcp_networks)r[2].toInt<uint8_t>();
	for (auto const & i : r[3])
		cap_descs.push_back(capability_desc(i));
}

void hankshake_msg::stream_RLP(dev::RLPStream & s)
{
	s.appendList(4) << id << version << (uint8_t)network;
	s.appendList(cap_descs.size());
	for (auto & desc : cap_descs)
		desc.stream_RLP(s);
}

void mcp::p2p::hankshake::ComposeOutPacket(dev::bytes const& data)
{
	if (State::ExchgPublic == m_curState || State::AckExchgPublic == m_curState || m_transferVersion != 0)
	{
		uint32_t len = (uint32_t)data.size();
		m_handshakeOutBuffer.clear();
		m_handshakeOutBuffer.resize(mcp::p2p::handshake_header_size + len);

		dev::bytesRef bytes_len(&m_handshakeOutBuffer[0], mcp::p2p::handshake_header_size);
		dev::bytesRef bytes_data(&m_handshakeOutBuffer[mcp::p2p::handshake_header_size], len);
		dev::bytes bytelen({ byte((len >> 16) & 0xff), byte((len >> 8) & 0xff), byte(len & 0xff) });
		bytesConstRef(&bytelen).copyTo(bytes_len);
		bytesConstRef(&data).copyTo(bytes_data);
	}
	else
	{
		uint32_t len = (uint32_t)data.size();
		m_handshakeOutBuffer.clear();
		m_handshakeOutBuffer.resize(len);
		dev::bytesRef ref(&m_handshakeOutBuffer[0], len);
		bytesConstRef(&data).copyTo(ref);
	}
}

void mcp::p2p::hankshake::send(dev::bytes const& data)
{
	if (data.empty())
	{
		LOG(m_log.info) << "p2p send data empty, handshake failed.";
		m_nextState = Error;
		transition();
		return;
	}
	ComposeOutPacket(data);

	auto self(shared_from_this());
	ba::async_write(*m_socket, ba::buffer(m_handshakeOutBuffer), [this, self](boost::system::error_code ec, std::size_t)
	{
		transition(ec);
	});
}

void mcp::p2p::hankshake::read_info()
{
	m_handshakeInBuffer.resize(mcp::p2p::handshake_header_size);
	//read header
	auto self(shared_from_this());
	ba::async_read(*m_socket, ba::buffer(m_handshakeInBuffer, m_handshakeInBuffer.size()), [this, self](boost::system::error_code ec, std::size_t)
	{
		if (ec)
		{
			transition(ec);
			return;
		}
		uint32_t len = (uint32_t)(m_handshakeInBuffer[2]) | (uint32_t)(m_handshakeInBuffer[1]) << 8 | (uint32_t)(m_handshakeInBuffer[0]) << 16;
		if (len == 0)
		{
			m_nextState = Error;
			transition(ec);
			return;
		}
		if (packet_size() == len)
		{
			//read body
			m_handshakeInBuffer.resize(len);
			ba::async_read(*m_socket, ba::buffer(m_handshakeInBuffer, m_handshakeInBuffer.size()), [this, self](boost::system::error_code ec, std::size_t)
			{
				if (ec)
				{
					transition(ec);
					return;
				}
				do_process();
			});
		}
		else //remote version 0
		{
			m_curState = State::New;
			dev::bytes buf(packet_size() - mcp::p2p::handshake_header_size);
			std::shared_ptr<dev::bytes> buf_ptr = std::make_shared<dev::bytes>(buf);
			ba::async_read(*m_socket, ba::buffer(*buf_ptr, buf_ptr->size()), [this, self, buf_ptr](boost::system::error_code ec, std::size_t)
			{
				if (ec)
				{
					transition(ec);
					return;
				}
				dev::bytes buf = *buf_ptr;
				m_handshakeInBuffer.resize(m_handshakeInBuffer.size() + buf.size());
				dev::bytesRef ref(&m_handshakeInBuffer[mcp::p2p::handshake_header_size], buf.size());
				bytesConstRef(&buf).copyTo(ref);
				m_nextState = AckAuth; //skip transaction info
				do_process();
			});
		}
	});
}

void mcp::p2p::hankshake::read()
{
	m_handshakeInBuffer.clear();
	auto self(shared_from_this());
	if (State::ExchgPublic == m_curState || State::AckExchgPublic == m_curState) //compatible verson 0
		read_info();
	else if (m_transferVersion != 0) //version 1, need read header first
	{
		m_handshakeInBuffer.resize(mcp::p2p::handshake_header_size);
		//read header
		ba::async_read(*m_socket, ba::buffer(m_handshakeInBuffer, m_handshakeInBuffer.size()), [this, self](boost::system::error_code ec, std::size_t)
		{
			if (ec)
			{
				transition(ec);
				return;
			}
			uint32_t len = (uint32_t)(m_handshakeInBuffer[2]) | (uint32_t)(m_handshakeInBuffer[1]) << 8 | (uint32_t)(m_handshakeInBuffer[0]) << 16;
			if (len == 0 || len != packet_size())
			{
				m_nextState = Error;
				transition(ec);
				return;
			}
			//read body
			m_handshakeInBuffer.resize(len);
			ba::async_read(*m_socket, ba::buffer(m_handshakeInBuffer, m_handshakeInBuffer.size()), [this, self](boost::system::error_code ec, std::size_t)
			{
				if (ec)
				{
					transition(ec);
					return;
				}
				do_process();
			});
		});
	}
	else //version 0, have no header, read body
	{
		m_handshakeInBuffer.resize(packet_size());
		ba::async_read(*m_socket, ba::buffer(m_handshakeInBuffer, m_handshakeInBuffer.size()), [this, self](boost::system::error_code ec, std::size_t)
		{
			if (ec)
			{
				transition(ec);
				return;
			}
			do_process();
		});
	}
}

uint32_t mcp::p2p::hankshake::packet_size()
{
	uint32_t size = 0;
	if (State::ExchgPublic == m_curState || State::AckExchgPublic == m_curState)
		size = 37;
	else if (State::New == m_curState)
		size = 227;
	else if (State::AckAuth == m_curState)
		size = 130;

	return size;
}

void mcp::p2p::hankshake::do_process()
{
	switch (m_curState)
	{
	case State::ExchgPublic:
	case State::AckExchgPublic:
		readInfo();
		break;
	case State::New:
		readAuth();
		break;
	case State::AckAuth:
		readAck();
		break;
	default:
		break;
	}
	transition();
}

void mcp::p2p::hankshake::writeInfo()
{
	std::list<capability_desc> caps;
	hankshake_msg handmsg(m_host->id(), mcp::p2p::version, mcp::mcp_network, caps);
	RLPStream s;

	handmsg.stream_RLP(s);
	dev::bytes buf;
	s.swapOut(buf);

	send(buf);
}

void hankshake::writeAuth()
{
	dev::bytes buf(signature::size + public_key_comp::size + public_key_comp::size + nonce::size + 1);
	dev::bytes buf_cipher;
	dev::bytesRef sig(&buf[0], signature::size);
	dev::bytesRef hepubk(&buf[signature::size], public_key_comp::size);
	dev::bytesRef pubk(&buf[signature::size + public_key_comp::size], public_key_comp::size);
	dev::bytesRef nonce_l(&buf[signature::size + public_key_comp::size + public_key_comp::size], nonce::size);

	// E(remote-pubk, S(ecdhe-random, ecdh-public) || H(ecdhe-random-pubk) || pubk || nonce || 0x0)
	mcp::signature sig_data;
	mcp::encry::sign(m_host->alias.secret(), m_ecdheLocal.pub().ref(), sig_data);
	sig_data.ref().copyTo(sig);

	m_ecdheLocal.pub().ref().copyTo(hepubk);
	m_host->alias.pub_comp().ref().copyTo(pubk);

	m_nonce.ref().copyTo(nonce_l);
	buf[buf.size() - 1] = 0x0;
	encrypt_dh(m_ecdheRemote, &buf, buf_cipher);

	send(buf_cipher);
}

void hankshake::writeAck()
{
	dev::bytes buf(public_key_comp::size + nonce::size + 1);
	dev::bytes buf_cipher;
	bytesRef epubk(&buf[0], public_key_comp::size);
	bytesRef nonce_l(&buf[public_key_comp::size], nonce::size);

	m_ecdheLocal.pub().ref().copyTo(epubk);
	m_nonce.ref().copyTo(nonce_l);
	buf[buf.size() - 1] = 0x0;
	encrypt_dh(m_ecdheRemote, &buf, buf_cipher);

	send(buf_cipher);
}

void hankshake::setAuthValues(mcp::signature const& _sig, public_key_comp const& _hePubk, public_key_comp const& _remotePubk, mcp::nonce const& _remoteNonce)
{
	bool ret = mcp::encry::verify(_hePubk, _sig, _remotePubk.ref());
	if (!ret)
	{
        LOG(m_log.info) << "remote sign key or ephemeral public key error: ";
		m_nextState = Error;
		return;
	}

	if (m_remote != _remotePubk && !m_remote.is_zero())
	{
		LOG(m_log.info) << "remote key error: " << m_remote.to_string() << " :" << _remotePubk.to_string();
		m_nextState = Error;
		return;
	}

	_hePubk.ref().copyTo(m_ecdheRemote.ref());	/// transfer encrypt public key
	_remotePubk.ref().copyTo(m_remote.ref());	/// transfer signature public key

	_remoteNonce.ref().copyTo(m_remoteNonce.ref());
}

void mcp::p2p::hankshake::readInfo()
{
	bytesRef frame(&m_handshakeInBuffer);
	dev::RLP rlp(frame);
	hankshake_msg msg(rlp);

	if (m_originated && m_remote != msg.id)	//remote id modify, bootstrap need clear this id
	{
		m_host->replace_bootstrap(m_remote, msg.id);
	}

	m_transferVersion = mcp::p2p::version < msg.version ? mcp::p2p::version : msg.version;
	m_remoteVersion = msg.version;
	m_remote = msg.id;

	if (msg.network != mcp::mcp_network)
	{
		LOG(m_log.info) << "p2p mcp_network error,remote mcp_network: " << (int)msg.network << " local mcp_network " << (int)mcp::mcp_network;
		m_nextState = Error;
		return;
	}
}

void hankshake::readAuth()
{
	dev::bytes buf;
	if (dencrypt_dh(m_ecdheLocal.secret(), bytesConstRef(&m_handshakeInBuffer), buf))
	{
		bytesConstRef data(&buf);

		signature sig;
		data.cropped(0, signature::size).copyTo(sig.ref());

		public_key_comp hepubk;
		data.cropped(signature::size, public_key_comp::size).copyTo(hepubk.ref());

		public_key_comp pubk;
		data.cropped(signature::size + public_key_comp::size, public_key_comp::size).copyTo(pubk.ref());

		mcp::nonce nonce_l;
		data.cropped(signature::size + public_key_comp::size + public_key_comp::size, nonce::size).copyTo(nonce_l.ref());

		setAuthValues(sig, hepubk, pubk, nonce_l);
	}
	else
	{
		boost::system::error_code ec;
		LOG(m_log.info) << "p2p.connect.ingress readAuth decrypt_x25519 error. " << m_socket->remote_endpoint(ec);
		m_nextState = Error;
	}
}

void hankshake::readAck()
{
	dev::bytes buf;
	if (dencrypt_dh(m_ecdheLocal.secret(), bytesConstRef(&m_handshakeInBuffer), buf))
	{
		bytesConstRef(&buf).cropped(0, public_key_comp::size).copyTo(m_ecdheRemote.ref());

		bytesConstRef(&buf).cropped(public_key_comp::size, nonce::size).copyTo(m_remoteNonce.ref());
	}
	else
	{
		boost::system::error_code ec;
		LOG(m_log.info) << "p2p.connect.ingress readAck decrypt_x25519 error. " << m_socket->remote_endpoint(ec);
		m_nextState = Error;
	}
}

void hankshake::cancel()
{
	m_cancel = true;
	m_idleTimer.cancel();

	if (m_originated)
	{
		std::lock_guard<std::mutex> lock(m_host->pending_conns_mutex);
		m_host->pending_conns.erase(m_remote);
	}
	try
	{
		m_io.reset();
		if (m_socket->is_open())
		{
			m_socket->close();
		}
	}
	catch (const std::exception&)
	{
	}
}

void hankshake::error()
{
	auto connected = m_socket->is_open();
	boost::system::error_code ec;
	if (connected && !m_socket->remote_endpoint(ec).address().is_unspecified())
        LOG(m_log.info) << "Disconnecting " << m_socket->remote_endpoint(ec) << " (Handshake Failed)";
	else
        LOG(m_log.info) << "Handshake Failed (Connection reset by peer)";

	cancel();
}

void hankshake::transition(boost::system::error_code _ech)
{
	// reset timeout
	m_idleTimer.cancel();

	if (_ech || m_nextState == Error || m_cancel)
	{
		if (m_originated && _ech)
			m_host->attempt_outs.record(m_remote);

        LOG(m_log.info) << "Handshake Failed (I/O Error: " << _ech.message() << ")";
		return error();
	}

	if (m_nextState == StartSession)
	{
		LOG(m_log.info) << "Handshake Failed remote probability aggressor.";
		return error();
	}

	auto self(shared_from_this());
	m_idleTimer.expires_from_now(c_timeout);
	m_idleTimer.async_wait([this, self](boost::system::error_code const& _ec)
	{
		if (!_ec)
		{
			boost::system::error_code e;
			if (!m_socket->remote_endpoint(e).address().is_unspecified())
                LOG(m_log.info) << "Disconnecting " << m_socket->remote_endpoint(e)
				<< " (Handshake Timeout)";
			cancel();
		}
	});

	if (m_nextState == ExchgPublic)
	{
		m_nextState = AckExchgPublic;
		m_curState = ExchgPublic;
		if (m_originated)
			writeInfo();
		else
			read();
	}
	else if (m_nextState == AckExchgPublic)
	{
		m_nextState = New;
		m_curState = AckExchgPublic;
		if (m_originated)
			read();
		else
			writeInfo();
	}
	else if (m_nextState == New)
	{
		m_nextState = AckAuth;
		m_curState = New;
		if (m_originated)
			writeAuth();
		else
			read();
	}
	else if (m_nextState == AckAuth)
	{
		m_nextState = WriteHello;
		m_curState = AckAuth;
		if (m_originated)
			read();
		else
			writeAck();
	}
	else if (m_nextState == WriteHello)
	{
		m_nextState = ReadHello;

		/// This pointer will be freed if there is an error otherwise
		/// it will be passed to Host which will take ownership.
		m_io.reset(new frame_coder(*this));

		hankshake_msg handmsg(m_host->id(), mcp::p2p::version, mcp::mcp_network, m_host->caps());
		RLPStream s;
		s.append((unsigned)packet_type::ack);
		handmsg.stream_RLP(s);

		bytes packet;
		s.swapOut(packet);
		m_io->write_single_frame_packet(&packet, m_handshakeOutBuffer);

		ba::async_write(*m_socket, ba::buffer(m_handshakeOutBuffer), [this, self](boost::system::error_code ec, std::size_t)
		{
			transition(ec);
		});
	}
	else if (m_nextState == ReadHello)
	{
		// Authenticate and decrypt initial hello frame with initial frame_coder
		// and request m_host to start session.
		m_nextState = StartSession;

		// read frame header
		unsigned const handshakeSize = crypto_cipher_len + mcp::p2p::handshake_header_size;
		m_handshakeInBuffer.resize(handshakeSize);
		ba::async_read(*m_socket, boost::asio::buffer(m_handshakeInBuffer, handshakeSize), [this, self](boost::system::error_code ec, std::size_t)
		{
			if (ec)
				transition(ec);
			else
			{
				if (!m_io)
				{
                    LOG(m_log.info) << "Internal error in handshake: frame_coder disappeared.";
					m_nextState = Error;
					transition();
					return;

				}
				/// authenticate and decrypt header
				uint32_t frameSize;
				if (!m_io->auth_and_decrypt_header(m_handshakeInBuffer, frameSize))
				{
					m_nextState = Error;
					transition();
					return;
				}

				/// check frame size
				if (frameSize > 1024 || frameSize == 0)
				{
                    LOG(m_log.info)
						<< (m_originated ? "p2p.connect.egress" : "p2p.connect.ingress")
						<< " hello frame is too large " << frameSize;
					m_nextState = Error;
					transition();
					return;
				}

				/// read padded frame and mac
				m_handshakeInBuffer.resize(frameSize);
				ba::async_read(*m_socket, boost::asio::buffer(m_handshakeInBuffer, m_handshakeInBuffer.size()), [this, self](boost::system::error_code ec, std::size_t)
				{
					m_idleTimer.cancel();
					if (ec)
						transition(ec);
					else
					{
						if (!m_io)
						{
                            LOG(m_log.info) << "Internal error in handshake: frame_coder disappeared.";
							m_nextState = Error;
							transition();
							return;

						}

						if (!m_io->auth_and_decrypt_frame(m_handshakeInBuffer))
						{
                            LOG(m_log.info)
								<< (m_originated ? "p2p.connect.egress" : "p2p.connect.ingress")
								<< " hello frame: decrypt failed";
							m_nextState = Error;
							transition();
							return;
						}

						bytesRef frame(&m_handshakeInBuffer);
						packet_type packetType = (packet_type)frame[0];
						if (packetType != packet_type::ack)
						{
                            LOG(m_log.info)
								<< (m_originated ? "p2p.connect.egress" : "p2p.connect.ingress")
								<< " hello frame: invalid packet type";
							m_nextState = Error;
							transition();
							return;
						}

						try
						{
							dev::RLP rlp(frame.cropped(1));

							m_host->start_peer(m_remote, rlp, move(m_io), m_socket);
						}
						catch (std::exception const& _e)
						{
                            LOG(m_log.info) << "Handshake causing an exception: " << _e.what();
							m_nextState = Error;
							transition();
						}
					}
				});
			}
		});
	}

}
