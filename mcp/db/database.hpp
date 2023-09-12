#pragma once

#include<boost/filesystem.hpp> 
#include <libdevcore/Common.h>
#include <mcp/common/assert.hpp>

#include <mcp/db/write_batch.hpp>
#include <mcp/db/db_transaction.hpp>
#include <mcp/db/db_iterator.hpp>
#include <mcp/db/column.hpp>
#include <mcp/common/log.hpp>
#include <rocksdb/advanced_cache.h>
#include <rocksdb/sst_file_manager.h>
#include <rocksdb/rate_limiter.h>
#include <rocksdb/slice_transform.h>
#include <mcp/common/mcp_json.hpp>

namespace mcp
{
	namespace db
	{
		void check_status(rocksdb::Status const& _status);

		class rocksdb_logger : public rocksdb::Logger {
		public:
			using Logger::Logv;
			rocksdb_logger() : Logger(rocksdb::InfoLogLevel::ERROR_LEVEL) {}

			void Logv(const char* format, va_list ap) override 
			{
			}
		};

		class database_config
		{
		public:
			database_config():cache_size(2* 1024) {};
			void serialize_json(mcp::json &) const;
			bool deserialize_json(mcp::json const &);
			bool parse_old_version_data(mcp::json const &, uint64_t const&);
			uint64_t cache_size; //MB
			static uint64_t write_buffer_size; //MB
			static bool cache_filter; //Caching Index and Filter Blocks
		};

		struct index_info
		{
			int col_index = 0;
			bool shared = false;
			std::string prefix = "";
		};

		class forward_iterator;
		class backward_iterator;
		enum class db_column_index;
		class db_transaction;
		class db_column;
		class database
		{
			friend class write_batch;
			friend class db_transaction;
			friend class db_column;
		public:
			database(boost::filesystem::path const& path_a);
			~database();
			bool open();

			std::string get_rocksdb_state(uint64_t limit);

			void put(int const& _index, dev::Slice const& _k, dev::Slice const& _v,
				std::shared_ptr<rocksdb::WriteOptions> write_ops_a = nullptr);
			bool get(int const& _index, dev::Slice const& _k, std::string& _v,
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr);
			void del(int const& _index, dev::Slice const& _k,
				std::shared_ptr<rocksdb::WriteOptions> write_ops_a = nullptr);
			void del_range(int const& index, dev::Slice const& start, dev::Slice const& end,
				std::shared_ptr<rocksdb::WriteOptions> write_ops_a = nullptr);
			bool exists(int const& _index, dev::Slice const& _k,
				std::shared_ptr<rocksdb::ReadOptions> read_ops_a = nullptr);
			//bool merge(int const& _index, dev::Slice const & _k, dev::Slice const & _v,
			//	std::shared_ptr<rocksdb::WriteOptions> write_ops_a = nullptr);

			//write_batch create_write_batch(int index);
			mcp::db::db_transaction create_transaction(std::shared_ptr<rocksdb::WriteOptions> write_options_a = nullptr, std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a = nullptr);
			int create_column_family(std::string const& name_a, std::shared_ptr<rocksdb::ColumnFamilyOptions> cfops);
			int set_column_family(int index_a, std::string const & name_a="");
			std::shared_ptr<rocksdb::ManagedSnapshot> create_snapshot();
			//void release_snapshot(std::shared_ptr<rocksdb::ManagedSnapshot> _snapshot) { m_db->ReleaseSnapshot(_snapshot.snapshot()); };

			static std::shared_ptr<rocksdb::ReadOptions> default_read_options();
			static std::shared_ptr<rocksdb::WriteOptions> default_write_options();

			static std::shared_ptr<rocksdb::Cache> get_table_cache() { return table_cache; }
			static void init_table_cache(uint64_t size) { table_cache = rocksdb::NewLRUCache(size * 1024 * 1024); }
		private:
			rocksdb::Options default_DB_options();
			rocksdb::TransactionDBOptions default_trans_DB_options();

			rocksdb::Status open_rocksdb(std::string path_a);
			//void create_column();

			rocksdb::ColumnFamilyHandle* get_column_family_handle(int index, std::shared_ptr<index_info> info = nullptr);

			rocksdb::TransactionDB* get_db() { return m_db; }
			//std::shared_ptr<rocksdb::ReadOptions> get_read_options() { return m_read_options; }
			//std::shared_ptr<rocksdb::WriteOptions> get_write_options() { return m_write_options; }

			std::string m_path;
			rocksdb::TransactionDB* m_db;
			std::shared_ptr<rocksdb::ReadOptions> m_read_options;
			std::shared_ptr<rocksdb::WriteOptions> m_write_options;
			std::shared_ptr<db_column> m_column;
			static std::shared_ptr<rocksdb::Cache> table_cache;
			static std::shared_ptr<rocksdb::SstFileManager> rocksdb_sst_file_manager;

			int m_count;

			std::map<int, index_info> m_index;

			mcp::log m_log = { mcp::log("db") };
		};
	}
}

