#include "wallet_store.hpp"

mcp::wallet_store::wallet_store(bool & error_a, boost::filesystem::path const & _path) :
	m_database(std::make_shared<mcp::db::database>(_path))
{
	if (error_a)
		return;

	//auto tbops = mcp::db::db_column::default_table_options(mcp::db::database::get_table_cache());
	//auto cfops = mcp::db::db_column::default_column_family_options(tbops);

	//error_a = !m_database->open();
	//if (error_a)
	//	std::cerr << "Wallet store db open error" << std::endl;
}



