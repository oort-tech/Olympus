#pragma once
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>

namespace mcp
{
	
    #define  LOG BOOST_LOG
	enum Verbosity
	{
		VerbositySilent = -1,
		VerbosityError = 0,
		VerbosityWarning = 1,
		VerbosityInfo = 2,
		VerbosityDebug = 3,
		VerbosityTrace = 4,
	};
	using Logger = boost::log::sources::severity_channel_logger<>;
	using Logger_mt = boost::log::sources::severity_channel_logger_mt<>;
	//not thread safe
    inline Logger createLogger(int _severity, std::string const& _channel)
	{
		return Logger(boost::log::keywords::severity = _severity, boost::log::keywords::channel = _channel);
	}
	//thread safe
	inline Logger_mt createLogger_mt(int _severity, std::string const& _channel)
	{
		return Logger_mt(boost::log::keywords::severity = _severity, boost::log::keywords::channel = _channel);
	}
	class logging;
	class log
	{
	public:
		log(mcp::logging &);
		Logger_mt Silent;
		Logger_mt Error;
		Logger_mt Warning;
		Logger_mt Info;
		Logger_mt Debug;
		Logger_mt Trace;
		Logger_mt thread;
		Logger_mt fuction;
	private:
		mcp::logging &m_logging;
	};






}



























