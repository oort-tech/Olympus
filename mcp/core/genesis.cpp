#include "genesis.hpp"
#include <mcp/common/log.hpp>

std::string const mini_test_genesis_data = R"%%%({
    "from":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "to":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "amount":"1132623791600000000000000000",
    "data":"",
	"exec_timestamp":"1514764800"
})%%%";

std::string const test_genesis_data = R"%%%({
    "from":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "to":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "amount":"1132623791600000000000000000",
    "data":"",
	"exec_timestamp":"1514764800"
})%%%";

std::string const beta_genesis_data = R"%%%({
    "from":"0x6e5d834741ce6f0df3c106b7b602535f2a11b970",
    "to":"0x6e5d834741ce6f0df3c106b7b602535f2a11b970",
    "amount":"2000000000000000000000000000",
    "data":"",
	"exec_timestamp":"1514764800"
})%%%";

std::string const live_genesis_data = R"%%%({
    "from":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "to":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "amount":"1132623791600000000000000000",
    "data":"",
	"exec_timestamp":"1562288400"
})%%%";

bool mcp::genesis::try_initialize(mcp::db::db_transaction & transaction_a, mcp::block_store & store_a)
{
	std::string genesis_data;
	switch (mcp::mcp_network)
	{
	case mcp::mcp_networks::mcp_mini_test_network:
		genesis_data  = mini_test_genesis_data;
		break;
	case mcp::mcp_networks::mcp_test_network:
		genesis_data = test_genesis_data;
		break;
	case mcp::mcp_networks::mcp_beta_network:
		genesis_data = beta_genesis_data;
		break;
	case mcp::mcp_networks::mcp_live_network:
		genesis_data = live_genesis_data;
		break;
	}

	mcp::json json = mcp::json::parse(genesis_data);
	bool error(false);
	std::unique_ptr<mcp::block> block(std::make_unique<mcp::block>());
	block->init_from_genesis_json(error, json);
	if (error)
		throw std::runtime_error("deserialize genesis block error");
	block_hash = block->hash();
	mcp::block_hash genesis_hash;
	bool exists(!store_a.genesis_hash_get(transaction_a, genesis_hash));

	if (exists)
	{
		if(genesis_hash != block_hash)
			throw std::runtime_error("genesis block changed");

		return false;
	}

	store_a.genesis_hash_put(transaction_a, block_hash);
	store_a.block_put(transaction_a, block_hash, *block);

	//block state
	mcp::block_state block_state;

	block_state.block_type = mcp::block_type::genesis;
	block_state.is_free = true;
	block_state.level = 0;
	block_state.is_stable = true;
    block_state.status = mcp::block_status::ok;
	block_state.main_chain_index = 0;
	block_state.mc_timestamp = block->hashables->exec_timestamp;
	block_state.stable_timestamp = block->hashables->exec_timestamp;

	block_state.is_on_main_chain = true;
	block_state.witnessed_level = block_state.level;
	block_state.best_parent = 0;
	block_state.earliest_included_mc_index = boost::none;
	block_state.latest_included_mc_index = boost::none;
	block_state.bp_included_mc_index = boost::none;
	block_state.earliest_bp_included_mc_index = boost::none;
	block_state.latest_bp_included_mc_index = boost::none;

	//mci
	store_a.main_chain_put(transaction_a, *block_state.main_chain_index, block_hash);

	//stable index
	store_a.stable_block_put(transaction_a, 0, block_hash);
	store_a.last_stable_index_put(transaction_a, 0);

	//free
	store_a.dag_free_put(transaction_a, mcp::free_key(block_state.witnessed_level, block_state.level, block_hash));

	//genesis account
	mcp::account const & genesis_account(block->hashables->from);

	//add dag account info
	mcp::dag_account_info info;
	store_a.dag_account_get(transaction_a, genesis_account, info);
	info.latest_stable_block = block_hash;
	store_a.dag_account_put(transaction_a, genesis_account, info);

	//genesis account state
    // sichaoy: nonce of genesis account?
	std::shared_ptr<mcp::account_state> to_state(std::make_shared<mcp::account_state>(genesis_account, block_hash, 0, 0, block->hashables->amount));
	store_a.account_state_put(transaction_a, to_state->hash(), *to_state);
	store_a.latest_account_state_put(transaction_a, block->hashables->to, to_state->hash());

	std::set<mcp::account_state_hash> to_states;
	to_states.insert(to_state->hash());
	block_state.receipt = mcp::transaction_receipt(0, to_states, 0, {});
	store_a.block_state_put(transaction_a, block_hash, block_state);

	//summary hash
	mcp::summary_hash previous_summary_hash(0);
	std::list<mcp::summary_hash> p_summary_hashs; //no parents
	std::list<mcp::summary_hash> l_summary_hashs; //no links
	std::set<mcp::summary_hash> summary_skiplist; //no skiplist
	mcp::summary_hash summary_hash = mcp::summary::gen_summary_hash(block_hash, previous_summary_hash, p_summary_hashs, l_summary_hashs, summary_skiplist,
		block_state.status, block_state.stable_index, block_state.mc_timestamp, block_state.receipt);

    mcp::log log_node("node");
	LOG(log_node.info) << "Genesis Summary:" << summary_hash.to_string();

	store_a.block_summary_put(transaction_a, block_hash, summary_hash);
	store_a.summary_block_put(transaction_a, summary_hash, block_hash);

	return true;
}

mcp::block_hash mcp::genesis::block_hash(0);

