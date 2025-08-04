#include "block_store.hpp"
#include "genesis.hpp"
#include <mcp/common/utility.hpp>
#include <boost/endian/conversion.hpp>
#include <mcp/common/log.hpp>
using namespace mcp;

mcp::block_store::block_store(bool & error_a, boost::filesystem::path const & path_a)
{
	if (error_a)
		return;

	auto based = mcp::db::defaultBlockBasedTableOptions(2048);
	auto ops = mcp::db::defaultDBOptions(based);
	ops->prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(1));
	ops->memtable_prefix_bloom_size_ratio = 0.02;

	m_db = std::make_shared<mcp::db::database>(path_a, ops);
}

bool mcp::block_store::upgrade()
{
	bool ok(true);
	if (version_get() == 1)
	{
	}

	return ok;
}

std::string mcp::block_store::get_rocksdb_state(uint64_t limit)
{
	std::string str = "";
	if (m_db)
	{
		str = m_db->get_rocksdb_state(limit);
	}
	return str;
}

void mcp::block_store::version_put(mcp::db::db_transaction & transaction_a, int version_a)
{
	dev::h256 version_value(version_a);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(version_key), mcp::h256_to_slice(version_value));
}

int mcp::block_store::version_get()
{
	mcp::db::db_transaction transaction(create_transaction());
	std::string data;
	bool exists(transaction.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(version_key), data));
	int result;
	if (!exists)
	{
		result = 1;
	}
	else
	{
		dev::h256 version_value(mcp::slice_to_h256(dev::Slice(data)));
		// assert_x(version_value.qwords[2] == 0 && version_value.qwords[1] == 0 && version_value.qwords[0] == 0);
		result = ((dev::h256::Arith) version_value).convert_to<int>();
	}
	return result;
}

std::shared_ptr<mcp::block> mcp::block_store::block_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::blocks), mcp::h256_to_slice(hash_a), value));
	std::shared_ptr<mcp::block> result = nullptr;
	if (exists)
	{
		dev::RLP r(value);
		result = std::make_shared<mcp::block>(r);
		assert_x_msg(result != nullptr, "hash:" + hash_a.hex() + " ,data:" + value);
	}
	return result;
}

bool mcp::block_store::block_exists(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a)
{
	std::string result;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::blocks), mcp::h256_to_slice(hash_a), result));
	return exists;
}

void mcp::block_store::block_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a, mcp::block const & block_a)
{
	// all parts of block except data
	dev::bytes b_value;
	{
		dev::RLPStream s;
		block_a.streamRLP(s);
		s.swapOut(b_value);
	}

	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::blocks), mcp::h256_to_slice(hash_a), s_value);
	transaction_a.count_add("block", 1);
}

size_t mcp::block_store::block_count(mcp::db::db_transaction & transaction_a)
{
	return transaction_a.count_get("block");
}


bool mcp::block_store::transaction_exists(mcp::db::db_transaction & transaction_a, h256 const& hash_a)
{
	std::string result;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::transactions), mcp::h256_to_slice(hash_a), result));
	return exists;
}

std::shared_ptr<mcp::Transaction> mcp::block_store::transaction_get(mcp::db::db_transaction & transaction_a, h256 const& hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::transactions), mcp::h256_to_slice(hash_a), value));
	std::shared_ptr<mcp::Transaction> result = nullptr;
	if (exists)
	{
		dev::RLP r(value);
		result = std::make_shared<mcp::Transaction>(r,CheckTransaction::None);

		/// genesis set sender.
		auto isGenesisT = mcp::genesis::isGenesisTransaction(hash_a);
		if (isGenesisT.first)
			result->forceSender(isGenesisT.second);
	}
	return result;
}

void mcp::block_store::transaction_put(mcp::db::db_transaction & transaction_a, h256 const& hash_a, mcp::Transaction const& _t)
{
	// all parts of block except data
	dev::bytes b_value;
	{
		dev::RLPStream s;
		_t.streamRLP(s);
		s.swapOut(b_value);
	}

	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::transactions), mcp::h256_to_slice(hash_a), s_value);
}

bool mcp::block_store::account_nonce_get(mcp::db::db_transaction & transaction_a, Address const & account_a, u256& nonce_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::account_nonce), mcp::account_to_slice(account_a), value));
	if (exists)
		nonce_a = ((dev::h256::Arith)mcp::slice_to_h256(value)).convert_to<u256>();
	return exists;
}

void mcp::block_store::account_nonce_put(mcp::db::db_transaction & transaction_a, Address const & account_a, u256 const& nonce_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::account_nonce), mcp::account_to_slice(account_a), mcp::h256_to_slice(nonce_a));
}

std::shared_ptr<mcp::TransactionAddress> mcp::block_store::transaction_address_get(mcp::db::db_transaction & transaction_a, h256 const& hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::transaction_address), mcp::h256_to_slice(hash_a), value));
	std::shared_ptr<mcp::TransactionAddress> result = nullptr;
	if (exists)
	{
		dev::RLP r(value);
		result = std::make_shared<mcp::TransactionAddress>(r);
	}
	return result;
}

void mcp::block_store::transaction_address_put(mcp::db::db_transaction & transaction_a, h256 const& hash_a, mcp::TransactionAddress const& _td)
{
	dev::bytes b_value = _td.rlp();
	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::transaction_address), mcp::h256_to_slice(hash_a), s_value);
}

bool mcp::block_store::approve_exists(mcp::db::db_transaction & transaction_a, h256 const& hash_a)
{
	std::string result;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::approves), mcp::h256_to_slice(hash_a), result));
	return exists;
}

std::shared_ptr<mcp::approve> mcp::block_store::approve_get(mcp::db::db_transaction & transaction_a, h256 const& hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::approves), mcp::h256_to_slice(hash_a), value));
	std::shared_ptr<mcp::approve> result = nullptr;
	if (exists)
	{
		dev::RLP r(value);
		result = std::make_shared<mcp::approve>(r,CheckTransaction::None);
	}
	return result;
}

void mcp::block_store::approve_put(mcp::db::db_transaction & transaction_a, h256 const& hash_a, mcp::approve const& _t)
{
	// all parts of block except data
	dev::bytes b_value;
	{
		dev::RLPStream s;
		_t.streamRLP(s);
		s.swapOut(b_value);
	}

	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::approves), mcp::h256_to_slice(hash_a), s_value);
}

bool mcp::block_store::dag_account_get(mcp::db::db_transaction & transaction_a, dev::Address const & account_a, mcp::dag_account_info & info_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::dag_account_info), mcp::account_to_slice(account_a), value));
	if (exists)
	{
		dev::RLP r(value);
		bool error(false);
		info_a = mcp::dag_account_info(error, r);
		assert_x(!error);
	}
	return !exists;
}

void mcp::block_store::dag_account_put(mcp::db::db_transaction & transaction_a, dev::Address const & account_a, mcp::dag_account_info const & info_a)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		info_a.stream_RLP(s);
		s.swapOut(b_value);
	}
	dev::Slice s_value((char *)b_value.data(), b_value.size());

	transaction_a.put(static_cast<uint8_t>(StorePrefix::dag_account_info), mcp::account_to_slice(account_a), s_value);
}

bool mcp::block_store::block_summary_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash & summary_hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::block_summary), mcp::h256_to_slice(block_hash_a), value));
	if (exists)
	{
		summary_hash_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::block_summary_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash const & summary_hash_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::block_summary), mcp::h256_to_slice(block_hash_a), mcp::h256_to_slice(summary_hash_a));
}


bool mcp::block_store::summary_block_get(mcp::db::db_transaction & transaction_a, mcp::summary_hash const &  summary_hash_a, mcp::block_hash & block_hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::summary_block), mcp::h256_to_slice(summary_hash_a), value));
	if (exists)
	{
		block_hash_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::summary_block_put(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_hash_a, mcp::block_hash const & block_hash_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::summary_block), mcp::h256_to_slice(summary_hash_a), mcp::h256_to_slice(block_hash_a));
}

std::shared_ptr<mcp::block_state> mcp::block_store::block_state_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::block_state), mcp::h256_to_slice(hash_a), value));
	std::shared_ptr<mcp::block_state> result;
	if (exists)
	{
		dev::RLP r(value);
		bool error(false);
		result = std::make_shared<mcp::block_state>(error, r);
		assert_x(!error);
		assert_x(result != nullptr);
	}
	return result;
}

void mcp::block_store::block_state_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a, mcp::block_state const & state_a)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		state_a.stream_RLP(s);
		s.swapOut(b_value);
	}
	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::block_state), mcp::h256_to_slice(hash_a), s_value);
}

mcp::db::forward_iterator mcp::block_store::dag_free_begin(mcp::db::db_transaction & transaction_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	mcp::db::forward_iterator result(transaction_a.begin(static_cast<uint8_t>(StorePrefix::dag_free), snapshot_a));
	return result;
}

mcp::db::forward_iterator mcp::block_store::dag_free_begin(mcp::db::db_transaction & transaction_a, mcp::free_key const & key_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	dev::bytes b_key;
	{
		mcp::vectorstream stream(b_key);
		key_a.serialize(stream);
	}
	dev::Slice s_key((char*)b_key.data(), b_key.size());
	mcp::db::forward_iterator result(transaction_a.begin(static_cast<uint8_t>(StorePrefix::dag_free), s_key, snapshot_a));
	return result;
}

mcp::db::backward_iterator mcp::block_store::dag_free_rbegin(mcp::db::db_transaction & transaction_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	mcp::db::backward_iterator result(transaction_a.rbegin(static_cast<uint8_t>(StorePrefix::dag_free), snapshot_a));
	return result;
}

void mcp::block_store::dag_free_put(mcp::db::db_transaction & transaction_a, mcp::free_key const & key_a)
{
	dev::bytes b_key;
	{
		mcp::vectorstream stream(b_key);
		key_a.serialize(stream);
	}
	dev::Slice s_key((char*)b_key.data(), b_key.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::dag_free), s_key, dev::Slice());
	//transaction_a.count_add("dag_free", 1);
}

void mcp::block_store::dag_free_del(mcp::db::db_transaction & transaction_a, mcp::free_key const & key_a)
{
	dev::bytes b_key;
	{
		mcp::vectorstream stream(b_key);
		key_a.serialize(stream);
	}
	dev::Slice s_key((char*)b_key.data(), b_key.size());

	{
		std::string result;
		bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::dag_free), s_key, result));
		if (!exists)
		{
			assert(false);
		}
	}

	transaction_a.del(static_cast<uint8_t>(StorePrefix::dag_free), s_key);

	//transaction_a.count_reduce("dag_free", 1);
}

//size_t mcp::block_store::dag_free_count(mcp::db::db_transaction & transaction_a)
//{
//	return transaction_a.count_get("dag_free");
//}


bool mcp::block_store::main_chain_get(mcp::db::db_transaction & transaction_a, uint64_t const & mci_a, mcp::block_hash & hash_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	std::string value;
	dev::h64 mci(mci_a);
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::main_chain), mcp::h64_to_slice(mci), value, snapshot_a));
	if (exists)
	{
		hash_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::main_chain_put(mcp::db::db_transaction & transaction_a, uint64_t const & mci_a, mcp::block_hash const & hash_a)
{
	dev::h64 mci(mci_a);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::main_chain), mcp::h64_to_slice(mci), mcp::h256_to_slice(hash_a));
}

void mcp::block_store::main_chain_del(mcp::db::db_transaction & transaction_a, uint64_t const & mci_a)
{
	dev::h64 mci(mci_a);
	transaction_a.del(static_cast<uint8_t>(StorePrefix::main_chain), mcp::h64_to_slice(mci));
}

size_t mcp::block_store::stable_block_count(mcp::db::db_transaction & transaction_a)
{
	return last_stable_index_get(transaction_a) + 1;
}

bool mcp::block_store::stable_block_get(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::block_hash & hash_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	std::string value;
	dev::h64 index(index_a);
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::stable_block), mcp::h64_to_slice(index), value, snapshot_a));
	if (exists)
	{
		hash_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

bool mcp::block_store::stable_block_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const& hash_a, uint64_t & index_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::stable_block_number), mcp::h256_to_slice(hash_a), value, snapshot_a));
	if (exists)
	{
		index_a = ((dev::h64::Arith)mcp::slice_to_h64(value)).convert_to<uint64_t>();
	}
	return !exists;
}

void mcp::block_store::stable_block_put(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::block_hash const & hash_a)
{
	dev::h64 index(index_a);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::stable_block), mcp::h64_to_slice(index), mcp::h256_to_slice(hash_a));
	transaction_a.put(static_cast<uint8_t>(StorePrefix::stable_block_number), mcp::h256_to_slice(hash_a), mcp::h64_to_slice(index));
}

size_t mcp::block_store::transaction_unstable_count(mcp::db::db_transaction & transaction_a)
{
	return transaction_a.count_get("transaction_unstable");
}

void mcp::block_store::transaction_unstable_count_add(mcp::db::db_transaction & transaction_a, uint32_t v)
{
	transaction_a.count_add("transaction_unstable", v);
}

void mcp::block_store::transaction_unstable_count_reduce(mcp::db::db_transaction & transaction_a, uint32_t v)
{
	transaction_a.count_reduce("transaction_unstable", v);
}

size_t mcp::block_store::transaction_count(mcp::db::db_transaction & transaction_a)
{
	return transaction_a.count_get("transaction");
}

void mcp::block_store::transaction_count_add(mcp::db::db_transaction & transaction_a, uint32_t v)
{
	transaction_a.count_add("transaction", v);
}

size_t mcp::block_store::approve_unstable_count(mcp::db::db_transaction & transaction_a)
{
	return transaction_a.count_get("approve_unstable");
}

void mcp::block_store::approve_unstable_count_add(mcp::db::db_transaction & transaction_a, uint32_t v)
{
	transaction_a.count_add("approve_unstable", v);
}

void mcp::block_store::approve_unstable_count_reduce(mcp::db::db_transaction & transaction_a, uint32_t v)
{
	transaction_a.count_reduce("approve_unstable", v);
}

size_t mcp::block_store::approve_count(mcp::db::db_transaction & transaction_a)
{
	return transaction_a.count_get("approve");
}

void mcp::block_store::approve_count_add(mcp::db::db_transaction & transaction_a, uint32_t v)
{
	transaction_a.count_add("approve", v);
}

uint64_t mcp::block_store::last_mci_get(mcp::db::db_transaction & transaction_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(last_mci_key), value));
	uint64_t result(0);
	if (exists)
		result = ((dev::h64::Arith) mcp::slice_to_h64(value)).convert_to<uint64_t>();
	return result;
}

void mcp::block_store::last_mci_put(mcp::db::db_transaction & transaction_a, uint64_t const & last_mci_a)
{
	dev::h64 last_mci(last_mci_a);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(last_mci_key), mcp::h64_to_slice(last_mci));
}


uint64_t mcp::block_store::last_stable_mci_get(mcp::db::db_transaction & transaction_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(last_stable_mci_key), value));
	uint64_t result(0);
	if (exists)
		result = ((dev::h64::Arith)mcp::slice_to_h64(value)).convert_to<uint64_t>();
	return result;
}

void mcp::block_store::last_stable_mci_put(mcp::db::db_transaction & transaction_a, uint64_t const & last_stable_mci_a)
{
	dev::h64 last_stable_mci(last_stable_mci_a);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(last_stable_mci_key), mcp::h64_to_slice(last_stable_mci));
}


mcp::advance_info mcp::block_store::advance_info_get(mcp::db::db_transaction & transaction_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(advance_info_key), value));
	mcp::advance_info result;
	if (exists)
		result = mcp::advance_info(value);
	return result;
}

void mcp::block_store::advance_info_put(mcp::db::db_transaction & transaction_a, mcp::advance_info const & value_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(advance_info_key), value_a.val());
}


uint64_t mcp::block_store::last_stable_index_get(mcp::db::db_transaction & transaction_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(last_stable_index_key), value));
	uint64_t result(0);
	if (exists)
		result = ((dev::h64::Arith)mcp::slice_to_h64(value)).convert_to<uint64_t>();
	return result;
}

void mcp::block_store::last_stable_index_put(mcp::db::db_transaction & transaction_a, uint64_t const & last_stable_index_a)
{
	dev::h64 last_stable_index(last_stable_index_a);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(last_stable_index_key), mcp::h64_to_slice(last_stable_index));
}


void mcp::block_store::block_children_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & p_hash_a, std::list<mcp::block_hash> & c_hashs_a)
{
	mcp::block_child_key key(p_hash_a, mcp::block_hash(0));
	mcp::db::forward_iterator it(transaction_a.begin(static_cast<uint8_t>(StorePrefix::block_child), key.val()));
	while (true)
	{
		if (!it.valid())
			break;
		mcp::block_child_key child_key(it.key());
		if (child_key.hash != p_hash_a)
			break;

		c_hashs_a.push_back(child_key.child_hash);

		++it;
	}
}

void mcp::block_store::block_child_put(mcp::db::db_transaction & transaction_a, mcp::block_child_key const & key_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::block_child), key_a.val(), dev::Slice());
}


bool mcp::block_store::skiplist_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a, mcp::skiplist_info & skiplist_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::skiplist), mcp::h256_to_slice(hash_a), value));
	if (exists)
	{
		dev::RLP r(value);
		skiplist_a = mcp::skiplist_info(r);
	}
	return !exists;
}

void mcp::block_store::skiplist_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & hash_a, mcp::skiplist_info const & skiplist_a)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		skiplist_a.stream_RLP(s);
		s.swapOut(b_value);
	}
	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::skiplist), mcp::h256_to_slice(hash_a), s_value);
}

bool mcp::block_store::successor_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash & successor_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::successor), mcp::h256_to_slice(root_a), value, snapshot_a));
	if (exists)
	{
		successor_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::successor_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a, mcp::block_hash const & successor_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::successor), mcp::h256_to_slice(root_a), mcp::h256_to_slice(successor_a));
}

void mcp::block_store::successor_del(mcp::db::db_transaction & transaction_a, mcp::block_hash const & root_a)
{
	transaction_a.del(static_cast<uint8_t>(StorePrefix::successor), mcp::h256_to_slice(root_a));
}


bool mcp::block_store::genesis_hash_get(mcp::db::db_transaction & transaction_a, mcp::block_hash & genesis_hash)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(genesis_hash_key), value));
	if (exists)
	{
		genesis_hash = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::genesis_hash_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & genesis_hash)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(genesis_hash_key), mcp::h256_to_slice(genesis_hash));
}

// sync db
bool mcp::block_store::catchup_chain_summaries_get(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::summary_hash & hash_a)
{
	std::string value;
	dev::h64 index(index_a);
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::catchup_chain_summaries), mcp::h64_to_slice(index), value));
	if (exists)
	{
		hash_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::catchup_chain_summaries_put(mcp::db::db_transaction & transaction_a, uint64_t const & index_a, mcp::summary_hash const & hash_a)
{
	dev::h64 index(index_a);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::catchup_chain_summaries), mcp::h64_to_slice(index), mcp::h256_to_slice(hash_a));
}

void mcp::block_store::catchup_chain_summaries_del(mcp::db::db_transaction & transaction_a, uint64_t const & index_a)
{
	dev::h64 index(index_a);
	transaction_a.del(static_cast<uint8_t>(StorePrefix::catchup_chain_summaries), mcp::h64_to_slice(index));
}

void mcp::block_store::catchup_chain_summaries_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a)
{
	dev::Slicebytes start(8,0);
	dev::Slicebytes end(8, 0xFF);
	m_db->del_range(static_cast<uint8_t>(StorePrefix::catchup_chain_summaries), dev::Slice(start.data(), start.size()), dev::Slice(end.data(), end.size()), write_option_a);
}

bool mcp::block_store::catchup_chain_block_summary_get(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash & summary_hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::catchup_chain_block_summary), mcp::h256_to_slice(block_hash_a), value));
	if (exists)
	{
		summary_hash_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::catchup_chain_block_summary_put(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a, mcp::summary_hash const & summary_hash_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::catchup_chain_block_summary), mcp::h256_to_slice(block_hash_a), mcp::h256_to_slice(summary_hash_a));
}

void mcp::block_store::catchup_chain_block_summary_del(mcp::db::db_transaction & transaction_a, mcp::block_hash const & block_hash_a)
{
	transaction_a.del(static_cast<uint8_t>(StorePrefix::catchup_chain_block_summary), mcp::h256_to_slice(block_hash_a));
}

void mcp::block_store::catchup_chain_block_summary_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a)
{
	mcp::summary_hash min(0);

	std::array<uint8_t, mcp::summary_hash::size> maxValue;
	maxValue.fill(255);
	mcp::summary_hash max(maxValue.data(), mcp::summary_hash::ConstructFromPointer);

	m_db->del_range(static_cast<uint8_t>(StorePrefix::catchup_chain_block_summary), mcp::h256_to_slice(min), mcp::h256_to_slice(max), write_option_a);
}


bool mcp::block_store::catchup_chain_summary_block_get(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_hash_a, mcp::block_hash & block_hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::catchup_chain_summary_block), mcp::h256_to_slice(summary_hash_a), value));
	if (exists)
	{
		block_hash_a = mcp::slice_to_h256(value);
	}
	return !exists;
}

void mcp::block_store::catchup_chain_summary_block_put(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_hash_a, mcp::block_hash const & block_hash_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::catchup_chain_summary_block), mcp::h256_to_slice(summary_hash_a), mcp::h256_to_slice(block_hash_a));
}

void mcp::block_store::catchup_chain_summary_block_del(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_hash_a)
{
	transaction_a.del(static_cast<uint8_t>(StorePrefix::catchup_chain_summary_block), mcp::h256_to_slice(summary_hash_a));
}

void mcp::block_store::catchup_chain_summary_block_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a)
{
	mcp::summary_hash min(0);
	std::array<uint8_t, mcp::summary_hash::size> maxValue;
	maxValue.fill(255);
	mcp::summary_hash max(maxValue.data(), mcp::summary_hash::ConstructFromPointer);

	m_db->del_range(static_cast<uint8_t>(StorePrefix::catchup_chain_summary_block), mcp::h256_to_slice(min), mcp::h256_to_slice(max), write_option_a);
}

bool mcp::block_store::hash_tree_summary_exists(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::hash_tree_summary), mcp::h256_to_slice(summary_a), value));
	return exists;
}

void mcp::block_store::hash_tree_summary_put(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::hash_tree_summary), mcp::h256_to_slice(summary_a), dev::Slice());
}

void mcp::block_store::hash_tree_summary_del(mcp::db::db_transaction & transaction_a, mcp::summary_hash const & summary_a)
{
	transaction_a.del(static_cast<uint8_t>(StorePrefix::hash_tree_summary), mcp::h256_to_slice(summary_a));
}

void mcp::block_store::hash_tree_summary_clear(std::shared_ptr<rocksdb::WriteOptions> write_option_a)
{
	mcp::summary_hash min(0);
	std::array<uint8_t, mcp::summary_hash::size> maxValue;
	maxValue.fill(255);
	mcp::summary_hash max(maxValue.data(), mcp::summary_hash::ConstructFromPointer);

	m_db->del_range(static_cast<uint8_t>(StorePrefix::hash_tree_summary), mcp::h256_to_slice(min), mcp::h256_to_slice(max), write_option_a);
}

bool mcp::block_store::catchup_index_get(mcp::db::db_transaction & transaction_a, uint64_t & _v)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(catchup_index), value));
	if (exists)
	{
		_v = ((dev::h64::Arith)mcp::slice_to_h64(value)).convert_to<uint64_t>();
	}
	return !exists;
}

void mcp::block_store::catchup_index_put(mcp::db::db_transaction & transaction_a, uint64_t const& _v)
{
	dev::h64 v(_v);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(catchup_index), mcp::h64_to_slice(v));
}

void mcp::block_store::catchup_index_del(mcp::db::db_transaction & transaction_a)
{
	transaction_a.del(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(catchup_index));
}

bool mcp::block_store::catchup_max_index_get(mcp::db::db_transaction & transaction_a, uint64_t & _v)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(catchup_max_index), value));
	if (exists)
	{
		_v = ((dev::h64::Arith)mcp::slice_to_h64(value)).convert_to<uint64_t>();
	}
	return !exists;
}

void mcp::block_store::catchup_max_index_put(mcp::db::db_transaction & transaction_a, uint64_t const& _v)
{
	dev::h64 v(_v);
	transaction_a.put(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(catchup_max_index), mcp::h64_to_slice(v));
}

void mcp::block_store::catchup_max_index_del(mcp::db::db_transaction & transaction_a)
{
	transaction_a.del(static_cast<uint8_t>(StorePrefix::prop), mcp::h256_to_slice(catchup_max_index));
}

std::shared_ptr<dev::eth::TransactionReceipt> mcp::block_store::transaction_receipt_get(mcp::db::db_transaction & transaction_a, h256 const & hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::transaction_receipt), mcp::h256_to_slice(hash_a), value));
	std::shared_ptr<dev::eth::TransactionReceipt> result;
	if (exists)
	{
		dev::RLP r(value);
		result = std::make_shared<dev::eth::TransactionReceipt>(r);
	}
	return result;
}

void mcp::block_store::transaction_receipt_put(mcp::db::db_transaction & transaction_a, h256 const& hash_a, dev::eth::TransactionReceipt const& receipt)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		receipt.streamRLP(s);
		s.swapOut(b_value);
	}
	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::transaction_receipt), mcp::h256_to_slice(hash_a), s_value);
}

std::shared_ptr<dev::ApproveReceipt> mcp::block_store::approve_receipt_get(mcp::db::db_transaction & transaction_a, h256 const & hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::approve_receipt), mcp::h256_to_slice(hash_a), value));
	std::shared_ptr<dev::ApproveReceipt> result;
	if (exists)
	{
		dev::RLP r(value);
		result = std::make_shared<dev::ApproveReceipt>(r);
	}
	return result;
}

void mcp::block_store::approve_receipt_put(mcp::db::db_transaction & transaction_a, h256 const& hash_a, dev::ApproveReceipt const& receipt)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		receipt.streamRLP(s);
		s.swapOut(b_value);
	}
	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::approve_receipt), mcp::h256_to_slice(hash_a), s_value);
}

void mcp::block_store::epoch_approves_get(mcp::db::db_transaction & transaction_a, uint64_t const & epoch, std::list<h256> & hashs_a)
{
	mcp::epoch_approves_key key(epoch, h256());
	mcp::db::forward_iterator it(transaction_a.begin(static_cast<uint8_t>(StorePrefix::epoch_approves), key.val()));
	while (true)
	{
		if (!it.valid())
			break;
		mcp::epoch_approves_key approve_key(it.key());
		if (approve_key.epoch != epoch)
			break;

		hashs_a.push_back(approve_key.hash);

		++it;
	}
}

void mcp::block_store::epoch_approves_put(mcp::db::db_transaction & transaction_a, mcp::epoch_approves_key const & key_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::epoch_approves), key_a.val(), dev::Slice());
}

std::shared_ptr<mcp::witness_param> mcp::block_store::epoch_param_get(mcp::db::db_transaction & transaction_a, uint64_t const & epoch)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::epoch_param), mcp::h64_to_slice(h64(epoch)), value));
	std::shared_ptr<witness_param> result;
	if (exists)
	{
		dev::RLP r(value);
		result = std::make_shared<witness_param>(r);
	}
	return result;
}

void mcp::block_store::epoch_param_put(mcp::db::db_transaction & transaction_a, uint64_t const & epoch, witness_param & param)
{
	dev::bytes b_value = param.rlp();
	dev::Slice s_value((char *)b_value.data(), b_value.size());
	transaction_a.put(static_cast<uint8_t>(StorePrefix::epoch_param), mcp::h64_to_slice(h64(epoch)), s_value);
}

bool mcp::block_store::epoch_work_transaction_get(mcp::db::db_transaction & transaction_a, Epoch const & epoch, h256 & hash_a)
{
	std::string value;
	bool exists(transaction_a.get(static_cast<uint8_t>(StorePrefix::epoch_work_transaction), mcp::h64_to_slice(h64(epoch)), value));
	if (exists)
		hash_a = mcp::slice_to_h256(value);
	return exists;
}

void mcp::block_store::epoch_work_transaction_put(mcp::db::db_transaction & transaction_a, Epoch const & epoch, h256 const& hash_a)
{
	transaction_a.put(static_cast<uint8_t>(StorePrefix::epoch_work_transaction), mcp::h64_to_slice(h64(epoch)), mcp::h256_to_slice(hash_a));
}

mcp::StakingList mcp::block_store::GetStakingList(mcp::db::db_transaction & _transaction, Epoch const & _epoch)
{
	mcp::StakingList ret;
	std::string value;
	bool exists(_transaction.get(static_cast<uint8_t>(StorePrefix::stakingList), mcp::h64_to_slice(h64(_epoch)), value));
	if (exists)
	{
		dev::RLP r(value);
		assert_x(r.isList());
		for (dev::RLP _r : r)
		{
			assert_x(_r.itemCount() == 2);
			auto _a = (dev::Address)_r[0];
			auto _b = _r[1].toInt<dev::u256>();
			ret[_a] = _b;
		}	
	}
	return ret;
}

void mcp::block_store::PutStakingList(mcp::db::db_transaction & _transaction, Epoch const & _epoch, mcp::StakingList const & _sl)
{
	dev::bytes b_value;
	dev::RLPStream s;
	s.appendList(_sl.size());
	for (auto _v : _sl)
	{
		s.appendList(2); 
		s << _v.first << _v.second;
	}
	s.swapOut(b_value);

	dev::Slice s_value((char *)b_value.data(), b_value.size());
	_transaction.put(static_cast<uint8_t>(StorePrefix::stakingList), mcp::h64_to_slice(h64(_epoch)), s_value);
}

dev::h256 const mcp::block_store::version_key(0);
dev::h256 const mcp::block_store::genesis_hash_key(1);
dev::h256 const mcp::block_store::genesis_transaction_hash_key(2);
dev::h256 const mcp::block_store::last_mci_key(3);
dev::h256 const mcp::block_store::last_stable_mci_key(4);
dev::h256 const mcp::block_store::advance_info_key(5);
dev::h256 const mcp::block_store::last_stable_index_key(6);
dev::h256 const mcp::block_store::catchup_index(7);
dev::h256 const mcp::block_store::catchup_max_index(8);
