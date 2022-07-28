#pragma once

#include <chrono>
#include <cstddef>
#include <set>
#include <map>
#include <utility>
#include <mcp/common/numbers.hpp>
#include <libdevcore/Address.h>
#include <libdevcore/Guards.h>

namespace mcp
{
using namespace dev;
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

size_t const static max_data_size(32000); //32k
size_t const static skiplist_divisor(10);
uint64_t const static block_max_gas(8000000);
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
	size_t witness_count;
	size_t majority_of_witnesses;
	std::set<dev::Address> witness_list;
};

class param
{
public:
	static void init()
	{
		init_block_param();
		init_witness_param();
	}

	static mcp::block_param const & block_param(uint64_t const & last_summary_mci_a)
	{
		mcp::block_param const & b_param
			= find_by_last_summary_mci<mcp::block_param>(last_summary_mci_a, block_param_map);
		return b_param;
	}

	static mcp::witness_param const & witness_param(uint64_t const & last_summary_mci_a)
	{
		DEV_READ_GUARDED(m_mutex_witness){
			mcp::witness_param const & w_param
				= find_by_last_summary_mci<mcp::witness_param>(last_summary_mci_a, witness_param_map);
			return w_param;
		}
	}

	static bool is_witness(uint64_t const & last_summary_mci_a, dev::Address const & account_a)
	{
		DEV_READ_GUARDED(m_mutex_witness){
			mcp::witness_param const & w_param = witness_param(last_summary_mci_a);
			if (w_param.witness_list.count(account_a))
				return true;
			return false;
		}
	}

	static mcp::witness_param const & curr_witness_param()
	{
		DEV_READ_GUARDED(m_mutex_witness){
			auto it(witness_param_map.rbegin());
			assert_x(it != witness_param_map.rend());
			return it->second;
		}
	}

	static std::set<dev::Address> to_witness_list(std::vector<std::string> const & witness_strs)
	{
		std::set<dev::Address> witness_list;
		for (std::string w_str : witness_strs)
		{
			dev::Address w_acc(w_str);
			witness_list.insert(w_acc);
		}
		return witness_list;
	}

	static void add_witness_param(uint64_t const & last_summary_mci_a, mcp::witness_param &w_param){
		DEV_WRITE_GUARDED(m_mutex_witness){
			mcp::param::witness_param_map.insert({last_summary_mci_a, w_param });
		}
	}

private:
	static void init_block_param()
	{
		mcp::block_param b_param_v0;
		b_param_v0.max_parent_size = 16;
		b_param_v0.max_link_size = 4096;
		block_param_map.insert({ 0, b_param_v0 });

		//chain_id = (uint64_t)mcp::mcp_network;
		chain_id = (uint64_t)971;
		switch (mcp::mcp_network)
		{
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			gas_price = 10000000;
			break;
		}
		case mcp::mcp_networks::mcp_test_network:
		{
			gas_price = 10000000;
			break;
		}
		case mcp::mcp_networks::mcp_beta_network:
		{
			gas_price = 10000000;
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
			gas_price = (uint256_t)5e13;
			break;
		}
		default:
			assert_x_msg(false, "Invalid network");
		}
	}

	static void init_witness_param()
	{

		switch (mcp::mcp_network)
		{
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			std::vector<std::string> mini_test_witness_str_list_v0 = {
				"0x1144B522F45265C2DFDBAEE8E324719E63A1694C"
			};
			mcp::witness_param w_param_v0;
			w_param_v0.witness_count = 1;
			w_param_v0.majority_of_witnesses = w_param_v0.witness_count * 2 / 3 + 1;
			w_param_v0.witness_list = to_witness_list(mini_test_witness_str_list_v0);
			assert_x(w_param_v0.witness_list.size() == w_param_v0.witness_count);

			witness_param_map.insert({ 0, w_param_v0 });
			break;
		}
		case mcp::mcp_networks::mcp_test_network:
		{
			std::vector<std::string> test_witness_str_list_v0 = {
				"0x49a1b41e8ccb704f5c069ef89b08cd33f764e9b3",
				"0xf0821dc4ba9419b865aa412170377ca3b44cdb58",
				"0x329e6b5b8e59fc73d892958b2ff6a89474e3d067",
				"0x827cce78dc6ec7051f2d7bb9e7adaefba7ca3248",
				"0x918d3fe1dbff02fc7521d4a04b50017ce1a7c2ea",
				"0x929f336edb0a39ad5532a462d4a84e1546c5e5de",
				"0x1895ac1edc15389b905bb19537eb0c5b33d8c77a",
				"0x05174fa7ab39a36391b17850a2db9afdcf57190e",
				"0xa11b98c54d4189adda8eda97e13c214fedaf0a0f",
				"0xa65ec5c65031d668094cb1b81bb8253ea64a23d7",
				"0xba618c1e3e90d16e6c15d92ed198780dc4ad39c2",
				"0xc2cf7b9eb048c34c2b00175a884543366bbcd029",
				"0xc543a3868f3613eecd109761f71e31832ecf51ba",
				"0xdab8a5fb82eb24ad321751bb2dd8e4cc9a4e45e5"
			};
			mcp::witness_param w_param_v0;
			w_param_v0.witness_count = 14;
			w_param_v0.majority_of_witnesses = w_param_v0.witness_count * 2 / 3 + 1;
			w_param_v0.witness_list = to_witness_list(test_witness_str_list_v0);
			assert_x(w_param_v0.witness_list.size() == w_param_v0.witness_count);

			witness_param_map.insert({ 0, w_param_v0 });
			break;
		}
		case mcp::mcp_networks::mcp_beta_network:
		{
			std::vector<std::string> beta_witness_str_list_v0 = {
				"0x6d76b7de9fa746bdfe2d5462ff46778a06bb2c35",
				"0x7f4f900abde901c79c1fe91a81ccd876595eceac",
				"0x94ab8f03fffc515d332894ea4be45df8aeacff4e",
				"0x545c6ddf180635303a27d92954da916dde931006",
				"0xa5356ce9415722e6c71a66c31cea172c2ccd7d90",
				"0xac8720f7149e200b479cf0325d7d36e491c410c4",
				"0xae8b58cc95649df86ed4583c57d136ee6c057f74",
				"0xb3cb7476c6241a6a72809727ebe0cf2db5bec98d",
				"0xb5bb1e0e692d8e7cfd2b17d220318dded1f34eb4",
				"0xb62e7871da077799a5c834565d8c162da3ee334e",
				"0xb75bfe4aa1e9aa99a1d87017d68d023e2cca48ae",
				"0xc757c14c4e20d604227c27935cd9f37150d27626",
				"0xd4c19e0c6a219e3a0e0b7249667cea21a69a6fdc",
				"0xdf691895cf79f2ca139b3e5d0714280877971eea"
			};
			mcp::witness_param w_param_v0;
			w_param_v0.witness_count = 14;
			w_param_v0.majority_of_witnesses = w_param_v0.witness_count * 2 / 3 + 1;
			w_param_v0.witness_list = to_witness_list(beta_witness_str_list_v0);
			assert_x(w_param_v0.witness_list.size() == w_param_v0.witness_count);

			witness_param_map.insert({ 0, w_param_v0 });
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
			std::vector<std::string> live_witness_str_list_v0 = {
				"0x1EBEB508001C6F8FC1F87114DAE750D340EB402F",
				"0x2E308F70360D93307AF7EF8360B6AB5C521855BF",
				"0x3EA5BB9580A34DD866B2C831A3A6C277392BC18C",
				"0x9CABCF9D976EFACF73D8D03ABDF1C04E9911F00A",
				"0x31BAD08FE6B8E595763970F7D4CC219DE447C98C",
				"0x88F76825F13A98D0BBB32B2AC70AAADD2ECE4B67",
				"0x299F85C02DB107FF870E7085FE4FDFEEAB23D745",
				"0x389E1CCDE77191F42FD935A29EFF787B76BD6C0E",
				"0x424CEF3F560CD5ECF8EBB5BEE2D85F266452C49C",
				"0x0485E42D1C146E6A2E5C902A739E8BCB0603C141",
				"0x713F1D0BA5CD198BE4C539B2EB6C9A450479451D",
				"0x89146AB369D9F8C3699C2B9B061CDBF312052528",
				"0x381170D03819F5F1EB41FF6FE5D403470D13DB92",
				"0x474360B9DA0ADD7E598D6711B0E89BC7A952FC6C"
			};
			mcp::witness_param w_param_v0;
			w_param_v0.witness_count = 14;
			w_param_v0.majority_of_witnesses = w_param_v0.witness_count * 2 / 3 + 1;
			w_param_v0.witness_list = to_witness_list(live_witness_str_list_v0);
			assert_x(w_param_v0.witness_list.size() == w_param_v0.witness_count);

			witness_param_map.insert({ 0, w_param_v0 });
			break;
		}
		default:
			assert_x_msg(false, "Invalid network");
		}
	}

	template<class T>
	static T const & find_by_last_summary_mci(uint64_t const & last_summary_mci_a, std::map<uint64_t, T> const & maps_a)
	{
		for (auto it(maps_a.rbegin()); it != maps_a.rend(); it++)
		{
			uint64_t const & min_last_summary_mci(it->first);
			if (last_summary_mci_a >= min_last_summary_mci)
			{
				T const & result(it->second);
				return result;
			}
		}
		assert_x(false);
	}

	//min last summary mci -> block param
	static std::map<uint64_t, mcp::block_param> block_param_map;

	//min last summary mci -> witness param
	static std::map<uint64_t, mcp::witness_param> witness_param_map;
	static dev::SharedMutex m_mutex_witness;
};

}
