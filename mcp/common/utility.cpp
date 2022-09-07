#include "utility.hpp"
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

