#include <mcp/common/numbers.hpp>

#include <mcp/common/common.hpp>

#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include <string>
#include <vector>

#include <libdevcore/SHA3.h>

#include <cryptopp/hkdf.h>

#include <cryptopp/cryptlib.h>
#include <cryptopp/keccak.h>

using namespace dev;

void test_sha3()
{
	
	std::cout << "-------------sha3---------------" << std::endl;

	dev::h256 input("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	std::cout << dev::sha3(input.ref()).hex();
	std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() << "ns" << std::endl;
	
}

void test_sha512()
{
	
	std::cout << "-------------sha512---------------" << std::endl;

	dev::h256 input("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	dev::h512 result;

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	CryptoPP::SHA512 sha512;
	sha512.Update(input.data(), input.size);
	sha512.Final(result.data());

	std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() << "ns" << std::endl;

	std::cout << "input:" << input.hex() << std::endl;
	std::cout << "result:" << result.hex() << std::endl;
	
}

void test_aes()
{
	
	std::cout << "-------------aes-256-ctr---------------" << std::endl;

	dev::h256 key("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	dev::h128 iv("A695DDC35ED9F3183A09FED1E6D92083");

	dev::h256 plaintext("AF8460A7D28A396C62D6C51620B87789C862ED8783374EEF7B783145F540EB19");

	dev::h256 ciphertext;
	dev::h256 decrypttext;

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption enc(key.data(), key.size, iv.data());
		enc.ProcessData(ciphertext.data(), plaintext.data(), plaintext.size);

		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "Encryption duration:" << dur.count() << "ns" << std::endl;

		std::cout << "key:" << key.hex() << std::endl;
		std::cout << "iv:" << iv.hex() << std::endl;
		std::cout << "plaintext:" << plaintext.hex() << std::endl;
		std::cout << "ciphertext:" << ciphertext.hex() << std::endl;
	}

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption dec(key.data(), key.size, iv.data());
		dec.ProcessData(decrypttext.data(), ciphertext.data(), ciphertext.size);

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

		CryptoPP::AES::Encryption alg(key.data(), key.size);
		CryptoPP::CTR_Mode_ExternalCipher::Encryption enc(alg, iv.data());
		enc.ProcessData(ciphertext.data(), plaintext.data(), plaintext.size);

		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "Encryption duration:" << dur.count() << "ns" << std::endl;

		std::cout << "key:" << key.hex() << std::endl;
		std::cout << "iv:" << iv.hex() << std::endl;
		std::cout << "plaintext:" << plaintext.hex() << std::endl;
		std::cout << "ciphertext:" << ciphertext.hex() << std::endl;
	}

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		CryptoPP::AES::Encryption alg(key.data(), key.size);
		CryptoPP::CTR_Mode_ExternalCipher::Decryption enc(alg, iv.data());
		enc.ProcessData(decrypttext.data(), ciphertext.data(), ciphertext.size);

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
	auto sec = dev::Secret("d79703a37d55fd5afc17fa4bf98047f9c6592559abe107d01fad13f8cdd0cd2a");
	static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
		secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
		&secp256k1_context_destroy
	};
	auto* ctx = s_ctx.get();
	secp256k1_pubkey rawPubkey = dev::toPublickey(sec);
	std::cout << "sec to rawPubkey:" << dev::toHex(dev::bytesRef(rawPubkey.data, 64)) << std::endl;

	dev::Public _public = dev::toPublic(sec);
	std::cout << "sec to pub 512:" << _public.hex() << std::endl;
	dev::PublicCompressed _publicCompressed = dev::toPublicCompressed(sec);
	std::cout << "sec to PublicCompressed:" << _publicCompressed.hex() << std::endl;
	dev::Public pub2 = dev::toPublic(_publicCompressed);
	std::cout << "PublicCompressed to pub 512:" << pub2.hex() << std::endl;

	dev::PublicCompressed _publicCompressed2 = dev::toPublicCompressed(_public);
	std::cout << "pub 512 to PublicCompressed:" << _publicCompressed2.hex() << std::endl;

	{
		static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
			secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
			&secp256k1_context_destroy
		};
		auto* ctx = s_ctx.get();

		secp256k1_pubkey rawPubkey;
		if (!secp256k1_ec_pubkey_parse(
			ctx, &rawPubkey, _publicCompressed.data(), dev::PublicCompressed::size))
			return;

		std::cout << "PublicCompressed to rawPubkey:" << dev::toHex(dev::bytesRef(rawPubkey.data, 64)) << std::endl;

		std::array<dev::byte, 65> serializedPubkey;
		auto serializedPubkeySize = serializedPubkey.size();
		secp256k1_ec_pubkey_serialize(
			ctx, serializedPubkey.data(), &serializedPubkeySize, &rawPubkey, SECP256K1_EC_UNCOMPRESSED);
		assert(serializedPubkeySize == serializedPubkey.size());
		// Expect single byte header of value 0x04 -- uncompressed public key.
		assert(serializedPubkey[0] == 0x04);
		// Create the Public skipping the header.

		dev::Public pu = dev::Public{ &serializedPubkey[1], dev::Public::ConstructFromPointer };

		std::cout << "PublicCompressed to pub:" << pu.hex() << std::endl;
	}

	{
		static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
			secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
			&secp256k1_context_destroy
		};
		auto* ctx = s_ctx.get();

		std::array<dev::byte, 65> p;
		p[0] = 0x04;
		_public.ref().copyTo(dev::bytesRef(&p[1], 64));

		std::cout << "Public to p:" << dev::toHex(p) << std::endl;

		secp256k1_pubkey rawPubkey;
		if (!secp256k1_ec_pubkey_parse(
			ctx, &rawPubkey, p.data(), 65))
			return;

		std::cout << "Public to rawPubkey:" << dev::toHex(dev::bytesRef(rawPubkey.data, 64)) << std::endl;

		dev::PublicCompressed serializedPubkey;
		size_t serializedPubkeySize = dev::PublicCompressed::size;
		secp256k1_ec_pubkey_serialize(
			ctx, serializedPubkey.asArray().data(), &serializedPubkeySize, &rawPubkey, SECP256K1_EC_COMPRESSED);
		assert(serializedPubkeySize == serializedPubkey.asArray().size());
		// Expect single byte header of value 0x02 or 0x03 -- compressed public key.
		assert(serializedPubkey[0] == 0x02 || serializedPubkey[0] == 0x03);
		// Create the Public skipping the header.

		std::cout << "Public to PublicCompressed:" << serializedPubkey.hex() << std::endl;
	}

	std::cout << "pub toAddress:" << dev::toAddress(dev::toPublic(sec)).hex() << std::endl;
}

void test_eth_sign()
{
	dev::bytes data = dev::fromHex("0x5363686f6f6c627573");
	std::string prefix = "Ethereum Signed Message:\n" + std::to_string(data.size());

	dev::bytes msg;
	msg.resize(prefix.size() + data.size() + 1);
	msg[0] = 0x19;
	dev::bytesRef((unsigned char*) prefix.data(), prefix.size()).copyTo(dev::bytesRef(msg.data() + 1, prefix.size()));
	dev::bytesRef(data.data(), data.size()).copyTo(dev::bytesRef(msg.data() + prefix.size() + 1, data.size()));
	
	dev::bytes digest;
	CryptoPP::Keccak_256 hash;
	hash.Update((const dev::byte*) msg.data(), msg.size());
	digest.resize(hash.DigestSize());
	hash.Final(digest.data());

	std::cout << "Message: " << msg.data() << std::endl;
	std::cout << "Keccak256" << dev::toHex(digest) << std::endl;
	
}