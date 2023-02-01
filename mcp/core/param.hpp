#pragma once

#include "config.hpp"
#include "block_cache.hpp"

namespace mcp
{
class param
{
public:
	static void init(std::shared_ptr<mcp::block_cache> cache_a)
	{
		init_block_param();
		init_witness_param();
		cache = cache_a;
	}

	static mcp::block_param const & block_param(uint64_t const & last_epoch_a)
	{
		mcp::block_param const & b_param
			= find_by_last_epoch<mcp::block_param>(last_epoch_a, block_param_map);
		return b_param;
	}

	static mcp::witness_param const & witness_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a)
	{
		DEV_READ_GUARDED(m_mutex_witness){
			mcp::witness_param const & w_param
				= find_param(transaction_a, epoch_a);;
			return w_param;
		}
	}

	static bool is_witness(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a, dev::Address const & account_a)
	{
		DEV_READ_GUARDED(m_mutex_witness){
			mcp::witness_param const & w_param = find_param(transaction_a,epoch_a);
			if (w_param.witness_list.count(account_a))
				return true;
			return false;
		}
	}

	static WitnessList to_witness_list(std::vector<std::string> const & witness_strs)
	{
		WitnessList witness_list;
		for (std::string w_str : witness_strs)
		{
			dev::Address w_acc(w_str);
			witness_list.insert(w_acc);
		}
		return witness_list;
	}

	static void add_witness_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a, mcp::witness_param &w_param){
		DEV_WRITE_GUARDED(m_mutex_witness){
			cache->epoch_param_put(transaction_a, epoch_a, std::make_shared<mcp::witness_param>(w_param));
			mcp::param::witness_param_map.insert({epoch_a, w_param });
			if (mcp::param::witness_param_map.size() > 3)
			{
				mcp::param::witness_param_map.erase(mcp::param::witness_param_map.begin());
			}
		}
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
			chain_id = (uint64_t)9700;
			gas_price = 10000000;
			break;
		}
		case mcp::mcp_networks::mcp_test_network:
		{
			chain_id = (uint64_t)9700;
			gas_price = 10000000;
			break;
		}
		case mcp::mcp_networks::mcp_beta_network:
		{
			chain_id = (uint64_t)972;// Ascraeus 972; huygens 971
			gas_price = 10000000;
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
			chain_id = (uint64_t)970;
			gas_price = (uint256_t)5e13;
			break;
		}
		default:
			assert_x_msg(false, "Invalid network");
		}
	}

	static void init_witness_param()
	{
		std::vector<std::string> witness_str_list_v0;
		switch (mcp::mcp_network)
		{
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			witness_str_list_v0 = {
				"0x1144B522F45265C2DFDBAEE8E324719E63A1694C"
			};
			break;
		}
		case mcp::mcp_networks::mcp_test_network:
		{
			witness_str_list_v0 = {
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
			break;
		}
		case mcp::mcp_networks::mcp_beta_network:
		{
			/*witness_str_list_v0 = { //for huygens
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
			};*/
			witness_str_list_v0 = { //for ascraeus
				"0x0cefadfedc6b2d21b1a3c5b58b0ccc1d3cdff6f2",
				"0x0ede6b6ca19f6bb7ce5be21546beae64c6762b6b",
				"0x111a6899a9d63d4295e6de66f791acdaca6d07c6",
				"0x234a808020b60abd2e85b68a57b19bc6aa7ac217",
				"0x27821d50355795d2ce792553201a36afc232c4c1",
				"0x2e2cb4884db9f2976a6b23e0544ea4d2d6f13c45",
				"0x33d640ed625551c4ab29f81e2481f937c6cf24ee",
				"0x3b9cf59b26286c0faf3f714ac17cc59284dae5fb",
				"0x3ca4ded5891e7482f7ae34ff44ba86679bc3584d",
				"0x4089240950b8a3118ddd647d14d53e77a61ff618",
				"0x422ceefcce450aa293f81777c3fa4972349778ab",
				"0x442f16643aeb9d466add91a464d9aa6acd63625d",
				"0x49eb9d07b82dbdc6efd3ca14b71336a6a56d2962",
				"0x4a625c5ddceb732a9e73d30e98f52e87bf53d8ee"
			};
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
			witness_str_list_v0 = {
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
			break;
		}
		default:
			assert_x_msg(false, "Invalid network");
		}

		init_param.witness_count = witness_str_list_v0.size();
		init_param.majority_of_witnesses = init_param.witness_count * 2 / 3 + 1;
		init_param.witness_list = to_witness_list(witness_str_list_v0);
		assert_x(init_param.witness_list.size() == init_param.witness_count);
	}

	template<class T>
	static T const & find_by_last_epoch(Epoch const & epoch_a, std::map<uint64_t, T> const & maps_a)
	{
		for (auto it(maps_a.rbegin()); it != maps_a.rend(); it++)
		{
			uint64_t const & min_last_epoch(it->first);
			if (epoch_a >= min_last_epoch)
			{
				T const & result(it->second);
				return result;
			}
		}
		assert_x(false);
	}

	static mcp::witness_param const & find_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a)
	{
		if (epoch_a <= 1)
			return init_param;
		auto it = witness_param_map.find(epoch_a);
		if (it != witness_param_map.end())
			return it->second;
		auto _p = cache->epoch_param_get(transaction_a, epoch_a);
		if (_p)
			return *_p;
		return mcp::witness_param();
	}

	//epoch -> block param
	static std::map<uint64_t, mcp::block_param> block_param_map;

	//epoch -> witness param
	static std::map<uint64_t, mcp::witness_param> witness_param_map;
	static dev::SharedMutex m_mutex_witness;
	static mcp::witness_param init_param;

	static std::shared_ptr<mcp::block_cache> cache;
};

}
