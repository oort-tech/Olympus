#pragma once
#include <mcp/common/utility.hpp>

namespace mcp
{
	std::string getPassword(std::string const &_prompt);
	std::string createPassword(std::string const &_prompt);
	bool validatePasswordSize(std::string const &_password);
	bool validatePassword(std::string const &_password);
}