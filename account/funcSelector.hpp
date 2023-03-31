// Olympus: mcp C++ client, tools and libraries.
// Licensed under the GNU General Public License, Version 3.

/// @file
/// This file defines contract abi function selector.
#pragma once
#include <libdevcore/FixedHash.h>
#include <libdevcore/SHA3.h>

namespace dev
{
	inline dev::FixedHash<4> selectorFromSignatureH32(std::string const& _signature)
	{
		return dev::FixedHash<4>(dev::sha3(_signature), dev::FixedHash<4>::AlignLeft);
	}
}

