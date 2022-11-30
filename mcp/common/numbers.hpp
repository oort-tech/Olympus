#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include <cryptopp/osrng.h>
#include <unordered_set>

#include <libdevcore/Common.h>
#include <libdevcore/SHA3.h>
#include <libdevcore/Address.h>
#include <libdevcrypto/Common.h>

#include <mcp/common/assert.hpp>

namespace mcp
{
// We operate on streams of uint8_t by convention
using stream = std::basic_streambuf<uint8_t>;
// Random pool used by mcp.
// This must be thread_local as long as the AutoSeededRandomPool implementation requires it
extern thread_local CryptoPP::AutoSeededRandomPool random_pool;
using uint256_t = dev::u256;
// Read a raw byte stream the size of `T' and fill value.
template <typename T>
bool read(mcp::stream & stream_a, T & value)
{
	static_assert (std::is_pod<T>::value, "Can't stream read non-standard layout types");
	auto amount_read(stream_a.sgetn(reinterpret_cast<uint8_t *> (&value), sizeof(value)));
	return amount_read != sizeof(value);
}
template <typename T>
void write(mcp::stream & stream_a, T const & value)
{
	static_assert (std::is_pod<T>::value, "Can't stream write non-standard layout types");
	auto amount_written(stream_a.sputn(reinterpret_cast<uint8_t const *> (&value), sizeof(value)));
	assert_x(amount_written == sizeof(value));
}

// All keys and hashes are 256 bit.
using amount = dev::u256;
using block_hash = dev::h256;
using link_hash = dev::h256;
using summary_hash = dev::h256;
using data_hash = dev::h256;
using code_hash = dev::h256;
using state_root = dev::h256;
using sync_request_hash = dev::h256;

namespace p2p
{
	using node_id = dev::h512;
}
}

namespace boost
{
    template <>
    struct hash<dev::h256>
    {
        size_t operator() (dev::h256 const & value_a) const
        {
            std::hash<dev::h256> hash;
            return hash(value_a);
        }
    };

	template <>
	struct hash<dev::Address>
	{
		size_t operator() (dev::Address const & data_a) const
		{
			std::hash<dev::Address> hash;
			return hash(data_a);
		}
	};
}
