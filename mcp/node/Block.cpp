#include "Block.hpp"
#include "chain.hpp"
#include <libdevcore/CommonJS.h>
#include <mcp/core/genesis.hpp>
#include <mcp/common/Exceptions.h>

using namespace mcp;
using namespace dev::eth;

mcp::Block::Block(/*mcp::block_store& store_a, */chain const& _bc, OverlayDB const& _db, BaseState _bs):
	//m_store(store_a),
	m_state(Invalid256, _db, _bs)
{
	m_sealEngine = _bc.sealEngine();
}

mcp::Block::Block(Block const& _s):
	//m_store(_s.m_store),
	m_state(_s.m_state),
	m_transactions(_s.m_transactions),
	m_receipts(_s.m_receipts),
	//m_transactionSet(_s.m_transactionSet),
	//m_precommit(_s.m_state),
	m_previousBlockState(_s.m_previousBlockState),
	m_currentBlockState(_s.m_currentBlockState),
	m_currentBlock(_s.m_currentBlock),
	//m_currentBytes(_s.m_currentBytes),
	//m_author(_s.m_author),
	m_sealEngine(_s.m_sealEngine)
{
}

mcp::Block& mcp::Block::operator=(Block const& _s)
{
	if (&_s == this)
		return *this;

	//m_store = _s.m_store;
	m_state = _s.m_state;
	m_transactions = _s.m_transactions;
	m_receipts = _s.m_receipts;
	//m_transactionSet = _s.m_transactionSet;
	m_previousBlockState = _s.m_previousBlockState;
	m_currentBlockState = _s.m_currentBlockState;
	m_currentBlock = _s.m_currentBlock;
	//m_currentBytes = _s.m_currentBytes;
	//m_author = _s.m_author;
	m_sealEngine = _s.m_sealEngine;

	//m_precommit = m_state;
	//m_committedToSeal = false;
	return *this;
}

mcp::Block::Block(/*mcp::block_store& store_a, */chain const& _bc, OverlayDB const& _db, h256 const& _root) :
	//m_store(store_a),
	m_state(Invalid256, _db, BaseState::PreExisting)
{
	m_sealEngine = _bc.sealEngine();
	m_state.setRoot(_root);
}

void mcp::Block::populateFromChain(std::shared_ptr<mcp::block_state> _cstate, std::shared_ptr<mcp::block> _cblock, std::shared_ptr<mcp::block_state> _pstate, Transactions& _txs)
{
	m_currentBlockState = _cstate;
	m_currentBlock = _cblock;
	m_previousBlockState = _pstate;
	m_transactions = std::move(_txs);

	m_state.setRoot(m_currentBlockState->m_stateRoot);
}

mcp::ExecutionResult mcp::Block::execute(/*mcp::db::db_transaction& transaction_a, *//*chain const& _bc,*/ /*std::shared_ptr<mcp::iblock_cache> cache_a,*/ Transaction const& _t, dev::eth::McInfo const& mc_info_a, Permanence _p/*, dev::eth::OnOpFunc const& _onOp*/)
{
	try
	{
		dev::eth::EnvInfo env(/*transaction_a, m_store, cache_a,*/ mc_info_a, mcp::chainID());
		//auto chain_ptr(std::make_shared<chain>(_bc));

		//dev::OverlayDB _db = dev::OverlayDB(std::make_unique<mcp::block_store>(m_store));
		//chain_state c_state(/*transaction_a,*/ 0,/* m_store,*/ chain_ptr,/* cache_a,*/ _db);
		std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> resultReceipt =
			m_state.execute(env, *m_sealEngine, _p, _t/*, _onOp*/);

		if (_p == Permanence::Committed)
		{
			// Add to the user-originated transactions that we've executed.
			//m_transactions.push_back(_t);
			m_receipts.push_back(resultReceipt.second);
			//m_transactionSet.insert(_t.sha3());
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
		//m_transactions.push_back(_t);
		m_receipts.push_back(receipt);
		//m_transactionSet.insert(_t.sha3());
	}

	return mcp::ExecutionResult();
}

u256 mcp::Block::enactOn(dev::eth::McInfo const& _mc, VerifiedBlockRef const& _block, chain const& _bc)
{
    //resetCurrent();
	//mcp::db::db_transaction& transaction_a(_param.timeout_tx.get_transaction());
	std::vector<bytes> receipts;
	{
		unsigned index = 0;
		for (auto const& it : _block.transactions)
		{
			if (nullptr == it)///processed
			{
				m_receipts.push_back(TransactionReceiptPlaceholder());
				continue;
			}

			/// exec transactions
			//dev::eth::McInfo mc_info(_param.blockNum, _param.mci, _param.mc_timestamp, _param.mc_last_summary_mci);
			execute(/*transaction_a, *//*_bc,*/ /*_param.cache,*/ *it, _mc, Permanence::Committed/*, dev::eth::OnOpFunc()*/);
		}
	}

	return 0;

    //return enact(*_block, _bc);
}

dev::eth::McInfo mcp::Block::info() const
{
	return dev::eth::McInfo(m_currentBlockState->stable_index,
		*m_currentBlockState->main_chain_index,
		m_currentBlockState->mc_timestamp,
		*m_currentBlockState->main_chain_index,
		m_currentBlock->from()
	);
}

u256 mcp::Block::enact(mcp::block const& _block, chain const& _bc)
{
    ////noteChain(_bc);
    //DEV_TIMED_ABOVE("txExec", 500)
    //    for (Transaction const& tr : _block.transactions)
    //    {
    //        try
    //        {
    //            //				cnote << "Enacting transaction: " << tr.nonce() << tr.from() << state().transactionsFrom(tr.from()) << tr.value();
    //            execute(_bc.lastBlockHashes(), tr);
    //            //				cnote << "Now: " << tr.from() << state().transactionsFrom(tr.from());
    //            //				cnote << m_state;
    //        }
    //        catch (Exception& ex)
    //        {
    //            ex << errinfo_transactionIndex(i);
    //            throw;
    //        }

    //        RLPStream receiptRLP;
    //        m_receipts.back().streamRLP(receiptRLP);
    //        receipts.push_back(receiptRLP.out());
    //        ++i;

    //        h256 receiptsRoot;
    //        DEV_TIMED_ABOVE(".receiptsRoot()", 500)
    //            receiptsRoot = orderedTrieRoot(receipts);

    //        //// Initialise total difficulty calculation.
    //        //u256 tdIncrease = m_currentBlock.difficulty();
    //    }

    //DEV_TIMED_ABOVE("applyRewards", 500)
    //    applyRewards(rewarded, _bc.sealEngine()->blockReward(m_currentBlock.number()));

    //DEV_TIMED_ABOVE("commit", 500)
    //    m_state.commit(removeEmptyAccounts ? State::CommitBehaviour::RemoveEmptyAccounts : State::CommitBehaviour::KeepEmptyAccounts);

    //return tdIncrease;
    return 0;
}

void mcp::Block::cleanup()
{
	// Commit the new trie to disk.
	//LOG(m_log.debug) << "Committing to disk: stateRoot " << m_currentBlock.stateRoot() << " = "
	//	<< rootHash() << " = " << toHex(asBytes(db().lookup(rootHash())));

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

	m_state.db().commit();	// TODO: State API for this?

	//LOG(m_logger) << "Committed: stateRoot " << m_currentBlock.stateRoot() << " = " << rootHash()
	//	<< " = " << toHex(asBytes(db().lookup(rootHash())));

	//m_previousBlock = m_currentBlock;
	//sealEngine()->populateFromParent(m_currentBlock, m_previousBlock);

	//LOG(m_logger) << "finalising enactment. current -> previous, hash is "
	//	<< m_previousBlock.hash();

	//resetCurrent();
	//m_transactions.clear();
	m_receipts.clear();
	//m_transactionSet.clear();
}

//void mcp::Block::resetCurrent()
//{
//	//m_transactions.clear();
//	m_receipts.clear();
//	//m_transactionSet.clear();
//
//	//m_state.setRoot(m_previousBlock.stateRoot());
//}
