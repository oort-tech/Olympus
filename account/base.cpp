#include "base.hpp"

namespace dev
{
	BoundContract NewBoundContract(dev::h160 const& _address, ABI const& _abi, ContractCaller const& _caller)
	{
		return BoundContract(_address, _abi, _caller);
	}
}