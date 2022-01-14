#include <mcp/common/numbers.hpp>

#include <mcp/common/common.hpp>
#include <blake2/blake2.h>

#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include <string>
#include <vector>

void test_argon2()
{
	std::cout << "-------------argon2---------------" << std::endl;

	std::string password = "123456";
	mcp::uint128_union salt;
	salt.decode_hex("5E844EE4D2E26920F8B0C4B784692905");
	uint32_t version = 0x13;

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	mcp::uint256_union result;

	uint32_t memory_cost = 16 * 1024 * 1024;
	auto status = crypto_pwhash(result.bytes.data(),
		result.bytes.size(),
		password.data(),
		password.size(),
		salt.bytes.data(),
		1, memory_cost, crypto_pwhash_ALG_ARGON2ID13);

	std::chrono::milliseconds dur = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() << "ms" << std::endl;

	if (status != 0)
		std::cout << "Caculate argon2 fail";
	else
	{
		std::cout << "password: " << password << std::endl;
		std::cout << "memory_cost(KB): " << memory_cost / 1024 << std::endl;
		std::cout << "salt:" << salt.to_string() << std::endl;
		std::cout << "result:" << result.to_string() << std::endl;
	}
}

void test_blake2()
{
	std::cout << "-------------blake2---------------" << std::endl;

	mcp::uint256_union input;
	input.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	mcp::uint256_union result;
	blake2b_state state;
	auto status(blake2b_init(&state, result.bytes.size()));
	if(status == 0)
	{ 
		status = blake2b_update(&state, input.bytes.data(), input.bytes.size());
		if (status == 0)
		{ 
			status == blake2b_final(&state, result.bytes.data(), result.bytes.size());
			if (status == 0)
			{
				std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
				std::cout << "duration:" << dur.count() << "ns" << std::endl;

				std::cout << "input:" << input.to_string() << std::endl;
				std::cout << "result:" << result.to_string() << std::endl;

				return;
			}
		}
	}

	std::cout << "Caculate blake2 fail";
}

void test_sha512()
{
	std::cout << "-------------sha512---------------" << std::endl;

	mcp::uint256_union input;
	input.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	mcp::uint512_union result;

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	CryptoPP::SHA512 sha512;
	sha512.Update(input.bytes.data(), input.bytes.size());
	sha512.Final(result.bytes.data());

	std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() << "ns" << std::endl;

	std::cout << "input:" << input.to_string() << std::endl;
	std::cout << "result:" << result.to_string() << std::endl;
}

void test_ed25519()
{
	std::cout << "-------------ed25519---------------" << std::endl;

	mcp::uint256_union prv;
	prv.decode_hex("0000000000000000000000000000000000000000000000000000000000000000");

	mcp::public_key pub;

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		mcp::encry::generate_public_from_secret(prv, pub);
		
		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "ed25519_publickey duration:" << dur.count() << "ns" << std::endl;

		std::cout << "prv:" << prv.to_string() << std::endl;
		std::cout << "pub:" << pub.to_string() << std::endl;
	}



	mcp::uint256_union message;
	message.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	mcp::signature signature;

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		mcp::encry::sign(prv, pub, message.ref(), signature);

		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "sign duration:" << dur.count() << "ns" << std::endl;

		std::cout << "message:" << message.to_string() << std::endl;
		std::cout << "signature:" << signature.to_string() << std::endl;
	}

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		bool status = mcp::encry::verify(pub, signature, message.ref());

		if (status)
		{
			std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
			std::cout << "sign_open duration:" << dur.count() << "ns" << std::endl;
		}
		else
		{
			std::cout << "ed25519 sign verify fail";
			return;
		}
	}
}

void test_aes()
{
	std::cout << "-------------aes-256-ctr---------------" << std::endl;

	mcp::uint256_union key;
	key.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	mcp::uint128_union iv;
	iv.decode_hex("A695DDC35ED9F3183A09FED1E6D92083");

	mcp::uint256_union plaintext;
	plaintext.decode_hex("AF8460A7D28A396C62D6C51620B87789C862ED8783374EEF7B783145F540EB19");

	mcp::uint256_union ciphertext;
	mcp::uint256_union decrypttext;

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption enc(key.bytes.data(), key.bytes.size(), iv.bytes.data());
		enc.ProcessData(ciphertext.bytes.data(), plaintext.bytes.data(), plaintext.bytes.size());

		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "Encryption duration:" << dur.count() << "ns" << std::endl;

		std::cout << "key:" << key.to_string() << std::endl;
		std::cout << "iv:" << iv.to_string() << std::endl;
		std::cout << "plaintext:" << plaintext.to_string() << std::endl;
		std::cout << "ciphertext:" << ciphertext.to_string() << std::endl;
	}

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption dec(key.bytes.data(), key.bytes.size(), iv.bytes.data());
		dec.ProcessData(decrypttext.bytes.data(), ciphertext.bytes.data(), ciphertext.bytes.size());

		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "Decryption duration:" << dur.count() << "ns" << std::endl;

		if (decrypttext == plaintext)
		{
			std::cout << "Decryption ok" << std::endl;
		}
		else
		{
			std::cout << "Decryption fail" << std::endl;
		}	}

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		CryptoPP::AES::Encryption alg(key.bytes.data(), sizeof(key.bytes));
		CryptoPP::CTR_Mode_ExternalCipher::Encryption enc(alg, iv.bytes.data());
		enc.ProcessData(ciphertext.bytes.data(), plaintext.bytes.data(), sizeof(plaintext.bytes));

		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "Encryption duration:" << dur.count() << "ns" << std::endl;

		std::cout << "key:" << key.to_string() << std::endl;
		std::cout << "iv:" << iv.to_string() << std::endl;
		std::cout << "plaintext:" << plaintext.to_string() << std::endl;
		std::cout << "ciphertext:" << ciphertext.to_string() << std::endl;
	}

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		CryptoPP::AES::Encryption alg(key.bytes.data(), sizeof(key.bytes));
		CryptoPP::CTR_Mode_ExternalCipher::Decryption enc(alg, iv.bytes.data());
		enc.ProcessData(decrypttext.bytes.data(), ciphertext.bytes.data(), sizeof(ciphertext.bytes));

		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "Decryption duration:" << dur.count() << "ns" << std::endl;

		if (decrypttext == plaintext)
		{
			std::cout << "Decryption ok" << std::endl;
		}
		else
		{
			std::cout << "Decryption fail" << std::endl;
		}
	}
}
