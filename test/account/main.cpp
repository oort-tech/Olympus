#include <test/account/main.hpp>
#include <mcp/common/numbers.hpp>
#include <mcp/common/stopwatch.hpp>
#include <boost/endian/conversion.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/core/common.hpp>
#include <mcp/wallet/wallet.hpp>
#include <mcp/core/transaction.hpp>
#include <libdevcore/CommonJS.h>
using namespace dev;
using namespace mcp;

void test_number()
{
	/*
	mcp::stopwatch sw1;
	mcp::stopwatch sw2;

	for(int i = 0; i < 100000000; i++)
	{
		mcp::uint256_union x1;
		mcp::uint256_union x2;
		mcp::random_pool.GenerateBlock(x1.bytes.data(), x1.bytes.size());
		mcp::random_pool.GenerateBlock(x2.bytes.data(), x2.bytes.size());

		bool b1(false);
		bool b2(false);

		sw2.start();
		b2 = x1.number() < x2.number();
		sw2.stop();

		sw1.start();
		b1 = x1 < x2;
		sw1.stop();

		if (b1 != b2)
			std::cout << "error:b1 != b2";
	}

	std::cout << "ws1: " << sw1.get_elapsed().count() << " ms" << std::endl;
	std::cout << "ws2: " << sw2.get_elapsed().count() << " ms" << std::endl;
	*/
}

void test1()
{
	std::string msg = "f2052e88cb2b777773aa3abab2024cd9a2a305043584c0b18639dc303841c4fc";
	auto sec = dev::Secret("d79703a37d55fd5afc17fa4bf98047f9c6592559abe107d01fad13f8cdd0cd2a");
	static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
		secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
		&secp256k1_context_destroy
	};
	auto* ctx = s_ctx.get();
	secp256k1_pubkey rawPubkey;
	if (!secp256k1_ec_pubkey_create(ctx, &rawPubkey, sec.data()))
		return ;
	std::array<byte, 65> serializedPubkey;
	unsigned char output[32] = { 0 };
	auto serializedPubkeySize = serializedPubkey.size();
	secp256k1_ec_pubkey_serialize(
		ctx,
		serializedPubkey.data(),
		&serializedPubkeySize,
		&rawPubkey,
		SECP256K1_EC_COMPRESSED
	);
	std::cout << "1 rawPubkey:" << toHex(std::vector<unsigned char>(rawPubkey.data, rawPubkey.data + sizeof(rawPubkey.data))) << std::endl;
	std::cout << "1 rawPubkey:" << toHex(serializedPubkey) << std::endl;

	std::vector<uint8_t> proof; proof.resize(81);

	if (secp256k1_vrf_prove(proof.data(), sec.data(), &rawPubkey, msg.data(), msg.size())) {
		std::cout << "[send_approve] secp256k1_vrf_prove ok" << std::endl;
	}
	else {
		std::cout << "[send_approve] secp256k1_vrf_prove fail" << std::endl;
	}

	if (secp256k1_vrf_verify(output, proof.data(), serializedPubkey.data(), msg.data(), msg.size())) {
		std::cout << "[send_approve] secp256k1_vrf_verify ok" << std::endl;
	}
	else {
		std::cout << "[send_approve] secp256k1_vrf_verify fail" << std::endl;
	}
}

void test2()
{
	std::string msg = "f2052e88cb2b777773aa3abab2024cd9a2a305043584c0b18639dc303841c4fc";
	auto sec = dev::Secret("d79703a37d55fd5afc17fa4bf98047f9c6592559abe107d01fad13f8cdd0cd2a");
	secp256k1_pubkey rawPubkey = dev::toPublickey(sec);
	dev::Public _public = dev::toPublic(sec);
	dev::PublicCompressed _publicCompressed = dev::toPublicCompressed(_public);

	//secp256k1_pubkey rawPubkey;
	//auto pub = dev::toPublic(sec);
	//bytesConstRef(&pub.asBytes()).copyTo(bytesRef(rawPubkey.data, 64));

	h256 output;
	h648 proof;
	//unsigned char output[32] = { 0 };
	//std::vector<uint8_t> proof; proof.resize(81);

	if (secp256k1_vrf_prove(proof.data(), sec.data(), &rawPubkey, msg.data(), msg.size())) {
		std::cout << "[send_approve] secp256k1_vrf_prove ok" << std::endl;
		std::cout << "proof:" << proof.hex() << std::endl;
	}
	else {
		std::cout << "[send_approve] secp256k1_vrf_prove fail" << std::endl;
	}

	if (secp256k1_vrf_verify(output.data(), proof.data(), _publicCompressed.data(), msg.data(), msg.size())) {
		std::cout << "[send_approve] secp256k1_vrf_verify ok" << std::endl;
		std::cout << "out:" << output.hex() << std::endl;
	}
	else {
		std::cout << "[send_approve] secp256k1_vrf_verify fail" << std::endl;
	}
}

void test3()
{
	auto sec = dev::Secret("d79703a37d55fd5afc17fa4bf98047f9c6592559abe107d01fad13f8cdd0cd2a");
	static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
		secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
		&secp256k1_context_destroy
	};
	auto* ctx = s_ctx.get();
	secp256k1_pubkey rawPubkey = dev::toPublickey(sec);
	std::cout << "sec to rawPubkey:" << toHex(bytesRef(rawPubkey.data, 64)) << std::endl;

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
			ctx, &rawPubkey, _publicCompressed.data(), PublicCompressed::size))
			return;

		std::cout << "PublicCompressed to rawPubkey:" << toHex(bytesRef(rawPubkey.data, 64)) << std::endl;

		std::array<byte, 65> serializedPubkey;
		auto serializedPubkeySize = serializedPubkey.size();
		secp256k1_ec_pubkey_serialize(
			ctx, serializedPubkey.data(), &serializedPubkeySize, &rawPubkey, SECP256K1_EC_UNCOMPRESSED);
		assert(serializedPubkeySize == serializedPubkey.size());
		// Expect single byte header of value 0x04 -- uncompressed public key.
		assert(serializedPubkey[0] == 0x04);
		// Create the Public skipping the header.

		Public pu = Public{ &serializedPubkey[1], Public::ConstructFromPointer };
		
		std::cout << "PublicCompressed to pub:" << pu.hex() << std::endl;
	}

	{
		static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
			secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
			&secp256k1_context_destroy
		};
		auto* ctx = s_ctx.get();

		std::array<byte, 65> p;
		p[0] = 0x04;
		_public.ref().copyTo(bytesRef(&p[1],64));

		std::cout << "Public to p:" << toHex(p) << std::endl;

		secp256k1_pubkey rawPubkey;
		if (!secp256k1_ec_pubkey_parse(
			ctx, &rawPubkey, p.data(), 65))
			return;

		std::cout << "Public to rawPubkey:" << toHex(bytesRef(rawPubkey.data, 64)) << std::endl;

		PublicCompressed serializedPubkey;
		size_t serializedPubkeySize = PublicCompressed::size;
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


int main(int argc, char * const * argv)
{	
	test_abi();
	test_decode();
	//test1();
	//std::cout << ".........................................................." << std::endl;
	//test2();

	//test3();

	//auto t = "f86c01839896808252089461eb4109074a677f457d0725208587782e64aa55888ac7230489e80000808207b9a021dd688c2e96f6d2358b0d3e1bb52d3a4f4ba00127abfea8848091f332d4eea7a072f48cf31ed2df38c1a0eebbe7ff615ed8446efce916e3bfbbd1c7050d27d9b3";
	//mcp::Transaction _t(dev::jsToBytes(t, OnFailed::Throw), CheckTransaction::None);
	//std::cout << "hash:" << _t.sha3().hex() << std::endl;
	//std::cout << "hash:" << _t.sha3().size << std::endl;


	//bool error_a = false;
	//mcp::block_store store(error_a, "blockdb");

	//{
	//	mcp::db::db_transaction transaction(store.create_transaction());
	//	store.catchup_index_del(transaction);
	//	store.catchup_max_index_del(transaction);
	//	store.catchup_chain_summaries_clear();
	//	store.catchup_chain_summary_block_clear();
	//	store.catchup_chain_block_summary_clear();
	//}
	//
	//{
	//	mcp::summary_hash last_from_hash(0);
	//	uint64_t			index;
	//	uint64_t			max_index;
	//	mcp::db::db_transaction transaction(store.create_transaction());
	//	if (!store.catchup_index_get(transaction, index))//exist
	//	{
	//		if (store.catchup_max_index_get(transaction, max_index))//must be exist
	//		{
	//			std::cout << "max_index error, index:" << index;
	//		}

	//		if (store.catchup_chain_summaries_get(transaction, index, last_from_hash))
	//		{
	//			std::cout << "last_from_hash error, index:" << index;
	//		}
	//		std::cout << "index:" << index << " , max_index:" << max_index
	//			<< " , hash:" << last_from_hash.to_string();
	//	}
	//	else
	//		std::cout << "data is clear.";
	//}
	//

	//test_argon2();
	//test_sha512();
	//test_ed25519();
	//test_aes();
	// test_x25519();

	//test_create_account();
	// test_account_encoding();
	 //test_secp256k1();

	//test_secure_string();

	//test_number

	// test_account_decrypt();

	// test_signature();
	//test_sha3();
	/*
	mcp::uint256_union s_max("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
	std::cout << s_max.to_string() << std::endl;
	std::cout << s_max.number() << std::endl;

	h256 s_max1{ "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141" };
	std::cout << s_max1.hex() << std::endl;
	*/
	// test_encrypt_decrypt();
	// test_eth_sign();

	std::cout << std::endl;
	std::cout << "Press \"Enter\" to exit...";
	std::string ret;
	std::getline(std::cin, ret);
}