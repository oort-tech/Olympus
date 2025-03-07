#include "ChainOperationParams.hpp"

dev::eth::EVMSchedule const& mcp::ChainOperationParams::forkScheduleForBlockMci(uint64_t const& _mci) const
{
    if (_mci >= HalleyForkBlock)
        return dev::eth::HalleySchedule;
    else
        return dev::eth::BerlinSchedule;
}
