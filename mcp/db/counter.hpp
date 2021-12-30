#pragma once
#include <mcp/db/db_transaction.hpp>
#include <rocksdb/merge_operator.h>

namespace mcp
{
	namespace db
	{
		inline void encode_fixed64(char* buf, uint64_t value)
		{
			buf[0] = value & 0xff;
			buf[1] = (value >> 8) & 0xff;
			buf[2] = (value >> 16) & 0xff;
			buf[3] = (value >> 24) & 0xff;
			buf[4] = (value >> 32) & 0xff;
			buf[5] = (value >> 40) & 0xff;
			buf[6] = (value >> 48) & 0xff;
			buf[7] = (value >> 56) & 0xff;
		}

		inline uint32_t decode_fixed32(const char* ptr) 
		{
			return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0])))
				| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8)
				| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16)
				| (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
		}

		inline uint64_t decode_fixed64(const char* ptr)
		{
			uint64_t lo = decode_fixed32(ptr);
			uint64_t hi = decode_fixed32(ptr + 4);
			return (hi << 32) | lo;
		}


		inline void put_fixed64(std::string* dst, uint64_t value)
		{
			char buf[sizeof(value)];
			encode_fixed64(buf, value);
			dst->append(buf, sizeof(buf));
		}

		class uint64_add_operator : public rocksdb::AssociativeMergeOperator 
		{
		public:
			virtual bool Merge(
				const rocksdb::Slice& key, 
				const rocksdb::Slice* existing_value,
				const rocksdb::Slice& value, 
				std::string* new_value,
				rocksdb::Logger* logger
			) const override 
			{
				uint64_t orig_value = 0;
				if (existing_value) 
				{
					orig_value = decode_integer(*existing_value, logger);
				}
				int64_t operand = decode_integer(value, logger);

				assert_x_msg(new_value, "rocksdb operate error, msg: merge value ptr is null");

				new_value->clear();
				uint64_t _value = orig_value + operand;

				put_fixed64(new_value, _value);

				return true;  // Return true always since corruption will be treated as 0
			}

			virtual const char* Name() const override 
			{
				return "uint64_add_operator";
			}

		private:
			uint64_t decode_integer(const rocksdb::Slice& value, rocksdb::Logger* logger) const 
			{
				uint64_t result = 0;

				if (value.size() == sizeof(uint64_t)) 
				{
					result = decode_fixed64(value.data());
				}
				else
				{
					std::string str = "rocksDB operate error, msg: data lenth error. data: ";
					str.append(value.data());
					assert_x_msg(false, str);
				}

				return result;
			}

		};

		class count_merge_operator : public rocksdb::AssociativeMergeOperator 
		{
		public:
			count_merge_operator() 
			{
				m_merge_operator = std::make_shared<uint64_add_operator>();
			}

			virtual bool Merge(const rocksdb::Slice& key,
				const rocksdb::Slice* existing_value,
				const rocksdb::Slice& value,
				std::string* new_value,
				rocksdb::Logger* logger) const override 
			{
				assert(new_value->empty());
				if (existing_value == nullptr) 
				{
					new_value->assign(value.data(), value.size());
					return true;
				}

				return m_merge_operator->PartialMerge(
					key,
					*existing_value,
					value,
					new_value,
					logger);
			}

			virtual const char* Name() const override 
			{
				return "uint64_add_operator";
			}

		private:
			std::shared_ptr<MergeOperator> m_merge_operator;
		};


		class db_transaction;
		class counters 
		{
		public:
			explicit counters(int const & index, uint64_t const & defaultCount = 0) :
				m_default(defaultCount),
				m_index(index)
			{
			}

			virtual ~counters() {}

			// mapped to a rocksdb Delete
			void del(db_transaction & txn_a, std::string const & key);

			// mapped to a rocksdb Get
			void get(db_transaction & txn_a, std::string const & key, uint64_t * value, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a);

		protected:
			uint64_t m_default;
			int m_index;
		};

		// Implement 'add' directly with the new Merge operation
		class merge_based_counters : public counters 
		{
		public:
			explicit merge_based_counters(int const & index, uint64_t const & defaultCount = 0)
				: counters(index, defaultCount)
			{
			}
			void get(db_transaction & txn_a, std::string const & key, uint64_t * value, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a);
			// mapped to a rocksdb Merge operation
			void add(std::string const& key, int64_t const& value);
			void commit(db_transaction& txn_a);
			void rollback() { clear(); }

		private:
			void clear() { m_merge.clear(); }
			std::unordered_map<std::string, int64_t> m_merge;
		};
	}
}
