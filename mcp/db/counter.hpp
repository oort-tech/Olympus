#pragma once
#include <rocksdb/merge_operator.h>
#include <mcp/common/assert.hpp>
#include <libdevcore/Log.h>

namespace mcp
{
	namespace db
	{
		inline void encode_fixed64(char* buf, uint64_t value)
		{
			buf[0] = static_cast<char>(value & 0xff);
			buf[1] = static_cast<char>((value >> 8) & 0xff);
			buf[2] = static_cast<char>((value >> 16) & 0xff);
			buf[3] = static_cast<char>((value >> 24) & 0xff);
			buf[4] = static_cast<char>((value >> 32) & 0xff);
			buf[5] = static_cast<char>((value >> 40) & 0xff);
			buf[6] = static_cast<char>((value >> 48) & 0xff);
			buf[7] = static_cast<char>((value >> 56) & 0xff);
		}

		inline uint64_t decode_fixed64(const char* ptr)
		{
			return (static_cast<uint64_t>(static_cast<unsigned char>(ptr[0])) |
				(static_cast<uint64_t>(static_cast<unsigned char>(ptr[1])) << 8) |
				(static_cast<uint64_t>(static_cast<unsigned char>(ptr[2])) << 16) |
				(static_cast<uint64_t>(static_cast<unsigned char>(ptr[3])) << 24) |
				(static_cast<uint64_t>(static_cast<unsigned char>(ptr[4])) << 32) |
				(static_cast<uint64_t>(static_cast<unsigned char>(ptr[5])) << 40) |
				(static_cast<uint64_t>(static_cast<unsigned char>(ptr[6])) << 48) |
				(static_cast<uint64_t>(static_cast<unsigned char>(ptr[7])) << 56));
		}

		inline void put_fixed64(std::string* dst, uint64_t value)
		{
			char buf[sizeof(value)];
			encode_fixed64(buf, value);
			dst->append(buf, sizeof(buf));
		}

		class UInt64SafeOperator : public rocksdb::AssociativeMergeOperator
		{
		public:
			virtual ~UInt64SafeOperator() = default;

			virtual bool Merge(
				const rocksdb::Slice& key, 
				const rocksdb::Slice* existing_value,//A value that already exists in the database (may be empty)
				const rocksdb::Slice& value, //the value to merge
				std::string* new_value,//Output: The new value after merging
				rocksdb::Logger* logger
			) const override 
			{
				if (value.size() != sizeof(uint64_t))
					assert_x_msg(false, "rocksdb merge Operand has invalid size");

				uint64_t base = 0;
				if (existing_value)
				{
					if (existing_value->size() != sizeof(uint64_t))
						assert_x_msg(false, "rocksdb merge Existing value has invalid size");
					base = decode_fixed64(existing_value->data());
				}

				const int64_t operand = static_cast<int64_t>(decode_fixed64(value.data()));
				if (operand < 0 && base < static_cast<uint64_t>(-operand))
					assert_x_msg(false, "rocksdb merge overflow, base:" + std::to_string(base) + " ,operand:" + std::to_string(operand));

				uint64_t result = (operand >= 0) ?
					base + operand :
					base - static_cast<uint64_t>(-operand);

				new_value->clear();
				put_fixed64(new_value, result);
				return true;
			}

			virtual const char* Name() const override 
			{
				return "uint64_safe_operator";
			}
		};
	}
}
