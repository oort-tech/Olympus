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

/*
void test_argon2()
{
	std::cout << "-------------argon2---------------" << std::endl;

	std::string password = "123456";
	mcp::uint128_union salt;
	salt.decode_hex("5E844EE4D2E26920F8B0C4B784692905");
	uint32_t version = 0x13;

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	dev::h256 result;

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
*/


void test_sha3()
{
	/*
	std::cout << "-------------sha3---------------" << std::endl;

	dev::h256 input;
	input.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	std::cout << dev::sha3(input.ref()).hex();
	std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << "duration:" << dur.count() << "ns" << std::endl;
	*/
}

void test_sha512()
{
	/*
	std::cout << "-------------sha512---------------" << std::endl;

	dev::h256 input;
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
	*/
}

void test_ed25519()
{
	/*std::cout << "-------------ed25519---------------" << std::endl;

	dev::h256 prv;
	prv.decode_hex("0000000000000000000000000000000000000000000000000000000000000000");

	dev::Public pub;

	{
		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		mcp::encry::generate_public_from_secret(prv, pub);
		
		std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
		std::cout << "ed25519_publickey duration:" << dur.count() << "ns" << std::endl;

		std::cout << "prv:" << prv.to_string() << std::endl;
		std::cout << "pub:" << pub.to_string() << std::endl;
	}



	dev::h256 message;
	message.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	dev::Signature signature;

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
	}*/
}

void test_aes()
{
	/*
	std::cout << "-------------aes-256-ctr---------------" << std::endl;

	dev::h256 key;
	key.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	mcp::uint128_union iv;
	iv.decode_hex("A695DDC35ED9F3183A09FED1E6D92083");

	dev::h256 plaintext;
	plaintext.decode_hex("AF8460A7D28A396C62D6C51620B87789C862ED8783374EEF7B783145F540EB19");

	dev::h256 ciphertext;
	dev::h256 decrypttext;

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
	*/
}

void test_secp256k1()
{
	dev::Secret s("7CDE0F6EDCD3BCC088A6C0D30235DCCD7B29F1E0C5F1137CBD99734C348D3216");
	dev::KeyPair alias(s);
	std::cout << "-----------------------------start" << std::endl;
	std::cout << "test  PUB:" << dev::toHex(alias.pub()) << std::endl;
	std::cout << "test  sec:" << dev::toHex(alias.secret().ref()) << std::endl;
	dev::bytes buf1(2);buf1[0] = 0x1; buf1[1] = 0x2;
	dev::bytes Cipher;
	std::cout << "test  buf:" << dev::toHex(buf1) << std::endl;
	dev::encryptECIES(alias.pub(), &buf1, Cipher);
	std::cout << "test  Cipher:" << dev::toHex(Cipher) << std::endl;
	dev::bytes buf2(2);
	auto dd = dev::decryptECIES(alias.secret(), dev::bytesConstRef(&Cipher), buf2);
	std::cout << "dd:" << dd << std::endl;
	std::cout << "test  buf1:" << dev::toHex(buf2) << std::endl;
	std::cout << "-----------------------------end" << std::endl;

	// dev::Signature sig;
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
	/*
	dev::Secret prv;
	prv.decode_hex("72A4E26A6EEFB3B91247FC866A0613E48C37546F1E3B212B455FA5D305B4F9BF");

	dev::Public pub;
	dev::PublicCompressed pub_comp;

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


	dev::h256 message;
	dev::sha3("msg").ref().copyTo(message.ref());

	dev::Signature signature;

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
	*/
}

void test_x25519()
{
	/*
	dev::Secret prv;
	prv.decode_hex("BC1C100CA9C2B7E2DF6D1F46744AD3F51D0532BF6ACA5063D382EFFF8A5E28C7");

	dev::h256 message;
	message.decode_hex("AF8460A7D28A396C62D6C51620B87789C862ED8783374EEF7B783145F540EB19");

	/*
	dev::Secret sec;
	dev::PublicCompressed pub_comp;
	if (crypto_sign_seed_keypair(pub_comp.ref().data(), sec.ref().data(), prv.ref().data()) == 0) {
		dev::bytes cipher;
		mcp::p2p::encrypt_dh(pub_comp, message.ref(), cipher);
		std::cout << "Encrypted: " << mcp::bytes_to_hex(cipher) << std::endl;

		dev::bytes plain;
		mcp::p2p::dencrypt_dh(sec, dev::bytesConstRef(cipher.data(), cipher.size()), plain);
		std::cout << "Decrypted: " << mcp::bytes_to_hex(plain) << std::endl;
	}
	*/
	/*mcp::key_pair keys = mcp::key_pair::create();

	dev::bytes cipher;
	mcp::p2p::encrypt_dh(keys.pub_comp(), message.ref(), cipher);
	std::cout << "Encrypted: " << mcp::bytes_to_hex(cipher) << std::endl;

	dev::bytes plain;
	mcp::p2p::dencrypt_dh(keys.secret(), dev::bytesConstRef(cipher.data(), cipher.size()), plain);
	std::cout << "Decrypted: " << mcp::bytes_to_hex(plain) << std::endl;
	*/
}

void test_signature()
{
	/*
	std::string sig = "32a203eacdfd2647981dff155082e9d6d1c4c96cd0473de347ff7589c64118c91d84caa88930ce61121e1b160d1a1cd11401896b67380a7fc870d1cc3d6d679401";
	dev::Signature signature;

	if (signature.decode_hex(sig)) {
		std::cout << "decode error" << std::endl;
	}
	else {
		std::cout << signature.to_string() << std::endl;
	}
	*/
}

void test_encrypt_decrypt()
{
	/*mcp::key_pair senderKey = mcp::key_pair::create();
	mcp::key_pair receiverKey = mcp::key_pair::create();

	dev::Secret encKey;
	if (mcp::encry::get_encryption_key(encKey, receiverKey.pub_comp().data(), receiverKey.pub_comp().size, senderKey.secret())) {
		return;
	}

	dev::h128 iv = dev::h128("A695DDC35ED9F3183A09FED1E6D92083");

	dev::h256 plaintext = dev::h256("AF8460A7D28A396C62D6C51620B87789C862ED8783374EEF7B783145F540EB19");

	dev::h256 ciphertext;
	mcp::encry::encryption(ciphertext.data(), plaintext.data(), plaintext.size, iv.data(), encKey.data());

	dev::Secret decKey;
	if (mcp::encry::get_encryption_key(decKey, senderKey.pub_comp().data(), senderKey.pub_comp().size, receiverKey.secret())) {
		return;
	}

	std::cout << "encKey Text:" << dev::toHex(encKey.ref()) << std::endl;
	std::cout << "decKey Text:" << dev::toHex(decKey.ref()) << std::endl;

	dev::h256 decrypttext;
	mcp::encry::dencryption(decrypttext.data(), ciphertext.data(), ciphertext.size, iv.data(), decKey.data());

	std::cout << "Plain Text:" << plaintext.hex() << std::endl;
	std::cout << "Decrypted Text:" << decrypttext.hex() << std::endl;*/
}

void test_eth_sign()
{
	/*
	dev::bytes data;
	std::string data_text = "0x5363686f6f6c627573";
	if (mcp::hex_to_bytes(data_text, data))
	{
		return;
	}
	std::string prefix = "Ethereum Signed Message:\n" + std::to_string(data.size());

	dev::bytes msg;
	msg.resize(prefix.size() + data.size() + 1);
	msg[0] = 0x19;
	dev::bytesRef((unsigned char*) prefix.data(), prefix.size()).copyTo(dev::bytesRef(msg.data() + 1, prefix.size()));
	dev::bytesRef(data.data(), data.size()).copyTo(dev::bytesRef(msg.data() + prefix.size() + 1, data.size()));
	
	dev::bytes digest;
	CryptoPP::Keccak_256 hash;
	hash.Update((const byte*) msg.data(), msg.size());
	digest.resize(hash.DigestSize());
	hash.Final(digest.data());

	std::cout << "Message: " << msg.data() << std::endl;
	std::cout << "Keccak256" << mcp::bytes_to_hex(digest) << std::endl;
	*/
}