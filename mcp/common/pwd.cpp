#include "pwd.hpp"
#if defined(_WIN32)
#include <windows.h>
#else
#include <termios.h>
#endif

std::string mcp::getPassword(std::string const& _prompt)
{
	std::string msg;
#if defined(_WIN32)
	std::cout << _prompt << std::flush;
	// Get current Console input flags
	HANDLE hStdin;
	DWORD fdwSaveOldMode;
	bool isNotStdin(false);
	if ((hStdin = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	{
		assert_x(0);
	}
	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
	{
		isNotStdin = true;//for git bash
	}
	// Set console flags to no echo
	if (!isNotStdin && !SetConsoleMode(hStdin, fdwSaveOldMode & (~ENABLE_ECHO_INPUT)))
	{
		assert_x(0);
	}

	// Read the string
	std::string ret;
	std::getline(std::cin, ret);
	// Restore old input mode
	if (!isNotStdin && !SetConsoleMode(hStdin, fdwSaveOldMode))
	{
		assert_x(0);
	}
	//no echo need explicit enter at windows cmd.
	if (!isNotStdin)
	{
		std::cout << std::endl;
	}
	return ret;
#else
	struct termios oflags;
	struct termios nflags;
	char password[256];
	// disable echo in the terminal
	tcgetattr(fileno(stdin), &oflags);
	nflags = oflags;
	nflags.c_lflag &= ~ECHO;
	nflags.c_lflag |= ECHONL;
	if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
	{
		assert_x_msg(false, "tcsetattr");
	}
	printf("%s", _prompt.c_str());
	if (!fgets(password, sizeof(password), stdin))
	{
		assert_x_msg(false, "fgets");
	}
	password[strlen(password) - 1] = 0;
	// restore terminal
	if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
	{
		assert_x_msg(false, "tcsetattr get");
	}
	return password;
#endif
}



std::string mcp::createPassword(std::string const& _prompt)
{
	std::string ret;
	while (true)
	{
		ret = getPassword(_prompt);
		if (!validatePasswordSize(ret))
		{
			std::cout << "Invalid password! A valid password length must between 8 and 100.\n";
			continue;
		}
		if (!validatePassword(ret))
		{
			std::cout << "Invalid password! A valid password must contain characters from letters (a-Z, A-Z), digits (0-9) and special characters (!@#$%^&*).\n";
			continue;
		}
		std::string confirm = mcp::getPassword("Please confirm the passphrase by entering it again:");
		if (ret == confirm)
			break;
		std::cout << "Passwords were different. Try again.\n";
	}
	return ret;
}

bool mcp::validatePasswordSize(std::string const & _password)
{
	return _password.size() >= 8 && _password.size() <= 100;
}

bool mcp::validatePassword(std::string const & _password)
{
	bool result(true);

	static const char * legalchar = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
	for (auto i : _password)
	{
		if (std::strchr(legalchar, i) == nullptr)
		{
			result = false;
			break;
		}
	}
	return result;
}

