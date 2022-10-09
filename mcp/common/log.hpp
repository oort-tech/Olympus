#pragma once
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <boost/date_time/posix_time/posix_time_system.hpp>
#include <boost/filesystem.hpp> 
#include <mcp/common/mcp_json.hpp>
namespace mcp
{	
    #define  LOG BOOST_LOG
	enum Verbosity
	{
        VerbosityNone = -1,
        VerbosityError = 0,
		VerbosityWarning = 1,
		VerbosityInfo = 2,
		VerbosityDebug = 3,
		VerbosityTrace = 4,
	};
	using Logger = boost::log::sources::severity_channel_logger<>;
	using Logger_mt = boost::log::sources::severity_channel_logger_mt<>;
	//not thread safe
    inline Logger create_logger(int _severity, std::string const& _channel)
	{
		return Logger(boost::log::keywords::severity = _severity, boost::log::keywords::channel = _channel);
	}
	//thread safe
	inline Logger_mt create_logger_mt(int _severity, std::string const& _channel)
	{
		return Logger_mt(boost::log::keywords::severity = _severity, boost::log::keywords::channel = _channel);
	}

	class logging
	{
	public:
		logging();
		void serialize_json(mcp::json &) const;
		bool deserialize_json(mcp::json const &);
		bool parse_old_version_data(mcp::json const&, uint64_t const&);

		bool log_to_console() const;
		void init(boost::filesystem::path const &);

		bool set_global_log_level(std::string const &, bool);
		void set_module_log_level(mcp::Verbosity const&);
		bool set_module_log_level(std::string const &, bool);


		bool log_to_console_value;
		bool flush;
		uintmax_t max_size;
		uintmax_t rotation_size;

		std::string static  level_string_arrry[6];
		std::map<std::string, mcp::Verbosity> static string_level_map;
		std::map<std::string, mcp::Verbosity>  modules_level;
		boost::filesystem::path log_path;
	private:
		std::string verbosity_value;
		mcp::Verbosity  g_verbosity;
		std::string vmodule_value;
	};

	class log
	{
	public:	
        explicit log(std::string const&);
		Logger_mt error;
		Logger_mt warning;
		Logger_mt info;
		Logger_mt debug;
		Logger_mt trace;
        static std::atomic_flag is_inited;
		static void init(mcp::logging const& logging_a);
	};

	extern mcp::log g_log;
}

























