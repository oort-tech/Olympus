#pragma once

#include "Precompiled.h"
#include <libdevcore/Common.h>
#include <mcp/common/EVMSchedule.h>
#include <libdevcore/Address.h>

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
		bool IsOIP6(uint64_t const& _mci)
		{
			return _mci >= OIP6Block;
		}

		dev::eth::EVMSchedule const& forkScheduleForBlockMci(uint64_t const& _mci) const;

		uint64_t OIP4And5Block = 0;// OIP4,OIP5 block
		uint64_t HalleyForkBlock = 0;
		uint64_t OIP6Block = 0;// OIP6 block

		/// Precompiled contracts as specified in the chain params.
		std::unordered_map<dev::Address, dev::eth::PrecompiledContract> precompiled;

		//EVMSchedule lastForkWithAdditionalEIPsSchedule;
	};
}
