#include "utility.hpp"
#include <blake2/blake2.h>
#include <mcp/common/assert.hpp>
#include <boost/endian/conversion.hpp>

void mcp::open_or_create(std::fstream & stream_a, std::string const & path_a)
{
	stream_a.open(path_a, std::ios_base::in);
	if (stream_a.fail())
	{
		stream_a.open(path_a, std::ios_base::out);
	}
	stream_a.close();
	stream_a.open(path_a, std::ios_base::in | std::ios_base::out);
}

dev::h256 mcp::blake2b_hash(dev::bytesConstRef const & bytes)
{
	dev::h256 result;
	blake2b_state hash_l;
	auto status(blake2b_init(&hash_l, sizeof(result)));
	assert_x_msg(status == 0, "status:" + std::to_string(status));

	if (bytes.size() > 0)
		blake2b_update(&hash_l, bytes.data(), bytes.size());

	status = blake2b_final(&hash_l, result.data(), sizeof(result));
	assert_x_msg(status == 0, "status:" + std::to_string(status));
	return result;
}

dev::h256 mcp::blake2b_hash(std::vector<uint8_t> const & bytes)
{
	dev::bytesConstRef ref(bytes.data(), bytes.size());
	return blake2b_hash(ref);
}
