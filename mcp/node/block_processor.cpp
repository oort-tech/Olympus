#include "block_processor.hpp"
#include <mcp/common/stopwatch.hpp>
#include <mcp/core/genesis.hpp>


//////test
#include <libdevcore/CommonJS.h>

mcp::late_message_info::late_message_info(std::shared_ptr<mcp::block_processor_item> item_a) :
	item(item_a),
	timestamp(item_a->joint.block->exec_timestamp()),
	hash(item_a->joint.block->hash())
{
}

mcp::late_message_cache::late_message_cache(size_t const &capacity_a) : capacity(capacity_a)
{
}

void mcp::late_message_cache::add(mcp::late_message_info const &info)
{
	std::lock_guard<std::mutex> lock(container_mutex);
	auto result(container.insert(info));
	if (result.second && container.size() > capacity)
	{
		auto it = container.get<1>().rbegin();
		/* Beware that this isn't an iterator that refers to the same object - it actually refers to the next object in the sequence
		 * returns a corresponding forward iterator, need ++.
		 */
		it++;
		auto last(it.base());
		container.get<1>().erase(last);
	}
}

std::list<mcp::late_message_info> mcp::late_message_cache::purge_list_ealier_than(uint64_t const &timestamp)
{
	std::list<mcp::late_message_info> result;

	std::lock_guard<std::mutex> lock(container_mutex);
	auto lower_bound(container.get<1>().lower_bound(timestamp)); //first element lower than timestamp
	result.assign(container.get<1>().begin(), lower_bound);
	container.get<1>().erase(container.get<1>().begin(), lower_bound);

	return result;
}

size_t mcp::late_message_cache::size() const
{
	return container.size();
}

mcp::block_processor::block_processor(bool & error_a,
	mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
	std::shared_ptr<mcp::chain> chain_a, std::shared_ptr<mcp::node_sync> sync_a,
	std::shared_ptr<mcp::node_capability> capability_a, std::shared_ptr<mcp::validation> validation_a,
	std::shared_ptr<mcp::async_task> async_task_a, std::shared_ptr<TransactionQueue> tq,
	mcp::fast_steady_clock& steady_clock_a, std::shared_ptr<mcp::block_arrival> block_arrival_a,
	boost::asio::io_service &io_service_a,
	mcp::mru_list<mcp::block_hash>& invalid_block_cache_a, std::shared_ptr<mcp::alarm> alarm_a
):
	m_store(store_a),
	m_cache(cache_a),
	m_chain(chain_a),
	m_sync(sync_a),
	m_capability(capability_a),
	m_validation(validation_a),
	m_async_task(async_task_a),
	m_tq(tq),
	m_steady_clock(steady_clock_a),
	m_block_arrival(block_arrival_a),
	m_invalid_block_cache(invalid_block_cache_a),
	m_alarm(alarm_a),
	m_stopped(false),
	m_local_cache(std::make_shared<process_block_cache>(cache_a, store_a)),
	m_last_request_unknown_missing_time(std::chrono::steady_clock::now()),
	unhandle(std::make_shared<mcp::unhandle_cache>(block_arrival_a))
{
	if (error_a)
		return;

	std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
	std::shared_ptr<rocksdb::TransactionOptions> tx_option(mcp::db::db_transaction::default_trans_options());
	tx_option->skip_concurrency_control = true;

	mcp::timeout_db_transaction timeout_tx(m_store, m_tx_timeout_ms, write_option, tx_option,
		std::bind(&block_processor::before_db_commit_event, this),
		std::bind(&block_processor::after_db_commit_event, this));
	try
	{
		m_chain->init(error_a, timeout_tx, m_local_cache);
		timeout_tx.commit();
	}
	catch (std::exception const & e)
	{
		LOG(m_log.error) << "Chain init error: " << e.what() << std::endl << boost::stacktrace::stacktrace();
		timeout_tx.rollback();
		error_a = true;
	}
	catch (...)
	{
		LOG(m_log.error) << "Chain init error unknown error." << std::endl << boost::stacktrace::stacktrace();
		timeout_tx.rollback();
		error_a = true;
	}

	if (error_a)
		return;

	m_mt_process_block_thread = std::thread([this]() { this->mt_process_blocks(); });
	m_process_block_thread = std::thread([this]() { this->process_blocks(); });

	ongoing_retry_late_message();
}

mcp::block_processor::~block_processor()
{
	stop();
}

void mcp::block_processor::stop()
{
	LOG(m_log.info) << "Block processor stopped";
	if (m_stopped)
		return;

	m_chain->stop();
	m_stopped = true;
	{
		std::lock_guard<std::mutex> lock(m_mt_process_mutex);
		m_mt_process_condition.notify_all();
	}

	{
		std::lock_guard<std::mutex> lock(m_process_mutex);
		m_process_condition.notify_all();
	}

	if (m_mt_process_block_thread.joinable())
		m_mt_process_block_thread.join();
	if (m_process_block_thread.joinable())
		m_process_block_thread.join();
}

bool mcp::block_processor::is_full()
{
	return m_blocks_pending.size() >= m_max_pending_size;
}

void mcp::block_processor::add_item(std::shared_ptr<mcp::block_processor_item> item_a)
{
	if (!item_a->joint.block)
		return;

	mcp::joint_message const & joint(item_a->joint);
	std::shared_ptr<mcp::block> block(joint.block);
	mcp::block_hash const & block_hash(block->hash());

	if (block_hash == mcp::genesis::block_hash)
		return;

	if (!item_a->is_sync()
		&& !item_a->is_local())
	{
		//not processing broadcast block when syncing
        if (item_a->is_missing())
            blocks_missing_size++;
        if (m_sync->is_syncing())
        {
            if (item_a->is_missing())
                blocks_missing_throw_size++;
            return;
        }

		if (is_full() && item_a->joint.level != mcp::joint_processor_level::request)
			return;

		if (m_block_arrival->recent(block_hash))
		{
			//LOG(m_log.debug) << "Recent block:" << block_hash.to_string();

			block_processor_recent_block_size++;
			return;
		}
	}

	if (!item_a->is_sync() && item_a->joint.summary_hash.is_zero())
	{
		//LOG(m_log.debug) << "Add recent block:" << block_hash.to_string();

		m_block_arrival->add(block_hash);
	}

	if (item_a->is_local())
	{
		m_async_task->sync_async([this, joint]() {
			m_capability->broadcast_block(joint);
		});
	}

	block_processor_add++;

	m_mt_blocks_pending.push_back(item_a);
}

void mcp::block_processor::add_many_to_mt_process(std::queue<std::shared_ptr<mcp::block_processor_item>> items_a)
{
	while (!items_a.empty())
	{
		unsigned count = 0;
		std::lock_guard<std::mutex> lock(m_mt_process_mutex);
		while (!items_a.empty() && count < m_max_mt_count)
		{
			add_item(items_a.front());
			items_a.pop();
			count++;
		}
		m_mt_process_condition.notify_all();
	}
}

void mcp::block_processor::add_to_mt_process(std::shared_ptr<mcp::block_processor_item> item_a)
{
	std::lock_guard<std::mutex> lock(m_mt_process_mutex);
	add_item(item_a);
	m_mt_process_condition.notify_all();
}

void mcp::block_processor::mt_process_blocks()
{
	std::unique_lock<std::mutex> lock(m_mt_process_mutex);
	std::shared_ptr<mcp::db::db_transaction> transaction(std::make_shared<mcp::db::db_transaction>(m_store.create_transaction()));
	while (!m_stopped)
	{
		if (!m_mt_blocks_pending.empty())
		{
			mcp::stopwatch_guard sw("mt_process_blocks");

			std::swap(m_mt_blocks_processing, m_mt_blocks_pending);
			lock.unlock();

			while (!m_mt_blocks_processing.empty())
			{
				try
				{
					std::list<std::shared_ptr<std::promise<mcp::validate_status>>> results;
					unsigned count = std::min((unsigned)m_mt_blocks_processing.size(), m_max_mt_count);
					for (unsigned i = 0; i < count; i++)
					{
						std::shared_ptr<mcp::block_processor_item> item(m_mt_blocks_processing[i]);
						std::shared_ptr<std::promise<mcp::validate_status>> p(std::make_shared<std::promise<mcp::validate_status>>());
						m_async_task->sync_async([this, item, p, transaction]() {

							auto block = item->joint.block;
							mcp::block_hash const & block_hash(block->hash());

							if (block_hash == mcp::genesis::block_hash)
							{
								p->set_value(mcp::validate_status(false, "Genesis block"));
								return;
							}

							bool ok(true);
							std::string err_msg;
							try
							{
								mcp::base_validate_result result = m_validation->base_validate(*transaction, m_cache, item);
								switch (result.code)
								{
								case base_validate_result_codes::ok:
								{
									//check timestamp
									//put it at end for no need to do base validete when block release from late_message_cache
									if (mcp::seconds_since_epoch() < block->exec_timestamp())
									{
										ok = false;
										err_msg = boost::str(boost::format("Exec timestamp too late, block: %1%, exec_timestamp: %2%, sys_timestamp: %3%") % block_hash.to_string() % block->exec_timestamp() % mcp::seconds_since_epoch());
										LOG(m_log.debug) << err_msg;
										//cache late message
										if (block->exec_timestamp() < mcp::seconds_since_epoch() + 300) //5 minutes
										{
                                            m_late_message_cache.add(item);
										}
									}

									break;
								}
								case base_validate_result_codes::old:
								{
									base_validate_old_size++;
									break;
								}
								case base_validate_result_codes::invalid_signature:
								{
									ok = false;
									err_msg = "Invalid signature, hash:" + block_hash.to_string() + ",from:" + block->from().to_account() + ",signature:" + ((Signature)block->signature()).hex();
									LOG(m_log.debug) << err_msg;

									break;
								}
								case base_validate_result_codes::invalid_block:
								{
									ok = false;
									err_msg = boost::str(boost::format("Invalid block: %1%, error message: %2%") % block->hash().to_string() % result.err_msg);
									LOG(m_log.debug) << err_msg;

									//cache invalid block
									m_invalid_block_cache.add(block_hash);
									break;
								}
								case base_validate_result_codes::known_invalid_block:
								{
									ok = false;
									err_msg = boost::str(boost::format("Know invalid block: %1%") % block->hash().to_string());
									LOG(m_log.trace) << err_msg;

									break;
								}
								}
								p->set_value(mcp::validate_status(ok, err_msg));
							}
							catch (std::exception const & e)
							{
								LOG(m_log.error) << "mt_process_blocks async error:" << e.what();
								throw;
							}
						});

						results.push_back(p);
					}

					for (std::shared_ptr<std::promise<mcp::validate_status>> p : results)
					{
						mcp::validate_status status = p->get_future().get();
						std::shared_ptr<mcp::block_processor_item> item(m_mt_blocks_processing.front());
						if (status.ok)
						{
							add_to_process(item);
						}
						else
						{
							if (item->is_local())
								item->set_local_promise(status);
						}
						m_mt_blocks_processing.pop_front();
					}
				}
				catch (std::exception const & e)
				{
					LOG(m_log.error) << "mt_process_blocks error:" << e.what();
					throw;
				}
			}

			lock.lock();
		}
		else
		{
			m_mt_process_condition.wait(lock);
		}
	}
}

void mcp::block_processor::add_to_process(std::shared_ptr<mcp::block_processor_item> item_a)
{
	if (!item_a->is_sync() && !item_a->joint.summary_hash.is_zero())
	{
		assert_x(!item_a->is_local());
		//LOG(m_log.debug) << "Start sync:" << item_a->joint.block->hash().to_string();

		//start sync;
		mcp::p2p::node_id id(item_a->remote_node_id());
		m_async_task->sync_async([this, id]() {
			m_sync->request_catchup(id);
		});
	}

	std::lock_guard<std::mutex> lock(m_process_mutex);
	if (item_a->is_local())
	{
		m_local_blocks_pending.push_back(item_a);
	}
	else
	{
		m_blocks_pending.push_back(item_a);
        if (item_a->is_sync())
            blocks_pending_sync_size++;
	}
	m_process_condition.notify_all();
}

void mcp::block_processor::process_blocks()
{
	//try to do advance first
	{
		//mcp::stopwatch_guard sw("process_blocks do try_advance first:");

		std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
		std::shared_ptr<rocksdb::TransactionOptions> tx_option(mcp::db::db_transaction::default_trans_options());
		tx_option->skip_concurrency_control = true;

		mcp::timeout_db_transaction timeout_tx(m_store, m_tx_timeout_ms, write_option, tx_option,
			std::bind(&block_processor::before_db_commit_event, this),
			std::bind(&block_processor::after_db_commit_event, this));
		try 
		{ 
			m_chain->try_advance(timeout_tx, m_local_cache); 
			timeout_tx.commit();
		}
		catch (std::exception const & e)
		{
			LOG(m_log.error) << "Block process try_advance error: " << e.what() << std::endl << boost::stacktrace::stacktrace();
			timeout_tx.rollback();
			throw;
		}
		catch (...)
		{
			LOG(m_log.error) << "Block process try_advance unknown error." << std::endl << boost::stacktrace::stacktrace();
			timeout_tx.rollback();
			throw;
		}
	}

	std::unique_lock<std::mutex> lock(m_process_mutex);
	while (!m_stopped)
	{
		std::deque<std::shared_ptr<mcp::block_processor_item>> to_processing;
		if (!m_local_blocks_pending.empty() || !m_blocks_pending.empty())
		{
			while (to_processing.size() < m_max_local_processing_size)
			{
				if (!m_local_blocks_pending.empty())
				{
					to_processing.push_back(m_local_blocks_pending.front());
					m_local_blocks_pending.pop_front();
				}
				else
				{
					if (m_blocks_pending.empty())
						break;
					to_processing.push_back(m_blocks_pending.front());

                    if (m_blocks_pending.front()->is_sync())
                        blocks_pending_sync_size--;

					m_blocks_pending.pop_front();
				}
			}
		}

		if (!to_processing.empty())
		{
			lock.unlock();
			{
				mcp::stopwatch_guard sw("process_blocks");

				std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
				std::shared_ptr<rocksdb::TransactionOptions> tx_option(mcp::db::db_transaction::default_trans_options());
				tx_option->skip_concurrency_control = true;

				std::shared_ptr<mcp::chain> chain(m_chain);
				mcp::timeout_db_transaction timeout_tx(m_store, m_tx_timeout_ms, write_option, tx_option,
					std::bind(&block_processor::before_db_commit_event, this),
					std::bind(&block_processor::after_db_commit_event, this));
				try
				{
					if (!to_processing.empty())
					{
						do_process(timeout_tx, to_processing);
						timeout_tx.commit();
					}
				}
				catch (std::exception const &e)
				{
					LOG(m_log.error) << "Block process do_process error: " << e.what() << std::endl << boost::stacktrace::stacktrace();
					timeout_tx.rollback();
					throw;
				}
				catch (...)
				{
					LOG(m_log.error) << "Block process do_process unknown error." << std::endl << boost::stacktrace::stacktrace();
					timeout_tx.rollback();
					throw;
				}
			}

			std::this_thread::yield();
			lock.lock();
		}
		else
		{
			m_process_condition.wait(lock);
		}
	}
}

bool mcp::block_processor::try_process_local_item_first(mcp::timeout_db_transaction & timeout_tx)
{
	bool has_local(false);
	if (!m_local_blocks_pending.empty())
	{
		//process local item first
		std::shared_ptr<mcp::block_processor_item> local_item = nullptr;
		{
			std::lock_guard<std::mutex> lock(m_process_mutex);
			if (!m_local_blocks_pending.empty())
			{
				has_local = true;
				local_item = m_local_blocks_pending.front();
				m_local_blocks_pending.pop_front();
			}
		}
		if (has_local)
			do_process_one(timeout_tx, local_item);
	}
	return has_local;
}

void mcp::block_processor::do_process(mcp::timeout_db_transaction & timeout_tx, std::deque<std::shared_ptr<mcp::block_processor_item>> & blocks_processing)
{
	//mcp::stopwatch_guard sw("process_blocks: do_process");

	while (!blocks_processing.empty())
	{
		if (m_stopped)
			break;

		std::shared_ptr<mcp::block_processor_item> item(blocks_processing.front());

		if (!item->is_local())
		{
			bool has_local(try_process_local_item_first(timeout_tx));
			if (has_local)
				continue;
		}

		blocks_processing.pop_front();
		do_process_one(timeout_tx, item);
	}
}

void mcp::block_processor::do_process_one(mcp::timeout_db_transaction & timeout_tx, std::shared_ptr<mcp::block_processor_item> item)
{
	mcp::joint_message const & joint(item->joint);
	std::shared_ptr<mcp::block> block(joint.block);
	mcp::block_hash const & block_hash(block->hash());
	
	if (unhandle->exists(block_hash))
	{
		process_existing_missing(item->remote_node_id());
		return;
	}

	mcp::db::db_transaction & transaction(timeout_tx.get_transaction());
	//dag validate
	mcp::validate_result result(m_validation->dag_validate(transaction, m_local_cache, joint));
	switch (result.code)
	{
	case mcp::validate_result_codes::ok:
	{
		//broadcast
		if (!item->is_local() && !item->is_sync() && item->joint.summary_hash.is_zero())
		{
			m_async_task->sync_async([this, joint]() {
				m_capability->broadcast_block(joint);
			});
		}

		do_process_dag_item(timeout_tx, item);

		break;
	}
	case mcp::validate_result_codes::old:
	{
		dag_old_size++;
		LOG(m_log.trace) << boost::str(boost::format("Old block: %1%") % block_hash.to_string());
		break;
	}
	case mcp::validate_result_codes::missing_parents_and_previous:
	{
		if (item->is_local() && result.missing_links.size() > 0)
			break;

		if (result.missing_parents_and_previous.size() > 0 || result.missing_links.size() > 0)
		{
			if (result.missing_parents_and_previous.size() > 0)
			{
				assert_x(!item->is_local());
			}
			process_missing(item, result.missing_parents_and_previous, result.missing_links);
		}

		LOG(m_log.trace) << boost::str(boost::format("Missing parents and previous for: %1%") % block_hash.to_string());

		break;
	}
	case mcp::validate_result_codes::invalid_block:
	{
		LOG(m_log.info) << boost::str(boost::format("Invalid block: %1%, error message: %2%") % block_hash.to_string() % result.err_msg);
		assert_x(!item->is_local());
		//cache invalid block
		m_invalid_block_cache.add(block_hash);
		break;
	}
	case mcp::validate_result_codes::parents_and_previous_include_invalid_block:
	{
		LOG(m_log.trace) << boost::str(boost::format("Invalid block: %1%, error message: %2%") % block_hash.to_string() % result.err_msg);
		assert_x(!item->is_local());
		//cache invalid block
		m_invalid_block_cache.add(block_hash);
		break;
	}
	case mcp::validate_result_codes::known_invalid_block:
	{
		LOG(m_log.trace) << boost::str(boost::format("Known invalid block: %1%") % block_hash.to_string());
		assert_x(!item->is_local());
		break;
	}
	}

	//local dag
	if (item->is_local())
	{
		switch (result.code)
		{
		case mcp::validate_result_codes::ok:
		case mcp::validate_result_codes::old:
			m_ok_local_promises.push_back(item->get_local_promise());
			break;
		case mcp::validate_result_codes::missing_parents_and_previous:
		{
			std::string msg = "missing parents or previous or links";
			item->set_local_promise(mcp::validate_status(false, msg));
			break;
		}
		default:
		{
			std::string msg = "validate error";
			item->set_local_promise(mcp::validate_status(false, msg));
			break;
		}
		}
	}

	//unhandle
	switch (result.code)
	{
	case mcp::validate_result_codes::ok:
	{
		try_process_unhandle(item);
		break;
	}
	case mcp::validate_result_codes::invalid_block:
	case mcp::validate_result_codes::parents_and_previous_include_invalid_block:
	case mcp::validate_result_codes::known_invalid_block:
	{
		try_remove_invalid_unhandle(item->block_hash);
		break;
	}
	default:
		break;
	}


	if (item->is_local())
	{
		//commit local item right now
		timeout_tx.commit_and_continue();
	}
}

void mcp::block_processor::do_process_dag_item(mcp::timeout_db_transaction & timeout_tx, std::shared_ptr<mcp::block_processor_item> item_a)
{
	std::shared_ptr<mcp::block> block(item_a->joint.block);
	mcp::db::db_transaction & transaction(timeout_tx.get_transaction());

	//todo use cache
	for (auto const & link_hash : block->links())
	{
		auto t = m_tq->get(link_hash);
		m_store.transaction_put(transaction, link_hash, t);
		m_tq->drop(link_hash);

		//////test get RAW transaction to test interface
		//dev::bytes b_value;
		//{
		//	dev::RLPStream s;
		//	t.streamRLP(s);
		//	s.swapOut(b_value);
		//}
		//LOG(m_log.info) << ":::::::::::" << toJS(b_value);
	}

	if (block->links().size() > 0)
	{
		m_store.transaction_unstable_count_add(transaction, block->links().size());
		m_store.transaction_count_add(transaction, block->links().size());
	}

	/// save block and try advance 
	m_chain->save_dag_block(timeout_tx, m_local_cache, block);
	m_chain->try_advance(timeout_tx, m_local_cache);
}

void mcp::block_processor::process_missing(std::shared_ptr<mcp::block_processor_item> item_a, std::unordered_set<mcp::block_hash> const & missings, std::unordered_set<mcp::block_hash> const & light_missings)
{
    if (m_sync->is_syncing())
        return;
	bool success(unhandle->add(item_a->block_hash, missings, light_missings, item_a));
	if (success)
	{
		//LOG(m_log.debug) << "Add unhandle:" << item_a->block_hash.to_string();

		//to request missing_parents_and_previous
		uint64_t now = m_steady_clock.now_since_epoch();
		for (auto it = missings.begin(); it != missings.end(); it++)
		{
			auto hash(std::make_shared<mcp::block_hash>(*it));
			mcp::joint_request_item request_item(item_a->remote_node_id(), hash, mcp::requesting_block_cause::new_unknown);
			m_sync->request_new_missing_joints(request_item, now);
		}
		//for (auto it = light_missings.begin(); it != light_missings.end(); it++)
		//{
		//	auto hash(std::make_shared<mcp::block_hash>(*it));
		//	mcp::joint_request_item request_item(item_a->remote_node_id(), hash, mcp::requesting_block_cause::new_unknown);
		//	m_sync->request_new_missing_joints(request_item, now);
		//}
	}
	else
	{
		process_existing_missing(item_a->remote_node_id());
	}
}

void mcp::block_processor::process_existing_missing(mcp::p2p::node_id const & remote_node_id)
{
	//remove block arrival
	auto now = std::chrono::steady_clock::now();
	if (now - m_last_request_unknown_missing_time >= std::chrono::seconds(1))
	{
		m_last_request_unknown_missing_time = now;

		size_t missings_limit = 500;
		std::vector<mcp::block_hash> missings;
		std::vector<mcp::block_hash> light_missings;
		unhandle->get_missings(missings_limit, missings, light_missings);

		if (!missings.empty())
		{
			uint64_t now = m_steady_clock.now_since_epoch();
			for (auto it = missings.begin(); it != missings.end(); it++)
			{
				auto hash(std::make_shared<mcp::block_hash>(*it));
				mcp::joint_request_item request_item(remote_node_id, hash, mcp::requesting_block_cause::existing_unknown);
				m_sync->request_new_missing_joints(request_item, now);
			}
		}

		//if (!light_missings.empty())
		//{
		//	uint64_t now = m_steady_clock.now_since_epoch();
		//	for (auto it = light_missings.begin(); it != light_missings.end(); it++)
		//	{
		//		auto hash(std::make_shared<mcp::block_hash>(*it));
		//		mcp::joint_request_item request_item(remote_node_id, hash, mcp::block_type::light, mcp::requesting_block_cause::existing_unknown);
		//		m_sync->request_new_missing_joints(request_item, now);
		//	}
		//}
	}
}

void mcp::block_processor::try_process_unhandle(std::shared_ptr<mcp::block_processor_item> item_a)
{
	std::unordered_map<mcp::block_hash, std::shared_ptr<mcp::block_processor_item>> unhandle_items = unhandle->release_dependency(item_a->block_hash);
	if (!unhandle_items.empty())
	{
		std::lock_guard<std::mutex> lock(m_process_mutex);
		for (auto const & p : unhandle_items)
		{
			std::shared_ptr<mcp::block_processor_item> u_item(p.second);
			m_blocks_pending.push_front(u_item);

            if (u_item->is_sync())
                blocks_pending_sync_size++;
		}
	}
}

void mcp::block_processor::try_remove_invalid_unhandle(mcp::block_hash const & block_hash_a)
{
	std::queue<mcp::block_hash> invalid_hashs;
	invalid_hashs.push(block_hash_a);
	while (!invalid_hashs.empty())
	{
		mcp::block_hash const &invalid_hash(invalid_hashs.front());
		invalid_hashs.pop();
		m_invalid_block_cache.add(invalid_hash);
		std::unordered_map<mcp::block_hash, std::shared_ptr<mcp::block_processor_item>> unhandle_items = unhandle->release_dependency(invalid_hash);
		for (auto const &p : unhandle_items)
		{
			mcp::block_hash const &i_hash(p.first);
			invalid_hashs.push(i_hash);
		}
	}	
}

void mcp::block_processor::on_sync_completed(mcp::p2p::node_id const & remote_node_id_a)
{
	LOG(m_log.debug) << "On sync completed";

	process_existing_missing(remote_node_id_a);
}

void mcp::block_processor::before_db_commit_event()
{
	m_local_cache->mark_as_changing();
}

void mcp::block_processor::after_db_commit_event()
{
	m_local_cache->commit_and_clear_changing();

	m_chain->update_cache();

	//ok dag promise
	while (!m_ok_local_promises.empty())
	{
		std::shared_ptr<std::promise<mcp::validate_status>> local_promise(m_ok_local_promises.front());
		local_promise->set_value(mcp::validate_status(true, ""));
		m_ok_local_promises.pop_front();
	}

	m_sync->del_hash_tree_summaries();
	//m_sync->del_catchup_indexs();

	//m_chain->notify_observers();
}

std::string mcp::block_processor::get_processor_info()
{
	std::string str = "m_blocks_pending:" + std::to_string(m_blocks_pending.size())
        + " ,m_blocks_pending_sync:" + std::to_string(blocks_pending_sync_size)
        + " ,blocks_missing_size:" + std::to_string(blocks_missing_size)
        + " ,blocks_missing_throw_size:" + std::to_string(blocks_missing_throw_size)
		+ " ,m_local_blocks_pending:" + std::to_string(m_local_blocks_pending.size())
		+ " ,m_blocks_processing:" + std::to_string(m_blocks_processing.size())
		+ " ,m_mt_blocks_pending:" + std::to_string(m_mt_blocks_pending.size())
		+ " ,m_mt_blocks_processing:" + std::to_string(m_mt_blocks_processing.size())
		+ " ,m_ok_local_dag_promises:" + std::to_string(m_ok_local_promises.size())
		+ "ok:" + std::to_string(block_processor_add)
		+ ", recent block:" + std::to_string(block_processor_recent_block_size)
		+ ", invalid:" + std::to_string(m_invalid_block_cache.size())
		+ ", block arrival, " + std::to_string(m_block_arrival->arrival.size())
		;

    blocks_missing_size = 0;
    blocks_missing_throw_size = 0;
	return str;
}

void mcp::block_processor::ongoing_retry_late_message()
{
	auto late_msg_info_list(std::move(m_late_message_cache.purge_list_ealier_than(mcp::seconds_since_epoch())));
	int count = 0;
	std::queue<std::shared_ptr<mcp::block_processor_item>> items;
	for (auto info : late_msg_info_list)
	{
		add_to_process(std::move(info.item));
	}

	m_alarm->add(std::chrono::steady_clock::now() + std::chrono::seconds(1), [this]() {
		ongoing_retry_late_message();
	});
}

std::string mcp::block_processor::get_clear_unlink_info()
{
	std::string ret = " ,m_head_clear_size:" + std::to_string(m_head_clear_size)
		+ " ,m_clear_size:" + std::to_string(m_clear_size)
		+ " ,m_head_successor_clear_size:" + std::to_string(m_head_successor_clear_size);
	
	return ret;
}

