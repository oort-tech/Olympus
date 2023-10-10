#include "host.hpp"
#include<boost/algorithm/string.hpp>

using namespace mcp::p2p;

host::host(bool & error_a, p2p_config const & config_a, boost::asio::io_service & io_service_a, dev::Secret const & node_key,
	boost::filesystem::path const & application_path_a) :
	config(config_a),
	io_service(io_service_a),
	alias(node_key),
	acceptor(std::make_unique<bi::tcp::acceptor>(io_service_a)),
	is_run(false),
	last_ping(std::chrono::steady_clock::time_point::min()),
	last_try_connect(std::chrono::steady_clock::time_point::min()),
	last_try_connect_exemption(std::chrono::steady_clock::time_point::min()),
	m_peer_manager(std::make_shared<peer_manager>(error_a, application_path_a))
{
	if (error_a)
		return;

	for (std::string const & bn : config.bootstrap_nodes)
	{
		if (!boost::istarts_with(bn, "mcpnode://"))
		{
			LOG(m_log.warning) << "Invald boostrap node: " << bn;
			continue;
		}

		try {
			std::string node_str(bn.substr(10));

			std::vector<std::string> node_id_and_addr;
			boost::split(node_id_and_addr, node_str, boost::is_any_of("@"));
			if (node_id_and_addr.size() != 2)
			{
				LOG(m_log.warning) << "Invald boostrap node :" << bn;
				continue;
			}

			node_id nid(node_id_and_addr[0]);
			std::string addr(node_id_and_addr[1]);
			bi::tcp::endpoint ep;
			if (resolve_host(addr, ep) || nid == node_id())
			{
				LOG(m_log.warning) << "Invald boostrap node :" << bn;
				continue;
			}

			node_endpoint node_ep(ep.address(), ep.port(), ep.port());
			bootstrap_nodes.push_back(std::make_shared<node_info>(nid, node_ep));
		}
		catch (...) {
			LOG(m_log.warning) << "Invald boostrap node :" << bn;
			continue;
		}
	}

	for (std::string const & bn : config.exemption_nodes)
	{
		if (!boost::istarts_with(bn, "mcpnode://"))
		{
			LOG(m_log.warning) << "Invald exemption node: " << bn;
			continue;
		}

		try {
			std::string node_str(bn.substr(10));

			std::vector<std::string> node_id_and_addr;
			boost::split(node_id_and_addr, node_str, boost::is_any_of("@"));
			if (node_id_and_addr.size() != 2)
				LOG(m_log.warning) << "Invald exemption node :" << bn;

			node_id node_id(node_id_and_addr[0]);

			std::string addr(node_id_and_addr[1]);
			bi::tcp::endpoint ep;
			if (resolve_host(addr, ep))
			{
				LOG(m_log.warning) << "Invald exemption node :" << bn;
				continue;
			}

			node_endpoint node_ep(ep.address(), ep.port(), ep.port());
			exemption_nodes.push_back(std::make_shared<node_info>(node_id, node_ep, PeerType::Required));
		}
		catch (...) {
			LOG(m_log.warning) << "Invald exemption node :" << bn;
			continue;
		}
	}

}

void host::start()
{
	start_time = std::chrono::steady_clock::now();

	is_run = true;

	bi::address listen_ip;
	try
	{
		listen_ip = config.listen_ip.empty() ? bi::address_v4() : bi::address::from_string(config.listen_ip);
	}
	catch (std::exception const & e)
	{
        LOG(m_log.error) << "Invalid listen_ip:" << listen_ip << ", message:" << e.what();
		return;
	}

	uint16_t port(config.port);
	start_listen(listen_ip, port);
	map_public(listen_ip, port);
	accept_loop();

	m_node_table = std::make_shared<node_table>(m_peer_manager->store, alias, node_endpoint(listen_ip, port, port));
	m_node_table->set_event_handler(new host_node_table_event_handler(*this));
	m_node_table->start();

    LOG(m_log.info) << "P2P started, mcpnode://" << id().hex() << "@" << listen_ip << ":" << port;

	run_timer = std::make_unique<ba::deadline_timer>(io_service);
	run();
}

void host::stop()
{
	is_run = false;
	LOG(m_log.info) << "Host stopped";

	if (acceptor->is_open())
	{
		acceptor->cancel();
		acceptor->close();
	}

	if (run_timer)
		run_timer->cancel();

	// disconnect peers
	for (unsigned n = 0;; n = 0)
	{
		std::lock_guard<std::mutex> lock(m_peers_mutex);
		for (auto i : m_peers)
			if (auto p = i.second.lock())
				if (p->is_connected())
				{
					p->disconnect(disconnect_reason::client_quit);
					n++;
				}
		if (!n)
			break;
	}

	//clear peers
	{
		std::lock_guard<std::mutex> lock(m_peers_mutex);
		m_peers.clear();
	}
}

void mcp::p2p::host::register_capability(std::shared_ptr<icapability> cap)
{
	capabilities.insert(std::make_pair(cap->desc, cap));
}

void host::start_listen(bi::address const & listen_ip, uint16_t const & port)
{
	try
	{
		bi::tcp::endpoint endpoint(listen_ip, port);
		acceptor->open(endpoint.protocol());
		acceptor->set_option(bi::tcp::acceptor::reuse_address(true));
		acceptor->bind(endpoint);
		acceptor->listen();

        LOG(m_log.info) << boost::str(boost::format("P2P start listen on %1%:%2%") % listen_ip % port);
	}
	catch (std::exception const & e)
	{
        LOG(m_log.error) << boost::str(boost::format("Error while acceptor listen on %1%:%2%, message: %3%")
			% listen_ip % port % e.what());
		throw;
	}

}

void host::accept_loop()
{
	if (!is_run)
		return;

	auto socket(std::make_shared<bi::tcp::socket>(io_service));
	auto this_l(shared_from_this());
	acceptor->async_accept(*socket, [socket, this_l, this](boost::system::error_code const & ec) 
	{
		if (ec || !this_l->is_run)
		{
			//LOG(this_l->m_log.debug) << boost::str(boost::format("Error while accepting connections: %1%") % ec.message());
			try
			{
				if (socket->is_open())
					socket->close();
			}
			catch (...) {}
		}
		else
		{
			boost::system::error_code e;
			auto remote_ep = socket->remote_endpoint(e);
			bool exemption_node_flag = false;
			for (auto it = exemption_nodes.begin(); it != exemption_nodes.end(); it++)
			{
				auto info = *it;
				if (info->endpoint.address == remote_ep.address())
				{
					exemption_node_flag = true;
					break;
				}
			}

			if (this_l->avaliable_peer_count(peer_type::ingress) == 0 && !exemption_node_flag)
			{
                LOG(this_l->m_log.debug) << "Dropping socket due to too many peers, peer count: " << this_l->m_peers.size()
					<< ",pending peers: " << this_l->m_connecting.size() << ",remote endpoint: " << remote_ep
					<< ",max peers: " << this_l->max_peer_size(peer_type::ingress);
				try
				{
					if (socket->is_open())
						socket->close();
				}
				catch (...) {}

			}
			else
			{
				// incoming connection; we don't yet know nodeid
				auto handshake = std::make_shared<hankshake>(this_l, socket);
				m_connecting.push_back(handshake);
				handshake->start();
			}

			this_l->accept_loop();
		}
	});
}

void host::run()
{
	if (!is_run)
		return;

	m_node_table->process_events();

	// cleanup zombies
	DEV_GUARDED(x_connecting)
		m_connecting.remove_if([](std::weak_ptr<hankshake> h) { return h.expired(); });

	keep_alive_peers();

	try_connect_nodes();

	run_timer->expires_from_now(run_interval);
	run_timer->async_wait([this](boost::system::error_code const & error)
	{
		run();
	});
}

bool host::is_handshaking(node_id const& _id) const
{
	for (auto const& cIter : m_connecting)
	{
		std::shared_ptr<hankshake> const connecting = cIter.lock();
		if (connecting && connecting->remote() == _id)
			return true;
	}
	return false;
}

bool mcp::p2p::host::have_peer(node_id const & _id) const
{
	std::lock_guard<std::mutex> lock(m_peers_mutex);
	return m_peers.count(_id);
}


bool host::resolve_host(std::string const & addr, bi::tcp::endpoint & ep)
{
	bool error(false);
	std::vector<std::string> split;
	boost::split(split, addr, boost::is_any_of(":"));
	unsigned port = mcp::p2p::default_port;

	std::string host(split[0]);
	std::string port_str(split[1]);

	try
	{
		if (split.size() > 1)
			port = static_cast<uint16_t>(stoi(port_str));
	}
	catch (...) {}

	boost::system::error_code ec;
	bi::address address = bi::address::from_string(host, ec);
	if (!ec)
	{
		ep.address(address);
		ep.port(port);
	}
	else
	{
		boost::system::error_code ec;
		// resolve returns an iterator (host can resolve to multiple addresses)
		bi::tcp::resolver r(io_service);
		auto it = r.resolve(bi::tcp::resolver::query(host, std::to_string(port)), ec);
		if (ec)
		{
            LOG(m_log.info) << "Error resolving host address... " << addr << " : " << ec.message();
			error = true;
		}
		else
			ep = *it;
	}
	return error;
}

void host::connect(std::shared_ptr<node_info> const & ne)
{
	if (!is_run)
		return;
	if (ne->id == id())//self
		return;

	if (have_peer(ne->id))
	{
		//LOG(m_log.debug) << "Aborted connect, node already connected, node id: " << ne->id.hex();
		return;
	}

	bi::tcp::endpoint ep(ne->endpoint);
	std::shared_ptr<bi::tcp::socket> socket = std::make_shared<bi::tcp::socket>(io_service);
	auto handshake = std::make_shared<hankshake>(shared_from_this(), socket, ne->id);
	{
		Guard l(x_connecting);
		if (is_handshaking(ne->id))
		{
			//LOG(m_log.debug) << "Aborted connection. handshake with peer already in progress: " << ne->id.hex();
			return;
		}
		m_connecting.push_back(handshake);
	}
	socket->async_connect(ep, [ne, ep, handshake, this](boost::system::error_code const& ec)
	{
		if (ec)
		{
			//LOG(m_log.debug) << "Connection refused to node " << ne->id.hex() << "@" << ep << ", message: " << ec.message();
			//m_peer_manager->record_connect(ne->id, disconnect_reason::tcp_error);
		}
		else
		{
			//LOG(m_log.debug) << "Connecting to " << ne->id.hex() << "@" << ep;
			handshake->start();
		}
	});
}

size_t host::avaliable_peer_count(peer_type const & type, bool b)
{
	size_t count = 0;
	{
		if (b)
		{
			std::lock_guard<std::mutex> lock(m_peers_mutex);
			for (auto& i : m_peers)
				if (auto p = i.second.lock())
					if (p->is_connected())
						count++;
		}
		else
		{
			for (auto& i : m_peers)
				if (auto p = i.second.lock())
					if (p->is_connected())
						count++;
		}
	}

	if (max_peer_size(type) <= count)
		return 0;
	return max_peer_size(type) - count;
}

uint32_t host::max_peer_size(peer_type const & type)
{
	if (type == peer_type::egress)
		return config.max_peers / 2 + 1;
	else
		return config.max_peers;
}

void host::keep_alive_peers()
{
	if (std::chrono::steady_clock::now() - keep_alive_interval < last_ping)
		return;

	{
		std::lock_guard<std::mutex> lock(m_peers_mutex);
		for (auto it = m_peers.begin(); it != m_peers.end();)
			if (auto p = it->second.lock())
			{
				p->ping();
				++it;
			}
			else
				it = m_peers.erase(it);
	}

	last_ping = std::chrono::steady_clock::now();
}

void host::try_connect_nodes()
{
	if (std::chrono::steady_clock::now() - try_connect_interval < last_try_connect)
		return;

	size_t avaliable_count = avaliable_peer_count(peer_type::egress);

	if (std::chrono::steady_clock::now() - try_connect_interval_exemption > last_try_connect_exemption)
	{
		for (auto it = exemption_nodes.begin(); it != exemption_nodes.end(); it++)
		{
			auto info = *it;
			if (!m_peers.count(info->id) && m_peer_manager->should_reconnect(info->id, info->peer_type))
			{
				connect(info);
				m_node_table->add_node(*info, true);/// mark as known peer
			}
		}

		//connect to bootstrap nodes
		if (bootstrap_nodes.size() > 0 && avaliable_count > 0)
		{
			//only random pick one
			auto rindex(mcp::random_pool.GenerateWord32(0, bootstrap_nodes.size() - 1));
			auto info = bootstrap_nodes[rindex];
			if (!m_peers.count(info->id) && m_peer_manager->should_reconnect(info->id, info->peer_type))
			{
				connect(info);
				m_node_table->add_node(*info, true);/// mark as known peer
			}
		}

		last_try_connect_exemption = std::chrono::steady_clock::now();
	}

	
	if (avaliable_count > 0)
	{
		//random find node in node table
		auto node_infos(m_node_table->get_random_nodes(avaliable_count));
		for (auto nf : node_infos)
		{
			if (!m_peers.count(nf->id) && m_peer_manager->should_reconnect(nf->id, nf->peer_type))
			{
				connect(nf);
			}
		}
	}

	last_try_connect = std::chrono::steady_clock::now();
}

// called after successful handshake
void host::start_peer(mcp::p2p::node_id const& _id, dev::RLP const& _rlp, std::unique_ptr<mcp::p2p::RLPXFrameCoder>&& _io, std::shared_ptr<bi::tcp::socket> const & socket)
{
	if (!is_run)
		return;

	hankshake_msg handmsg(_rlp);
	node_id remote_node_id(_id);
	
	try
	{
		node_info _node_info = node_info_from_node_table(remote_node_id);
		if ( !(m_peer_manager->should_reconnect(remote_node_id, _node_info.peer_type)) )
		{
            boost::system::error_code e;
            LOG(m_log.debug) << "peer: " <<socket->remote_endpoint(e) <<",node id:"<< remote_node_id.hex() << " is bad peer.";
			try
			{
				if (socket->is_open())
					socket->close();
			}
			catch (...) {}

			return;
		}
		
		{
			std::lock_guard<std::mutex> lock(m_peers_mutex);
			std::shared_ptr<peer> new_peer(std::make_shared<peer>(socket, remote_node_id, m_peer_manager, move(_io), io_service));
			//check self connect
			if (remote_node_id == id())
			{
				new_peer->disconnect(disconnect_reason::self_connect);
				return;
			}
			if (handmsg.network != mcp::mcp_network)
			{
				LOG(m_log.info) << "p2p mcp_network error,remote mcp_network: " << (int)handmsg.network << " local mcp_network " << (int)mcp::mcp_network;
				if (socket->is_open())
					socket->close();
				new_peer->disconnect(disconnect_reason::network_error);
				return;
			}
			//check duplicate
			if (m_peers.count(remote_node_id) && !!m_peers[remote_node_id].lock())
			{
				auto exist_peer(m_peers[remote_node_id].lock());
				if (exist_peer->is_connected())
				{
					/// maybe A connect B and B connect A at the same time.
					/// A connect B, B as the receiver does not know the id of A, So it can't judge the repetition that it initiated.
					/// A and B are both the initiator and the receiver. have two connections. need to negotiate the use of one of the two connections.
					/// every connect have encrypted nonce. A and B see the same thing for each of these channels.
					/// A and B both choose the connection with the larger nonce and close the connection with the smaller nonce.
					if (std::chrono::steady_clock::now() < exist_peer->create_time() + std::chrono::seconds(2))
					{
						if (*exist_peer > *new_peer)
						{
							new_peer->drop(disconnect_reason::duplicate_peer, false);
							return;
						}
						else
						{
							exist_peer->drop(disconnect_reason::duplicate_peer, false);
							m_peers.erase(remote_node_id);
						}
					}
					else
					{
						boost::system::error_code ec;
						LOG(m_log.debug) << "Peer already exists, node id: " << remote_node_id.hex() << "@" << socket->remote_endpoint(ec);
						new_peer->disconnect(disconnect_reason::duplicate_peer);
						return;
					}
				}
			}
			//check max peers

			boost::system::error_code e;
			auto remote_ep = socket->remote_endpoint(e);
			bool exemption_node_flag = false;
			for (auto it = exemption_nodes.begin(); it != exemption_nodes.end(); it++)
			{
				auto info = *it;
				if (info->endpoint.address == remote_ep.address())
				{
					exemption_node_flag = true;
					break;
				}
			}

			if(avaliable_peer_count(peer_type::ingress,false) == 0 && _node_info.peer_type != PeerType::Required && !exemption_node_flag)
			{
				boost::system::error_code ec;
                LOG(m_log.debug) << "Too many peers. peer count: " << m_peers.size() << ",pending peers: " << m_connecting.size()
					<< ",remote node id: " << remote_node_id.hex() << ",remote endpoint: " << socket->remote_endpoint(ec) 
					<< ",max peers: " << max_peer_size(peer_type::ingress);

				new_peer->disconnect(disconnect_reason::too_many_peers);
				return;
			}

			//get peer capabilities
			unsigned offset = (unsigned)packet_type::user_packet;
			std::map<capability_desc, std::shared_ptr<peer_capability>> p_caps;
			for (auto const & pair : capabilities)
			{
				capability_desc const & desc(pair.first);
				if (std::find(handmsg.cap_descs.begin(), handmsg.cap_descs.end(), desc) != handmsg.cap_descs.end())
				{
					auto it(p_caps.find(desc));
					if (it != p_caps.end())
					{
						offset -= it->second->cap->packet_count();
					}

					auto const & cap(pair.second);
					p_caps[desc] = std::make_shared<peer_capability>(offset, cap);
					offset += cap->packet_count();
				}
			}
			if (p_caps.size() == 0)
			{
				new_peer->disconnect(disconnect_reason::useless_peer);
				return;
			}

			for (auto const & p_cap : p_caps)
				new_peer->register_capability(p_cap.second);

			new_peer->start();

			m_peers[remote_node_id] = new_peer;
		}
	}
	catch (std::exception const & e)
	{
		boost::system::error_code ec;
        LOG(m_log.warning) << boost::str(boost::format("Error while starting Peer %1% : %2%, message: %3%")
			% remote_node_id.hex() % socket->remote_endpoint(ec) % e.what());
		try
		{
			if (socket->is_open())
				socket->close();
		}
		catch (...) {}
	}
}

void host::on_node_table_event(node_id const & node_id_a, node_table_event_type const & type_a)
{
	if (type_a == node_table_event_type::node_entry_added)
	{
		//LOG(m_log.info) << "Node entry added, id:" << node_id_a.hex();

		if (std::shared_ptr<node_info> nf = m_node_table->get_node(node_id_a))
		{
			if (!m_peers.count(nf->id) && 
				(avaliable_peer_count(peer_type::egress) > 0 || nf->peer_type == PeerType::Required)
				)
				connect(nf);
		}
	}
	else if (type_a == node_table_event_type::node_entry_dropped)
	{
		//LOG(m_log.info) << "Node entry dropped, id:" << node_id_a.to_string();
	}
}

std::unordered_map<node_id, bi::tcp::endpoint> mcp::p2p::host::peers() const
{
	std::unordered_map<node_id, bi::tcp::endpoint> result;
	std::lock_guard<std::mutex> lock(m_peers_mutex);
	for (auto p : m_peers)
	{
		if (auto peer = p.second.lock())
			result.insert(std::make_pair(peer->remote_node_id(), peer->remote_endpoint()));
	}

	return result;
}

std::list<node_info> mcp::p2p::host::nodes() const
{
	return m_node_table->nodes();
}

std::map<std::string, uint64_t> mcp::p2p::host::get_peers_write_queue_size()
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    std::map<std::string, uint64_t> map_write_queue_size;
    for (auto i : m_peers)
    {
        auto p = i.second.lock();
        if (p)
        {
            auto size_l = p->get_write_queue_size();
            std::string node_ip = p->remote_endpoint().address().to_string();
            map_write_queue_size[node_ip] = size_l;

        }
    }
    return map_write_queue_size;
}

std::map<std::string, std::shared_ptr<mcp::p2p::peer_metrics>> mcp::p2p::host::get_peers_metrics()
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    std::map<std::string, std::shared_ptr<mcp::p2p::peer_metrics>> map_peers_metrics;
    for (auto i : m_peers)
    {
        auto p = i.second.lock();
        if (p)
        {
            auto peer_metrics_l = p->get_peer_metrics();
            std::stringstream node_ip_strm;
            node_ip_strm << p->remote_endpoint();
            map_peers_metrics[node_ip_strm.str()] = peer_metrics_l;

        }
    }
    return map_peers_metrics;
}

node_info host::node_info_from_node_table(node_id const& node_id) const
{
	std::shared_ptr<node_info> p = m_node_table->get_node(node_id);
	if (p)
	{
		node_info nodeinfo(*p);
		return nodeinfo;
	}
	node_endpoint nep(bi::address(), 0, 0);
	node_info nodeinfo(node_id,nep);
	return nodeinfo;
}

void host::map_public(bi::address const & listen_ip, uint16_t port)
{
	auto ifAddresses = upnp::getInterfaceAddresses();
	auto lset = !listen_ip.is_unspecified();
	bool listenIsPublic = lset && isPublicAddress(listen_ip);

	if (config.nat && listenIsPublic)
	{
        LOG(m_log.info) << "Is already the public network address:" << listen_ip;
	}
	else if (config.nat)
	{
		try
		{
			up.reset(new upnp);
		}
		catch (...) {}

		if (up && up->isValid())
		{
			up->traverseNAT(lset && ifAddresses.count(listen_ip) ? std::set<bi::address>({ listen_ip }) : ifAddresses, port);
		}
	}
}

void host::replace_bootstrap(node_id const& old_a, node_id new_a)
{
	for (auto bn : bootstrap_nodes)
	{
		if (bn->id == old_a)
			bn->id = new_a;
	}
	for (auto bn : exemption_nodes)
	{
		if (bn->id == old_a)
			bn->id = new_a;
	}
}

void host::onHandshakeFailed(node_id const& _n, HandshakeFailureReason _r)
{
	m_peer_manager->onHandshakeFailed(_n, _r);
}


