//log.cpp

#include <algorithm> 

#include "log.hpp"
#include <mcp/common/assert.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/function.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/expressions/predicates/begins_with.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
//#if defined(NDEBUG)
//#include <boost/log/sinks/async_frontend.hpp>
//template <class T>
//using log_sink = boost::log::sinks::asynchronous_sink<T>;
//#else
//#include <boost/log/sinks/sync_frontend.hpp>
//template <class T>
//using log_sink = boost::log::sinks::synchronous_sink<T>;
//#endif

BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", mcp::Verbosity)


mcp::logging::logging() :
	log_to_console_value(false),
	max_size(16 * 10 * 1024 * 1024),
	rotation_size(10 * 1024 * 1024),
	flush(true),
	verbosity_value("info"),
	vmodule_value(""),
	g_verbosity(mcp::Verbosity::VerbosityInfo)
{
	assert_x(string_level_map.count(verbosity_value));
	g_verbosity = string_level_map[verbosity_value];
	modules_level["node"] = g_verbosity;
	modules_level["p2p"] = g_verbosity;
	modules_level["rpc"] = g_verbosity;
	modules_level["pow"] = g_verbosity;
	modules_level["sync"] = g_verbosity;
	modules_level["db"] = g_verbosity;
	modules_level["vm"] = g_verbosity;
}

bool mcp::logging::set_global_log_level(std::string const & verbosity_a, bool is_p_error_a)
{
	bool error(false);
	auto it_r = string_level_map.find(verbosity_a);
	if (it_r == string_level_map.end())
	{
		if (is_p_error_a)
		{
			std::cerr << "verbosity:" << verbosity_value << " not exsist,please check config file.\n";
		}
		error = true;
		return error;
	}

	g_verbosity = it_r->second;
	set_module_log_level(g_verbosity);
	verbosity_value = verbosity_a;

	return error;
}

void mcp::logging::set_module_log_level(mcp::Verbosity const & verbosity_a)
{
	for (auto it = modules_level.begin(); it != modules_level.end(); it++)
	{
		it->second = verbosity_a;
	}
}

bool mcp::logging::set_module_log_level(std::string const & vmodule_value_a, bool is_p_error_a)
{
	bool error(false);
	if (!vmodule_value_a.empty())
	{
		std::vector<std::string> vec_vmodule;
		boost::split(vec_vmodule, vmodule_value_a, boost::is_any_of("=,"));
		if ((vec_vmodule.size() % 2) || (vec_vmodule.size() > modules_level.size() * 2))
		{
			if (is_p_error_a)
			{
				std::cerr << "vmodule formatter not valid ,please check config file.\n";
			}
			error = true;
			return error;
		}
		for (size_t i = 0; i < vec_vmodule.size(); i = i + 2)
		{
			auto it_modules_level = modules_level.find(vec_vmodule[i]);
			if (it_modules_level == modules_level.end())
			{
				if (is_p_error_a)
				{
					std::cerr << vec_vmodule[i] << " not exsist in module,please check config file\n.";
				}
				error = true;
				return error;
			}
			auto it_string_level = string_level_map.find(vec_vmodule[i + 1]);
			if (it_string_level == string_level_map.end())
			{
				if (is_p_error_a)
				{
					std::cerr << vec_vmodule[i + 1] << " not exsist in verbosity ,please check config file\n";
				}
				error = true;
				return error;
			}
			it_modules_level->second = it_string_level->second;
		}
		vmodule_value = vmodule_value_a;
	}
	return error;
}

void mcp::logging::init(boost::filesystem::path const & application_path_a)
{
	log_path = application_path_a;
}

std::string mcp::logging::level_string_arrry[6] = { "none","error","warning","info","debug","trace" };

std::map<std::string, mcp::Verbosity>  mcp::logging::string_level_map = { { "none"   ,  mcp::Verbosity::VerbosityNone },
{ "error"  ,  mcp::Verbosity::VerbosityError },
{ "warning",  mcp::Verbosity::VerbosityWarning },
{ "info"   ,  mcp::Verbosity::VerbosityInfo },
{ "debug"  ,  mcp::Verbosity::VerbosityDebug },
{ "trace"  ,  mcp::Verbosity::VerbosityTrace } };

void mcp::logging::serialize_json(mcp::json & json_a) const
{
	json_a["console"] = log_to_console_value ? "true" : "false";
	json_a["max_size"] = max_size;
	json_a["rotation_size"] = rotation_size;
	json_a["flush"] = flush ? "true" : "false";
	json_a["verbosity"] = verbosity_value;
	json_a["vmodule"] = vmodule_value;
}

bool mcp::logging::parse_old_version_data(mcp::json const &json_a, uint64_t const& version)
{
	auto error(false);
	try
	{
		if (version < 2)
		{
			if (json_a.count("node") && json_a["node"].is_object())
			{
				mcp::json j_node_l = json_a["node"].get<mcp::json>();

				if (j_node_l.count("logging") && j_node_l["logging"].is_object())
				{
					mcp::json j_logging_l = j_node_l["logging"].get<mcp::json>();

					if (j_logging_l.count("log_to_cerr") && j_logging_l["log_to_cerr"].is_string())
					{
						log_to_console_value = (j_logging_l["log_to_cerr"].get<std::string>() == "true" ? true : false);
					}

					if (j_logging_l.count("max_size") && j_logging_l["max_size"].is_string())
					{
						std::string max_size_text = j_logging_l["max_size"].get<std::string>();
						try
						{
							max_size = std::stoull(max_size_text);
						}
						catch (const std::exception&)
						{
							error = true;
						}
					}

					if (j_logging_l.count("rotation_size") && j_logging_l["rotation_size"].is_string())
					{
						std::string rotation_size_text = j_logging_l["rotation_size"].get<std::string>();
						try
						{
							rotation_size = std::stoull(rotation_size_text);
						}
						catch (const std::exception&)
						{
							error = true;
						}
					}

					if (j_logging_l.count("flush") && j_logging_l["flush"].is_string())
						flush = (j_logging_l["flush"].get<std::string>() == "true" ? true : false);
					else
						flush = true;
				}
				else
					error = true;
			}
			else
				error = true;
		}
		else
		{
			if (json_a.count("log") && json_a["log"].is_object())
			{
				mcp::json j_log_l = json_a["log"].get<mcp::json>();
				error |= deserialize_json(j_log_l);
			}
			else
				error = true;
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

bool mcp::logging::deserialize_json(mcp::json const & json_a)
{
	auto error(false);
	try
	{
		if (json_a.count("console") && json_a["console"].is_string())
		{
			log_to_console_value = (json_a["console"].get<std::string>() == "true" ? true : false);
		}
		else
		{
			error = true;
		}

		if (json_a.count("verbosity") && json_a["verbosity"].is_string())
		{
			verbosity_value = json_a["verbosity"].get<std::string>();
			error |= set_global_log_level(verbosity_value, true);
		}
		else
		{
			error = true;
		}

		if (json_a.count("vmodule") && json_a["vmodule"].is_string())
		{
			vmodule_value = json_a["vmodule"].get<std::string>();
			error |= set_module_log_level(vmodule_value, true);
		}
		else
		{
			error = true;
		}

		if (json_a.count("max_size") && json_a["max_size"].is_number_unsigned())
		{
			max_size = json_a["max_size"].get<uintmax_t>();
		}
		else
		{
			error = true;
		}

		if (json_a.count("rotation_size") && json_a["rotation_size"].is_number_unsigned())
		{
			rotation_size = json_a["rotation_size"].get<uintmax_t>();
		}
		else
		{
			rotation_size = 4194304;
		}

		if (json_a.count("flush") && json_a["flush"].is_string())
		{
			flush = (json_a["flush"].get<std::string>() == "true" ? true : false);
		}
		else
		{
			flush = true;
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

bool mcp::logging::log_to_console() const
{
	return log_to_console_value;
}

std::atomic_flag mcp::log::is_inited = ATOMIC_FLAG_INIT;
mcp::log::log(std::string const& channel_name)
{
	error    = create_logger_mt(mcp::VerbosityError, channel_name);
	warning  = create_logger_mt(mcp::VerbosityWarning, channel_name);
	info     = create_logger_mt(mcp::VerbosityInfo, channel_name);
	debug    = create_logger_mt(mcp::VerbosityDebug, channel_name);
	trace    = create_logger_mt(mcp::VerbosityTrace, channel_name);
}

boost::posix_time::time_facet *g_ptf = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S");
void sink_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& _formatos)
{
    boost::posix_time::ptime const&  x = rec[timestamp].get();
    std::tm tm_time = boost::posix_time::to_tm(x);
    //if you adjust time formatter,must adjust sztime.
    char sztime[22] = "";
    sprintf(sztime, "[%4d-%02d-%02d %02d:%02d:%02d]", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    _formatos << sztime;
    boost::log::attribute_value_set attr_set = rec.attribute_values();
    auto const & message_channel = rec[channel];
    std::string log_level;
    //no messageChannel boost_log_trival
    if (message_channel)
    {
        auto it_severity = attr_set.find("Severity");
        if (it_severity != attr_set.end())
        {

            auto second = it_severity->second;
            auto severity_rec_ref = second.extract_or_default<uint8_t>(0);
            if (severity_rec_ref)
            {
                uint8_t u_level = severity_rec_ref.get<int>();
                if (0 <= u_level && u_level <= 4)
                {
                    log_level = mcp::logging::level_string_arrry[u_level + 1];
                }
            }
        }
        _formatos << "[" << rec[channel] << "]";
        _formatos << "[" << log_level << "]";
    }
    _formatos << rec[boost::log::expressions::smessage];
}

void mcp::log::init(mcp::logging const &logging_a)
{
    if (!mcp::log::is_inited.test_and_set())
    {
        boost::log::filter flt = [&](boost::log::attribute_value_set const& _set) {
            bool nret(false);      
            auto const message_channel = _set[channel];
            //all record has Channel attre 
            // if without record is old , print
            if (!message_channel)
                return true;
            auto it = logging_a.modules_level.find(message_channel.get());
            if ( (it != logging_a.modules_level.end()) && (_set["Severity"].extract<int>()<= it->second))
            {
                nret = true;
            }
            return nret;
        };
        boost::log::add_common_attributes();

        //set time formatter
        std::locale::global(std::locale(std::cout.getloc(), g_ptf));

        if (logging_a.log_to_console())
        {
            auto psink_console = boost::log::add_console_log(std::cerr);
            psink_console->set_formatter(&sink_formatter);
            psink_console->set_filter(flt);
            psink_console->set_exception_handler(
                boost::log::make_exception_handler<std::exception>([](std::exception const& _ex) {
                std::cerr << "Exception from the logging library: " << _ex.what() << '\n';
            }));
        }
        auto psink_file = boost::log::add_file_log(boost::log::keywords::target = logging_a.log_path / "log",
            boost::log::keywords::file_name = logging_a.log_path / "log" / "log_%Y-%m-%d_%H-%M-%S.%N.log",
            boost::log::keywords::rotation_size = logging_a.rotation_size,
            boost::log::keywords::auto_flush = logging_a.flush,
            boost::log::keywords::scan_method = boost::log::sinks::file::scan_method::scan_matching,
            boost::log::keywords::max_size = logging_a.max_size );
        psink_file->set_formatter(&sink_formatter);
        psink_file->set_filter(flt);
        psink_file->set_exception_handler(
                            boost::log::make_exception_handler<std::exception>([](std::exception const& _ex) {
                            std::cerr << "Exception from the logging library: " << _ex.what() << '\n';
                     }));
    }
}