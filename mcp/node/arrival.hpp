#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <mcp/common/numbers.hpp>

namespace mcp
{
	class block_arrival_info
	{
	public:
		std::chrono::steady_clock::time_point arrival;
		mcp::block_hash hash;
	};
	class block_arrival
	{
	public:
		void add(mcp::block_hash const &);
		void remove(mcp::block_hash const & hash_a);
		bool recent(mcp::block_hash const & hash_a);
		boost::multi_index_container<
			mcp::block_arrival_info,
			boost::multi_index::indexed_by<
			boost::multi_index::ordered_non_unique<boost::multi_index::member<mcp::block_arrival_info, std::chrono::steady_clock::time_point, &mcp::block_arrival_info::arrival>>,
			boost::multi_index::hashed_unique<boost::multi_index::member<mcp::block_arrival_info, mcp::block_hash, &mcp::block_arrival_info::hash>>>>
			arrival;
		std::mutex mutex;
	};
}