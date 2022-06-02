#include <mcp/rpc/rpc.hpp>
#include <mcp/rpc/rpc_ws.hpp>
#include <boost/program_options.hpp> 
#include <mcp/node/witness.hpp>

namespace mcp
{
	class thread_runner
	{
	public:
		thread_runner(boost::asio::io_service &, unsigned, std::string const &service_name);
		~thread_runner();
		void join();
		std::vector<std::thread> threads;
	};
}

namespace mcp_daemon
{
    void add_options(boost::program_options::options_description &);
	void ongoing_report(
		mcp::block_store& store, std::shared_ptr<mcp::p2p::host> host,
		std::shared_ptr<mcp::async_task> sync_async, std::shared_ptr<mcp::async_task> background,
		std::shared_ptr<mcp::block_cache> block_cache, std::shared_ptr<mcp::node_sync> sync,
		std::shared_ptr<mcp::block_processor> processor, std::shared_ptr<mcp::node_capability> capability,
		std::shared_ptr<mcp::chain> chain, std::shared_ptr<mcp::alarm> alarm,
		std::shared_ptr<mcp::TransactionQueue>,
		mcp::log& log
	);
    std::string get_home_directory(std::string path);
	class daemon
	{
	public:
		void run(boost::filesystem::path const &, boost::program_options::variables_map &vm);
		mcp::log m_log = { mcp::log("node") };
	};

	class thread_config
	{
	public:
		thread_config();
		void serialize_json(mcp::json &) const;
		bool deserialize_json(mcp::json const &);
		bool parse_old_version_data(mcp::json const &, uint64_t const&);
		std::vector<std::pair<boost::asio::ip::address, uint16_t>> work_peers;
		unsigned io_threads;
		unsigned bg_threads;
		unsigned sync_threads;
		unsigned work_threads;
	};
	class daemon_config
	{
	public:
		daemon_config();

        //bool upgrade_json(mcp::json &, std::string const&);
        bool parse_old_version_data(mcp::json const&, uint64_t const&);

        bool deserialize_json(bool &, mcp::json &);
        void serialize_json(mcp::json &);

		void readfile2bytes(dev::bytes &,boost::filesystem::path const&);
		void writebytes2file(dev::bytes &, boost::filesystem::path const &);
		void readfile2string(std::string & ret, boost::filesystem::path const & filepath);
		void writestring2file(std::string const & str, boost::filesystem::path const & filepath);
		
		void set_network(mcp::mcp_networks const & network_a);

        thread_config node;
        mcp::rpc_config rpc;
		mcp::rpc_ws_config rpc_ws;
        mcp::logging logging;
        mcp::p2p::p2p_config p2p;
        mcp::witness_config witness;
		mcp::db::database_config db;

	private:
		uint64_t m_current_version = 3;
		bool m_is_network_set = false;
	};

    bool parse_command_to_config(mcp_daemon::daemon_config &, boost::program_options::variables_map const &);
}
