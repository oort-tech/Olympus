#pragma once

#include <chrono>
#include <cstddef>
#include <set>
#include <map>
#include <utility>
#include <mcp/common/numbers.hpp>

namespace mcp
{
// Network variants with different genesis blocks and network parameters
enum class mcp_networks
{
	mcp_live_network = 1,
	mcp_beta_network = 2,
	mcp_test_network = 3,
	mcp_mini_test_network = 4
};

extern mcp::mcp_networks mcp_network;

bool is_test_network();


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
	std::set<mcp::account> witness_list;
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
		mcp::witness_param const & w_param
			= find_by_last_summary_mci<mcp::witness_param>(last_summary_mci_a, witness_param_map);
		return w_param;
	}

	static bool is_witness(uint64_t const & last_summary_mci_a, mcp::account const & account_a)
	{
		mcp::witness_param const & w_param = witness_param(last_summary_mci_a);
		if (w_param.witness_list.count(account_a))
			return true;
		return false;
	}

	static mcp::witness_param const & curr_witness_param()
	{
		auto it(witness_param_map.rbegin());
		assert_x(it != witness_param_map.rend());
		return it->second;
	}

private:
	static void init_block_param()
	{
		mcp::block_param b_param_v0;
		b_param_v0.max_parent_size = 16;
		b_param_v0.max_link_size = 4096;
		block_param_map.insert({ 0, b_param_v0 });

		switch (mcp::mcp_network)
		{
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			break;
		}
		case mcp::mcp_networks::mcp_test_network:
		{
			break;
		}
		case mcp::mcp_networks::mcp_beta_network:
		{
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
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
				"0x5D6DDC1717A32A49CAB87A6BCC3ECFE42675F9AD"
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
				"0x5D6DDC1717A32A49CAB87A6BCC3ECFE42675F9AD",
				"mcp3NKc8iLShwqPHf6teuZGAQacs2JWSKETGrKw7G9paK8HjsaLgw",
				"mcp3PnmJLSmUAdLLPS1CvvUy4Tu8AHNMxPWbgdpbmGWiWVkRcwGjG",
				"mcp3Q6B6sv5uFF4cJy5VCh8SFSqo9nKVha9gqeCFKrMCHRRWzRzNc",
				"mcp3tQcdUztFjAjggZjgZJ97PH3Z7p4M8tEFhE2NKubLFV1Nk87nv",
				"mcp3vD3tncqz4RMoUcoqD5z6DwAobjX9AKAabcpYtGnCWPAnzDfyX",
				"mcp3Yk4HcWeg9HnVChvEzfFQ4sjx5YzkPxHif4wtfUgwQo3AWTXfH",
				"mcp3zpaqYbySCA1nKadrrRhU2W2d4eJzCRx6DhvgjybRBYHJP1yvT",
				"mcp4eTWbCGM3Dsrr5LehSNEREZkodTFUnuTyC3a25SKa6vq4giRMp",
				"mcp4fAnePQiimjUusbXU11Qx6VQosYWLGRto6NzDTUnyfaXfDCYiA",
				"mcp4iEexeAyLWmURo8Hy4dSj8jaC5igFxErpyA8VpKy2qKGHQAZQ4",
				"mcp4n3nPwym2VfAkJoKrLQGj1WnsUEi7oUkCR8tEsYLssVQemY4QS",
				"mcp46EaBCqezG5o41zDoXTjEtq1UhCqzsx6gYLzdmtoBtfB7Zc3Sc",
				"mcp48kUDucgzpUszYXF45n6SLV5D8oGbTRxmvdjTJ9Bw18gjKMQJi"
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
				"mcp3apDZkSoF6z3xz5ZtWBYH2k89gGB8SaganyWP2V13ro7rB86Tu",
				"mcp3HXbkSR2BmHgi83f8YQVxQHTGLxHTGaYatdDQ7DouxPgFJ9zTA",
				"mcp3kAn315vAn41vU9KzykzVwSik8oHWkfXixEJokiTpiQramWajo",
				"mcp3n61711SRRCZpLHLH3qn1xLCkx7sZpR67kDL2fp3BadGU54RLn",
				"mcp4aziSEw4MvpPQh46iYbBSwksHGwezXDRLBTH5hxFhrewgTYdaM",
				"mcp4cLfkPfkiYRC67QmqpDaRqGcqQFSa65kxToV5fQqX5ZyWkEUei",
				"mcp4peN6FqSL2kCNnGnPpJt3DzmTVT1j798i3zVfgMtJQ2cNHPWkA",
				"mcp4THWHNGvFezLZGkWZ76ZWgeaw1mZTWXqt8nRtV3iEdNzGRecgt",
				"mcp4WNsbSp9k2WkBcYfWgDdiH5F9Sgdh6HfirfxwXUM6a6yGn2uha",
				"mcp4XbkA9m25jsW1JuSb2Z6zqL1n1RnrzVhx3KUvFqXxuatEM52qk",
				"mcp33qM3kGTmTabC9rgjSg6Ai39msuxdgppJ1m453zMvK2at1JCs9",
				"mcp49AdeQG1FN4gteyPhQCueGctZdDihS8nLcNUddyLDAXA5EC7mv",
				"mcp314J3dULRWfyMS853GvhDcLcdB379Q87w9yG9ZYj7WPtTTxKrE",
				"mcp353iC6Z1pjnAPY7VkSeXgaB5EcEcVD2vGPsiLvoGGiZoNerwxS"
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
				"mcp_3BtHMM4eq77KHBcELK8HLpttpotpQ5HAVHFnZ7wPM8nJtjJPwF",
				"mcp_3CaNZSDM1fcvepTdk9qa2WiVJRXE4PqUJDbtTsAh7j5pSABv8Z",
				"mcp_3CsaNBrc7dniUYbBYaiv5zqFdyShoaGC7y7dA2w63wdkJQoALC",
				"mcp_3eS7sHXD94LmCemy9yC2w5JQd95PyWod68irjxfWJKGqmxNoNu",
				"mcp_3hzJ8c8t5nwbFwfCS1sgZcfD2uTHcUqZwSpD6jtuDW8sH8KPwA",
				"mcp_3kHxSg4ksGoUMJAStwHAFoeVR9FbV3XPKHNn6K5VzMeuJ3nbGC",
				"mcp_3mBqrJmexoXcgzvg1XnjyLb4e2Yn37sgTkU8J1xsBSxuQ681fj",
				"mcp_3Q9Mfa4FvksCQKdnLEdDnNpk2jdMwxCRbU6xaPWcuAwHQKTmmT",
				"mcp_3QVjBEedsMa7ZznPc5j6RikwCEDPiBQeeaSxZDegKjpY3jFeMg",
				"mcp_3Uh9XTs5oewuLP1Z5GT7uShHbayv4CVA81qUddyS5TuTAZ3g5M",
				"mcp_3uXQTK2TSe14YMcJKpng3x6EfZqft9yoccSX6hfYEyCMZWSJaS",
				"mcp_4QcrnPUoWTMydJsdTeYstUTWfzH4rvs89pb8MP3dtD7Jd9oqWY",
				"mcp_4QzjDVm2pqwhwVanNwPaRPmfpKFsBgLcGLn6gynkdMLL5ARZ73",
				"mcp_496PALNxdsG6maAnwofNevVJs4kzmoeg3ATQY4KorNbxSis4TP"
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

	static std::set<mcp::account> to_witness_list(std::vector<std::string> const & witness_strs)
	{
		std::set<mcp::account> witness_list;
		for (std::string w_str : witness_strs)
		{
			mcp::account w_acc;
			w_acc.decode_account(w_str);
			witness_list.insert(w_acc);
		}
		return witness_list;
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
};

}
