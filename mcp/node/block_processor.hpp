#pragma once

#include <mcp/node/message.hpp>
#include <mcp/core/timeout_db_transaction.hpp>
#include <mcp/node/unhandle.hpp>
#include <mcp/node/process_block_cache.hpp>
#include <mcp/node/transaction_queue.hpp>
#include <mcp/node/approve_queue.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/node/chain.hpp>
#include <mcp/node/sync.hpp>
#include <mcp/node/node_capability.hpp>
#include <mcp/consensus/validation.hpp>
#include <mcp/common/async_task.hpp>
#include <mcp/common/alarm.hpp>

#include <chrono>
#include <queue>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace mcp
{
	class late_message_info
	{
	public:
		late_message_info(std::shared_ptr<mcp::block_processor_item> item_a);
		uint64_t timestamp;
		mcp::block_hash hash;
		std::shared_ptr<mcp::block_processor_item> item;
	};

	class late_message_cache
	{
	public:
		late_message_cache(size_t const &capacity_a = 10000);

		void add(mcp::late_message_info const &info);
		std::list<mcp::late_message_info> purge_list_ealier_than(uint64_t const &timestamp);
		size_t size() const;

	private:
		boost::multi_index_container<
			mcp::late_message_info,
			boost::multi_index::indexed_by<
			boost::multi_index::hashed_unique<boost::multi_index::member<late_message_info, mcp::block_hash, &late_message_info::hash>>,
			boost::multi_index::ordered_non_unique<boost::multi_index::member<late_message_info, uint64_t, &late_message_info::timestamp>>>>
			container;
		std::mutex container_mutex;
		size_t capacity;
	};

	class put_clear_item
	{
	public:
		put_clear_item() = default;
		put_clear_item(uint64_t const & time_a, mcp::block_hash const& hash_a):
			m_time(time_a),
			m_clear_block_hash(hash_a)
		{
		}

		uint64_t				m_time;
		mcp::block_hash			m_clear_block_hash;
	};

	class chain;
	class node_sync;
	class node_capability;
	class TransactionQueue;
	class ApproveQueue;
	class unhandle_cache;
	// Processing blocks is a potentially long IO operation
	// This class isolates block insertion from other operations like servicing network operations
	class block_processor
	{
	public:
		block_processor(bool & error_a,
			mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
			std::shared_ptr<mcp::chain> chain_a, std::shared_ptr<mcp::node_sync> sync_a,
			std::shared_ptr<mcp::node_capability> capability_a, std::shared_ptr<mcp::validation> validation_a,
			std::shared_ptr<mcp::async_task> async_task_a, std::shared_ptr<TransactionQueue> tq, std::shared_ptr<ApproveQueue> aq,
			boost::asio::io_service &io_service_a, std::shared_ptr<mcp::alarm> alarm_a
		);
		~block_processor();
		void stop();

		void add_many_to_mt_process(std::queue<std::shared_ptr<mcp::block_processor_item>> items_a);
		void add_to_mt_process(std::shared_ptr<mcp::block_processor_item> item_a);
		void add_to_process(std::shared_ptr<mcp::block_processor_item> item_a,bool retry = false);
		
		void on_sync_completed(mcp::p2p::node_id const & remote_node_id_a);

		void onTransactionReady(h256 const& _t);

		void onApproveImported(h256 const& _t);

		bool is_full();

		std::shared_ptr<mcp::unhandle_cache> unhandle;

		std::string get_processor_info();
		uint64_t dag_old_size = 0;
		uint64_t light_old_size = 0;
		uint64_t base_validate_old_size = 0;
	private:
		void add_item(std::shared_ptr<mcp::block_processor_item> item_a);

		void mt_process_blocks();

		void process_blocks();
		bool try_process_local_item_first();
		void do_process(std::deque<std::shared_ptr<mcp::block_processor_item>>& dag_blocks_processing);
		void do_process_one(std::shared_ptr<mcp::block_processor_item> item);
		void do_process_dag_item(mcp::timeout_db_transaction & timeout_tx, std::shared_ptr<mcp::block_processor_item> item_a);

		void process_missing(std::shared_ptr<mcp::block_processor_item> item_a, std::unordered_set<mcp::block_hash> const & missings, h256Hash const & transactions, h256Hash const & approves);
		void process_existing_missing(mcp::p2p::node_id const & remote_node_id);
		void try_process_unhandle(std::shared_ptr<mcp::block_processor_item> item_a);
		void try_remove_invalid_unhandle(mcp::block_hash const & block_hash_a);

		void before_db_commit_event();
		void after_db_commit_event();

		void ongoing_retry_late_message();

		mcp::block_store m_store;
		std::shared_ptr<mcp::block_cache> m_cache;
		std::shared_ptr<mcp::chain> m_chain;
		std::shared_ptr<mcp::node_sync> m_sync;
		std::shared_ptr<mcp::node_capability> m_capability;
		std::shared_ptr<mcp::validation> m_validation;
		std::shared_ptr<mcp::async_task> m_async_task;
		std::shared_ptr<mcp::alarm> m_alarm;
		std::shared_ptr<TransactionQueue> m_tq;                  ///< Maintains a list of incoming transactions not yet in a block on the blockchain.
		std::shared_ptr<ApproveQueue> m_aq;                  ///< Maintains a list of incoming approves not yet in a block on the blockchain.


		std::shared_ptr<mcp::process_block_cache> m_local_cache;
		mcp::late_message_cache m_late_message_cache;

		const uint32_t m_tx_timeout_ms = 1000;

		bool m_stopped;
		const unsigned m_max_mt_count = 16;
		std::mutex m_mt_process_mutex;
		std::condition_variable m_mt_process_condition;
		std::deque<std::shared_ptr<mcp::block_processor_item>> m_mt_blocks_pending;
		std::deque<std::shared_ptr<mcp::block_processor_item>> m_mt_blocks_processing;
		std::thread m_mt_process_block_thread;

		const unsigned m_max_pending_size = 50000;
		const unsigned m_max_local_processing_size = 1000;
		std::mutex m_process_mutex;
		std::condition_variable m_process_condition;
		std::deque<std::shared_ptr<mcp::block_processor_item>> m_local_blocks_pending;
		std::deque<std::shared_ptr<mcp::block_processor_item>> m_blocks_pending;
		std::deque<std::shared_ptr<mcp::block>> m_blocks_processing;

		///transaction ready
		std::mutex m_transaction_hashs_mutex;
		std::condition_variable m_transaction_hashs_condition;
		h256Hash m_transaction_hashs_pending;
		h256Hash m_transaction_hashs_processing;
		std::thread m_transaction_hashs_thread;
		void process_ready_transaction();

        std::atomic<uint64_t> blocks_pending_sync_size = { 0 };
        std::atomic<uint64_t> blocks_missing_size = { 0 };
        std::atomic<uint64_t> blocks_missing_throw_size = { 0 };

		std::thread m_process_block_thread;

		std::deque<std::shared_ptr<std::promise<mcp::validate_status>>> m_ok_local_promises;
		std::chrono::time_point<std::chrono::steady_clock> m_last_request_unknown_missing_time;

		//info
		std::atomic<uint64_t> block_processor_add = { 0 };
		std::atomic<uint64_t> block_processor_recent_block_size = { 0 };

		mcp::log m_log = { mcp::log("node") };
	};

} // namespace mcp
