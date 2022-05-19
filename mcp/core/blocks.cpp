#include "blocks.hpp"
#include "common.hpp"
#include "config.hpp"
#include <libdevcore/CommonIO.h>
//#include <mcp/common/utility.hpp>
//#include <boost/endian/conversion.hpp>
#include <mcp/common/common.hpp>
//#include <mcp/common/log.hpp>

mcp::block::block(mcp::account from, mcp::block_hash const & previous, std::vector<mcp::block_hash> const & parents, h256s links,
	mcp::block_hash const & last_summary, mcp::block_hash const & last_summary_block, mcp::block_hash const & last_stable_block,
	uint64_t const & exec_timestamp, dev::Secret const& s) :
	m_from(from),
	m_previous(previous),
	m_parents(parents),
	m_links(links),
	m_last_summary(last_summary),
	m_last_summary_block(last_summary_block),
	m_last_stable_block(last_stable_block),
	m_exec_timestamp(exec_timestamp)
{
	if (s)
	{
		auto sig = dev::sign(s, hash().number());
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

		if (rlp.itemCount() != 9)
			BOOST_THROW_EXCEPTION(InvalidBlockFormat() << errinfo_comment("too many or to low fields in the block RLP"));
		m_from = rlp[0].isEmpty() ? mcp::account() : rlp[0].toHash<mcp::account>(RLP::VeryStrict);
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

		m_last_summary = (mcp::summary_hash)rlp[4];
		m_last_summary_block = (mcp::block_hash)rlp[5];
		m_last_stable_block = (mcp::block_hash)rlp[6];
		m_exec_timestamp = (uint64_t)rlp[7];
		m_vrs = (Signature)rlp[8];
		
	}
	catch (Exception& _e)
	{
		_e << errinfo_name("invalid block format: " + toString(rlp) + " RLP: " + toHex(rlp.data()));
		throw;
	}
}

void mcp::block::streamRLP(dev::RLPStream & s, IncludeSignature sig) const
{
	s.appendList((sig ? 1 : 0) + 8);//broadcast 11 fileds,hash 8 fileds
	s << m_from << m_previous;

	s.appendList(m_parents.size());
	for (auto const & parent : m_parents)
		s << parent;

	s.appendList(m_links.size());
	for (auto it(m_links.begin()); it != m_links.end(); it++)
		s << *it;

	s << m_last_summary << m_last_summary_block << m_last_stable_block << m_exec_timestamp;
	if (sig)
	{
		s << (Signature)m_vrs;
	}
}

//bool mcp::block::operator== (mcp::block const & other_a) const
//{
//	return hash() == other_a.hash() && m_vrs == other_a.m_vrs;
//}

void mcp::block::serialize_json(std::string & string_a) const
{
	mcp::json json;
	serialize_json(json);
	string_a = json.dump();
}

void mcp::block::serialize_json(mcp::json & json_a) const
{
	json_a["hash"] = hash().to_string();
	json_a["from"] = m_from.to_account();

	//previous
	mcp::json content_l = mcp::json::object();
	content_l["previous"] = m_previous.to_string();
	//parents
	mcp::json j_parents = mcp::json::array();
	for (mcp::block_hash const & p : m_parents)
	{
		j_parents.push_back(p.to_string());
	}
	content_l["parents"] = j_parents;
	//links
	mcp::json j_links = mcp::json::array();
	for (auto it(m_links.begin()); it != m_links.end(); it++)
	{
		h256 const & link(*it);
		j_links.push_back(link.hex());
	}
	content_l["links"] = j_links;
	//last_summary,last_summary_block,last_stable_block,timestamp
	content_l["last_summary"] = m_last_summary.to_string();
	content_l["last_summary_block"] = m_last_summary_block.to_string();
	content_l["last_stable_block"] = m_last_stable_block.to_string();
	content_l["timestamp"] = m_exec_timestamp;
	json_a["content"] = content_l;

	std::string signature_l = ((Signature)m_vrs).hex();
	json_a["signature"] = signature_l;
}

void mcp::block::serialize_json_eth(std::string & string_a) const
{
	mcp::json json;
	serialize_json_eth(json);
	string_a = json.dump();
}

void mcp::block::serialize_json_eth(mcp::json & json_a) const
{
	json_a["hash"] = hash().to_string(true);
	json_a["parentHash"] = m_previous.to_string(true);
	json_a["gasLimit"] = uint256_to_hex_nofill(mcp::block_max_gas);
	//json_a["gasUsed"] = uint256_to_hex_nofill(gas);
	//json_a["minGasPrice"] = uint256_to_hex_nofill(gas_price);
	json_a["timestamp"] = uint64_to_hex_nofill(m_exec_timestamp);
	json_a["transactions"] = mcp::json::array();
	json_a["transactions"].push_back(json_a["hash"]);
}

//std::string mcp::block::to_json()
//{
//	std::string result;
//	serialize_json(result);
//	return result;
//}

mcp::block_hash & mcp::block::hash() const
{
	if (m_hashWith.is_zero())
	{
		dev::RLPStream s;
		streamRLP(s, WithoutSignature);
		auto ret = dev::sha3(s.out()).ref();
		//m_hashWith = ret;
		dev::bytesConstRef(ret).copyTo(m_hashWith.ref());//todo used h256
	}
	return m_hashWith;
}



mcp::block_hash mcp::block::root() const
{
	return isZeroH256(m_previous.is_zero()) ? (const mcp::block_hash)m_from : m_previous; //todo maybe error
}

void mcp::block::init_from_genesis_transaction(mcp::account const& from, h256 const& hash, std::string time)
{
	m_from = from;
	m_previous = 0;
	h256s links;
	links.push_back(hash);
	m_last_summary = 0;
	m_last_summary_block = 0;
	m_last_stable_block = 0;

	uint64_t exec_timestamp;
	std::stringstream exec_timestamp_ss(time);
	if ((exec_timestamp_ss >> exec_timestamp).fail())
		throw std::runtime_error("deserialize genesis block exec_timestamp error");
	m_exec_timestamp = exec_timestamp;
	m_vrs = SignatureStruct(h256(0),h256(0),0);
}

//u256 mcp::block::rawV() const
//{
//	if (!m_vrs)
//		BOOST_THROW_EXCEPTION(TransactionIsUnsigned());
//
//	int const vOffset = (int)mcp::mcp_network * 2 + 35;
//	return m_vrs->v + vOffset;
//}