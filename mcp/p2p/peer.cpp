#include "peer.hpp"
using namespace mcp::p2p;

peer::peer(std::shared_ptr<bi::tcp::socket> const & socket_a, node_id const & node_id_a, std::shared_ptr<peer_manager> peer_manager_a, std::unique_ptr<RLPXFrameCoder>&& _io, ba::io_service& io) :
	socket(socket_a),
	m_node_id(node_id_a),
	m_io_service(std::ref(io)),
	m_peer_manager(peer_manager_a),
	is_dropped(false),
	m_pmetrics(std::make_shared<peer_metrics>()),
	m_io(move(_io))
{
	_last_received = std::chrono::steady_clock::now();
	_create = std::chrono::steady_clock::now();
	read_header_buffer.resize(mcp::p2p::tcp_header_size);
}

peer::~peer()
{
    LOG(m_log.debug) << "Peer deconstruction:" << m_node_id.hex();

    try {
        if (socket->is_open())
            socket->close();
    }
    catch (...) {}
}

void peer::register_capability(std::shared_ptr<peer_capability> const & cap)
{
    capabilities.push_back(cap);
}

void peer::start()
{
    boost::system::error_code ec;
    LOG(m_log.info) << "Peer start, node id: " << m_node_id.hex() << "@" << socket->remote_endpoint(ec);

    auto this_l(shared_from_this());
    for (auto pc : capabilities)
        pc->cap->on_connect(this_l, pc->offset);

	m_peer_manager->record_connect(remote_node_id(), disconnect_reason::no_disconnect);
    ping();
    read_loop();
}

bool peer::is_connected()
{
    return socket->is_open();
}

void peer::disconnect(disconnect_reason const & reason)
{
    if (socket->is_open())
    {
        dev::RLPStream s;
        prep(s, (unsigned)packet_type::disconect, 1) << (unsigned)reason;
        send(s);
    }
    drop(reason);
}

std::chrono::steady_clock::time_point peer::last_received()
{
    return _last_received;
}

node_id mcp::p2p::peer::remote_node_id() const
{
    return m_node_id;
}

bi::tcp::endpoint mcp::p2p::peer::remote_endpoint() const
{
    boost::system::error_code ec;
    return socket->remote_endpoint(ec);
}

void peer::ping()
{
    dev::RLPStream s;
    send(prep(s, (unsigned)packet_type::ping));
}

std::shared_ptr<mcp::p2p::peer_metrics> mcp::p2p::peer::get_peer_metrics()
{
    m_pmetrics->write_write_queue_size = write_queue.size();
	m_pmetrics->read_read_queue_size = read_queue.size();
	m_pmetrics->write_queue_buffer_size = surplus_size;
    return m_pmetrics;
}

bool mcp::p2p::peer::operator>(peer const & _p) const
{
	return *m_io > *_p.m_io;
}

void peer::read_loop()
{
    if (is_dropped)
        return;

    auto this_l(shared_from_this());
	read_header_buffer.resize(h256::size);
	ba::async_read(*socket, boost::asio::buffer(read_header_buffer, read_header_buffer.size()), [this, this_l](boost::system::error_code ec, std::size_t size)
    {
        if (is_dropped)
            return;

		if (!checkRead(h256::size, ec, size))
			return;

		if (!m_io->authAndDecryptHeader(bytesRef(read_header_buffer.data(), size)))
		{
			LOG(m_log.debug) << "Header decrypt failed";
			drop(disconnect_reason::bad_protocol);
			return;
		}
		uint16_t hProtocolId;
		uint32_t hLength;
		uint8_t hPadding;
		try
		{
			RLPXFrameInfo header(bytesConstRef(read_header_buffer.data(), size));
			hProtocolId = header.protocolId;
			hLength = header.length;
			hPadding = header.padding;
		}
		catch (std::exception const& _e)
		{
			LOG(m_log.debug) << "Exception decoding frame header RLP: " << _e.what() << " "
				<< bytesConstRef(read_header_buffer.data(), h128::size).cropped(3);
			drop(disconnect_reason::bad_protocol);
			return;
		}
		/// read padded frame and mac
		auto packet_size = hLength + hPadding + h128::size;
		read_buffer.resize(packet_size);
        ba::async_read(*socket, boost::asio::buffer(read_buffer, packet_size), [this, this_l, packet_size, hLength](boost::system::error_code ec, std::size_t size)
        {
            if (is_dropped)
                return;
			if (!checkRead(packet_size, ec, size))
				return;

			if (!m_io->authAndDecryptFrame(bytesRef(read_buffer.data(), packet_size)))
			{
				drop(disconnect_reason::bad_protocol);
				return;
			}
			bytesConstRef frame(read_buffer.data(), hLength);
			bool is_do_read(false);
			{
				std::lock_guard<std::mutex> lock(read_queue_mutex); 
				read_queue.push_back(frame.toBytes());
				is_do_read = read_queue.size() == 1;
			}
			if (is_do_read)
			{
				m_io_service.post([this, this_l]() {
					do_read();
				});
			}
			read_loop();
        });
    });
}

void peer::do_read()
{
	if (is_dropped)
		return;

	if (!socket->is_open())
		return;

	dev::bytes buffer;
	try
	{
		{
			std::lock_guard<std::mutex> lock(read_queue_mutex);
			if (read_queue.empty())
				return;

			buffer.resize(read_queue[0].size());
			buffer = std::move(read_queue[0]);
		}

		if (buffer.size() < mcp::p2p::tcp_header_size)
		{
			LOG(m_log.debug) << boost::str(boost::format("buffer size mismatch %1%, min size %2%") % buffer.size() % mcp::p2p::tcp_header_size);
			drop(disconnect_reason::bad_protocol);
			return;
		}
		
		uint32_t original_buffer_size(
			m_io->deserializePacketSize(
				dev::bytesConstRef(buffer.data(), mcp::p2p::tcp_header_size).toBytes()
			)
		);

		if (original_buffer_size > mcp::p2p::max_tcp_packet_size)
		{
			LOG(m_log.debug) << boost::str(boost::format("Too large body size %1%, max message body size %2%") % original_buffer_size % mcp::p2p::max_tcp_packet_size);
			drop(disconnect_reason::bad_protocol);
			return;
		}
		dev::bytes package_buffer(original_buffer_size);
		const int decompressed_size = LZ4_decompress_safe(
			(const char*)buffer.data() + mcp::p2p::tcp_header_size,
			(char*)package_buffer.data(), 
			buffer.size() - mcp::p2p::tcp_header_size,
			original_buffer_size
		);

		if (decompressed_size != original_buffer_size)
		{
			LOG(m_log.debug) << boost::str(boost::format("Lz4 decompression size mismatch %1%, decompressed size %2%") % original_buffer_size % decompressed_size);
			drop(disconnect_reason::bad_protocol);
			return;
		}

		uint32_t offset = 0;
		while (offset < original_buffer_size)
		{
			if (original_buffer_size < offset + 4)
			{
				LOG(m_log.debug) << boost::str(boost::format("Peer send message header lenth error,buffer size %1%, offset %2%") % original_buffer_size % offset);
				drop(disconnect_reason::bad_protocol);
				return;
			}
			uint32_t isize(
				m_io->deserializePacketSize(
					dev::bytesConstRef(package_buffer.data() + offset, 4).toBytes()
				)
			);

			if (!isize || isize > mcp::p2p::max_tcp_packet_size ||
				isize > original_buffer_size - offset - 4)
			{
				LOG(m_log.debug) << boost::str(boost::format("Peer send message lenth mismatch %1%, max message body size %2%") % isize % (package_buffer.size() - offset - 4));
				drop(disconnect_reason::bad_protocol);
				return;
			}

			dev::bytesConstRef packet(package_buffer.data() + offset + 4, isize);
			offset = offset + 4 + isize;
			if (!check_packet(packet))
			{
                LOG(m_log.debug) << boost::str(boost::format("invalid packet, size: %1%, packet: %2%") % packet.size() % toHex(packet));
				disconnect(disconnect_reason::bad_protocol);
				return;
			}
			else
			{
				auto packet_type = dev::RLP(packet.cropped(0, 1)).toInt<unsigned>();
				std::shared_ptr<dev::RLP> r(std::make_shared<dev::RLP>(packet.cropped(1)));
				bool ok = read_packet(packet_type, r);
				if (!ok)
                    LOG(m_log.debug) << "invalid rlp packet:" << *r;
			}
		}

		{
			std::lock_guard<std::mutex> lock(read_queue_mutex);
			read_queue.pop_front();

			if (read_queue.empty())
				return;
		}

		auto this_l(shared_from_this());
		m_io_service.post([this, this_l]() {
			do_read();
		});
	}
	catch (std::exception const & ex)
	{
		if (buffer.size() > 0)
		{
            LOG(m_log.warning) << "Error while peer convert data to RLP, buffer size:" << buffer.size() << ", buffer:" << dev::toHex(bytesConstRef(&buffer)) << ", message:" << ex.what();
		}
		else
		{
            LOG(m_log.warning) << "Error while peer convert data to RLP, buffer size:" << buffer.size() << ", message:" << ex.what();
		}
		disconnect(disconnect_reason::bad_protocol);
	}
}

bool peer::checkRead(std::size_t _expected, boost::system::error_code _ec, std::size_t _length)
{
	if (_ec && _ec.category() != boost::asio::error::get_misc_category() && _ec.value() != boost::asio::error::eof)
	{
        LOG(m_log.debug) << "Error reading: " << _ec.message();
		drop(disconnect_reason::tcp_error);
		return false;
	}
	else if (_ec && _length < _expected)
	{
        LOG(m_log.debug) << "Error reading - Abrupt peer disconnect: " << _ec.message();
		drop(disconnect_reason::tcp_error);
		return false;
	}
	else if (_length != _expected)
	{
		// with static m_data-sized buffer this shouldn't happen unless there's a regression
		// sec recommends checking anyways (instead of assert)
        LOG(m_log.debug) << "Error reading - TCP read buffer length differs from expected frame size.";
		disconnect(disconnect_reason::tcp_error);
		return false;
	}

	return true;
}

bool peer::check_packet(dev::bytesConstRef msg)
{
    if (msg[0] > 0x7f || msg.size() < 2)
        return false;
    if (dev::RLP(msg.cropped(1)).actualSize() + 1 != msg.size())
        return false;
    return true;
}

bool peer::read_packet(unsigned const & type, std::shared_ptr<dev::RLP> r)
{
    _last_received = std::chrono::steady_clock::now();

    try
    {
        if (type < (unsigned)packet_type::user_packet)
        {
            switch ((packet_type)type)
            {
            case packet_type::ping:
            {
                dev::RLPStream s;
                send(prep(s, (unsigned)packet_type::pong));
                break;
            }
            case packet_type::pong:
            {
                break;
            }
            case packet_type::disconect:
            {
                auto reason = (disconnect_reason)(*r)[0].toInt<unsigned>();
                if (!(*r)[0].isInt())
                    drop(disconnect_reason::bad_protocol);
                else
                {
                    std::string reason_str = reason_of(reason);
                    LOG(m_log.info) << "Disconnect (reason: " << reason_str << ")";
                    drop(disconnect_reason::disconnect_requested);
                }
                break;
            }
            default:
                return false;
            }

            return true;
        }

        auto this_l(shared_from_this());
        for (auto & p_cap : capabilities)
        {
            if (type >= p_cap->offset && type < p_cap->offset + p_cap->cap->packet_count())
                return p_cap->cap->read_packet(this_l, type - p_cap->offset, r);
        }

        return false;
    }
    catch (std::exception const & e)
    {
        LOG(m_log.warning) << boost::str(boost::format("Error while reading packet, packet type: %1% , rlp: %2%, message: %3%") % (unsigned)type % *r %e.what());
        disconnect(disconnect_reason::bad_protocol);
        return true;
    }
    return true;
}

dev::RLPStream & peer::prep(dev::RLPStream & s, unsigned const & type, unsigned const & size)
{
    return s.append((unsigned)type).appendList(size);
}

void peer::send(dev::RLPStream & s)
{
    if (is_dropped)
        return;

	if (!socket->is_open())
	{
		boost::system::error_code ec;
        LOG(m_log.debug) << "remote socket is closed: " << m_node_id.hex()
			<< "@" << socket->remote_endpoint(ec);

		return;
	}
        
	if (surplus_size > SEND_BUFFER_LIMIT)
	{
		if (!bprintf)
		{
			boost::system::error_code ec;
            LOG(m_log.debug) << "Peer write too slow " << m_node_id.hex()
				<< "@" << socket->remote_endpoint(ec) << " , surplus_size: " << surplus_size;

			bprintf = true;
		}

		//drop async for db transaction issue
		auto this_l(shared_from_this());
		m_io_service.post([this, this_l]() {
			drop(mcp::p2p::disconnect_reason::tcp_error);
		});

		return;
	}

    dev::bytes b;
    s.swapOut(b);
    dev::bytesConstRef packet(&b);
    if (!check_packet(packet))
    {
        LOG(m_log.warning) << "Invalid send packet:" << dev::toHex(packet);
    }

    size_t packet_size(b.size());
    if (packet_size > mcp::p2p::max_tcp_packet_size)
    {
        LOG(m_log.error) << "Peer send: packet size too large, size:" << packet_size
            << ", max packet size:" << mcp::p2p::max_tcp_packet_size;
        throw std::runtime_error("Size too large");
    }

	m_io->writeFramePacketHeader(b);
	bool is_do_write(false);
	{
		std::lock_guard<std::mutex> lock(write_queue_mutex);
		surplus_size += b.size();
		write_queue.push_back(std::move(b));
		is_do_write = write_queue.size() == 1;
	}

	if (is_do_write)
		do_write();
}

void peer::do_write()
{
	if (is_dropped)
		return;

	uint32_t group_buffer_size = 0;
	uint32_t group_item_count = 1;
	{
		std::lock_guard<std::mutex> lock(write_queue_mutex);
		size_t write_queue_size = write_queue.size();
		if (write_queue_size == 0)
		{
            LOG(m_log.info) << "do_write write_queue_size :" << write_queue_size;
			return;
		}

		for (uint32_t i = 0; i < write_queue_size; i++)
		{
			if (i == 0)
			{
				group_buffer_size = write_queue[i].size();
				continue;
			}

			if (group_buffer_size + write_queue[i].size() > GROUP_BUFFER_SIZE_LIMIT)
				break;

			group_buffer_size += write_queue[i].size();
			group_item_count++;
		}

		write_bufs.resize(group_buffer_size);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < group_item_count; i++)
		{
			dev::bytesConstRef(write_queue[i].data(), write_queue[i].size()).copyTo(dev::bytesRef(write_bufs.data() + offset, write_queue[i].size()));
			offset += write_queue[i].size();
		}
	}	

	dev::bytes ori_size(m_io->serializePacketSize(write_bufs.size()));	//used for Decompression
	lz4(write_bufs);

	write_bufs.resize(mcp::p2p::tcp_header_size + write_bufs.size());
	dev::bytesConstRef(write_bufs.data(), write_bufs.size()).copyTo(dev::bytesRef(write_bufs.data() + mcp::p2p::tcp_header_size, write_bufs.size()));
	dev::bytesConstRef(ori_size.data(), mcp::p2p::tcp_header_size).copyTo(dev::bytesRef(write_bufs.data(), mcp::p2p::tcp_header_size));

	//encry
	m_io->writeSingleFramePacket(dev::bytesConstRef(write_bufs.data(), write_bufs.size()), write_bufs);
	if (!socket->is_open())
		return;

	auto this_l(shared_from_this());
	ba::async_write(*socket, ba::buffer(write_bufs),
		[this, this_l, group_buffer_size, group_item_count](boost::system::error_code ec, std::size_t size) {

		if (is_dropped)
			return;

		if (ec)
		{
            LOG(m_log.warning) << "Error while peer sending, message:" << ec.message() << "@" << socket->remote_endpoint(ec);
			drop(disconnect_reason::tcp_error);
			return;
		}

		m_pmetrics->send_size += group_buffer_size;
		m_pmetrics->send_count += group_item_count;

		{
			std::lock_guard<std::mutex> lock(write_queue_mutex);
			surplus_size -= group_buffer_size;

			for (uint32_t i = 0; i < group_item_count; i++)
			{
				if (!write_queue.empty())
					write_queue.pop_front();
			}

			if (write_queue.empty())
				return;
		}

		do_write();
	});
}

void peer::lz4(bytes& o_bytes)
{
	const int max_dst_size = LZ4_compressBound(o_bytes.size() + 1);

	dev::bytes compressed_bufs(max_dst_size);
	const int compressed_data_size = LZ4_compress_default((const char*)write_bufs.data(), (char *)compressed_bufs.data(), o_bytes.size(), max_dst_size);
	compressed_bufs.resize(compressed_data_size);
	o_bytes = std::move(compressed_bufs);
}

void peer::drop(disconnect_reason const & reason, bool record)
{
	bool st = false;
    if (!is_dropped.compare_exchange_strong(st, true))
        return;

	auto this_l(shared_from_this());
	for (auto pc : capabilities)
		pc->cap->on_disconnect(this_l);

    boost::system::error_code ec;
    LOG(m_log.info) << "Peer dropped reason of " << reason_of(reason) << " ,id:" << m_node_id.hex() << "@" << socket->remote_endpoint(ec);

	if (record)
		m_peer_manager->record_connect(remote_node_id(), reason);
    if (socket->is_open())
    {
        try
        {
            //boost::system::error_code ec;
            //LOG(m_log.info) << "Closing " << socket->remote_endpoint(ec) << " (" << reason_of(reason) << ")";
            socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            socket->close();
        }
        catch (...) {}
    }
}
