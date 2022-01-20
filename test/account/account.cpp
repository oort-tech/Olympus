#include <mcp/common/numbers.hpp>
#include <mcp/wallet/wallet.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/common/common.hpp>
#include <string>
#include <vector>

void test_create_account()
{
	std::cout << "-------------test_create_account---------------" << std::endl;

	mcp::uint128_union kdf_salt;
	//mcp::random_pool.GenerateBlock(kdf_salt.bytes.data(), kdf_salt.bytes.size());
	kdf_salt.decode_hex("59536608906137A7B8CA0D71A02D753C");

	mcp::uint128_union iv;
	//mcp::random_pool.GenerateBlock(iv.bytes.data(), iv.bytes.size());
	iv.decode_hex("49D19B50925B9F0E6CAD21D3D73AE34E");

	mcp::raw_key prv;
	//mcp::random_pool.GenerateBlock(prv.data.bytes.data(), prv.data.bytes.size());
	prv.data.decode_hex("72A4E26A6EEFB3B91247FC866A0613E48C37546F1E3B212B455FA5D305B4F9BF");

	std::string password = "123456";

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	mcp::raw_key derive_pwd;
	mcp::kdf kdf_v;
	kdf_v.phs(derive_pwd, password, kdf_salt);

	mcp::uint256_union ciphertext;
	ciphertext.encrypt(prv, derive_pwd, iv);

	mcp::public_key pub;
	mcp::encry::generate_public_from_secret(prv.data, pub);

	std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() / 1000000 << "ms" << std::endl;

	std::cout << "kdf_salt:" << kdf_salt.to_string() << std::endl;
	std::cout << "password:" << password << std::endl;
	std::cout << "derive_pwd:" << derive_pwd.data.to_string() << std::endl;
	std::cout << "iv:" << iv.to_string() << std::endl;
	std::cout << "prv:" << prv.data.to_string() << std::endl;
	std::cout << std::endl;
	std::cout << "ciphertext:" << ciphertext.to_string() << std::endl;
	std::cout << "pub:" << pub.to_string() << std::endl;
	std::cout << "account:" << mcp::account(pub).to_account() << std::endl;

	if (mcp::mcp_network == mcp::mcp_networks::mcp_test_network)
	{
		if (ciphertext.to_string() == "4311A4B0C71B444FA37FA76FD62FC2957F9E139F51C8D584A4ECCAF1463C4E35"
			&&pub.to_string() == "01635E3763EED1E7C1B7611F5CA8CF90C340BEB79BF7DC674D5146D08D32DA976585E03660DB6008988E3B001F1797B717E31BAC6E8A211F674C4A4D83347129"
			&& mcp::account(pub).to_account() == "0x4B11E15F46EADC68E34ED9FC0A4170079A8A69C6")
			std::cout << "create_account: ok" << std::endl;
		else
			std::cout << "create_account: fail" << std::endl;
	}
	else
	{
		if (ciphertext.to_string() == "4311A4B0C71B444FA37FA76FD62FC2957F9E139F51C8D584A4ECCAF1463C4E35"
			&&pub.to_string() == "01635E3763EED1E7C1B7611F5CA8CF90C340BEB79BF7DC674D5146D08D32DA976585E03660DB6008988E3B001F1797B717E31BAC6E8A211F674C4A4D83347129"
			&& mcp::account(pub).to_account() == "0x4B11E15F46EADC68E34ED9FC0A4170079A8A69C6")
			std::cout << "create_account: ok" << std::endl;
		else
			std::cout << "create_account: fail" << std::endl;
	}
}

void test_account_encoding()
{
	std::cout << "-------------test_account_encoding---------------" << std::endl;

	mcp::public_key pub;
	pub.decode_hex("01635E3763EED1E7C1B7611F5CA8CF90C340BEB79BF7DC674D5146D08D32DA976585E03660DB6008988E3B001F1797B717E31BAC6E8A211F674C4A4D83347129");
	std::cout << "pub: " << pub.to_string() << std::endl;

	std::string account = mcp::account(pub).to_account();
	std::cout << "encode_account: " << account << std::endl;
	if (account == "0x4B11E15F46EADC68E34ED9FC0A4170079A8A69C6")
		std::cout << "encode_account: ok" << std::endl;
	else
		std::cout << "encode_account: fail" << std::endl;

	mcp::account decode_account;
	decode_account.decode_account(account);
	std::cout << "decode_account: " << decode_account.to_account() << std::endl;

	if (decode_account == mcp::account(pub))
		std::cout << "decode_account: ok" << std::endl;
	else
		std::cout << "decode_account: fail" << std::endl;
}
