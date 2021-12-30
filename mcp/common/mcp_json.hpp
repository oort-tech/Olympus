#pragma once
#include <mcp/common/json.hpp>
#include <mcp/common/fifo_map.hpp>

namespace mcp
{
    template<class K, class V, class dummy_compare, class A>
    using fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
    using json = nlohmann::basic_json<fifo_map>;
};







