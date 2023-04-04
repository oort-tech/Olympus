// Olympus: mcp C++ client, tools and libraries.
// Licensed under the GNU General Public License, Version 3.

/// @file
/// This file defines contract abi.
#pragma once

#include "type.hpp"

namespace dev
{
	class Argument
	{
	public:
		Argument(std::string const& _n, std::shared_ptr<Type> _t, bool const& _i): Name(_n), Typ(_t), Indexed(_i) {}
		std::string Name;
		std::shared_ptr<Type>    Typ;
		bool	Indexed;  /// indexed is only used by events
	};	

	class Arguments
	{
	public:
		Arguments() {};
		void push_back(Argument a) { abiArgs.push_back(a); }
		size_t size() const { return abiArgs.size(); }
		const Argument& operator[](int i) const { return abiArgs[i]; }

		/// pack inputs
		void pack(int& inputOffset, dev::bytes& variableInput, dev::bytes& ret){}
		template<typename FirstArg, typename... Args>
		void pack(int& inputOffset, dev::bytes& variableInput, dev::bytes& ret, FirstArg const& _firstArg, Args const&... args);

		template<typename... Args>
		dev::bytes Pack(Args const&... args);

		/// unpack outputs
		void unpack(std::vector<Argument> const& nonIndexedArgs, int& virtualArgs, dev::bytes const& data) {}
		template<typename FirstArg, typename... Args>
		void unpack(std::vector<Argument> const& nonIndexedArgs, int& virtualArgs, dev::bytes const& data, FirstArg& _firstArg, Args&... args);

		template<typename... Args>
		void Unpack(dev::bytes const& data, Args &... result);

	private:
		/// NonIndexed returns the arguments with indexed arguments filtered out.
		std::vector<Argument> NonIndexed();
		std::vector<Argument> abiArgs;
	};
	void from_json(const nlohmann::json& j, Arguments& p);


	template<typename FirstArg, typename... Args>
	inline void Arguments::pack(int& inputOffset, dev::bytes& variableInput, dev::bytes& ret, FirstArg const& _firstArg, Args const&... args)
	{
		/// param index
		int index = abiArgs.size() - sizeof...(args) - 1;
		/// param input type
		Argument input = abiArgs[index];
		/// pack the input
		dev::bytes packed = input.Typ->Pack(_firstArg);
		/// check for dynamic types
		if (input.Typ->isDynamicType())
		{
			/// set the offset
			ret += Type::encode(inputOffset);
			/// calculate next offset
			inputOffset += packed.size();
			/// append to variable input
			variableInput += packed;
		}
		else
			ret += packed;
		pack(inputOffset, variableInput, ret, args...);
	}

	template<typename... Args>
	inline dev::bytes Arguments::Pack(Args const&... args)
	{
		// Make sure arguments match up and pack them
		if (sizeof...(args) != abiArgs.size())
			BOOST_THROW_EXCEPTION(dev::FailedABI());

		// variable input is the output appended at the end of packed
		// output. This is used for strings and bytes types input.
		dev::bytes variableInput;
		int inputOffset = 0;
		for (auto abiArg : abiArgs)
			inputOffset += abiArg.Typ->getTypeSize();

		dev::bytes ret;
		pack(inputOffset, variableInput, ret, args...);
		// append the variable input at the end of the packed input
		ret += variableInput;

		return ret;
	}
	
	template<typename FirstArg, typename ...Args>
	inline void Arguments::unpack(std::vector<Argument> const& nonIndexedArgs, int& virtualArgs, dev::bytes const& data, FirstArg& _firstArg, Args& ...args)
	{
		/// param index
		int index = abiArgs.size() - sizeof...(args)-1;
		Argument output = abiArgs[index];
		output.Typ->Unpack((index + virtualArgs) * 32, data, _firstArg);
		if (output.Typ->T == ValueType::ArrayTy && !output.Typ->isDynamicType())
		{
			/// If we have a static array, like [3]uint256, these are coded as
			/// just like uint256,uint256,uint256.
			/// This means that we need to add two 'virtual' arguments when
			/// we count the index from now on.
			///
			/// Array values nested multiple levels deep are also encoded inline:
			/// [2][3]uint256: uint256,uint256,uint256,uint256,uint256,uint256
			///
			/// Calculate the full array size to get the correct offset for the next argument.
			/// Decrement it by 1, as the normal index increment is still applied.
			virtualArgs += output.Typ->getTypeSize() / 32 - 1;
		}
		else if (output.Typ->T == TupleTy && !output.Typ->isDynamicType())
		{
			/// If we have a static tuple, like (uint256, bool, uint256), these are
			/// coded as just like uint256,bool,uint256
			virtualArgs += output.Typ->getTypeSize() / 32 - 1;
		}
		///next
		unpack(nonIndexedArgs, virtualArgs, data, args...);
	}

	/// Unpack performs the operation hexdata -> boost::tuple.
	template<typename... Args>
	inline void Arguments::Unpack(dev::bytes const & data, Args &... result)
	{
		std::vector<Argument> nonIndexedArgs = NonIndexed();
		/// Make sure arguments match up and pack them
		if (sizeof...(result) != nonIndexedArgs.size())
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		int virtualArgs = 0;
		unpack(nonIndexedArgs, virtualArgs, data, result...);
	}
}

