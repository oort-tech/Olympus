#pragma once
#include <account/base.hpp>
#include <libdevcore/Address.h>

namespace mcp
{
	const std::string DENContractABI = "[{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"name\":\"stores4\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"intstores\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"addressstores\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"stringstores\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"boolstores\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"bytesstores\",\"type\":\"bytes\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"key\",\"type\":\"int256\"},{\"internalType\":\"int256\",\"name\":\"v1\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"v2\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"v3\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"v4\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"v5\",\"type\":\"bytes\"}],\"name\":\"setreturns\",\"outputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"key\",\"type\":\"int256\"}],\"name\":\"getreturns\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"\",\"type\":\"bytes\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true}]";
	const dev::Address DENContractAddress = dev::Address("0xD2b53c6dcF4EB754DE108ec0420EE7EcfC1223B3");
	const dev::Address DENcallcAddress = dev::Address("0x1144b522f45265c2dfdbaee8e324719e63a1694c");

	class DENContractCaller
	{
	public:
		DENContractCaller(){}
		DENContractCaller(dev::BoundContract _contract) :contract(_contract) {}

		//demo
		// returns int v1, dev::Address v2, std::string v3, bool v4, dev::bytes v5
		template<typename... Args>
		void getreturns(int & v1, dev::h160& v2, std::string& v3, bool& v4, dev::bytes& v5, Args const&... args);
	private:
		dev::BoundContract contract;
	};

	DENContractCaller NewDENContractCaller(dev::ContractCaller const& _caller);

	/// args: abi input
	/// v1,v2,v3,v4,v5 return values
	template<typename... Args>
	inline void DENContractCaller::getreturns(int & v1, dev::h160& v2, std::string& v3, bool& v4, dev::bytes& v5, Args const&... args)
	{
		std::string method = "getreturns";
		dev::CallOpts opts{ DENcallcAddress };
		dev::bytes ret = contract.Call(&opts, method, args...);
		contract.Unpack(method, ret, v1, v2, v3, v4, v5);
	}

	///ContractCaller instance
	extern DENContractCaller DENCaller;
}
