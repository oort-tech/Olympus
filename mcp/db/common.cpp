#include "common.hpp"
#include <mcp/common/assert.hpp>

uint64_t mcp::db::database_config::write_buffer_size = 1024;
bool mcp::db::database_config::cache_filter = true;

//check return status
void mcp::db::check_status(rocksdb::Status const& _status)
{
	if (_status.ok())
		return;

	assert_x_msg(_status.ok(), "rocksDB operate error, msg:" + _status.ToString());
}

void mcp::db::database_config::serialize_json(mcp::json& json_a) const
{
	json_a["cache"] = cache_size;
	json_a["write_buffer"] = write_buffer_size;
	json_a["cache_filter"] = cache_filter ? "true" : "false";
}

bool mcp::db::database_config::deserialize_json(mcp::json const& json_a)
{
	auto error(false);
	try
	{
		if (json_a.count("cache") && json_a["cache"].is_number_unsigned())
			cache_size = json_a["cache"].get<std::uint64_t>();
		if (json_a.count("write_buffer") && json_a["write_buffer"].is_number_unsigned())
			write_buffer_size = json_a["write_buffer"].get<std::uint64_t>();
		if (json_a.count("cache_filter") && json_a["cache_filter"].is_string())
			cache_filter = (json_a["cache_filter"].get<std::string>() == "true" ? true : false);
	}
	catch (std::runtime_error const&)
	{
		error = true;
	}
	return error;
}

bool mcp::db::database_config::parse_old_version_data(mcp::json const& json_a, uint64_t const& version)
{
	auto error(false);
	try
	{
		/// parse json used low version
		switch (version)
		{
			//case 0:
			//{
			//	/// parse
			//	break;
			//}
			//case 1:
			//{
			//	/// parse
			//	break;
			//}
		default:
			error |= deserialize_json(json_a);
			break;
		}
	}
	catch (std::runtime_error const&)
	{
		error = true;
	}
	return error;
}