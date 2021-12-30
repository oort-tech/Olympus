#include "counter.hpp"

void mcp::db::counters::del(db_transaction & txn_a, std::string const& key)
{
	txn_a.del(m_index, key);
}

void mcp::db::counters::get(db_transaction & txn_a, std::string const & key, uint64_t * value, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{

	std::string str;
	bool s = txn_a.get(m_index, key, str, snapshot_a);

	if (s) 
	{
		// deserialization
		if (str.size() != sizeof(uint64_t)) 
		{
			assert_x_msg(false, "rocksdb operate error, msg: value corruption");
		}
		*value = decode_fixed64(&str[0]);
	}
	else 
	{
		// return default value if not found;
		*value = m_default;
	}
}

void mcp::db::merge_based_counters::get(db_transaction & txn_a, std::string const & key, uint64_t * value, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a)
{
	counters::get(txn_a, key, value, snapshot_a);

	auto it = m_merge.find(key);
	if (it != m_merge.end())
	{
		*value += it->second;
	}
}

void mcp::db::merge_based_counters::add(std::string const& key, int64_t const& value)
{
	auto it = m_merge.find(key);
	if (it != m_merge.end())
	{
		it->second += value;
	}
	else
	{
		m_merge.insert(std::make_pair(key, value));
	}
}

void mcp::db::merge_based_counters::commit(db_transaction & txn_a)
{
	for (auto it : m_merge)
	{
		if (it.second == 0)
			continue;
		char encoded[sizeof(uint64_t)];
		encode_fixed64(encoded, it.second);
		dev::Slice slice(encoded, sizeof(uint64_t));
		txn_a.merge(m_index, it.first, slice);
	}

	//after commit need clear
	clear();
}
