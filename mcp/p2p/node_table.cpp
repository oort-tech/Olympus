#include "node_table.hpp"

using namespace mcp::p2p;

node_table::node_table(mcp::p2p::peer_store& store_a, mcp::key_pair const & alias_a, node_endpoint const & endpoint_a) :
	m_store(store_a),
	my_node_info((node_id) alias_a.pub_comp(), endpoint_a),
	secret(alias_a.secret()),
	socket(std::make_unique<bi::udp::socket>(io_service)),
	my_endpoint(endpoint_a),
	is_cancel(false)
{
	for (unsigned i = 0; i < s_bins; i++)
		states[i].distance = i;
}

node_table::~node_table()
{
	is_cancel = true;
	if (discover_timer)
		discover_timer->cancel();
	if (write_info_timer)
		write_info_timer->cancel();
	socket->close();
	io_service.stop();
	if(io_service_thread.joinable())
		io_service_thread.join();
}

void node_table::start()
{
	bi::udp::endpoint udp_endpoint(my_endpoint);
	socket->open(bi::udp::v4());
	try
	{
		try
		{
			socket->bind(udp_endpoint);
            LOG(m_log.info) << "Node discovery start on " << udp_endpoint;
		}
		catch (...)
		{
			auto uep(bi::udp::endpoint(bi::udp::v4(), udp_endpoint.port()));
			socket->bind(uep);
            LOG(m_log.error) << "Node discovery bind fail on " << udp_endpoint << " and start on " << uep;
		}

		mcp::db::db_transaction transaction(m_store.create_transaction());
		auto it = m_store.node_begin(transaction);
		while (it.valid())
		{
			mcp::p2p::node_id id(it.value().data());

			dev::bytes result(it.key().size());
			std::copy((byte *)it.key().data(), (byte *)it.key().data() + sizeof(result), result.data());
			dev::RLP r(result);
			if (r.itemCount() == 3)
			{
				mcp::p2p::node_endpoint ep(r);
				mcp::p2p::node_info n(id, ep);
				add_node(n);
			}
			++it;
		}

		receive_loop();
		discover_loop();
		process_write_node_info();
	}
	catch (std::exception const & e)
	{
        LOG(m_log.error) << "Node discovery start fail: " << e.what() << ", endpoint:" << udp_endpoint;
	}
	
	io_service_thread = std::thread([this]()
	{  
		if(!is_cancel)
			io_service.run(); 
	});
}

void node_table::discover_loop()
{
	if (is_cancel)
		return;

	discover_timer = std::make_unique<ba::deadline_timer>(io_service);
	discover_timer->expires_from_now(discover_interval);
	auto this_l(shared_from_this());
	discover_timer->async_wait([this](boost::system::error_code const & ec)
	{
		if (ec.value() == boost::asio::error::operation_aborted)
			return;

		if (ec)
		{
            LOG(m_log.warning) << "Discover loop error: " << ec.value() << ":" << ec.message();
		}

		node_id rand_node_id;
		mcp::random_pool.GenerateBlock(rand_node_id.data(), rand_node_id.size);
		do_discover(rand_node_id);
	});
}

void node_table::do_discover(node_id const & rand_node_id, unsigned const & round, std::shared_ptr<std::set<std::shared_ptr<node_entry>>> tried_a)
{
	if (is_cancel)
		return;

	if (round == max_discover_rounds)
	{
		discover_loop();
		return;
	}
	else if (!round && !tried_a)
		// initialized tried on first round
		tried_a = std::make_shared<std::set<std::shared_ptr<node_entry>>>();

	std::vector<std::shared_ptr<node_entry>> nearest = nearest_node_entries(rand_node_id);
	std::list<std::shared_ptr<node_entry>> tried;
	for (unsigned i = 0; i < nearest.size() && tried.size() < s_alpha; i++)
	{
		if (!tried_a->count(nearest[i]))
		{
			auto n = nearest[i];
			tried.push_back(n);

			//send find node
			find_node_packet p(my_node_info.id, rand_node_id);

			//add find node timeout
			{
				std::lock_guard<std::mutex> lock(find_node_timeouts_mutex);
				find_node_timeouts[n->id] = std::chrono::steady_clock::now();
			}

			send((bi::udp::endpoint)n->endpoint, p);
		}
	}

	if (tried.empty())
	{
		discover_loop();
		return;
	}

	while (!tried.empty())
	{
		tried_a->insert(tried.front());
		tried.pop_front();
	}

	discover_timer = std::make_unique<ba::deadline_timer>(io_service);
	discover_timer->expires_from_now(boost::posix_time::milliseconds(req_timeout.count() * 2));
	discover_timer->async_wait([this, rand_node_id, round, tried_a](boost::system::error_code const& ec)
	{
		if (ec.value() == boost::asio::error::operation_aborted)
			return;

		if (ec)
		{
            LOG(m_log.warning) << "Do discover error: " << ec.value() << ":" << ec.message();
		}

		do_discover(rand_node_id, round + 1, tried_a);
	});
}

void node_table::receive_loop()
{
	if (is_cancel)
		return;

	auto this_l(shared_from_this());
	socket->async_receive_from(boost::asio::buffer(recv_buffer), recv_endpoint, [this, this_l](boost::system::error_code ec, size_t size)
	{
		if (is_cancel)
			return;

		if (ec)
		{
			//LOG(this_l->m_log.info) << "Receiving UDP message from " << recv_endpoint << " failed. " << ec.value() << " : " << ec.message();
		}

		if (size > 0)
			handle_receive(recv_endpoint, dev::bytesConstRef(recv_buffer.data(), size));
		receive_loop();
	});
}

void node_table::handle_receive(bi::udp::endpoint const & from, dev::bytesConstRef const & data)
{
	try {
		std::unique_ptr<discover_packet> packet = interpret_packet(from, data);
		//LOG(m_log.info) << "Receive packet, " << packet->source_id.to_string() << "@" << from << " ,type:" << (uint32_t)packet->packet_type();
		if (!packet)
			return;

		if (packet->is_expired())
		{
            LOG(m_log.debug) << "Invalid packet (timestamp in the past) from " << from.address().to_string() << ":" << from.port();
			return;
		}

		switch (packet->packet_type())
		{
		case  discover_packet_type::ping:
		{
			auto in = dynamic_cast<ping_packet const&>(*packet);
			node_endpoint from_node_endpoint(from.address(), from.port(), in.tcp_port);
			add_node(node_info(packet->source_id, from_node_endpoint));

			pong_packet p(my_node_info.id);
			send(from, p);
			break;
		}
		case discover_packet_type::pong:
		{
			auto in = dynamic_cast<pong_packet const &>(*packet);
			// whenever a pong is received, check if it's in evictions
			bool found = false;
			node_id evicted_node_id;
			eviction_entry eviction_entry;
			{
				std::lock_guard<std::mutex> lock(evictions_mutex);
				auto exists = evictions.find(packet->source_id);
				if (exists != evictions.end())
				{
					if (exists->second.evicted_time > std::chrono::steady_clock::now())
					{
						found = true;
						evicted_node_id = exists->first;
						eviction_entry = exists->second;
						evictions.erase(exists);
					}
				}
			}

			if (found)
			{
				if (auto n = get_node(eviction_entry.new_node_id))
					drop_node(n);
				if (auto n = get_node(evicted_node_id))
					n->pending = false;
			}
			else
			{
				// if not, check if it's known/pending or a pubk discovery ping
				if (auto n = get_node(packet->source_id))
					n->pending = false;
				else
					add_node(node_info(packet->source_id, node_endpoint(from.address(), from.port(), from.port())));
			}

			break;
		}
		case discover_packet_type::find_node:
		{
			auto in = dynamic_cast<find_node_packet const&>(*packet);
			std::shared_ptr<bi::udp::endpoint> _from = std::make_shared<bi::udp::endpoint>(from);
			std::vector<std::shared_ptr<node_entry>> nearest = nearest_node_entries(in.target, packet->source_id, _from);
			static unsigned const nlimit = (max_udp_packet_size - 130) / neighbour::max_size;
			for (unsigned offset = 0; offset < nearest.size(); offset += nlimit)
			{
				neighbours_packet p(my_node_info.id, nearest, offset, nlimit);
				send(from, p);
			}
			break;
		}
		case  discover_packet_type::neighbours:
		{
			auto in = dynamic_cast<neighbours_packet const&>(*packet);
			bool expected = false;
			auto now = std::chrono::steady_clock::now();
			{
				std::lock_guard<std::mutex> lock(find_node_timeouts_mutex);
				auto it(find_node_timeouts.find(in.source_id));
				if (it != find_node_timeouts.end())
				{
					if ((now - it->second) < req_timeout)
						expected = true;
					else
						find_node_timeouts.erase(it);
				}
			}
			if (!expected)
			{
                LOG(m_log.debug) << "Dropping unsolicited neighbours packet from " << from;
				break;
			}

			for (auto n : in.neighbours)
			{
				add_node(node_info(n.id, n.endpoint));
			}
			break;
		}
		}

		note_active_node(packet->source_id, from);
	}
	catch (std::exception const& _e)
	{
        LOG(m_log.error) << "Exception processing message from " << from.address().to_string() << ":" << from.port() << ": " << _e.what();
	}
	catch (...)
	{
        LOG(m_log.error) << "Exception processing message from " << from.address().to_string() << ":" << from.port();
	}
}

std::unique_ptr<discover_packet> node_table::interpret_packet(bi::udp::endpoint const & from, dev::bytesConstRef data)
{
	std::unique_ptr<discover_packet> packet = std::unique_ptr<discover_packet>();
	// hash + node id + sig + network + packet type + packet (smallest possible packet is ping packet which is 5 bytes)
	if (data.size() < hash256::size + node_id::size + dev::Signature::size + 1 + 1 + 5)
	{
        LOG(m_log.debug) << "Invalid packet (too small) from " << from.address().to_string() << ":" << from.port();
		return packet;
	}
	dev::bytesConstRef hash(data.cropped(0, hash256::size));
	dev::bytesConstRef bytes_to_hash_cref(data.cropped(hash256::size, data.size() - hash256::size));
	dev::bytesConstRef node_id_cref(bytes_to_hash_cref.cropped(0, node_id::size));
	dev::bytesConstRef rlp_sig_cref(bytes_to_hash_cref.cropped(node_id::size, dev::Signature::size));
	dev::bytesConstRef rlp_cref(bytes_to_hash_cref.cropped(node_id::size + dev::Signature::size));

	hash256 echo(mcp::blake2b_hash(bytes_to_hash_cref));
	dev::bytes echo_bytes = echo.asBytes();
	if (!hash.contentsEqual(echo_bytes))
	{
        LOG(m_log.debug) << "Invalid packet (bad hash) from " << from.address().to_string() << ":" << from.port();
		return packet;
	}

	node_id from_node_id;
	dev::bytesRef from_node_id_ref(from_node_id.data(), from_node_id.size);
	node_id_cref.copyTo(from_node_id_ref);

	dev::Signature rlp_sig;
	rlp_sig_cref.copyTo(rlp_sig.ref());

	hash256 rlp_hash(mcp::blake2b_hash(rlp_cref));

	//LOG(m_log.debug) << boost::str(boost::format("receive packet sig, node id:%1%, hash:%2%, sig:%3%") % from_node_id.to_string() % rlp_hash.to_string() % rlp_sig.to_string());

	///
	bool is_bad_sig(!mcp::encry::verify(from_node_id, rlp_sig, dev::h256(rlp_hash.ref())));

	if (is_bad_sig)
	{
        LOG(m_log.debug) << "Invalid packet (bad signature) from " << from.address().to_string() << ":" << from.port();
		return packet;
	}

	mcp::mcp_networks network_type((mcp::mcp_networks)rlp_cref[0]);
	if(network_type != mcp::mcp_network)
	{
        LOG(m_log.debug) << "Invalid network type " << (unsigned)network_type << " from " << from.address().to_string() << ":" << from.port();
		return packet;
	}

	try
	{
		discover_packet_type p_type((discover_packet_type)rlp_cref[1]);
		dev::bytesConstRef packet_cref(rlp_cref.cropped(2));
		switch (p_type)
		{
		case discover_packet_type::ping:
		{
			packet = std::make_unique<ping_packet>(from_node_id);
			break;
		}
		case  discover_packet_type::pong:
		{
			packet = std::make_unique<pong_packet>(from_node_id);
			break;
		}
		case discover_packet_type::find_node:
		{
			packet = std::make_unique<find_node_packet>(from_node_id);
			break;
		}
		case  discover_packet_type::neighbours:
		{
			packet = std::make_unique<neighbours_packet>(from_node_id);
			break;
		}
		default:
		{
            LOG(m_log.debug) << "Invalid packet (unknown packet type) from " << from.address().to_string() << ":" << from.port();
			break;
		}
		}
		packet->interpret_RLP(packet_cref);
	}
	catch (std::exception const & e)
	{
        LOG(m_log.debug) << "Invalid packet format " << from.address().to_string() << ":" << from.port() << " message:" << e.what();
		return packet;
	}

	return packet;
}

void node_table::send(bi::udp::endpoint const & to_endpoint, discover_packet const & packet)
{
	//LOG(m_log.debug) << "send udp packet@" << to_endpoint << " ,type:" << (uint32_t)packet.packet_type();
	send_udp_datagram datagram(to_endpoint);
	datagram.add_packet_and_sign(secret, packet);

	if (datagram.data.size() > max_udp_packet_size)
        LOG(m_log.debug) << "Sending truncated datagram, size: " << datagram.data.size() << ", packet type:" << (unsigned)packet.packet_type();

	bool doWrite = false;
	{
		std::lock_guard<std::mutex> lock(send_queue_mutex);
		send_queue.push_back(datagram);
		doWrite = (send_queue.size() == 1);
	}

	if (doWrite)
		do_write();
}

void node_table::do_write()
{
	if (is_cancel)
		return;

	send_udp_datagram const * datagram = nullptr;
	{
		std::lock_guard<std::mutex> lock(send_queue_mutex);
		datagram = &send_queue[0];
	}
	auto this_l(shared_from_this());
	bi::udp::endpoint endpoint(datagram->endpoint);
	socket->async_send_to(boost::asio::buffer(datagram->data), endpoint, [this, this_l, endpoint](boost::system::error_code ec, std::size_t size)
	{
		if (ec)
		{
            LOG(m_log.warning) << "Sending UDP message failed. " << ec.value() << " : " << ec.message();
		}

		{
			std::lock_guard<std::mutex> lock(send_queue_mutex);
			send_queue.pop_front();
			if (send_queue.empty())
				return;
		}
		do_write();
	});
}

void node_table::ping(node_endpoint const & to)
{
	node_endpoint src;
	ping_packet p(my_node_info.id, my_endpoint.tcp_port);
	send(to, p);
}

std::shared_ptr<node_entry> node_table::get_node(node_id node_id_a)
{
	std::lock_guard<std::mutex> lock(m_nodes_mutex);
	return m_nodes.count(node_id_a) ? m_nodes[node_id_a] : std::shared_ptr<node_entry>();
}

std::vector<std::shared_ptr<node_entry>> node_table::nearest_node_entries(node_id const& target_a, node_id const& source_a, std::shared_ptr<bi::udp::endpoint> from)
{
	// send s_alpha FindNode packets to nodes we know, closest to target
	static unsigned last_bin = s_bins - 1;
	unsigned head = node_entry::calc_distance(my_node_info.id, target_a);
	unsigned tail = head == 0 ? last_bin : (head - 1) % s_bins;

	std::map<unsigned, std::list<std::shared_ptr<node_entry>>> found;

	// if d is 0, then we roll look forward, if last, we reverse, else, spread from d
	if (head > 1 && tail != last_bin)
		while (head != tail && head < s_bins)
		{
			std::lock_guard<std::mutex> lock(states_mutex);
			for (auto const & n : states[head].nodes)
				if (auto p = n.lock())
					found[node_entry::calc_distance(target_a, p->id)].push_back(p);

			if (tail)
				for (auto const & n : states[tail].nodes)
					if (auto p = n.lock())
						found[node_entry::calc_distance(target_a, p->id)].push_back(p);

			head++;
			if (tail)
				tail--;
		}
	else if (head < 2)
		while (head < s_bins)
		{
			std::lock_guard<std::mutex> lock(states_mutex);
			for (auto const & n : states[head].nodes)
				if (auto p = n.lock())
					found[node_entry::calc_distance(target_a, p->id)].push_back(p);
			head++;
		}
	else
		while (tail > 0)
		{
			std::lock_guard<std::mutex> lock(states_mutex);
			for (auto const& n : states[tail].nodes)
				if (auto p = n.lock())
					found[node_entry::calc_distance(target_a, p->id)].push_back(p);
			tail--;
		}
	
	std::vector<std::shared_ptr<node_entry>> ret;
	for (auto& nodes : found)
	{
		for (auto const& n : nodes.second)
		{
			if (ret.size() < s_bucket_size && !!n->endpoint
				&& n->id != source_a) //filter remote id
			{
				if (isPublicAddress(n->endpoint.address))
				{
					ret.push_back(n);
				}
				else if(nullptr == from
					|| (isPrivateAddress(from->address()) && isSameNetwork(from->address(), n->endpoint.address))
					)
				{
					ret.push_back(n);
				}
			}
		}
	}
		
	return ret;
}

std::list<std::shared_ptr<node_info>> node_table::snapshot(unsigned& index) const
{
	std::list<std::shared_ptr<node_info>> ret;
	std::lock_guard<std::mutex> lock(states_mutex);
	while (index < s_bins)
	{
		if (states[index].nodes.empty())
			index++;
		else
		{
			for (auto const & ne : states[index].nodes)
			{
				if (auto n = ne.lock())
					ret.push_back(n);
			}
			break;
		}
	}
	return ret;
}

std::list<node_info>  node_table::nodes() const
{
	std::list<node_info> result;
	std::lock_guard<std::mutex> lock(m_nodes_mutex);
	for (auto p : m_nodes)
	{
		result.push_back(node_info(p.second->id, p.second->endpoint));
	}
	return result;
}

void node_table::add_node(node_info const & node_a, node_relation relation_a)
{
	if (!node_a.endpoint || node_a.id == my_node_info.id)
		return;

	{
		std::lock_guard<std::mutex> lock(m_nodes_mutex);
		if (m_nodes.count(node_a.id))
		{
			auto ne(m_nodes[node_a.id]);
			if (ne->endpoint != node_a.endpoint)
				ne->endpoint = node_a.endpoint;

			if (!ne->pending)
				return;
		}
		else
		{
			auto node = std::make_shared<node_entry>(my_node_info.id, node_a.id, node_a.endpoint, node_a.peer_type);
			m_nodes[node_a.id] = node;
		}
	}

	ping(node_a.endpoint);
}

std::list<std::shared_ptr<node_info>> node_table::get_random_nodes(size_t const & max_size) const
{
	std::list<std::shared_ptr<node_info>> result;

	std::vector<std::list<std::weak_ptr<node_entry>>> temp_buckets;
	temp_buckets.reserve(s_bins);

	{
		std::lock_guard<std::mutex> lock(states_mutex);
		for (auto const & bucket : states)
		{
			if (bucket.nodes.size() > 0)
				temp_buckets.push_back(bucket.nodes);
		}
	}

	if (temp_buckets.size() == 0)
		return result;

	// Shuffle the buckets.
	for(int i = temp_buckets.size() - 1; i > 0; i--)
	{
		int j = mcp::random_pool.GenerateWord32(0, i);
		temp_buckets[i].swap(temp_buckets[j]);
	}

	// Move head of each bucket into result, removing buckets that become empty.
	int index = 0;
	while (result.size() < max_size)
	{
		auto it = temp_buckets.begin() + index;
		auto & bucket = *it;
		do 
		{
			if (auto ne = bucket.back().lock())
			{
				result.push_back(ne);
				bucket.pop_back();
				break;
			}
		} 
		while (bucket.size() > 0);

		if (bucket.size() == 0)
			temp_buckets.erase(it);

		if (temp_buckets.size() == 0)
			break;

		index = (index + 1) % temp_buckets.size();
	}

	return result;
}

void node_table::drop_node(std::shared_ptr<node_entry> node_a)
{
	{
		std::lock_guard<std::mutex> lock(states_mutex);
		node_bucket & s = bucket_UNSAFE(node_a.get());
		s.nodes.remove_if([node_a](std::weak_ptr<node_entry> const & bucket_entry)
		{
			return bucket_entry.lock() == node_a;
		});
		mcp::db::db_transaction transaction(m_store.create_transaction());
		m_store.node_del(transaction, node_a);
	}

	{
		std::lock_guard<std::mutex> lock(m_nodes_mutex);
		m_nodes.erase(node_a->id);
	}

	// notify host
    LOG(m_log.debug) << "p2p.nodes.drop " << node_a->id.hex();
	if (node_event_handler)
		node_event_handler->append_event(node_a->id, node_table_event_type::node_entry_dropped);
}

node_bucket & node_table::bucket_UNSAFE(node_entry const * node_a)
{
	return states[node_a->distance - 1];
}

void node_table::note_active_node(node_id const & node_id_a, bi::udp::endpoint const& endpoint_a)
{
	//self
	if (node_id_a == my_node_info.id)
		return;

	std::shared_ptr<node_entry> new_node = get_node(node_id_a);
	if (new_node && !new_node->pending)
	{
		//LOG(m_log.debug) << "Noting active node: " << node_id_a.to_string() << " " << endpoint_a.address().to_string() << ":" << endpoint_a.port();
		new_node->endpoint.address = endpoint_a.address();
		new_node->endpoint.udp_port = endpoint_a.port();

		std::shared_ptr<node_entry> node_to_evict;
		{
			std::lock_guard<std::mutex> lock(states_mutex);
			// Find a bucket to put a node to
			node_bucket & s = bucket_UNSAFE(new_node.get());
			auto & bucket_nodes = s.nodes;

			// check if the node is already in the bucket
			auto it = bucket_nodes.begin();
			for (; it != bucket_nodes.end(); it++)
			{
				if (it->lock() == new_node)
					break;
			}

			if (it != bucket_nodes.end())
			{
				// if it was in the bucket, move it to the last position
				bucket_nodes.splice(bucket_nodes.end(), bucket_nodes, it);
			}
			else
			{
				if (bucket_nodes.size() < s_bucket_size)
				{
					// if it was not there, just add it as a most recently seen node
					// (i.e. to the end of the list)
					bucket_nodes.push_back(new_node);
					if (node_event_handler)
						node_event_handler->append_event(new_node->id, node_table_event_type::node_entry_added);
				}
				else
				{
					// if bucket is full, start eviction process for the least recently seen node
					node_to_evict = bucket_nodes.front().lock();
					// It could have been replaced in addNode(), then weak_ptr is expired.
					// If so, just add a new one instead of expired
					if (!node_to_evict)
					{
						bucket_nodes.pop_front();
						bucket_nodes.push_back(new_node);
						if (node_event_handler)
							node_event_handler->append_event(new_node->id, node_table_event_type::node_entry_added);
					}
				}
			}
		}

		if (node_to_evict)
			evict(node_to_evict, new_node);
	}
}

void node_table::evict(std::shared_ptr<node_entry> const & node_to_evict, std::shared_ptr<node_entry> const & new_node)
{
	if (!socket->is_open())
		return;

	unsigned evicts = 0;
	{
		std::lock_guard<std::mutex> lock(evictions_mutex);
		eviction_entry eviction_entry{ new_node->id, std::chrono::steady_clock::now() };
		evictions.emplace(node_to_evict->id, eviction_entry);
		evicts = evictions.size();
	}

	if (evicts == 1)
		do_check_evictions();
	ping(node_to_evict->endpoint);
}

void node_table::do_check_evictions()
{
	eviction_check_timer = std::make_unique<ba::deadline_timer>(io_service);
	eviction_check_timer->expires_from_now(eviction_check_interval);
	eviction_check_timer->async_wait([this](boost::system::error_code const& ec)
	{
		if (ec)
		{
            LOG(m_log.debug) << "Check Evictions timer was probably cancelled: " << ec.value() << " " << ec.message();
		}

		if (ec.value() == boost::asio::error::operation_aborted)
			return;

		bool evictions_remain = false;
		std::list<std::shared_ptr<node_entry>> drop;
		{
			std::lock_guard<std::mutex> evictions_lock(evictions_mutex);
			std::lock_guard<std::mutex> nodes_lock(m_nodes_mutex);
			for (auto & e : evictions)
				if (std::chrono::steady_clock::now() - e.second.evicted_time > req_timeout)
					if (m_nodes.count(e.second.new_node_id))
						drop.push_back(m_nodes[e.second.new_node_id]);
			evictions_remain = (evictions.size() - drop.size() > 0);
		}

		drop.unique();
		for (auto n : drop)
			drop_node(n);

		if (evictions_remain)
			do_check_evictions();
	});
}

void node_table::process_events()
{
	if (node_event_handler)
		node_event_handler->process_events();
}

void node_table::set_event_handler(node_table_event_handler * handler)
{
	node_event_handler.reset(handler);
}

void mcp::p2p::node_table::process_write_node_info()
{
	write_info_timer = std::make_unique<ba::deadline_timer>(io_service);
	write_info_timer->expires_from_now(write_info_interval);

	auto this_l(shared_from_this());
	write_info_timer->async_wait([this](boost::system::error_code const & ec)
	{
		if (ec.value() == boost::asio::error::operation_aborted)
			return;

		if (ec)
		{
			LOG(m_log.warning) << "Discover loop error: " << ec.value() << ":" << ec.message();
		}

		unsigned index = 0;
		while (!is_cancel && index < s_bins)
		{
			std::list<std::shared_ptr<node_info>> node_infos = snapshot(index);
			index++;
			mcp::db::db_transaction transaction(m_store.create_transaction());
			for (auto const & nf : node_infos)
			{
				mcp::p2p::node_id id(0);
				if (m_store.node_get(transaction, nf, id) &&
					id == nf->id)		//exist and id same as db, continue
				{
					continue;
				}
				m_store.node_put(transaction, nf);
			}
		}

		process_write_node_info();
	});
}
