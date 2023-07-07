#include "blocks.hpp"
#include "common.hpp"
#include "config.hpp"
#include <libdevcore/CommonIO.h>
#include <mcp/common/common.hpp>

mcp::block::block(dev::Address from, mcp::block_hash const & previous, std::vector<mcp::block_hash> const & parents, h256s links, h256s approves,
	mcp::block_hash const & last_summary, mcp::block_hash const & last_summary_block, mcp::block_hash const & last_stable_block,
	uint64_t const & exec_timestamp, dev::Secret const& s) :
	m_from(from),
	m_previous(previous),
	m_parents(parents),
	m_links(links),
	m_approves(approves),
	m_last_summary(last_summary),
	m_last_summary_block(last_summary_block),
	m_last_stable_block(last_stable_block),
	m_exec_timestamp(exec_timestamp)
{
	if (s)
	{
		auto sig = dev::sign(s, hash());
		dev::SignatureStruct sigStruct = *(dev::SignatureStruct const*)&sig;
		if (sigStruct.isValid())
			m_vrs = sigStruct;
	}
}

mcp::block::block(dev::RLP const & rlp)
{
	try
	{
		if (!rlp.isList())
			BOOST_THROW_EXCEPTION(InvalidBlockFormat() << errinfo_comment("block RLP must be a list"));

		if (rlp.itemCount() != 10)
			BOOST_THROW_EXCEPTION(InvalidBlockFormat() << errinfo_comment("too many or to low fields in the block RLP"));
		m_from = rlp[0].isEmpty() ? dev::Address() : rlp[0].toHash<dev::Address>(RLP::VeryStrict);
		m_previous = (mcp::block_hash)rlp[1];

		dev::RLP const & parents_rlp = rlp[2];
		m_parents.reserve(parents_rlp.itemCount());
		for (dev::RLP const & parent : parents_rlp)
		{
			m_parents.push_back((mcp::block_hash)parent);
		}

		dev::RLP const & links_rlp = rlp[3];
		for (dev::RLP const & link : links_rlp)
		{
			m_links.push_back((h256)link);
		}

		dev::RLP const & approves_rlp = rlp[4];
		for (dev::RLP const & approve : approves_rlp)
		{
			m_approves.push_back((h256)approve);
		}


		m_last_summary = (mcp::summary_hash)rlp[5];
		m_last_summary_block = (mcp::block_hash)rlp[6];
		m_last_stable_block = (mcp::block_hash)rlp[7];
		m_exec_timestamp = (uint64_t)rlp[8];
		m_vrs = (Signature)rlp[9];
		
	}
	catch (Exception& _e)
	{
		_e << errinfo_name("invalid block format: " + toString(rlp) + " RLP: " + toHex(rlp.data()));
		throw;
	}
}

void mcp::block::streamRLP(dev::RLPStream & s, IncludeSignature sig) const
{
	s.appendList((sig ? 1 : 0) + 9);//broadcast 11 fileds,hash 8 fileds
	s << m_from << m_previous;

	s.appendList(m_parents.size());
	for (auto const & parent : m_parents)
		s << parent;

	s.appendList(m_links.size());
	for (auto it(m_links.begin()); it != m_links.end(); it++)
		s << *it;

	s.appendList(m_approves.size());
	for (auto it(m_approves.begin()); it != m_approves.end(); it++)
		s << *it;

	s << m_last_summary << m_last_summary_block << m_last_stable_block << m_exec_timestamp;
	if (sig)
	{
		s << (Signature)m_vrs;
	}
}

mcp::block_hash & mcp::block::hash() const
{
	if (m_hashWith == mcp::block_hash(0))
	{
		dev::RLPStream s;
		streamRLP(s, WithoutSignature);
		m_hashWith = dev::sha3(s.out());
	}
	return m_hashWith;
}



mcp::block_hash mcp::block::root() const
{
	return m_previous == mcp::block_hash(0) ? mcp::block_hash(m_from) : m_previous; //todo maybe error
}

void mcp::block::init_from_genesis_transaction(dev::Address const& from, h256s const& hashes, std::string time)
{
	m_from = from;
	m_previous.clear();
	m_links = hashes;
	m_last_summary.clear();
	m_last_summary_block.clear();
	m_last_stable_block.clear();

	uint64_t exec_timestamp;
	std::stringstream exec_timestamp_ss(time);
	if ((exec_timestamp_ss >> exec_timestamp).fail())
		throw std::runtime_error("deserialize genesis block exec_timestamp error");
	m_exec_timestamp = exec_timestamp;
	m_vrs = SignatureStruct(h256(0),h256(0),0);
}
