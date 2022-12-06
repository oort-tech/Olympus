#pragma once

#include "blocks.hpp"
#include <mcp/core/common.hpp>
#include <mcp/db/database.hpp>
#include <mcp/core/transaction_receipt.hpp>
#include <mcp/core/approve_receipt.hpp>

namespace mcp
{
	/**
	* Manages block storage and iteration
	*/
	class block_store
	{
	public:
		block_store(bool &, boost::filesystem::path const &);

		bool upgrade();

		std::string get_rocksdb_state(uint64_t limit);

		bool block_exists(mcp::db::db_transaction &, mcp::block_hash const &);
		std::shared_ptr<mcp::block> block_get(mcp::db::db_transaction &, mcp::block_hash const &);
		void block_put(mcp::db::db_transaction &, mcp::block_hash const &, mcp::block const &);
		mcp::db::forward_iterator block_begin(mcp::db::db_transaction & transaction_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);

		size_t block_count(mcp::db::db_transaction &);

		/// transactions
		bool transaction_exists(mcp::db::db_transaction &, h256 const &);
		std::shared_ptr<mcp::Transaction> transaction_get(mcp::db::db_transaction &, h256 const &);
		void transaction_put(mcp::db::db_transaction &, h256 const &, mcp::Transaction const &);

		/// account processed nonce, but maybe not stable
		/// return true if exist
		bool account_nonce_get(mcp::db::db_transaction & transaction_a, Address const & account_a, u256& nonce_a);
		void account_nonce_put(mcp::db::db_transaction & transaction_a, Address const & account_a, u256 const& nonce_a);

		/// transaction -> block
		std::shared_ptr<mcp::TransactionAddress> transaction_address_get(mcp::db::db_transaction &, h256 const &);
		void transaction_address_put(mcp::db::db_transaction &, h256 const &, mcp::TransactionAddress const &);
		
		/// approves
		bool approve_exists(mcp::db::db_transaction &, h256 const &);
		std::shared_ptr<mcp::approve> approve_get(mcp::db::db_transaction &, h256 const &);
		void approve_put(mcp::db::db_transaction &, h256 const &, mcp::approve const &);

		std::shared_ptr<mcp::account_state> account_state_get(mcp::db::db_transaction & transaction_a, h256 const& hash_a);
		void account_state_put(mcp::db::db_transaction & transaction_a, h256 const& hash_a, mcp::account_state const & value_a);

		bool latest_account_state_get(mcp::db::db_transaction & transaction_a, Address const & account_a, h256& hash_a);
		void latest_account_state_put(mcp::db::db_transaction & transaction_a, Address const & account_a, h256 const& hash_a);

		bool contract_main_trie_node_get(mcp::db::db_transaction & transaction_a, mcp::code_hash const & hash_a, std::string & value_a);
		void contract_main_trie_node_put(mcp::db::db_transaction & transaction_a, mcp::code_hash const & hash_a, std::string const & value_a);

		bool contract_aux_state_key_get(mcp::db::db_transaction & transaction_a, dev::bytes const & key_a, dev::bytes & value_a);
		void contract_aux_state_key_put(mcp::db::db_transaction & transaction_a, dev::bytes const & key_a, dev::bytes const & value_a);

		bool dag_account_get(mcp::db::db_transaction & transaction_a, dev::Address const & account_a, mcp::dag_account_info & info_a);
		void dag_account_put(mcp::db::db_transaction & transaction_a, dev::Address const & account_a, mcp::dag_account_info const & info_a);

		bool block_summary_get(mcp::db::db_transaction &, mcp::block_hash const &, mcp::summary_hash &);
		void block_summary_put(mcp::db::db_transaction &, mcp::block_hash const &, mcp::summary_hash const &);

		bool summary_block_get(mcp::db::db_transaction &, mcp::summary_hash const &, mcp::block_hash &);
		void summary_block_put(mcp::db::db_transaction &, mcp::summary_hash const &, mcp::block_hash const &);

		mcp::db::forward_iterator dag_free_begin(mcp::db::db_transaction & transaction_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);
		mcp::db::forward_iterator dag_free_begin(mcp::db::db_transaction & transaction_a, mcp::free_key const & key_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);
		mcp::db::backward_iterator dag_free_rbegin(mcp::db::db_transaction & transaction_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);
		void dag_free_put(mcp::db::db_transaction & transaction_a, mcp::free_key const & key_a);
		void dag_free_del(mcp::db::db_transaction & transaction_a, mcp::free_key const & key_a);
		size_t dag_free_count(mcp::db::db_transaction & transaction_a);

		bool main_chain_get(mcp::db::db_transaction & transaction_a, uint64_t const & mci, mcp::block_hash & hash_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);
		void main_chain_put(mcp::db::db_transaction & transaction_a, uint64_t const & mci, mcp::block_hash const & hash_a);
		void main_chain_del(mcp::db::db_transaction & transaction_a, uint64_t const & mci);

		std::shared_ptr<mcp::block_state> block_state_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a);
		void block_state_put(mcp::db::db_transaction &, mcp::block_hash const &, mcp::block_state const &);

		size_t stable_block_count(mcp::db::db_transaction & transaction_a);
		bool stable_block_get(mcp::db::db_transaction & transaction_a, uint64_t const & index, mcp::block_hash & hash_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);
		void stable_block_put(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::block_hash const & hash_a);
		bool stable_block_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const& hash_a, uint64_t & index_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);

		size_t transaction_unstable_count(mcp::db::db_transaction & transaction_a);
		void transaction_unstable_count_add(mcp::db::db_transaction & transaction_a, uint32_t v = 1);
		void transaction_unstable_count_reduce(mcp::db::db_transaction & transaction_a, uint32_t v = 1);
		size_t transaction_count(mcp::db::db_transaction & transaction_a);
		void transaction_count_add(mcp::db::db_transaction & transaction_a, uint32_t v = 1);
		
		size_t approve_unstable_count(mcp::db::db_transaction & transaction_a);
		void approve_unstable_count_add(mcp::db::db_transaction & transaction_a, uint32_t v = 1);
		void approve_unstable_count_reduce(mcp::db::db_transaction & transaction_a, uint32_t v = 1);
		size_t approve_count(mcp::db::db_transaction & transaction_a);
		void approve_count_add(mcp::db::db_transaction & transaction_a, uint32_t v = 1);

		//sync
		bool catchup_chain_summaries_get(mcp::db::db_transaction & transaction_a, uint64_t const &, mcp::summary_hash &);
		void catchup_chain_summaries_put(mcp::db::db_transaction & transaction_a, uint64_t const &, mcp::summary_hash const &);
		void catchup_chain_summaries_del(mcp::db::db_transaction &, uint64_t const &);
		void catchup_chain_summaries_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a = nullptr);

		bool catchup_chain_block_summary_get(mcp::db::db_transaction &, mcp::block_hash const &, mcp::summary_hash &);
		void catchup_chain_block_summary_put(mcp::db::db_transaction &, mcp::block_hash const &, mcp::summary_hash const &);
		void catchup_chain_block_summary_del(mcp::db::db_transaction &, mcp::block_hash const &);
		void catchup_chain_block_summary_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a = nullptr);

		bool catchup_chain_summary_block_get(mcp::db::db_transaction &, mcp::summary_hash const &, mcp::block_hash &);
		void catchup_chain_summary_block_put(mcp::db::db_transaction &, mcp::summary_hash const &, mcp::block_hash const &);
		void catchup_chain_summary_block_del(mcp::db::db_transaction &, mcp::summary_hash const &);
		void catchup_chain_summary_block_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a = nullptr);

		bool hash_tree_summary_exists(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_a);
		void hash_tree_summary_put(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_a);
		void hash_tree_summary_del(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_a);
		void hash_tree_summary_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a = nullptr);

		// Others
		uint64_t last_mci_get(mcp::db::db_transaction & transaction_a);
		void last_mci_put(mcp::db::db_transaction & transaction_a, uint64_t const & last_mci_value);

		uint64_t last_stable_mci_get(mcp::db::db_transaction & transaction_a);
		void last_stable_mci_put(mcp::db::db_transaction & transaction_a, uint64_t const & last_stable_mci_value);

		mcp::advance_info advance_info_get(mcp::db::db_transaction & transaction_a);
		void advance_info_put(mcp::db::db_transaction & transaction_a, mcp::advance_info const & value_a);

		uint64_t last_stable_index_get(mcp::db::db_transaction & transaction_a);
		void last_stable_index_put(mcp::db::db_transaction & transaction_a, uint64_t const & last_stable_index_value);

		void block_children_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & p_hash_a, std::list<mcp::block_hash> & c_hashs_a);
		mcp::db::forward_iterator block_child_begin(mcp::db::db_transaction & transaction_a, mcp::block_child_key const & key_a);
		void block_child_put(mcp::db::db_transaction & transaction_a, mcp::block_child_key const & key_a);

		bool skiplist_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a, mcp::skiplist_info & skiplist);
		void skiplist_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a, mcp::skiplist_info const & skiplist);

		bool successor_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash & successor_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);
		void successor_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash const & successor_a);
		void successor_del(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a);

		bool genesis_hash_get(mcp::db::db_transaction & transaction_a, mcp::block_hash & genesis_hash);
		void genesis_hash_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & genesis_hash);

		bool catchup_index_get(mcp::db::db_transaction & transaction_a, uint64_t & _v);
		void catchup_index_put(mcp::db::db_transaction & transaction_a, uint64_t const& _v);
		void catchup_index_del(mcp::db::db_transaction & transaction_a);

		bool catchup_max_index_get(mcp::db::db_transaction & transaction_a, uint64_t & _v);
		void catchup_max_index_put(mcp::db::db_transaction & transaction_a, uint64_t const& _v);
		void catchup_max_index_del(mcp::db::db_transaction & transaction_a);

		bool traces_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::list<std::shared_ptr<mcp::trace>> & traces_a);
		void traces_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, std::list<std::shared_ptr<mcp::trace>> const & traces_a);

		std::shared_ptr<dev::eth::TransactionReceipt> transaction_receipt_get(mcp::db::db_transaction & transaction_a, h256 const& hash_a);
		void transaction_receipt_put(mcp::db::db_transaction &, h256 const& hash_a, dev::eth::TransactionReceipt const& receipt);

		std::shared_ptr<dev::ApproveReceipt> approve_receipt_get(mcp::db::db_transaction & transaction_a, h256 const& hash_a);
		void approve_receipt_put(mcp::db::db_transaction &, h256 const& hash_a, dev::ApproveReceipt const& receipt);

		void epoch_approves_get(mcp::db::db_transaction & transaction_a, uint64_t const & epoch, std::list<h256> & hashs_a);
		void epoch_approves_put(mcp::db::db_transaction & transaction_a, mcp::epoch_approves_key const & key_a);

		void epoch_approve_receipts_get(mcp::db::db_transaction & transaction_a, uint64_t const & epoch, std::list<h256> & hashs_a);
		void epoch_approve_receipts_put(mcp::db::db_transaction & transaction_a, mcp::epoch_approves_key const & key_a);

		bool epoch_elected_approve_receipts_get(mcp::db::db_transaction & transaction_a, uint64_t const & epoch, epoch_elected_list & list);
		void epoch_elected_approve_receipts_put(mcp::db::db_transaction & transaction_a, uint64_t const & epoch, epoch_elected_list & list);

		void version_put(mcp::db::db_transaction &, int);
		int version_get();

		uint64_t last_epoch_get(mcp::db::db_transaction & transaction_a);
		void last_epoch_put(mcp::db::db_transaction & transaction_a, uint64_t const & last_epoch_a);

		bool transaction_account_state_get(mcp::db::db_transaction & transaction_a, mcp::link_hash const & link_a, std::vector<h256> & hashs_a);
		void transaction_account_state_put(mcp::db::db_transaction & transaction_a, mcp::link_hash const & link_a, std::vector<h256> & hashs_a);

		mcp::db::db_transaction create_transaction(std::shared_ptr<rocksdb::WriteOptions> write_options_a = nullptr,
			std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a = nullptr)
		{
			return m_db->create_transaction(write_options_a, txn_ops_a);
		}

		std::shared_ptr<rocksdb::ManagedSnapshot> create_snapshot() { return m_db->create_snapshot(); }
		//void release_snapshot(std::shared_ptr<rocksdb::ManagedSnapshot> _snapshot) { m_db->release_snapshot(_snapshot); }

		std::shared_ptr<mcp::db::database> m_db;
		// account -> dag account info                                 
		int dag_account_info;
		// account -> account info                                        
		int account_info;
		//account state hash -> account state
		int account_state;
		//account -> latest account state
		int latest_account_state;
		// block_hash -> block
		int blocks;

		// transaction hash -> transaction
		int transactions;

		int approves;

		// account -> nonce
		int account_nonce;

		// transaction hash -> TransactionAddress
		int transaction_address;

		//block hash -> block state
		int block_state;
		//block hash , child block hash -> nullptr
		int block_child;

		//witnessed level, level, block hash -> nullptr
		int dag_free;
		//main chain index -> block hash
		int main_chain;
		//block stable index-> block hash
		int stable_block;
		//block block hash-> stable index
		int stable_block_number;
		// block hash -> summary hash
		int block_summary;
		// summary hash -> block hash
		int summary_block;
		//block hash -> skiplist
		int skiplist;
		//block root (previous or account) -> block hash
		int successor;

		//key -> value
		int prop;

		int catchup_chain_summaries;
		int catchup_chain_block_summary;
		int catchup_chain_summary_block;
		int hash_tree_summary;

		// contract state
		int contract_main;
		int contract_aux;

		//version key
		static dev::h256 const version_key;
		int unlink_block;
		int unlink_info;
		int next_unlink;
		int next_unlink_index;
		int head_unlink;

		//traces
		int traces;

		//hash -> transaction receipt
		int transaction_receipt;
		int approve_receipt;

		int epoch_approves;
		int epoch_approve_receipts;
		int epoch_elected_approve_receipts;

		int transaction_account_state;

		//genesis hash key
		static dev::h256 const genesis_hash_key;
		//genesis transaction hash key
		static dev::h256 const genesis_transaction_hash_key;
		//last main chain index key
		static dev::h256 const last_mci_key;
		//last stable main chain index key
		static dev::h256 const last_stable_mci_key;
		//advance info key
		static dev::h256 const advance_info_key;
		//last stable index key
		static dev::h256 const last_stable_index_key;
		//catch up index key
		static dev::h256 const catchup_index;
		//catch up max index key
		static dev::h256 const catchup_max_index;
		//
		static dev::h256 const last_epoch_key;
	};
}
