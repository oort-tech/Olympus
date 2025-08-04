#include <test/account/main.hpp>
#include <mcp/common/numbers.hpp>
#include <mcp/common/stopwatch.hpp>
#include <boost/endian/conversion.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/core/common.hpp>
#include <mcp/wallet/wallet.hpp>
#include <mcp/core/transaction.hpp>
#include <libdevcore/CommonJS.h>
#include <mcp/db/counter.hpp>
#include <rocksdb/sst_file_manager.h>

rocksdb::Options default_DB_options()
{
	rocksdb::Options options;
	options.create_if_missing = true;
	options.create_missing_column_families = true;
	options.db_write_buffer_size = 512 * 1024 * 1024;
	options.max_background_jobs = 8;
	options.max_subcompactions = 4;
	options.compaction_pri = rocksdb::kMinOverlappingRatio;
	options.sst_file_manager = std::shared_ptr<rocksdb::SstFileManager>(rocksdb::NewSstFileManager(rocksdb::Env::Default(), nullptr, "", 0));
	options.atomic_flush = true;
	options.max_total_wal_size = 512 * 1024 * 1024;
	return options;
}

void check_status(rocksdb::Status const& _status)
{
	if (_status.ok())
		return;

	cnote << _status.ToString();
}

rocksdb::TransactionDB* m_db;
std::vector<rocksdb::ColumnFamilyDescriptor> m_FamilyDescriptor;
std::vector<rocksdb::ColumnFamilyHandle*> m_handles;
auto m_read_options = rocksdb::ReadOptions();
auto m_write_options = rocksdb::WriteOptions();

auto merge_value = [&](int64_t value) {
	std::string operand;
	mcp::db::put_fixed64(&operand, static_cast<uint64_t>(value));
	return m_db->Merge(rocksdb::WriteOptions(), "counter", operand);
};

auto get_value = [&]() {
	std::string result;
	check_status(m_db->Get(rocksdb::ReadOptions(), "counter", &result));
	uint64_t final_value = mcp::db::decode_fixed64(result.data());
	cnote << "value:" << final_value;
};

void checkValue(std::string _key)
{
	cnote << "----------------------------------";

	rocksdb::ColumnFamilyHandle* countHandle = m_handles[0];///count merge handle
	rocksdb::ColumnFamilyHandle* defaultHandle = m_handles[1];

	std::string value1;
	check_status(m_db->Get(m_read_options, _key, &value1));
	cnote << "1 get no Handle: " << value1;

	std::string value2;
	check_status(m_db->Get(m_read_options, defaultHandle, _key, &value2));
	cnote << "2 get defaultHandle: " << value2;

	std::string value3;
	check_status(m_db->Get(m_read_options, countHandle, _key, &value3));
	cnote << "3 get countHandle: " << value3;

	{
		auto it = m_db->NewIterator(m_read_options);
		it->SeekToFirst();
		while (it->Valid())
		{
			cnote << "4 it no Handle: " << it->key().ToString() << " " << it->value().ToString();
			it->Next();
		}
	}
	{
		auto it = m_db->NewIterator(m_read_options, defaultHandle);
		it->SeekToFirst();
		while (it->Valid())
		{
			cnote << "5 it defaultHandle: " << it->key().ToString() << " " << it->value().ToString();
			it->Next();
		}
	}
	{
		auto it = m_db->NewIterator(m_read_options, countHandle);
		it->SeekToFirst();
		while (it->Valid())
		{
			cnote << "6 it countHandle: " << it->key().ToString() << " " << it->value().ToString();
			it->Next();
		}
	}
}

void checkMergeValue()
{
	cnote << "----------------------------------";

	rocksdb::ColumnFamilyHandle* countHandle = m_handles[0];///count merge handle
	rocksdb::ColumnFamilyHandle* defaultHandle = m_handles[1];

	std::string value1;
	check_status(m_db->Get(m_read_options, "counter", &value1));
	cnote << "1 get no Handle: " << mcp::db::decode_fixed64(value1.data());

	std::string value2;
	check_status(m_db->Get(m_read_options, defaultHandle, "counter", &value2));
	cnote << "2 get defaultHandle: " << mcp::db::decode_fixed64(value2.data());

	std::string value3;
	check_status(m_db->Get(m_read_options, countHandle, "counter", &value3));
	cnote << "3 get countHandle: " << mcp::db::decode_fixed64(value3.data());

	{
		auto it = m_db->NewIterator(m_read_options);
		it->SeekToFirst();
		while (it->Valid())
		{
			cnote << "4 it no Handle: " << it->key().ToString() << " " << mcp::db::decode_fixed64(it->value().ToString().data());
			it->Next();
		}
	}
	{
		auto it = m_db->NewIterator(m_read_options, defaultHandle);
		it->SeekToFirst();
		while (it->Valid())
		{
			cnote << "5 it defaultHandle: " << it->key().ToString() << " " << mcp::db::decode_fixed64(it->value().ToString().data());
			it->Next();
		}
	}
	{
		auto it = m_db->NewIterator(m_read_options, countHandle);
		it->SeekToFirst();
		while (it->Valid())
		{
			cnote << "6 it countHandle: " << it->key().ToString() << " " << mcp::db::decode_fixed64(it->value().ToString().data());
			it->Next();
		}
	}
}

int main(int argc, char * const * argv)
{	
	uint8_t _block = 0x01;
	dev::Slicebytes _prefix(1, _block);
	cnote << "_prefix:" << _prefix << "_prefix:" << dev::toHex(_prefix);
	std::string _k = "abc";
	dev::Slice _key(_k.data(), _k.size());
	cnote << "_key:" << dev::toHex(_key);
	auto _rt = _prefix + _key;
	cnote << "_rt:" << dev::toHex(_rt);
	cnote << "s_value:" << dev::toHex(dev::Slice(_rt.data(), _rt.size()));
	rocksdb::Slice a("1");
	std::string b = a.ToString();

	{
		//family  #0
		auto tbops = mcp::db::default_column_table_options(mcp::db::get_table_cache(64));
		auto cfops = mcp::db::default_column_family_options(tbops);
		cfops->merge_operator = std::make_shared<mcp::db::UInt64SafeOperator>();
		cfops->max_successive_merges = 1000;
		m_FamilyDescriptor.push_back(rocksdb::ColumnFamilyDescriptor("count", *cfops));
	}
	{
		//family  #1
		auto tbops_prefix = mcp::db::default_column_table_options(mcp::db::get_table_cache(2048));
		if (rocksdb::BlockBasedTableOptions::IndexType::kBinarySearch == tbops_prefix->index_type)
			tbops_prefix->index_type = rocksdb::BlockBasedTableOptions::IndexType::kHashSearch;
		auto cfops_prefix = mcp::db::default_column_family_options(tbops_prefix);
		//merge
		cfops_prefix->merge_operator = std::make_shared<mcp::db::UInt64SafeOperator>();
		//cfops_prefix->paranoid_checks = true;
		cfops_prefix->prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(3));
		cfops_prefix->memtable_prefix_bloom_size_ratio = 0.02;
		m_FamilyDescriptor.push_back(rocksdb::ColumnFamilyDescriptor(rocksdb::kDefaultColumnFamilyName, *cfops_prefix));
	}
	rocksdb::Status status = rocksdb::TransactionDB::Open(
		default_DB_options(),
		rocksdb::TransactionDBOptions(),
		"./db",
		m_FamilyDescriptor,
		&m_handles,
		&m_db);
	if (!status.ok())
	{
		cnote << "open error." << status.ToString();
		return 0;
	}
	rocksdb::ColumnFamilyHandle* countHandle = m_handles[0];///count merge handle
	rocksdb::ColumnFamilyHandle* defaultHandle = m_handles[1];
	//put
	{
		check_status(m_db->Put(m_write_options, "aaa", "aaaaaa"));
		checkValue("aaa");		
	}
	//defaultHandle
	{
		check_status(m_db->Put(m_write_options, defaultHandle, "bbb", "bbbbbb"));
		checkValue("bbb");
	}
	//txn
	{
		rocksdb::Transaction* m_txn = m_db->BeginTransaction(m_write_options);
		check_status(m_txn->Put("ccc", "cccccc"));
		check_status(m_txn->Commit());
		checkValue("ccc");
	}
	//txn defaultHandle
	{
		rocksdb::Transaction* m_txn = m_db->BeginTransaction(m_write_options);
		check_status(m_txn->Put(defaultHandle, "ddd", "dddddd"));
		check_status(m_txn->Commit());
		checkValue("ddd");
	}
	cnote << ""; cnote << "";

	std::string operand;
	mcp::db::put_fixed64(&operand, static_cast<uint64_t>(1));
	//put
	{
		check_status(m_db->Merge(m_write_options, "counter", operand));
		checkMergeValue();
	}
	//defaultHandle
	{
		check_status(m_db->Merge(m_write_options, defaultHandle, "counter", operand));
		checkMergeValue();
	}
	//txn
	{
		rocksdb::Transaction* m_txn = m_db->BeginTransaction(m_write_options);
		check_status(m_txn->Merge("counter", operand));
		checkMergeValue();
		check_status(m_txn->Commit());
		checkMergeValue();
	}
	//txn defaultHandle
	{
		rocksdb::Transaction* m_txn = m_db->BeginTransaction(m_write_options);
		check_status(m_txn->Merge(defaultHandle, "counter", operand));
		check_status(m_txn->Commit());
		checkMergeValue();
	}

	{
		//check_status(merge_value(100));
		//checkMergeValue();

		//check_status(merge_value(-2)); // 减去 2
		//get_value(); // 当前值: 98

		//// 测试边界情况
		//check_status(merge_value(2)); // 加 2

		/////error
		//{
		//	//// 测试下溢
		//	//check_status(merge_value(-200)); // 尝试减去 101 (应该失败)
		//	//get_value(); // 值仍为 100 (操作失败)

		//	//// 测试溢出
		//	//uint64_t max_value = std::numeric_limits<uint64_t>::max();
		//	//std::string max_value_buf;
		//	//mcp::db::put_fixed64(&max_value_buf, max_value);
		//	//check_status(m_db->Put(rocksdb::WriteOptions(), "counter", max_value_buf));
		//	//check_status(merge_value(1)); // 尝试加 1 (应该失败)
		//	//check_status(m_db->Merge(rocksdb::WriteOptions(), "counter", "zzzzzzzz"));
		//}
		//get_value(); // 初始值: 100
	}

	/*test_abi();
	test_decode();
	test_vrf();
	test_sha512();
	test_aes();
	test_create_account();
	test_account_encoding();
	test_secp256k1();
	test_secure_string();
	test_account_decrypt();
	test_sha3();
	test_eth_sign();*/

	std::cout << std::endl;
	std::cout << "Press \"Enter\" to exit...";
	std::string ret;
	std::getline(std::cin, ret);
}