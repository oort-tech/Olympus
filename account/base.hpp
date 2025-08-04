// Olympus: mcp C++ client, tools and libraries.
// Licensed under the GNU General Public License, Version 3.

/// @file
/// This file defines contract abi.
#pragma once
#include "abi.hpp"

namespace dev
{
	/// CallOpts is the collection of options to fine tune a contract call request.
	struct CallOpts
	{
		dev::h160	From;/// Optional the sender address, otherwise the first account is used
	};

	/// todo...
	/// TransactOpts is the collection of authorization data required to create a valid transaction.
	struct TransactOpts{};
	/// FilterOpts is the collection of options to fine tune filtering for events
	/// within a bound contract.
	struct FilterOpts {};

	/// call contract needs from, to, data, result
	using ContractCaller = std::function<dev::bytes(dev::h160 const&, dev::h160 const&, dev::bytes const&)>;
	class BoundContract
	{
	public:
		BoundContract(){}
		BoundContract(dev::h160 const& _address, ABI const& _abi, ContractCaller const& _caller):
			address(_address), 
			abi(_abi), 
			caller(_caller)
		{}

		/// Call invokes the (constant) contract method with params as input values and
		/// sets the output to result. The result type might be a single field for simple
		/// returns, a slice of interfaces for anonymous returns and a struct for named
		/// returns.
		template<typename... Args>
		dev::bytes Call(CallOpts* opts, std::string const & method, Args const&... args);

		///Pack arguments with method. for write abi.
		template<typename... Args>
		dev::bytes Pack(std::string const & method, Args const&... args);

		/// unpack codes with abi
		template<typename... Args>
		void Unpack(std::string const & method, dev::bytes const& output, Args&... args);
	private:
		dev::h160	address;	/// Deployment address of the contract on the blockchain
		ABI			abi;		/// Reflect based ABI to access the correct methods
		ContractCaller caller;	/// Read interface to interact with the blockchain
	};


	/// NewBoundContract creates a low level contract interface through which calls
	/// and transactions may be made through.
	BoundContract NewBoundContract(dev::h160 const& _address, ABI const& _abi, ContractCaller const& _caller);
	
	template<typename ...Args>
	inline dev::bytes BoundContract::Call(CallOpts * opts, std::string const & method, Args const & ...args)
	{
		//if (opts == nullptr)
		//	opts = new CallOpts;
		dev::bytes input = abi.Pack(method, args...);
		//dev::bytes output;
		//caller(opts->From, address, input, output);
		//return output;
		return caller(opts->From, address, input);
	}

	template<typename ...Args>
	inline dev::bytes BoundContract::Pack(std::string const & method, Args const & ...args)
	{
		return abi.Pack(method, args...);
	}

	template<typename ...Args>
	inline void BoundContract::Unpack(std::string const& method, dev::bytes const& output, Args& ...args)
	{
		abi.Unpack(method, output, args...);
	}

}
