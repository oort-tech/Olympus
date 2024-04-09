#pragma once

#include <libdevcore/Common.h>

namespace mcp
{
	struct ChainOperationParams
	{
		ChainOperationParams(){}

		bool IsGasUsedFork(uint64_t const& _mci)
		{
			return _mci >= gasUsedForkBlock;
		}
		uint64_t gasUsedForkBlock = 0;// gas used switch block , 0 = already on fork
	};
}
