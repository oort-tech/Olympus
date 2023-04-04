#include "utils.hpp"

namespace dev
{
	//ToCamelCase converts an under-score string to a camel-case string
	std::string ToCamelCase(std::string input)
	{
		std::vector <std::string> fields;
		boost::split_regex(fields, input, boost::regex("_"));
		for (auto it = fields.begin(); it != fields.end(); ++it)
			(*it)[0] = std::toupper((*it)[0]);
		return boost::algorithm::join(fields, "");
	}

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
	std::string ResolveNameConflict(std::string rawName, std::function<bool(std::string const&)> used)
	{
		std::string name = rawName;
		bool ok = used(name);
		for (size_t idx = 0; ok; idx++)
		{
			name = boost::str(boost::format("%1%(%2%)") % rawName % idx);
			ok = used(name);
		}
		return name;
	}
}

