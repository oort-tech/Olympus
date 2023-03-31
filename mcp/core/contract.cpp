#include "contract.hpp"

namespace mcp
{
	DENContractCaller NewDENContractCaller(dev::ContractCaller const& _caller)
	{
		auto parsed = dev::JSON(DENContractABI);
		return DENContractCaller(dev::NewBoundContract(DENContractAddress, parsed, _caller));
	}

	DENContractCaller DENCaller;
}
