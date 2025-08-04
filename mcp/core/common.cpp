#include "common.hpp"
#include "config.hpp"
#include <mcp/common/Exceptions.h>
#include <mcp/common/SecureTrieDB.h>
#include <mcp/common/common.hpp>
#include <mcp/common/working.hpp>
#include <libevm/VMFace.h>
#include <boost/endian/conversion.hpp>
#include <libdevcore/CommonJS.h>
#include <libdevcore/OverlayDB.h>


using namespace dev::eth;

mcp::TransactionException mcp::toTransactionException(Exception const& _e)
{
	// Basic Transaction exceptions
	if (!!dynamic_cast<RLPException const*>(&_e))
		return TransactionException::BadRLP;
	if (!!dynamic_cast<dev::eth::OutOfGasIntrinsic const*>(&_e))
		return TransactionException::OutOfGasIntrinsic;
	if (!!dynamic_cast<dev::eth::InvalidSignature const*>(&_e))
		return TransactionException::InvalidSignature;
	// Executive exceptions
	if (!!dynamic_cast<OutOfGasBase const*>(&_e))
		return TransactionException::OutOfGasBase;
	if (!!dynamic_cast<dev::eth::InvalidNonce const*>(&_e))
		return TransactionException::InvalidNonce;
	if (!!dynamic_cast<dev::eth::NotEnoughCash const*>(&_e))
		return TransactionException::NotEnoughCash;
	if (!!dynamic_cast<dev::eth::BlockGasLimitReached const*>(&_e))
		return TransactionException::BlockGasLimitReached;
	if (!!dynamic_cast<AddressAlreadyUsed const*>(&_e))
		return TransactionException::AddressAlreadyUsed;
	// VM execution exceptions
	if (!!dynamic_cast<BadInstruction const*>(&_e))
		return TransactionException::BadInstruction;
	if (!!dynamic_cast<BadJumpDestination const*>(&_e))
		return TransactionException::BadJumpDestination;
	if (!!dynamic_cast<OutOfGas const*>(&_e))
		return TransactionException::OutOfGas;
	if (!!dynamic_cast<OutOfStack const*>(&_e))
		return TransactionException::OutOfStack;
	if (!!dynamic_cast<StackUnderflow const*>(&_e))
		return TransactionException::StackUnderflow;
	return TransactionException::Unknown;
}

std::string mcp::to_transaction_exception_messge(mcp::TransactionException const & exception_a)
{
	std::string error_msg;
	switch (exception_a)
	{
	case mcp::TransactionException::None:
		error_msg = "None";
		break;
	case mcp::TransactionException::BadRLP:
		error_msg = "Bad RLP";
		break;
	case mcp::TransactionException::InvalidFormat:
		error_msg = "Invalid Format";
		break;
	case mcp::TransactionException::OutOfGasIntrinsic:
		error_msg = "Out Of Gas Intrinsic";
		break;
	case mcp::TransactionException::InvalidSignature:
		error_msg = "Invalid Signature";
		break;
	case mcp::TransactionException::InvalidNonce:
		error_msg = "Invalid Nonce";
		break;
	case mcp::TransactionException::NotEnoughCash:
		error_msg = "Not Enough Cash";
		break;
	case mcp::TransactionException::OutOfGasBase:
		error_msg = "Out Of GasBase";
		break;
	case mcp::TransactionException::BlockGasLimitReached:
		error_msg = "Block Gas Limit Reached";
		break;
	case mcp::TransactionException::BadInstruction:
		error_msg = "invalid code: must not begin with 0xef";
		break;
	case mcp::TransactionException::BadJumpDestination:
		error_msg = "invalid jump destination";
		break;
	case mcp::TransactionException::OutOfGas:
		error_msg = "Gas required exceeds allowance";
		break;
	case mcp::TransactionException::OutOfStack:
		error_msg = "Out Of Stack";
		break;
	case mcp::TransactionException::StackUnderflow:
		error_msg = "Stack Under flow";
		break;
	case mcp::TransactionException::RevertInstruction:
		error_msg = "execution reverted";
		break;
	case mcp::TransactionException::InvalidZeroSignatureFormat:
		error_msg = "Invalid Zero Signature Format";
		break;
	case mcp::TransactionException::AddressAlreadyUsed:
		error_msg = "Address Already Used";
		break;
	case mcp::TransactionException::OutOfGasPriceIntrinsic:
		error_msg = "Intrinsic gas price too low";
		break;
	default:
		error_msg = "Unknown Error";
		break;
	}

	return error_msg;
}

mcp::dag_account_info::dag_account_info(bool & error_a, dev::RLP const & r)
{
	error_a = r.itemCount() != 1;
	latest_stable_block = (mcp::block_hash)r[0];
}

void mcp::dag_account_info::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(1);
	s << latest_stable_block;
}

mcp::block_state::block_state() :
    status(mcp::block_status::unknown),
    is_free(false),
    is_stable(false),
	stable_index(0),
    is_on_main_chain(false),
    main_chain_index(boost::none),
	earliest_included_mc_index(boost::none),
    latest_included_mc_index(boost::none),
    level(0),
    witnessed_level(0),
    best_parent(0),
    mc_timestamp(0),
	stable_timestamp(0),
	bp_included_mc_index(boost::none),
	earliest_bp_included_mc_index(boost::none),
	latest_bp_included_mc_index(boost::none)
{
}

mcp::block_state::block_state(bool & error_a, dev::RLP const & r)
{
	error_a = r.itemCount() == 0;
	if (error_a)
		return;

	error_a = (r.itemCount() != 16 && r.itemCount() != 20);
	if (error_a)
		return;

	status = (mcp::block_status)(r[0].toInt<uint8_t>());
	is_free = r[1].toInt();
	is_stable = r[2].toInt();
	{
		dev::RLP const & main_rlp = r[3];
		if (main_rlp.itemCount() == 1)
		{
			main_chain_index = main_rlp[0].toInt();
		}
		else
		{
			main_chain_index = boost::none;
		}
	}

	level = (uint64_t)r[4];
	mc_timestamp = (uint64_t)r[5];
	stable_timestamp = (uint64_t)r[6];

	is_on_main_chain = r[7].toInt();
	witnessed_level = (uint64_t)r[8];
	best_parent = (mcp::block_hash)r[9];

	{
		dev::RLP const & rlp = r[10];
		if (rlp.itemCount() == 1)
		{
			earliest_included_mc_index = rlp[0].toInt();
		}
		else
		{
			earliest_included_mc_index = boost::none;
		}
	}

	{
		dev::RLP const & rlp = r[11];
		if (rlp.itemCount() == 1)
		{
			latest_included_mc_index = rlp[0].toInt();
		}
		else
		{
			latest_included_mc_index = boost::none;
		}
	}

	{
		dev::RLP const & rlp = r[12];
		if (rlp.itemCount() == 1)
		{
			bp_included_mc_index = rlp[0].toInt();
		}
		else
		{
			bp_included_mc_index = boost::none;
		}
	}

	{
		dev::RLP const & rlp = r[13];
		if (rlp.itemCount() == 1)
		{
			earliest_bp_included_mc_index = rlp[0].toInt();
		}
		else
		{
			earliest_bp_included_mc_index = boost::none;
		}
	}

	{
		dev::RLP const & rlp = r[14];
		if (rlp.itemCount() == 1)
		{
			latest_bp_included_mc_index = rlp[0].toInt();
		}
		else
		{
			latest_bp_included_mc_index = boost::none;
		}
	}

	stable_index = (uint64_t)r[15];

	if (r.itemCount() == 16)
		return;
	m_stateRoot = (h256)r[16];
	m_transactionsRoot = (h256)r[17];
	m_receiptsRoot = (h256)r[18];
	m_logBloom = (log_bloom)r[19];
}

void mcp::block_state::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(20);

	s << (uint8_t)status << is_free << is_stable;

	if (main_chain_index)
	{
		s.appendList(1);
		s << *main_chain_index;
	}
	else
	{
		s.appendList(0);
	}

	s << level << mc_timestamp << stable_timestamp
		<< is_on_main_chain << witnessed_level << best_parent;

	if (earliest_included_mc_index)
	{
		s.appendList(1);
		s << *earliest_included_mc_index;
	}
	else
	{
		s.appendList(0);
	}

	if (latest_included_mc_index)
	{
		s.appendList(1);
		s << *latest_included_mc_index;
	}
	else
	{
		s.appendList(0);
	}

	if (bp_included_mc_index)
	{
		s.appendList(1);
		s << *bp_included_mc_index;
	}
	else
	{
		s.appendList(0);
	}

	if (earliest_bp_included_mc_index)
	{
		s.appendList(1);
		s << *earliest_bp_included_mc_index;
	}
	else
	{
		s.appendList(0);
	}

	if (latest_bp_included_mc_index)
	{
		s.appendList(1);
		s << *latest_bp_included_mc_index;
	}
	else
	{
		s.appendList(0);
	}

	s << stable_index << m_stateRoot << m_transactionsRoot << m_receiptsRoot << m_logBloom;
}

mcp::free_key::free_key(uint64_t const & witnessed_level_a, uint64_t const & level_a, mcp::block_hash const & hash_a) :
    witnessed_level_desc(witnessed_level_a),
    level_desc(level_a),
    hash_asc(hash_a)
{
}

mcp::free_key::free_key(dev::Slice const & val_a)
{
    mcp::bufferstream stream(reinterpret_cast<uint8_t const *> (val_a.data()), val_a.size());
    deserialize(stream);
}

bool mcp::free_key::operator==(mcp::free_key const & other) const
{
    return witnessed_level_desc == other.witnessed_level_desc
        && level_desc == other.level_desc
        && hash_asc == other.hash_asc;
}

void mcp::free_key::serialize(mcp::stream & stream_a) const
{
    uint64_t be_witnessed_level_desc(boost::endian::native_to_big(std::numeric_limits<uint64_t>::max() - witnessed_level_desc));
    write(stream_a, be_witnessed_level_desc);

    uint64_t be_level_desc(boost::endian::native_to_big(std::numeric_limits<uint64_t>::max() - level_desc));
    write(stream_a, be_level_desc);

    write(stream_a, hash_asc.asArray());
}

void mcp::free_key::deserialize(mcp::stream & stream_a)
{
    uint64_t be_witnessed_level_desc(0);
    read(stream_a, be_witnessed_level_desc);
    witnessed_level_desc = std::numeric_limits<uint64_t>::max() - boost::endian::big_to_native(be_witnessed_level_desc);

    uint64_t be_level_desc(0);
    read(stream_a, be_level_desc);
    level_desc = std::numeric_limits<uint64_t>::max() - boost::endian::big_to_native(be_level_desc);

    read(stream_a, hash_asc.asArray());
}


mcp::hash_tree_info::hash_tree_info()
{
}

mcp::hash_tree_info::hash_tree_info(mcp::block_hash const & b_hash_a, mcp::summary_hash const & s_hash_a) :
    b_hash(b_hash_a), s_hash(s_hash_a)
{
}

mcp::hash_tree_info::hash_tree_info(dev::Slice const & val_a)
{
    assert_x(val_a.size() == sizeof(*this));
    std::copy(reinterpret_cast<uint8_t const *> (val_a.data()), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(*this), reinterpret_cast<uint8_t *> (this));
}

dev::Slice mcp::hash_tree_info::val() const
{
    return dev::Slice((char *)this, sizeof(*this));
}

mcp::block_child_key::block_child_key(mcp::block_hash const & hash_a, mcp::block_hash const & child_hash_a) :
    hash(hash_a), child_hash(child_hash_a)
{
}

mcp::block_child_key::block_child_key(dev::Slice const & val_a)
{
    assert_x(val_a.size() == sizeof(*this));
    std::copy(reinterpret_cast<uint8_t const *> (val_a.data()), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(*this), reinterpret_cast<uint8_t *> (this));
}

bool mcp::block_child_key::operator==(mcp::block_child_key const & other) const
{
    return hash == other.hash && child_hash == other.child_hash;
}

dev::Slice mcp::block_child_key::val() const
{
    return dev::Slice((char *)this, sizeof(*this));
}

mcp::account_state::account_state(bool & error_a, dev::RLP const & r, Changedness _c) :
	m_isUnchanged(_c == Unchanged)
{
	error_a = r.itemCount() != 8;
	m_account = (Address)r[0];
	m_ts = (h256)r[1];
	m_previous = (h256)r[2];
	m_nonce = (u256)r[3];
	m_balance = (u256)r[4];
	m_storageRoot = (h256)r[5];
	m_codeHash = (h256)r[6];
	m_isAlive = r[7].toInt();

	record_init_hash();
}

void mcp::account_state::record_init_hash()
{
	init_hash = hash();
}

void mcp::account_state::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(8);
	s << m_account  << m_ts  << m_previous << m_nonce << m_balance << m_storageRoot << m_codeHash << m_isAlive;
}

h256 mcp::account_state::hash()
{
	RLPStream s;
	stream_RLP(s);

	return dev::sha3(s.out());
}

void mcp::account_state::setCode(dev::bytes&& _code)
{
	auto const newHash = sha3(_code);
	if (newHash != m_codeHash)
	{
		m_codeCache = std::move(_code);
		m_hasNewCode = true;
		m_codeHash = newHash;
	}
}

void mcp::account_state::resetCode()
{
	m_codeCache.clear();
	m_hasNewCode = false;
	m_codeHash = EmptySHA3;
}

u256 mcp::account_state::originalStorageValue(u256 const& _key, dev::OverlayDB const& _db) const
{
    auto it = m_storageOriginal.find(_key);
    if (it != m_storageOriginal.end())
        return it->second;

    // Not in the original values cache - go to the DB.
    SecureTrieDB<h256, dev::OverlayDB> const memdb(const_cast<dev::OverlayDB*>(&_db), m_storageRoot);
    std::string const payload = memdb.at(_key);
    auto const value = payload.size() ? RLP(payload).toInt<u256>() : 0;
    m_storageOriginal[_key] = value;
    return value;
}


mcp::skiplist_info::skiplist_info()
{
}

mcp::skiplist_info::skiplist_info(std::set<mcp::block_hash> const & list_a) :
    list(list_a)
{
}

mcp::skiplist_info::skiplist_info(dev::RLP const & r)
{
    assert_x(r.isList());
    for (auto sk : r)
        list.insert((mcp::block_hash) sk);
}

void mcp::skiplist_info::stream_RLP(dev::RLPStream & s) const
{
    s.appendList(list.size());
    for (mcp::block_hash sk : list)
        s << sk;
}

mcp::summary_hash mcp::summary::gen_summary_hash(mcp::block_hash const & block_hash, mcp::summary_hash const & previous_hash,
	std::list<mcp::summary_hash> const & parent_hashs, h256 const & receipts_root, std::set<mcp::summary_hash> const & skiplist,
	mcp::block_status const & status_a, uint64_t const& stable_index_a, uint64_t const& mc_timestamp_a)
{
	dev::RLPStream s;
	s.appendList(8);
	s << block_hash << previous_hash;

	s.appendList(parent_hashs.size());
	for (auto & parent : parent_hashs)
		s << parent;

	s << receipts_root;
	s.appendList(skiplist.size());
	for (auto & sk : skiplist)
		s << sk;
	s << (uint8_t)status_a << stable_index_a << mc_timestamp_a;

	return dev::sha3(s.out());
}

mcp::advance_info::advance_info():
	mci(0),
	witness_block(0)
{
}

mcp::advance_info::advance_info(uint64_t const & mci_a, mcp::block_hash const & witness_block_a) :
	mci(mci_a),
	witness_block(witness_block_a)
{
}

mcp::advance_info::advance_info(dev::Slice const & val_a)
{
	assert_x(val_a.size() == sizeof(*this));
	std::copy(reinterpret_cast<uint8_t const *> (val_a.data()), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(*this), reinterpret_cast<uint8_t *> (this));
}

dev::Slice mcp::advance_info::val() const
{
	return dev::Slice((char*)this, sizeof(*this));
}

dev::Slice mcp::h64_to_slice(h64 const & value)
{
	return dev::Slice(reinterpret_cast<char const*>(value.data()), value.size);
}
dev::h64 mcp::slice_to_h64(dev::Slice const & slice)
{
	return dev::h64(slice.toBytes());
}

dev::Slice mcp::h256_to_slice(h256 const & value)
{
	return dev::Slice(reinterpret_cast<char const*>(value.data()), value.size);
}
dev::h256 mcp::slice_to_h256(dev::Slice const & slice)
{
	return dev::h256(slice.toBytes());
}

dev::Slice mcp::h512_to_slice(h512 const & value)
{
	return dev::Slice(reinterpret_cast<char const*>(value.data()), value.size);
}

h512 mcp::slice_to_h512(dev::Slice const & slice)
{
	return dev::h512(slice.toBytes());
}

dev::Slice mcp::account_to_slice(dev::Address const & value)
{
	return dev::Slice((char*)value.data(), value.size);
}

dev::Address mcp::slice_to_account(dev::Slice const & slice)
{
	return Address(slice.toBytes());
}

bool mcp::isAddress(std::string const& _s)
{
	if (_s.length() != 42 || _s.substr(0, 2) != "0x")
		return false;
	return dev::isHex(_s);
}

bool mcp::isH256(std::string const& _s)
{
	if (_s.length() != (h256::size * 2 + 2) || _s.substr(0, 2) != "0x")
		return false;
	return dev::isHex(_s);
}

//mcp::call_trace_action::call_trace_action(bool & error_a, dev::RLP const & r)
//{
//	error_a = r.itemCount() != 6;
//	if (error_a)
//		return;
//
//	call_type = (std::string)r[0];
//	from = (Address)r[1];
//	gas = (u256)r[2];
//	data = (dev::bytes)r[3];
//	to = (Address)r[4];
//	amount = (u256)r[5];
//}
//
//void mcp::call_trace_action::stream_RLP(dev::RLPStream & s) const
//{
//	s.appendList(6);
//	s << call_type << from << gas << data << to << amount;
//}
//
//void mcp::call_trace_action::serialize_json(mcp::json & json_a) const
//{
//	json_a["call_type"] = call_type;
//	json_a["from"] = dev::toJS(from);
//	json_a["gas"] = gas.str();
//	json_a["data"] = dev::toJS(data);
//	json_a["to"] = dev::toJS(to);
//	json_a["amount"] = amount.str();
//}
//
//mcp::call_trace_result::call_trace_result(bool & error_a, dev::RLP const & r)
//{
//	error_a = r.itemCount() != 2;
//	if (error_a)
//		return;
//
//	gas_used = (mcp::uint256_t)r[0];
//	output = (dev::bytes)r[1];
//}
//
//void mcp::call_trace_result::stream_RLP(dev::RLPStream & s) const
//{
//	s.appendList(2);
//	s << gas_used << output;
//}
//
//void mcp::call_trace_result::serialize_json(mcp::json & json_a) const
//{
//	json_a["gas_used"] = gas_used.str();
//	json_a["output"] = dev::toJS(output);
//}
//
//mcp::create_trace_action::create_trace_action(bool & error_a, dev::RLP const & r)
//{
//	error_a = r.itemCount() != 4;
//	if (error_a)
//		return;
//
//	from = (Address)r[0];
//	gas = (u256)r[1];
//	init = (dev::bytes)r[2];
//	amount = (u256)r[3];
//}
//
//void mcp::create_trace_action::stream_RLP(dev::RLPStream & s) const
//{
//	s.appendList(4);
//	s << from << gas << init << amount;
//}
//
//void mcp::create_trace_action::serialize_json(mcp::json & json_a) const
//{
//	json_a["from"] = dev::toJS(from);
//	json_a["gas"] = gas.str();
//	json_a["init"] = dev::toJS(init);
//	json_a["amount"] = amount.str();
//}
//
//mcp::create_trace_result::create_trace_result(bool & error_a, dev::RLP const & r)
//{
//	error_a = r.itemCount() != 3;
//	if (error_a)
//		return;
//
//	gas_used = (u256)r[0];
//	contract_account = (Address)r[1];
//	code = (dev::bytes)r[2];
//}
//
//void mcp::create_trace_result::stream_RLP(dev::RLPStream & s) const
//{
//	s.appendList(3);
//	s << gas_used << contract_account << code;
//}
//
//void mcp::create_trace_result::serialize_json(mcp::json & json_a) const
//{
//	json_a["gas_used"] = gas_used.str();
//	json_a["contract_account"] = dev::toJS(contract_account);
//	json_a["code"] = dev::toJS(code);
//}
//
//mcp::suicide_trace_action::suicide_trace_action(bool & error_a, dev::RLP const & r)
//{
//	error_a = r.itemCount() != 3;
//	if (error_a)
//		return;
//
//	contract_account = (Address)r[0];
//	refund_account = (Address)r[1];
//	balance = (u256)r[2];
//}
//
//void mcp::suicide_trace_action::stream_RLP(dev::RLPStream & s) const
//{
//	s.appendList(3);
//	s << contract_account << refund_account << balance;
//}
//
//void mcp::suicide_trace_action::serialize_json(mcp::json & json_a) const
//{
//	json_a["contract_account"] = dev::toJS(contract_account);
//	json_a["refund_account"] = dev::toJS(refund_account);
//	json_a["balance"] = balance.str();
//}
//
//mcp::trace::trace(bool & error_a, dev::RLP const & r)
//{
//	error_a = r.itemCount() != 5;
//	if (error_a)
//		return;
//
//	type = (mcp::trace_type)r[0].toInt<uint8_t>();
//	switch (type)
//	{
//	case mcp::trace_type::call:
//		action = std::make_shared<mcp::call_trace_action>(error_a, r[1]);
//		break;
//	case  mcp::trace_type::create:
//		action = std::make_shared<mcp::create_trace_action>(error_a, r[1]);
//		break;
//	case  mcp::trace_type::suicide:
//		action = std::make_shared<mcp::suicide_trace_action>(error_a, r[1]);
//		break;
//	default:
//		assert_x_msg(false, "Invalid trace type");
//	}
//	if (error_a)
//		return;
//
//	error_message = (std::string)r[2];
//	
//	dev::RLP const & result_rlp = r[3];
//	if (result_rlp.itemCount() > 0)
//	{
//		assert_x(error_message.empty());
//		error_a = result_rlp.itemCount() != 1;
//		if (error_a)
//			return;
//
//		switch (type)
//		{
//		case mcp::trace_type::call:
//			result = std::make_shared<mcp::call_trace_result>(error_a, result_rlp[0]);
//			break;
//		case  mcp::trace_type::create:
//			result = std::make_shared<mcp::create_trace_result>(error_a, result_rlp[0]);
//			break;
//		case  mcp::trace_type::suicide:
//			result = nullptr;
//			break;
//		default:
//			assert_x_msg(false, "Invalid trace type");
//		}
//	}
//	else
//		result = nullptr;
//
//	depth = r[4].toInt<uint32_t>();
//
//}
//
//void mcp::trace::stream_RLP(dev::RLPStream & s) const
//{
//	s.appendList(5);
//
//	s << (uint8_t)type;
//	action->stream_RLP(s);
//	s << error_message;
//	if (result)
//	{
//		s.appendList(1);
//		result->stream_RLP(s);
//	}
//	else
//		s.appendList(0);
//	s << depth;
//}
//
//void mcp::trace::serialize_json(mcp::json & json_a) const
//{
//	json_a["type"] = (uint8_t)type;
//
//	mcp::json action_l = mcp::json::object();
//	action->serialize_json(action_l);
//	json_a["action"] = action_l;
//
//	if (!error_message.empty())
//		json_a["error"] = error_message;
//	else
//	{
//		if (result)
//		{
//			mcp::json result_l = mcp::json::object();
//			result->serialize_json(result_l);
//			json_a["result"] = result_l;
//		}
//		else
//			json_a["result"] = nullptr;
//	}
//}

boost::filesystem::path mcp::working_path()
{
	auto result(mcp::app_path());
	switch (mcp::mcp_network)
	{
	case mcp::mcp_networks::mcp_mini_test_network:
		result /= "mcpMiniTest";
		break;
	case mcp::mcp_networks::mcp_test_network:
		result /= "mcpTest";
		break;
	case mcp::mcp_networks::mcp_beta_network:
		result /= "mcpBeta";
		break;
	case mcp::mcp_networks::mcp_live_network:
		result /= "mcp";
		break;
	}
	return result;
}

boost::filesystem::path mcp::unique_path()
{
	auto result(working_path() / boost::filesystem::unique_path());
	return result;
}

mcp::epoch_approves_key::epoch_approves_key(dev::Slice const & val_a)
{
    assert_x(val_a.size() == sizeof(*this));
    std::copy(reinterpret_cast<uint8_t const *> (val_a.data()), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(*this), reinterpret_cast<uint8_t *> (this));
}

