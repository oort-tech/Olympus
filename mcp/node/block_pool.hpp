#pragma once

#include <mcp/node/common.hpp>
#include <mcp/db/database.hpp>
#include <mcp/blockstore.hpp>

#include <map>
#include <set>
#include <queue>

#include <boost/multi_index/random_access_index.hpp>

namespace mcp
{
	class light_queue_item
	{
	public:
		light_queue_item() = default;
		light_queue_item(mcp::block_processor_item const && item_a) :
			item(item_a),
			linked(false)
		{
		}

		mcp::block_processor_item item;
		bool linked;
	};

	struct unlink
	{
		mcp::block_hash latest_linked = 0; // equal previous of earliest_unlink
		mcp::block_hash earliest_unlink = 0;
		mcp::block_hash latest_unlink = 0;
	};

	class node;
	
	class block_pool
	{
	public:
		block_pool(mcp::node & node_a);

		bool add(mcp::db::db_transaction & transaction_a, std::shared_ptr<mcp::iblock_cache> cache_a, mcp::block_processor_item const & item);
		void try_get_pendings(mcp::block_hash const & missing_hash_a, std::list<mcp::block_processor_item>& pendings);
		void try_delete_pending_by_invalid_hash(mcp::block_hash const & missing_hash_a);

		std::shared_ptr<mcp::light_queue_item> get_light_queue_item(mcp::block_hash const & block_hash_a);
		std::shared_ptr<mcp::block> get_block_by_hash(mcp::block_hash const & block_hash_a);
		std::shared_ptr<std::list<mcp::block_hash>> random_get_links(mcp::uint256_t const & min_gas_price, size_t const & limit_a);
		std::shared_ptr<std::list<mcp::block_hash>> random_get_latest_unlinks(size_t const & limit);
		mcp::block_hash get_latest_block(mcp::block_type const & type_a, mcp::account const & account_a);
		void erase(mcp::block_processor_item const & item_a);
		bool is_full();

		void put_successor_change_account(mcp::account const& account_a);
		void on_successor_changes();

		size_t dag_pending_size() const { return m_dag_pending.size(); }
		size_t light_queue_size() const { return m_light_queue.size(); }
		size_t light_pending_size() const { return m_light_pending.size(); }
		size_t pending_missing_size() const { return m_dependences.size(); }
		size_t unknown_pending_missing_size() const { return m_missings.size(); }
		size_t unlink_size() const { return m_unlink.size(); }

	private:
		void add_to_dag_pending(mcp::block_processor_item const & item);
		bool add_to_light_queue(mcp::block_processor_item const & item, std::shared_ptr<mcp::block> previous_block);
		void add_to_light_pending(mcp::block_processor_item const & item);

		void update_pending_missing(mcp::block_hash const & new_block_hash_a, mcp::block_type const & new_block_type_a, std::unordered_set<mcp::block_hash> const & dependences_a, std::unordered_set<mcp::block_hash> const & missings_a);

		bool exists_in_dag_pending(mcp::block_hash const & block_hash_a);
		bool exists_in_light_queue(mcp::block_hash const & block_hash_a);
		bool exists_in_light_pending(mcp::block_hash const & block_hash_a);

		void request_missings(p2p::node_id const& id);

		std::shared_ptr<mcp::light_queue_item> get_light_queue_item_internal(mcp::block_hash const & block_hash_a);
		unlink get_unlink(mcp::account const & account_a);
		void on_successor_change(std::unordered_set<mcp::account> const & accounts_a);
		bool get_light_unlink(mcp::account const & account_a, mcp::unlink & unlink_a);

		class missing_item
		{
		public:
			missing_item(mcp::block_hash const & missing_a, mcp::block_hash const & pending_a, mcp::block_type const & pending_type_a) :
				missing(missing_a),
				pending(pending_a),
				pending_type(pending_type_a)
			{
			}

			mcp::block_hash missing;
			mcp::block_hash pending;
			mcp::block_type pending_type;
		};

		mcp::node & m_node;
		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::chrono::time_point<std::chrono::steady_clock> m_last_request_unknown_missing_time;

		std::mutex m_mutex;

		//block hash -> dag pending item
		std::unordered_map<mcp::block_hash, mcp::block_processor_item> m_dag_pending;

		//block hash -> light queue item
		std::unordered_map<mcp::block_hash, std::shared_ptr<mcp::light_queue_item>> m_light_queue;
		//block hash -> light pending item
		std::unordered_map<mcp::block_hash, mcp::block_processor_item> m_light_pending;

		//missing , pending , pending type
		boost::multi_index_container<
			missing_item,
			boost::multi_index::indexed_by<
			boost::multi_index::hashed_non_unique<boost::multi_index::member<missing_item, mcp::block_hash, &missing_item::missing>>,
			boost::multi_index::hashed_non_unique<boost::multi_index::member<missing_item, mcp::block_hash, &missing_item::pending>>
			>>
			m_dependences;

		boost::multi_index_container<
			mcp::block_hash,
			boost::multi_index::indexed_by<
			boost::multi_index::random_access<>,
			boost::multi_index::hashed_unique<boost::multi_index::identity<mcp::block_hash>>
			>>
			m_missings;

		
		std::unordered_map<mcp::account, unlink> m_unlink;

		std::unordered_map<mcp::block_hash, mcp::block_hash> m_light_successor;
		std::unordered_set<mcp::account> m_to_del_accounts;
	};
}
