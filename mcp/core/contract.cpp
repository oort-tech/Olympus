#include "contract.hpp"

namespace mcp
{
	DENContractCaller NewDENContractCaller(dev::ContractCaller const& _caller)
	{
		auto parsed = dev::JSON(DENContractABI);
		return DENContractCaller(dev::NewBoundContract(DENContractAddress, parsed, _caller));
	}

	DENContractCaller DENCaller;

	///for main caller
	dev::bytes MainContractCaller::BatchTransfer(std::map<dev::Address, u256> const& _v)
	{
		std::string method = "batchTransfer";
		std::vector<dev::Address> address;
		std::vector<u256> values;
		for (auto it : _v)
		{
			address.push_back(it.first);
			values.push_back(it.second);
		}
		return contract.Pack(method, address, values);
	}

	Transaction InitMainContractTransaction()
	{
		TransactionSkeleton ts;
		ts.from = MainCallcAddress;
		ts.data = MainContractByteCode;
		ts.gasPrice = 10000000;
		//ts.gas = mcp::tx_max_gas;
		ts.gas = 1215903;
		ts.nonce = 1;///genesis account used 1
		Transaction _t(ts);
		_t.setSignature(h256("06460da14eb32eda6a16745b8f6632b2df073db58f5a801d7be8ee569be5c89f"), h256("087ddfd6e85788e21f2a3130c667b816add20ab718ab104f3f006ccfe1681565"), 1);
		return _t;
	}

	MainContractCaller NewMainContractCaller(dev::ContractCaller const& _caller)
	{
		auto parsed = dev::JSON(MainContractABI);
		return MainContractCaller(dev::NewBoundContract(MainContractAddress, parsed, _caller));
	}
	MainContractCaller MainCaller;
}
