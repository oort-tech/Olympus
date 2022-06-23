#include "transaction_queue.hpp"
#include <thread>

namespace mcp
{
	int FutureTimeUnknownSize = 64;

	using namespace std;
	using namespace dev;

	constexpr size_t c_maxVerificationQueueSize = 8192;
	constexpr size_t c_maxDroppedTransactionCount = 1024;

	TransactionQueue::TransactionQueue(
		mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::chain> chain_a,
		std::shared_ptr<mcp::async_task> async_task_a
	):
		m_store(store_a),
		m_cache(cache_a),
		m_chain(chain_a),
		m_async_task(async_task_a),
		m_current{ PriorityCompare{ *this } },
		m_dropped(100000),
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

	ImportResult TransactionQueue::check_WITH_LOCK(h256 const& _h, IfDropped _ik)
	{
		if (m_known.count(_h) )
			return ImportResult::AlreadyKnown;

		mcp::db::db_transaction t(m_store.create_transaction());
		if (_ik == IfDropped::Ignore && (m_dropped.touch(_h) || m_cache->transaction_exists(t, _h)))
			return ImportResult::AlreadyInChain;

		return ImportResult::Success;
	}

	ImportResult TransactionQueue::import(Transaction const& _transaction, bool isLoccal, IfDropped _ik)
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
				_transaction.safeSender();  /// Perform EC recovery outside of the write lock
				UpgradeGuard ul(l);
				checkTx(_transaction); ///check balance and nonce
				ret = manageImport_WITH_LOCK(h, _transaction, isLoccal);

#if 0	///////////////////////////////test
				LOG(m_log.debug) << getInfo();
				LOG(m_log.debug) << "--------------m_currentByHash-------------";
				for (auto it = m_currentByHash.begin(); it != m_currentByHash.end(); it++)
				{
					LOG(m_log.debug) << it->first.hex() << " , " << (*it->second).transaction.sha3().hex() << " ,nonce:" << (*it->second).transaction.nonce();
				}
				LOG(m_log.debug) << "--------------m_currentByHash end-------------";

				LOG(m_log.debug) << "--------------m_currentByAddressAndNonce-------------";
				for (auto it = m_currentByAddressAndNonce.begin(); it != m_currentByAddressAndNonce.end(); it++)
				{
					LOG(m_log.debug) << "address:" << it->first.hex();
					auto m = it->second;
					for (auto at = m.begin(); at != m.end(); at++)
					{
						LOG(m_log.debug) << "--nonce:" << at->first << " ,hash:"  << (*at->second).transaction.sha3().hex();
					}
				}
				LOG(m_log.debug) << "--------------m_currentByAddressAndNonce end-------------";

				LOG(m_log.debug) << "--------------m_sameCurrentByAddressAndNonce-------------";
				for (auto it = m_sameCurrentByAddressAndNonce.begin(); it != m_sameCurrentByAddressAndNonce.end(); it++)
				{
					LOG(m_log.debug) << "address:" << it->first.hex();
					auto m = it->second;
					for (auto at = m.begin(); at != m.end(); at++)
					{
						LOG(m_log.debug) << "--nonce:" << at->first;
						auto mm = at->second;
						for (auto mt = mm.begin(); mt != mm.end(); mt++)
						{
							LOG(m_log.debug) << "----hash:" << mt->first.hex() << " ,it:" << (*mt->second).transaction.sha3().hex();
						}
					}
				}
				LOG(m_log.debug) << "--------------m_sameCurrentByAddressAndNonce end-------------";

				LOG(m_log.debug) << "--------------m_future-------------";
				for (auto it = m_future.begin(); it != m_future.end(); it++)
				{
					LOG(m_log.debug) << "address:" << it->first.hex();
					for (auto at = it->second.begin(); at != it->second.end(); at++)
					{
						LOG(m_log.debug) << "--nonce:" << at->first << " ,hash:" << at->second.transaction.sha3().hex();
					}
				}
				LOG(m_log.debug) << "--------------m_future end-------------";

#endif // 1
			}
		}
		return ret;
	}

	ImportResult TransactionQueue::importLocal(Transaction const& _transaction)
	{
		auto ret = import(_transaction,true);
		if (ImportResult::Success == ret)/// first import && successed,broadcast it
		{
			m_async_task->sync_async([this, _transaction]() {
				m_capability->broadcast_transaction(_transaction);
			});
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

	bool TransactionQueue::exist(h256 const& _hash)
	{
		ReadGuard l(m_lock);
		return m_currentByHash.count(_hash);
	}

	h256Hash TransactionQueue::knownTransactions() const
	{
		ReadGuard l(m_lock);
		return m_known;
	}


	ImportResult TransactionQueue::manageImport_WITH_LOCK(h256 const& _h, Transaction const& _transaction, bool isLocal)
	{
		try
		{
			assert(_h == _transaction.sha3());
			// Remove any prior transaction with the same nonce but a lower gas price.
			// Bomb out if there's a prior transaction with higher gas price.
			
			// If valid, append to transactions.
			auto r = isFuture_WITH_LOCK(_transaction);
			if (NonceRange::FutureTooBig == r)
			{
				return ImportResult::FutureTimeKnown; ///if local throw error,if broadcast punish remote node.
			}
			else if (NonceRange::Future == r)/// future
			{
				return insertFuture_WITH_LOCK(make_pair(_h, _transaction), isLocal);
				LOG(m_log.debug) << "Queued vaguely not legit-looking transaction " << _h.hex();
			}
			else ///current
			{
				return insertCurrent_WITH_LOCK(make_pair(_h, _transaction), isLocal);
				LOG(m_log.debug) << "Queued vaguely legit-looking transaction " << _h.hex();
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

	u256 TransactionQueue::maxNonce(Address const& _a, BlockNumber const blockTag) const
	{
		ReadGuard l(m_lock);
		return maxNonce_WITH_LOCK(_a, blockTag);
	}

	u256 TransactionQueue::maxNonce_WITH_LOCK(Address const& _a, BlockNumber const blockTag) const
	{
		u256 ret = 0;
		auto cs = m_currentByAddressAndNonce.find(_a);
		if (cs != m_currentByAddressAndNonce.end() && !cs->second.empty())
			ret = cs->second.rbegin()->first + 1;

		if (blockTag == PendingBlock) {
			auto fs = m_future.find(_a);
			if (fs != m_future.end() && !fs->second.empty())
				ret = std::max(ret, fs->second.rbegin()->first + 1);
		}

		return ret;
	}


	ImportResult TransactionQueue::insertCurrent_WITH_LOCK(std::pair<h256, Transaction> const& _p, bool isLocal)
	{
		if (m_currentByHash.count(_p.first))
		{
			LOG(m_log.debug) << "Transaction hash" << _p.first.hex() << "already in current?!";
			return ImportResult::Success;
		}
		Transaction const& ts = _p.second;
		auto from = ts.sender();
		auto fs = m_currentByAddressAndNonce.find(ts.from());
		if (fs != m_currentByAddressAndNonce.end()) /// have transaction used nonce,replace it
		{
			auto t = fs->second.find(ts.nonce());
			if (t != fs->second.end()) /// have same nonce
			{
				if (ts.gasPrice() < (*t->second).transaction.gasPrice())
				{
					if (isLocal) /// if local return error
						return ImportResult::OverbidGasPrice;

					/// insert into parallel queue
					PriorityQueue::iterator handle = m_current.emplace(VerifiedTransaction(ts));
					m_currentByHash[_p.first] = handle;
					if (!m_sameCurrentByAddressAndNonce.count(from) || !m_sameCurrentByAddressAndNonce[from].count(ts.nonce()))
					{
						std::map<h256, PriorityQueue::iterator> m;
						m.insert(std::make_pair(_p.first, handle));
						m_sameCurrentByAddressAndNonce[from].insert(std::make_pair(ts.nonce(), m));
					}
					else /// account and nonce existed
					{
						m_sameCurrentByAddressAndNonce[from][ts.nonce()].emplace(std::make_pair(_p.first, handle));
					}
				}
				else
				{
					/// move to parallel queue from currentByAddressAndNonce.
					PriorityQueue::iterator oldhandle = t->second;
					if (!m_sameCurrentByAddressAndNonce.count(from) || !m_sameCurrentByAddressAndNonce[from].count(ts.nonce()))
					{
						std::map<h256, PriorityQueue::iterator> m;
						m.emplace(std::make_pair((*oldhandle).transaction.sha3(), oldhandle));
						m_sameCurrentByAddressAndNonce[from].insert(std::make_pair(ts.nonce(), m));
					}
					else /// account and nonce existed
					{
						m_sameCurrentByAddressAndNonce[from][ts.nonce()].emplace(std::make_pair((*oldhandle).transaction.sha3(), oldhandle));
					}

					/// Insert into current,and replace m_currentByAddressAndNonce with new transaction
					PriorityQueue::iterator handle = m_current.emplace(VerifiedTransaction(ts));
					m_currentByHash[_p.first] = handle;
					t->second = handle;
				}
				m_known.insert(_p.first);
				return ImportResult::Success;
			}
		}

		
		/// Insert into current
		auto inserted = m_currentByAddressAndNonce[ts.from()].insert(std::make_pair(ts.nonce(), PriorityQueue::iterator()));
		PriorityQueue::iterator handle = m_current.emplace(VerifiedTransaction(ts));
		inserted.first->second = handle;
		m_currentByHash[_p.first] = handle;

		/// Move following transactions from future to current
		makeCurrent_WITH_LOCK(ts);
		m_known.insert(_p.first);
		return ImportResult::Success;
	}

	ImportResult TransactionQueue::insertFuture_WITH_LOCK(std::pair<h256, Transaction> const& _p, bool isLocal)
	{
		if (m_currentByHash.count(_p.first))
			assert_x(false);

		auto fs = m_future.find(_p.second.from());
		if (fs != m_future.end()) /// have transaction used nonce,replace it
		{
			auto t = fs->second.find(_p.second.nonce());
			if (t != fs->second.end())
			{
				if (_p.second.gasPrice() < t->second.transaction.gasPrice())
				{
					if (isLocal) /// if local return error
						return ImportResult::OverbidGasPrice;
				}
				else
				{
					m_known.erase(t->second.transaction.sha3());
					fs->second.erase(t);
					fs->second.emplace(_p.second.nonce(), _p.second);
					m_known.insert(_p.first);
				}
				return ImportResult::Success;
			}
		}

		/// insert with new object
		auto& target = m_future[_p.second.from()];
		// Insert into current
		target.emplace(_p.second.nonce(), move(VerifiedTransaction(_p.second)));
		++m_futureSize;
		m_known.insert(_p.first);

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

		return ImportResult::Success;
	}


	/// block A,B,C,D. A <-B <-D,A <-C <-D
	/// processing sequence maybe A,B,C,D ands maybe A,C,B,D
	/// there have two transaction,T1 and T2,used same from account and nonce. so that,one in m_currentByAddressAndNonce and another in m_sameCurrentByAddressAndNonce.
	/// B links T1,C links T2. So both T1 and T2 could be saved first.However, if you delete according to the nonce, the second save may cause problems.
	/// Therefore, if the first time in current then deleted it. 
	/// if in sameCurrent not only delete it but also needs move the same account and nonce transaction in current to sameCurrent.because composer need get transaction hash from current queue.
	/// todo: but if unlined transaction can not be removed. transaction stable can remove other same account and nonce?
	bool TransactionQueue::remove_WITH_LOCK(h256 const& _txHash)
	{
		auto t = m_currentByHash.find(_txHash);
		if (t == m_currentByHash.end())
			return false;
		Address from = (*t->second).transaction.from();
		u256 nonce = (*t->second).transaction.nonce();

		
		if (m_currentByAddressAndNonce.count(from) && m_currentByAddressAndNonce[from].count(nonce))
		{
			/// Not the same transaction needs move it to the sameCurrent queue
			if (m_currentByAddressAndNonce[from][nonce]->transaction.sha3() != _txHash)
			{
				auto handle = m_currentByAddressAndNonce[from][nonce];
				if (!m_sameCurrentByAddressAndNonce.count(from) || !m_sameCurrentByAddressAndNonce[from].count(nonce))
				{
					std::map<h256, PriorityQueue::iterator> m;
					m.insert(std::make_pair(handle->transaction.sha3(), handle));
					m_sameCurrentByAddressAndNonce[from].insert(std::make_pair(nonce, m));
				}
				else /// account and nonce existed
				{
					m_sameCurrentByAddressAndNonce[from][nonce].emplace(std::make_pair(handle->transaction.sha3(), handle));
				}
			}

			m_currentByAddressAndNonce[from].erase(nonce);
			if (m_currentByAddressAndNonce[from].empty())
				m_currentByAddressAndNonce.erase(from);
			
		}

		if (m_sameCurrentByAddressAndNonce.count(from) && m_sameCurrentByAddressAndNonce[from].count(nonce))
		{
			auto& target = m_sameCurrentByAddressAndNonce[from][nonce];
			if (m_sameCurrentByAddressAndNonce[from][nonce].count(_txHash))
			{
				m_sameCurrentByAddressAndNonce[from][nonce].erase(_txHash);
				if (m_sameCurrentByAddressAndNonce[from][nonce].empty())
				{
					m_sameCurrentByAddressAndNonce[from].erase(nonce);
					if (m_sameCurrentByAddressAndNonce[from].empty())
					{
						m_sameCurrentByAddressAndNonce.erase(from);
					}
				}
			}
		}

		m_current.erase(t->second);
		m_currentByHash.erase(t);
		m_known.erase(_txHash);
		
		return true;
	}


	void TransactionQueue::makeCurrent_WITH_LOCK(Transaction const& _t)
	{
		//bool newCurrent = false;
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
					//newCurrent = true;
				}
				fs->second.erase(fb, ft);
				if (fs->second.empty())
					m_future.erase(_t.from());
			}
		}

		//if (newCurrent)
		//	m_onReady();
	}

	NonceRange TransactionQueue::isFuture_WITH_LOCK(Transaction const& _transaction)
	{
		/// account's first transaction
		if (_transaction.nonce() == 0) 
			return NonceRange::Current;

		/// have transactions in queue,but not equal to the last transaction plus 1, it is future
		auto cs = m_currentByAddressAndNonce.find(_transaction.from());
		if (cs != m_currentByAddressAndNonce.end())
		{
			assert_x(cs->second.size());/// 
			auto it = cs->second.rbegin(); ///last nonce
			auto nonce = (*it->second).transaction.nonce();
			/// Future transactions exceed the limit
			if (_transaction.nonce() > nonce + FutureTimeUnknownSize)
				return NonceRange::FutureTooBig;
			if (_transaction.nonce() > nonce + 1)
				return NonceRange::Future;
		}
		else /// if queue have no transaction of this account,need query the nonce of the last transaction in the database
		{
			u256 pNonce = 0;
			mcp::db::db_transaction t(m_store.create_transaction());
			/// exist && and it's not equal to the last transaction plus 1
			m_cache->account_nonce_get(t, _transaction.sender(), pNonce);
			if (_transaction.nonce() > pNonce + FutureTimeUnknownSize)
				return NonceRange::FutureTooBig;
			if (_transaction.nonce() > pNonce + 1)
				return NonceRange::Future;
		}
		return NonceRange::Current;
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

	std::shared_ptr<Transaction> TransactionQueue::get(h256 const& _txHash) const
	{
		UpgradableGuard l(m_lock);

		auto t = m_currentByHash.find(_txHash);
		if (t == m_currentByHash.end())
			return nullptr;

		return std::make_shared<Transaction>((*t->second).transaction);
	}


	void TransactionQueue::enqueue(RLP const& _data, p2p::node_id const& _nodeId)
	{
		bool queued = false;
		{
			Guard l(x_queue);
			if (m_unverified.size() >= c_maxVerificationQueueSize)
			{
				LOG(m_log.info) << "Transaction verification queue is full. Dropping transactions";
				return;
			}
			m_unverified.emplace_back(UnverifiedTransaction(_data.data(), _nodeId));
			queued = true;
		}
		
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
				Transaction t(work.transaction, CheckTransaction::Everything);
				ImportResult ir = import(t,false);
				m_onImport(ir, t.sha3(), work.nodeId);

				if (ImportResult::Success == ir)/// first import && successed,broadcast it
				{
					m_onImportProcessed(t.sha3());
					m_async_task->sync_async([this, t]() {
						m_capability->broadcast_transaction(t);
					});
				}
			}
			catch (...)
			{
				m_onImport(ImportResult::BadChain, h256(0), work.nodeId);///  Notify capability and P2P to process peer. diconnect peer if bad transaction  
				// should not happen as exceptions are handled in import.
				LOG(m_log.error) << "Bad transaction:" << boost::current_exception_diagnostic_information();
			}
		}
	}

	void TransactionQueue::validateTx(Transaction const& _t)
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
	}

	void TransactionQueue::checkTx(Transaction const& _t)
	{
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

	std::string TransactionQueue::getInfo()
	{
		std::string str = "transactionQueue current:" + std::to_string(m_current.size())
			+ " ,currentByHash:" + std::to_string(m_currentByHash.size())
			+ " ,currentByAddressAndNonce:" + std::to_string(m_currentByAddressAndNonce.size())
			+ " ,sameCurrentByAddressAndNonce:" + std::to_string(m_sameCurrentByAddressAndNonce.size())
			+ " ,future:" + std::to_string(m_future.size())
			+ " ,m_unverified:" + std::to_string(m_unverified.size());

		return str;
	}

}
