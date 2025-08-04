#include "param.hpp"

//std::map<uint64_t, mcp::block_param> mcp::param::block_param_map = {};
//std::map<uint64_t, mcp::witness_param> mcp::param::witness_param_map = {};
//dev::SharedMutex mcp::param::m_mutex_witness;
////std::shared_ptr<mcp::block_cache> mcp::param::cache = nullptr;
//mcp::witness_param mcp::param::init_param;

mcp::param* mcp::param::s_this = nullptr;
