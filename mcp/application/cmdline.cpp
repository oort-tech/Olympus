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
		auto account(instance.key_manager->create(password));
		std::cout << boost::str(boost::format("\nAccount: %1%\n") % dev::Address(account).hexPrefixed());
	}
	else if (vm.count("account_remove"))
	{
		if (vm.count("account"))
		{
			std::string password = mcp::getPassword("Enter the current passphrase for the remove account:");
			vm_instance instance(data_path);
			std::string account_text = vm["account"].as<std::string>();
			if (mcp::isAddress(account_text))
			{
				dev::Address account(account_text);
				bool exists(instance.key_manager->exists(account));
				if (exists)
				{
					if (!instance.key_manager->remove(account, password))
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
		if (vm.count("file"))
		{
			std::string filename(vm["file"].as<std::string>());
			std::ifstream stream;
			stream.open(filename.c_str());
			if (!stream.fail())
			{
				std::stringstream contents;
				contents << stream.rdbuf();
				try
				{
					vm_instance instance(data_path);
					mcp::key_content kc;
					mcp::json js = mcp::json::parse(contents.str());
					if (instance.key_manager->import(js, kc))
					{
						std::cerr << "Import account " << kc.address.hexPrefixed() << std::endl;
						result = false;
					}
					else
						std::cerr << "Unable to import account\n";
				}
				catch (...)
				{
					std::cerr << "Unable to parse json\n";
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
		Addresses account_list(instance.key_manager->list());
		for (dev::Address account : account_list)
		{
			std::cout << account.hexPrefixed() << '\n';
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
