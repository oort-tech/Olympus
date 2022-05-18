#include "config.hpp"

mcp::mcp_networks mcp::mcp_network = mcp::mcp_networks::mcp_live_network;

dev::u256 mcp::gas_price;
uint64_t mcp::chain_id;

bool mcp::is_test_network()
{
	return mcp_network == mcp_networks::mcp_test_network || mcp_network == mcp_networks::mcp_mini_test_network;
}

std::map<uint64_t, mcp::block_param> mcp::param::block_param_map = {};
std::map<uint64_t, mcp::witness_param> mcp::param::witness_param_map = {};
