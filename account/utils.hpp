// Olympus: mcp C++ client, tools and libraries.
// Licensed under the GNU General Public License, Version 3.
/// @file
/// This file defines encoder.

#pragma once

#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <libdevcore/Common.h>
#include <libdevcore/FixedHash.h>

namespace dev
{
	using rational = boost::rational<dev::bigint>;

	// ToCamelCase converts an under-score string to a camel-case string
	std::string ToCamelCase(std::string input);

	// ResolveNameConflict returns the next available name for a given thing.
	// This helper can be used for lots of purposes:
	//
	// - In solidity function overloading is supported, this function can fix
	//   the name conflicts of overloaded functions.
	// - In golang binding generation, the parameter(in function, event, error,
	//	 and struct definition) name will be converted to camelcase style which
	//	 may eventually lead to name conflicts.
	//
	// Name conflicts are mostly resolved by adding number suffix.
	// 	 e.g. if the abi contains Methods send, send1
	//   ResolveNameConflict would return send2 for input send.
	std::string ResolveNameConflict(std::string rawName, std::function<bool(std::string const&)> used);

	// isLetter reports whether a given 'rune' is classified as a Letter.
	// This method is copied from reflect/type.go
	static bool isLetter(char ch)
	{
		return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_'; ///todo: do not support unicode letter.need add bigger than 0x80 and is a letter.
	}

	// isValidFieldName checks if a string is a valid (struct) field name or not.
	//
	// According to the language spec, a field name should be an identifier.
	//
	// identifier = letter { letter | unicode_digit } .
	// letter = unicode_letter | "_" .
	// This method is copied from reflect/type.go
	static bool isValidFieldName(std::string fieldName)
	{
		for (size_t i = 0; i < fieldName.length(); i++)
		{
			if (i == 0 && !isLetter(fieldName[0]))
				return false;

			if (!(isLetter(fieldName[i]) || std::isdigit(fieldName[i]))) {
				return false;
			}
		}

		return fieldName.length() > 0;
	}
}

