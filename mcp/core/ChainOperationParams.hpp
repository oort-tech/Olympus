#pragma once

#include <libdevcore/Common.h>
#include <mcp/common/EVMSchedule.h>

namespace mcp
{
	struct ChainOperationParams
	{
		ChainOperationParams(){}

		bool IsOIP4(uint64_t const& _mci)
		{
			return _mci >= OIP4And5Block;
		}
		bool IsOIP5(uint64_t const& _mci)
		{
			return _mci >= OIP4And5Block;
		}

		dev::eth::EVMSchedule const& forkScheduleForBlockMci(uint64_t const& _mci) const;

		uint64_t OIP4And5Block = 0;// OIP4,OIP5 block
		uint64_t HalleyForkBlock = 0;
	};
}
