#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

#include <boost/asio.hpp>

namespace mcp
{
	class operation
	{
	public:
		bool operator>(mcp::operation const &) const;
		std::chrono::steady_clock::time_point wakeup;
		std::function<void()> function;
	};

	class alarm
	{
	public:
		alarm(boost::asio::io_service &);
		~alarm() { stop(); }
		void add(std::chrono::steady_clock::time_point const &, std::function<void()> const &);
		void run();
		void stop();
		boost::asio::io_service &service;
		std::mutex mutex;
		std::condition_variable condition;
		std::priority_queue<operation, std::vector<operation>, std::greater<operation>> operations;
		std::thread thread;
	};
}