#pragma once

#include "common.hpp"

namespace mcp
{
	class requesting_mageger
	{
	public:
		requesting_mageger();
		bool add(mcp::requesting_item& item_a, bool const& count_a = false);
		bool try_erase(h256 const& _h);
		std::list<requesting_item> clear_by_time(uint64_t const& time_a);
		uint64_t size() { return m_request_info.size(); }

		std::string get_info();
	private:
		boost::multi_index_container<
			mcp::requesting_item,
			boost::multi_index::indexed_by<
				boost::multi_index::hashed_unique<boost::multi_index::member<mcp::requesting_item, h256, &mcp::requesting_item::m_request_hash> >,
				boost::multi_index::hashed_non_unique<boost::multi_index::member<mcp::requesting_item, uint64_t, &mcp::requesting_item::m_time> >
			>
		> m_request_info;
		std::atomic<uint64_t> m_random_uint = { 0 };  ///random number, create request id

		static const int STALLED_TIMEOUT = 5000; ///retry time,external used
		static const int RETYR_TIMES = 3;

		mutable SharedMutex m_lock;  ///< General lock.
		///logs
		std::unordered_map<mcp::sub_packet_type, std::unordered_map<mcp::requesting_block_cause, uint64_t>> counts;
	};

	extern requesting_mageger RequestingMageger;
}
