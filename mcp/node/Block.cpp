#include "Block.hpp"
#include "chain.hpp"
#include <libdevcore/CommonJS.h>
#include <mcp/core/genesis.hpp>
#include <mcp/core/contract.hpp>
#include <mcp/common/Exceptions.h>

using namespace mcp;
using namespace dev::eth;

mcp::Block::Block(chain const& _bc, OverlayDB const& _db, dev::eth::McInfo const& _mc, BaseState _bs):
	m_McInfo(_mc),
	m_state(_db, _bs)
{
	m_sealEngine = _bc.sealEngine();
}

mcp::Block::Block(Block const& _s):
	m_state(_s.m_state),
	m_transactions(_s.m_transactions),
	m_receipts(_s.m_receipts),
	m_previousBlockState(_s.m_previousBlockState),
	m_McInfo(_s.m_McInfo),
	m_sealEngine(_s.m_sealEngine)
{
}

mcp::Block& mcp::Block::operator=(Block const& _s)
{
	if (&_s == this)
		return *this;

	m_state = _s.m_state;
	m_transactions = _s.m_transactions;
	m_receipts = _s.m_receipts;
	m_previousBlockState = _s.m_previousBlockState;
	m_McInfo = _s.m_McInfo;
	m_sealEngine = _s.m_sealEngine;

	return *this;
}

mcp::Block::Block(chain const& _bc, OverlayDB const& _db, h256 const& _root, dev::eth::McInfo const& _mc) :
	m_McInfo(_mc),
	m_state(_db, BaseState::PreExisting)
{
	m_sealEngine = _bc.sealEngine();
	m_state.setRoot(_root);
}

log_bloom mcp::Block::logBloom() const
{
	log_bloom ret;
	for (TransactionReceipt const& i : m_receipts)
		ret |= i.bloom();
	return ret;
}

void mcp::Block::populateFromChain(std::shared_ptr<mcp::block_state> _cstate, std::shared_ptr<mcp::block> _cblock, std::shared_ptr<mcp::block_state> _pstate, Transactions& _txs)
{
	m_previousBlockState = _pstate;
	m_transactions = std::move(_txs);

	if (_cstate && _cblock)
	{
		m_McInfo = dev::eth::McInfo(_cstate->stable_index,
			*_cstate->main_chain_index,
			_cstate->mc_timestamp,
			_cblock->from()
		);
	}
	
	m_state.setRoot(_cstate->m_stateRoot);
}

mcp::ExecutionResult mcp::Block::execute(Transaction const& _t, Permanence _p)
{
	try
	{
		dev::eth::EnvInfo env(info(), mcp::chainID());

		std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> resultReceipt =
			m_state.execute(env, *m_sealEngine, _p, _t);

		if (_p == Permanence::Committed)
		{
			// Add to the user-originated transactions that we've executed.
			m_receipts.push_back(resultReceipt.second);
		}
		return resultReceipt.first;
	}
	catch (dev::eth::NotEnoughCash const& _e)
	{
		LOG(m_log.info) << "transaction exec not enough cash,hash: " << _t.sha3().hex()
			<< ", from: " << dev::toJS(_t.sender())
			<< ", to: " << dev::toJS(_t.to())
			<< ", value: " << _t.value();
	}
	catch (dev::eth::InvalidNonce const& _e)
	{
		LOG(m_log.info) << "transaction exec not expect nonce,hash: " << _t.sha3().hexPrefixed()
			<< ", from: " << dev::toJS(_t.sender())
			<< ", to: " << dev::toJS(_t.to())
			<< ", value: " << _t.value();
	}
	//catch (Exception const& _e)
	//{
	//	cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
	//		<< diagnostic_information(_e);
	//	exit(1);
	//}
	catch (std::exception const& _e)
	{
		std::cerr << _e.what() << std::endl;
		throw;
	}

	if (_p == Permanence::Committed)
	{
		TransactionReceipt const receipt = TransactionReceipt(0, 0, mcp::log_entries());
		m_receipts.push_back(receipt);
	}

	return mcp::ExecutionResult();
}

u256 mcp::Block::enactOn(VerifiedBlockRef const& _block, chain const& _bc)
{
	{
		unsigned index = 0;
		for (auto const& it : _block.transactions)
		{
			/// exec transactions
			execute(*it, Permanence::Committed);
		}
	}

	return 0;
}

dev::eth::McInfo mcp::Block::info(bool isPopulateFromParent) const
{
	if (isPopulateFromParent)
	{
		return dev::eth::McInfo(
			m_McInfo.block_number + 1, m_McInfo.mci + 1,
			mcp::seconds_since_epoch(), m_McInfo.author
		);
	}

	return m_McInfo;
}

std::pair<Transaction, dev::eth::TransactionReceipt> mcp::Block::ApplyWorkTransaction(dev::bytes const& _data)
{
	Transaction _t = systemTransaction(_data);
	try
	{
		dev::eth::EnvInfo env(info(), mcp::chainID());
		std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> _result =
			m_state.execute(env, *m_sealEngine, Permanence::Committed, _t);

		assert_x(_result.second.statusCode());//for test
		return std::make_pair(_t, _result.second);
	}
	catch (std::exception const& _e)
	{
		assert_x_msg(false, "earned rewards error.")
	}
}

mcp::StakingList mcp::Block::getStakingList()
{
	mcp::StakingList _sl;
	/// contract logic. Get up to 500 records each time. But may return less than 500.
	auto _handler = [this](int _index)
	{
		auto _tx = systemTransaction(MainCaller.PackGetWitnesses(0));
		auto _ex = execute(_tx, Permanence::Reverted);
		assert_x(!_ex.Failed());
		return MainCaller.UnpackGetWitnesses(_ex.output);
	};

	static const int batchSize = 500;
	auto _all = _handler(0);
	_sl.insert(_all.first.begin(), _all.first.end());
	int total = batchSize;
	while (total < _all.second)
	{
		auto _tmp = _handler(total);
		total += batchSize;
		_sl.insert(_tmp.first.begin(), _tmp.first.end());
	}
	return _sl;
}

mcp::MainInfo mcp::Block::getMainInfo()
{
	auto _tx = systemTransaction(MainCaller.PackGetMainInfo());
	auto _ex = execute(_tx, Permanence::Reverted);
	assert_x(!_ex.Failed());
	return MainCaller.UnpackGetMainInfo(_ex.output);
}

void mcp::Block::cleanup()
{
	try
	{
		EnforceRefs er(db(), true);
		rootHash();
	}
	catch (BadRoot const&)
	{
		cwarn << "Trie corrupt! :-(";
		throw;
	}

	m_state.db().commit();
	m_transactions.clear();
	m_receipts.clear();
}

mcp::Transaction mcp::Block::systemTransaction(dev::bytes const& _data)
{
	u256 nonce = transactionsFrom(MainCallcAddress);
	u256 gas = mcp::tx_max_gas;
	u256 gasPrice = mcp::gas_price;
	Transaction _t(0, gasPrice, gas, MainContractAddress, _data, nonce);
	_t.forceSender(MainCallcAddress);
	_t.setSignature(h256(0), h256(0), 0);
	return _t;
}
