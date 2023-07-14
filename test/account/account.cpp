#include <mcp/common/numbers.hpp>
#include <mcp/wallet/wallet.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/common/common.hpp>
#include <string>
#include <vector>

#include <cryptopp/sha.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/scrypt.h>
#include <cryptopp/pwdbased.h>

#include <cryptopp/cryptlib.h>
#include <cryptopp/secblock.h>
#include <cryptopp/scrypt.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

void test_create_account()
{
	//std::cout << "-------------test_create_account---------------" << std::endl;

	//dev::h128 kdf_salt;
	//mcp::random_pool.GenerateBlock((byte*)kdf_salt.data(), kdf_salt.size);
	//// kdf_salt = dev::h128("59555A1474D77707BC6CF1FA7DE67199");

	//dev::h128 iv;
	//mcp::random_pool.GenerateBlock((byte*)iv.data(), iv.size);
	//// iv = dev::h128("E9A53520669C4131592E581CA81E873C");

	//dev::h256 prv;
	//mcp::random_pool.GenerateBlock((byte*)prv.data(), prv.size);
	//// prv = dev::Secret("D79703A37D55FD5AFC17FA4BF98047F9C6592559ABE107D01FAD13F8CDD0CD2A");

	//std::string password = "12345678";

	//std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	//dev::Secret derive_pwd;
	//mcp::kdf kdf_v;
	//kdf_v.phs(derive_pwd, password, kdf_salt);

	//dev::h256 ciphertext;
	//mcp::encry::encryption(ciphertext.data(), prv.data(), prv.size, iv.data(), derive_pwd.data());
	//
	//dev::Public pub = dev::toPublic(dev::Secret(prv));

	//std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	//std::cout << "duration:" << dur.count() / 1000000 << "ms" << std::endl;

	//std::cout << "kdf_salt:" << kdf_salt.hex() << std::endl;
	//std::cout << "password:" << password << std::endl;
	//std::cout << "iv:" << iv.hex() << std::endl;
	//std::cout << std::endl;
	//std::cout << "ciphertext:" << ciphertext.hex() << std::endl;
	//std::cout << "prv:" << prv.hex() << std::endl;
	//std::cout << "pub:" << pub.hex() << std::endl;
	//std::cout << "account:" << toAddress(pub).hexPrefixed() << std::endl;
	//
	//std::cout << "------------------------------------------------------" << std::endl;

	//mcp::json js;
	//js["account"] = toAddress(pub).hexPrefixed();
	//js["kdf_salt"] = kdf_salt.hex();
	//js["iv"] = iv.hex();
	//js["ciphertext"] = ciphertext.hex();

	//std::cout << js.dump() << std::endl;
}

void test_account_encoding()
{
	std::cout << "-------------test_account_encoding---------------" << std::endl;
	std::string pass = "12345678";
	dev::h256 salt = dev::h256("f7833b3bf2ef37426141e09bc7d5437521253736a4aac6bd83fe586507b841dc");
	int N = 131072;
	int R = 8;
	int P = 1;
	
	dev::Secret derived;
	CryptoPP::Scrypt scrypt;
	scrypt.DeriveKey((byte*)derived.data(), derived.size, 
		(byte*)pass.data(), pass.length(),
		salt.data(), salt.size, 
		N, 
		R, 
		P);
	std::cout << "derived:" << toHex(derived.ref()) << std::endl;
	dev::h128 encryptKey(derived.ref().cropped(0, dev::h128::size));

	dev::Secret data("E4C31704E60E3CE2339A208482FFA743341AE365427CE1CEADEEE85D21ABF2E3");
	dev::h128 iv = dev::h128("c16adf7a5520dbdc08d7e08984451e0f");
	dev::h256 ciphertext;

	mcp::encry::aesCTRXOR(ciphertext, encryptKey, iv, data.ref());

	h128 a(derived.ref().cropped(dev::h128::size));
	auto b = a.asBytes() + ciphertext.asBytes();
	h256 mac = dev::sha3(b);

	std::cout << "encryptKey:" << encryptKey.hex() << std::endl;
	std::cout << "iv:" << iv.hex() << std::endl;
	std::cout << "data:" << toHex(data.ref()) << std::endl;
	std::cout << "ciphertext:" << ciphertext.hex() << std::endl;
	std::cout << "mac:" << mac.hex() << std::endl;

	std::cout << "address:" << dev::toAddress(dev::toPublic(data)) << std::endl;
}

void test_account_decrypt()
{
	std::cout << "-------------test_account_decrypt---------------" << std::endl;
	dev::Secret derived("cdad1a5eefcdf530fa147310e004ff763aa0278cbda97e9e34734edc052291d1");
	dev::h128 encryptKey(derived.ref().cropped(0, dev::h128::size));

	//dev::Secret data("E4C31704E60E3CE2339A208482FFA743341AE365427CE1CEADEEE85D21ABF2E3");
	dev::h128 iv = dev::h128("c16adf7a5520dbdc08d7e08984451e0f");
	dev::h256 ciphertext("79cb955240fa62deae7c05c6fed77e55cef7dfa7f70db49e4979c0f3403742bf");

	dev::h256 plainText;
	bytesConstRef ciphertextRef = ciphertext.ref();
	mcp::encry::aesCTRXOR(plainText, encryptKey, iv, ciphertextRef);

	std::cout << "plainText:" << plainText.hex() << std::endl;

	std::cout << "address:" << dev::toAddress(dev::toPublic(Secret(plainText.ref()))) << std::endl;
}