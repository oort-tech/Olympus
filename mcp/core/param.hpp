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
		return get()->m_block_param;
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
			return w_param.witness_list.count(account_a);
		}
	}

	static void add_witness_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a, mcp::witness_param &w_param){
		DEV_WRITE_GUARDED(get()->m_mutex_witness){
			get()->cache->epoch_param_put(transaction_a, epoch_a, std::make_shared<mcp::witness_param>(w_param));
		}
	}

	static mcp::witness_param const & genesis_witness_param()
	{
		return get()->genesisParam;
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

	void init_block_param()
	{
		m_block_param.max_parent_size = 16;
		m_block_param.max_link_size = 4096;
		gas_price = (uint256_t)1e11;

		switch (mcp::mcp_network)
		{
		case mcp::mcp_networks::mcp_mini_test_network:
		{
			chain_id = (uint64_t)9900;
			HalleyForkBlock = 100000;
			OIP6Block = 200000;////todo
			break;
		}
		case mcp::mcp_networks::mcp_test_network:
		{
			chain_id = (uint64_t)9800;
			HalleyForkBlock = 100000;
			OIP6Block = 200000;////todo
			break;
		}
		case mcp::mcp_networks::mcp_beta_network:
		{
			chain_id = (uint64_t)9700;// Ascraeus 972; huygens 971; dev 9700
			OIP4And5Block = 110000;
			HalleyForkBlock = 320000;
			OIP6Block = 0;////todo
			break;
		}
		case mcp::mcp_networks::mcp_live_network:
		{
			chain_id = (uint64_t)970;
			OIP4And5Block = 6000000;
			HalleyForkBlock = 24700000;
			OIP6Block = 0;////todo
			break;
		}
		default:
			assert_x_msg(false, "Invalid network");
		}
	}

	void init_witness_param()
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

		genesisParam.witness_count = witness_str_list_v0.size();
		genesisParam.majority_of_witnesses = genesisParam.witness_count * 2 / 3 + 1;
		for (std::string w_str : witness_str_list_v0)
			genesisParam.witness_list.insert(dev::Address(w_str));
		assert_x(genesisParam.witness_list.size() == genesisParam.witness_count);
	}

	mcp::witness_param find_param(mcp::db::db_transaction & transaction_a, Epoch const & epoch_a)
	{
		if (epoch_a <= 1)
			return genesisParam;
		auto _p = cache->epoch_param_get(transaction_a, epoch_a);
		if (_p)
			return *_p;
		return mcp::witness_param();
	}

	mcp::block_param m_block_param;
	dev::SharedMutex m_mutex_witness;
	mcp::witness_param genesisParam;

	static param* s_this;
	std::shared_ptr<mcp::block_cache> cache;
};

}
