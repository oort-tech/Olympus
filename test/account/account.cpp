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
	std::cout << "-------------test_create_account---------------" << std::endl;

	dev::Secret prv;
	mcp::random_pool.GenerateBlock((dev::byte*)prv.data(), prv.size);
	// prv = dev::Secret("D79703A37D55FD5AFC17FA4BF98047F9C6592559ABE107D01FAD13F8CDD0CD2A");

	///KDF salt
	dev::h256 salt;
	mcp::random_pool.GenerateBlock(salt.data(), salt.size);
	// kdf_salt = dev::h128("59555A1474D77707BC6CF1FA7DE67199");

	int N = 131072;
	int R = 8;
	int P = 1;
	std::string _auth = "12345678";

	dev::Secret derivedKey;
	CryptoPP::Scrypt scrypt;
	scrypt.DeriveKey((dev::byte*)derivedKey.data(), derivedKey.size,
		(dev::byte*)_auth.data(), _auth.length(),
		salt.data(), salt.size,
		N,
		P,
		P);

	///aes encrypt key
	dev::h128 encryptKey(derivedKey.ref().cropped(0, dev::h128::size));
	///aes iv
	dev::h128 iv;
	mcp::random_pool.GenerateBlock(iv.data(), iv.size);
	// iv = dev::h128("E9A53520669C4131592E581CA81E873C");

	/// aes encrypt
	dev::h256 ciphertext;
	bytesConstRef _dataRef = prv.ref();
	mcp::encry::aesCTRXOR(ciphertext, encryptKey, iv, _dataRef);
	///
	dev::h256 mac = dev::sha3(dev::h128(derivedKey.ref().cropped(dev::h128::size)).asBytes() + ciphertext.asBytes());

	dev::Public pub = dev::toPublic(prv);

	std::cout << "ciphertext:" << ciphertext.hex() << std::endl;
	std::cout << "prv:" << toHex(prv.ref()) << std::endl;
	std::cout << "pub:" << pub.hex() << std::endl;
	std::cout << "account:" << toAddress(pub).hexPrefixed() << std::endl;
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
	scrypt.DeriveKey((dev::byte*)derived.data(), derived.size,
		(dev::byte*)pass.data(), pass.length(),
		salt.data(), salt.size, 
		N, 
		R, 
		P);
	std::cout << "derived:" << toHex(derived.ref()) << std::endl;
	dev::h128 encryptKey(derived.ref().cropped(0, dev::h128::size));

	dev::Secret data("E4C31704E60E3CE2339A208482FFA743341AE365427CE1CEADEEE85D21ABF2E3");
	dev::h128 iv = dev::h128("c16adf7a5520dbdc08d7e08984451e0f");
	dev::h256 ciphertext;

	bytesConstRef dataRef = data.ref();
	mcp::encry::aesCTRXOR(ciphertext, encryptKey, iv, dataRef);

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