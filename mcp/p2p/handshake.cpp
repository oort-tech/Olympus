#include "handshake.hpp"
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

	id = r[0].toHash<node_id>();
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
	uint32_t len = (uint32_t)data.size();
	m_handshakeOutBuffer.clear();
	m_handshakeOutBuffer.resize(mcp::p2p::handshake_header_size + len);

	dev::bytesRef bytes_len(&m_handshakeOutBuffer[0], mcp::p2p::handshake_header_size);
	dev::bytesRef bytes_data(&m_handshakeOutBuffer[mcp::p2p::handshake_header_size], len);
	dev::bytes bytelen({ byte((len >> 16) & 0xff), byte((len >> 8) & 0xff), byte(len & 0xff) });
	bytesConstRef(&bytelen).copyTo(bytes_len);
	bytesConstRef(&data).copyTo(bytes_data);
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

void mcp::p2p::hankshake::read()
{
	m_handshakeInBuffer.clear();
	auto self(shared_from_this());
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

uint32_t mcp::p2p::hankshake::packet_size()
{
	uint32_t size = 0;
	if (State::ExchgPublic == m_curState || State::AckExchgPublic == m_curState)
		size = 71;
	else if (State::New == m_curState)
		size = 307;
	else if (State::AckAuth == m_curState)
		size = 210;

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
	dev::bytes buf(Signature::size + h256::size + Public::size + h256::size + 1);
	bytesRef sig(&buf[0], Signature::size);
	bytesRef hepubk(&buf[Signature::size], h256::size);
	bytesRef pubk(&buf[Signature::size + h256::size], Public::size);
	bytesRef nonce(&buf[Signature::size + h256::size + Public::size], h256::size);

	//// E(remote-pubk, S(ecdhe-random, ecdh-public) || H(ecdhe-random-pubk) || pubk || nonce || 0x0)
	Secret staticShared;
	crypto::ecdh::agree(m_host->alias.secret(), m_remote, staticShared);
	sign(m_ecdheLocal.secret(), staticShared.makeInsecure() ^ m_nonce).ref().copyTo(sig);
	sha3(m_ecdheLocal.pub().ref(), hepubk);
	m_host->alias.pub().ref().copyTo(pubk);
	m_nonce.ref().copyTo(nonce);
	buf[buf.size() - 1] = 0x0;
	encryptECIES(m_remote, &buf, m_authCipher);

	send(m_authCipher);
}

void hankshake::writeAck()
{
	dev::bytes buf(Public::size + h256::size + 1);
	bytesRef epubk(&buf[0], dev::Public::size);
	bytesRef nonce(&buf[dev::Public::size], h256::size);
	m_ecdheLocal.pub().ref().copyTo(epubk);
	m_nonce.ref().copyTo(nonce);
	buf[buf.size() - 1] = 0x0;
	encryptECIES(m_remote, &buf, m_ackCipher);

	send(m_ackCipher);
}

void hankshake::setAuthValues(dev::Signature const& _sig, Public const& _remotePubk, h256 const& _remoteNonce)
{
	_remotePubk.ref().copyTo(m_remote.ref());
	_remoteNonce.ref().copyTo(m_remoteNonce.ref());
	Secret sharedSecret;
	crypto::ecdh::agree(m_host->alias.secret(), _remotePubk, sharedSecret);
	m_ecdheRemote = recover(_sig, sharedSecret.makeInsecure() ^ _remoteNonce);
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
	m_authCipher = std::move(m_handshakeInBuffer);
	dev::bytes buf;
	if (decryptECIES(m_host->alias.secret(), bytesConstRef(&m_authCipher), buf))
	{
		bytesConstRef data(&buf);

		Signature sig(data.cropped(0, Signature::size));
		Public pubk(data.cropped(Signature::size + h256::size, Public::size));
		h256 nonce(data.cropped(Signature::size + h256::size + Public::size, h256::size));
		setAuthValues(sig, pubk, nonce);
	}
	else
	{
		boost::system::error_code ec;
		LOG(m_log.info) << "p2p.connect.ingress readAuth decryptECIES error. " << m_socket->remote_endpoint(ec);
		m_nextState = Error;
	}
}

void hankshake::readAck()
{
	m_ackCipher = std::move(m_handshakeInBuffer);
	dev::bytes buf;
	if (decryptECIES(m_host->alias.secret(), bytesConstRef(&m_ackCipher), buf))
	{
		bytesConstRef(&buf).cropped(0, Public::size).copyTo(m_ecdheRemote.ref());
		bytesConstRef(&buf).cropped(Public::size, h256::size).copyTo(m_remoteNonce.ref());
	}
	else
	{
		boost::system::error_code ec;
		LOG(m_log.info) << "p2p.connect.ingress readAck decryptECIES error. " << m_socket->remote_endpoint(ec);
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
		m_io.reset(new RLPXFrameCoder(*this));

		hankshake_msg handmsg(m_host->id(), mcp::p2p::version, mcp::mcp_network, m_host->caps());
		RLPStream s;
		s.append((unsigned)packet_type::ack);
		handmsg.stream_RLP(s);

		bytes packet;
		s.swapOut(packet);

		m_io->writeSingleFramePacket(&packet, m_handshakeOutBuffer);

		ba::async_write(*m_socket, ba::buffer(m_handshakeOutBuffer), [this, self](boost::system::error_code ec, std::size_t)
		{
			transition(ec);
		});
	}
	else if (m_nextState == ReadHello)
	{
		// Authenticate and decrypt initial hello frame with initial frame coder
		// and request m_host to start session.
		m_nextState = StartSession;

		// read frame header
		constexpr size_t handshakeSizeBytes = 32;
		m_handshakeInBuffer.resize(handshakeSizeBytes);
		ba::async_read(*m_socket, boost::asio::buffer(m_handshakeInBuffer, handshakeSizeBytes), [this, self](boost::system::error_code ec, std::size_t)
		{
			if (ec)
				transition(ec);
			else
			{
				if (!m_io)
				{
                    LOG(m_log.info) << "Internal error in handshake: frame coder disappeared.";
					m_nextState = Error;
					transition();
					return;

				}

				/// authenticate and decrypt header
				if (!m_io->authAndDecryptHeader(bytesRef(m_handshakeInBuffer.data(), m_handshakeInBuffer.size())))
				{
					m_nextState = Error;
					transition();
					return;
				}

				/// check frame size
				bytes const& header = m_handshakeInBuffer;
				uint32_t const frameSize = (uint32_t)(header[2]) | (uint32_t)(header[1]) << 8 |
					(uint32_t)(header[0]) << 16;
				constexpr size_t expectedFrameSizeBytes = 1024;

				if (frameSize > expectedFrameSizeBytes || frameSize == 0)
				{
                    LOG(m_log.info)
						<< (m_originated ? "p2p.connect.egress" : "p2p.connect.ingress")
						<< " hello frame is too large " << frameSize;
					m_nextState = Error;
					transition();
					return;
				}

				/// read padded frame and mac
				constexpr size_t byteBoundary = 16;
				m_handshakeInBuffer.resize(
					frameSize + ((byteBoundary - (frameSize % byteBoundary)) % byteBoundary) +
					h128::size);
				ba::async_read(*m_socket, boost::asio::buffer(m_handshakeInBuffer, m_handshakeInBuffer.size()), [this, self, frameSize](boost::system::error_code ec, std::size_t)
				{
					m_idleTimer.cancel();
					if (ec)
						transition(ec);
					else
					{
						if (!m_io)
						{
                            LOG(m_log.info) << "Internal error in handshake: frame coder disappeared.";
							m_nextState = Error;
							transition();
							return;

						}

						if (!m_io->authAndDecryptFrame(bytesRef(&m_handshakeInBuffer)))
						{
                            LOG(m_log.info)
								<< (m_originated ? "p2p.connect.egress" : "p2p.connect.ingress")
								<< " hello frame: decrypt failed";
							m_nextState = Error;
							transition();
							return;
						}
						bytesConstRef frame(m_handshakeInBuffer.data(), frameSize);

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
