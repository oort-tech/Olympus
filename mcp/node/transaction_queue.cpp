#include "transaction_queue.hpp"
#include <thread>

namespace mcp
{
	using namespace std;
	using namespace dev;

	constexpr size_t c_maxVerificationQueueSize = 8192;
	constexpr size_t c_maxDroppedTransactionCount = 1024;

	TransactionQueue::TransactionQueue(mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::chain> chain_a):
		m_store(store_a),
		m_cache(cache_a),
		m_chain(chain_a),
		m_current{ PriorityCompare{ *this } },
		m_dropped(50000),
		m_limit(50000),
		m_futureLimit(50000)
	{
		unsigned verifierThreads = std::max(thread::hardware_concurrency(), 3U) - 2U;
		for (unsigned i = 0; i < verifierThreads; ++i)
			m_verifiers.emplace_back([=]() {
			setThreadName("txcheck" + toString(i));
			this->verifierBody();
		});
	}

	TransactionQueue::~TransactionQueue()
	{
		DEV_GUARDED(x_queue)
			m_aborting = true;
		m_queueReady.notify_all();
		for (auto& i : m_verifiers)
			i.join();
	}

	ImportResult TransactionQueue::import(bytesConstRef _transactionRLP, IfDropped _ik)
	{
		RLP const rlp(_transactionRLP);
		try
		{
			transaction t = transaction(rlp, CheckTransaction::Everything);
			return import(t, _ik);
		}
		catch (Exception const&)
		{
			return ImportResult::Malformed;
		}
	}

	ImportResult TransactionQueue::check_WITH_LOCK(h256 const& _h, IfDropped _ik)
	{
		if (m_known.count(_h))
			return ImportResult::AlreadyKnown;

		if (m_dropped.touch(_h) && _ik == IfDropped::Ignore)
			return ImportResult::AlreadyInChain;

		return ImportResult::Success;
	}

	ImportResult TransactionQueue::import(transaction const& _transaction, IfDropped _ik)
	{
		validateTx(_transaction);
		//if (_transaction.hasZeroSignature())
		//	return ImportResult::ZeroSignature;
		// Check if we already know this transaction.
		h256 h = _transaction.sha3();

		ImportResult ret;
		{
			UpgradableGuard l(m_lock);
			auto ir = check_WITH_LOCK(h, _ik);
			if (ir != ImportResult::Success)
				return ir;

			{
				_transaction.safeSender();  // Perform EC recovery outside of the write lock
				UpgradeGuard ul(l);
				ret = manageImport_WITH_LOCK(h, _transaction);
			}
		}
		return ret;
	}

	h256s TransactionQueue::topTransactions(unsigned _limit, h256Hash const& _avoid) const
	{
		ReadGuard l(m_lock);
		h256s ret;
		for (auto cs = m_currentByAddressAndNonce.begin(); ret.size() < _limit && cs != m_currentByAddressAndNonce.end(); ++cs)
		{
			for (auto t = cs->second.begin(); t != cs->second.end(); t++)
			{
				auto hash = (*t->second).transaction.sha3();
				if (!_avoid.count(hash))
					ret.push_back(hash);
			}
		}
			
		return ret;
	}

	h256Hash TransactionQueue::knownTransactions() const
	{
		ReadGuard l(m_lock);
		return m_known;
	}


	ImportResult TransactionQueue::manageImport_WITH_LOCK(h256 const& _h, transaction const& _transaction)
	{
		try
		{
			assert(_h == _transaction.sha3());
			// Remove any prior transaction with the same nonce but a lower gas price.
			// Bomb out if there's a prior transaction with higher gas price.
			auto cs = m_currentByAddressAndNonce.find(_transaction.from());
			if (cs != m_currentByAddressAndNonce.end())
			{
				auto t = cs->second.find(_transaction.nonce());
				if (t != cs->second.end())
				{
					if (_transaction.gasPrice() < (*t->second).transaction.gasPrice())
						return ImportResult::OverbidGasPrice;
					//else
					//{
					//	h256 dropped = (*t->second).transaction.hash();
					//	remove_WITH_LOCK(dropped);
					//	m_onReplaced(dropped);
					//}
				}
			}
			auto fs = m_future.find(_transaction.from());
			if (fs != m_future.end())
			{
				auto t = fs->second.find(_transaction.nonce());
				if (t != fs->second.end())
				{
					if (_transaction.gasPrice() < t->second.transaction.gasPrice())
						return ImportResult::OverbidGasPrice;
					else
					{
						fs->second.erase(t);
						--m_futureSize;
						if (fs->second.empty())
							m_future.erase(fs);
					}
				}
			}
			// If valid, append to transactions.
			insertCurrent_WITH_LOCK(make_pair(_h, _transaction));
			LOG(m_log.debug) << "Queued vaguely legit-looking transaction " << _h;

			while (m_current.size() > m_limit)
			{
				LOG(m_log.debug) << "Dropping out of bounds transaction " << _h;
				remove_WITH_LOCK(m_current.rbegin()->transaction.sha3());
			}

			//m_onReady();
		}
		catch (Exception const& _e)
		{
			LOG(m_log.debug) << "Ignoring invalid transaction: " << diagnostic_information(_e);
			return ImportResult::Malformed;
		}
		catch (std::exception const& _e)
		{
			LOG(m_log.debug) << "Ignoring invalid transaction: " << _e.what();
			return ImportResult::Malformed;
		}

		return ImportResult::Success;
	}

	u256 TransactionQueue::maxNonce(Address const& _a) const
	{
		ReadGuard l(m_lock);
		return maxNonce_WITH_LOCK(_a);
	}

	u256 TransactionQueue::maxNonce_WITH_LOCK(Address const& _a) const
	{
		u256 ret = 0;
		auto cs = m_currentByAddressAndNonce.find(_a);
		if (cs != m_currentByAddressAndNonce.end() && !cs->second.empty())
			ret = cs->second.rbegin()->first + 1;
		auto fs = m_future.find(_a);
		if (fs != m_future.end() && !fs->second.empty())
			ret = std::max(ret, fs->second.rbegin()->first + 1);
		return ret;
	}


	void TransactionQueue::insertCurrent_WITH_LOCK(std::pair<h256, transaction> const& _p)
	{
		if (m_currentByHash.count(_p.first))
		{
			LOG(m_log.debug) << "Transaction hash" << _p.first << "already in current?!";
			return;
		}

		transaction const& t = _p.second;
		// Insert into current
		auto inserted = m_currentByAddressAndNonce[t.from()].insert(std::make_pair(t.nonce(), PriorityQueue::iterator()));
		PriorityQueue::iterator handle = m_current.emplace(VerifiedTransaction(t));
		inserted.first->second = handle;
		m_currentByHash[_p.first] = handle;

		// Move following transactions from future to current
		makeCurrent_WITH_LOCK(t);
		m_known.insert(_p.first);
	}

	bool TransactionQueue::remove_WITH_LOCK(h256 const& _txHash)
	{
		auto t = m_currentByHash.find(_txHash);
		if (t == m_currentByHash.end())
			return false;

		Address from = (*t->second).transaction.from();
		auto it = m_currentByAddressAndNonce.find(from);
		assert(it != m_currentByAddressAndNonce.end());
		it->second.erase((*t->second).transaction.nonce());
		m_current.erase(t->second);
		m_currentByHash.erase(t);
		if (it->second.empty())
			m_currentByAddressAndNonce.erase(it);
		m_known.erase(_txHash);
		return true;
	}


	void TransactionQueue::makeCurrent_WITH_LOCK(transaction const& _t)
	{
		bool newCurrent = false;
		auto fs = m_future.find(_t.from());
		if (fs != m_future.end())
		{
			u256 nonce = _t.nonce() + 1;
			auto fb = fs->second.find(nonce);
			if (fb != fs->second.end())
			{
				auto ft = fb;
				while (ft != fs->second.end() && ft->second.transaction.nonce() == nonce)
				{
					auto inserted = m_currentByAddressAndNonce[_t.from()].insert(std::make_pair(ft->second.transaction.nonce(), PriorityQueue::iterator()));
					PriorityQueue::iterator handle = m_current.emplace(move(ft->second));
					inserted.first->second = handle;
					m_currentByHash[(*handle).transaction.sha3()] = handle;
					--m_futureSize;
					++ft;
					++nonce;
					newCurrent = true;
				}
				fs->second.erase(fb, ft);
				if (fs->second.empty())
					m_future.erase(_t.from());
			}
		}

		while (m_futureSize > m_futureLimit)
		{
			// TODO: priority queue for future transactions
			// For now just drop random chain end
			--m_futureSize;
			LOG(m_log.debug) << "Dropping out of bounds future transaction "
				<< m_future.begin()->second.rbegin()->second.transaction.sha3();
			m_future.begin()->second.erase(--m_future.begin()->second.end());
			if (m_future.begin()->second.empty())
				m_future.erase(m_future.begin());
		}

		//if (newCurrent)
		//	m_onReady();
	}


	void TransactionQueue::drop(h256 const& _txHash)
	{
		UpgradableGuard l(m_lock);

		if (!m_known.count(_txHash))
			return;

		UpgradeGuard ul(l);
		m_dropped.insert(_txHash, true /* placeholder value */);
		remove_WITH_LOCK(_txHash);
	}

	transaction TransactionQueue::get(h256 const& _txHash) const
	{
		UpgradableGuard l(m_lock);

		auto t = m_currentByHash.find(_txHash);
		if (t == m_currentByHash.end())
			return transaction();

		return (*t->second).transaction;
	}


	void TransactionQueue::enqueue(RLP const& _data, h512 const& _nodeId)
	{
		bool queued = false;
		{
			Guard l(x_queue);
			unsigned itemCount = _data.itemCount();
			for (unsigned i = 0; i < itemCount; ++i)
			{
				if (m_unverified.size() >= c_maxVerificationQueueSize)
				{
					LOG(m_log.info) << "Transaction verification queue is full. Dropping "
						<< itemCount - i << " transactions";
					break;
				}
				m_unverified.emplace_back(UnverifiedTransaction(_data[i].data(), _nodeId));
				queued = true;
			}
		}
		if (queued)
			m_queueReady.notify_all();
	}

	void TransactionQueue::verifierBody()
	{
		while (!m_aborting)
		{
			UnverifiedTransaction work;

			{
				unique_lock<Mutex> l(x_queue);
				m_queueReady.wait(l, [&]() { return !m_unverified.empty() || m_aborting; });
				if (m_aborting)
					return;
				work = move(m_unverified.front());
				m_unverified.pop_front();
			}

			try
			{
				transaction t(work.transaction, CheckTransaction::Everything);
				ImportResult ir = import(t);
				//m_onImport(ir, t.sha3(), work.nodeId); //Notify capability and P2P to process peer. diconnect peer if bad transaction  
			}
			catch (...)
			{
				// should not happen as exceptions are handled in import.
				cwarn << "Bad transaction:" << boost::current_exception_diagnostic_information();
			}
		}
	}

	void TransactionQueue::validateTx(transaction const& _t)
	{
		if (_t.hasZeroSignature())
			BOOST_THROW_EXCEPTION(ZeroSignatureTransaction());
		_t.checkChainId(mcp::chain_id);
		_t.checkLowS();

		eth::EVMSchedule const& schedule = dev::eth::EVMSchedule();
		/// Pre calculate the gas needed for execution
		if (_t.baseGasRequired(schedule) > _t.gas())
			BOOST_THROW_EXCEPTION(OutOfGasIntrinsic() << RequirementError(
			(bigint)(_t.baseGasRequired(schedule)), (bigint)_t.gas()));

		/// Avoid transactions that would take us beyond the block gas limit.
		if ((uint256_t)_t.gas() > mcp::uint256_t(mcp::block_max_gas))
			BOOST_THROW_EXCEPTION(BlockGasLimitReached() << RequirementErrorComment(
			(bigint)(mcp::block_max_gas), (bigint)_t.gas(),
				std::string("_gasUsed + (bigint)_t.gas() > _header.gasLimit()")));

		/// Avoid transactions that are less than the lower gas price limit.
		if ((uint256_t)_t.gasPrice() < mcp::uint256_t(mcp::gas_price))
			BOOST_THROW_EXCEPTION(BlockGasLimitReached() << RequirementErrorComment(
			(bigint)(mcp::block_max_gas), (bigint)_t.gas(),
				std::string("_gasUsed + (bigint)_t.gas() < lower.gasLimit()")));

		/// nonce great than last stable transaction nonce,It doesn't mean it's right,meybe exist pending transactions
		mcp::db::db_transaction transaction(m_store.create_transaction());
		mcp::chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
		auto nonce = c_state.getNonce(_t.sender());
		if (nonce > _t.nonce())
			BOOST_THROW_EXCEPTION(
				InvalidNonce() << RequirementError((bigint)_t.nonce(), (bigint)nonce));

		/// check balance
		bigint gasCost = (bigint)_t.gas() * _t.gasPrice();
		bigint totalCost = _t.value() + gasCost;
		if (c_state.balance(_t.sender()) < totalCost)
			BOOST_THROW_EXCEPTION(
				NotEnoughCash() << RequirementError(totalCost, (bigint)c_state.balance(_t.sender())) << errinfo_comment(_t.sender().hex()));
	}

}