#pragma once

#include <boost/log/trivial.hpp>
#include <boost/stacktrace.hpp>

#define assert_x(expression) if(!(expression)) { BOOST_LOG_TRIVIAL(error) << "Assert failed: " #expression << "," << __FILE__ << "," <<  __LINE__  << std::endl << boost::stacktrace::stacktrace(); throw std::runtime_error("assert_x:" #expression); }
#define assert_x_msg(expression, msg) if(!(expression)) { BOOST_LOG_TRIVIAL(error) << "Assert failed: " #expression << "," << __FILE__ << "," <<  __LINE__ << ",Message:" << msg << std::endl << boost::stacktrace::stacktrace() ; throw std::runtime_error(msg); }
