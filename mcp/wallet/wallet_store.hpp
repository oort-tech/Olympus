#pragma once
#include <mcp/core/common.hpp>

namespace mcp
{
	class wallet_store
	{
	public:
		wallet_store(bool & error_a, boost::filesystem::path const& _path);

		//mcp::db::db_transaction create_transaction() { return m_database->create_transaction(); };

	private:
		std::shared_ptr<mcp::db::database> m_database;
	};
}