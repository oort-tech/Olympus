#pragma once

#include <mcp/core/block_store.hpp>
#include <mcp/core/block_cache.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

namespace mcp
{
	class TransactionQueue;
	class ApproveQueue;
	class process_block_cache : public mcp::iblock_cache
	{
	public:
		explicit process_block_cache(std::shared_ptr<mcp::block_cache> cache_a, mcp::block_store & store_a, std::shared_ptr<TransactionQueue> tq, std::shared_ptr<ApproveQueue> aq);

		bool block_exists(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a);
		std::shared_ptr<mcp::block> block_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a);
		void block_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::shared_ptr<mcp::block> block_a);

		std::shared_ptr<mcp::block_state> block_state_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a);
		void block_state_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::shared_ptr<mcp::block_state> block_state_a);

		std::shared_ptr<mcp::account_state> latest_account_state_get(mcp::db::db_transaction & transaction_a, Address const & account_a);
		void latest_account_state_put(mcp::db::db_transaction & transaction_a, Address const & account_a, std::shared_ptr<mcp::account_state> account_state_a);

		/// transaction
		bool transaction_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash);
		std::shared_ptr<Transaction> transaction_get(mcp::db::db_transaction & transaction_a, h256 const&_hash);
		void transaction_put(mcp::db::db_transaction & transaction_a, std::shared_ptr<Transaction> _t);
		void transaction_del_from_queue(h256 const& _hash);

		bool account_nonce_get(mcp::db::db_transaction & transaction_a, Address const & account_a, u256 & nonce_a);
		void account_nonce_put(mcp::db::db_transaction & transaction_a, Address const & account_a, u256 const & nonce_a);

		void transaction_address_put(mcp::db::db_transaction & transaction_a, h256 const & hash, std::shared_ptr<mcp::TransactionAddress> const& td);

		/// approve
		bool approve_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash);
		std::shared_ptr<approve> approve_get(mcp::db::db_transaction & transaction_a, h256 const&_hash);
		void approve_put(mcp::db::db_transaction & transaction_a, std::shared_ptr<approve> _t);

		bool successor_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash & successor_a);
		void successor_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash const & successor_a);
		void successor_del(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a);

		bool block_summary_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash & summary_a);
		void block_summary_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::block_hash const & summary_a);

		void block_number_put(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::block_hash const & hash_a);

		bool transaction_receipt_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash);
		std::shared_ptr<dev::eth::TransactionReceipt> transaction_receipt_get(mcp::db::db_transaction & transaction_a, h256 const&_hash);
		void transaction_receipt_put(mcp::db::db_transaction & transaction_a, h256 const& _hash, std::shared_ptr<dev::eth::TransactionReceipt> _t);

		bool approve_receipt_exists(mcp::db::db_transaction & transaction_a, h256 const& _hash);
		std::shared_ptr<dev::ApproveReceipt> approve_receipt_get(mcp::db::db_transaction & transaction_a, h256 const&_hash);
		void approve_receipt_put(mcp::db::db_transaction & transaction_a, h256 const& _hash, std::shared_ptr<dev::ApproveReceipt> _t);


		void mark_as_changing();
		void commit_and_clear_changing();

	private:
		mcp::block_store & m_store;
		std::shared_ptr<mcp::block_cache> m_cache;
		std::shared_ptr<TransactionQueue> m_tq;
		std::shared_ptr<ApproveQueue> m_aq;

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

		size_t m_max_latest_account_state_puts_size = 10000;
		std::unordered_set<Address> m_latest_account_state_puts_flushed;
		boost::multi_index_container<
			put_item<Address, std::shared_ptr<mcp::account_state>>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<Address, std::shared_ptr<mcp::account_state>>, Address, &put_item<Address, std::shared_ptr<mcp::account_state>>::key>>
			>>
			m_latest_account_state_puts;

		size_t m_max_transaction_puts_size = 10000;
		std::unordered_set<h256> m_transaction_puts_flushed;
		boost::multi_index_container<
			put_item<h256, std::shared_ptr<Transaction>>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<h256, std::shared_ptr<Transaction>>, h256, &put_item<h256, std::shared_ptr<Transaction>>::key>>
			>>
			m_transaction_puts;
		h256s m_transaction_dels;///delete from transaction queue

		size_t m_max_account_nonce_puts_size = 10000;
		std::unordered_set<Address> m_account_nonce_puts_flushed;
		boost::multi_index_container<
			put_item<Address, u256>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<Address, u256>, Address, &put_item<Address, u256>::key>>
			>>
			m_account_nonce_puts;

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

		size_t m_max_transaction_receipt_puts_size = 10000;
		std::unordered_set<h256> m_transaction_receipt_puts_flushed;
		boost::multi_index_container<
			put_item<h256, std::shared_ptr<dev::eth::TransactionReceipt>>,
			boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::hashed_unique<boost::multi_index::member<put_item<h256, std::shared_ptr<dev::eth::TransactionReceipt>>, h256, &put_item<h256, std::shared_ptr<dev::eth::TransactionReceipt>>::key>>
			>>
			m_transaction_receipt_puts;
	};

} // namespace mcp
