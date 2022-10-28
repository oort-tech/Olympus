#pragma once

#include "common.hpp"
#include <mcp/node/message.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/core/block_cache.hpp>
#include <mcp/p2p/capability.hpp>
#include <mcp/p2p/peer.hpp>
#include <mcp/node/sync.hpp>
#include <mcp/common/async_task.hpp>
#include <mcp/node/arrival.hpp>

#include <thread>

namespace mcp
{
	class peer_info
	{
	public:
		peer_info(std::shared_ptr<p2p::peer> peer_a, unsigned const & offset_a):
			peer(peer_a),
			offset(offset_a),
			known_blocks(100),
			known_transactions(10000),
			known_approves(10000),
			last_hanlde_peer_info_request_time(std::chrono::steady_clock::now()),
			last_peer_info_request_time(std::chrono::steady_clock::now())
		{
		}
		
		std::shared_ptr<p2p::peer> try_lock_peer() const
		{
			return peer.lock();
		}

		bool is_known_block(mcp::block_hash const & block_hash_a) const 
		{
			return known_blocks.contains(block_hash_a);
		}

		void mark_as_known_block(mcp::block_hash const & block_hash_a)
		{
			known_blocks.add(block_hash_a);
		}

		bool is_known_transaction(h256 const & hash_a) const
		{
			return known_transactions.contains(hash_a);
		}

		void mark_as_known_transaction(h256 const & hash_a)
		{
			known_transactions.add(hash_a);
		}

		int size() { return known_transactions.size(); }

		bool is_known_approve(h256 const & hash_a) const
		{
			return known_approves.contains(hash_a);
		}

		void mark_as_known_approve(h256 const & hash_a)
		{
			known_approves.add(hash_a);
		}

		std::weak_ptr<p2p::peer> peer;
		unsigned offset;
		std::chrono::steady_clock::time_point last_hanlde_peer_info_request_time;
		std::chrono::steady_clock::time_point last_peer_info_request_time;

	private:
		mcp::mru_list<mcp::block_hash, std::mutex> known_blocks;
		mcp::mru_list<h256, std::mutex> known_transactions;
		mcp::mru_list<h256, std::mutex> known_approves;
	};

    class local_remote_ack_hello {   
    public:
        local_remote_ack_hello() :r_l_result(false), l_r_result(false){}
        bool r_l_result;
        bool l_r_result;
    };

	//class node_capability;
	class requesting_mageger
	{
	public:
		requesting_mageger(){}
		bool add(mcp::requesting_item& item_a, bool const& count_a = false);
		bool exist_erase(mcp::sync_request_hash const& request_id_a);
		void erase(mcp::block_hash const& hash_a);
		bool exist(mcp::block_hash const& hash_a);
		std::list<requesting_item> clear_by_time(uint64_t const& time_a);
		uint64_t size() { return m_request_info.size(); }

		std::string get_info();
	private:
		boost::multi_index_container<
			mcp::requesting_item,
			boost::multi_index::indexed_by<
				boost::multi_index::hashed_unique<boost::multi_index::member<mcp::requesting_item, mcp::sync_request_hash, &mcp::requesting_item::m_request_id> >,
				boost::multi_index::hashed_unique<boost::multi_index::member<mcp::requesting_item ,mcp::block_hash, &mcp::requesting_item::m_request_hash> >,
				boost::multi_index::hashed_non_unique<boost::multi_index::member<mcp::requesting_item, uint64_t, &mcp::requesting_item::m_time> >
			>
		> m_request_info;
		//uint64_t m_timeout;//internal used ,if retry 3 times not get response will be throw away
		std::atomic<uint64_t> m_random_uint = { 0 };  //random number, create request id

		static const int STALLED_TIMEOUT = 5000; //retry time,external used
		static const int RETYR_TIMES = 3;

		//info
		uint64_t arrival_filter_count = 0;
	};

    class requesting_block_info
    {
    public:
        requesting_block_info(mcp::p2p::node_id const & r_node_a, uint64_t const r_time_a, mcp::requesting_block_cause const & r_cause_a):
            m_r_node(r_node_a), m_r_time(r_time_a), m_r_cause(r_cause_a), m_r_count(0)
        {}
        requesting_block_info() = default;
        mcp::p2p::node_id m_r_node;
        uint64_t m_r_time;
        mcp::requesting_block_cause m_r_cause;
        uint32_t m_r_count;
        const static uint32_t  max_request_count = 2;
    };

	class block_processor;
	class TransactionQueue;
	class ApproveQueue;
	class node_sync;
	class node_capability : public p2p::icapability
	{
		friend class node_sync;
	public:
		node_capability(boost::asio::io_service &io_service_a, mcp::block_store& store_a,
			mcp::fast_steady_clock& steady_clock_a, std::shared_ptr<mcp::block_cache> cache_a,
			std::shared_ptr<mcp::async_task> async_task_a, std::shared_ptr<mcp::block_arrival> block_arrival_a,
			std::shared_ptr<mcp::TransactionQueue> tq,
			std::shared_ptr<mcp::ApproveQueue> aq
		);
		void set_sync(std::shared_ptr<mcp::node_sync> sync_a) { m_sync = sync_a; }
		void set_processor(std::shared_ptr<mcp::block_processor> block_processor_a) { m_block_processor = block_processor_a; }

		~node_capability() { stop(); }
		void on_connect(std::shared_ptr<p2p::peer> peer_a, unsigned const & offset);
		void on_disconnect(std::shared_ptr<p2p::peer> peer_a);
		bool read_packet(std::shared_ptr<p2p::peer> peer_a, unsigned const & type, std::shared_ptr<dev::RLP> r);
		void broadcast_block(mcp::joint_message const & message);
		void broadcast_transaction(mcp::Transaction const & message);
		void broadcast_approve(mcp::approve const & message);
		void mark_as_known_block(p2p::node_id node_id_a, mcp::block_hash block_hash_a);
		void mark_as_known_transaction(p2p::node_id node_id_a, h256 _h);

        bool check_remotenode_hello(mcp::block_hash const & block_hash_a);
        bool is_local_remote_ack_ok_hello(p2p::node_id node_id_a);
        void confirm_remote_connect(p2p::node_id node_id_a);
        void set_local_ack_hello(p2p::node_id node_id_a,bool);
        void set_remote_ack_hello(p2p::node_id node_id_a,bool);
        bool is_remote_in_waitting_hello(p2p::node_id node_id_a);
        void timeout_for_ack_hello(p2p::node_id node_id_a);
        //
        bool is_peer_exsist(p2p::node_id const &id);

        //sync
        static const int COLLECT_PEER_INFO_INTERVAL = 10 * 1000; //ms
		const boost::posix_time::milliseconds no_joint_interval = boost::posix_time::milliseconds(10000);

        void send_hello_info(p2p::node_id const & );
        void send_hello_info_request(p2p::node_id const &);
        void send_hello_info_ack(p2p::node_id const &);

		void stop();

		uint64_t num_peers();

        //std::unordered_map<p2p::node_id, std::shared_ptr<mcp::capability_metrics>> m_node_id_cap_metrics;
		mcp::requesting_mageger m_requesting;
		std::mutex m_requesting_lock;

        uint64_t del_joint_in_asso_count = 0;
        uint64_t add_joint_in_asso_count = 0;    

		std::atomic<uint64_t> receive_joint_request_count = { 0 };
		std::atomic<uint64_t> receive_catchup_request_count = { 0 };
		std::atomic<uint64_t> receive_catchup_response_count = { 0 };
		std::atomic<uint64_t> receive_hash_tree_request_count = { 0 };
		std::atomic<uint64_t> receive_hash_tree_response_count = { 0 };
		std::atomic<uint64_t> receive_peer_info_request_count = { 0 };
		std::atomic<uint64_t> receive_peer_info_count = { 0 };
    private:
		/// transaction processed callback
		void onTransactionImported(ImportResult _ir, p2p::node_id const& _nodeId);
		void onApproveImported(ImportApproveResult _ir, h256 const& _h, p2p::node_id const& _nodeId);

		std::unordered_map<p2p::node_id, mcp::peer_info> m_peers;
		std::mutex m_peers_mutex;
		bool m_stopped;
        mcp::block_hash  m_genesis;
        std::map<p2p::node_id,mcp::local_remote_ack_hello> m_wait_confirm_remote_node;
        std::unordered_map<p2p::node_id, std::unique_ptr<boost::asio::deadline_timer>> m_sync_peer_info_request_timer;
        
		//block request timeout
		std::unique_ptr<boost::asio::deadline_timer> m_request_timer;
		std::atomic_flag m_request_associng = ATOMIC_FLAG_INIT;
		void request_block_timeout();

		mcp::log m_log = { mcp::log("p2p") };

		boost::asio::io_service& m_io_service;
		mcp::block_store m_store;
		mcp::fast_steady_clock& m_steady_clock;
		std::shared_ptr<mcp::block_cache> m_cache;
		std::shared_ptr<mcp::node_sync> m_sync;
		std::shared_ptr<mcp::block_processor> m_block_processor;
		std::shared_ptr<mcp::async_task> m_async_task;
		std::shared_ptr<mcp::block_arrival> m_block_arrival;
		std::shared_ptr<TransactionQueue> m_tq;                  ///< Maintains a list of incoming transactions not yet in a block on the blockchain.
		std::shared_ptr<ApproveQueue> m_aq;
	};
}
