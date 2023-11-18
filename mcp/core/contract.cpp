#include "contract.hpp"
#include "param.hpp"
#include "genesis.hpp"
#include <libdevcore/CommonJS.h>

namespace mcp
{
	DENContractCaller NewDENContractCaller(dev::ContractCaller const& _caller)
	{
		auto parsed = dev::JSON(DENContractABI);
		return DENContractCaller(dev::NewBoundContract(DENContractAddress, parsed, _caller));
	}

	DENContractCaller DENCaller;

	///for main caller.
	dev::bytes MainContractCaller::DistributeRewards(std::map<dev::Address, u256> const& _v)
	{
		std::string method = "distributeRewards";
		h160s address;
		u256s values;
		for (auto it : _v)
		{
			address.push_back(it.first);
			values.push_back(it.second);
		}
		return contract.Pack(method, address, values);
	}

	///for initializer.
	dev::bytes MainContractCaller::InitWitnesses(WitnessList const& _v)
	{
		std::string method = "initWitnesses";
		h160s _a;
		_a.assign(_v.begin(), _v.end());
		return contract.Pack(method, _a);
	}

	std::pair<StakingList, int> MainContractCaller::GetWitnesses(int const& start)
	{
		std::string method = "getWitnesses";
		dev::CallOpts opts{ MainCallcAddress };
		dev::bytes ret = contract.Call(&opts, method, start);

		h160s _l;
		u256s _b;
		int _t;
		contract.Unpack(method, ret, _l, _b, _t);

		StakingList _r;
		for (int i = 0; i < _l.size(); i++)
		{
			u256 temp = 0;
			if (i < _b.size())
				temp = _b[i];
			_r.insert(std::make_pair(_l[i], temp));
		}

		return std::make_pair(_r,_t);
	}

	MainInfo MainContractCaller::GetMainInfo()
	{
		std::string method = "getInfo";
		dev::CallOpts opts{ MainCallcAddress };
		dev::bytes ret = contract.Call(&opts, method);

		MainInfo _r;
		auto p = boost::make_tuple(boost::ref(_r.amount), boost::ref(_r.onMci), boost::ref(_r.notOnMci));
		contract.Unpack(method, ret, p);
		return _r;
	}

	Transactions InitMainContractTransaction()
	{
		int count = mcp::param::genesis_witness_param().witness_count;
		WitnessList list = mcp::param::genesis_witness_param().witness_list;
		dev::u256 _base = jsToU256("1000000000000000000");
		dev::u256 _stakeAmount = 800000 * _base;

		Transactions _r;
		///100000 for system contract gas. 800000 * count for staking.
		TransactionSkeleton _tsInit;
		_tsInit.from = mcp::genesis::GenesisAddress;
		_tsInit.to = MainCallcAddress;
		_tsInit.gasPrice = mcp::gas_price;
		_tsInit.value = _stakeAmount * count + (100000*_base);
		_tsInit.gas = mcp::tx_max_gas;
		_tsInit.nonce = 1;
		Transaction _tInit(_tsInit);
		_tInit.setSignature(h256(0), h256(0), 0);
		_r.push_back(_tInit);

		///Deposit contract
		TransactionSkeleton _tsDeposit;
		_tsDeposit.from = MainCallcAddress;
		_tsDeposit.data = MainContractByteCodeDeposit;
		_tsDeposit.gasPrice = mcp::gas_price;
		_tsDeposit.value = 0;
		_tsDeposit.gas = mcp::tx_max_gas;
		_tsDeposit.nonce = 0;
		Transaction _tDeposit(_tsDeposit);
		_tDeposit.setSignature(h256(0), h256(0), 0);
		_r.push_back(_tDeposit);

		///Admin contract
		TransactionSkeleton _tsAdmin;
		_tsAdmin.from = MainCallcAddress;
		_tsAdmin.data = MainContractByteCodeAdmin;
		_tsAdmin.gasPrice = mcp::gas_price;
		_tsAdmin.value = 0;
		_tsAdmin.gas = mcp::tx_max_gas;
		_tsAdmin.nonce = 1;
		Transaction _tAdmin(_tsAdmin);
		_tAdmin.setSignature(h256(0), h256(0), 0);
		_r.push_back(_tAdmin);

		///Proxy contract
		TransactionSkeleton _tsProxy;
		_tsProxy.from = MainCallcAddress;
		_tsProxy.data = MainContractByteCodeProxy;
		_tsProxy.gasPrice = mcp::gas_price;
		_tsProxy.value = 0;
		_tsProxy.gas = mcp::tx_max_gas;
		_tsProxy.nonce = 2;
		Transaction _tProxy(_tsProxy);
		_tProxy.setSignature(h256(0), h256(0), 0);
		_r.push_back(_tProxy);

		///staking
		TransactionSkeleton _tsStaking;
		_tsStaking.from = MainCallcAddress;
		_tsStaking.to = MainContractAddress;
		_tsStaking.data = MainCaller.InitWitnesses(list);
		_tsStaking.gasPrice = mcp::gas_price;
		_tsStaking.value = _stakeAmount * count;
		_tsStaking.gas = mcp::tx_max_gas;
		_tsStaking.nonce = 3;
		Transaction _tStaking(_tsStaking);
		_tStaking.setSignature(h256(0), h256(0), 0);
		_r.push_back(_tStaking);

		///10000000 for contract reword.
		TransactionSkeleton _tsr;
		_tsr.from = mcp::genesis::GenesisAddress;
		_tsr.to = MainContractAddress;
		_tsr.gasPrice = mcp::gas_price;
		_tsr.value = 10000000 * _base;
		_tsr.gas = mcp::tx_max_gas;
		_tsr.nonce = 2;
		Transaction _tr(_tsr);
		_tr.setSignature(h256(0), h256(0), 0);
		_r.push_back(_tr);

		return _r;
	}

	MainContractCaller NewMainContractCaller(dev::ContractCaller const& _caller)
	{
		auto parsed = dev::JSON(MainContractABI);
		return MainContractCaller(dev::NewBoundContract(MainContractAddress, parsed, _caller));
	}
	MainContractCaller MainCaller;
}
