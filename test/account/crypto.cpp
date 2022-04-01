#include <mcp/common/numbers.hpp>

#include <mcp/common/common.hpp>
#include <blake2/blake2.h>

#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include <string>
#include <vector>

#include <libdevcore/SHA3.h>

#include <mcp/p2p/dh_x25519.hpp>

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

void test_sha3()
{
	std::cout << "-------------sha3---------------" << std::endl;

	mcp::uint256_union input;
	input.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	std::cout << dev::sha3(input.ref()).hex();
	std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() << "ns" << std::endl;
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

		mcp::encry::sign(prv, message.ref(), signature);

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

void test_secp256k1()
{
	std::cout << "-------------secp256k1---------------" << std::endl;

	// mcp::signature sig;
	// sig.decode_hex("01635E3763EED1E7C1B7611F5CA8CF90C340BEB79BF7DC674D5146D08D32DA976585E03660DB6008988E3B001F1797B717E31BAC6E8A211F674C4A4D83347129FA");
	// std::string text = "01635E3763EED1E7C1B7611F5CA8CF90C340BEB79BF7DC674D5146D08D32DA976585E03660DB6008988E3B001F1797B717E31BAC6E8A211F674C4A4D83347129FA";

	// sig.r.decode_hex(text.substr(0, sig.r.size * 2));
	// sig.s.decode_hex(text.substr(sig.r.size * 2, sig.s.size * 2));
	
	// std::stringstream stream(text.substr((sig.r.size + sig.s.size) * 2, 2));
	// stream << std::hex << std::noshowbase;

	// uint v;
	// stream >> v;
	// sig.v = static_cast<byte>(v);
	
	// std::cout << "r: " << sig.r.to_string() << std::endl;
	// std::cout << "s: " << sig.s.to_string() << std::endl;
	// std::cout << "v: " << std::hex << (uint) sig.v << std::endl;
	// std::cout << "signature: " << sig.to_string() << std::endl;

	mcp::secret_key prv;
	prv.decode_hex("72A4E26A6EEFB3B91247FC866A0613E48C37546F1E3B212B455FA5D305B4F9BF");

	mcp::public_key pub;
	mcp::public_key_comp pub_comp;

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		if (mcp::encry::generate_public_from_secret(prv, pub, pub_comp)) {
			std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
			std::cout << "secp256k1_publickey duration:" << dur.count() << "ns" << std::endl;

			std::cout << "prv:" << prv.to_string() << std::endl;
			std::cout << "pub:" << pub.to_string() << std::endl;
			std::cout << "pub_comp:" << pub_comp.to_string() << std::endl;
		} else {
			std::cout << "secp256k1_publickey failed..." << std::endl;
			return;
		}
	}


	mcp::uint256_union message;
	dev::sha3("msg").ref().copyTo(message.ref());

	mcp::signature signature;

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		if (mcp::encry::sign(prv, message.ref(), signature)) {
			std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
			std::cout << "sign duration:" << dur.count() << "ns" << std::endl;

			std::cout << "message:" << message.to_string() << std::endl;
			std::cout << "signature:" << signature.to_string() << std::endl;
		} else {
			std::cout << "secp256k1 sign fail";
			return;
		}
	}

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		bool status = mcp::encry::verify(pub_comp, signature, message.ref());

		if (status)
		{
			std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
			std::cout << "sign_open duration:" << dur.count() << "ns" << std::endl;
		}
		else
		{
			std::cout << "secp256k1 sign verify fail";
			return;
		}
	}
}

void test_x25519()
{
	mcp::secret_key prv;
	prv.decode_hex("BC1C100CA9C2B7E2DF6D1F46744AD3F51D0532BF6ACA5063D382EFFF8A5E28C7");

	mcp::uint256_union message;
	message.decode_hex("AF8460A7D28A396C62D6C51620B87789C862ED8783374EEF7B783145F540EB19");

	mcp::secret_key sec;
	mcp::public_key_comp pub_comp;
	if (crypto_sign_seed_keypair(pub_comp.ref().data(), sec.ref().data(), prv.ref().data()) == 0) {
		dev::bytes cipher;
		mcp::p2p::encrypt_dh(pub_comp, message.ref(), cipher);
		std::cout << "Encrypted: " << mcp::bytes_to_hex(cipher) << std::endl;

		dev::bytes plain;
		mcp::p2p::dencrypt_dh(sec, dev::bytesConstRef(cipher.data(), cipher.size()), plain);
		std::cout << "Decrypted: " << mcp::bytes_to_hex(plain) << std::endl;
	}
}

void test_signature()
{
	std::string sig = "32a203eacdfd2647981dff155082e9d6d1c4c96cd0473de347ff7589c64118c91d84caa88930ce61121e1b160d1a1cd11401896b67380a7fc870d1cc3d6d679401";
	mcp::signature signature;

	if (signature.decode_hex(sig)) {
		std::cout << "decode error" << std::endl;
	}
	else {
		std::cout << signature.to_string() << std::endl;
	}
}