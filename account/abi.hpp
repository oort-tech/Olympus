// Olympus: mcp C++ client, tools and libraries.
// Licensed under the GNU General Public License, Version 3.

/// @file
/// This file defines contract abi.
#pragma once
#include "method.hpp"
#include "event.hpp"
#include "error.hpp"
#include <map>

namespace dev
{
	struct AbiMarshaling
	{
		std::string Type;
		std::string Name;
		Arguments Inputs;
		Arguments Outputs;

		/// Status indicator which can be: "pure", "view",
		/// "nonpayable" or "payable".
		std::string StateMutability;

		/// Deprecated Status indicators, but removed in v0.6.0.
		bool Constant;  /// True if function is either pure or view
		bool Payable; /// True if function is payable

		/// Event relevant indicator represents the event is
		/// declared as anonymous.
		bool Anonymous;
	};

	void from_json(const nlohmann::json& j, AbiMarshaling& p);
	using AbiMarshalings = std::vector<AbiMarshaling>;


	class ABI
	{
	public:
		ABI(){}
		ABI(AbiMarshalings const&);
		bool HasFallback() { return Fallback.Type == FunctionType::Fallback; }
		bool HasReceive() { return Receive.Type == FunctionType::Receive; }

		template<typename... Args>
		dev::bytes Pack(std::string const & name, Args const&... args);

		template<typename... Args>
		void Unpack(std::string const & name, dev::bytes const& data, Args&... result);

	private:
		Arguments getArguments(std::string const & name, dev::bytes const& data);
		Method Constructor;
		std::map<std::string, Method> Methods;
		std::map<std::string, Event> Events;
		std::map<std::string, Error> Errors;

		/// Additional "special" functions introduced in solidity v0.6.0.
		/// It's separated from the original default fallback. Each contract
		/// can only define one fallback and receive function.
		Method Fallback; // Note it's also used to represent legacy fallback before v0.6.0
		Method Receive;
	};

	/// Pack the given method name to conform the ABI. Method call's data
	/// will consist of method_id, args0, arg1, ... argN. Method id consists
	/// of 4 bytes and arguments are all 32 bytes.
	/// Method ids are created from the first 4 bytes of the hash of the
	/// methods string signature. (signature = baz(uint32,string32))
	template<typename... Args>
	inline dev::bytes ABI::Pack(std::string const & name, Args const&... args)
	{
		/// Fetch the ABI of the requested method
		if (name == "")
		{
			/// constructor
			return this->Constructor.Inputs.Pack(args...);
		}
		if (!Methods.count(name))
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		Method method = Methods[name];
		dev::bytes arguments = method.Inputs.Pack(args...);

		return method.ID + arguments;
	}

	/// Unpack unpacks the output according to the abi specification.
	template<typename... Args>
	inline void ABI::Unpack(std::string const & name, dev::bytes const& data, Args&... result)
	{
		Arguments args = getArguments(name, data);
		args.Unpack(data, result...);
	}

	ABI JSON(std::string reader);
	
	// UnpackRevert resolves the abi-encoded revert reason. According to the solidity
	// spec https://solidity.readthedocs.io/en/latest/control-structures.html#revert,
	// the provided revert reason is abi-encoded as if it were a call to a function
	// `Error(string)`. So it's a special tool for it.
	bool UnpackRevert(dev::bytes const& data, std::string& result);
}
