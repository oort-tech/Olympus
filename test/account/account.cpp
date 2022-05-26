#include <mcp/common/numbers.hpp>
#include <mcp/wallet/wallet.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/common/common.hpp>
#include <string>
#include <vector>

#include <cryptopp/sha.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/blake2.h>

void test_create_account()
{
	std::cout << "-------------test_create_account---------------" << std::endl;

	dev::h128 kdf_salt;
	// mcp::random_pool.GenerateBlock((byte*)kdf_salt.data(), kdf_salt.size);
	kdf_salt = dev::h128("59555A1474D77707BC6CF1FA7DE67199");

	dev::h128 iv;
	// mcp::random_pool.GenerateBlock((byte*)iv.data(), iv.size);
	iv = dev::h128("E9A53520669C4131592E581CA81E873C");

	dev::Secret prv;
	// mcp::random_pool.GenerateBlock((byte*)prv.data(), prv.size);
	prv = dev::Secret("D79703A37D55FD5AFC17FA4BF98047F9C6592559ABE107D01FAD13F8CDD0CD2A");

	std::string password = "12345678";

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	dev::Secret derive_pwd;
	mcp::kdf kdf_v;
	kdf_v.phs(derive_pwd, password, kdf_salt);

	dev::h256 ciphertext;
	mcp::encry::encryption(ciphertext.data(), prv.data(), prv.size, iv.data(), derive_pwd.data());
	
	dev::Public pub = dev::toPublic(prv);

	std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() / 1000000 << "ms" << std::endl;

	std::cout << "kdf_salt:" << kdf_salt.hex() << std::endl;
	std::cout << "password:" << password << std::endl;
	std::cout << "iv:" << iv.hex() << std::endl;
	std::cout << std::endl;
	std::cout << "ciphertext:" << ciphertext.hex() << std::endl;
	std::cout << "pub:" << pub.hex() << std::endl;
	std::cout << "account:" << toAddress(pub).hexPrefixed() << std::endl;
	
	std::cout << "------------------------------------------------------" << std::endl;

	mcp::json js;
	js["account"] = toAddress(pub).hexPrefixed();
	js["kdf_salt"] = kdf_salt.hex();
	js["iv"] = iv.hex();
	js["ciphertext"] = ciphertext.hex();

	std::cout << js.dump() << std::endl;
}

void test_account_encoding()
{
	std::cout << "-------------test_account_encoding---------------" << std::endl;
	/*
	dev::Public pub;
	pub.decode_hex("01635E3763EED1E7C1B7611F5CA8CF90C340BEB79BF7DC674D5146D08D32DA976585E03660DB6008988E3B001F1797B717E31BAC6E8A211F674C4A4D83347129");
	std::cout << "pub: " << pub.to_string() << std::endl;

	std::string account = fromPublic(pub).hexPrefixed();
	std::cout << "encode_account: " << account << std::endl;
	if (account == "0x4B11E15F46EADC68E34ED9FC0A4170079A8A69C6")
		std::cout << "encode_account: ok" << std::endl;
	else
		std::cout << "encode_account: fail" << std::endl;

	dev::Address decode_account(account);
	std::cout << "decode_account: " << decode_account.hexPrefixed() << std::endl;

	if (decode_account == fromPublic(pub))
		std::cout << "decode_account: ok" << std::endl;
	else
		std::cout << "decode_account: fail" << std::endl;
	*/
}

void test_account_decrypt()
{
	std::cout << "-------------test_account_decrypt---------------" << std::endl;
	dev::h128 kdf_salt = dev::h128("59555a1474d77707bc6cf1fa7de67199");

	dev::h128 iv = dev::h128("e9a53520669c4131592e581ca81e873c");

	dev::Secret prv;

	std::string password_a = "12345678";

	mcp::kdf m_kdf;
	dev::Secret derive_pwd;
	m_kdf.phs(derive_pwd, password_a, kdf_salt);

	dev::h256 ciphertext = dev::h256("7f94753fb4536fc9cc0345fdc9971276be19081271a4340d9579e269e58f7b5b");

	mcp::encry::dencryption(
		(unsigned char*)prv.data(),
		(const unsigned char*)ciphertext.data(),
		ciphertext.size,
		(const unsigned char*)iv.data(),
		(const unsigned char*)derive_pwd.data()
	);

	dev::Public compare = dev::toPublic(prv);

	std::cout << "public:" << compare.hex() << std::endl;
	std::cout << "account:" << toAddress(compare).hexPrefixed() << std::endl;
}