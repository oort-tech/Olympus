#pragma once

#include "config.hpp"
#include "block_cache.hpp"
#include "ChainOperationParams.hpp"
#include "SealEngine.h"

namespace mcp
{
class param : public ChainOperationParams
{
public:
	static void init(std::shared_ptr<mcp::block_cache> cache_a)
	{
		get()->init_block_param();
		get()->init_witness_param();
		get()->Load();
		get()->cache = cache_a;
	}

	static mcp::block_param const & block_param(uint64_t const & last_epoch_a)
	{
		mcp::block_param const & b_param
			= get()->find_by_last_epoch<mcp::block_param>(last_epoch_a, get()->block_param_map);
		return b_param;
	}

	static mcp::witness_param witness_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a)
	{
		DEV_READ_GUARDED(get()->m_mutex_witness){
			return get()->find_param(transaction_a, epoch_a);;
		}
	}

	static bool is_witness(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a, dev::Address const & account_a)
	{
		DEV_READ_GUARDED(get()->m_mutex_witness){
			mcp::witness_param w_param = get()->find_param(transaction_a,epoch_a);
			if (w_param.witness_list.count(account_a))
				return true;
			return false;
		}
	}

	//static WitnessList to_witness_list(std::vector<std::string> const & witness_strs)
	//{
	//	WitnessList witness_list;
	//	for (std::string w_str : witness_strs)
	//	{
	//		dev::Address w_acc(w_str);
	//		witness_list.insert(w_acc);
	//	}
	//	return witness_list;
	//}

	static void add_witness_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a, mcp::witness_param &w_param){
		DEV_WRITE_GUARDED(get()->m_mutex_witness){
			get()->cache->epoch_param_put(transaction_a, epoch_a, std::make_shared<mcp::witness_param>(w_param));
			get()->witness_param_map.insert({epoch_a, w_param });
			if (get()->witness_param_map.size() > 3)
			{
				get()->witness_param_map.erase(get()->witness_param_map.begin());
			}
		}
	}

	static mcp::witness_param const & genesis_witness_param()
	{
		return get()->init_param;
	}

	static SealEngineFace* createSealEngine()
	{
		auto _seal = new SealEngineFace;
		_seal->setChainParams(*get());
		return _seal;
	}

	static param* get() { if (!s_this) s_this = new param; return s_this; }

private:
	
	void Load()
	{
		precompiled.insert(std::make_pair(dev::Address(1), dev::eth::PrecompiledContract(3000, 0, dev::eth::PrecompiledRegistrar::executor("ecrecover"))));
		precompiled.insert(std::make_pair(dev::Address(2), dev::eth::PrecompiledContract(60, 12, dev::eth::PrecompiledRegistrar::executor("sha256"))));
		precompiled.insert(std::make_pair(dev::Address(3), dev::eth::PrecompiledContract(600, 120, dev::eth::PrecompiledRegistrar::executor("ripemd160"))));
		precompiled.insert(std::make_pair(dev::Address(4), dev::eth::PrecompiledContract(15, 3, dev::eth::PrecompiledRegistrar::executor("identity"))));
		precompiled.insert(std::make_pair(dev::Address(5), dev::eth::PrecompiledContract(dev::eth::PrecompiledRegistrar::pricer("modexp"), dev::eth::PrecompiledRegistrar::executor("modexp"))));
		precompiled.insert(std::make_pair(dev::Address(6), dev::eth::PrecompiledContract(500, 0, dev::eth::PrecompiledRegistrar::executor("alt_bn128_G1_add"))));
		precompiled.insert(std::make_pair(dev::Address(7), dev::eth::PrecompiledContract(40000, 0, dev::eth::PrecompiledRegistrar::executor("alt_bn128_G1_mul"))));
		precompiled.insert(std::make_pair(dev::Address(8), dev::eth::PrecompiledContract(dev::eth::PrecompiledRegistrar::pricer("alt_bn128_pairing_product"), dev::eth::PrecompiledRegistrar::executor("alt_bn128_pairing_product"))));
	}

	/*static*/ void init_block_param()
	{
		mcp::block_param b_param_v0;
		b_param_v0.max_parent_size = 16;
		b_param_v0.max_link_size = 4096;
		block_param_map.insert({ 0, b_param_v0 });

		switch (mcp::mcp_network)
		{
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			chain_id = (uint64_t)9900;
			gas_price = (uint256_t)1e11;
			///*ChainConfig->*/HalleyForkBlock = 100;
			break;
		}
		case mcp::mcp_networks::mcp_test_network:
		{
			chain_id = (uint64_t)9800;
			gas_price = (uint256_t)1e11;
			/*ChainConfig->*/HalleyForkBlock = 10000;
			break;
		}
		case mcp::mcp_networks::mcp_beta_network:
		{
			chain_id = (uint64_t)9700;// Ascraeus 972; huygens 971; dev 9700
			gas_price = (uint256_t)1e11;
			/*ChainConfig->*/OIP4And5Block = 110000;
			/*ChainConfig->*/HalleyForkBlock = 320000;
			OIP6Block = 0;////todo
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
			chain_id = (uint64_t)970;
			gas_price = (uint256_t)1e11;
			/*ChainConfig->*/OIP4And5Block = 6000000;
			/*ChainConfig->*/HalleyForkBlock = 24700000;
			OIP6Block = 0;////todo
			break;
		}
		default:
			assert_x_msg(false, "Invalid network");
		}
	}

	/*static*/ void init_witness_param()
	{
		std::vector<std::string> witness_str_list_v0;
		switch (mcp::mcp_network)
		{
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			witness_str_list_v0 = {
				"0x49a1b41e8ccb704f5c069ef89b08cd33f764e9b3"
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
			};
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
			};*/
			witness_str_list_v0 = { //for dev
				"0x3a6a7279f855753642b70ba212732cd8f07a76a5",
				"0x4b0a1c92d99eb6f14ad8bb1d44e39ed82e93607f",
				"0x6af0346951232ee7ed0b292c6298618785f44aef",
				"0x8b1828360976a9b65013ba5f7d5cccbf100f63cd",
				"0x36ebfd1cb3b6aa9fc956be32d2254b38ae54a602",
				"0x89d2b24c241868c4beeab34066cbdf03292d557e",
				"0x316a3e5402760f54559113f1f1edd41e20a40564",
				"0x330893a10dc64279dd9075dcabf154422639e04e",
				"0x505151dc304a045b0d33bf70f97508d60d75fc3d",
				"0x8799220389860e5a19f3212d23f29bcdadfae467",
				"0xc717a0cd56547c6f5dd11ed63db434ef33cc81d5",
				"0xd64d32432e60e41a2dfb53133f6ab4aaa0cb91b7",
				"0xdc558615973c0b4c5b50af47b07f7afcade10a4a",
				"0xe1abd49612e5d5e4f756b9e86b1b42fe1f4fc5dc"
			};
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
			witness_str_list_v0 = {
				"0xd8a2336adc8fd251a041e9962404054e87b13db6",
				"0xeb9f93e9534006a492880fc6a68c275a61ef1843",
				"0x21cf9aa9a05a146f1bcd10deaea4cc0b82bdbde6",
				"0x26cc6514914f61682b58d5e85fea72d438229717",
				"0x8706f6e6230405c9f6eba6ca2157823cad51b5d3",
				"0x0823a3d416812354834694215826b77fc4437f6c",
				"0x78640ac13a4729c294d203cc59b96bb22c900807",
				"0x1e3d15e7c3b3c053cf22d7c679726989d78bfbbb",
				"0x870de0793e2a3822667d8b8dffd3c1a60df91d22",
				"0xdd3f007d6f0b506d3f50dd60a4dcb6b89163ec10",
				"0xb23d73051b04b190e1349a5c1fd34eb90e001c26",
				"0x9e03fb78e51f4e7e5f5064d0a78bb15040617b3b",
				"0x86fe716b54225937ab912a90d3eaa67467b5b66c",
				"0x87ec316964ce91508a2841ebb1f9de2c202d91d6"
			};
			break;
		}
		default:
			assert_x_msg(false, "Invalid network");
		}

		init_param.witness_count = witness_str_list_v0.size();
		init_param.majority_of_witnesses = init_param.witness_count * 2 / 3 + 1;
		//init_param.witness_list = to_witness_list(witness_str_list_v0);
		for (std::string w_str : witness_str_list_v0)
			init_param.witness_list.insert(dev::Address(w_str));
		assert_x(init_param.witness_list.size() == init_param.witness_count);
	}

	template<class T>
	/*static*/ T const & find_by_last_epoch(Epoch const & epoch_a, std::map<uint64_t, T> const & maps_a)
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

	/*static*/ mcp::witness_param find_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a)
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
	/*static*/ std::map<uint64_t, mcp::block_param> block_param_map;

	//epoch -> witness param
	/*static*/ std::map<uint64_t, mcp::witness_param> witness_param_map;
	/*static*/ dev::SharedMutex m_mutex_witness;
	/*static*/ mcp::witness_param init_param;

	static param* s_this;
	/*static*/ std::shared_ptr<mcp::block_cache> cache;
};

}
