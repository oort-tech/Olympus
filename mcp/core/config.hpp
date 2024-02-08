#pragma once

#include <chrono>
#include <cstddef>
#include <set>
#include <map>
#include <utility>
#include <mcp/common/numbers.hpp>
#include <libdevcore/Address.h>
#include <libdevcore/Guards.h>
#include <libdevcore/RLP.h>

namespace mcp
{
using namespace dev;
using WitnessList = std::set<dev::Address>;
using Epoch = uint64_t;
// Network variants with different genesis blocks and network parameters
enum class mcp_networks
{
	mcp_live_network = 1,
	mcp_beta_network = 2,
	mcp_test_network = 3,
	mcp_mini_test_network = 4
};

extern mcp::mcp_networks mcp_network;

extern dev::u256 gas_price;
extern uint64_t chain_id;

bool is_test_network();
mcp::uint256_t chainID();
Epoch epoch(uint64_t last_summary_mci);

uint64_t const epoch_period = 10000; ///Advance 10000 mci switching epochs
size_t const static max_data_size(131072); //128k
size_t const static skiplist_divisor(10);
uint64_t const static tx_max_gas(50000000);
//uint256_t const static tx_max_gas_fee(1000000000000000000);
uint64_t const static max_link_block_size(2048);

class block_param
{
public:
	size_t max_parent_size;
	size_t max_link_size;
};

class witness_param
{
public:
	witness_param() = default;
	witness_param(dev::RLP const & rlp);
	void streamRLP(dev::RLPStream & s) const;
	bytes rlp() const { RLPStream s; streamRLP(s); return s.out(); }

	size_t witness_count = 0;
	size_t majority_of_witnesses = 0;
	WitnessList witness_list = WitnessList();
};

}
