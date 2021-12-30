#pragma once

#include <unordered_map>
#include <map>
#include <chrono>
#include <mutex>
#include <boost/optional/optional.hpp>

namespace mcp
{
    class stopwatch
    {
    public:
        stopwatch();
        std::chrono::milliseconds get_elapsed();
        void start();
        void stop();
        void reset();

	private:
		std::mutex m_mutex;
        boost::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> m_last_start_time;
        std::chrono::milliseconds m_last_elapsed;
	};

    class stopwatch_manager
    {
    public:
		static void start(std::string const & name);
		static void stop(std::string const & name);
		static void reset(std::string const & name);
		static void reset_all();
		static std::chrono::milliseconds get_elapsed(std::string const & name);
		static std::map<std::string, std::chrono::milliseconds> list_elapseds();
    private:
        static std::mutex m_stopwatchs_mutex;
		static std::unordered_map<std::string, mcp::stopwatch> m_stopwatchs;
    };

	class stopwatch_guard
	{
	public:
		stopwatch_guard(std::string const & name):
			m_name(name)
		{
			mcp::stopwatch_manager::start(m_name);
		}

		~stopwatch_guard()
		{
			mcp::stopwatch_manager::stop(m_name);
		}
	private:
		std::string m_name;
	};
}
