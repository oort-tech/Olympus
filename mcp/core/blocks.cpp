#include "blocks.hpp"
#include <mcp/common/utility.hpp>
#include <boost/endian/conversion.hpp>
#include <mcp/common/common.hpp>

mcp::block_hashables::block_hashables(mcp::block_type type_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a,
	mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
	mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a, uint256_t const & gas_a, uint256_t const& gas_price_a,
	mcp::data_hash const & data_hash_a, uint64_t const & exec_timestamp_a, mcp::uint64_union const & work_a) :
	type(type_a),
	from(from_a),
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
	data_hash(data_hash_a),
	exec_timestamp(exec_timestamp_a),
	work(work_a)
{
}

mcp::block_hashables::block_hashables(bool & error_a, dev::RLP const & r)
{
	error_a = r.itemCount() == 0;
	if (error_a)
		return;

	type = (mcp::block_type)r[0].toInt<uint8_t>();
	switch (type)
	{
	case mcp::block_type::genesis:
	{
		error_a = r.itemCount() != 9;
		if (error_a)
			return;

		from = (mcp::account)r[1];
		to = (mcp::account)r[2];
		amount = (mcp::amount)r[3];
		previous = (mcp::block_hash)r[4];

		gas = (uint256_t)r[5];
		gas_price = (uint256_t)r[6];
		data_hash = (mcp::data_hash)r[7];
		exec_timestamp = (uint64_t)r[8];

		last_summary = 0;
		last_summary_block = 0;
		last_stable_block = 0;
		break;
	}
	case mcp::block_type::dag:
	{
		error_a = r.itemCount() != 9;
		if (error_a)
			return;

		from = (mcp::account)r[1];
		previous = (mcp::block_hash)r[2];

		dev::RLP const & parents_rlp = r[3];
		parents.reserve(parents_rlp.itemCount());
		for (dev::RLP const & parent : parents_rlp)
		{
			parents.push_back((mcp::block_hash)parent);
		}

		dev::RLP const & links_rlp = r[4];
		for (dev::RLP const & link : links_rlp)
		{
			links->push_back((mcp::block_hash)link);
		}

		last_summary = (mcp::summary_hash)r[5];
		last_summary_block = (mcp::block_hash)r[6];
		last_stable_block = (mcp::block_hash)r[7];
		exec_timestamp = (uint64_t)r[8];

		to = 0;
		amount = 0;
		gas = 0;
		data_hash = 0;
		//data: empty

		break;
	}
	case mcp::block_type::light:
	{
		error_a = r.itemCount() != 8 && r.itemCount() != 9;
		if (error_a)
			return;

		from = (mcp::account)r[1];
		to = (mcp::account)r[2];
		amount = (mcp::amount)r[3];
		previous = (mcp::block_hash)r[4];
		gas = (uint256_t)r[5];
		gas_price = (uint256_t)r[6];
		data_hash = (mcp::data_hash)r[7];
		if (r.itemCount() == 9)
			light_version = (uint8_t)r[8];
		else
			light_version = 0;

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
		type = mcp::block_type::genesis;

		std::string from_l = json_a["from"];
		error_a = from.decode_account(from_l);
		if (error_a)
			return;

		std::string to_l = json_a["to"];
		error_a = to.decode_account(to_l);
		if (error_a)
			return;

		std::string amount_l = json_a["amount"];
		error_a = !boost::conversion::try_lexical_convert(amount_l, amount);
		if (error_a)
			return;

		dev::bytes data;
		std::string data_l = json_a["data"];
		error_a = mcp::hex_to_bytes(data_l, data);
		if (error_a)
			return;

		data_hash = mcp::block::data_hash(data);

		std::string exec_timestamp_l = json_a["exec_timestamp"];
		std::stringstream exec_timestamp_ss(exec_timestamp_l);
		error_a = (exec_timestamp_ss >> exec_timestamp).fail();
		if (error_a)
			return;

		last_summary = 0;
		last_summary_block = 0;
		last_stable_block = 0;
	}
	catch (std::runtime_error const &)
	{
		error_a = true;
	}
}

void mcp::block_hashables::stream_RLP(dev::RLPStream & s) const
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

		s.appendList(9);
		s << (uint8_t)type << from << to << amount << previous
			<< gas << gas_price << data_hash << exec_timestamp;

		break;
	}
	case mcp::block_type::dag:
	{
		assert_x(to.is_zero());
		assert_x(amount.is_zero());
		assert_x(gas.is_zero());
		assert_x(data_hash.is_zero());
		s.appendList(9);
		s << (uint8_t)type << from << previous;

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

		if (light_version > 0)
			s.appendList(9);
		else
			s.appendList(8);
		s << (uint8_t)type << from << to << amount << previous
			<< gas << gas_price << data_hash;
		if (light_version > 0)
			s << light_version;
		break;
	}
	default:
		assert_x_msg(false, "Invalid block type");
		break;
	}
}

void mcp::block_hashables::serialize_json(mcp::json & json_a) const
{
	json_a["type"] = (uint8_t)type;
	json_a["from"] = from.to_account();

	mcp::json content_l = mcp::json::object();
	switch (type)
	{
	case mcp::block_type::genesis:
	{
		content_l["to"] = to.to_account();
		content_l["amount"] = amount.str();
		content_l["data_hash"] = data_hash.to_string();
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
		content_l["data_hash"] = data_hash.to_string();
		content_l["version"] = light_version;
		break;
	}
	default:
		assert_x_msg(false, "Invalid block type");
		break;
	}

	json_a["content"] = content_l;
}

void mcp::block_hashables::hash(blake2b_state & hash_a) const
{
	blake2b_update(&hash_a, &type, sizeof(type));
	blake2b_update(&hash_a, from.bytes.data(), sizeof(from.bytes));

	switch (type)
	{
	case mcp::block_type::genesis:
	{
		blake2b_update(&hash_a, to.bytes.data(), sizeof(to.bytes));
		mcp::uint256_union amount_u(amount);
		blake2b_update(&hash_a, amount_u.bytes.data(), sizeof(amount_u.bytes));
		blake2b_update(&hash_a, data_hash.bytes.data(), sizeof(data_hash.bytes));

		mcp::witness_param w_param(mcp::param::witness_param(0));
		assert_x(w_param.witness_list.size() > 0);

		// commented by michael at 1/19
		// for (mcp::account witness : w_param.witness_list)
		// 	blake2b_update(&hash_a, witness.bytes.data(), sizeof(witness.bytes));

		auto big_exec_timestamp = boost::endian::native_to_big(exec_timestamp);
		blake2b_update(&hash_a, &big_exec_timestamp, sizeof(big_exec_timestamp));
		break;
	}
	case mcp::block_type::dag:
	{
		blake2b_update(&hash_a, previous.bytes.data(), sizeof(previous.bytes));
		for (auto p : parents)
			blake2b_update(&hash_a, p.bytes.data(), sizeof(p.bytes));

		for (auto it(links->begin()); it != links->end(); it++)
			blake2b_update(&hash_a, (*it).bytes.data(), sizeof((*it).bytes));

		blake2b_update(&hash_a, last_summary_block.bytes.data(), sizeof(last_summary_block.bytes));
		blake2b_update(&hash_a, last_summary.bytes.data(), sizeof(last_summary.bytes));
		blake2b_update(&hash_a, last_stable_block.bytes.data(), sizeof(last_stable_block.bytes));
		auto big_exec_timestamp = boost::endian::native_to_big(exec_timestamp);
		blake2b_update(&hash_a, &big_exec_timestamp, sizeof(big_exec_timestamp));
		break;
	}
	case mcp::block_type::light:
	{
		blake2b_update(&hash_a, to.bytes.data(), sizeof(to.bytes));
		mcp::uint256_union amount_u(amount);
		blake2b_update(&hash_a, amount_u.bytes.data(), sizeof(amount_u.bytes));
		blake2b_update(&hash_a, previous.bytes.data(), sizeof(previous.bytes));
		mcp::uint256_union gas_union(gas);
		blake2b_update(&hash_a, gas_union.bytes.data(), sizeof(gas_union.bytes));
		if (light_version > 0)
		{
			mcp::uint256_union gas_price_union(gas_price);
			blake2b_update(&hash_a, gas_price_union.bytes.data(), sizeof(gas_price_union.bytes));
		}
		blake2b_update(&hash_a, data_hash.bytes.data(), sizeof(data_hash.bytes));

		break;
	}
	}
}

mcp::uint64_union mcp::block_hashables::block_work() const
{
	return work;
}

void mcp::block_hashables::block_work_set(mcp::uint64_union const & work_a)
{
	work = work_a;
}

mcp::block::block()
	: hashables(std::make_unique<mcp::block_hashables>())
{
}

mcp::block::block(mcp::block_type type_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a,
	mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
	mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a,
	uint256_t const & gas, uint256_t const & gas_price_a, mcp::data_hash const & data_hash_a, std::vector<uint8_t> const & data_a,
	uint64_t const & exec_timestamp_a, mcp::uint64_union const & work_a) :
	signature(0),
	data(data_a)
{
	hashables = std::make_unique<mcp::block_hashables>(type_a, from_a, to_a, amount_a, previous_a, parents_a, links_a,
		last_summary_a, last_summary_block_a, last_stable_block_a, gas, gas_price_a, data_hash_a, exec_timestamp_a, work_a);
}

void mcp::block::init_from_genesis_json(bool & error_a, mcp::json const & json_a)
{
	if (!error_a)
	{
		hashables->init_from_genesis_json(error_a, json_a);
		if (error_a)
			return;

		std::string data_l = json_a["data"];
		error_a = mcp::hex_to_bytes(data_l, data);
		if (error_a)
			return;

		signature = mcp::signature(0);
	}
}

mcp::data_hash mcp::block::data_hash(std::vector<uint8_t> const & data_a)
{
	if (data_a.size() == 0)
		return 0;
	return mcp::blake2b_hash(data_a);
}

mcp::block::block(bool & error_a, dev::RLP const & r, bool with_data)
{
	error_a = (r.itemCount() != 2 && r.itemCount() != 3);
	if (error_a)
		return;

	auto const & r_hashables(r[0]);
	hashables = std::make_unique<mcp::block_hashables>(error_a, r_hashables);

	if (error_a)
		return;

	signature = (mcp::signature)r[1];

	if (hashables->type != mcp::block_type::dag && with_data)
		data = r[2].toBytes();
}

void mcp::block::stream_RLP(dev::RLPStream & s) const
{
	switch (hashables->type)
	{
	case mcp::block_type::genesis:
	case mcp::block_type::light:
	{
		s.appendList(3);
		hashables->stream_RLP(s);
		s << signature << data;
		break;
	}
	case mcp::block_type::dag:
	{
		s.appendList(2);
		hashables->stream_RLP(s);
		s << signature;
		break;
	}
	default:
		assert_x_msg(false, "Invalid block type");
		break;
	}
}

void mcp::block::stream_RLP_nodata(dev::RLPStream & s) const
{
	s.appendList(2);
	hashables->stream_RLP(s);
	s << signature;
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

	hashables->serialize_json(json_a);

	std::string signature_l = signature.to_string();
	json_a["signature"] = signature_l;

	switch (hashables->type)
	{
	case mcp::block_type::genesis:
	case mcp::block_type::light:
	{
		std::string data_str(mcp::bytes_to_hex(data));
		json_a["content"]["data"] = data_str;
		break;
	}
	case mcp::block_type::dag:
		break;
	default:
		assert_x_msg(false, "Invalid block type");
		break;
	}
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
		mcp::uint256_union & result(m_hash);
		blake2b_state hash_l;
		auto status(blake2b_init(&hash_l, sizeof(result.bytes)));
		assert_x(status == 0);

		hashables->hash(hash_l);

		status = blake2b_final(&hash_l, result.bytes.data(), sizeof(result.bytes));
		assert_x(status == 0);
	}
	return m_hash;
}

bool mcp::block::operator== (mcp::block const & other_a) const
{
	return hash() == other_a.hash() && signature == other_a.signature;
}

mcp::block_hash mcp::block::root() const
{
	return !previous().is_zero() ? previous() : (const mcp::block_hash) hashables->from;
}

void mcp::block::set_signature(mcp::signature signature_a)
{
	signature = signature_a;
}

void mcp::block::set_signature(mcp::raw_key const & prv_a, mcp::public_key const & pub_a)
{
	signature = mcp::sign_message(prv_a, pub_a, hash());
}

mcp::uint64_union mcp::block::block_work() const
{
	return hashables->work;
}

void mcp::block::block_work_set(mcp::uint64_union const & work_a)
{
	hashables->block_work_set(work_a);
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