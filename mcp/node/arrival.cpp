#include "arrival.hpp"

void mcp::block_arrival::add(mcp::block_hash const & hash_a)
{
	std::lock_guard<std::mutex> lock(mutex);
	auto now(std::chrono::steady_clock::now());
	arrival.insert(mcp::block_arrival_info{ now, hash_a });
}

void mcp::block_arrival::remove(mcp::block_hash const & hash_a)
{
	std::lock_guard<std::mutex> lock(mutex);
	arrival.get<1>().erase(hash_a);
}

bool mcp::block_arrival::recent(mcp::block_hash const & hash_a)
{
	std::lock_guard<std::mutex> lock(mutex);
	auto now(std::chrono::steady_clock::now());
	while (!arrival.empty() && arrival.begin()->arrival + std::chrono::seconds(120) < now)
	{
		arrival.erase(arrival.begin());
	}
	bool exists = arrival.get<1>().find(hash_a) != arrival.get<1>().end();
	return exists;
}