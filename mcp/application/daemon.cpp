#include <fstream>
#include <iostream>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <mcp/core/param.hpp>
#include <mcp/core/contract.hpp>
#include <mcp/application/daemon.hpp>
#include <mcp/common/pwd.hpp>
#include <mcp/node/witness.hpp>
#include <mcp/node/requesting.hpp>
#include <mcp/common/log.hpp>

mcp::thread_runner::thread_runner(boost::asio::io_service & service_a, unsigned service_threads_a, std::string const &service_name)
{
	for (unsigned i(0); i < service_threads_a; ++i)
	{
		threads.push_back(std::thread([&service_a, service_name]() {
			try
			{
				service_a.run();
				BOOST_LOG_TRIVIAL(error) << service_name << " io_service run exit normal ";
			}
			catch (std::exception const & e)
			{
				BOOST_LOG_TRIVIAL(error) << service_name << " io_service run exception: " << e.what();
				throw;
			}
			catch (...)
			{
				BOOST_LOG_TRIVIAL(error) << service_name << " unhandled io_service exception";
				throw;
			}
		}));
	}
}

mcp::thread_runner::~thread_runner()
{
	join();
}

void mcp::thread_runner::join()
{
	for (auto & i : threads)
	{
		if (i.joinable())
		{
			i.join();
		}
	}
}

mcp_daemon::thread_config::thread_config() :
	io_threads(std::max<unsigned>(2, std::thread::hardware_concurrency() / 2)),
	bg_threads(std::max<unsigned>(1, std::thread::hardware_concurrency() / 2)),
	sync_threads(std::max<unsigned>(1, std::thread::hardware_concurrency() / 2)),
	work_threads(std::max<unsigned>(1, std::thread::hardware_concurrency()))
{
}

void mcp_daemon::thread_config::serialize_json(mcp::json & json_a) const
{
	json_a["io_threads"] = io_threads;
	json_a["bg_threads"] = bg_threads;
	json_a["sync_threads"] = sync_threads;
	json_a["work_threads"] = work_threads;
}

bool mcp_daemon::thread_config::deserialize_json(mcp::json const & json_a)
{
	auto error(false);
	try
	{
		if (json_a.count("io_threads") && json_a["io_threads"].is_number_unsigned())
		{
			io_threads = json_a["io_threads"].get<unsigned>();
		}
		else
		{
			error = true;
		}

		if (json_a.count("bg_threads") && json_a["bg_threads"].is_number_unsigned())
		{
			bg_threads = json_a["bg_threads"].get<unsigned>();
		}
		else
		{
			error = true;
		}

		if (json_a.count("sync_threads") && json_a["sync_threads"].is_number_unsigned())
		{
			sync_threads = json_a["sync_threads"].get<unsigned>();
		}
		else
		{
			error = true;
		}

		if (json_a.count("work_threads") && json_a["work_threads"].is_number_unsigned())
		{
			work_threads = json_a["work_threads"].get<unsigned>();
		}
		else
		{
			error = true;
		}

		error |= bg_threads == 0;
		error |= io_threads == 0;
		error |= sync_threads == 0;
		error |= work_threads == 0;
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

bool mcp_daemon::thread_config::parse_old_version_data(mcp::json const & json_a, uint64_t const& version)
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
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

mcp_daemon::daemon_config::daemon_config()
{
}

void mcp_daemon::daemon_config::serialize_json(mcp::json & json_a)
{
    json_a["version"] = m_current_version;
	json_a["network"] = (unsigned)mcp::mcp_network;

    mcp::json j_node_l;
    node.serialize_json(j_node_l);
    json_a["node"] = j_node_l;

    mcp::json j_rpc_l;
    rpc.serialize_json(j_rpc_l);
    json_a["rpc"] = j_rpc_l;

    mcp::json j_rpc_ws_l;
    rpc_ws.serialize_json(j_rpc_ws_l);
    json_a["ws"] = j_rpc_ws_l;

    mcp::json j_logging_l;
    logging.serialize_json(j_logging_l);
    json_a["log"] = j_logging_l;

    mcp::json j_p2p_l;
    p2p.serialize_json(j_p2p_l);
    json_a["p2p"] = j_p2p_l;

    mcp::json j_witness_l;
    witness.serialize_json(j_witness_l);
    json_a["witness"] = j_witness_l;

	mcp::json j_db_l;
	db.serialize_json(j_db_l);
	json_a["database"] = j_db_l;
}

bool mcp_daemon::daemon_config::deserialize_json(bool & upgraded_a, mcp::json & json_a)
{
    auto error(false);
    try
    {
		if (json_a.empty())	//if null, create new
		{
			upgraded_a = true;
			serialize_json(json_a);
			return false;
		}
		
		if (!m_is_network_set)
		{
			unsigned u_network;
			if (!json_a.count("network"))
			{
				std::cerr << "Config \"network\" not found" << std::endl;
				exit(1);
			}
			else if (json_a["network"].is_string())
				u_network = boost::lexical_cast<unsigned>(json_a["network"].get<std::string>());
			else
				u_network = json_a["network"].get<unsigned>();

			set_network((mcp::mcp_networks)u_network);
		}

		if (!(json_a.count("node") && json_a["node"].is_object() &&
			json_a.count("rpc") && json_a["rpc"].is_object() &&
			json_a.count("ws") && json_a["ws"].is_object() &&
			json_a.count("log") && json_a["log"].is_object() &&
			json_a.count("p2p") && json_a["p2p"].is_object() &&
			json_a.count("witness") && json_a["witness"].is_object() &&
			json_a.count("database") && json_a["database"].is_object()))
			return true;

		uint64_t old_version;

		if (!json_a.count("version"))
			old_version = 0;
		else if (json_a["version"].is_string())
			old_version = boost::lexical_cast<uint64_t>(json_a["version"].get<std::string>());
		else
			old_version = json_a["version"].get<uint64_t>();

		if (old_version != m_current_version) //need upgrade
		{
			error = parse_old_version_data(json_a, old_version);
			if (!error)
			{
				upgraded_a = true;
				json_a.clear();
				serialize_json(json_a);
			}
			return error;
		}

		mcp::json j_node_l = json_a["node"].get<mcp::json>();;
		error |= node.deserialize_json(j_node_l);
		mcp::json j_rpc_l = json_a["rpc"].get<mcp::json>();;
		error |= rpc.deserialize_json(j_rpc_l);
		mcp::json j_ws_l = json_a["ws"].get<mcp::json>();;
		error |= rpc_ws.deserialize_json(j_ws_l);
		mcp::json j_log_l = json_a["log"].get<mcp::json>();
		error |= logging.deserialize_json(j_log_l);
		mcp::json j_p2p_l = json_a["p2p"].get<mcp::json>();
		error |= p2p.deserialize_json(j_p2p_l);
		mcp::json j_witness_l = json_a["witness"].get<mcp::json>();
		error |= witness.deserialize_json(j_witness_l);
		mcp::json j_db_l = json_a["database"].get<mcp::json>();
		error |= db.deserialize_json(j_db_l);
    }
    catch (std::runtime_error const &)
    {
        error = true;
    }
    return error;
}

bool mcp_daemon::daemon_config::parse_old_version_data(mcp::json const & json_a, uint64_t const& version)
{
    auto error(false);
    try
    {
        if (!json_a.empty())
        {
			mcp::json j_node_l = json_a["node"].get<mcp::json>();;
			error |= node.parse_old_version_data(j_node_l, version);
			mcp::json j_rpc_l = json_a["rpc"].get<mcp::json>();;
			error |= rpc.parse_old_version_data(j_rpc_l, version);
			mcp::json j_ws_l = json_a["ws"].get<mcp::json>();;
			error |= rpc_ws.parse_old_version_data(j_ws_l, version);
			mcp::json j_log_l = json_a["log"].get<mcp::json>();
			error |= logging.parse_old_version_data(j_log_l, version);
			mcp::json j_p2p_l = json_a["p2p"].get<mcp::json>();
			error |= p2p.parse_old_version_data(j_p2p_l, version);
			mcp::json j_witness_l = json_a["witness"].get<mcp::json>();
			error |= witness.parse_old_version_data(j_witness_l, version);
			mcp::json j_db_l = json_a["database"].get<mcp::json>();
			error |= db.parse_old_version_data(j_db_l, version);
        }
    }
    catch (std::runtime_error const &)
    {
        error = true;
    }
    return error;
}

void mcp_daemon::daemon_config::readfile2bytes(dev::bytes &ret, boost::filesystem::path const&filepath)
{
    std::ifstream in;
    in.open(filepath.string(), std::ios_base::in | std::ios_base::binary);
    if (in.is_open())
    {
        size_t const c_elementSize = sizeof(typename dev::bytes::value_type);
        in.seekg(0, in.end);
        size_t length = in.tellg();
        if (length)
        {
            in.seekg(0, in.beg);
            ret.resize((length + c_elementSize - 1) / c_elementSize);
            in.read(const_cast<char*>(reinterpret_cast<char const*>(ret.data())), length);
        }
        in.close();
    }
}

void mcp_daemon::daemon_config::writebytes2file(dev::bytes & bytes, boost::filesystem::path const&filepath)
{
    std::ofstream out;
    out.open(filepath.string(), std::ios_base::out | std::ios_base::binary);
    if (out.is_open())
    {
        out.write(reinterpret_cast<char const*>(bytes.data()), bytes.size());
        return;
    }
}

void mcp_daemon::daemon_config::readfile2string(std::string & str, boost::filesystem::path const&filepath)
{
    std::ifstream in;
    in.open(filepath.string(), std::ios_base::in);
    if (in.is_open())
    {
        in >> str;
        in.close();
    }
}

void mcp_daemon::daemon_config::writestring2file(std::string const & str, boost::filesystem::path const & filepath)
{
    std::ofstream out;
    out.open(filepath.string(), std::ios_base::out | std::ios_base::binary);
    if (out.is_open())
    {
        out << str;
        out.close();
    }
}

void mcp_daemon::daemon_config::set_network(mcp::mcp_networks const & network_a)
{
	if (network_a != mcp::mcp_networks::mcp_live_network
		&& network_a != mcp::mcp_networks::mcp_beta_network
		&& network_a != mcp::mcp_networks::mcp_test_network
		&& network_a != mcp::mcp_networks::mcp_mini_test_network)
	{
		std::cerr << "Invalid config \"network\": " << (unsigned)network_a << std::endl;
		exit(1);
	}

	mcp::mcp_network = network_a;
	m_is_network_set = true;
}

void mcp_daemon::add_options(boost::program_options::options_description & description_a)
{
	description_a.add_options()
		("config", boost::program_options::value<std::string>(), "Config file")
		("network", boost::program_options::value<unsigned>(), "Network id");

    //node
    description_a.add_options()
        ("io_threads", boost::program_options::value<uint16_t>(), "Number of io threads")
        ("bg_threads", boost::program_options::value<uint16_t>(), "Number of background threads")
        ("sync_threads", boost::program_options::value<uint16_t>(), "Number of syncing threads");

    //rpc
    description_a.add_options()
        ("rpc", "Enable the HTTP-RPC server")
        ("rpc_addr", boost::program_options::value<std::string>(), "HTTP-RPC server listening interface (default: 127.0.0.1)")
        ("rpc_port", boost::program_options::value<uint16_t>(), "HTTP-RPC server listening port (default: 8765)")
        ("rpc_control", "Enable the HTTP-RPC write permission");

    //ws_rpc
    description_a.add_options()
        ("ws", "Enable the WS-RPC server")
        ("ws_addr", boost::program_options::value<std::string>(), "WS-RPC server listening interface (default: 127.0.0.1)")
        ("ws_port", boost::program_options::value<uint16_t>(), "WS-RPC server listening port (default: 8764)")
        ("ws_control", "Enable the WS-RPC write permission");

    //log
    description_a.add_options()
        ("console", "Enable output log to console")
        ("max_size", boost::program_options::value<uint64_t>(), "The maximum total size of rotated files")
        ("rotation_size", boost::program_options::value<uint64_t>(), "The size of the file at which rotation should occur")
        ("flush", "Automatically flush the file after each written record")
        ("verbosity", boost::program_options::value<std::string>(), "Logging verbosity: none,error,warning,info,debug,trace (default: info)")
        ("vmodule", boost::program_options::value<std::string>(), "Per-module verbosity: comma-separated list of <module>=<level>");

    //p2p
    description_a.add_options()
        ("addr", boost::program_options::value<std::string>(), "Network listening interface   (default: 127.0.0.1)")
        ("port", boost::program_options::value<uint16_t>(), "Network listening port (default: 30606)")
        ("max_peers", boost::program_options::value<uint16_t>(), "Maximum number of network peers (network disabled if set to 0) (default: 25)")
        ("bootstrap_nodes", boost::program_options::value<std::string>(), "Comma separated enode URLs for P2P discovery bootstrap")
        ("exemption_nodes", boost::program_options::value<std::string>(), "Comma separated enode URLs for P2P exemption node")
        ("nat", "Nat penetration");

    //witness
    description_a.add_options()
        ("witness", "Enable witness pattern")
        ("witness_account", boost::program_options::value<std::string>(), "Witness account or account file")
        ("password", boost::program_options::value<std::string>(), "Witness account password")
        ("last_block", boost::program_options::value<std::string>(), "Current witness account last block hash")
		("gas_price", boost::program_options::value<std::string>(), "witness account lowest gas price");

	//database
	description_a.add_options()
		("cache", boost::program_options::value<uint64_t>(), "database block cache")
		("write_buffer", boost::program_options::value<uint64_t>(), "database write buffer");
}

bool mcp_daemon::parse_command_to_config(mcp_daemon::daemon_config & config_a, boost::program_options::variables_map const & vm_a)
{
    bool error(false);

    //node
    if (vm_a.count("io_threads") > 0)
    {
        config_a.node.io_threads = vm_a["io_threads"].as<uint16_t>();
    }
    if (vm_a.count("bg_threads") > 0)
    {
        config_a.node.bg_threads = vm_a["bg_threads"].as<uint16_t>();
    }
    if (vm_a.count("sync_threads") > 0)
    {
        config_a.node.sync_threads = vm_a["sync_threads"].as<uint16_t>();
    }
    if (vm_a.count("work_threads") > 0)
    {
        config_a.node.work_threads = vm_a["work_threads"].as<uint16_t>();
    }

    //rpc
    if (vm_a.count("rpc")>0)
    {
        config_a.rpc.rpc_enable = true;
    }
    if (vm_a.count("rpc_addr"))
    {
        std::string address = vm_a["rpc_addr"].as<std::string>();
        boost::system::error_code ec;
        config_a.rpc.address = boost::asio::ip::address::from_string(address, ec);
        if (ec)
        {
            error = true;
        }
    }
    if (vm_a.count("rpc_port"))
    {
        config_a.rpc.port = vm_a["rpc_port"].as<uint16_t>();
    }
    if (vm_a.count("rpc_control")>0)
    {
        config_a.rpc.enable_control = true;
    }    

    //ws
    if (vm_a.count("ws")>0)
    {
        config_a.rpc_ws.rpc_ws_enable = true;
    }
    if (vm_a.count("ws_addr"))
    {
        std::string address = vm_a["ws_addr"].as<std::string>();
        boost::system::error_code ec;
        config_a.rpc_ws.address = boost::asio::ip::address::from_string(address, ec);
        if (ec)
        {
            error = true;
        }
    }
    if (vm_a.count("ws_port"))
    {
        config_a.rpc_ws.port = vm_a["ws_port"].as<uint16_t>();
    }
    if (vm_a.count("ws_control")>0)
    {
        config_a.rpc_ws.enable_control = true;
    }

    //log
    if (vm_a.count("console") > 0)
    {
        config_a.logging.log_to_console_value = true;
    }
    if (vm_a.count("max_size"))
    {
        config_a.logging.max_size = vm_a["max_size"].as<uint64_t>();
    }
    if (vm_a.count("rotation_size"))
    {
        config_a.logging.rotation_size = vm_a["rotation_size"].as<uint16_t>();
    }
    if (vm_a.count("flush"))
    {
        config_a.logging.flush = true;
    }
    if (vm_a.count("verbosity"))
    {
        std::string s_g_verbosity = vm_a["verbosity"].as<std::string>();
        config_a.logging.set_global_log_level(s_g_verbosity, false);
    }
    if (vm_a.count("vmodule"))
    {
        std::string s_vmodule = vm_a["vmodule"].as<std::string>();
        config_a.logging.set_module_log_level(s_vmodule, false);
    }
   
    //p2p
    if (vm_a.count("host"))
    {
        config_a.p2p.listen_ip = vm_a["host"].as<std::string>();
    }
    if (vm_a.count("port"))
    {
        config_a.p2p.port = vm_a["port"].as<uint16_t>();
    }
    if (vm_a.count("max_peers"))
    {
        config_a.p2p.max_peers = vm_a["max_peers"].as<uint32_t>();
    }
    if (vm_a.count("bootstrap_nodes"))
    {
        std::string s_bootstrap_nodes = vm_a["bootstrap_nodes"].as<std::string>();
        if (!s_bootstrap_nodes.empty())
        {
            config_a.p2p.bootstrap_nodes.clear();
            boost::split(config_a.p2p.bootstrap_nodes, s_bootstrap_nodes, boost::is_any_of(","));
        }
    }
    if (vm_a.count("exemption_nodes"))
    {
        std::string s_exemption_nodes = vm_a["exemption_nodes"].as<std::string>();
        if (!s_exemption_nodes.empty())
        {
            config_a.p2p.bootstrap_nodes.clear();
            boost::split(config_a.p2p.exemption_nodes, s_exemption_nodes, boost::is_any_of(","));
        }
    }
	if (vm_a.count("nat"))
	{
		config_a.p2p.nat = true;
	}

    //witness
    if (vm_a.count("witness")>0)
    {
        config_a.witness.is_witness = true;
    }
    if (vm_a.count("witness_account"))
    {
        config_a.witness.account_or_file = vm_a["witness_account"].as<std::string>();
    }
    if (vm_a.count("password"))
    {
        config_a.witness.password = vm_a["password"].as<std::string>();
    }

	//database
	if (vm_a.count("cache"))
	{
		config_a.db.cache_size = vm_a["cache"].as<uint32_t>();
	}
	if (vm_a.count("write_buffer"))
	{
		config_a.db.write_buffer_size = vm_a["write_buffer"].as<uint32_t>();
	}

    return error;
}

std::string mcp_daemon::get_home_directory(std::string path)
{
    if (path.size() > 0 && path[0] == '~')
    {
        char const* home = getenv("HOME");
        if (home || getenv("USERPROFILE") == home)
        {
            path.replace(0, 1, home);
        }
        else
        {
            char const* drive = getenv("HOMEDRIVE");
            char const* home_path = getenv("HOMEPATH");
            if (drive && home_path)
            {
                path.replace(0, 1, std::string(drive) + std::string(home_path));
            }
        }
    }
    return path;
}

void mcp_daemon::daemon::run(boost::filesystem::path const &data_path, boost::program_options::variables_map &vm)
{
    boost::filesystem::create_directories(data_path);

	boost::filesystem::path config_path;
	bool is_config_file(vm.count("config") > 0);
	if (is_config_file)
	{
		std::string config_path_str(get_home_directory(vm["config"].as<std::string>()));
		std::fstream config_stream;
		config_stream.open(config_path_str, std::ios_base::in);
		if (config_stream.fail())
		{
			std::cerr << "Error config file " << config_path_str << " not found" << std::endl;
			return;
		}
		config_stream.close();

		config_path = boost::filesystem::path(config_path_str);
	}
	else
		config_path = data_path / "config.json";
	
	mcp_daemon::daemon_config config;
	if (vm.count("network") > 0)
	{
		config.set_network((mcp::mcp_networks)vm["network"].as<unsigned>());
	}

	bool error(mcp::fetch_object(config, config_path, is_config_file));
	if (!error)
		error |= mcp_daemon::parse_command_to_config(config, vm);
	if (error)
	{
		std::cerr << "Error deserializing config, path:" << config_path << "\n";
		return;
	}

	//init log
	config.logging.init(data_path);
	mcp::log::init(config.logging);

	//default bootstrap nodes
	if (config.p2p.bootstrap_nodes.empty())
    {
        switch (mcp::mcp_network)
        {
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			break;
		}
        case mcp::mcp_networks::mcp_test_network:
        {
            break;
        }
        case mcp::mcp_networks::mcp_live_network:
        {
            break;
        }
        case mcp::mcp_networks::mcp_beta_network:
        {
            break;
        }
        default:
            break;
        }

    }
    
	//witness 	
	if (config.witness.is_witness)
	{
		if (config.witness.account_or_file.empty())
		{
			std::cerr << "witness need account or file\n ";
			return;
		}
		else
		{
			if (!mcp::isAddress(config.witness.account_or_file))
			{
				std::string path = get_home_directory(config.witness.account_or_file);
				config.witness.account_or_file.clear();
				config.readfile2string(config.witness.account_or_file, path);
				if (config.witness.account_or_file.empty())
				{
					std::cerr << "decode account error or file path error.\n ";
					return;
				}
			}

		}

		if (config.witness.password.empty())
		{
			config.witness.password = mcp::getPassword("Enter the current passphrase for the witness account:");
		}
	}

	boost::asio::io_service io_service;
	boost::asio::io_service sync_io_service;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> sync_io_service_work = boost::asio::make_work_guard(sync_io_service);

	boost::asio::io_service bg_io_service;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> bg_io_service_work = boost::asio::make_work_guard(bg_io_service);

	try
	{
		//node key
		boost::filesystem::path nodekey_path(data_path / "nodekey");
		std::string nodekey_str;
		config.readfile2string(nodekey_str, nodekey_path);

		dev::Secret seed;
		try {
			if (!nodekey_str.empty()) {
				seed = dev::Secret(nodekey_str);
			}
			else {
				throw "Nodekey is empty";
			}
		}
		catch (...) {
			mcp::random_pool.GenerateBlock((byte*)seed.data(), seed.size);
			config.writestring2file(dev::toHex(seed.ref()), nodekey_path);
		}

		mcp::db::database::init_table_cache(config.db.cache_size);

		///chain store
		mcp::block_store chain_store(error, data_path / "chaindb");
		if (error)
		{
			std::cerr << "chain_store initializing error\n";
			return;
		}
		///key manager store
		mcp::key_store key_store(error, data_path / "keydb");
		if (error)
		{
			std::cerr << "key_store initializing error\n";
			return;
		}

		///alarm
		std::shared_ptr<mcp::alarm> alarm(std::make_shared<mcp::alarm>(bg_io_service));
		std::shared_ptr<mcp::async_task> sync_async(std::make_shared<mcp::async_task>(sync_io_service));
		std::shared_ptr<mcp::async_task> background(std::make_shared<mcp::async_task>(bg_io_service));

		///key_manager
		std::shared_ptr<mcp::key_manager> key_manager(std::make_shared<mcp::key_manager>(data_path, key_store));

		///cache
		std::shared_ptr<mcp::block_cache> cache(std::make_shared<mcp::block_cache>(chain_store));

		mcp::param::init(cache);
		///chain
		std::shared_ptr<mcp::chain> chain(std::make_shared<mcp::chain>(chain_store, cache));

		///contract caller
		mcp::DENCaller = NewDENContractCaller(std::bind(&mcp::chain::call, chain, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		mcp::MainCaller = NewMainContractCaller(std::bind(&mcp::chain::call, chain, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		/// transaction queue
		std::shared_ptr<mcp::TransactionQueue> TQ(std::make_shared<mcp::TransactionQueue>(io_service, chain_store, cache, chain, sync_async));
		chain->set_TQ(TQ);
		/// approve queue
		std::shared_ptr<mcp::ApproveQueue> AQ(std::make_shared<mcp::ApproveQueue>(chain_store, cache, chain, sync_async));

		///validation
		std::shared_ptr<mcp::validation> validation(std::make_shared<mcp::validation>(chain_store, cache, TQ, AQ));
		///node_capability
		std::shared_ptr<mcp::node_capability> capability(std::make_shared<mcp::node_capability>(io_service, chain_store, cache, sync_async, TQ, AQ));
		TQ->set_capability(capability);
		AQ->set_capability(capability);

		///composer
		std::shared_ptr<mcp::composer> composer(std::make_shared<mcp::composer>(chain_store, cache, TQ, AQ));

		///sync
		std::shared_ptr<mcp::node_sync> sync(std::make_shared<mcp::node_sync>(capability, chain_store, chain, cache, TQ, AQ, sync_async, bg_io_service));
		capability->set_sync(sync);

		/// block processor
		std::shared_ptr<mcp::block_processor> processor(std::make_shared<mcp::block_processor>(error, chain_store, cache, chain, sync, capability, validation, sync_async, TQ, AQ, bg_io_service, alarm));
		if (error)
			return;
		capability->set_processor(processor);
		sync->set_processor(processor);

		///wallet
		std::shared_ptr<mcp::wallet> wallet(std::make_shared<mcp::wallet>(chain_store, cache, key_manager, TQ));
		//host
		std::shared_ptr<mcp::p2p::host> host(std::make_shared<mcp::p2p::host>(error, config.p2p, io_service, seed, data_path));
		if (error)
		{
			std::cerr << "host initializing error\n";
			return;
		}
		host->register_capability(capability);
		host->start();

		//witness node start
		std::shared_ptr<mcp::witness> witness = nullptr;
		if (config.witness.is_witness)
		{
			mcp::error_message error_msg;
			witness = std::make_shared<mcp::witness>(error_msg,
				key_manager, chain_store, alarm, composer, chain, processor, cache, TQ, AQ,
				config.witness.account_or_file, config.witness.password);

			if (error_msg.error)
			{
				std::cerr << error_msg.message << std::endl;
				return;
			}
			witness->start();
		}

		std::shared_ptr<mcp::rpc> rpc = get_rpc(
			chain_store, chain, cache, key_manager, wallet, host, background, composer,
			io_service, config.rpc
		);
		if (config.rpc.rpc_enable)
		{
			rpc->start();
		}
		else
		{
			LOG(m_log.info) << "RPC is disabled";
		}

		//std::shared_ptr<mcp::rpc_ws> rpc_ws = get_rpc_ws(io_service, background, config.rpc_ws);
		//if (config.rpc_ws.rpc_ws_enable)
		//{
		//	rpc_ws->start();
		//	rpc_ws->register_subscribe("new_block");
		//	rpc_ws->register_subscribe("stable_block");
		//	chain->set_ws_new_block_func(
		//		std::bind(&mcp::rpc_ws::on_new_block, rpc_ws, std::placeholders::_1)
		//	);
		//	chain->set_ws_stable_block_func(
		//		std::bind(&mcp::rpc_ws::on_stable_block, rpc_ws, std::placeholders::_1)
		//	);
		//	chain->set_ws_stable_mci_func(
		//		std::bind(&mcp::rpc_ws::on_stable_mci, rpc_ws, std::placeholders::_1)
		//	);
		//}
		//else
		//{
		//	LOG(m_log.info) << "WebSocket RPC is disabled";
		//}

		ongoing_report(chain_store, host, sync_async, background, cache,
			sync, processor, capability,chain, alarm, TQ, AQ, witness, m_log);

		std::unique_ptr<mcp::thread_runner> runner = std::make_unique<mcp::thread_runner>(io_service, config.node.io_threads, "io_service");
		std::unique_ptr<mcp::thread_runner> sync_runner = std::make_unique<mcp::thread_runner>(sync_io_service, config.node.sync_threads, "sync_io_service");
		std::unique_ptr<mcp::thread_runner> bg_runner = std::make_unique<mcp::thread_runner>(bg_io_service, config.node.bg_threads, "bg_io_service");

		runner->join();
		sync_runner->join();
		bg_runner->join();
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error while running node (" << e.what() << ")\n";
	}
}

void mcp_daemon::ongoing_report(
	mcp::block_store& store, std::shared_ptr<mcp::p2p::host> host,
	std::shared_ptr<mcp::async_task> sync_async, std::shared_ptr<mcp::async_task> background,
	std::shared_ptr<mcp::block_cache> cache, std::shared_ptr<mcp::node_sync> sync,
	std::shared_ptr<mcp::block_processor> processor, std::shared_ptr<mcp::node_capability> capability,
	std::shared_ptr<mcp::chain> chain, std::shared_ptr<mcp::alarm> alarm,
	std::shared_ptr<mcp::TransactionQueue> tq,
	std::shared_ptr<mcp::ApproveQueue> aq,
	std::shared_ptr<mcp::witness> witness,
	mcp::log& log
)
{
	auto map_peers_metrics = host->get_peers_metrics();
	for (auto i : map_peers_metrics)
	{
		auto p = i.second;
		if (p->write_write_queue_size > 0
			|| p->read_read_queue_size > 0
			|| p->write_queue_buffer_size > 0)
		{
			std::stringstream peer_metrics_info;
			peer_metrics_info << "peer:" << i.first;
			peer_metrics_info << ", write queue size:" << p->write_write_queue_size;
			peer_metrics_info << ", read queue size:" << p->read_read_queue_size;
			peer_metrics_info << ", write queue buffer size:" << p->write_queue_buffer_size;
			peer_metrics_info << ", send size:" << p->send_size;
			peer_metrics_info << ", send count:" << p->send_count;
			LOG(log.info) << peer_metrics_info.str();
		}
	}

	//io service sync
	LOG(log.info) << "sync_async: " << sync_async->get_size();
	//io service background
	LOG(log.info) << "background: " << background->get_size();


	LOG(log.info) << "block cache: " << cache->report_cache_size();

	LOG(log.info) << "container: "
		<< processor->get_processor_info();

	LOG(log.info) << "sync info:" << sync->get_sync_info();

	LOG(log.info) << "unhandle: "
		<< "unhandlde_size:" << processor->unhandle->unhandlde_size()
		<< ", dependency_size:" << processor->unhandle->dependency_size()
		<< ", missing_size:" << processor->unhandle->missing_size()
		<< ", light_missing_size:" << processor->unhandle->light_missing_size()
		<< ", approve_missing_size:" << processor->unhandle->approve_missing_size()
		<< ", tips_size:" << processor->unhandle->tips_size();

	LOG(log.info) << "block_processor dag_old_size: " << processor->dag_old_size
		<< " , light_old_size : " << processor->light_old_size
		<< " , base_validate_old_size : " << processor->base_validate_old_size;

	LOG(log.info) << "RequestingMageger info: " << mcp::RequestingMageger.get_info();

	LOG(log.info) << "peer count:" << host->peers().size();

	mcp::db::db_transaction transaction(store.create_transaction());
	size_t block_count(store.block_count(transaction));
	size_t stable_count(store.stable_block_count(transaction));
	size_t transaction_unstable_count(store.transaction_unstable_count(transaction));
	size_t transaction_count(store.transaction_count(transaction));
	size_t approve_unstable_count(store.approve_unstable_count(transaction));
	size_t approve_count(store.approve_count(transaction));
	size_t dag_free_count(store.dag_free_count(transaction));

	uint64_t last_mci = chain->last_mci();
	uint64_t last_stable_mci = chain->last_stable_mci();

	LOG(log.info) << "block:" << block_count
		<< ", unstable block:" << block_count - stable_count
		<< ", stable block:" << stable_count
		<< ", unstable transaction:" << transaction_unstable_count
		<< ", transaction:" << transaction_count
		<< ", unstable approve:" << approve_unstable_count
		<< ", approve:" << approve_count
		<< ", dag free:" << dag_free_count
		<< ", last_stable_mci:" << last_stable_mci
		<< ", last_mci:" << last_mci;

	LOG(log.info) << "TQ:" << tq->getInfo();
	LOG(log.info) << "AQ:" << aq->getInfo();

	LOG(log.info) << "capability send: "
		<< ", broadcast_joint:" << mcp::CapMetricsSend.broadcast_joint
		<< ", joint_request:" << mcp::CapMetricsSend.joint_request
		<< ", send_joint:" << mcp::CapMetricsSend.send_joint
		<< ", joint:" << mcp::CapMetricsSend.joint
		<< ", broadcast_transaction:" << mcp::CapMetricsSend.broadcast_transaction
		<< ", transaction_request:" << mcp::CapMetricsSend.transaction_request
		<< ", send_transaction:" << mcp::CapMetricsSend.send_transaction
		<< ", transaction:" << mcp::CapMetricsSend.transaction
		<< ", broadcast_approve:" << mcp::CapMetricsSend.broadcast_approve
		<< ", approve_request:" << mcp::CapMetricsSend.approve_request
		<< ", send_approve:" << mcp::CapMetricsSend.send_approve
		<< ", approve:" << mcp::CapMetricsSend.approve
		<< ", catchup_request:" << mcp::CapMetricsSend.catchup_request
		<< ", send_catchup:" << mcp::CapMetricsSend.send_catchup
		<< ", catchup_response:" << mcp::CapMetricsSend.catchup_response
		<< ", hash_tree_request:" << mcp::CapMetricsSend.hash_tree_request
		<< ", send_hash_tree:" << mcp::CapMetricsSend.send_hash_tree
		<< ", hash_tree_response:" << mcp::CapMetricsSend.hash_tree_response
		<< ", peer_info_request:" << mcp::CapMetricsSend.peer_info_request
		<< ", send_peer_info:" << mcp::CapMetricsSend.send_peer_info
		<< ", peer_info:" << mcp::CapMetricsSend.peer_info
		<< ", hello_info_request:" << mcp::CapMetricsSend.hello_info_request
		<< ", send_hello_info:" << mcp::CapMetricsSend.send_hello_info
		<< ", hello_info:" << mcp::CapMetricsSend.hello_info
		<< ", hello_info_ack:" << mcp::CapMetricsSend.hello_info_ack;
	LOG(log.info) << "capability receive: "
		<< ", joint_request:" << mcp::CapMetricsRecieved.joint_request
		<< ", send_joint:" << mcp::CapMetricsRecieved.send_joint
		<< ", joint:" << mcp::CapMetricsRecieved.joint
		<< ", transaction_request:" << mcp::CapMetricsRecieved.transaction_request
		<< ", send_transaction:" << mcp::CapMetricsRecieved.send_transaction
		<< ", transaction:" << mcp::CapMetricsRecieved.transaction
		<< ", approve_request:" << mcp::CapMetricsRecieved.approve_request
		<< ", send_approve:" << mcp::CapMetricsRecieved.send_approve
		<< ", approve:" << mcp::CapMetricsRecieved.approve
		<< ", catchup_request:" << mcp::CapMetricsRecieved.catchup_request
		<< ", send_catchup:" << mcp::CapMetricsRecieved.send_catchup
		<< ", catchup_response:" << mcp::CapMetricsRecieved.catchup_response
		<< ", hash_tree_request:" << mcp::CapMetricsRecieved.hash_tree_request
		<< ", send_hash_tree:" << mcp::CapMetricsRecieved.send_hash_tree
		<< ", hash_tree_response:" << mcp::CapMetricsRecieved.hash_tree_response
		<< ", peer_info_request:" << mcp::CapMetricsRecieved.peer_info_request
		<< ", send_peer_info:" << mcp::CapMetricsRecieved.send_peer_info
		<< ", peer_info:" << mcp::CapMetricsRecieved.peer_info
		<< ", hello_info_request:" << mcp::CapMetricsRecieved.hello_info_request
		<< ", send_hello_info:" << mcp::CapMetricsRecieved.send_hello_info
		<< ", hello_info:" << mcp::CapMetricsRecieved.hello_info
		<< ", hello_info_ack:" << mcp::CapMetricsRecieved.hello_info_ack;

	if (witness)
		LOG(log.info) << "witness:" << witness->getInfo();

	LOG(log.info) << store.get_rocksdb_state(32 * 1024 * 1024);

	auto elapseds = mcp::stopwatch_manager::list_elapseds();
	for (auto p : elapseds)
	{
		LOG(log.info) << p.first << ":" << p.second.count() / 1000 << "s";
	}

	alarm->add(std::chrono::steady_clock::now() + std::chrono::seconds(20), [&store, host, sync_async, background, cache,
		sync, processor, capability, chain, alarm, tq, aq, witness, &log]() {
		ongoing_report(store, host, sync_async, background, cache,
			sync, processor, capability, chain, alarm, tq, aq, witness, log);
	});
}



