//#include "column.hpp"
//#include "common.hpp"
//#include <rocksdb/filter_policy.h>

//mcp::db::db_column::db_column()
//{
//	//std::shared_ptr<rocksdb::ColumnFamilyOptions> cfops = default_column_family_options();
//	//m_column_families.push_back(rocksdb::ColumnFamilyDescriptor(rocksdb::kDefaultColumnFamilyName, *cfops));
//}
//
//mcp::db::db_column::~db_column()
//{
//}

//std::shared_ptr<rocksdb::ColumnFamilyOptions> mcp::db::db_column::default_column_family_options(std::shared_ptr<rocksdb::BlockBasedTableOptions> table_options)
//{ 
//	rocksdb::ColumnFamilyOptions faOption;
//	faOption.compression = rocksdb::CompressionType::kLZ4Compression;
//
//	faOption.write_buffer_size = 128 * 1024 * 1024;
//	//faOption.max_write_buffer_number = 4;
//	//faOption.min_write_buffer_number_to_merge = 4;
//	faOption.max_bytes_for_level_base = 1024 * 1024 * 1024;
//	//faOption.hard_pending_compaction_bytes_limit = 128 * 1024 * 1024 * 1024;
//	faOption.level_compaction_dynamic_level_bytes = true;
//	//faOption.optimize_filters_for_hits = true;
//	faOption.target_file_size_base = 128 * 1024 * 1024;
//	if (table_options)
//		faOption.table_factory.reset(NewBlockBasedTableFactory(*table_options));
//	
//
//	return std::make_shared<rocksdb::ColumnFamilyOptions>(faOption);
//}

//std::shared_ptr<rocksdb::BlockBasedTableOptions> mcp::db::db_column::default_table_options(std::shared_ptr<rocksdb::Cache> cache)
//{
//	rocksdb::BlockBasedTableOptions table_options;
//
//	if (cache)
//		table_options.block_cache = cache;
//
//	//Partitioned Index Filters
//	if (database_config::cache_filter)
//	{
//		table_options.index_type = rocksdb::BlockBasedTableOptions::IndexType::kTwoLevelIndexSearch;
//		table_options.partition_filters = true;
//		table_options.metadata_block_size = 4 * 1024;
//	}
//
//	table_options.data_block_index_type = rocksdb::BlockBasedTableOptions::DataBlockIndexType::kDataBlockBinaryAndHash;
//	table_options.filter_policy.reset(rocksdb::NewBloomFilterPolicy(10, false));
//	table_options.format_version = 4;
//	table_options.block_size = 16 * 1024;
//	return std::make_shared<rocksdb::BlockBasedTableOptions>(table_options);
//}

//rocksdb::ColumnFamilyHandle * mcp::db::db_column::get_column_family_handle(int index)
//{
//	//auto it = m_index.find(index);
//	//if (it == m_index.end())
//	//	return nullptr;
//
//	if (m_handles.size() < index)
//		return nullptr;
//
//	return m_handles[index];
//}

//int mcp::db::db_column::insert_column_families(std::string const& name, std::shared_ptr<rocksdb::ColumnFamilyOptions> cfops)
//{
//	rocksdb::ColumnFamilyOptions faOption = *cfops;
//	m_column_families.push_back(rocksdb::ColumnFamilyDescriptor(name, faOption));
//	return m_column_families.size() - 1;
//}

//void mcp::db::db_column::preserve_index()
//{
//	int index = 0;
//	for (auto handle : m_handles)
//	{
//		m_index.insert(std::pair<int, int>(index++, handle->GetID()));
//	}
//}


