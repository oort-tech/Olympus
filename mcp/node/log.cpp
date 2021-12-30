//log.cpp
#include "log.hpp"
#include <mcp/node/node.hpp>

#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/function.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/exception_handler.hpp>

#if defined(NDEBUG)
#include <boost/log/sinks/async_frontend.hpp>
template <class T>
using log_sink = boost::log::sinks::asynchronous_sink<T>;
#else
#include <boost/log/sinks/sync_frontend.hpp>
template <class T>
using log_sink = boost::log::sinks::synchronous_sink<T>;
#endif

BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(context, "Context", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(threadName, "ThreadName", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

mcp::log::log(mcp::logging & logging_a):m_logging(logging_a)
{
	Silent   = createLogger_mt(mcp::VerbositySilent,"Silent");
	Error    = createLogger_mt(mcp::VerbosityError,"Error");
	Warning  = createLogger_mt(mcp::VerbosityWarning,"Warning");
	Info     = createLogger_mt(mcp::VerbosityInfo,"Info");
	Debug    = createLogger_mt(mcp::VerbosityDebug,"Debug");
	Trace    = createLogger_mt(mcp::VerbosityTrace,"Trace");
	thread   = createLogger_mt(mcp::VerbosityTrace, "Trace");
	fuction  = createLogger_mt(mcp::VerbosityTrace, "Trace");

	auto sink = boost::make_shared<log_sink<boost::log::sinks::text_ostream_backend>>();

	boost::log::formatter fmt = boost::log::expressions::stream
		<< "  " << boost::log::expressions::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S")
		<< " " << " [" << channel <<"]"
		<< " " << boost::log::expressions::smessage;
	sink->set_formatter(fmt);
	sink->set_filter([=](boost::log::attribute_value_set const& _set) {
		if (_set["Severity"].extract<int>() > m_logging.log_level)
			return false;
	});
	boost::log::core::get()->add_sink(sink);

}