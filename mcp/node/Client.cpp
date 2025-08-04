#include "Client.hpp"
#include <mcp/core/genesis.hpp>
#include <mcp/node/sync.hpp>
#include <mcp/core/param.hpp>

using namespace mcp;

mcp::Client::Client(mcp::block_store& store_a, 
	std::shared_ptr<mcp::chain> chain_a, 
	std::shared_ptr<mcp::block_cache> cache_a, 
	std::shared_ptr<mcp::p2p::host> host_a):
	m_store(store_a),
	m_chain(chain_a),
	m_cache(cache_a),
	m_host(host_a)
{
}

std::pair<u256, ExecutionResult> mcp::Client::estimateGas(Address const& _from, u256 _value, Address _dest, bytes const& _data, int64_t _maxGas, u256 _gasPrice, BlockNumber _blockNumber, GasEstimationCallback const& _callback)
{
	try
	{
		int64_t upperBound = _maxGas;
		if (upperBound == Invalid256 || upperBound > mcp::tx_max_gas)
			upperBound = mcp::tx_max_gas;

		int64_t lowerBound = Transaction::baseGasRequired(!_dest, &_data, dev::eth::EVMSchedule());
		u256 gasPrice = _gasPrice == Invalid256 ? mcp::gas_price : _gasPrice;
		ExecutionResult er;

		/// Pre calculate the gas needed for execution
		if (upperBound < lowerBound)
		{
			er.excepted = TransactionException::OutOfGas;
			return std::make_pair(u256(), er);;
		}

		/// Avoid transactions that are less than the lower gas price limit.
		if (gasPrice < mcp::gas_price)
		{
			er.excepted = TransactionException::OutOfGasPriceIntrinsic;
			return std::make_pair(u256(), er);
		}
		dev::eth::McInfo mc_info;
		if (!getMcInfo(mc_info, _blockNumber))
			BOOST_THROW_EXCEPTION(BlockNotFound());

		dev::eth::EnvInfo env(/*transaction_a, m_store, /*cache_a,*/ mc_info, mcp::chainID());
		auto bk = blockByNumber(_blockNumber);

		auto _T([this, bk/*c_state*/, _from, _value, _dest, _data, gasPrice](int64_t const& gas)
			{
				u256 n = bk.transactionsFrom(_from);
				Transaction t;
				if (_dest)
					t = Transaction(_value, gasPrice, gas, _dest, _data, n);
				else
					t = Transaction(_value, gasPrice, gas, _data, n);
				t.setSignature(h256(0), h256(0), 0);
				t.forceSender(_from);
				return t;
			}
		);

		/// return if used lowerBound successed.
		{
			Transaction t = _T(lowerBound);
			chain_state tempState(bk.state());
			tempState.addBalance(_from, lowerBound * gasPrice + _value);
			er = tempState.execute(env, *bc().sealEngine(), Permanence::Reverted, t/*, dev::eth::OnOpFunc()*/).first;

			if (er.excepted == TransactionException::None)
				return std::make_pair(lowerBound, er);
		}

		/// Reject the transaction as invalid if it still fails at the highest allowance
		{
			Transaction t = _T(upperBound);
			chain_state tempState(bk.state());
			tempState.addBalance(_from, upperBound * gasPrice + _value);
			er = tempState.execute(env, *bc().sealEngine(), Permanence::Reverted, t/*, dev::eth::OnOpFunc()*/).first;

			/// If the error is not nil(consensus error), it means the provided message
			/// call or transaction will never be accepted no matter how much gas it is
			/// assigned. Return the error directly, don't struggle any more.

			if (er.excepted != TransactionException::None)
				return std::make_pair(u256(), er);
		}
		/// Execute the binary search and hone in on an executable gas limit
		while (lowerBound + 1 < upperBound)
		{
			int64_t mid = (lowerBound + upperBound) / 2;
			Transaction t = _T(mid);
			chain_state tempState(bk.state());
			tempState.addBalance(_from, upperBound * gasPrice + _value);
			ExecutionResult result = tempState.execute(env, *bc().sealEngine(), Permanence::Reverted, t/*, dev::eth::OnOpFunc()*/).first;

			if (result.excepted != TransactionException::None
				/*|| result.codeDeposit == CodeDeposit::Failed*/ /// throw exception if failed. not used yet?
				)
			{
				lowerBound = mid;
			}
			else
			{
				upperBound = mid;
				er = result;
			}
			if (_callback)
				_callback(GasEstimationProgress{ lowerBound, upperBound });
		}

		if (_callback)
			_callback(GasEstimationProgress{ lowerBound, upperBound });

		return std::make_pair(upperBound, er);
	}
	catch (std::exception const& e)
	{
		LOG(m_log.error) << "estimate_gas error:" << e.what();
		return std::make_pair(u256(), ExecutionResult());
	}
	catch (...)
	{
		LOG(m_log.error) << "estimate_gas unknown error";
		/// TODO: Some sort of notification of failure.
		return std::make_pair(u256(), ExecutionResult());
	}
}

u256 mcp::Client::balanceAt(Address _a, BlockNumber _block) const
{
	return blockByNumber(_block).balance(_a);
}

u256 mcp::Client::countAt(Address _a, BlockNumber _block) const
{
	return blockByNumber(_block).transactionsFrom(_a);
}

u256 mcp::Client::stateAt(Address _a, u256 _l, BlockNumber _block) const
{
	return blockByNumber(_block).storage(_a, _l);
}

dev::bytes mcp::Client::codeAt(Address _a, BlockNumber _block) const
{
	return blockByNumber(_block).code(_a);
}

localised_log_entries mcp::Client::logs(LogFilter const& _filter) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());

	auto _handler = [this, &transaction, &_filter](std::shared_ptr<mcp::block> _block, std::shared_ptr<mcp::block_state> _state, localised_log_entries& io_logs)
	{
		for (size_t i = 0; i < _block->links().size(); i++)
		{
			dev::h256 th = _block->links().at(i);
			auto td = m_cache->transaction_address_get(transaction, th);
			if (td == nullptr || td->blockHash != _block->hash())///not first linked, ignore.
				continue;

			auto receipt = m_cache->transaction_receipt_get(transaction, th);
			assert_x(receipt);
			log_entries le = _filter.matches(*receipt, *_state->main_chain_index);
			for (unsigned j = 0; j < le.size(); ++j)
				io_logs.push_back(localised_log_entry(le[j], _block->hash(), _state->stable_index, th, i, j));
		}
	};

	mcp::localised_log_entries ret;
	/// Block filter requested
	if (_filter.blockHash())
	{
		auto state = m_cache->block_state_get(transaction, _filter.blockHash());
		if (!state || !state->is_stable)
			BOOST_THROW_EXCEPTION(BlockNotFound());
		auto _block = m_cache->block_get(transaction, _filter.blockHash());
		if (!_block)
			BOOST_THROW_EXCEPTION(BlockNotFound());

		_handler(_block, state, ret);
		//j_response["result"] = toJson(ret);
		return ret;
	}

	mcp::BlockNumber _from = _filter.fromBlock();
	mcp::BlockNumber _to = _filter.toBlock();
	mcp::BlockNumber _lastStable = m_chain->last_stable_index();
	if (_from == LatestBlock || _from == PendingBlock)
		_from = _lastStable;
	if (_to == LatestBlock || _to == PendingBlock)
		_to = _lastStable;

	//if (_filter.toBlock() - _filter.fromBlock() >= 2000)///max 2000
	//	BOOST_THROW_EXCEPTION(RPC_Error_TooLargeSearchRange("Query Returned More Than 2000 Results"));//-32005 query returned more than 10000 results
	for (uint64_t i(_from); i <= _to; i++)
	{
		auto _block = m_cache->block_get(transaction, i);
		if (!_block)
			break;
		if (!_block->links().size())///have no logs
			continue;
		auto state = m_cache->block_state_get(transaction, _block->hash());
		if (!state || !state->is_stable)
			break;

		_handler(_block, state, ret);
	}

	return ret;
}

ExecutionResult mcp::Client::call(Address const& _from, u256 _value, Address _dest, bytes const& _data, u256 _gas, u256 _gasPrice, BlockNumber _blockNumber)
{
	dev::eth::McInfo mc_info;
	if (!getMcInfo(mc_info, _blockNumber))
		BOOST_THROW_EXCEPTION(BlockNotFound());

	Block temp = blockByNumber(_blockNumber);
	u256 nonce = temp.transactionsFrom(_from);
	u256 gas = _gas == Invalid256 ? mcp::tx_max_gas : _gas;
	u256 gasPrice = _gasPrice == Invalid256 ? mcp::gas_price: _gasPrice;
	Transaction _t(_value, gasPrice, gas, _dest, _data, nonce);
	_t.forceSender(_from);
	//_t.setSignature(h256(0), h256(0), 0);
	ExecutionResult const& ret = temp.execute(_t, mc_info, Permanence::Reverted/*, dev::eth::OnOpFunc()*/);

	return ret;
}

dev::bytes mcp::Client::callSystem(dev::Address const& _from, dev::Address const& _contractAddress, dev::bytes const& _data)
{
	ExecutionResult const& ar = call(_from, 0, _contractAddress, _data, Invalid256, Invalid256, PendingBlock);
	//result = ar.output;
	return ar.output;
}

LocalisedTransaction mcp::Client::localisedTransaction(h256 const& _transactionHash) const
{
	auto transaction = m_store.create_transaction();
	auto t = m_cache->transaction_get(transaction, _transactionHash);
	auto td = m_cache->transaction_address_get(transaction, _transactionHash);
	if (td == nullptr || t == nullptr)
		BOOST_THROW_EXCEPTION(TransactionNotFound());
	uint64_t block_number = 0;
	m_cache->block_number_get(transaction, td->blockHash, block_number);/// not must be existed
	/// todo: bu yiding zheng que,huancun zhong de block number shi duoshao ?????
	return LocalisedTransaction(*t, td->blockHash, td->index, block_number);
}

LocalisedTransaction mcp::Client::localisedTransaction(h256 const& _blockHash, unsigned _i) const
{
	auto transaction = m_store.create_transaction();
	auto block(m_cache->block_get(transaction, _blockHash));
	uint64_t block_number;
	if (block == nullptr || m_cache->block_number_get(transaction, _blockHash, block_number))
		BOOST_THROW_EXCEPTION(BlockNotFound());
	if (_i >= block->links().size())
		BOOST_THROW_EXCEPTION(TransactionNotFound());

	dev::h256 hash = block->links().at(_i);//todo error!!!!!!!!!!!!!!!!!!
	auto t = m_cache->transaction_get(transaction, hash);
	if (t == nullptr)
		BOOST_THROW_EXCEPTION(TransactionNotFound());

	return LocalisedTransaction(*t, block->hash(), _i, block_number);
}

LocalisedTransaction mcp::Client::localisedTransaction(BlockNumber const& _block, unsigned _i) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash block_hash;
	if (m_cache->block_number_get(transaction, _block, block_hash))
		BOOST_THROW_EXCEPTION(BlockNotFound());
	auto block(m_cache->block_get(transaction, block_hash));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(BlockNotFound());
	if (_i >= block->links().size())
		BOOST_THROW_EXCEPTION(TransactionNotFound());

	dev::h256 hash = block->links().at(_i);
	auto t = m_cache->transaction_get(transaction, hash);
	if (t == nullptr)
		BOOST_THROW_EXCEPTION(TransactionNotFound());

	return LocalisedTransaction(*t, block_hash, _i, _block);
}

LocalisedBlock mcp::Client::localisedBlock(BlockNumber const& _block) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());

	auto block(m_cache->block_get(transaction, _block));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(BlockNotFound());
	auto state = m_cache->block_state_get(transaction, block->hash());
	if (block == nullptr || state == nullptr || !state->is_stable)
		BOOST_THROW_EXCEPTION(BlockNotFound());

	dev::h256 _parentHash(0);///genesis block have no parent
	if (_block && m_cache->block_number_get(transaction, _block - 1, _parentHash))
		BOOST_THROW_EXCEPTION(BlockNotFound());

	mcp::Transactions txs;
	for (auto& th : block->links())
	{
		auto td = m_cache->transaction_address_get(transaction, th);
		if (td == nullptr || td->blockHash != block->hash())///not first linked, ignore.
			continue;
		auto t = m_cache->transaction_get(transaction, th);
		txs.push_back(*t);
	}

	//mcp::summary_hash stateRoot;/// stateRoot
	//m_cache->block_summary_get(transaction, block->hash(), stateRoot);
	//dev::h256 receiptsRoot;/// receiptsRoot
	//m_store.GetBlockReceiptsRoot(transaction, block->hash(), receiptsRoot);

	return mcp::LocalisedBlock(*block,
		_block,
		txs,
		state->m_stateRoot,
		state->m_receiptsRoot,
		_parentHash
	);
}

LocalisedBlock mcp::Client::localisedBlock(h256 const& _block) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block = m_cache->block_get(transaction, _block);
	auto state = m_cache->block_state_get(transaction, _block);
	if (block == nullptr || state == nullptr || !state->is_stable)
		BOOST_THROW_EXCEPTION(BlockNotFound());

	dev::h256 _parentHash(0);///genesis block have no parent
	if (state->stable_index && m_cache->block_number_get(transaction, state->stable_index - 1, _parentHash))
		BOOST_THROW_EXCEPTION(BlockNotFound());

	mcp::Transactions txs;
	for (auto& th : block->links())
	{
		auto td = m_cache->transaction_address_get(transaction, th);
		if (td == nullptr || td->blockHash != _block)///not first linked, ignore.
			continue;
		auto t = m_cache->transaction_get(transaction, th);
		txs.push_back(*t);
	}

	//mcp::summary_hash stateRoot;/// stateRoot
	//m_cache->block_summary_get(transaction, block->hash(), stateRoot);
	//dev::h256 receiptsRoot;/// receiptsRoot
	//m_store.GetBlockReceiptsRoot(transaction, block->hash(), receiptsRoot);

	return mcp::LocalisedBlock(*block,
		state->stable_index,
		txs,
		state->m_stateRoot,
		state->m_receiptsRoot,
		_parentHash
	);
}

mcp::block mcp::Client::blockInfo(h256 const& _block) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, _block));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(BlockNotFound());
	return *block;
}

block mcp::Client::blockInfo(BlockNumber const& _block) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, _block));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(BlockNotFound());
	return *block;
}

block_state mcp::Client::blockState(h256 const& _block) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, _block));
	if (state == nullptr)
		BOOST_THROW_EXCEPTION(BlockNotFound());
	return *state;
}

h256 mcp::Client::blockSummary(h256 const& _block) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	h256 summary;
	if (m_cache->block_summary_get(transaction, _block, summary))
		BOOST_THROW_EXCEPTION(BlockNotFound());
	return summary;
}

h256 mcp::Client::mciHash(uint64_t const& _mci) /*const*/
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	h256 _h;
	if (!m_store.main_chain_get(transaction, _mci, _h))
		BOOST_THROW_EXCEPTION(MciNotFound());
	return _h;
}

dev::eth::LocalisedTransactionReceipt mcp::Client::localisedTransactionReceipt(h256 const& _transactionHash) const
{
	auto transaction = m_store.create_transaction();
	auto t = m_cache->transaction_get(transaction, _transactionHash);
	auto tr = m_cache->transaction_receipt_get(transaction, _transactionHash);
	auto td = m_cache->transaction_address_get(transaction, _transactionHash);

	if (t == nullptr || tr == nullptr || td == nullptr)
		BOOST_THROW_EXCEPTION(TransactionNotFound());

	uint64_t block_number = 0;
	if (m_cache->block_number_get(transaction, td->blockHash, block_number))
		BOOST_THROW_EXCEPTION(TransactionNotFound());

	return dev::eth::LocalisedTransactionReceipt(
		*tr,
		t->sha3(),
		td->blockHash,
		block_number,
		t->from(),
		t->to(),
		td->index,
		toAddress(t->from(), t->nonce()));
}

unsigned mcp::Client::transactionCount(h256 _blockHash) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, _blockHash));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(BlockNotFound());

	return block->links().size();
}

unsigned mcp::Client::transactionCount(BlockNumber _block) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	//mcp::block_hash block_hash;
	//if (m_cache->block_number_get(transaction, block_number, block_hash))
	//	BOOST_THROW_EXCEPTION(RPC_Error_NoResult());

	auto block(m_cache->block_get(transaction, _block));
	if (block == nullptr)
		BOOST_THROW_EXCEPTION(BlockNotFound());

	return block->links().size();
}

Approves mcp::Client::epochApproves(Epoch _epoch)
{
	Approves ret;
	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::list<h256> hashs;
	m_store.epoch_approves_get(transaction, _epoch, hashs);

	for (auto hash : hashs)
	{
		auto approve = m_cache->approve_get(transaction, hash);
		ret.push_back(*approve);
	}
	return ret;
}

ApproveReceipt mcp::Client::approveReceipt(dev::h256 const& _h) const
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto _a = m_cache->approve_receipt_get(transaction, _h);
	if (_a == nullptr)
		BOOST_THROW_EXCEPTION(ApproveNotFound());
	return *_a;
}

uint64_t mcp::Client::number() const
{
	return bc().number();
}

uint64_t mcp::Client::lastStableMci() const
{
	return m_chain->last_stable_mci();
}

uint64_t mcp::Client::lastMci() const
{
	return m_chain->last_mci();
}

mcp::Epoch mcp::Client::lastEpoch() const
{
	return m_chain->last_epoch();
}

Block mcp::Client::block(h256 const& _h, bool _debug) const
{
	mcp::db::db_transaction _t(m_store.create_transaction());
	auto _currentState = m_cache->block_state_get(_t, _h);
	if (nullptr == _currentState)
		BOOST_THROW_EXCEPTION(RootNotFound());
	auto block(m_cache->block_get(_t, _h));
	if (nullptr == block)
		BOOST_THROW_EXCEPTION(RootNotFound());

	auto _previousState = _currentState;
	if (_currentState->stable_index)
	{
		h256 _previousHash;
		m_cache->block_number_get(_t, _currentState->stable_index - 1, _previousHash);
		_previousState = m_cache->block_state_get(_t, _previousHash);
	}

	dev::OverlayDB _db = dev::OverlayDB(m_store.db());
	Block ret{ bc(), _db };
	Transactions _txs;
	for (h256 const& _th : block->links())
	{
		auto _tx = m_cache->transaction_get(_t, _th);
		_txs.push_back(*_tx);
	}
	ret.populateFromChain(_currentState, block, _previousState, _txs);

	return ret;
}

Block mcp::Client::blockByNumber(BlockNumber _h) const
{
	if (_h == LatestBlock || _h == PendingBlock)
		return m_chain->postSeal();
	return block(bc().numberHash(_h));
}

h256 mcp::Client::workTransactionHash(Epoch _epoch)
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	h256 _h;
	m_store.epoch_work_transaction_get(transaction, _epoch, _h);
	return _h;
}

//json mcp::Client::status() const
//{
//	json result;
//	result["syncing"] = mcp::node_sync::is_syncing() ? 1 : 0;
//	result["last_stable_mci"] = m_chain->last_stable_mci();
//	result["last_mci"] = m_chain->last_mci();
//	result["last_stable_block_index"] = m_chain->last_stable_index();
//	result["epoch"] = m_chain->last_epoch();
//	result["epoch_period"] = mcp::epoch_period;
//	return result;
//}

WitnessList mcp::Client::witnessList(Epoch _epoch) const
{
	if (_epoch > m_chain->last_epoch())
		BOOST_THROW_EXCEPTION(EpochNotFound());

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::witness_param const& w_param(mcp::param::witness_param(transaction, _epoch));
	return w_param.witness_list;
}

std::unordered_map<p2p::node_id, bi::tcp::endpoint> mcp::Client::peers() const
{
	return m_host->peers();
}

std::list<p2p::node_info> mcp::Client::nodes() const
{
	return m_host->nodes();
}

bool mcp::Client::netListening() const
{
	return m_host->is_started();
}

size_t mcp::Client::peersCount() const
{
	return m_host->get_peers_count();
}

int mcp::Client::storeVersion()
{
	return m_store.version_get();
}

bool mcp::Client::getMcInfo(dev::eth::McInfo& mc_info_a, uint64_t& block_number)
{
	uint64_t _bn = block_number;
	if (block_number == LatestBlock || block_number == PendingBlock)
		_bn = m_chain->last_stable_index();

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash block_hash;
	bool exists(!m_cache->block_number_get(transaction, _bn, block_hash));
	if (!exists)
		return false;

	std::shared_ptr<mcp::block_state> mc_state(m_cache->block_state_get(transaction, block_hash));
	assert_x(mc_state);
	assert_x(mc_state->is_stable);
	assert_x(mc_state->main_chain_index);
	assert_x(mc_state->mc_timestamp > 0);

	uint64_t last_summary_mci(0);
	Address _author;
	if (block_hash != mcp::genesis::block_hash)
	{
		std::shared_ptr<mcp::block> mc_block(m_cache->block_get(transaction, block_hash));
		assert_x(mc_block);
		std::shared_ptr<mcp::block_state> last_summary_state(m_cache->block_state_get(transaction, mc_block->last_summary_block()));
		assert_x(last_summary_state);
		assert_x(last_summary_state->is_stable);
		assert_x(last_summary_state->is_on_main_chain);
		assert_x(last_summary_state->main_chain_index);
		last_summary_mci = *last_summary_state->main_chain_index;
		_author = mc_block->from();
	}

	mc_info_a = dev::eth::McInfo(mc_state->stable_index, *mc_state->main_chain_index, mc_state->mc_timestamp, last_summary_mci, _author);

	return true;
}
