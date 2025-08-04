#pragma once
#include <mcp/common/numbers.hpp>
#include <mcp/common/mcp_json.hpp>
#include <mcp/core/log_entry.hpp>
#include <libdevcore/TrieHash.h>
#include "transaction.hpp"
#include "approve.hpp"
#include "common.hpp"

//const dev::h256 ZeroUnclesSha3("0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421");

namespace mcp
{
	class block
	{
	public:
		block()= default;
		block(dev::Address from, mcp::block_hash const & previous, std::vector<mcp::block_hash> const & parents, h256s links, h256s approves,
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
		h256s const& approves() { return  m_approves; }
		mcp::block_hash const& last_summary() { return  m_last_summary; }
		mcp::block_hash const& last_summary_block() { return  m_last_summary_block; }
		mcp::block_hash const& last_stable_block() { return  m_last_stable_block; }
		uint64_t const& exec_timestamp() { return  m_exec_timestamp; }

		/// @returns the signature of the transaction (the signature has the sender encoded in it)
		/// @throws TransactionIsUnsigned if signature was not initialized
		SignatureStruct const& signature() const { return m_vrs; };

		mcp::block_hash root() const;

		void init_from_genesis_transaction(dev::Address const& from, h256s const& hashes, std::string time);
	private:
		dev::Address m_from;// Other fields maybe same with other witness,cause same hash. so that, block must used from fields
		mcp::block_hash m_previous;
		std::vector<mcp::block_hash> m_parents;
		h256s m_links;
		h256s m_approves;
		mcp::block_hash m_last_summary;
		mcp::block_hash m_last_summary_block;
		mcp::block_hash m_last_stable_block;
		uint64_t m_exec_timestamp;
		SignatureStruct m_vrs;	///< The signature of the transaction. Encodes the sender.

		mutable mcp::block_hash m_hashWith = mcp::block_hash(0);  ///< Cached hash of block 
	};

	class LocalisedBlock : public block
	{
	public:
		LocalisedBlock(
			block const& _b,
			uint64_t const& _blockNumber,
			mcp::Transactions const& _txs,
			dev::h256 const& _stateRoot,
			dev::h256 const& _receiptsRoot,
			dev::h256 const& _parent
		) :
			block(_b),
			m_blockNumber(_blockNumber),
			m_txs(_txs),
			m_stateRoot(_stateRoot),
			m_receiptsRoot(_receiptsRoot),
			m_parent(_parent),
			m_sha3Uncles(dev::EmptyListSHA3)
		{
			std::vector<bytes> transactionsRoot;
			for (size_t i = 0; i < _txs.size(); i++)
			{
				m_gasUsed += _txs[i].gas();
				m_minGasPrice = m_minGasPrice == 0 ? _txs[i].gasPrice() : std::min(m_minGasPrice, _txs[i].gasPrice());
				transactionsRoot.push_back(_txs[i].sha3().asBytes());
			}
			m_transactionsRoot = dev::orderedTrieRoot(transactionsRoot);
		}

		uint64_t blockNumber() const { return m_blockNumber; }
		dev::u256 gasUsed() const { return m_gasUsed; }
		dev::u256 minGasPrice() const { return m_minGasPrice; }
		mcp::Transactions transactions() const { return m_txs; }
		dev::h256 parent() const { return m_parent; }
		dev::h256 sha3Uncles() const { return m_sha3Uncles; }
		log_bloom const& bloom() const { return m_bloom; }
		dev::h256 transactionsRoot() const { return m_transactionsRoot; }
		dev::h256 stateRoot() const { return m_stateRoot; }
		dev::h256 receiptsRoot() const { return m_receiptsRoot; }
		size_t size() const { RLPStream s; streamRLP(s); return s.out().size(); }
		dev::h256s uncles() const { return m_uncles; }
	private:
		uint64_t m_blockNumber;
		dev::u256 m_gasUsed;
		dev::u256 m_minGasPrice;
		mcp::Transactions m_txs;
		dev::h256 m_parent;
		dev::h256 m_sha3Uncles;
		dev::h256 m_transactionsRoot;
		dev::h256 m_stateRoot;
		dev::h256 m_receiptsRoot;
		dev::h256s m_uncles;
		log_bloom m_bloom;
	};
}
