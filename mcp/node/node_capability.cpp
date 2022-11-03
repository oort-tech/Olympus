#include "node_capability.hpp"
#include "requesting.hpp"
#include <fstream>

mcp::node_capability::node_capability(
	boost::asio::io_service& io_service_a, mcp::block_store& store_a,
	mcp::fast_steady_clock& steady_clock_a, std::shared_ptr<mcp::block_cache> cache_a,
	std::shared_ptr<mcp::async_task> async_task_a, std::shared_ptr<mcp::block_arrival> block_arrival_a,
	std::shared_ptr<mcp::TransactionQueue> tq,
	std::shared_ptr<mcp::ApproveQueue> aq
	)
    :icapability(p2p::capability_desc("mcp", 0), (unsigned)mcp::sub_packet_type::packet_count),
	m_io_service(io_service_a),
	m_store(store_a),
	m_steady_clock(steady_clock_a),
	m_cache(cache_a),
	m_async_task(async_task_a),
	m_block_arrival(block_arrival_a),
	m_tq(tq),
	m_aq(aq),
    m_stopped(false),
    m_genesis(0)
{
	m_request_timer = std::make_unique<ba::deadline_timer>(m_io_service);
	m_tq->onImport([this](ImportResult _ir, p2p::node_id const& _nodeId) { onTransactionImported(_ir, _nodeId); });
	m_aq->onImport([this](ImportApproveResult _ir, h256 const& _h, p2p::node_id const& _nodeId) { onApproveImported(_ir, _h, _nodeId); });
}

void mcp::node_capability::stop()
{
	boost::system::error_code ec;
	if (m_request_timer)
		m_request_timer->cancel(ec);

	m_stopped = true;
}


void mcp::node_capability::on_connect(std::shared_ptr<p2p::peer> peer_a, unsigned const & offset)
{
    auto  node_id = peer_a->remote_node_id();
    if (m_genesis == mcp::block_hash(0))
    {
        mcp::db::db_transaction transaction(m_store.create_transaction());
        bool error = m_store.genesis_hash_get(transaction, m_genesis);
        assert_x(!error);
    }
    
    {
        std::lock_guard<std::mutex> lock(m_peers_mutex);
        m_wait_confirm_remote_node[node_id] = mcp::local_remote_ack_hello();
        m_peers.insert(std::make_pair(node_id, mcp::peer_info(peer_a, offset)));

        //one peer one timer
        m_sync_peer_info_request_timer[node_id] = std::make_unique<ba::deadline_timer>(m_io_service);

    }
    send_hello_info_request(node_id);

    {    
        std::lock_guard<std::mutex> lock(m_peers_mutex);
        if (m_sync_peer_info_request_timer.count(node_id) > 0)
        {
            m_sync_peer_info_request_timer[node_id]->expires_from_now(boost::posix_time::seconds(180));
            m_sync_peer_info_request_timer[node_id]->async_wait([this, node_id](boost::system::error_code const & error)
            {
                if (!error)
                    timeout_for_ack_hello(node_id);
            });
        }
    }

	if (!m_request_associng.test_and_set())
	{
		request_block_timeout();
	}
}

void mcp::node_capability::on_disconnect(std::shared_ptr<p2p::peer> peer_a)
{
    {
        std::lock_guard<std::mutex> lock(m_peers_mutex);
        m_peers.erase(peer_a->remote_node_id());


        if (m_wait_confirm_remote_node.count(peer_a->remote_node_id()) > 0)
        {
            m_wait_confirm_remote_node.erase(peer_a->remote_node_id());
        }
        if (m_sync_peer_info_request_timer.count(peer_a->remote_node_id()) > 0)
        {
            m_sync_peer_info_request_timer.erase(peer_a->remote_node_id());
        }
        
    }
   
}

void mcp::node_capability::request_block_timeout()
{
	uint64_t now = m_steady_clock.now_since_epoch();

	std::list<mcp::requesting_item> requests;
	{
		requests = RequestingMageger.clear_by_time(now);
	}
	mcp::db::db_transaction transaction(m_store.create_transaction());
	for (auto const& it : requests)
	{
		bool exist = false;
		if (it.m_type == mcp::sub_packet_type::transaction_request) /// transaction
		{
			h256 h(it.m_request_hash);
			if (m_tq->exist(h) || m_cache->transaction_exists(transaction, h))
				exist = true;
		}
		else if (it.m_type == mcp::sub_packet_type::approve_request) /// approve
		{
			h256 h(it.m_request_hash);
			if (m_aq->exist(h) || m_cache->approve_exists(transaction, h))
				exist = true;
		}
		else 
		{
			if (m_block_processor->unhandle->exists(it.m_request_hash) || m_store.block_exists(transaction, it.m_request_hash))
				exist = true;
		}
		
		
		if (!exist)
		{
			auto hash(std::make_shared<mcp::block_hash>(it.m_request_hash));
			mcp::requesting_item request_item(it.m_node_id, it.m_request_hash, mcp::requesting_block_cause::new_unknown, now);
			if (it.m_type == mcp::sub_packet_type::transaction_request)
				m_sync->request_new_missing_transactions(request_item, true);
			else if (it.m_type == mcp::sub_packet_type::approve_request)
				m_sync->request_new_missing_approves(request_item, true);
			else
				m_sync->request_new_missing_joints(request_item, true);
		}
		else
		{
			RequestingMageger.try_erase(it.m_request_hash);
		}
	}

	m_request_timer->expires_from_now(boost::posix_time::seconds(20));
	m_request_timer->async_wait([this](boost::system::error_code const & error)
	{
		if (!error)
			request_block_timeout();
	});
}

void mcp::node_capability::timeout_for_ack_hello(p2p::node_id node_id_a)
{
    std::shared_ptr<p2p::peer> p;
    {
        std::lock_guard<std::mutex> lock(m_peers_mutex);
        if (m_wait_confirm_remote_node.count(node_id_a) > 0 && m_peers.count(node_id_a) > 0)
        {
            mcp::peer_info &pi(m_peers.at(node_id_a));
            p = pi.try_lock_peer();
            if (!p)
            {
                //weak_ptr use_count=0,peer drop but we dont know.
                m_wait_confirm_remote_node.erase(node_id_a);
                m_peers.erase(node_id_a);
                m_sync_peer_info_request_timer.erase(node_id_a);
            }
        }
    }
    if (p)
    {
        p->disconnect(p2p::disconnect_reason::tcp_error);
    }
}


bool mcp::node_capability::is_peer_exsist(p2p::node_id const & id)
{
    return (m_peers.count(id) > 0);
}

void mcp::node_capability::confirm_remote_connect(p2p::node_id node_id_a)
{
    {
        std::lock_guard<std::mutex> lock(m_peers_mutex);
        m_wait_confirm_remote_node.erase(node_id_a);
        if (m_sync_peer_info_request_timer.count(node_id_a) > 0)
        {
            m_sync_peer_info_request_timer[node_id_a]->expires_from_now(boost::posix_time::milliseconds(1000));
            m_sync_peer_info_request_timer[node_id_a]->async_wait([this, node_id_a](boost::system::error_code const & error)
            {
				if (!error)
				{
					m_sync->send_peer_info_request(node_id_a);
				}
            });
        }

    }
}

bool mcp::node_capability::is_local_remote_ack_ok_hello(p2p::node_id node_id_a)
{
    bool result(false);
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    if (m_wait_confirm_remote_node.count(node_id_a) > 0)
    {
        result = m_wait_confirm_remote_node[node_id_a].l_r_result && m_wait_confirm_remote_node[node_id_a].r_l_result;
    }
    return result;
}

bool mcp::node_capability::check_remotenode_hello(mcp::block_hash const & block_hash_a)
{
    return block_hash_a == m_genesis;
}

inline bool mcp::node_capability::is_remote_in_waitting_hello(p2p::node_id node_id_a)
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    return m_wait_confirm_remote_node.count(node_id_a);
}

void mcp::node_capability::set_local_ack_hello(p2p::node_id node_id_a, bool local)
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    if (m_wait_confirm_remote_node.count(node_id_a) > 0)
    {
        m_wait_confirm_remote_node[node_id_a].l_r_result = local;
    }
}

void mcp::node_capability::set_remote_ack_hello(p2p::node_id node_id_a, bool remote)
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    if (m_wait_confirm_remote_node.count(node_id_a) > 0)
    {
        m_wait_confirm_remote_node[node_id_a].r_l_result = remote;
    }
}

bool mcp::node_capability::read_packet(std::shared_ptr<p2p::peer> peer_a, unsigned const & type, std::shared_ptr<dev::RLP> rlp)
{

    //LOG(m_log.trace) << "node id: " << peer_a->remote_node_id().to_string() << ", packet type: " << type << ", rlp: " << *rlp;
    //check remote node genesis is ok
    mcp::sub_packet_type pack_type = (mcp::sub_packet_type)type;
    if (pack_type != mcp::sub_packet_type::hello_info && pack_type != mcp::sub_packet_type::hello_info_request &&  pack_type != mcp::sub_packet_type::hello_info_ack)
    {
        if (is_remote_in_waitting_hello(peer_a->remote_node_id()))
        {
            return true;
        }
    }

    try
    {
        dev::RLP const & r(*rlp);
        switch (pack_type)
        {
        case mcp::sub_packet_type::joint:
        {
            bool error(r.itemCount() != 1);
            mcp::joint_message joint(error, r[0]);

            mcp::CapMetricsRecieved.joint++;
            if (error)
            {
                LOG(m_log.error) << "Invalid new block message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }

			std::shared_ptr<mcp::block_processor_item> block_item_l(std::make_shared<mcp::block_processor_item>(joint, peer_a->remote_node_id()));
			mcp::block_hash block_hash(joint.block->hash());
			{
				if (RequestingMageger.try_erase(joint.request_id)) /// is missing blocks,it's doesn't matter whether it's broadcast or requested 
				{
					block_item_l->joint.level = mcp::joint_processor_level::request; ///if block processor full also need add this block
					block_item_l->set_missing();
					if (joint.request_id != mcp::sync_request_hash(0))
						joint.request_id.clear(); ///broadcast do not need id
				}
			}
			
			m_block_processor->add_to_mt_process(block_item_l);

            //LOG(m_log.info) << "Joint message, block hash: " << block_hash.hex();
            mark_as_known_block(peer_a->remote_node_id(), block_hash);

            break;
        }
        case mcp::sub_packet_type::joint_request:
        {
            bool error(r.itemCount() != 1);
            mcp::joint_request_message request(error, r[0]);

            if (error)
            {
                LOG(m_log.error) << "Invalid joint request message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }
            //LOG(m_log.trace) << "Recv joint request message, block hash " << request.block_hash.to_string();
			mcp::CapMetricsRecieved.joint_request++;
			m_async_task->sync_async([this, peer_a, request]() {
				m_sync->joint_request_handler(peer_a->remote_node_id(), request);
			});

            break;
        }
		case mcp::sub_packet_type::transaction:
		{
			if (r.itemCount() != 1)
			{
				LOG(m_log.error) << "Invalid transaction message rlp: " << r[0];
				peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
				return true;
			}
			try
			{
				Transaction t(r[0], CheckTransaction::Cheap);///Signature will be checked later
				auto _f = source::broadcast;
				{
					if (RequestingMageger.try_erase(mcp::block_hash(t.sha3())))
						_f = source::request;
				}
				mark_as_known_transaction(peer_a->remote_node_id(), t.sha3());
				mcp::CapMetricsRecieved.transaction++;
				m_tq->enqueue(std::make_shared<Transaction>(t), peer_a->remote_node_id(), _f);
			}
			catch (...)///Malformed transaction
			{
				LOG(m_log.error) << "Bad transaction:" << boost::current_exception_diagnostic_information();
				peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
			}
			break;
		}
		case mcp::sub_packet_type::transaction_request:
		{
			bool error(r.itemCount() != 1);
			mcp::transaction_request_message request(error, r[0]);

			if (error)
			{
				LOG(m_log.error) << "Invalid transaction request message rlp: " << r[0];
				peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
				return true;
			}
			//LOG(m_log.trace) << "Recv joint request message, block hash " << request.block_hash.to_string();
			mcp::CapMetricsRecieved.transaction_request++;
			m_async_task->sync_async([this, peer_a, request]() {
				m_sync->transaction_request_handler(peer_a->remote_node_id(), request);
			});

			break;
		}
		case mcp::sub_packet_type::approve:
		{
			LOG(m_log.trace) << "[read_packet] approve";
			bool error(r.itemCount() != 1);

			mcp::CapMetricsRecieved.approve++;
			if (error)
			{
				LOG(m_log.error) << "Invalid new approve message rlp: " << r[0];
				peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
				return true;
			}
			m_aq->enqueue(r[0], peer_a->remote_node_id());

			break;
		}
		case mcp::sub_packet_type::approve_request:
		{
			LOG(m_log.trace) << "[read_packet] approve_request";
			bool error(r.itemCount() != 1);
			mcp::approve_request_message request(error, r[0]);

			if (error)
			{
				LOG(m_log.error) << "Invalid approve request message rlp: " << r[0];
				peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
				return true;
			}
			mcp::CapMetricsRecieved.approve_request++;
			m_async_task->sync_async([this, peer_a, request]() {
				m_sync->approve_request_handler(peer_a->remote_node_id(), request);
			});

			break;
		}
        case mcp::sub_packet_type::catchup_request:
        {
            bool error(r.itemCount() != 1);
            mcp::catchup_request_message request(error, r[0]);

            if (error)
            {
                LOG(m_log.error) << "Invalid catchup request message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }

            //LOG(m_log.trace) << "Catchup request message, last stable mci: " << request.last_stable_mci
            //    << ", last known mci: " << request.last_known_mci
            //    << ", unstable_mc_joints_tail: " << request.unstable_mc_joints_tail.to_string()
            //    << ", first_catchup_chain_summary: " << request.first_catchup_chain_summary.to_string();

			mcp::CapMetricsRecieved.catchup_request++;
			m_async_task->sync_async([this, peer_a, request]() {
				m_sync->catchup_chain_request_handler(peer_a->remote_node_id(), request);
			});

            break;
        }
        case mcp::sub_packet_type::catchup_response:
        {
            bool error(r.itemCount() != 1);
            mcp::catchup_response_message response(error, r[0]);

            if (error)
            {
                LOG(m_log.error) << "Invalid catchup response message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }

            //LOG(m_log.trace) << "Catchup response message, unstable_mc_joints size: " << response.unstable_mc_joints.size()
            //    << ", stable_last_summary_joints size: " << response.stable_last_summary_joints.size();

            if (m_sync->get_current_request_id() != response.request_id)
            {
                LOG(m_log.error) << "catchup_request request id error, response.request_id:" << response.request_id.hex() << ",current_request_id:" << m_sync->get_current_request_id().hex();
                return true;
            }

			if (m_sync->response_for_sync_request(peer_a->remote_node_id(), mcp::sub_packet_type::catchup_request))
			{
				mcp::CapMetricsRecieved.catchup_response++;
				m_async_task->sync_async([this, peer_a, response]() {
					m_sync->catchup_chain_response_handler(peer_a->remote_node_id(), response);
				});
			}
			
            break;
        }
        case mcp::sub_packet_type::hash_tree_request:
        {
            bool error(r.itemCount() != 1);
            mcp::hash_tree_request_message request(error, r[0]);

            if (error)
            {
                LOG(m_log.error) << "Invalid hash tree request message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }

            //LOG(m_log.trace) << "recv hash tree request, from_summary " << request.from_summary.to_string()
            //    << ", to_summary: " << request.to_summary.to_string();

			mcp::CapMetricsRecieved.hash_tree_request++;
			m_async_task->sync_async([this, peer_a, request]() {
				m_sync->hash_tree_request_handler(peer_a->remote_node_id(), request);
			});

            break;
        }
        case mcp::sub_packet_type::hash_tree_response:
        {
            bool error(r.itemCount() != 1);
            mcp::hash_tree_response_message response(error, r[0]);

            if (error)
            {
                LOG(m_log.error) << "Invalid hash tree response message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }

            //LOG(m_log.trace) << "recv hash tree response, arr_summary size: " << response.arr_summaries.size();
            if (m_sync->get_current_request_id() != response.request_id)
            {
                LOG(m_log.error) << "hash_tree_request:return timeout. response.request_id:" << response.request_id.hex() << ",current_request_id:" << m_sync->get_current_request_id().hex();
                return true;
            }

			mcp::CapMetricsRecieved.hash_tree_response++;
			if (m_sync->response_for_sync_request(peer_a->remote_node_id(), mcp::sub_packet_type::hash_tree_request))
			{
				m_async_task->sync_async([this, peer_a, response]() {
					m_sync->hash_tree_response_handler(peer_a->remote_node_id(), response);
				});
			}

            break;
        }
        case mcp::sub_packet_type::peer_info:
        {
			/// sync do not deal peer info. just request unprocessed transactions and blocks.
			if (m_sync->is_syncing()) 
				return true;

            bool error(r.itemCount() != 1);
            mcp::peer_info_message pi(error, r[0]);

            if (error)
            {
                LOG(m_log.error) << "Invalid peer info message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }

			mcp::CapMetricsRecieved.peer_info++;

			//dag
			for (auto it = pi.arr_tip_blocks.begin(); it != pi.arr_tip_blocks.end(); it++)
			{
				auto hash(std::make_shared<mcp::block_hash>(*it));

				m_async_task->sync_async([peer_a, hash, this]() {
					try
					{
						mcp::db::db_transaction transaction(m_store.create_transaction());
						if (!m_cache->block_exists(transaction, *hash))
						{
							m_block_arrival->remove(*hash);
							uint64_t _time = m_steady_clock.now_since_epoch();
							mcp::requesting_item item(peer_a->remote_node_id(), *hash, mcp::requesting_block_cause::request_peer_info, _time);
							m_sync->request_new_missing_joints(item);
						}
					}
					catch (const std::exception& e)
					{
						LOG(m_log.error) << "peer_info error:" << e.what();
						throw;
					}
				});
			}

			//transaction
			for (auto it = pi.arr_light_tip_blocks.begin(); it != pi.arr_light_tip_blocks.end(); it++)
			{
				auto hash(std::make_shared<h256>(*it));
				m_async_task->sync_async([peer_a, hash, this]() {
					try
					{
						if (m_tq->exist(*hash))
							return;
						mcp::db::db_transaction transaction(m_store.create_transaction());
						if (!m_cache->transaction_exists(transaction, *hash))
						{
							//m_block_arrival->remove(*hash);
							uint64_t _time = m_steady_clock.now_since_epoch();

							mcp::requesting_item item(peer_a->remote_node_id(), *hash, mcp::requesting_block_cause::request_peer_info, _time);
							m_sync->request_new_missing_transactions(item);
						}
					}
					catch (const std::exception& e)
					{
						LOG(m_log.error) << "peer_info error:" << e.what();
						throw;
					}
				});
			}

			//approve
			for (auto it = pi.arr_light_approve_blocks.begin(); it != pi.arr_light_approve_blocks.end(); it++)
			{
				auto hash(std::make_shared<h256>(*it));
				m_async_task->sync_async([peer_a, hash, this]() {
					try
					{
						if (m_aq->exist(*hash))
							return;
						mcp::db::db_transaction transaction(m_store.create_transaction());
						if (!m_cache->approve_exists(transaction, *hash))
						{
							//m_block_arrival->remove(*hash);
							uint64_t _time = m_steady_clock.now_since_epoch();

							mcp::requesting_item item(peer_a->remote_node_id(), *hash, mcp::requesting_block_cause::request_peer_info, _time);
							m_sync->request_new_missing_approves(item);
						}
					}
					catch (const std::exception& e)
					{
						LOG(m_log.error) << "peer_info error:" << e.what();
						throw;
					}
				});
			}

            break;
        }
        case mcp::sub_packet_type::peer_info_request:
        {
			mcp::CapMetricsRecieved.peer_info_request++;
            //LOG(m_log.trace) << "recv peer info request. ";
			m_async_task->sync_async([peer_a, this]() {
				try
				{
					mcp::p2p::node_id id(peer_a->remote_node_id());
					bool is_handle(false);
					{
						std::lock_guard<std::mutex> lock(m_peers_mutex);
						if (m_peers.count(id))
						{
							mcp::peer_info &pi(m_peers.at(id));
							if (auto p = pi.try_lock_peer())
							{
								std::chrono::steady_clock::time_point now(m_steady_clock.now());
								if (now - pi.last_hanlde_peer_info_request_time > std::chrono::milliseconds(COLLECT_PEER_INFO_INTERVAL / 2))
								{
									pi.last_hanlde_peer_info_request_time = now;
									is_handle = true;
								}
							}
						}
					}
					if (is_handle)
						m_sync->peer_info_request_handler(id);
				}
				catch (const std::exception& e)
				{
					LOG(m_log.error) << "peer_info_request error:" << e.what();
					throw;
				}
				
			});

            break;
        }
        case mcp::sub_packet_type::hello_info:
        {
			mcp::CapMetricsRecieved.hello_info++;
            //LOG(m_log.trace) << "recv genesis info. ";
            bool error(r.itemCount() != 1);
            if (error)
            {
                LOG(m_log.error) << "Invalid genesis info message rlp: " << r[0];
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
                return true;
            }

            mcp::block_hash genesis_hash_remote = (mcp::block_hash)r[0][0];

            if (check_remotenode_hello(genesis_hash_remote))
            {
                send_hello_info_ack(peer_a->remote_node_id());
                set_local_ack_hello(peer_a->remote_node_id(), true);
                if (is_local_remote_ack_ok_hello(peer_a->remote_node_id()))
                {
                    confirm_remote_connect(peer_a->remote_node_id());
                }
            }
            else
            {
                LOG(m_log.error) << "check_remotenode_genesis error: remote_node_id = " << peer_a->remote_node_id().hex();
                peer_a->disconnect(p2p::disconnect_reason::bad_protocol);
            }

            break;
        }
        case mcp::sub_packet_type::hello_info_request:
        {
			mcp::CapMetricsRecieved.hello_info_request++;
            //LOG(m_log.trace) << "recv genesis info request.";
            send_hello_info(peer_a->remote_node_id());
            break;
        }
        case mcp::sub_packet_type::hello_info_ack:
        {
			mcp::CapMetricsRecieved.hello_info_ack++;
            //LOG(m_log.trace) << "recv genesis info confirm.";
            set_remote_ack_hello(peer_a->remote_node_id(), true);
            if (is_local_remote_ack_ok_hello(peer_a->remote_node_id()))
            {
                confirm_remote_connect(peer_a->remote_node_id());
            }
            break;
        }
        default:
            return false;
        }
    }
    catch (std::exception const & e)
    {
        LOG(m_log.trace) << "Peer error, node id: " << peer_a->remote_node_id().hex()
            << ", packet type: " << type << ", rlp: " << *rlp << ", message: " << e.what();
        throw;
    }

    return true;
}


void mcp::node_capability::broadcast_block(mcp::joint_message const & message)
{
	try
	{
		mcp::block_hash block_hash(message.block->hash());
		std::lock_guard<std::mutex> lock(m_peers_mutex);
		for (auto it = m_peers.begin(); it != m_peers.end();)
		{
			mcp::peer_info &pi(it->second);
			if (auto p = pi.try_lock_peer())
			{
				it++;
				if (pi.is_known_block(block_hash))
					continue;

				mcp::CapMetricsSend.broadcast_joint++;

				dev::RLPStream s;
				p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::joint, 1);
				message.stream_RLP(s);
				p->send(s);
			}
			else
				it = m_peers.erase(it);
		}
	}
	catch (const std::exception& e)
	{
		LOG(m_log.error) << "broadcast_block error, error: " << e.what();
		throw;
	}
}

void mcp::node_capability::broadcast_transaction(mcp::Transaction const & message)
{
	try
	{
		auto hash(message.sha3());
		std::lock_guard<std::mutex> lock(m_peers_mutex);
		for (auto it = m_peers.begin(); it != m_peers.end();)
		{
			mcp::peer_info &pi(it->second);
			if (auto p = pi.try_lock_peer())
			{
				it++;
				if (pi.is_known_transaction(hash))
					continue;

				mcp::CapMetricsSend.broadcast_transaction++;

				dev::RLPStream s;
				p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::transaction, 1);
				message.streamRLP(s);
				p->send(s);
			}
			else
				it = m_peers.erase(it);
		}
	}
	catch (const std::exception& e)
	{
		LOG(m_log.error) << "broadcast_block error, error: " << e.what();
		throw;
	}
}

void mcp::node_capability::broadcast_approve(mcp::approve const & message)
{
	try
	{
		auto hash(message.sha3());
		std::lock_guard<std::mutex> lock(m_peers_mutex);
		for (auto it = m_peers.begin(); it != m_peers.end();)
		{
			mcp::peer_info &pi(it->second);
			if (auto p = pi.try_lock_peer())
			{
				it++;
				if (pi.is_known_approve(hash))
					continue;

				mcp::CapMetricsSend.broadcast_approve++;

				dev::RLPStream s;
				p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::approve, 1);
				message.streamRLP(s);
				p->send(s);
			}
			else
				it = m_peers.erase(it);
		}
	}
	catch (const std::exception& e)
	{
		LOG(m_log.error) << "broadcast_approve error, error: " << e.what();
		throw;
	}
}

void mcp::node_capability::mark_as_known_block(p2p::node_id node_id_a, mcp::block_hash block_hash_a)
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    if (m_peers.count(node_id_a))
        m_peers.at(node_id_a).mark_as_known_block(block_hash_a);
}

void mcp::node_capability::mark_as_known_transaction(p2p::node_id node_id_a, h256 _h)
{
	std::lock_guard<std::mutex> lock(m_peers_mutex);
	if (m_peers.count(node_id_a))
		m_peers.at(node_id_a).mark_as_known_transaction(_h);
}

uint64_t mcp::node_capability::num_peers()
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    for (auto it = m_peers.begin(); it != m_peers.end();)
    {
        mcp::peer_info &pi(it->second);
        if (auto p = pi.try_lock_peer())
        {
            it++;
        }
        else
            it = m_peers.erase(it);
    }

    return m_peers.size();
}

void mcp::node_capability::send_hello_info(p2p::node_id const &id)
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    if (m_peers.count(id))
    {
        mcp::peer_info &pi(m_peers.at(id));
        if (auto p = pi.try_lock_peer())
        {
			mcp::CapMetricsSend.send_hello_info++;

            dev::RLPStream s;
            p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::hello_info, 1);
            s.appendList(2);
            s << m_genesis;
            s << desc.version;
            p->send(s);
        }
    }

}

void mcp::node_capability::send_hello_info_request(p2p::node_id const &id)
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    if (m_peers.count(id))
    {
        mcp::peer_info &pi(m_peers.at(id));
        if (auto p = pi.try_lock_peer())
        {
			mcp::CapMetricsSend.hello_info_request++;

            dev::RLPStream s;
            p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::hello_info_request, 1);
            s.appendList(0);
            p->send(s);
        }
    }
}

void mcp::node_capability::send_hello_info_ack(p2p::node_id const &id)
{
    std::lock_guard<std::mutex> lock(m_peers_mutex);
    if (m_peers.count(id))
    {
        mcp::peer_info &pi(m_peers.at(id));
        if (auto p = pi.try_lock_peer())
        {
			mcp::CapMetricsSend.hello_info_ack++;

            dev::RLPStream s;
            p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::hello_info_ack, 1);
            s.appendList(0);
            p->send(s);
        }
    }
}

void mcp::node_capability::onTransactionImported(ImportResult _ir, p2p::node_id const& _nodeId)
{
	///if used sync_async io service execute delay, maybe some transactions through the filter
	switch (_ir)
	{
	case mcp::ImportResult::Success:
		break;
	case mcp::ImportResult::FutureFull:
		break;
	case mcp::ImportResult::AlreadyInChain:
		break;
	case mcp::ImportResult::AlreadyKnown:
		break;
	case mcp::ImportResult::Malformed:
		break;
	case mcp::ImportResult::OverbidGasPrice:
		break;
	case mcp::ImportResult::BadProcol:
	{
		std::shared_ptr<p2p::peer> p = nullptr;
		{
			std::lock_guard<std::mutex> lock(m_peers_mutex);
			if (!m_peers.count(_nodeId))
				return;

			mcp::peer_info &pi(m_peers.at(_nodeId));
			p = pi.try_lock_peer();
		}
		/// todo:disconnect call drop.drop call on_disconnect.on_disconnect will used m_peers_mutex. So it locks twice. need modify logic.
		if (p)
			p->disconnect(p2p::disconnect_reason::bad_protocol);
		break;
	}
	case mcp::ImportResult::InvalidNonce:
		break;
	default:
		break;
	}
}

void mcp::node_capability::onApproveImported(ImportApproveResult _ir, h256 const& _h, p2p::node_id const& _nodeId)
{
	///io service execute delay, maybe some approve through the filter
	 if (_h != h256())
	{
		mcp::block_hash h(_h);
		std::lock_guard<std::mutex> lock(m_peers_mutex);
		if (m_peers.count(_nodeId))
		{
			m_peers.at(_nodeId).mark_as_known_approve(_h);
		}
		RequestingMageger.try_erase(h);
	}
	else
	{
		std::shared_ptr<p2p::peer> p = nullptr;
		{
			std::lock_guard<std::mutex> lock(m_peers_mutex);
			if (!m_peers.count(_nodeId))
				return;

			mcp::peer_info &pi(m_peers.at(_nodeId));
			p = pi.try_lock_peer();
		}
		/// todo:disconnect call drop.drop call on_disconnect.on_disconnect will used m_peers_mutex. So it locks twice. need modify logic.
		if (p)
			p->disconnect(p2p::disconnect_reason::bad_protocol);
	}
	/// todo different import result add or reduce rating for the peer
}




