#include <test/account/main.hpp>
#include <mcp/common/numbers.hpp>
#include <mcp/common/stopwatch.hpp>
#include <boost/endian/conversion.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/core/common.hpp>
#include <mcp/wallet/wallet.hpp>
#include <mcp/core/transaction.hpp>
#include <libdevcore/CommonJS.h>

int main(int argc, char * const * argv)
{	
	test_abi();
	test_decode();
	test_vrf();
	test_sha512();
	test_aes();
	test_create_account();
	test_account_encoding();
	test_secp256k1();
	test_secure_string();
	test_account_decrypt();
	test_sha3();
	test_eth_sign();

	std::cout << std::endl;
	std::cout << "Press \"Enter\" to exit...";
	std::string ret;
	std::getline(std::cin, ret);
}