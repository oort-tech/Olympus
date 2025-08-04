#pragma once

#include <libdevcore/Common.h>
#include <rocksdb/iterator.h>

namespace mcp
{
	namespace db
	{
		class db_iterator
		{
		public:
			db_iterator(rocksdb::Slice const& prefix_a = rocksdb::Slice());
			~db_iterator();
			bool valid();

			dev::Slice key();
			dev::Slice value();

			dev::Slice operator*() { return value(); };

		protected:
			rocksdb::Iterator* m_it;

			rocksdb::Slice m_prefix;
		};

		class forward_iterator : public db_iterator
		{
		public:
			forward_iterator();
			forward_iterator(rocksdb::Iterator* it_a);
			forward_iterator(rocksdb::Iterator* it_a, rocksdb::Slice const& k_a, rocksdb::Slice const& prefix_a = rocksdb::Slice());
			forward_iterator(forward_iterator &&);
			forward_iterator(forward_iterator const &) = delete;
			forward_iterator& operator++();
			forward_iterator& operator= (forward_iterator && other_a);
			forward_iterator& operator= (forward_iterator const &) = delete;
		};

		class backward_iterator : public db_iterator
		{
		public:
			backward_iterator();
			backward_iterator(rocksdb::Iterator* it_a);
			backward_iterator(rocksdb::Iterator* it_a, rocksdb::Slice const& k_a, rocksdb::Slice const& prefix_a = rocksdb::Slice());
			backward_iterator(backward_iterator && other_a);
			backward_iterator(backward_iterator const &) = delete;
			backward_iterator& operator++();
			backward_iterator& operator= (backward_iterator && other_a);
			backward_iterator& operator= (backward_iterator const &) = delete;
		};
	}
}
