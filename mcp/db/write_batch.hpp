#pragma once

#include <rocksdb/write_batch.h>
#include <mcp/db/database.hpp>

namespace mcp
{
	namespace db
	{
		class database;
		enum class db_column_index;
		/*atomic operation, destructor commit the data to database, must be commit if want get the commit data*/
		class write_batch
		{
		public:
			write_batch(database& m_db_a);
			~write_batch();
			void put(int const& index, dev::Slice const& _k, dev::Slice const& _v);
			void del(int const& index, dev::Slice const& _k);

			void commit();
		private:
			rocksdb::WriteBatch m_write_batch;
			database& m_db;
		};
	}
}
