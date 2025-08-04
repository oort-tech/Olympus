#pragma once

#include "common.hpp"
#include "column.hpp"
#include "db_transaction.hpp"
#include <boost/filesystem.hpp> 
#include <libdevcore/Common.h>
#include <mcp/common/log.hpp>
#include <rocksdb/advanced_cache.h>
#include <rocksdb/slice_transform.h>
#include <rocksdb/snapshot.h>
#include <rocksdb/db.h>
#include <rocksdb/table.h>
#include <libdevcore/db.h>
using namespace dev::db;

namespace mcp
{
	namespace db
	{
		std::shared_ptr<rocksdb::BlockBasedTableOptions> defaultBlockBasedTableOptions(uint64_t size);
		std::shared_ptr <rocksdb::Options> defaultDBOptions(std::shared_ptr<rocksdb::BlockBasedTableOptions> base);

		class db_transaction;
		class database : public DatabaseFace
		{
			friend class db_transaction;
		public:
			database(boost::filesystem::path const& path_a, std::shared_ptr<rocksdb::Options> Ops);
			~database();

			std::string get_rocksdb_state(uint64_t limit);

			std::string lookup(Slice _key) const override;
			bool exists(Slice _key) const override;
			void insert(Slice _key, Slice _value) override;
			void kill(Slice _key) override;
			std::unique_ptr<WriteBatchFace> createWriteBatch() const override;
			void commit(std::unique_ptr<WriteBatchFace> _batch) override;
			void forEach(std::function<bool(Slice, Slice)> f) const override;

			void del_range(uint8_t const& _prefix, dev::Slice const& start, dev::Slice const& end,
				std::shared_ptr<rocksdb::WriteOptions> write_ops_a = nullptr);

			mcp::db::db_transaction create_transaction(std::shared_ptr<rocksdb::WriteOptions> write_options_a = nullptr, std::shared_ptr<rocksdb::TransactionOptions> txn_ops_a = nullptr);
			std::shared_ptr<rocksdb::ManagedSnapshot> create_snapshot();

			static std::shared_ptr<rocksdb::ReadOptions> default_read_options();
			static std::shared_ptr<rocksdb::WriteOptions> default_write_options();

			rocksdb::TransactionDB* get_db() { return m_db; }
		private:
			rocksdb::TransactionDB* m_db;
			std::shared_ptr<rocksdb::ReadOptions> m_read_options;
			std::shared_ptr<rocksdb::WriteOptions> m_write_options;
		};
	}
}

