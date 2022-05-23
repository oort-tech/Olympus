#pragma once
#include <mcp/common/numbers.hpp>
#include <mcp/common/mcp_json.hpp>
#include "transaction.hpp"


namespace mcp
{
	class block
	{
	public:
		block()= default;
		block(dev::Address from, mcp::block_hash const & previous, std::vector<mcp::block_hash> const & parents, h256s links,
			mcp::block_hash const & last_summary, mcp::block_hash const & last_summary_block, mcp::block_hash const & last_stable_block,
			uint64_t const & exec_timestamp, dev::Secret const& s);
		block(dev::RLP const & rlp);
		void streamRLP(dev::RLPStream & s, IncludeSignature sig = WithSignature) const;

		virtual ~block() = default;
		mcp::block_hash & hash() const;

		dev::Address const& from() { return m_from; }
		mcp::block_hash const& previous() { return  m_previous; }
		std::vector<mcp::block_hash> const& parents() { return  m_parents; }
		h256s const& links() { return  m_links; }
		mcp::block_hash const& last_summary() { return  m_last_summary; }
		mcp::block_hash const& last_summary_block() { return  m_last_summary_block; }
		mcp::block_hash const& last_stable_block() { return  m_last_stable_block; }
		uint64_t const& exec_timestamp() { return  m_exec_timestamp; }

		/// @returns the signature of the transaction (the signature has the sender encoded in it)
		/// @throws TransactionIsUnsigned if signature was not initialized
		SignatureStruct const& signature() const { return m_vrs; };

		mcp::block_hash root() const;

		void init_from_genesis_transaction(dev::Address const& from, h256 const& hash, std::string time);

		//bool operator== (mcp::block const &) const;

		void serialize_json(std::string &) const;
		void serialize_json(mcp::json & json_a) const;

		// added by michael at 2/7
		void serialize_json_eth(std::string &) const;
		void serialize_json_eth(mcp::json & json_a) const;

		///// @returns v value of the transaction (has chainID and recoveryID encoded in it)
		///// @throws TransactionIsUnsigned if signature was not initialized
		//u256 rawV() const;
	private:
		dev::Address m_from;// Other fields maybe same with other witness,cause same hash. so that, block must used from fields
		mcp::block_hash m_previous;
		std::vector<mcp::block_hash> m_parents;
		h256s m_links;
		mcp::block_hash m_last_summary;
		mcp::block_hash m_last_summary_block;
		mcp::block_hash m_last_stable_block;
		uint64_t m_exec_timestamp;
		SignatureStruct m_vrs;	///< The signature of the transaction. Encodes the sender.

		mutable mcp::block_hash m_hashWith = 0;  ///< Cached hash of block 
	};
}
