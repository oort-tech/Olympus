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
	kdf_salt.decode_hex("AF8460A7D28A396C62D6C51620B87789");

	mcp::uint128_union iv;
	//mcp::random_pool.GenerateBlock(iv.bytes.data(), iv.bytes.size());
	iv.decode_hex("A695DDC35ED9F3183A09FED1E6D92083");

	mcp::raw_key prv;
	//mcp::random_pool.GenerateBlock(prv.data.bytes.data(), prv.data.bytes.size());
	prv.data.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

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
	std::cout << "account:" << pub.to_account() << std::endl;

	if (mcp::mcp_network == mcp::mcp_networks::mcp_test_network)
	{
		if (ciphertext.to_string() == "96D6B77BC031116919956F1904F25601C29036A9232D638536964E8ADC034360"
			&&pub.to_string() == "34E85B176BE32EFAD87C9EB1EBFC6C54482A6BECBD297F9FDF3BFA8EA342162C"
			&& pub.to_account() == "mcp_3M3dbuG3hWoeykQroyhJssdS15Bzocyh7wryG75qUWDxoyzBca")
			std::cout << "create_account: ok" << std::endl;
		else
			std::cout << "create_account: fail" << std::endl;
	}
	else
	{
		if (ciphertext.to_string() == "84904D31C8FB89B7CE2A06DB1F2F3A1542EBE9782A595675836E0D83EFA0DB0F"
			&&pub.to_string() == "34E85B176BE32EFAD87C9EB1EBFC6C54482A6BECBD297F9FDF3BFA8EA342162C"
			&& pub.to_account() == "mcp_3M3dbuG3hWoeykQroyhJssdS15Bzocyh7wryG75qUWDxoyzBca")
			std::cout << "create_account: ok" << std::endl;
		else
			std::cout << "create_account: fail" << std::endl;
	}
}

void test_account_encoding()
{
	std::cout << "-------------test_account_encoding---------------" << std::endl;

	mcp::uint256_union pub;
	pub.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");
	std::cout << "pub: " << pub.to_string() << std::endl;

	std::string account = pub.to_account();
	std::cout << "encode_account: " << account << std::endl;
	if (account == "mcp_3fNUxH2ix1TvrfEMK3s4SVq3YxEhYjzQxDjnYDtEYYKkYVumhu")
		std::cout << "encode_account: ok" << std::endl;
	else
		std::cout << "encode_account: fail" << std::endl;

	mcp::uint256_union decode_pub;
	decode_pub.decode_account(account);
	std::cout << "decode_account:" << decode_pub.to_string() << std::endl;

	if(decode_pub == pub)
		std::cout << "decode_account: ok" << std::endl;
	else
		std::cout << "decode_account: fail" << std::endl;
}
