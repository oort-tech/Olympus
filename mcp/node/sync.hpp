#pragma once
#include <mcp/node/node_capability.hpp>
#include <mcp/p2p/host.hpp>
#include <mcp/common/stopwatch.hpp>
#include <mcp/common/log.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/node/block_processor.hpp>

// added by michael at 1/14
#include <mcp/wallet/key_manager.hpp>

namespace mcp
{
	enum class sync_result {
		ok,
		catchup_chain_continue,
		catchup_chain_unstable_check_fail,
		catchup_chain_summary_check_fail,
		catchup_chain_stable_mci_check_fail,
		request_next_hash_tree_one_summary,
		request_next_hash_tree_no_summary
	};

	enum class sub_packet_type;
	class sync_request_status
	{
	public:
		sync_request_status() = default;
		sync_request_status(mcp::p2p::node_id const &request_node_id_a, mcp::sub_packet_type const & request_type_a);
		sync_request_status(mcp::sync_request_status const &);
		bool operator== (mcp::sync_request_status const &);
		void operator= (mcp::sync_request_status const &);
		mcp::p2p::node_id     request_node_id;
		mcp::sub_packet_type  request_type;
	};
	enum class sync_status
	{
		ok,
		pending,
		syncing
	};

	class sync_info
	{
	public:
		sync_info() { clear(); }
		void clear();
		void set_info(mcp::summary_hash const & from_summary, mcp::summary_hash const & to_summary, uint64_t const & index_a);

		uint64_t new_unknown_joint = 0;
		uint64_t peer_info_joint = 0;
		uint64_t existing_unknown_joint = 0;

		mcp::summary_hash	request_hash_tree_from_summary = 0;
		mcp::summary_hash	request_hash_tree_to_summary = 0;
		uint64_t			request_hash_tree_start_index = 0;
		mcp::block_hash		last_stable_block_expected;
		std::list<joint_message> unstable_mc_joints;

		uint64_t			index = 0;		//sync current form summary
		bool				first = true;
		uint64_t			max_index = 0;	//synced max index

		std::map<uint64_t, uint64_t> catchup_del_index;
		uint64_t			current_del_catchup = 0;
		std::unordered_map<mcp::block_hash, uint64_t> to_summary_index;
		p2p::node_id id = 0;
		uint64_t			del_catchup_index = 0;
		uint64_t			version = 0;
		std::mutex			version_mutex;
	};

	class node_sync
	{
	public:
		node_sync(
			std::shared_ptr<mcp::node_capability> capability_a, mcp::block_store& store_a,
			std::shared_ptr<mcp::chain> chain_a, std::shared_ptr<mcp::block_cache> cache_a,
			std::shared_ptr<mcp::async_task> async_task_a,
			mcp::fast_steady_clock& steady_clock_a, boost::asio::io_service & io_service_a,
			// added by michael at 1/14
			std::shared_ptr<mcp::key_manager> key_manager
		);
		~node_sync() { stop(); }
		void set_processor(std::shared_ptr<mcp::block_processor> block_processor_a) { m_block_processor = block_processor_a; }
		static bool is_syncing() 
		{ 
			return m_status == mcp::sync_status::syncing; 
		}
		static std::string get_syncing_status()
		{
			std::string ret = "";
			if (m_status == mcp::sync_status::ok)
				ret = "ok";
			else if (m_status == mcp::sync_status::pending)
				ret = "pending";
			else if (m_status == mcp::sync_status::syncing)
				ret = "syncing";

			for (auto it = m_request_info.to_summary_index.begin(); it != m_request_info.to_summary_index.end(); it++)
			{
				ret = ret + " ," + it->first.to_string() + " ," + std::to_string(it->second);
			}

			return ret;
		}
		mcp::sync_request_hash get_current_request_id() { return m_current_request_id; }
		void request_catchup(p2p::node_id const& id);
		void catchup_chain_request_handler(p2p::node_id const& id, mcp::catchup_request_message const& request);
		bool response_for_sync_request(p2p::node_id const & request_node_id_a, mcp::sub_packet_type const & request_type_a);
		void catchup_chain_response_handler(p2p::node_id const& id, mcp::catchup_response_message const& response);
		void hash_tree_request_handler(p2p::node_id const& id, mcp::hash_tree_request_message const& message);
		void hash_tree_response_handler(p2p::node_id const &, mcp::hash_tree_response_message const &);

		void peer_info_request_handler(p2p::node_id const &);
		void request_new_missing_joints(mcp::joint_request_item& item_a, uint64_t& millisecondsSinceEpoch, bool const& is_timeout = false);

		void joint_request_handler(p2p::node_id const &, mcp::joint_request_message const &);
		void send_peer_info_request(p2p::node_id id);
		void send_peer_info(p2p::node_id const &, mcp::peer_info_message const &);

		void del_hash_tree_summaries();
		void put_hash_tree_summaries(mcp::summary_hash const& hash);

		void del_catchup_indexs();
		void deal_exist_catchup_index(mcp::block_hash const& hash_a);
		
		void stop();

		std::string get_sync_info();
	private:
		void request_remote_mc(mcp::db::db_transaction & transaction_a, p2p::node_id const& id, mcp::summary_hash const& from_summary, mcp::block_hash const& unstable_tail_block);
		void send_catchup_request(p2p::node_id const& id, mcp::catchup_request_message const& message);
		void prepare_catchup_chain(mcp::catchup_request_message const& request, mcp::catchup_response_message & response);
		void send_catchup_response(p2p::node_id const& id, mcp::catchup_response_message const& message);
		mcp::sync_result process_catchup_chain(mcp::catchup_response_message const& catchup_chain);
		void request_catchup_second(p2p::node_id const &id);
		mcp::sync_result request_next_hash_tree(p2p::node_id const& id, uint64_t const & next_start_index = 0);
		void send_hash_tree_request(p2p::node_id const& id, mcp::hash_tree_request_message const& message);
		void read_hash_tree(mcp::hash_tree_request_message const& hash_tree_request, mcp::hash_tree_response_message & hash_tree_response);
		void send_hash_tree_response(p2p::node_id const& id, mcp::hash_tree_response_message const& message);
		void process_hash_tree(p2p::node_id const &, mcp::hash_tree_response_message const &);

		void send_block(p2p::node_id const & id, mcp::joint_message const & message);

		bool is_request_hash_tree();
		bool check_summaries_exist(mcp::db::db_transaction &transaction, std::list<mcp::summary_hash> const& summaries);
		bool check_summaries_exist(mcp::db::db_transaction &transaction, std::set<mcp::summary_hash> const& summaries);
		void add_hash_tree_summary(mcp::timeout_db_transaction & tx_a, mcp::summary_hash const & summary_a);

		void purge_handled_summaries_from_hash_tree();
		size_t get_block_from_free(mcp::db::forward_iterator & it, const unsigned get_max_count, std::vector<mcp::block_hash> & get_result);
		size_t move_free_iterator_random(mcp::db::forward_iterator & it, const unsigned& free_count, const unsigned& max_cap);

		void add_task_sync_request_timer(p2p::node_id const & request_node_id_a, mcp::sub_packet_type const & request_type_a);

		void process_request_joints();
		void send_joint_request(p2p::node_id const &, mcp::joint_request_message const &);

		void clear_catchup_info(bool lock = true);
		void del_catchup_index(std::map<uint64_t, uint64_t> const& map_a);
        void del_catchup_index(uint64_t index);

		void del_hash_tree_summary(std::list<mcp::summary_hash> const & summaries_a);
		std::list<mcp::summary_hash> m_to_del_hash_tree_summaries;

		std::shared_ptr<mcp::node_capability> m_capability;
		mcp::block_store & m_store;
		std::shared_ptr<mcp::chain> m_chain;
		std::shared_ptr<mcp::block_cache> m_cache;
		std::shared_ptr<mcp::block_processor> m_block_processor;
		std::shared_ptr<mcp::async_task> m_async_task;
		mcp::fast_steady_clock& m_steady_clock;

		static sync_info m_request_info;

		std::map<uint64_t, mcp::sync_request_status> m_sync_requests;
		std::atomic<uint64_t> m_sync_request_id = { 0 };
		mcp::sync_request_hash m_current_request_id = 0;
		mcp::catchup_request_message m_current_catchup_request;
		std::unique_ptr<boost::asio::deadline_timer> m_sync_timer;
		std::unique_ptr<boost::asio::deadline_timer> m_sync_request_timer;

		//thread 
		std::deque<mcp::joint_request_item> m_joint_request_pending;
		std::mutex m_mutex_joint_request;
		std::condition_variable m_condition;
		std::thread m_request_joints_thread;

		mcp::log log_sync = { mcp::log("sync") };

		std::mutex m_del_catchup_mutex;

		std::atomic<bool> m_task_clear_flag;
		static std::atomic<sync_status> m_status;
		bool m_stoped;

		// added by michael at 1/14
		std::shared_ptr<mcp::key_manager> m_key_manager;
	};
}