#include "block_processor.hpp"
#include "arrival.hpp"
#include <mcp/common/stopwatch.hpp>
#include <mcp/core/genesis.hpp>
#include <libdevcore/CommonJS.h>

constexpr uint32_t tx_timeout_ms = 1000;
constexpr unsigned max_mt_count = 16;
constexpr unsigned max_pending_size = 5000;
constexpr unsigned max_local_processing_size = 100;

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
	std::shared_ptr<mcp::async_task> async_task_a, std::shared_ptr<TransactionQueue> tq, std::shared_ptr<ApproveQueue> aq,
	boost::asio::io_service &io_service_a, std::shared_ptr<mcp::alarm> alarm_a
):
	m_store(store_a),
	m_cache(cache_a),
	m_chain(chain_a),
	m_sync(sync_a),
	m_capability(capability_a),
	m_validation(validation_a),
	m_async_task(async_task_a),
	m_tq(tq),
	m_aq(aq),
	m_alarm(alarm_a),
	m_stopped(false),
	m_local_cache(std::make_shared<process_block_cache>(cache_a, store_a, tq, aq)),
	m_last_request_unknown_missing_time(std::chrono::steady_clock::now()),
	unhandle(std::make_shared<mcp::unhandle_cache>(tq, aq))
{
	if (error_a)
		return;

	std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
	std::shared_ptr<rocksdb::TransactionOptions> tx_option(mcp::db::db_transaction::default_trans_options());
	tx_option->skip_concurrency_control = true;

	mcp::timeout_db_transaction timeout_tx(m_store, tx_timeout_ms, write_option, tx_option,
		std::bind(&block_processor::before_db_commit_event, this),
		std::bind(&block_processor::after_db_commit_event, this));
	try
	{
		m_chain->init(error_a, timeout_tx, m_local_cache/*, m_cache*/);
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

	m_tq->onReady([this](h256 const& _h) { onTransactionReady(_h); });
	m_aq->onReady([this](h256 const& _h) { onApproveImported(_h); });

	m_mt_process_block_thread = std::thread([this]() { this->mt_process_blocks(); });
	m_process_block_thread = std::thread([this]() { this->process_blocks(); });
	m_ready_hashs_thread = std::thread([this]() { this->process_ready_func(); });

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

	{
		std::lock_guard<std::mutex> lock(m_ready_hashs_mutex);
		m_ready_hashs_condition.notify_all();
	}

	if (m_mt_process_block_thread.joinable())
		m_mt_process_block_thread.join();
	if (m_process_block_thread.joinable())
		m_process_block_thread.join();
	if (m_ready_hashs_thread.joinable())
		m_ready_hashs_thread.join();
}

bool mcp::block_processor::is_full()
{
	return m_blocks_pending.size() >= max_pending_size;
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

	///throw if queue is full
	if (item_a->is_broadCast() && (is_full() || unhandle->is_full() || BlockArrival.recent(block_hash)))
		return;

	///broadcase,request,loacl
	if (!item_a->is_sync())
	{
		if (m_sync->is_syncing())
		{
			//if (item_a->is_broadCast() || item_a->is_local())///throw broadcast and local block if syncing.
				return;
		}
		if (item_a->joint.summary_hash == mcp::summary_hash(0))
			BlockArrival.add(block_hash);
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
		while (!items_a.empty() && count < max_mt_count)
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
					unsigned count = std::min((unsigned)m_mt_blocks_processing.size(), max_mt_count);
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
										err_msg = boost::str(boost::format("Exec timestamp too late, block: %1%, exec_timestamp: %2%, sys_timestamp: %3%") % block_hash.hex() % block->exec_timestamp() % mcp::seconds_since_epoch());
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
									ok = false;
									base_validate_old_size++;
									break;
								}
								case base_validate_result_codes::invalid_signature:
								{
									ok = false;
									err_msg = "Invalid signature, hash:" + block_hash.hex() + ",from:" + block->from().hexPrefixed() + ",signature:" + ((Signature)block->signature()).hex();
									LOG(m_log.debug) << err_msg;
									if (!item->is_local())
										m_onImport(ImportResult::Malformed, item->remote_node_id());///  Notify capability and P2P to process peer. diconnect peer.  
									
									break;
								}
								case base_validate_result_codes::invalid_block:
								{
									ok = false;
									err_msg = boost::str(boost::format("Invalid block: %1%, error message: %2%") % block->hash().hex() % result.err_msg);
									LOG(m_log.debug) << err_msg;

									//cache invalid block
									InvalidBlockCache.add(block_hash);
									if (!item->is_local())
										m_onImport(ImportResult::Malformed, item->remote_node_id());///  Notify capability and P2P to process peer. diconnect peer.  
									break;
								}
								case base_validate_result_codes::known_invalid_block:
								{
									ok = false;
									err_msg = boost::str(boost::format("Know invalid block: %1%") % block->hash().hex());
									LOG(m_log.trace) << err_msg;
									if (!item->is_local())
										m_onImport(ImportResult::Malformed, item->remote_node_id());///  Notify capability and P2P to process peer. diconnect peer.  
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

void mcp::block_processor::add_to_process(std::shared_ptr<mcp::block_processor_item> item_a, bool ready)
{
	if (!item_a->is_sync() && item_a->joint.summary_hash != mcp::summary_hash(0))
	{
		assert_x(!item_a->is_local());
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
		if (ready)
			m_blocks_pending.push_front(item_a);
		else
			m_blocks_pending.push_back(item_a);
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

		mcp::timeout_db_transaction timeout_tx(m_store, tx_timeout_ms, write_option, tx_option,
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
			while (to_processing.size() < max_local_processing_size)
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

					m_blocks_pending.pop_front();
				}
			}
		}

		if (!to_processing.empty())
		{
			lock.unlock();
			{
				mcp::stopwatch_guard sw("process_blocks");
				if (!to_processing.empty())
				{
					do_process(to_processing);
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

bool mcp::block_processor::try_process_local_item_first()
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
			do_process_one(local_item);
	}
	return has_local;
}

void mcp::block_processor::do_process(std::deque<std::shared_ptr<mcp::block_processor_item>> & blocks_processing)
{
	//mcp::stopwatch_guard sw("process_blocks: do_process");

	while (!blocks_processing.empty())
	{
		if (m_stopped)
			break;

		std::shared_ptr<mcp::block_processor_item> item(blocks_processing.front());

		if (!item->is_local())
		{
			bool has_local(try_process_local_item_first());
			if (has_local)
				continue;
		}

		blocks_processing.pop_front();
		do_process_one(item);
	}
}

void mcp::block_processor::do_process_one(std::shared_ptr<mcp::block_processor_item> item)
{
	mcp::joint_message const & joint(item->joint);
	std::shared_ptr<mcp::block> block(joint.block);
	mcp::block_hash const & block_hash(block->hash());

	if (unhandle->exists(block_hash))
	{
		process_existing_missing(item->remote_node_id());
		return;
	}

	std::shared_ptr<rocksdb::WriteOptions> write_option(mcp::db::database::default_write_options());
	std::shared_ptr<rocksdb::TransactionOptions> tx_option(mcp::db::db_transaction::default_trans_options());
	tx_option->skip_concurrency_control = true;
	std::shared_ptr<mcp::chain> chain(m_chain);
	mcp::timeout_db_transaction timeout_tx(m_store, tx_timeout_ms, write_option, tx_option,
		std::bind(&block_processor::before_db_commit_event, this),
		std::bind(&block_processor::after_db_commit_event, this));

	try
	{
		mcp::db::db_transaction & transaction(timeout_tx.get_transaction());
		//dag validate
		mcp::validate_result result(m_validation->dag_validate(transaction, m_local_cache, joint));

		//LOG(m_log.info) << "do_process_one, block:" << block_hash.hex() << " ,result:" << int(result.code);

		switch (result.code)
		{
		case mcp::validate_result_codes::ok:
		{
			//broadcast
			if (!item->is_local() && !item->is_sync() && item->joint.summary_hash == mcp::summary_hash(0))
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
			LOG(m_log.trace) << boost::str(boost::format("Old block: %1%") % block_hash.hex());
			break;
		}
		case mcp::validate_result_codes::missing_parents_and_previous:
		{
			assert_x(!(item->is_local() && result.missing_links.size() > 0));
			assert_x(!item->is_sync());
			if (result.missing_parents_and_previous.size() > 0 || result.missing_links.size() > 0 || result.missing_approves.size() > 0)
			{
				if (result.missing_parents_and_previous.size() > 0)
				{
					assert_x(!item->is_local());
				}
				process_missing(item, result.missing_parents_and_previous, result.missing_links, result.missing_approves);
			}

			LOG(m_log.trace) << boost::str(boost::format("Missing parents and previous for: %1%") % block_hash.hex());

			break;
		}
		case mcp::validate_result_codes::invalid_block:
		{
			LOG(m_log.info) << boost::str(boost::format("Invalid block: %1%, error message: %2%") % block_hash.hex() % result.err_msg);
			assert_x(!item->is_local());
			//cache invalid block
			InvalidBlockCache.add(block_hash);
			break;
		}
		case mcp::validate_result_codes::parents_and_previous_include_invalid_block:
		{
			LOG(m_log.info) << boost::str(boost::format("Invalid block: %1%, error message: %2%") % block_hash.hex() % result.err_msg);
			assert_x(!item->is_local());
			//cache invalid block
			InvalidBlockCache.add(block_hash);
			break;
		}
		case mcp::validate_result_codes::known_invalid_block:
		{
			LOG(m_log.trace) << boost::str(boost::format("Known invalid block: %1%") % block_hash.hex());
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

			/// Notify capability and P2P to process peer. diconnect peer.  
			/// must not a local block.remote node id existed.
			m_onImport(ImportResult::Malformed, item->remote_node_id());
			break;
		}
		default:
			break;
		}

		timeout_tx.commit();
	}
	catch (std::exception const &e)
	{
		LOG(m_log.error) << "Block process do_process_one error: " << e.what() << std::endl << boost::stacktrace::stacktrace();
		timeout_tx.rollback();
		throw;
	}
	catch (...)
	{
		LOG(m_log.error) << "Block process do_process_one unknown error." << std::endl << boost::stacktrace::stacktrace();
		timeout_tx.rollback();
		throw;
	}
}

void mcp::block_processor::do_process_dag_item(mcp::timeout_db_transaction & timeout_tx, std::shared_ptr<mcp::block_processor_item> item_a)
{
	std::shared_ptr<mcp::block> block(item_a->joint.block);
	mcp::db::db_transaction & transaction(timeout_tx.get_transaction());

	mcp::block_hash const & block_hash(block->hash());
	for (auto const & link_hash : block->links())
	{
		/// Unprocessed transactions cannot be discarded because the cache is full.
		auto t = m_tq->get(link_hash);
		if (t == nullptr || m_local_cache->transaction_exists(transaction, link_hash)) /// transaction maybe processed yet
		{
			continue;
		}
		m_chain->save_transaction(timeout_tx, m_local_cache, t);
	}

	for (auto const & approve_hash : block->approves())
	{
		/// Unprocessed transactions cannot be discarded because the cache is full.
		auto t = m_aq->get(approve_hash);
		if (t == nullptr || m_local_cache->approve_exists(transaction, approve_hash)) /// transaction maybe processed yet
		{
			continue;
		}
		m_chain->save_approve(timeout_tx, m_local_cache, t);
	}

	/// save block and try advance 
	m_chain->save_dag_block(timeout_tx, m_local_cache, block);
	m_chain->try_advance(timeout_tx, m_local_cache);
}

void mcp::block_processor::process_missing(std::shared_ptr<mcp::block_processor_item> item_a, std::unordered_set<mcp::block_hash> const & missings, h256Hash const & transactions, h256Hash const & approves)
{
	unhandle_add_result r(unhandle->add(item_a, missings, transactions, approves));
	/// if the block links too much,request_catchup exec before this function, modify_syncing status, ensures that the block's missing transactions are requested.
	if (m_sync->is_syncing())
		return;
	if (r == unhandle_add_result::Nothing)
	{
		return;
	}
	if (r == unhandle_add_result::Success)
	{
		//to request missing_parents_and_previous
		uint64_t now = SteadyClock.now_since_epoch();
		for (auto it = missings.begin(); it != missings.end(); it++)
		{
			//LOG(m_log.info) << "process_missing, block:" << item_a->block_hash.hex() << " ,parent:" << (*it).hex();
			mcp::requesting_item request_item(item_a->remote_node_id(), *it, mcp::requesting_block_cause::new_unknown, now);
			m_sync->request_new_missing_joints(request_item);
		}
		for (auto it = transactions.begin(); it != transactions.end(); it++)
		{
			//LOG(m_log.info) << "process_missing, block:" << item_a->block_hash.hex() << " ,transaction:" << (*it).hex();
			mcp::requesting_item request_item(item_a->remote_node_id(), *it, mcp::requesting_block_cause::new_unknown, now);
			m_sync->request_new_missing_transactions(request_item);
		}
		for (auto it = approves.begin(); it != approves.end(); it++)
		{
			//LOG(m_log.info) << "process_missing, block:" << item_a->block_hash.hex() << " ,approve:" << (*it).hex();
			mcp::requesting_item request_item(item_a->remote_node_id(), *it, mcp::requesting_block_cause::new_unknown, now);
			m_sync->request_new_missing_approves(request_item);
		}
	}
	else if (r == unhandle_add_result::Exist)
	{
		process_existing_missing(item_a->remote_node_id());
	}
	else if (r == unhandle_add_result::Retry)
	{
		m_blocks_pending.push_front(item_a);
	}
}

void mcp::block_processor::process_existing_missing(mcp::p2p::node_id const & remote_node_id)
{
	if (m_sync->is_syncing())
		return;
	//remove block arrival
	auto now = std::chrono::steady_clock::now();
	if (now - m_last_request_unknown_missing_time >= std::chrono::seconds(1))
	{
		m_last_request_unknown_missing_time = now;

		size_t missings_limit = 500;
		std::vector<mcp::block_hash> missings;
		std::vector<h256> light_missings;
		std::vector<h256> approve_missings;
		unhandle->get_missings(missings_limit, missings, light_missings, approve_missings);

		if (!missings.empty())
		{
			//LOG(m_log.info) << "[process_existing_missing] missings.size=" << missings.size();
			uint64_t now = SteadyClock.now_since_epoch();
			for (auto it = missings.begin(); it != missings.end(); it++)
			{
				//LOG(m_log.info) << "[process_existing_missing] parent:" << (*it).hex();
				mcp::requesting_item request_item(remote_node_id, *it, mcp::requesting_block_cause::existing_unknown, now);
				m_sync->request_new_missing_joints(request_item);
			}
		}

		if (!light_missings.empty())
		{
			//LOG(m_log.info) << "[process_existing_missing] transaction.size=" << light_missings.size();
			uint64_t now = SteadyClock.now_since_epoch();
			for (auto it = light_missings.begin(); it != light_missings.end(); it++)
			{
				//LOG(m_log.info) << "[process_existing_missing] transaction:" << (*it).hex();
				mcp::requesting_item request_item(remote_node_id, *it, mcp::requesting_block_cause::existing_unknown, now);
				m_sync->request_new_missing_transactions(request_item);
			}
		}

		if (!approve_missings.empty())
		{
    		//LOG(m_log.info) << "[process_existing_missing] approve_missings.size="<<approve_missings.size();
			uint64_t now = SteadyClock.now_since_epoch();
			for (auto it = approve_missings.begin(); it != approve_missings.end(); it++)
			{
				mcp::requesting_item request_item(remote_node_id, *it, mcp::requesting_block_cause::existing_unknown, now);
				m_sync->request_new_missing_approves(request_item);
			}
		}
	}
}

void mcp::block_processor::try_process_unhandle(std::shared_ptr<mcp::block_processor_item> item_a)
{
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> unhandle_items = unhandle->release_dependency(item_a->block_hash);
	if (!unhandle_items.empty())
	{
		std::lock_guard<std::mutex> lock(m_process_mutex);
		for (auto const & p : unhandle_items)
		{
			std::shared_ptr<mcp::block_processor_item> u_item(p);
			m_blocks_pending.push_front(u_item);
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
		InvalidBlockCache.add(invalid_hash);
		std::unordered_set<std::shared_ptr<mcp::block_processor_item>> unhandle_items = unhandle->release_dependency(invalid_hash);
		for (auto const &p : unhandle_items)
		{
			mcp::block_hash const &i_hash(p->block_hash);
			invalid_hashs.push(i_hash);
		}
	}	
}

void mcp::block_processor::on_sync_completed(mcp::p2p::node_id const & remote_node_id_a)
{
	LOG(m_log.debug) << "On sync completed";

	process_existing_missing(remote_node_id_a);
}

void mcp::block_processor::onTransactionReady(h256 const& _t)
{
	std::lock_guard<std::mutex> lock(m_ready_hashs_mutex);
	m_ready_hashs_pending.insert(std::make_pair(_t, hashType::Transaction));
	m_ready_hashs_condition.notify_all();
}

void mcp::block_processor::process_ready_func()
{
	std::unique_lock<std::mutex> lock(m_ready_hashs_mutex);
	while (!m_stopped)
	{
		if (!m_ready_hashs_pending.empty())
		{
			mcp::stopwatch_guard sw("process_ready_func");

			std::swap(m_ready_hashs_processing, m_ready_hashs_pending);
			lock.unlock();

			h256Hash ts;
			h256Hash as;
			for (auto it : m_ready_hashs_processing)
			{
				if (it.second)///approve
					as.insert(it.first);
				else
					ts.insert(it.first);
			}

			if (!ts.empty())
			{
				std::unordered_set<std::shared_ptr<mcp::block_processor_item>> unhandle_items = unhandle->release_transaction_dependency(ts);
				if (!unhandle_items.empty())
				{
					for (auto const & p : unhandle_items)
					{
						add_to_process(p,true);
					}
				}
			}
			if (!as.empty())
			{
				std::unordered_set<std::shared_ptr<mcp::block_processor_item>> unhandle_items = unhandle->release_approve_dependency(as);
				if (!unhandle_items.empty())
				{
					for (auto const & p : unhandle_items)
					{
						add_to_process(p, true);
					}
				}
			}
			
			m_ready_hashs_processing.clear();
			lock.lock();
		}
		else
		{
			m_ready_hashs_condition.wait(lock);
		}
	}
}

void mcp::block_processor::onApproveImported(h256 const& _t)
{
	std::lock_guard<std::mutex> lock(m_ready_hashs_mutex);
	m_ready_hashs_pending.insert(std::make_pair(_t, hashType::Approve));
	m_ready_hashs_condition.notify_all();
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

	//m_chain->notify_observers();
}

std::string mcp::block_processor::get_processor_info()
{
	std::string str = "m_blocks_pending:" + std::to_string(m_blocks_pending.size())
		+ " ,m_local_blocks_pending:" + std::to_string(m_local_blocks_pending.size())
		+ " ,m_mt_blocks_pending:" + std::to_string(m_mt_blocks_pending.size())
		+ " ,m_mt_blocks_processing:" + std::to_string(m_mt_blocks_processing.size())
		+ " ,m_ok_local_dag_promises:" + std::to_string(m_ok_local_promises.size())
		+ " ,ok:" + std::to_string(block_processor_add)
		+ ", invalid:" + std::to_string(InvalidBlockCache.size())
		+ ", block arrival: " + std::to_string(BlockArrival.arrival.size())
		+ ", dag_old_size: " + std::to_string(dag_old_size)
		+ ", base_validate_old_size: " + std::to_string(base_validate_old_size)
		;

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


