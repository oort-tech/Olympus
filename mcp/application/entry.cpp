#include <mcp/application/daemon.hpp>
#include <mcp/application/cmdline.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

int main(int argc, char * const * argv)
{
	boost::program_options::options_description description("Command line options");
    description.add_options()
        ("account_create", "Create new account")
        ("account_remove", "Remove account")
        ("account_import", "Imports account from json file")
        ("account_list", "List all accounts")
        ("account", boost::program_options::value<std::string>(), "Defines <account> for other commands")
        ("file", boost::program_options::value<std::string>(), "Defines <file> for other commands")
        ("data_path", boost::program_options::value<std::string>(), "Use the supplied path as the data directory");


    mcp_daemon::add_options(description);

	description.add_options()
		("help", "Print out options")
		("version", "Prints out version")
		("daemon", "Start node daemon");

	boost::program_options::variables_map vm;

	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);
		boost::program_options::notify(vm);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what();
		return -1;
	}

	int result(0);
	boost::filesystem::path data_path = vm.count("data_path") ? boost::filesystem::path(mcp_daemon::get_home_directory(vm["data_path"].as<std::string>())) : mcp::working_path();

	if (vm.count("version"))
	{
		std::cout << "Version: " << STR(MCP_VERSION) << std::endl;
	}
	else if (vm.count("daemon") > 0)
	{
		mcp_daemon::daemon daemon;
		daemon.run(data_path,vm);
	}
	else if (!mcp::handle_node_options(vm))
	{
		;
	}
	else
	{
		std::cout << description << std::endl;
		result = -1;
	}
	return result;
}
