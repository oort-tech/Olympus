#include "alarm.hpp"

bool mcp::operation::operator> (mcp::operation const & other_a) const
{
	return wakeup > other_a.wakeup;
}

mcp::alarm::alarm(boost::asio::io_service & service_a) :
	service(service_a),
	thread([this]() { run(); })
{
}

void mcp::alarm::stop()
{
	add(std::chrono::steady_clock::now(), nullptr);
	thread.join();
}

void mcp::alarm::run()
{
	std::unique_lock<std::mutex> lock(mutex);
	auto done(false);
	while (!done)
	{
		if (!operations.empty())
		{
			auto & operation(operations.top());
			if (operation.function)
			{
				if (operation.wakeup <= std::chrono::steady_clock::now())
				{
					service.post(operation.function);
					operations.pop();
				}
				else
				{
					auto wakeup(operation.wakeup);
					condition.wait_until(lock, wakeup);
				}
			}
			else
			{
				done = true;
			}
		}
		else
		{
			condition.wait(lock);
		}
	}
}

void mcp::alarm::add(std::chrono::steady_clock::time_point const & wakeup_a, std::function<void()> const & operation)
{
	std::lock_guard<std::mutex> lock(mutex);
	operations.push(mcp::operation({ wakeup_a, operation }));
	condition.notify_all();
}