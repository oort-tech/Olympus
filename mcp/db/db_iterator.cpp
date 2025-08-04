#include "db_iterator.hpp"
#include "common.hpp"
#include <mcp/common/assert.hpp>

mcp::db::db_iterator::db_iterator(rocksdb::Slice const& prefix_a):
	m_it(nullptr),
	m_prefix(prefix_a)
{
}

mcp::db::db_iterator::~db_iterator()
{
	if (m_it != nullptr)
	{
		delete m_it;
	}
}

bool mcp::db::db_iterator::valid()
{
	if (!m_it)
		return false;

	bool valid(m_it->Valid());
	if (!valid)
		check_status(m_it->status());
	else
	{
		// if used same column GetIterator does not honor ReadOptions before commit(prefix_same_as_start, iterate_upper_bound)
		auto _key = m_it->key();
		assert_x(_key.size() != 0);
		auto _k = _key.data();
		assert_x(_k != nullptr);
		assert_x(m_prefix.size() < _key.size());

		if (memcmp(m_prefix.data(), _k, m_prefix.size()) != 0)
			valid = false;
	}
	return valid;
}

dev::Slice mcp::db::db_iterator::key()
{
	auto _k = m_it->key();
	int prefix_len = m_prefix.size();
	if (prefix_len == 0)
		return dev::Slice(_k.data(), _k.size());
	else
		return dev::Slice(_k.data() + prefix_len, _k.size() - prefix_len);
}

dev::Slice mcp::db::db_iterator::value()
{
	auto _v = m_it->value();
	return dev::Slice(_v.data(), _v.size());
}


mcp::db::forward_iterator::forward_iterator()
{
	m_it = nullptr;
}

mcp::db::forward_iterator::forward_iterator(rocksdb::Iterator * it_a)
{
	m_it = it_a;
	m_it->SeekToFirst();
}

mcp::db::forward_iterator::forward_iterator(rocksdb::Iterator * it_a, rocksdb::Slice const & k_a, rocksdb::Slice const& prefix_a):
	db_iterator(prefix_a)
{
	m_it = it_a;
	m_it->Seek(k_a);
}

mcp::db::forward_iterator::forward_iterator(mcp::db::forward_iterator && other_a)
{
	m_it = other_a.m_it;
	other_a.m_it = nullptr;
	m_prefix = other_a.m_prefix;
}

mcp::db::forward_iterator & mcp::db::forward_iterator::operator++()
{
	m_it->Next();
	return *this;
}

mcp::db::forward_iterator & mcp::db::forward_iterator::operator= (mcp::db::forward_iterator && other_a)
{
	if (m_it != nullptr)
	{
		delete m_it;
	}
	m_it = other_a.m_it;
	other_a.m_it = nullptr;
	m_prefix = other_a.m_prefix;
	return *this;
}


mcp::db::backward_iterator::backward_iterator()
{
	m_it = nullptr;
}

mcp::db::backward_iterator::backward_iterator(rocksdb::Iterator * it_a)
{
	m_it = it_a;
	m_it->SeekToLast();
	//LOG(m_log.info) << "test3" << rocksdb::get_perf_context()->ToString();
}

mcp::db::backward_iterator::backward_iterator(rocksdb::Iterator * it_a, rocksdb::Slice const & k_a, rocksdb::Slice const& prefix_a) :
	db_iterator(prefix_a)
{
	m_it = it_a;
	m_it->SeekForPrev(k_a);
	//LOG(m_log.info) << "test4" << rocksdb::get_perf_context()->ToString();
}

mcp::db::backward_iterator::backward_iterator(mcp::db::backward_iterator && other_a)
{
	m_it = other_a.m_it;
	other_a.m_it = nullptr;
	m_prefix = other_a.m_prefix;
}

mcp::db::backward_iterator & mcp::db::backward_iterator::operator++()
{
	m_it->Prev();
	return *this;
}

mcp::db::backward_iterator & mcp::db::backward_iterator::operator= (mcp::db::backward_iterator && other_a)
{
	if (m_it != nullptr)
	{
		delete m_it;
	}
	m_it = other_a.m_it;
	other_a.m_it = nullptr;
	m_prefix = other_a.m_prefix;
	return *this;
}

