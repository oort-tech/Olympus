// Olympus: mcp C++ client, tools and libraries.
// Licensed under the GNU General Public License, Version 3.

/// @file
/// This file defines contract abi interface type.
#pragma once
#include "argument.hpp"

namespace dev
{
	class Error
	{
	public:
		Error() {};

	private:
		std::string	Name;
		Arguments	Inputs;
		std::string	str;

		/// Sig contains the string signature according to the ABI spec.
		/// e.g.	 error foo(uint32 a, int b) = "foo(uint32,int256)"
		/// Please note that "int" is substitute for its canonical representation "int256"
		std::string	Sig;

		/// ID returns the canonical representation of the error's signature used by the
		/// abi definition to identify event names and types.
		dev::h256 ID;
	};

}

