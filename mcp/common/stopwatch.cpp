#include "stopwatch.hpp"

mcp::stopwatch::stopwatch() :
    m_last_start_time(boost::none),
    m_last_elapsed(0)
{

}

void mcp::stopwatch::start()
{
	std::lock_guard<std::mutex> lock(m_mutex);
    m_last_start_time = std::chrono::high_resolution_clock::now();
}

void mcp::stopwatch::stop()
{
	std::lock_guard<std::mutex> lock(m_mutex);
    if(m_last_start_time)
    {
	    m_last_elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - *m_last_start_time);
        m_last_start_time = boost::none;
    }
}

void mcp::stopwatch::reset()
{
	std::lock_guard<std::mutex> lock(m_mutex);
    m_last_elapsed = std::chrono::milliseconds(0);
    m_last_start_time = boost::none;
}

std::chrono::milliseconds mcp::stopwatch::get_elapsed()
{
	std::lock_guard<std::mutex> lock(m_mutex);
    if(m_last_start_time)
        return m_last_elapsed + std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - *m_last_start_time);
    else
        return m_last_elapsed;
}

void mcp::stopwatch_manager::start(std::string const & name)
{
    std::lock_guard<std::mutex> lock(m_stopwatchs_mutex);
    m_stopwatchs[name].start();
}

void mcp::stopwatch_manager::stop(std::string const & name)
{
    std::lock_guard<std::mutex> lock(m_stopwatchs_mutex);
    if(m_stopwatchs.count(name))
        m_stopwatchs[name].stop();
}

void mcp::stopwatch_manager::reset(std::string const & name)
{
    std::lock_guard<std::mutex> lock(m_stopwatchs_mutex);
    if(m_stopwatchs.count(name))
        m_stopwatchs[name].reset();
}

void mcp::stopwatch_manager::reset_all()
{
	std::lock_guard<std::mutex> lock(m_stopwatchs_mutex);
	for (auto it = m_stopwatchs.begin(); it != m_stopwatchs.end(); it++)
	{
		it->second.reset();
	}
}

std::chrono::milliseconds mcp::stopwatch_manager::get_elapsed(std::string const & name)
{
    std::lock_guard<std::mutex> lock(m_stopwatchs_mutex);
    if(m_stopwatchs.count(name))
        return m_stopwatchs[name].get_elapsed();
    return std::chrono::milliseconds(0);
}

std::map<std::string, std::chrono::milliseconds> mcp::stopwatch_manager::list_elapseds()
{
    std::map<std::string, std::chrono::milliseconds> elapseds;
    std::lock_guard<std::mutex> lock(m_stopwatchs_mutex);
    for(auto it = m_stopwatchs.begin(); it != m_stopwatchs.end(); it++)
    {
        elapseds[it->first] = it->second.get_elapsed();
    }
    return elapseds;
}

std::mutex mcp::stopwatch_manager::m_stopwatchs_mutex;
std::unordered_map<std::string, mcp::stopwatch> mcp::stopwatch_manager::m_stopwatchs;