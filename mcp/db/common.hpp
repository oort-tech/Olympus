#pragma once
#include <rocksdb/status.h>
#include <mcp/common/mcp_json.hpp>

namespace mcp
{
	namespace db
	{
		void check_status(rocksdb::Status const& _status);

		class database_config
		{
		public:
			database_config() :cache_size(2 * 1024) {};
			void serialize_json(mcp::json&) const;
			bool deserialize_json(mcp::json const&);
			bool parse_old_version_data(mcp::json const&, uint64_t const&);
			uint64_t cache_size; //MB
			static uint64_t write_buffer_size; //MB
			static bool cache_filter; //Caching Index and Filter Blocks
		};
	}
}

