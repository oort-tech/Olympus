#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace mcp
{
	class async_task
	{
	public:
		async_task(boost::asio::io_service &io_service_a) : io_service(io_service_a){}

		template <typename T>
		void sync_async(T action_a)
		{
			m_add++;
			io_service.post([this, action_a]()
			{
				action_a();
				m_reduce++;
			});
		}
		std::string get_size()
		{
			std::string ret = "count:" + std::to_string(m_add - m_reduce)
				+ " ,add:" + std::to_string(m_add)
				+ " ,reduce:" + std::to_string(m_reduce);
			return ret;
		}
	private:
		boost::asio::io_service& io_service;

		std::atomic<int> m_add{ 0 };
		std::atomic<int> m_reduce{ 0 };
	};
}
