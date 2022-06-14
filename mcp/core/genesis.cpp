#include "genesis.hpp"
#include "config.hpp"
#include <mcp/common/log.hpp>
#include <libdevcore/CommonJS.h>
#include <libdevcore/TrieHash.h>

std::string const mini_test_genesis_data = R"%%%({
    "from":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "to":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "value":"2000000000000000000000000000",
    "data":"",
	"exec_timestamp":"1514764800"
})%%%";

std::string const test_genesis_data = R"%%%({
    "from":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "to":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "value":"2000000000000000000000000000",
    "data":"",
	"exec_timestamp":"1514764800"
})%%%";

std::string const beta_genesis_data = R"%%%({
    "from":"0x2953233ace71ff6419fbed672f4e92a8aaf1f98b",
    "to":"0x2953233ace71ff6419fbed672f4e92a8aaf1f98b",
    "value":"2000000000000000000000000000",
    "data":"",
	"exec_timestamp":"1514764800"
})%%%";

std::string const live_genesis_data = R"%%%({
    "from":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "to":"0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    "value":"2000000000000000000000000000",
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

	TransactionSkeleton _t;
	_t.from = jsToFixed<20>(json["from"]);
	_t.to = jsToFixed<20>(json["to"]);
	_t.value = jsToU256(json["value"]);
	_t.nonce = 0;
	Transaction ts(_t);
	
	std::string from_account = json["from"];
	dev::Address from(from_account);
	//if (from.decode_account(json["from"]))
	//	throw std::runtime_error("deserialize genesis block from error");
	std::unique_ptr<mcp::block> block(std::make_unique<mcp::block>());
	block->init_from_genesis_transaction(from, ts.sha3(IncludeSignature::WithoutSignature), json["exec_timestamp"]);

	block_hash = block->hash();
	mcp::block_hash genesis_hash;
	bool exists(!store_a.genesis_hash_get(transaction_a, genesis_hash));
	if (exists)
	{
		std::cout << block_hash .hex() << ":" << genesis_hash.hex() << std::endl;
		if(genesis_hash != block_hash)
			throw std::runtime_error("genesis block changed");

		return false;
	}
	
	store_a.genesis_hash_put(transaction_a, block_hash);
	store_a.block_put(transaction_a, block_hash, *block);

	//block state
	mcp::block_state block_state;

	block_state.is_free = true;
	block_state.level = 0;
	block_state.is_stable = true;
    block_state.status = mcp::block_status::ok;
	block_state.main_chain_index = 0;
	block_state.mc_timestamp = block->exec_timestamp();
	block_state.stable_timestamp = block->exec_timestamp();

	block_state.is_on_main_chain = true;
	block_state.witnessed_level = block_state.level;
	block_state.best_parent.clear();
	block_state.earliest_included_mc_index = boost::none;
	block_state.latest_included_mc_index = boost::none;
	block_state.bp_included_mc_index = boost::none;
	block_state.earliest_bp_included_mc_index = boost::none;
	block_state.latest_bp_included_mc_index = boost::none;
	store_a.block_state_put(transaction_a, block_hash, block_state);

	//mci
	store_a.main_chain_put(transaction_a, *block_state.main_chain_index, block_hash);

	//stable index
	store_a.stable_block_put(transaction_a, 0, block_hash);
	store_a.last_stable_index_put(transaction_a, 0);

	//free
	store_a.dag_free_put(transaction_a, mcp::free_key(block_state.witnessed_level, block_state.level, block_hash));

	//genesis account
	dev::Address const & genesis_account(block->from());

	//add dag account info
	mcp::dag_account_info info;
	store_a.dag_account_get(transaction_a, genesis_account, info);
	info.latest_stable_block = block_hash;
	store_a.dag_account_put(transaction_a, genesis_account, info);

	//genesis account state
    // sichaoy: nonce of genesis account?
	mcp::account_state to_state(ts.sender(), ts.sha3(IncludeSignature::WithoutSignature), h256(0), 0, ts.value());
	to_state.incNonce();//nonce + 1 Stored for the next nonce
	store_a.account_state_put(transaction_a, to_state.hash(), to_state);
	store_a.latest_account_state_put(transaction_a, ts.to(), to_state.hash());
	store_a.account_nonce_put(transaction_a, ts.sender(), _t.nonce);

	dev::eth::TransactionReceipt const receipt = dev::eth::TransactionReceipt(true, 0, mcp::log_entries());
	std::vector<bytes> receipts;
	RLPStream receiptRLP;
	receipt.streamRLP(receiptRLP);
	receipts.push_back(receiptRLP.out());
	h256 receiptsRoot = dev::orderedTrieRoot(receipts);
	//summary hash
	mcp::summary_hash previous_summary_hash(0);
	std::list<mcp::summary_hash> p_summary_hashs; //no parents
	std::list<mcp::summary_hash> l_summary_hashs; //no links
	std::set<mcp::summary_hash> summary_skiplist; //no skiplist
	mcp::summary_hash summary_hash = mcp::summary::gen_summary_hash(block_hash, previous_summary_hash, p_summary_hashs, receiptsRoot, summary_skiplist,
		block_state.status, block_state.stable_index, block_state.mc_timestamp);

    mcp::log log_node("node");
	LOG(log_node.info) << "Genesis Summary:" << summary_hash.hex();

	store_a.block_summary_put(transaction_a, block_hash, summary_hash);
	store_a.summary_block_put(transaction_a, summary_hash, block_hash);

	return true;
}

mcp::block_hash mcp::genesis::block_hash(0);

