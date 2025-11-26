#include "genesis.hpp"
#include "config.hpp"
#include "transaction_receipt.hpp"
#include "contract.hpp"
#include <mcp/common/log.hpp>
#include <libdevcore/CommonJS.h>
#include <libdevcore/TrieHash.h>

std::unordered_map<mcp::block_hash, dev::Address> GenesisTransactions;

h160s GenesisDatas
{
	Address("0x8e0f9ae2eb92f2f7e03d3706d6b899931c6b81e4"),
	Address("0x0eb3f40062fc2883cc12ce131a4dd5faee181cdb"),
	Address("0x1144B522F45265C2DFDBAEE8E324719E63A1694C"),
	Address("0x1144B522F45265C2DFDBAEE8E324719E63A1694C")
};

std::pair<mcp::block, mcp::Transactions> mcp::genesis::try_initialize()
{
	GenesisAddress = GenesisDatas[(int)mcp::mcp_network - 1];
	TransactionSkeleton _t{ 
		GenesisAddress,
		GenesisAddress,
		jsToU256("2000000000000000000000000000"),
		bytes(),
		0,
		mcp::tx_max_gas,
		mcp::gas_price
	};
	Transaction ts(_t);
	ts.setSignature(h256(0), h256(0), 0);

	/// 0: genesis transaction.
	/// 1: transfer to the account that deployed system contract.
	/// 2: Admin contract transaction.
	/// 3: Deposit contract transaction.
	/// 4: Proxy contract transaction.
	/// 5: staking init witness transaction.
	h256s initHashes;
	initHashes.push_back(ts.sha3());
	GenesisTransactions.insert(std::pair(ts.sha3(), ts.sender()));
	/// genesis block linked initialized transaction 
	Transactions _tstaking = InitMainContractTransaction();
	for (Transaction _t : _tstaking)
	{
		initHashes.push_back(_t.sha3());
		GenesisTransactions.insert(std::pair(_t.sha3(), _t.sender()));
	}
	_tstaking.insert(_tstaking.begin(), ts);

	std::unique_ptr<mcp::block> block(std::make_unique<mcp::block>());
	block->init_from_genesis_transaction(ts.sender(), initHashes, "1700625600"/*json["exec_timestamp"]*/);
	block_hash = block->hash();
	return std::make_pair(*block, _tstaking);
}

std::pair<bool, dev::Address> mcp::genesis::isGenesisTransaction(mcp::block_hash const& _h)
{
	if (GenesisTransactions.count(_h))
		return std::make_pair(true, GenesisTransactions[_h]);
	return std::make_pair(false, dev::ZeroAddress);
}

mcp::block_hash mcp::genesis::block_hash(0);
dev::Address mcp::genesis::GenesisAddress(dev::ZeroAddress);

