#pragma once
#include <libdevcore/Address.h>
#include "common.hpp"
#include "transaction_receipt.hpp"

namespace mcp
{
	// FilterQuery contains options for contract log filtering.
	class LogFilter
	{
	public:
		LogFilter(mcp::BlockNumber _from = mcp::PendingBlock, mcp::BlockNumber _to = mcp::LatestBlock) : m_fromBlock(_from), m_toBlock(_to) {}

		/// Range filter is a filter which doesn't care about addresses or topics
		/// Matches are all entries from earliest to latest
		/// @returns true if addresses and topics are unspecified
		bool isRangeFilter() const;

		bool matches(log_bloom _bloom) const;
		log_entries matches(dev::eth::TransactionReceipt const& _r, uint64_t const& _mci) const;

		dev::h256 blockHash() const { return m_blockHash; }
		mcp::BlockNumber fromBlock() const { return m_fromBlock; }
		mcp::BlockNumber toBlock() const { return m_toBlock; }

		LogFilter withAddress(Address _a) { m_addresses.insert(_a); return *this; }
		LogFilter withTopic(unsigned _index, h256 const& _t) { if (_index < 4) m_topics[_index].insert(_t); return *this; }
		LogFilter withFrom(mcp::BlockNumber _b) { m_fromBlock = _b; return *this; }
		LogFilter withTo(mcp::BlockNumber _b) { m_toBlock = _b; return *this; }
		LogFilter withBlockHash(dev::h256 const& _h) { m_blockHash = _h; return *this; }

	private:
		dev::h256 m_blockHash;		// used by eth_getLogs, return logs only from block with this hash
		mcp::BlockNumber m_fromBlock;	// beginning of the queried range, nil means genesis block
		mcp::BlockNumber m_toBlock;	// end of the range, nil means latest block
		dev::AddressHash m_addresses;	// restricts matches to events created by specific contracts
		// The Topic list restricts matches to particular event topics. Each event has a list
		// of topics. Topics matches a prefix of that list. An empty element slice matches any
		// topic. Non-empty elements represent an alternative that matches any of the
		// contained topics.
		//
		// Examples:
		// {} or nil          matches any topic list
		// {{A}}              matches topic A in first position
		// {{}, {B}}          matches any topic in first position AND B in second position
		// {{A}, {B}}         matches topic A in first position AND B in second position
		// {{A, B}, {C, D}}   matches topic (A OR B) in first position AND (C OR D) in second position
		std::array<dev::h256Hash, 4> m_topics;
	};
}