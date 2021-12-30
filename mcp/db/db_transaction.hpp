#pragma once

#include <rocksdb/utilities/transaction_db.h>
#include <mcp/db/database.hpp>
#include <mcp/db/counter.hpp>

namespace mcp
{
	namespace db
	{
		class database;
		class forward_iterator;
		class backward_iterator;
		enum class db_column_index;
		class merge_based_counters;
		/*transaction atomic operation, destructor commit the data to database, can get the key real time*/
		class db_transaction
		{
		public:
			db_transaction(database& m_db_a,
				std::shared_ptr<rocksdb::WriteOptions> write_options_a = nullptr,
				std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a = nullptr
			);

			db_transaction(mcp::db::db_transaction && other_a);
			~db_transaction();
			static std::shared_ptr<rocksdb::TransactionOptions> default_trans_options();

			void put(int const& index, dev::Slice const& _k, dev::Slice const& _v);
			bool get(int const& index, dev::Slice const& _k, std::string& _v, 
				std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr, 
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr
			);
			void del(int const& index, dev::Slice const& _k);
			bool exists(int const& index, dev::Slice const& _k, 
				std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr,
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr
			);

			void count_add(std::string const& _k, uint32_t const& _v);
			void count_reduce(std::string const& _k, uint32_t const& _v);
			void count_del(std::string const& _k);
			uint64_t count_get(std::string const& _k, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);

			void commit();
			void rollback();

			forward_iterator begin(int const& index, 
				std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr,
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr);
			forward_iterator begin(int const& index, 
				dev::Slice const& _k, 
				std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr,
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr);

			backward_iterator rbegin(int const& index, 
				std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr,
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr);
			backward_iterator rbegin(int const& index, 
				dev::Slice const& _k, 
				std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr,
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr);

			bool merge(int const & index, std::string const& _k, dev::Slice const& _v);

			database& get_db() { return m_db; }

			mcp::db::db_transaction & operator= (mcp::db::db_transaction && other_a);
		private:
			database& m_db;
			rocksdb::Transaction* m_txn;
			bool m_commited_or_rollbacked;
			bool m_read_only;
		};	
	}
}
