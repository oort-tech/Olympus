#include "blocks.hpp"
#include <mcp/common/utility.hpp>
#include <boost/endian/conversion.hpp>
#include <mcp/common/common.hpp>
#include <mcp/common/log.hpp>

mcp::block_hashables::block_hashables(mcp::account const & to_a, mcp::amount const & amount_a,
	mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
	mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a, uint256_t const & gas_a, uint256_t const& gas_price_a,
	dev::bytes const & data_a, uint64_t const & exec_timestamp_a, uint256_t chainId_a) :
	to(to_a),
	amount(amount_a),
	previous(previous_a),
	parents(parents_a),
	links(links_a),
	last_summary(last_summary_a),
	last_summary_block(last_summary_block_a),
	last_stable_block(last_stable_block_a),
	gas(gas_a),
	gas_price(gas_price_a),
	data(data_a),
	exec_timestamp(exec_timestamp_a),
	chainID(chainId_a)
{
}

mcp::block_hashables::block_hashables(bool & error_a, mcp::block_type type, dev::RLP const & r)
{
	error_a = r.itemCount() == 0;
	if (error_a)
		return;

	switch (type)
	{
	case mcp::block_type::genesis:
	{
		error_a = r.itemCount() != 10;
		if (error_a)
			return;

		previous = (mcp::block_hash)r[0];
		gas_price = (uint256_t)r[1];
		gas = (uint256_t)r[2];
		to = (mcp::account)r[3];
		amount = (mcp::amount)r[4];
		data = (dev::bytes)r[5];
		chainID = (uint256_t)r[6];
		exec_timestamp = (uint64_t)r[7];

		last_summary = 0;
		last_summary_block = 0;
		last_stable_block = 0;
		break;
	}
	case mcp::block_type::dag:
	{
		error_a = r.itemCount() != 7;
		if (error_a)
			return;

		previous = (mcp::block_hash)r[0];

		dev::RLP const & parents_rlp = r[1];
		parents.reserve(parents_rlp.itemCount());
		for (dev::RLP const & parent : parents_rlp)
		{
			parents.push_back((mcp::block_hash)parent);
		}

		dev::RLP const & links_rlp = r[2];
		for (dev::RLP const & link : links_rlp)
		{
			links->push_back((mcp::block_hash)link);
		}

		last_summary = (mcp::summary_hash)r[3];
		last_summary_block = (mcp::block_hash)r[4];
		last_stable_block = (mcp::block_hash)r[5];
		exec_timestamp = (uint64_t)r[6];

		to = 0;
		amount = 0;
		gas = 0;

		break;
	}
	case mcp::block_type::light:
	{
		error_a = r.itemCount() != 9;
		if (error_a)
			return;

		previous = (mcp::block_hash)((uint256_t)r[0]);
		gas_price = (uint256_t)r[1];
		gas = (uint256_t)r[2];
		if (r[3].isEmpty())
		{
			to = 0;
		}
		else 
		{
			to = (mcp::account)r[3];
		}
		amount = (mcp::amount)r[4];
		data = (dev::bytes)r[5];
		chainID = (uint256_t)r[6];

		exec_timestamp = 0;
		last_summary = 0;
		last_summary_block = 0;
		last_stable_block = 0;
		break;
	}
	default:
		break;
	}
}

void mcp::block_hashables::init_from_genesis_json(bool & error_a, mcp::json const & json_a)
{
	try
	{
		std::string to_l = json_a["to"];
		error_a = to.decode_account(to_l);
		if (error_a)
			return;

		std::string amount_l = json_a["amount"];
		error_a = !boost::conversion::try_lexical_convert(amount_l, amount);
		if (error_a)
			return;

		std::string data_l = json_a["data"];
		error_a = mcp::hex_to_bytes(data_l, data);
		if (error_a)
			return;

		std::string exec_timestamp_l = json_a["exec_timestamp"];
		std::stringstream exec_timestamp_ss(exec_timestamp_l);
		error_a = (exec_timestamp_ss >> exec_timestamp).fail();
		if (error_a)
			return;
		chainID = mcp::mcp_network;

		last_summary = 0;
		last_summary_block = 0;
		last_stable_block = 0;
	}
	catch (std::runtime_error const &)
	{
		error_a = true;
	}
}

void mcp::block_hashables::stream_RLP(mcp::block_type type, dev::RLPStream & s) const
{
	switch (type)
	{
	case mcp::block_type::genesis:
	{
		assert_x(parents.empty());
		assert_x(links->empty());
		assert_x(last_summary.is_zero());
		assert_x(last_summary_block.is_zero());
		assert_x(last_stable_block.is_zero());

		s.appendList(10);
		s << previous.number() << gas_price << gas << to << amount <<
			data << chainID << exec_timestamp << 0 << 0;

		break;
	}
	case mcp::block_type::dag:
	{
		assert_x(to.is_zero());
		assert_x(amount.is_zero());
		assert_x(gas.is_zero());
		assert_x(data.empty());
		s.appendList(7);
		s << previous.number();

		s.appendList(parents.size());
		for (mcp::block_hash const & parent : parents)
			s << parent;

		s.appendList(links->size());
		for (auto it(links->begin()); it != links->end(); it++)
			s << *it;

		s << last_summary << last_summary_block << last_stable_block << exec_timestamp;
		break;
	}
	case mcp::block_type::light:
	{
		assert_x(parents.empty());
		assert_x(links->empty());
		assert_x(last_summary.is_zero());
		assert_x(last_summary_block.is_zero());
		assert_x(last_stable_block.is_zero());

		s.appendList(9);
		s << previous.number() << gas_price << gas;
		if (to.is_zero())
			s << "";
		else
			s << to;
		s << amount <<
			  data << chainID << 0 << 0;
		break;
	}
	default:
		assert_x_msg(false, "Invalid block type");
		break;
	}
}

void mcp::block_hashables::serialize_json(mcp::block_type type, mcp::json & json_a) const
{
	mcp::json content_l = mcp::json::object();
	switch (type)
	{
	case mcp::block_type::genesis:
	{
		content_l["to"] = to.to_account();
		content_l["amount"] = amount.str();
		content_l["data"] = mcp::bytes_to_hex(data);
		content_l["timestamp"] = exec_timestamp;
		break;
	}
	case mcp::block_type::dag:
	{
		content_l["previous"] = previous.to_string();

		mcp::json j_parents = mcp::json::array();
		for (mcp::block_hash const & p : parents)
		{
			j_parents.push_back(p.to_string());
		}
		content_l["parents"] = j_parents;

		mcp::json j_links = mcp::json::array();
		for (auto it(links->begin()); it != links->end(); it++)
		{
			mcp::block_hash const & link(*it);
			j_links.push_back(link.to_string());
		}
		content_l["links"] = j_links;

		content_l["last_summary"] = last_summary.to_string();
		content_l["last_summary_block"] = last_summary_block.to_string();
		content_l["last_stable_block"] = last_stable_block.to_string();

		content_l["timestamp"] = exec_timestamp;
		break;
	}
	case mcp::block_type::light:
	{
		content_l["to"] = to.to_account();
		content_l["amount"] = amount.str();
		content_l["previous"] = previous.to_string();
		content_l["gas"] = (uint64_t)gas;
		content_l["gas_price"] = gas_price.str();
		content_l["data"] = mcp::bytes_to_hex(data);
		break;
	}
	default:
		assert_x_msg(false, "Invalid block type");
		break;
	}

	json_a["content"] = content_l;
}

mcp::block::block()
	: hashables(std::make_unique<mcp::block_hashables>())
{
}

mcp::block::block(mcp::block_type type_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a,
	mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
	mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a,
	uint256_t const & gas, uint256_t const & gas_price_a, dev::bytes const & data_a,
	uint64_t const & exec_timestamp_a, uint256_t chainId_a) :
	m_type(type_a),
	m_from(from_a),
	signature(0)
{
	hashables = std::make_unique<mcp::block_hashables>(to_a, amount_a, previous_a, parents_a, links_a,
		last_summary_a, last_summary_block_a, last_stable_block_a, gas, gas_price_a, data_a, exec_timestamp_a, chainId_a);
}

void mcp::block::init_from_genesis_json(bool & error_a, mcp::json const & json_a)
{
	if (!error_a)
	{
		hashables->init_from_genesis_json(error_a, json_a);
		if (error_a)
			return;

		m_type = mcp::block_type::genesis;

		std::string from_l = json_a["from"];
		error_a = m_from.decode_account(from_l);
		if (error_a)
			return;

		signature = mcp::signature(0);
	}
}

mcp::block::block(bool & error_a, dev::RLP const & r, bool with_data)
{
	error_a = (r.itemCount() != 4);
	if (error_a)
		return;

	m_type = (mcp::block_type)r[0].toInt<uint8_t>();
	m_from = (mcp::account)r[1];
	signature = (mcp::signature)r[2];

	auto const & r_hashables(r[3]);
	hashables = std::make_unique<mcp::block_hashables>(error_a, m_type, r_hashables);

	if (error_a)
		return;
}

void mcp::block::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(4);
	s << (uint8_t)m_type << m_from << signature;
	hashables->stream_RLP(m_type, s);
}

mcp::block_hash const & mcp::block::previous() const
{
	return hashables->previous;
}

std::vector<mcp::block_hash> const & mcp::block::parents() const
{
	return hashables->parents;
}

std::shared_ptr<std::list<mcp::block_hash>> mcp::block::links() const
{
	return hashables->links;
}

void mcp::block::serialize_json(std::string & string_a) const
{
	mcp::json json;
	serialize_json(json);
	string_a = json.dump();
}

void mcp::block::serialize_json(mcp::json & json_a) const
{
	json_a["hash"] = hash().to_string();

	hashables->serialize_json(m_type,json_a);

	json_a["type"] = (uint8_t)m_type;
	json_a["from"] = m_from.to_account();

	std::string signature_l = signature.to_string();
	json_a["signature"] = signature_l;
}

std::string mcp::block::to_json()
{
	std::string result;
	serialize_json(result);
	return result;
}

mcp::block_hash & mcp::block::hash() const
{
	if (m_hash.is_zero())
	{
		dev::RLPStream s;
		hashables->stream_RLP(m_type,s);
		auto ret = dev::sha3(s.out()).ref();

		//mcp::log log_node("node");
		//LOG(log_node.info) << "----------------------, previous:" << hashables->previous.to_string() << " ,gas price:" << hashables->gas_price << " ,gas:" << hashables->gas
		//	<< " ,to:" << hashables->to.to_account() << " ,amount:" << hashables->amount << " ,data:" << mcp::bytes_to_hex(hashables->data) << " ,chainID:" << hashables->chainID ;
		//LOG(log_node.info) << "rlp data:" << mcp::bytes_to_hex(s.out());
		//LOG(log_node.info) << "hash data:" << dev::sha3(s.out()).hex();

		dev::bytesConstRef(ret).copyTo(m_hash.ref());//todo used h256
	}
	return m_hash;
}

bool mcp::block::operator== (mcp::block const & other_a) const
{
	return hash() == other_a.hash() && signature == other_a.signature;
}

mcp::block_hash mcp::block::root() const
{
	return !previous().is_zero() ? previous() : (const mcp::block_hash)m_from;
}

void mcp::block::set_signature(mcp::signature signature_a)
{
	signature = signature_a;
}

void mcp::block::set_signature(mcp::raw_key const & prv_a/*, mcp::public_key const & pub_a*/)
{
	signature = mcp::sign_message(prv_a, hash());
}

int64_t mcp::block::baseGasRequired(bool _contractCreation, dev::bytesConstRef _data, dev::eth::EVMSchedule const& _es)
{
	int64_t g = _contractCreation ? _es.txCreateGas : _es.txGas;

	// Calculate the cost of input data.
	// No risk of overflow by using int64 until txDataNonZeroGas is quite small
	// (the value not in billions).
	for (auto i : _data)
		g += i ? _es.txDataNonZeroGas : _es.txDataZeroGas;
	return g;
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
	hashables->serialize_json_eth(json_a);
	json_a["transactions"].push_back(json_a["hash"]);
}

void mcp::block_hashables::serialize_json_eth(mcp::json & json_a) const
{
	json_a["parentHash"] = previous.to_string(true);
	json_a["gasLimit"] = uint256_to_hex_nofill(mcp::block_max_gas);
	json_a["gasUsed"] = uint256_to_hex_nofill(gas);
	json_a["minGasPrice"] = uint256_to_hex_nofill(gas_price);
	json_a["timestamp"] = uint64_to_hex_nofill(exec_timestamp);
	json_a["transactions"] = mcp::json::array();
}