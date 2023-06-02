#include "contract.hpp"
#include <libdevcore/CommonJS.h>

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

	StakingList MainContractCaller::List()
	{
		std::string method = "list";
		dev::CallOpts opts{ MainCallcAddress };
		dev::bytes ret = contract.Call(&opts, method, true);

		std::vector<boost::tuple<dev::Address, dev::u256>> _l;
		contract.Unpack(method, ret, _l);

		StakingList _r;
		for (auto v : _l)
			_r.push_back(StakingInfo(v.get<0>(), v.get<1>()));

		return _r;
	}

	MainInfo MainContractCaller::GetMainInfo()
	{
		std::string method = "getinfo";
		dev::CallOpts opts{ MainCallcAddress };
		dev::bytes ret = contract.Call(&opts, method);

		MainInfo _r;
		auto p = boost::make_tuple(boost::ref(_r.amount), boost::ref(_r.onMci), boost::ref(_r.notOnMci));
		contract.Unpack(method, ret, p);
		return _r;
	}

	Transaction InitMainContractTransaction()
	{
		TransactionSkeleton ts;
		ts.from = MainCallcAddress;
		ts.data = MainContractByteCode;
		ts.gasPrice = 10000000;
		ts.value = jsToU256("100000000000000000000000000");///a hundred million
		ts.gas = 1843195;
		ts.nonce = 0;
		Transaction _t(ts);
		//_t.setSignature(h256("e767053c47fb1c069ae5cf0faada77ef5b6aeaaba5c2082b0a40ff0303493a07"), h256("7be79a3be63b25adc980c9179f348ab4921d70b6582ab910e29d7d3df6ba633e"), 1);
		_t.setSignature(h256("b78e35990bb2a7db14e3fe0e96f18d533a31fed460402f868a8ef37134098144"), h256("2a09aafe948dd0b6215cb8ef3f1ae30497fa90c6888433c3853648ab5b00c8cf"), 0);
		return _t;
	}

	MainContractCaller NewMainContractCaller(dev::ContractCaller const& _caller)
	{
		auto parsed = dev::JSON(MainContractABI);
		return MainContractCaller(dev::NewBoundContract(MainContractAddress, parsed, _caller));
	}
	MainContractCaller MainCaller;
}
