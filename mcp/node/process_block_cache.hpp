#pragma once

#include <mcp/core/block_store.hpp>
#include <mcp/core/block_cache.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

namespace mcp
{
	class process_block_cache : public mcp::iblock_cache
	{
	public:
		explicit process_block_cache(std::shared_ptr<mcp::block_cache> cache_a, mcp::block_store & store_a);

		bool block_exists(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a);
		std::shared_ptr<mcp::block> block_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a);
		void block_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::shared_ptr<mcp::block> block_a);

		std::shared_ptr<mcp::block_state> block_state_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a);
		void block_state_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::shared_ptr<mcp::block_state> block_state_a);

		std::shared_ptr<mcp::account_state> latest_account_state_get(mcp::db::db_transaction & transaction_a, Address const & account_a);
		void latest_account_state_put(mcp::db::db_transaction & transaction_a, Address const & account_a, std::shared_ptr<mcp::account_state> account_state_a);

		bool successor_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash & successor_a);
		void successor_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash const & successor_a);
		void successor_del(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a);

		bool block_summary_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash & summary_a);
		void block_summary_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::block_hash const & summary_a);

		void mark_as_changing();
		void commit_and_clear_changing();

	private:
		mcp::block_store & m_store;
		std::shared_ptr<mcp::block_cache> m_cache;

		template<class TKey, class TValue>
		class put_item
		{
		public:
			put_item(TKey const & key_a, TValue value_a) :
				key(key_a),
				value(value_a)
			{
			}

			TKey key;
			TValue value;
		};

		size_t m_max_block_puts_size = 10000;
		std::unordered_set<mcp::block_hash> m_block_puts_flushed;
		boost::multi_index_container<
			put_item<mcp::block_hash, std::shared_ptr<mcp::block>>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<mcp::block_hash, std::shared_ptr<mcp::block>>, mcp::block_hash, &put_item<mcp::block_hash, std::shared_ptr<mcp::block>>::key>>
			>>
			m_block_puts;

		size_t m_max_block_state_puts_size = 10000;
		std::unordered_set<mcp::block_hash> m_block_state_puts_flushed;
		boost::multi_index_container<
			put_item<mcp::block_hash, std::shared_ptr<mcp::block_state>>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<mcp::block_hash, std::shared_ptr<mcp::block_state>>, mcp::block_hash, &put_item<mcp::block_hash, std::shared_ptr<mcp::block_state>>::key>>
			>>
			m_block_state_puts;

		//size_t m_max_latest_account_state_puts_size = 10000;
		//std::unordered_set<Address> m_latest_account_state_puts_flushed;
		//boost::multi_index_container<
		//	put_item<Address, std::shared_ptr<mcp::account_state>>,
		//	boost::multi_index::indexed_by<
		//	boost::multi_index::sequenced<>,
		//	boost::multi_index::hashed_unique<boost::multi_index::member<put_item<Address, std::shared_ptr<mcp::account_state>>, Address, &put_item<Address, std::shared_ptr<mcp::account_state>>::key>>
		//	>>
		//	m_latest_account_state_puts;

		size_t m_max_successor_puts_size = 10000;
		std::unordered_set<mcp::block_hash> m_successor_puts_flushed;
		boost::multi_index_container<
			put_item<mcp::block_hash, mcp::block_hash>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<mcp::block_hash, mcp::block_hash>, mcp::block_hash, &put_item<mcp::block_hash, mcp::block_hash>::key>>
			>>
			m_successor_puts;
		std::unordered_set<mcp::block_hash> m_successor_dels;

		size_t m_max_block_summary_puts_size = 10000;
		std::unordered_set<mcp::block_hash> m_block_summary_puts_flushed;
		boost::multi_index_container<
			put_item<mcp::block_hash, mcp::block_hash>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<mcp::block_hash, mcp::block_hash>, mcp::block_hash, &put_item<mcp::block_hash, mcp::block_hash>::key>>
			>>
			m_block_summary_puts;
	};

} // namespace mcp
