#include <mcp/node/sync.hpp>

mcp::sync_request_status::sync_request_status(mcp::p2p::node_id const & request_node_id_a,
	mcp::sub_packet_type const & request_type_a) :
	request_node_id(request_node_id_a),
	request_type(request_type_a)
{
}

mcp::sync_request_status::sync_request_status(mcp::sync_request_status const &other_a)
{
	request_node_id = other_a.request_node_id;
	request_type = other_a.request_type;
}

bool mcp::sync_request_status::operator==(mcp::sync_request_status const & other_a)
{
	return request_node_id == other_a.request_node_id && request_type == other_a.request_type;
}

void mcp::sync_request_status::operator=(mcp::sync_request_status const &other_a)
{
	if (!(*this == other_a))
	{
		request_node_id = other_a.request_node_id;
		request_type = other_a.request_type;
	}
}

void mcp::sync_info::clear()
{
	request_hash_tree_from_summary = 0;
	request_hash_tree_to_summary = 0;
	request_hash_tree_start_index = 0;
	index = 0;
	max_index = 0;
	first = true;
	catchup_del_index.clear();
	current_del_catchup = 0;
	to_summary_index.clear();
	id = 0;
	del_catchup_index = 0;
}

void mcp::sync_info::set_info(mcp::summary_hash const & from_summary, mcp::summary_hash const & to_summary, uint64_t const & index_a)
{
	request_hash_tree_from_summary = from_summary;
	request_hash_tree_to_summary = to_summary;
	request_hash_tree_start_index = index_a;
}

std::atomic<mcp::sync_status> mcp::node_sync::m_status;
mcp::sync_info mcp::node_sync::m_request_info;
mcp::node_sync::node_sync(
	std::shared_ptr<mcp::node_capability> capability_a, mcp::block_store& store_a,
	std::shared_ptr<mcp::chain> chain_a, std::shared_ptr<mcp::block_cache> cache_a,
	std::shared_ptr<mcp::async_task> async_task_a,
	mcp::fast_steady_clock& steady_clock_a, boost::asio::io_service & io_service_a
) :
	m_capability(capability_a),
	m_store(store_a),
	m_chain(chain_a),
	m_cache(cache_a),
	m_async_task(async_task_a),
	m_steady_clock(steady_clock_a),
	m_stoped(false),
	m_task_clear_flag(false)
{
	m_request_joints_thread = std::thread([this]() { this->process_request_joints(); });
	m_sync_timer = std::make_unique<ba::deadline_timer>(io_service_a);
	m_sync_request_timer = std::make_unique<ba::deadline_timer>(io_service_a);
}

void mcp::node_sync::stop()
{
	m_stoped = true;
	{
		std::lock_guard<std::mutex> lock(m_mutex_joint_request);
		m_joint_request_pending.clear();
		m_condition.notify_all();
	}

	boost::system::error_code ec;
	if (m_sync_timer)
		m_sync_timer->cancel(ec);
	if (m_sync_request_timer)
		m_sync_request_timer->cancel(ec);

	if (m_request_joints_thread.joinable())
	{
		m_request_joints_thread.join();
	}
}

void mcp::node_sync::joint_request_handler(p2p::node_id const &id, mcp::joint_request_message const &request)
{
	try
	{
		mcp::stopwatch_guard sw("sync:joint_request_handler");

		if (m_stoped)
			return;

		mcp::db::db_transaction transaction(m_store.create_transaction());

		std::shared_ptr<mcp::block> block = m_cache->block_get(transaction, request.block_hash);
		if (block == nullptr 
			&& request.type == mcp::block_type::light)
		{
			std::shared_ptr<mcp::unlink_block> unlink_block = m_cache->unlink_block_get(transaction, request.block_hash);
			if (unlink_block == nullptr)
				return;
			block = unlink_block->block;
		}

		if (nullptr == block)
			return;

		std::shared_ptr<mcp::block_state> block_state(m_cache->block_state_get(transaction, request.block_hash));

		if (block_state && block_state->is_stable && *block_state->main_chain_index <= m_chain->min_retrievable_mci())
		{
			mcp::summary_hash summary;
			bool summary_exists(!m_cache->block_summary_get(transaction, request.block_hash, summary));
			assert_x(summary_exists);
			mcp::joint_message joint(block, summary);
			joint.request_id = request.request_id;
			send_block(id, joint);
		}
		else
		{
			////if block data cleaned, not send this block
			//if (!block->hashables->data_hash.is_zero() && block->data.empty())
			//{
			//	LOG(log_sync.info) << "joint_request_handler data_hash exist, data empty, hash:" << block->hashables->data_hash.to_string();
			//	return;
			//}
			mcp::joint_message joint(block);
			joint.request_id = request.request_id;
			send_block(id, joint);
		}
	}
	catch (const std::exception& e)
	{
		LOG(log_sync.error) << "joint_request_handler error:" << e.what();
		throw;
	}
}

void mcp::node_sync::request_catchup(p2p::node_id const& id)
{
	try
	{
		mcp::stopwatch_guard sw("node_sync:request_catchup");
		if (m_stoped)
			return;

		mcp::sync_status st = mcp::sync_status::ok;
		if(!m_status.compare_exchange_strong(st, mcp::sync_status::pending))
			return;
		
		{
			mcp::db::db_transaction transaction(m_store.create_transaction());

			{
				std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
				if (m_capability->m_peers.count(id))
				{
					mcp::peer_info &pi(m_capability->m_peers.at(id));
					if (auto p = pi.try_lock_peer())
					{
						LOG(log_sync.info) << "id:" << id.to_string() << " ,ip:" << p->remote_endpoint();
					}
				}
			}
			
			LOG(log_sync.info) << "request catch up";

			// if leftover from hash_tree_summary
			// no hash tree leftover, but with catchup chain leftover
			// if leftover from catchup chain and the chain is up to date
			//bool exist = false;
			//mcp::summary_hash last_from_hash(0);
			//{
			//	if (!m_store.catchup_index_get(transaction, m_request_info.index))//exist
			//	{
			//		if (m_store.catchup_max_index_get(transaction, m_request_info.max_index))//must be exist
			//			assert_x(false);

			//		m_request_info.current_del_catchup = m_request_info.index + 1;
			//		m_request_info.del_catchup_index = m_request_info.current_del_catchup;

			//		if (m_store.catchup_chain_summaries_get(transaction, m_request_info.index, last_from_hash))
			//			assert_x(false);

			//		m_request_info.first = false;
			//		exist = true;

			//		LOG(log_sync.info) << "request catch up: index:" << m_request_info.index << " ,  max index:" << m_request_info.max_index;
			//	}
			//}

			//if (exist)
			//{
			//	// catchup chain is complete or not, i.e. the first_catchup_chain_summary is already stable
			//	if (is_request_hash_tree() && last_from_hash == m_request_info.request_hash_tree_from_summary)
			//	{
			//		LOG(log_sync.info) << "request catch up: request mci :continue:summary:" << last_from_hash.to_string() << ",index:" << m_request_info.request_hash_tree_start_index;
			//		request_next_hash_tree(id, m_request_info.request_hash_tree_start_index);
			//		return;
			//	}
			//	mcp::block_hash bh;
			//	if (!m_store.summary_block_get(transaction, last_from_hash, bh))
			//	{
			//		LOG(log_sync.info) << "request catch up :create_hash_tree.";
			//		request_next_hash_tree(id);
			//		return;
			//	}
			//	else
			//	{
			//		m_request_info.clear();
			//		LOG(log_sync.info) << "request catch up:create_sync_mc:continue.";
			//		mcp::block_hash & from_summary = last_from_hash;
			//		request_remote_mc(transaction, id, from_summary, 0);
			//		return;
			//	}
			//}
			//else
			//{
				//last left over hash_tree_summaries
                std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
                write_option->disableWAL = true;
                {
                    mcp::db::db_transaction transaction(m_store.create_transaction(write_option));
                    try
                    {
                        m_store.catchup_index_del(transaction);
                        m_store.catchup_max_index_del(transaction);
                    }
                    catch (std::exception const & e)
                    {
                        transaction.rollback();
                        throw;
                    }
                }
				m_request_info.clear();
				purge_handled_summaries_from_hash_tree();
				request_remote_mc(transaction, id, 0, 0);
			//}
		}
	}
	catch (const std::exception& e)
	{
		LOG(log_sync.error) << "request catch up error:" << e.what();
		throw;
	}
}

void mcp::node_sync::request_remote_mc(mcp::db::db_transaction & transaction_a, p2p::node_id const & id, mcp::summary_hash const& from_summary, mcp::block_hash const& unstable_tail_block)
{
	mcp::catchup_request_message req_mg;

	uint64_t last_stable_mci(m_chain->last_stable_mci());
	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash last_stable_mc_hash;
	bool exists(!m_store.main_chain_get(transaction, last_stable_mci, last_stable_mc_hash));
	assert_x(exists);

	req_mg.last_stable_mci = last_stable_mci;
	req_mg.last_stable_mc_hash = last_stable_mc_hash;
	req_mg.last_known_mci = m_chain->last_mci();
	req_mg.unstable_mc_joints_tail = unstable_tail_block;
	req_mg.first_catchup_chain_summary = from_summary;

	mcp::witness_param const & w_param(mcp::param::curr_witness_param());
	req_mg.arr_witnesses = w_param.witness_list;
	req_mg.distinct_witness_size = w_param.witness_count - w_param.majority_of_witnesses + 1; //there must be at least one honest witness

	uint64_t timestamp(mcp::seconds_since_epoch());
	mcp::sub_packet_type ty(mcp::sub_packet_type::catchup_request);
	req_mg.request_id = m_capability->gen_sync_request_hash(id, timestamp, ty);

	LOG(log_sync.info) << "request_remote_mc::stable_mci:" << req_mg.last_stable_mci 
		<< ",last_known_mci:" << req_mg.last_known_mci 
		<< ",from_summary:" << from_summary.to_string() 
		<< ",unstable_tail_block:" << unstable_tail_block.to_string() 
		<< ",request_id:" << req_mg.request_id.to_string();;
	send_catchup_request(id, req_mg);
}

void mcp::node_sync::send_catchup_request(p2p::node_id const& id, mcp::catchup_request_message const& message)
{
	std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
	if (m_capability->m_peers.count(id))
	{
		add_task_sync_request_timer(id, mcp::sub_packet_type::catchup_request);
		mcp::peer_info &pi(m_capability->m_peers.at(id));
		if (auto p = pi.try_lock_peer())
		{
			m_capability->m_pcapability_metrics->catchup_request++;
			m_capability->m_node_id_cap_metrics[id]->catchup_request++;

			m_current_request_id = message.request_id;
			m_current_catchup_request = message;

			dev::RLPStream s;
			p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::catchup_request, 1);
			message.stream_RLP(s);
			p->send(s);
		}
	}
	else
	{
		clear_catchup_info();
	}
}

void mcp::node_sync::catchup_chain_request_handler(p2p::node_id const& id, mcp::catchup_request_message const& request)
{
	try
	{
		mcp::stopwatch_guard sw("sync:catchup_chain_request_handler");

		if (m_stoped)
			return;

		mcp::catchup_response_message response;
		response.request_id = request.request_id;

		// Check if the received request message is valid
		if ((request.last_stable_mci >= request.last_known_mci) &&
			(request.last_stable_mci > 0 || request.last_known_mci > 0))
		{
			response.status = mcp::sync_response_status::stable_mci_error;
			send_catchup_response(id, response);
			return;
		}

		if (request.last_stable_mci >= m_chain->last_stable_index())
		{
			response.status = mcp::sync_response_status::last_stable_mci_error;
			send_catchup_response(id, response);
			return;
		}

		{
			mcp::db::db_transaction transaction(m_store.create_transaction());
			mcp::block_hash mc_hash_remote;
			if (m_store.main_chain_get(transaction, request.last_stable_mci, mc_hash_remote))
			{
				response.status = mcp::sync_response_status::main_chain_error;
				send_catchup_response(id, response);
				return;
			}

			if (mc_hash_remote != request.last_stable_mc_hash)
			{
				response.status = mcp::sync_response_status::last_stable_mc_hash_error;
				send_catchup_response(id, response);
				return;
			}
		}

		// Prepare catchup chain and send response
		prepare_catchup_chain(request, response);

		// Check if the catchup chain has been prepared successfully
		// Or if the local chain is already current
		if (response.unstable_mc_joints.empty() && response.stable_last_summary_joints.empty())
		{
			response.status = mcp::sync_response_status::joints_empty_error;
			send_catchup_response(id, response);
			return;
		}

		// Send checkup response
		send_catchup_response(id, response);
	}
	catch (const std::exception& e)
	{
		LOG(log_sync.error) << "catchup_chain_request_handler error:" << e.what();
		throw;
	}
	
}

void mcp::node_sync::prepare_catchup_chain(mcp::catchup_request_message const& request, mcp::catchup_response_message & response)
{
	uint64_t last_stable_mci_remote = request.last_stable_mci;
	uint64_t last_known_mci_remote = request.last_known_mci;
	mcp::summary_hash first_catchup_chain_summary = request.first_catchup_chain_summary;
	std::set<mcp::account> arr_witnesses_remote = request.arr_witnesses;
	size_t distinct_witness_size_remote = request.distinct_witness_size;

	mcp::db::db_transaction transaction(m_store.create_transaction());

#pragma region prepare witness proof
	// read all joints on main chain after last stable mci
	std::list<mcp::joint_message> arr_unstable_mc_joints;
	std::list<mcp::joint_message> arr_stable_last_summary_joints;
	mcp::block_hash last_summary_hash(0);
	std::unordered_set<mcp::account> arr_found_witnesses;

	if (first_catchup_chain_summary == 0)
	{

		bool remaining_unstable_mc_joints_start = (request.unstable_mc_joints_tail == 0);

		uint64_t last_stable_mci;
		uint64_t last_mci;
		std::shared_ptr<rocksdb::ManagedSnapshot> snapshot = m_store.create_snapshot();
		while (true)
		{
			last_stable_mci = m_chain->last_stable_mci();
			last_mci = m_chain->last_mci();
			assert_x(last_mci > last_stable_mci);

			//make sure last mci exists in snapshot
			mcp::block_hash last_mc_hash;
			bool last_mc_exists(!m_store.main_chain_get(transaction, last_mci, last_mc_hash, snapshot));
			if (last_mc_exists)
				break;

			//m_store.release_snapshot(snapshot);
			snapshot = m_store.create_snapshot();
		}

		uint64_t mci = last_mci;
		for (; mci > last_stable_mci; mci--)
		{
			// count the majority witness blocks
			mcp::block_hash mc_hash;
			bool mc_exists(!m_store.main_chain_get(transaction, mci, mc_hash, snapshot));
			assert_x(mc_exists);

			std::shared_ptr<mcp::block> mc_block = m_cache->block_get(transaction, mc_hash);
			mcp::account acct = mc_block->from();

			//find first honest witness
			if (arr_found_witnesses.size() < distinct_witness_size_remote)
			{
				if (arr_witnesses_remote.count(acct) && !arr_found_witnesses.count(acct))
				{
					arr_found_witnesses.insert(acct);
				}

			}
			if (arr_found_witnesses.size() == distinct_witness_size_remote)
			{
				// Collect last summaries of last distinct witnessed blocks
				if (!mc_block->hashables->last_summary_block.is_zero() && last_summary_hash.is_zero())
				{
					last_summary_hash = mc_block->hashables->last_summary_block;
				}

				std::shared_ptr<mcp::block_state> first_last_summary_state(m_cache->block_state_get(transaction, mc_block->hashables->last_summary_block));
				assert_x(first_last_summary_state
					&& first_last_summary_state->is_on_main_chain
					&& first_last_summary_state->main_chain_index);
				if (last_stable_mci_remote >= *first_last_summary_state->main_chain_index)
					return;
			}

			// skip the unstable mc joints that is already transmited
			if (!remaining_unstable_mc_joints_start)
			{
				if (request.unstable_mc_joints_tail == mc_hash)
					remaining_unstable_mc_joints_start = true;
				continue;
			}

			mcp::joint_message joint(m_cache->block_get(transaction, mc_hash));
			arr_unstable_mc_joints.push_back(joint);

			if (arr_unstable_mc_joints.size() >= 500)
				break;
		}

		// If unstable_mc_joints is partial, skip stable_last_summary_joint construcion.
		if (mci != last_stable_mci)
		{
			//LOG(log_sync.info) << "arr_unstable_mc_joints size = " << arr_unstable_mc_joints.size();
			//LOG(log_sync.info) << "arr_stable_last_summary_joints = " << arr_stable_last_summary_joints.size();
			response.unstable_mc_joints = std::move(arr_unstable_mc_joints);
			response.stable_last_summary_joints = std::move(arr_stable_last_summary_joints);
			response.is_catchup_chain_complete = false;

			return;
		}

		if (arr_found_witnesses.size() != distinct_witness_size_remote)
		{
			LOG(log_sync.info) << "your witness list might be too much off, too few witness authored blocks";
			return;
		}
	}
#pragma endregion

	// create arr_stable_last_summary_joints
	mcp::block_hash last_summary_block;
	if (first_catchup_chain_summary == 0)
	{
		last_summary_block = last_summary_hash;
	}
	else
	{
		m_store.summary_block_get(transaction, first_catchup_chain_summary, last_summary_block);
	}

	std::shared_ptr<mcp::block_state> last_summary_state(m_cache->block_state_get(transaction, last_summary_block));
	if (!last_summary_state)
	{
		LOG(log_sync.info) << "Error to get the state of last summary block";
		return;
	}
	uint64_t last_summary_mci = *last_summary_state->main_chain_index;
	if (last_stable_mci_remote >= last_summary_mci)
	{
		LOG(log_sync.info) << "already current";
		return;
	}

	// goup
	while (1)
	{
		std::shared_ptr<mcp::block> bk = m_cache->block_get(transaction, last_summary_block);
		mcp::summary_hash sh;
		m_cache->block_summary_get(transaction, last_summary_block, sh);

		mcp::joint_message joint(bk);
		joint.summary_hash = sh;
		arr_stable_last_summary_joints.push_back(joint);

		LOG(log_sync.debug) << "arr_stable_last_summary_joints, mci = " << last_summary_mci
			<< ", summary = " << joint.summary_hash.to_string() << ",block:" << last_summary_block.to_string();
		assert_x(!sh.is_zero());
		if (last_summary_mci > last_stable_mci_remote && last_summary_mci > 0 && arr_stable_last_summary_joints.size() < 500)
		{
			// go to next - last summary
			last_summary_block = bk->hashables->last_summary_block;
			std::shared_ptr<mcp::block_state> bs(m_cache->block_state_get(transaction, last_summary_block));
			last_summary_mci = *bs->main_chain_index;
		}
		else
			break;
	}

	//LOG(log_sync.debug) << "arr_unstable_mc_joints size = " << arr_unstable_mc_joints.size();
	//LOG(log_sync.debug) << "arr_stable_last_summary_joints = " << arr_stable_last_summary_joints.size();
	response.unstable_mc_joints = arr_unstable_mc_joints;
	response.stable_last_summary_joints = arr_stable_last_summary_joints;

	if (last_summary_mci <= last_stable_mci_remote || last_summary_mci == 0)
		response.is_catchup_chain_complete = true;
	else
		response.is_catchup_chain_complete = false;
}

void mcp::node_sync::send_catchup_response(p2p::node_id const& id, mcp::catchup_response_message const& message)
{
	std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
	if (m_capability->m_peers.count(id))
	{
		mcp::peer_info &pi(m_capability->m_peers.at(id));
		if (auto p = pi.try_lock_peer())
		{
			m_capability->m_pcapability_metrics->catchup_response++;
			m_capability->m_node_id_cap_metrics[id]->catchup_response++;

			dev::RLPStream s;
			p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::catchup_response, 1);
			message.stream_RLP(s);
			p->send(s);
		}
	}
}

bool mcp::node_sync::response_for_sync_request(p2p::node_id const & request_node_id_a, mcp::sub_packet_type const & request_type_a)
{
	//LOG(log_sync.info) << "response_for_sync_request : success:" << request_node_id_a.to_string() << ",request_type:" << unsigned(request_type_a);
	bool exist = false;
	mcp::sync_request_status sync_request_time_out(request_node_id_a, request_type_a);
	{
		std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
		for (auto it = m_sync_requests.begin(); it != m_sync_requests.end(); it++)
		{
			if (it->second == sync_request_time_out)
			{
				m_sync_requests.erase(it->first);
				exist = true;

				boost::system::error_code ec;
				if (m_sync_request_timer)	//recieved cancel timer
					m_sync_request_timer->cancel(ec);
				break;
			}
		}
		m_task_clear_flag = true;
	}
	
	return exist;
}

void mcp::node_sync::catchup_chain_response_handler(p2p::node_id const& id, mcp::catchup_response_message const& response)
{
	try
	{
		mcp::stopwatch_guard sw("sync:catchup_chain_response_handler");

		if (m_stoped)
			return;

		if (response.status != mcp::sync_response_status::ok)
		{
			LOG(log_sync.info) << "catchup_chain_response_handler remote send error, code:" << (uint32_t)response.status;
			clear_catchup_info();
			return;
		}

		auto result = process_catchup_chain(response);
		if (result != mcp::sync_result::ok && result != mcp::sync_result::catchup_chain_continue)
		{
			LOG(log_sync.info) << "process_catchup_chain_debug:check fail start next request catch up";
			m_status = mcp::sync_status::ok;
			m_request_info.unstable_mc_joints.clear();
			return;
		}

        m_status = mcp::sync_status::syncing;

		if (response.is_catchup_chain_complete)
		{
			auto ret = request_next_hash_tree(id);
		}
		else
		{
			request_catchup_second(id);
		}
	}
	catch (const std::exception& e)
	{
		LOG(log_sync.error) << "catchup_chain_response_handler error:" << e.what();
		throw;
	}
}

void mcp::node_sync::request_catchup_second(p2p::node_id const & id)
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	// if leftover from unstable_mc_joints
	LOG(log_sync.info) << "request_chain_second";
	mcp::summary_hash from_summary(0);
	mcp::block_hash unstable_tail_block(0);

	if (!m_request_info.unstable_mc_joints.empty())
	{
		unstable_tail_block = m_request_info.unstable_mc_joints.back().block->hash();
	}
	else
	{
		uint64_t index = 0;
		{
			if (!m_store.catchup_index_get(transaction, index))//exist
			{
				if (m_store.catchup_chain_summaries_get(transaction, index, from_summary))
					assert_x(false);
			}
			else
				assert_x(false);
		}
	}
	request_remote_mc(transaction, id, from_summary, unstable_tail_block);
}

mcp::sync_result mcp::node_sync::request_next_hash_tree(p2p::node_id const& id, uint64_t const & next_start_index)
{
	mcp::sync_result result(mcp::sync_result::ok);

	m_request_info.id = id;
	mcp::summary_hash from_summary(0);
	mcp::summary_hash to_summary(0);
	uint64_t index_to = 0;
	{
		//get last request index
		mcp::db::db_transaction transaction(m_store.create_transaction());
		while (true)
		{
			if (next_start_index != 0)
				break;
			mcp::summary_hash summary_hash;
			if (m_store.catchup_chain_summaries_get(transaction, m_request_info.index - 1, summary_hash))
				break;
			mcp::block_hash block_hash;
			assert_x(!m_store.catchup_chain_summary_block_get(transaction, summary_hash, block_hash));
			bool exists(!m_store.summary_block_get(transaction, summary_hash, block_hash));
			if (!exists)
				break;
			LOG(log_sync.debug) << "send hash tree request:skip index:" << m_request_info.index;
			m_request_info.index--;
			if (m_request_info.index < 1)
			{
				clear_catchup_info();
				return mcp::sync_result::request_next_hash_tree_one_summary;
			}
		}

		if (m_request_info.first)
		{
			m_request_info.first = false;
			if (m_store.catchup_index_get(transaction, m_request_info.index))//first request
			{
				assert_x_msg(false,"request_next_hash_tree: index not exist" + std::to_string(m_request_info.index));
			}
			m_request_info.current_del_catchup = m_request_info.index + 1;
			m_request_info.del_catchup_index = m_request_info.current_del_catchup;
			if (m_store.catchup_max_index_get(transaction, m_request_info.max_index))//must be exist
				assert_x(false);
		}
		
		//get from_summary
		if (m_store.catchup_chain_summaries_get(transaction, m_request_info.index, from_summary))
		{
			if (next_start_index != 0)
			{
				assert_x_msg(false, "request_next_hash_tree: catchup_chain_summaries is impossible empty.");
			}
			LOG(log_sync.info) << "send hash tree request:error: summary is null.";
			mcp::sync_result result = mcp::sync_result::request_next_hash_tree_no_summary;
			m_request_info.clear();
			m_status = mcp::sync_status::ok;
			return result;
		}

		// If only one elmenet in catcup chain i.e. no to_summary is defined,return.
		if (m_request_info.index < 1)
		{
			LOG(log_sync.info) << "index small than 1.";
			return mcp::sync_result::request_next_hash_tree_one_summary;
		}

		//to summary
		index_to = m_request_info.index - 1;
		if (m_store.catchup_chain_summaries_get(transaction, index_to, to_summary))
		{
			assert_x_msg(false, "request_next_hash_tree: catchup_chain_summaries is impossible empty.");
		}
	}

	m_request_info.set_info(from_summary, to_summary, next_start_index);

	mcp::hash_tree_request_message request(from_summary, to_summary);
	request.next_start_index = next_start_index;

	uint64_t timestamp(mcp::seconds_since_epoch());
	mcp::sub_packet_type ty(mcp::sub_packet_type::hash_tree_request);
	request.request_id = m_capability->gen_sync_request_hash(id, timestamp, ty);

	LOG(log_sync.debug) << "send hash tree request, from summary: " << from_summary.to_string()
		<< ", to summary: " << to_summary.to_string() << ",next index:" << next_start_index << ",request_id:" << request.request_id.to_string();


	send_hash_tree_request(id, request);
	return result;
}

void mcp::node_sync::send_hash_tree_request(p2p::node_id const & id, mcp::hash_tree_request_message const & message)
{
	std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
	if (m_capability->m_peers.count(id))
	{
		add_task_sync_request_timer(id, mcp::sub_packet_type::hash_tree_request);
		mcp::peer_info &pi(m_capability->m_peers.at(id));
		if (auto p = pi.try_lock_peer())
		{
			m_capability->m_pcapability_metrics->hash_tree_request++;
			m_capability->m_node_id_cap_metrics[id]->hash_tree_request++;

			m_current_request_id = message.request_id;
			dev::RLPStream s;
			p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::hash_tree_request, 1);
			message.stream_RLP(s);
			p->send(s);
		}
	}
	else
	{
		clear_catchup_info();
	}
}

mcp::sync_result mcp::node_sync::process_catchup_chain(mcp::catchup_response_message const& catchup_chain)
{
	mcp::sync_result process_catchup_result(mcp::sync_result::ok);
	mcp::db::db_transaction transaction(m_store.create_transaction());
	{
		mcp::joint_message catchup_chain_tail;
		std::shared_ptr<mcp::block> catchup_chain_tail_block;

		mcp::block_hash last_summary_block;
		mcp::summary_hash last_summary;
		bool catchup_chain_block_summary_error(false);

		if (!catchup_chain.unstable_mc_joints.empty())
		{
			if (!m_request_info.unstable_mc_joints.empty())
			{
				auto const & my_last_parents(m_request_info.unstable_mc_joints.back().block->parents());
				mcp::block_hash const & remote_first_unstable_mc_hash(catchup_chain.unstable_mc_joints.front().block->hash());
				if (std::find(my_last_parents.begin(), my_last_parents.end(), remote_first_unstable_mc_hash) == my_last_parents.end())
				{
					LOG(log_sync.info) << "process_catchup_chain_error:not in parents";
					process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
					return  process_catchup_result;
				}
			}

			// append unstable_mc_joints
			m_request_info.unstable_mc_joints.insert(m_request_info.unstable_mc_joints.end(), catchup_chain.unstable_mc_joints.begin(),
				catchup_chain.unstable_mc_joints.end());

			// If stable_last_summary_joints is empty iff there is leftover on unstable_mc_joints.
			// If there is leftover, return immediately and waiting for next catchup chain response.
			if (catchup_chain.stable_last_summary_joints.empty())
			{
				process_catchup_result = mcp::sync_result::catchup_chain_continue;
				return  process_catchup_result;
			}


#pragma region process unstable_mc_joints since it is completed
			// witness proof
			std::vector<mcp::block_hash> arr_last_summary_blocks;
			std::map<mcp::block_hash, mcp::summary_hash> assoc_last_summary_by_block;

			uint64_t distinct_witness_size = m_current_catchup_request.distinct_witness_size;
			assert_x(distinct_witness_size > 0);
			std::set<mcp::account> const & arr_witnesses = m_current_catchup_request.arr_witnesses;
			std::vector<mcp::account> arr_found_witnesses;
			std::vector<mcp::joint_message> arr_witness_joints;

			std::vector<mcp::block_hash> arr_parent_blocks;

			LOG(log_sync.debug) << "unstable_mc_joints feached successfully, size: " << m_request_info.unstable_mc_joints.size();


			for (auto joint : m_request_info.unstable_mc_joints)
			{
				std::shared_ptr<mcp::block> block = joint.block;
				mcp::block_hash bh = block->hash();

				if (!joint.summary_hash.is_zero())
				{
					LOG(log_sync.info) << "process_catchup_chain_error:unstable mc but has summary";
					process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
					return  process_catchup_result;
				}
				if (!arr_parent_blocks.empty() &&
					std::find(arr_parent_blocks.begin(), arr_parent_blocks.end(), bh) == arr_parent_blocks.end())
				{
					LOG(log_sync.info) << "process_catchup_chain_error:not in parents";
					process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
					return  process_catchup_result;
				}

				mcp::account acct = block->from();
				if (arr_witnesses.count(acct))
				{
					if (std::find(arr_found_witnesses.begin(), arr_found_witnesses.end(), acct) == arr_found_witnesses.end())
					{
						arr_found_witnesses.push_back(acct);
					}
					arr_witness_joints.push_back(joint);
				}

				arr_parent_blocks = block->hashables->parents;

				// derive last_summary_block from receiver's own point of view
				if (!block->hashables->last_summary_block.is_zero() && arr_found_witnesses.size() >= distinct_witness_size)
				{
					arr_last_summary_blocks.push_back(block->hashables->last_summary_block);
					assoc_last_summary_by_block[block->hashables->last_summary_block] = block->hashables->last_summary;
				}
			}

			if (arr_found_witnesses.size() < distinct_witness_size)
			{
				LOG(log_sync.info) << "process_catchup_chain_error:not enough witnesses in received catchup response";
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return  process_catchup_result;
			}

			if (arr_last_summary_blocks.size() == 0)
			{
				LOG(log_sync.info) << "process_catchup_chain_error:process witness proof: no last summary blocks";
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return  process_catchup_result;
			}

			// validate signature of witnesss block
			for (auto joint : arr_witness_joints)
			{
				std::shared_ptr<mcp::block> block = joint.block;
				if (!validate_message(block->from(), block->hash(), block->signature))
				{
					LOG(log_sync.info) << "process_catchup_chain_error:invalid signature";
					process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
					return  process_catchup_result;
				}
			}

			catchup_chain_tail = catchup_chain.stable_last_summary_joints.front();
			catchup_chain_tail_block = catchup_chain_tail.block;

			// verify if the last summary match to the previous catchup chain
			if (std::find(arr_last_summary_blocks.begin(), arr_last_summary_blocks.end(), catchup_chain_tail_block->hash()) == arr_last_summary_blocks.end())
			{
				LOG(log_sync.info) << "process_catchup_chain_error:first stable block is not last summary block of any unstable block";
				process_catchup_result = mcp::sync_result::catchup_chain_unstable_check_fail;
				return  process_catchup_result;
			}

			last_summary_block = catchup_chain_tail_block->hash();
			last_summary = assoc_last_summary_by_block[last_summary_block];
			m_request_info.last_stable_block_expected = last_summary_block;

#pragma endregion

			// clear unstable_mc_joints after it has been process
			m_request_info.unstable_mc_joints.clear();
		}
		else
		{
			//// verify if the catchup chain can connect to pre-exist catchup chain locally
			//uint64_t index = 0;
			//// @@todo: what if two catchup chain have overlap, rightnow there is one overlap.
			//mcp::summary_hash sh;
			//{
			//	if (!m_store.sync_get(transaction, m_request_info.s_max_index, index))//exist
			//	{
			//		if (m_store.catchup_chain_summaries_get(transaction, index, sh))
			//		{
			//			LOG(log_sync.info) << "process_catchup_chain_error:No pre-exist catchup chain at local";
			//			process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
			//			return  process_catchup_result;
			//		}
			//	}
			//	else
			//	{
			//		LOG(log_sync.info) << "process_catchup_chain_error:No pre-exist catchup chain at local";
			//		process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
			//		return  process_catchup_result;
			//	}
			//}

			catchup_chain_tail = catchup_chain.stable_last_summary_joints.front();
			catchup_chain_tail_block = catchup_chain_tail.block;
			last_summary_block = catchup_chain_tail_block->hash();
			catchup_chain_block_summary_error = m_store.catchup_chain_block_summary_get(transaction, last_summary_block, last_summary);
		}

#pragma region validate the joints on catchup chain, and put their summaries in an array
		if (catchup_chain_tail.summary_hash != last_summary)
		{
			LOG(log_sync.info) << "process_catchup_chain_error:last summary do not match." << ",last_summary:" << last_summary.to_string() << ",last_summary_block:" << last_summary_block.to_string() << ",catchup_chain_tail.summary_hash:" << catchup_chain_tail.summary_hash.to_string() << ",catchup_chain_block_summary_error:" << catchup_chain_block_summary_error;
			process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
			return  process_catchup_result;
		}

        if (m_cache->block_exists(transaction, last_summary_block))
        {
            LOG(log_sync.info) << "process_catchup_chain_error:last_summary_block exist" << last_summary_block.to_string();
            process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
            return  process_catchup_result;
        }

		for (auto joint : catchup_chain.stable_last_summary_joints)
		{
			std::shared_ptr<mcp::block> block = joint.block;

			if (joint.summary_hash.is_zero())
			{
				LOG(log_sync.info) << "process_catchup_chain_error:joints has no summaries";
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return  process_catchup_result;
			}
			if (block->hash() != last_summary_block)
			{
				LOG(log_sync.info) << "process_catchup_chain_error:not the last summary block";
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return  process_catchup_result;
			}
			if (joint.summary_hash != last_summary)
			{
				LOG(log_sync.info) << "process_catchup_chain_error:not the last summary";
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return  process_catchup_result;
			}
			if (!block->hashables->last_summary_block.is_zero())
			{
				last_summary_block = block->hashables->last_summary_block;
				last_summary = block->hashables->last_summary;
			}
		}
#pragma endregion

#pragma region validate the head of catcup chain, if it is complete
		// If it's the last piecee of catchup chain, valid if the first summary joint is stable locally
		// Adjust first chain summary if necessary and make sure it is the only stable block in the entire chain
		if (catchup_chain.is_catchup_chain_complete)
		{
			mcp::joint_message joint = catchup_chain.stable_last_summary_joints.back();
			mcp::block_hash bh = joint.block->hash();
			bool exists(m_cache->block_exists(transaction, bh));
			if (!exists)
			{
				LOG(log_sync.error) << "catche up completed, but last block not exist";
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return process_catchup_result;
			}
		}
#pragma endregion
	}

	uint64_t index = 0;
	mcp::summary_hash last_sync_chain_tail(0);
	{
		// validation complete, now append the received catchup chain

		if (!m_store.catchup_index_get(transaction, index))//exist
		{
			if(m_store.catchup_chain_summaries_get(transaction, index, last_sync_chain_tail))
				assert_x(false);
		}
	}

	for (auto it(catchup_chain.stable_last_summary_joints.begin()); it != catchup_chain.stable_last_summary_joints.end(); it++)
	{
		mcp::joint_message const & joint = *it;
		if (index != 0
			&& it == catchup_chain.stable_last_summary_joints.begin())
		{
			if (last_sync_chain_tail != joint.summary_hash)
			{
				LOG(log_sync.error) << "catchup chain:chain tail not overlap:error:summary:" << last_sync_chain_tail.to_string() << ",now:" << joint.summary_hash.to_string();
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return process_catchup_result;
			}
			else
			{
				continue;
			}
		}
		else
		{
			mcp::block_hash bh(0);
			if (!m_store.catchup_chain_summary_block_get(transaction, joint.summary_hash, bh))
			{
				LOG(log_sync.error) << "catchup chain: not chain tail gen multi:summary:" << joint.summary_hash.to_string() << "block:" << bh.to_string();
				process_catchup_result = mcp::sync_result::catchup_chain_summary_check_fail;
				return process_catchup_result;
			}
		}

		{
			std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
			write_option->disableWAL = true;
			mcp::db::db_transaction transaction(m_store.create_transaction(write_option));
			try
			{
				m_store.catchup_index_put(transaction, index);
				m_store.catchup_max_index_put(transaction, index);
				m_store.catchup_chain_summaries_put(transaction, index, joint.summary_hash);
				m_store.catchup_chain_summary_block_put(transaction, joint.summary_hash, joint.block->hash());
				m_store.catchup_chain_block_summary_put(transaction, joint.block->hash(), joint.summary_hash);
			}
			catch (std::exception const & e)
			{
				LOG(log_sync.error) << "catchup chain: put summary fail, " << e.what();
				transaction.rollback();
				throw;
			}
		}
		LOG(log_sync.debug) << "catchup chain : index:" << index << ",summary hash:" << joint.summary_hash.to_string() << ",block:" << joint.block->hash().to_string();
		index++;
	}

	process_catchup_result = mcp::sync_result::ok;
	return process_catchup_result;
}



void mcp::node_sync::hash_tree_request_handler(p2p::node_id const& id, mcp::hash_tree_request_message const& message)
{
	try
	{
		mcp::stopwatch_guard sw("sync:hash_tree_request_handler");

		if (m_stoped)
			return;

		mcp::hash_tree_response_message response;
		// std::vector<mcp::hash_tree_response_message::summary_items> arr_summaries;
		read_hash_tree(message, response);

		// send hash tree responss
		send_hash_tree_response(id, response);
	}
	catch (const std::exception& e)
	{
		LOG(log_sync.error) << "hash_tree_request_handler error:" << e.what();
		throw;
	}
}

void mcp::node_sync::read_hash_tree(mcp::hash_tree_request_message const& hash_tree_request,
	mcp::hash_tree_response_message & hash_tree_response)
{
	hash_tree_response.request_id = hash_tree_request.request_id;
	uint64_t all_summary_item_size = 0;
	mcp::db::db_transaction transaction(m_store.create_transaction());

	mcp::block_hash from_block;
	mcp::block_hash to_block;

	bool exists(!m_store.summary_block_get(transaction, hash_tree_request.from_summary, from_block));
	if (!exists)
		return;
	exists = !m_store.summary_block_get(transaction, hash_tree_request.to_summary, to_block);
	if (!exists)
		return;

	std::shared_ptr<mcp::block_state> from_block_state(m_cache->block_state_get(transaction, from_block));
	if (!(from_block_state && from_block_state->is_stable))
	{
		LOG(log_sync.error) << "read_hash_tree from_block_state not exist or not stable";
		return;
	}
	std::shared_ptr<mcp::block_state> to_block_state(m_cache->block_state_get(transaction, to_block));
	if (!(to_block_state && to_block_state->is_stable))
	{
		LOG(log_sync.error) << "read_hash_tree to_block_state not exist or not stable";
		return;
	}

	uint64_t from_index = from_block_state->stable_index;
	if (hash_tree_request.next_start_index != 0)
	{
		from_index = hash_tree_request.next_start_index;
	}
	uint64_t to_index = to_block_state->stable_index;

	//LOG(log_sync.debug) << "read_hash_tree_start:from_summary = " << hash_tree_request.from_summary.to_string()
	//	<< ",to_summary = " << hash_tree_request.to_summary.to_string()
	//	<< ",from_index = " << from_index << ",to_index = " << to_index;

	uint64_t all_link_size = 0;
	for (uint64_t index = from_index; index <= to_index; index++)
	{
		mcp::block_hash bh;
		bool exists(!m_store.stable_block_get(transaction, index, bh));
		assert_x(exists);

		if (hash_tree_response.arr_summaries.size() >= mcp::p2p::max_summary_items
			|| all_link_size > 250000)
		{
			hash_tree_response.next_start_index = index;
			//LOG(log_sync.debug) << "read_hash_tree_next:" << "index:" << index;
			break;
		}

		std::shared_ptr<mcp::block> block_ptr = m_cache->block_get(transaction, bh);
		std::shared_ptr<mcp::block_state> bs(m_cache->block_state_get(transaction, bh));
		mcp::summary_hash sh;
		m_cache->block_summary_get(transaction, bh, sh);

		//previous summary hash
		mcp::summary_hash previous_summary(0);
		if (!block_ptr->previous().is_zero())
		{
			bool previous_summary_hash_error(m_cache->block_summary_get(transaction, block_ptr->previous(), previous_summary));
			assert_x(!previous_summary_hash_error);
		}

		std::list<mcp::summary_hash> p_summaries;
		std::list<mcp::summary_hash> l_summaries;
		std::set<mcp::summary_hash> s_summaries;

		// check if all the parent have summaries
		std::vector<mcp::block_hash> const & parents(block_ptr->parents());
		for (auto it = parents.begin(); it != parents.end(); ++it)
		{
			mcp::summary_hash sh;
			if (m_cache->block_summary_get(transaction, *it, sh))
			{
				LOG(log_sync.info) << "read_hash_tree: some parents have no summaries";
				p_summaries.clear();
				return;
			}
			p_summaries.push_back(sh);
		}

		// check if all the links have summaries
		std::shared_ptr<std::list<mcp::block_hash>> links(block_ptr->links());
		for (auto it = links->begin(); it != links->end(); ++it)
		{
			mcp::summary_hash sh;
			if (m_cache->block_summary_get(transaction, *it, sh))
			{
				LOG(log_sync.info) << "read_hash_tree: some parents have no summaries";
				l_summaries.clear();
				return;
			}
			//BOOST_LOG(log) << "read_hash_tree:"<<"block:"<< bh.to_string() <<",parent summary: " << sh.to_string();
			l_summaries.push_back(sh);
		}
		all_link_size += l_summaries.size();

		// check if all the blocks on skiplist have summaries
		mcp::skiplist_info s_info;
		m_store.skiplist_get(transaction, bh, s_info);
		for (auto it = s_info.list.begin(); it != s_info.list.end(); it++)
		{
			mcp::summary_hash sh;
			if (m_cache->block_summary_get(transaction, *it, sh))
			{
				LOG(log_sync.info) << "read_hash_tree:some skiplist blocks have no summaries";
				s_summaries.clear();
				return;
			}
			//LOG(log_sync.debug)  << "skiplist summary: " << sh.to_string() ;
			s_summaries.insert(sh);
		}

		// fill the summary items
		mcp::hash_tree_response_message::summary_items s(bh, sh, previous_summary, p_summaries, l_summaries, s_summaries, bs->status, bs->stable_index, bs->mc_timestamp, bs->receipt, bs->level, block_ptr, *bs->main_chain_index, s_info.list);
		hash_tree_response.arr_summaries.insert(s);

		//dev::RLPStream rlp_stream;
		//s.stream_RLP(rlp_stream);
		//dev::bytes b;
		//rlp_stream.swapOut(b);
		//all_summary_item_size += b.size();

		//LOG(log_sync.debug) << "mci: " << mci << ", level: " << bs->level << ", block: " << bh.to_string() << ", summary: " << sh.to_string() << ",item size = " << b.size();

	}
	//LOG(log_sync.debug) << "read_hash_tree_over:" << "arr_summaries size = " << hash_tree_response.arr_summaries.size() << ",all_summary_item_size = " << all_summary_item_size;
}

void mcp::node_sync::send_hash_tree_response(p2p::node_id const& id, mcp::hash_tree_response_message const& message)
{
	std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
	if (m_capability->m_peers.count(id))
	{
		mcp::peer_info &pi(m_capability->m_peers.at(id));
		if (auto p = pi.try_lock_peer())
		{
			m_capability->m_pcapability_metrics->hash_tree_response++;
			m_capability->m_node_id_cap_metrics[id]->hash_tree_response++;

			dev::RLPStream s;
			p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::hash_tree_response, 1);
			message.stream_RLP(s);
			p->send(s);
		}
	}
}

void mcp::node_sync::hash_tree_response_handler(p2p::node_id const &id, mcp::hash_tree_response_message const &message)
{
	try
	{
		mcp::stopwatch_guard sw("sync:hash_tree_response_handler");

		if (m_stoped)
			return;

		process_hash_tree(id, message);
	}
	catch (const std::exception& e)
	{
		LOG(log_sync.error) << "hash_tree_response_handler error:" << e.what();
		throw;
	}
}

void mcp::node_sync::process_hash_tree(p2p::node_id const &id, mcp::hash_tree_response_message const &hash_tree_response)
{
	if (m_block_processor->is_full())
	{
		LOG(log_sync.warning) << "process_hash_tree: reach pending size limit";
		m_sync_timer->expires_from_now(boost::posix_time::seconds(1));
		m_sync_timer->async_wait([this, id, hash_tree_response](boost::system::error_code const & error)
		{
			if (!error)
			{
				process_hash_tree(id, hash_tree_response);
			}
			else
			{
				m_status = mcp::sync_status::ok;
				LOG(log_sync.error) << "process_hash_tree:timer error: " << error.message();
			}
		});
		return;
	}

	LOG(log_sync.debug) << "process_hash_tree:" << hash_tree_response.request_id.to_string();

	std::queue<std::shared_ptr<mcp::block_processor_item>> items;

	std::shared_ptr<rocksdb::WriteOptions> w_option(mcp::db::database::default_write_options());
	w_option->disableWAL = true;
	uint32_t tx_timeout = 500;//ms
	mcp::timeout_db_transaction tx(m_store, tx_timeout, w_option);
	try
	{
		bool error = false;
		// for each summary in hash tree
		for (auto it = hash_tree_response.arr_summaries.begin(); it != hash_tree_response.arr_summaries.end(); ++it)
		{
			mcp::hash_tree_response_message::summary_items const & s_item = *it;

			// sanity check of the summary items
			if (!s_item.block)
			{
				LOG(log_sync.info) << "process_hash_tree : no block";
				error = true;
				break;
			}

			if (s_item.block_hash.is_zero() || s_item.block_hash != s_item.block->hash())
			{
				LOG(log_sync.info) << "process_hash_tree : invalid block hash";
				error = true;
				break;
			}
			if (s_item.summary.is_zero())
			{
				LOG(log_sync.info) << "process_hash_tree:no summary";
				error = true;
				break;
			}

			mcp::summary_hash s_hash = mcp::summary::gen_summary_hash(
				s_item.block_hash,			   // block hash
				s_item.previous_summary,	//previous summary hash
				s_item.parent_summaries,	// parent summary hashs
				s_item.link_summaries,	// link summary hashs
				s_item.skiplist_summaries, // skip_list
				s_item.status,
				s_item.stable_index,
				s_item.mc_timestamp,
				s_item.receipt
			);

			if (s_item.summary != s_hash)
			{
				LOG(log_sync.info) << "process_hash_tree:wrong summary hash" << ", local_summary: " << s_hash.to_string() << ",remote_hash:" << s_item.summary.to_string();
				LOG(log_sync.info) << "process_hash_tree:wrong summary hash" << ", block: " << s_item.block_hash.to_string() << ".status:" << (int)s_item.status;
				if (s_item.receipt)
				{
					LOG(log_sync.info) << "process_hash_tree:wrong summary hash" << ", to_state: " << s_item.receipt->from_state.to_string() << ".status:" << s_item.receipt->from_state.to_string();
				}
				LOG(log_sync.info) << "process_hash_tree:wrong summary hash" << ", previous_summary: " << s_item.previous_summary.to_string();
				for (auto i : s_item.parent_summaries)
				{
					LOG(log_sync.info) << "process_hash_tree:wrong summary hash" << ", parent_summary: " << i.to_string();
				}
				for (auto i : s_item.link_summaries)
				{
					LOG(log_sync.info) << "process_hash_tree:wrong summary hash" << ", link_summary: " << i.to_string();
				}
				for (auto i : s_item.skiplist_summaries)
				{
					LOG(log_sync.info) << "process_hash_tree:wrong summary hash" << ", skiplist_summary: " << i.to_string();
				}
				error = true;
				break;
			}

			//// Verify if a block with empty data is valid
			//if (s_item.block->hashables->data_hash != 0 && s_item.block->data.empty() &&
			//	(s_item.status != mcp::block_status::fork && s_item.status != mcp::block_status::invalid))
			//{
			//	error = true;
			//	break;
			//}

			// Verify if all the previous, parents ,links, skiplist of the block exists.
			// If true, add summary contents to hash_tree_summary table
			mcp::block_hash previous_hash;
			{
				mcp::db::db_transaction & transaction(tx.get_transaction());
				bool previous_summary_exists(s_item.previous_summary.is_zero()
					|| m_store.hash_tree_summary_exists(transaction, s_item.previous_summary)
					|| (!m_store.summary_block_get(transaction, s_item.previous_summary, previous_hash)));

				if (!previous_summary_exists)
				{
					LOG(log_sync.info) << "process_hash_tree: previous summary not exsist" << ",block:" << s_item.block_hash.to_string() << ", summary: " << s_hash.to_string()
						<< ", previous summary: " << s_item.previous_summary.to_string();
					error = true;
					break;
				}

				if (!check_summaries_exist(transaction, s_item.parent_summaries))
				{
					LOG(log_sync.info) << "process_hash_tree:check_summaries_exist:not exsist" << ",block:" << s_item.block_hash.to_string() << ", summary: " << s_hash.to_string();
					error = true;
					break;
				}

				if (!check_summaries_exist(transaction, s_item.link_summaries))
				{
					LOG(log_sync.info) << "process_hash_tree:check_summaries_exist:not exsist" << ",block:" << s_item.block_hash.to_string() << ", summary: " << s_hash.to_string();
					error = true;
					break;
				}

				if (!check_summaries_exist(transaction, s_item.skiplist_summaries))
				{
					LOG(log_sync.info) << "process_hash_tree:check_summaries_exist:not exsist" << ", summary: " << s_hash.to_string();
					error = true;
					break;
				}
			}

			if (s_item.summary == m_request_info.request_hash_tree_to_summary)
			{
                if (m_request_info.index <= 1)//last
                {
                    clear_catchup_info(false);
                    LOG(log_sync.info) << "sync success";
                    return;
                }

                del_catchup_index(m_request_info.index);
			}

			add_hash_tree_summary(tx, it->summary);

			mcp::joint_message joint(s_item.block, s_item.summary);
			std::shared_ptr<mcp::block_processor_item> item(std::make_shared<mcp::block_processor_item>(std::move(joint), id, mcp::remote_type::sync));
			items.push(item);
			//LOG(log_sync.debug) << "level: " << s_item.level << ",block: "<< s_item.block.to_string() << ", summary: " << s_hash.to_string() ;
		}
		tx.commit();
		if (error)
		{
			clear_catchup_info();
			return;
		}
	}
	catch (std::exception const & e)
	{
		LOG(log_sync.error) << "add summary fail, " << e.what();
		tx.rollback();
		throw;
	}

	m_block_processor->add_many_to_mt_process(std::move(items));

	if (hash_tree_response.next_start_index != 0)
	{
		LOG(log_sync.debug) << " process_hash_tree:next:index = " << hash_tree_response.next_start_index;
		request_next_hash_tree(id, hash_tree_response.next_start_index);
		return;
	}

	{
		m_request_info.index--;
		if (m_request_info.index > 0)
		{
			request_next_hash_tree(id);
		}
	}
}

// Delete the first element of catchup_chain_summaries
void mcp::node_sync::del_catchup_indexs()
{
	if (!m_request_info.catchup_del_index.empty())
	{
		std::map<uint64_t, uint64_t> del_indexs;
		std::swap(del_indexs, m_request_info.catchup_del_index);

		m_async_task->sync_async([this, del_indexs]() {
			del_catchup_index(del_indexs);
		});
	}
}

void mcp::node_sync::del_catchup_index(uint64_t index)
{
    std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
    write_option->disableWAL = true;
    mcp::db::db_transaction transaction(m_store.create_transaction(write_option));
    try
    {
        mcp::summary_hash head_elem_summary(0);
        if (!m_store.catchup_chain_summaries_get(transaction, index, head_elem_summary))
        {
            mcp::block_hash head_elem_block;
            m_store.catchup_chain_summary_block_get(transaction, head_elem_summary, head_elem_block);
            //current index sync accessed in db,set front index
            m_store.catchup_index_put(transaction, index - 1);
            m_store.catchup_chain_summaries_del(transaction, index);
            m_store.catchup_chain_summary_block_del(transaction, head_elem_summary);
            m_store.catchup_chain_block_summary_del(transaction, head_elem_block);
            transaction.commit();

            LOG(log_sync.debug) << "process_hash_tree: del_last_request_chain:" << ",index:" << index << ",summary:" << head_elem_summary.to_string() << ",block:" << head_elem_block.to_string();
        }
    }
    catch (const std::exception& e)
    {
        LOG(log_sync.error) << "process_hash_tree: del catchup chain fail, " << e.what();
        transaction.rollback();
        throw;
    }
}

void mcp::node_sync::del_catchup_index(std::map<uint64_t, uint64_t> const& map_a)
{
	uint64_t version = 0;
	{
		std::lock_guard<std::mutex> lock(m_request_info.version_mutex);
		version = m_request_info.version;
	}

	std::lock_guard<std::mutex> lock(m_del_catchup_mutex);
	uint64_t min_index = 0;
	auto it = map_a.begin();
	while (it != map_a.end())
	{
		if (it->second == version)
		{
			min_index = it->first;
			break;
		}
		else
			it++;
	}

	if (min_index == 0 || min_index >= m_request_info.current_del_catchup || m_request_info.current_del_catchup < 1)
		return;

	while (true)
	{
		uint64_t index = m_request_info.current_del_catchup - 1;
		if (index >= min_index)
		{
			m_request_info.current_del_catchup = index;

			if (index <= 1)//last
			{
				clear_catchup_info(false);
				LOG(log_sync.info) << "sync success";
				return;
			}

            del_catchup_index(index);
		}
		else
			break;
	}
}

void mcp::node_sync::deal_exist_catchup_index(mcp::block_hash const& hash_a)
{
	std::lock_guard<std::mutex> lock(m_request_info.version_mutex);
	if (m_request_info.to_summary_index.count(hash_a))
	{
        LOG(log_sync.debug) << "catchup_del_index hash:" << hash_a.to_string() << ",index:" << std::to_string(m_request_info.to_summary_index[hash_a]);

		m_request_info.catchup_del_index.insert(std::make_pair(m_request_info.to_summary_index[hash_a], m_request_info.version));
		m_request_info.to_summary_index.erase(hash_a);
	}
}

void mcp::node_sync::clear_catchup_info(bool lock)
{
	{
		{
			std::lock_guard<std::mutex> lock(m_request_info.version_mutex);
			m_request_info.version++;
			m_request_info.clear();
			m_sync_requests.clear();
		}
		if (lock)
			std::lock_guard<std::mutex> lock(m_del_catchup_mutex);

		std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
		write_option->disableWAL = true;
		{
			mcp::db::db_transaction transaction(m_store.create_transaction(write_option));
			try
			{
				m_store.catchup_index_del(transaction);
				m_store.catchup_max_index_del(transaction);

				LOG(log_sync.info) << "clear all sync table";
			}
			catch (std::exception const & e)
			{
				LOG(log_sync.error) << "request_next_hash_tree: del catchup chain fail, " << e.what();
				transaction.rollback();
				throw;
			}
		}

		m_store.catchup_chain_summaries_clear(write_option);
		m_store.catchup_chain_summary_block_clear(write_option);
		m_store.catchup_chain_block_summary_clear(write_option);
	}

	boost::system::error_code ec;
	if (m_sync_request_timer)
		m_sync_request_timer->cancel(ec);

    m_status = mcp::sync_status::ok;

	m_block_processor->on_sync_completed(m_request_info.id);
}

void mcp::node_sync::peer_info_request_handler(p2p::node_id const &id)
{
	mcp::stopwatch_guard sw("sync:peer_info_request_handler");

	if (m_stoped)
		return;
	mcp::db::db_transaction transaction(m_store.create_transaction());

	mcp::peer_info_message pi;
	pi.min_retrievable_mci = m_chain->min_retrievable_mci();
	mcp::db::forward_iterator it_dag = m_store.dag_free_begin(transaction);
	size_t have_get_block_count = 0;
	static const unsigned max_send_count = 512;

	//dag
	size_t dag_free_count_l = 0;
	auto snap = m_store.create_snapshot();
	auto it_dag_count = m_store.dag_free_begin(transaction, snap);
	while (it_dag_count.valid())
	{
		dag_free_count_l++;
		++it_dag_count;
	}
	static const unsigned max_dag_send_count = 15;
	if (it_dag.valid())
	{
		move_free_iterator_random(it_dag, dag_free_count_l - 1, max_dag_send_count);
		auto size = get_block_from_free(it_dag, max_dag_send_count, pi.arr_tip_blocks);
		have_get_block_count = have_get_block_count + size;
	}

	assert_x(have_get_block_count < max_send_count);
	unsigned max_latest_unlink_send_count = max_send_count - have_get_block_count;

	try
	{
		auto snap = m_store.create_snapshot();

		mcp::db::forward_iterator it = m_store.unlink_info_begin(transaction, snap);
		if (it.valid())
		{
			mcp::account rand_account;
			mcp::random_pool.GenerateBlock(rand_account.bytes.data(), rand_account.bytes.size());
			mcp::db::forward_iterator rand_it = m_store.unlink_info_begin(transaction, rand_account, snap);
			if (rand_it.valid())
			{
				it = std::move(rand_it);
			}

			mcp::account start_account(0);
			while (pi.arr_light_tip_blocks.size() < max_latest_unlink_send_count)
			{
				if (!it.valid())
					it = m_store.unlink_info_begin(transaction, snap);

				mcp::account current_account(mcp::slice_to_account(it.key()));
				if (start_account == current_account)  // eq start ,break
					break;
				if (start_account.is_zero())
					start_account = current_account;

				mcp::unlink_info info(it.value());
				if (!info.latest_unlink.is_zero())
					pi.arr_light_tip_blocks.insert(std::make_pair(current_account, info.latest_unlink));
				++it;
			}
		}
	}
	catch (const std::exception& e)
	{
		LOG(log_sync.error) << "latest_unlinks error:" << e.what();
		throw;
	}
	
	send_peer_info(id, pi);
	return;
}

void mcp::node_sync::send_block(p2p::node_id const & id, mcp::joint_message const & message)
{
	mcp::block_hash block_hash(message.block->hash());
	std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
	if (m_capability->m_peers.count(id))
	{
		mcp::peer_info &pi(m_capability->m_peers.at(id));
		if (auto p = pi.try_lock_peer())
		{
			m_capability->m_pcapability_metrics->joint++;
			m_capability->m_node_id_cap_metrics[id]->joint++;

			//not check known transaction, since it will break sync
			dev::RLPStream s;
			p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::joint, 1);
			message.stream_RLP(s);
			p->send(s);
		}
	}
}

bool mcp::node_sync::is_request_hash_tree()
{
	return (!m_request_info.request_hash_tree_from_summary.is_zero() && m_request_info.request_hash_tree_start_index != 0);
}

// check if all the summaries in the parameter exists in the node
// either in hash_tree_summaries or mainnet summaries
bool mcp::node_sync::check_summaries_exist(mcp::db::db_transaction & transaction, std::list<mcp::summary_hash> const&summaries)
{
	for (auto it = summaries.begin(); it != summaries.end(); ++it)
	{
		mcp::block_hash bh;
		bool exists(m_store.hash_tree_summary_exists(transaction, *it)
			|| !m_store.summary_block_get(transaction, *it, bh));

		if (!exists)
		{
			LOG(log_sync.info) << "check_summaries_exist:not exsist" << ",summary: " << it->to_string();
			return false;
		}

	}

	return true;
}

bool mcp::node_sync::check_summaries_exist(mcp::db::db_transaction & transaction, std::set<mcp::summary_hash> const& summaries)
{
	for (auto it = summaries.begin(); it != summaries.end(); ++it)
	{
		mcp::block_hash bh;
		bool exists(m_store.hash_tree_summary_exists(transaction, *it)
			|| !m_store.summary_block_get(transaction, *it, bh));

		if (!exists)
			return false;
	}

	return true;
}

void mcp::node_sync::add_hash_tree_summary(mcp::timeout_db_transaction & tx_a, mcp::summary_hash const& summary_a)
{
	mcp::db::db_transaction & transaction(tx_a.get_transaction());
	m_store.hash_tree_summary_put(transaction, summary_a);
	tx_a.commit_if_timeout();
}

void mcp::node_sync::del_hash_tree_summaries()
{
	if (!m_to_del_hash_tree_summaries.empty())
	{
		std::list<mcp::summary_hash> to_del_summaries;
		std::swap(to_del_summaries, m_to_del_hash_tree_summaries);
		m_async_task->sync_async([this, to_del_summaries]() {
			del_hash_tree_summary(to_del_summaries);
		});
	}
}

void mcp::node_sync::put_hash_tree_summaries(mcp::summary_hash const& hash)
{
	m_to_del_hash_tree_summaries.push_back(hash);
}

void mcp::node_sync::del_hash_tree_summary(std::list<mcp::summary_hash> const & summaries_a)
{
	std::shared_ptr<rocksdb::WriteOptions> w_option(mcp::db::database::default_write_options());
	w_option->disableWAL = true;
	uint32_t tx_timeout = 500;//ms
	mcp::timeout_db_transaction tx(m_store, tx_timeout, w_option);
	try
	{
		for (mcp::summary_hash const & summary : summaries_a)
		{
			m_store.hash_tree_summary_del(tx.get_transaction(), summary);
			tx.commit_if_timeout();
		}
		tx.commit();
	}
	catch (std::exception const & e)
	{
		LOG(log_sync.error) << "del summary fail, " << e.what();
		tx.rollback();
		throw;
	}
}

void mcp::node_sync::purge_handled_summaries_from_hash_tree()
{
	std::shared_ptr<rocksdb::WriteOptions> w_option(mcp::db::database::default_write_options());
	w_option->disableWAL = true;
	try
	{
		m_store.hash_tree_summary_clear(w_option);
	}
	catch (std::exception const & e)
	{
		LOG(log_sync.error) << "purge_handled_summaries: del hash tree fail, " << e.what();
		throw;
	}
}

size_t mcp::node_sync::get_block_from_free(mcp::db::forward_iterator & it, const unsigned get_max_count, std::vector<mcp::block_hash>& get_result)
{
	unsigned get_count(0);
	for (size_t i = 0; it.valid() && i < get_max_count; ++it, ++i)
	{
		free_key key(it.key());
		get_result.push_back(key.hash_asc);
		get_count++;
	}
	return get_count;
}


size_t mcp::node_sync::move_free_iterator_random(mcp::db::forward_iterator & it, const unsigned& free_count, const unsigned& max_cap)
{
	size_t offset(0);
	if (free_count > max_cap)
	{
		unsigned max_off_set = free_count - max_cap;
		offset = mcp::random_pool.GenerateWord32(0, max_off_set);
	}
	for (size_t i = 0; i < offset && it.valid(); ++i)
	{
		++it;
	}
	return offset;
}

//use gurrent multithread
void mcp::node_sync::add_task_sync_request_timer(p2p::node_id const & request_node_id_a, mcp::sub_packet_type const & request_type_a)
{
	m_sync_requests[m_sync_request_id] = mcp::sync_request_status(request_node_id_a, request_type_a);
	if (mcp::sub_packet_type::hash_tree_request == request_type_a)
		m_sync_request_timer->expires_from_now(boost::posix_time::seconds(10));
	else
		m_sync_request_timer->expires_from_now(boost::posix_time::seconds(6));
	m_task_clear_flag = false;
	m_sync_request_timer->async_wait([this, request_node_id_a](boost::system::error_code const & error)
	{
		if (!error)
		{
			std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
			if (m_task_clear_flag)
				return;

            LOG(log_sync.info) << "timeout_for_sync_request : node_id:" << request_node_id_a.to_string();
			clear_catchup_info();
		}
	});
	m_sync_request_id++;
}

void mcp::node_sync::request_new_missing_joints(mcp::joint_request_item& item_a, uint64_t& millisecondsSinceEpoch, bool const& is_timeout)
{
	mcp::stopwatch_guard sw("sync:request_new_missing_joints");

	if (m_stoped)
		return;

	{
		std::lock_guard<std::mutex> lock(m_capability->m_requesting_lock);
		if (item_a.cause == mcp::requesting_block_cause::request_peer_info)
			m_request_info.peer_info_joint++;
		else if (item_a.cause == mcp::requesting_block_cause::existing_unknown)
			m_request_info.existing_unknown_joint++;
		else if (item_a.cause == mcp::requesting_block_cause::new_unknown)
			m_request_info.new_unknown_joint++;
		else
			assert_x(false);

		mcp::requesting_item item(item_a.id, *item_a.block_hash, millisecondsSinceEpoch, item_a.type);
		
		if (!m_capability->m_requesting.add(item, is_timeout))
		{
			//LOG(log_sync.info) << "block already requested:" << item_a.block_hash->to_string();
			return;
		}
		else
			item_a.request_id = item.m_request_id;
	}

	std::lock_guard<std::mutex> lock(m_mutex_joint_request);
	m_joint_request_pending.push_back(item_a);

	m_condition.notify_all();
}

void mcp::node_sync::process_request_joints()
{
	std::unique_lock<std::mutex> lock(m_mutex_joint_request);
	while (!m_stoped)
	{
		if (!m_joint_request_pending.empty())
		{
			mcp::stopwatch_guard sw("sync:process_request_joints");

			auto item_a = std::move(m_joint_request_pending.front());
			m_joint_request_pending.pop_front();
			lock.unlock();

			//block if existed not request again
			if (!m_block_processor->unhandle->exists(*item_a.block_hash) ||
				item_a.from == mcp::joint_request_item_from::peerinfo)
			{
				mcp::joint_request_message message(item_a.request_id, *item_a.block_hash, item_a.type);
				send_joint_request(item_a.id, message);
				//LOG(log_sync.info) << "process_request_joints hash:" << message.block_hash.to_string();
			}

			lock.lock();
		}
		else
		{
			m_condition.wait(lock);
		}
	}
}

void mcp::node_sync::send_joint_request(p2p::node_id const & id, mcp::joint_request_message const & message)
{
	std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
	if (m_capability->m_peers.count(id))
	{
		mcp::peer_info &pi(m_capability->m_peers.at(id));
		if (auto p = pi.try_lock_peer())
		{
			m_capability->m_pcapability_metrics->joint_request++;
			m_capability->m_node_id_cap_metrics[id]->joint_request++;

			dev::RLPStream s;
			p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::joint_request, 1);
			message.stream_RLP(s);
			p->send(s);

			//LOG(log_sync.info) << "id:" << id .to_string() << " , send_joint_request hash:" << message.block_hash.to_string();
		}
	}
}

void mcp::node_sync::send_peer_info_request(p2p::node_id id)
{
	if (!is_syncing())
	{
		std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
		if (m_capability->m_peers.count(id))
		{
			mcp::peer_info &pi(m_capability->m_peers.at(id));
			if (auto p = pi.try_lock_peer())
			{
				std::chrono::steady_clock::time_point now(m_steady_clock.now());
				if (now - pi.last_peer_info_request_time > std::chrono::microseconds(node_capability::COLLECT_PEER_INFO_INTERVAL / 4))
				{
					pi.last_peer_info_request_time = now;

					m_capability->m_pcapability_metrics->peer_info_request++;
					m_capability->m_node_id_cap_metrics[id]->peer_info_request++;

					dev::RLPStream s;
					p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::peer_info_request, 1);
					mcp::peer_info_request_message message;
					message.stream_RLP(s);
					p->send(s);
					LOG(log_sync.debug) << "send_peer_info_request:node id:" << id.to_string();
				}
			}
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
		if (m_capability->m_sync_peer_info_request_timer.count(id) > 0)
		{
			auto r = mcp::random_pool.GenerateWord32(0, node_capability::COLLECT_PEER_INFO_INTERVAL / 2);
			boost::posix_time::milliseconds wait_time(node_capability::COLLECT_PEER_INFO_INTERVAL + r);

			m_capability->m_sync_peer_info_request_timer[id]->cancel();
			m_capability->m_sync_peer_info_request_timer[id]->expires_from_now(wait_time);
			m_capability->m_sync_peer_info_request_timer[id]->async_wait([this, id](boost::system::error_code const & error)
			{
				if (!error)
				{
					send_peer_info_request(id);
				}
				else
				{
					LOG(log_sync.info) << "send_peer_info_request : error:code:" << error.value() << ",msg:" << error.message() << ",node id:" << id.to_string();
				}

			});
		}
		else
		{
			LOG(log_sync.info) << "send_peer_info_request : add not success:" << ",node id:" << id.to_string();
		}
	}

}

void mcp::node_sync::send_peer_info(p2p::node_id const & id, mcp::peer_info_message const & message)
{
	std::lock_guard<std::mutex> lock(m_capability->m_peers_mutex);
	if (m_capability->m_peers.count(id))
	{
		mcp::peer_info &pi(m_capability->m_peers.at(id));
		if (auto p = pi.try_lock_peer())
		{
			m_capability->m_pcapability_metrics->peer_info++;
			m_capability->m_node_id_cap_metrics[id]->peer_info++;

			dev::RLPStream s;
			p->prep(s, pi.offset + (unsigned)mcp::sub_packet_type::peer_info, 1);
			message.stream_RLP(s);
			p->send(s);
		}
	}
}

std::string mcp::node_sync::get_sync_info()
{
	std::string str = "unstable_mc_joints:" + std::to_string(m_request_info.unstable_mc_joints.size());
	str = str + ", catchup_del_index:" + std::to_string(m_request_info.catchup_del_index.size());
	str = str + ", catchup to summary size:" + std::to_string(m_request_info.to_summary_index.size());
	str = str + ", m_joint_request_pending size:" + std::to_string(m_joint_request_pending.size());
	str = str + ", del_hash_tree_summaries size:" + std::to_string(m_to_del_hash_tree_summaries.size());
	return str;
}

