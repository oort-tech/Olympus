#include "cmdline.hpp"
#include <mcp/common/pwd.hpp>

bool mcp::handle_node_options(boost::program_options::variables_map & vm)
{
	auto result(false);
	boost::filesystem::path data_path = vm.count("data_path") ? boost::filesystem::path(vm["data_path"].as<std::string>()) : mcp::working_path();
	if (vm.count("account_create"))
	{
		std::string password = mcp::createPassword("Enter a passphrase with which to secure this account:");
		vm_instance instance(data_path);
		auto account(instance.key_manager->create(password, true));
		std::cout << boost::str(boost::format("\nAccount: %1%\n") % account.to_account());
	}
	else if (vm.count("account_remove"))
	{
		if (vm.count("account") == 1)
		{
			std::string password = mcp::getPassword("Enter the current passphrase for the remove account:");
			vm_instance instance(data_path);
			mcp::account account;
			if (!account.decode_account(vm["account"].as<std::string>()))
			{
				bool exists(instance.key_manager->exists(account));
				if (exists)
				{
					auto error(instance.key_manager->remove(account, password));
					if (error)
					{
						std::cerr << "Wrong password\n";
					}
				}
				else
				{
					std::cerr << "Account not found\n";
				}
			}
			else
			{
				std::cerr << "Invalid account\n";
			}
		}
		else
		{
			std::cerr << "Requires one <account> option\n";
		}
	}
	else if (vm.count("account_import"))
	{
		if (vm.count("file") == 1)
		{
			std::string filename(vm["file"].as<std::string>());
			std::ifstream stream;
			stream.open(filename.c_str());
			if (!stream.fail())
			{
				std::stringstream contents;
				contents << stream.rdbuf();

				vm_instance instance(data_path);
				mcp::key_content kc;
				bool error(instance.key_manager->import(contents.str(), kc));
				if (!error)
				{
					std::cerr << "Import account " << kc.account.to_account() << std::endl;
					result = false;
				}
				else
				{
					std::cerr << "Unable to import account\n";
				}
			}
			else
			{
				std::cerr << "Unable to open <file>\n";
			}
		}
		else
		{
			std::cerr << "Requires one <file> option\n";
		}
	}
	else if (vm.count("account_list"))
	{
		vm_instance instance(data_path);
		std::list<mcp::account> account_list(instance.key_manager->list());
		for (mcp::account account : account_list)
		{
			std::cout << account.to_account() << '\n';
		}
	}
	else
	{
		result = true;
	}
	return result;
}

mcp::vm_instance::vm_instance(boost::filesystem::path const & path) :
	error(false),
	key_store(error, path / "keydb")
{
	boost::filesystem::create_directories(path);
	key_manager = std::make_shared<mcp::key_manager>(path, key_store);
}
