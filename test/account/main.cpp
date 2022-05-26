#include <test/account/main.hpp>
#include <mcp/common/numbers.hpp>
#include <mcp/common/stopwatch.hpp>
#include <blake2/blake2.h>
#include <boost/endian/conversion.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/core/common.hpp>
#include <mcp/wallet/wallet.hpp>

void test_number()
{
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

}

void test_work()
{
	std::cout << "-------------work---------------" << std::endl;

	mcp::uint64_union work(123456);
	mcp::uint256_union root;
	root.decode_hex("5E844EE4D2E26920F8B0C4B7846929057CFCE48BF40BA269B173648999630053");

	std::cout << "work:" << work.to_string() << std::endl;
	std::cout << "root:" << root.to_string() << std::endl;

	mcp::uint64_union output;

	blake2b_state hash;
	blake2b_init(&hash, output.bytes.size());
	blake2b_update(&hash, work.bytes.data(), work.bytes.size());
	blake2b_update(&hash, root.bytes.data(), root.bytes.size());
	blake2b_final(&hash, output.bytes.data(), output.bytes.size());

	std::cout << "output:" << output.to_string() << std::endl;
}

int main(int argc, char * const * argv)
{
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
	//test_blake2();
	//test_sha512();
	//test_ed25519();
	//test_aes();
	// test_x25519();

	test_create_account();
	// test_account_encoding();
	// test_secp256k1();

	//test_secure_string();

	//test_number

	//test_work();

	test_account_decrypt();

	// test_signature();
	//test_blake2();
	//test_sha3();
	/*
	mcp::uint256_union s_max("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
	std::cout << s_max.to_string() << std::endl;
	std::cout << s_max.number() << std::endl;

	h256 s_max1{ "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141" };
	std::cout << s_max1.hex() << std::endl;
	*/
	test_encrypt_decrypt();
	// test_eth_sign();

	std::cout << std::endl;
	std::cout << "Press \"Enter\" to exit...";
	std::string ret;
	std::getline(std::cin, ret);
}