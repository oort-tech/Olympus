#pragma once
#include <boost/filesystem.hpp> 
#include <boost/program_options.hpp>
#include <mcp/wallet/key_store.hpp>
#include <mcp/wallet/key_manager.hpp>

namespace mcp
{
	bool handle_node_options(boost::program_options::variables_map &);
	class vm_instance
	{
	public:
		vm_instance(boost::filesystem::path const &path = mcp::working_path());
		bool error;
		mcp::key_store key_store;
		std::shared_ptr<mcp::key_manager> key_manager;
	};
}