#pragma once

#include <mcp/node/message.hpp>
#include <mcp/common/log.hpp>

#include <unordered_set>
#include <unordered_map>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <mcp/node/arrival.hpp>

namespace mcp
{

class unhandle_item
{
  public:
	unhandle_item() = default;
	unhandle_item(mcp::block_hash const &unhandle_hash_a, std::shared_ptr<mcp::block_processor_item> item_a, std::unordered_set<mcp::block_hash> const &dependency_hashs_a, std::unordered_set<mcp::block_hash> const &light_dependency_hashs_a);
	mcp::block_hash unhandle_hash;
	std::shared_ptr<mcp::block_processor_item> item;
	std::unordered_set<mcp::block_hash> dependency_hashs;
	std::unordered_set<mcp::block_hash> light_dependency_hashs;
};

class unhandle_cache
{
  public:
	unhandle_cache(std::shared_ptr<mcp::block_arrival> block_arrival_a, size_t const &capacity_a = 100000);

	bool add(mcp::block_hash const &hash_a, std::unordered_set<mcp::block_hash> const &dependency_hashs_a, std::unordered_set<mcp::block_hash> const &light_dependency_hashs_a, std::shared_ptr<mcp::block_processor_item> item_a);
	std::unordered_map<mcp::block_hash, std::shared_ptr<mcp::block_processor_item>> release_dependency(mcp::block_hash const &dependency_hash_a);
	void get_missings(size_t const & missing_limit_a, std::vector<mcp::block_hash>& missings_a, std::vector<mcp::block_hash>& light_missings_a);

	bool exists(mcp::block_hash const & block_hash_a);

	size_t unhandlde_size() const;
	size_t dependency_size() const;
    size_t missing_size() const;
	size_t light_missing_size() const;
    size_t tips_size() const;
   
    uint64_t add_unhandle_ok_count = 0;
    uint64_t unhandle_full_count = 0;
    uint64_t unhandle_exist_count = 0;

  private:
	void del_unhandle_in_dependencies(mcp::block_hash const &unhandle_a);

	//unhandle hash -> unhandle item
	std::unordered_map<mcp::block_hash, mcp::unhandle_item> m_unhandles;
	//dependency hash -> unhandle hashs
	std::unordered_map<mcp::block_hash, std::shared_ptr<std::unordered_set<mcp::block_hash>>> m_dependencies;

	std::unordered_set<mcp::block_hash> m_missings;
	std::unordered_set<mcp::block_hash> m_light_missings;
	std::unordered_set<mcp::block_hash> m_tips;

	size_t m_capacity;
    const int m_max_search_count = 100;
	std::mutex m_mutux;
	std::shared_ptr<mcp::block_arrival> m_block_arrival;

    mcp::log m_log = { mcp::log("node") };
};

} // namespace mcp
