#include "transaction_queue.hpp"
#include <thread>

namespace mcp
{
	

	using namespace std;
	using namespace dev;

	constexpr size_t c_maxAccountPendingSize = 64;
	constexpr size_t c_maxVerificationQueueSize = 40960;
	constexpr size_t c_maxDroppedTransactionCount = 100000;
	constexpr size_t c_maxPendingTransactionCount = 100000;
	constexpr size_t c_maxReadyTransactionCount = 100000;

	TransactionQueue::TransactionQueue(
		boost::asio::io_service& io_service_a, mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::chain> chain_a,
		std::shared_ptr<mcp::async_task> async_task_a
	):
		m_store(store_a),
		m_cache(cache_a),
		m_chain(chain_a),
		m_async_task(async_task_a),
		m_dropped(c_maxDroppedTransactionCount),
		m_pendingLimit(c_maxPendingTransactionCount)
	{
		unsigned verifierThreads = std::max(thread::hardware_concurrency(), 3U) - 2U;
		for (unsigned i = 0; i < verifierThreads; ++i)
			m_verifiers.emplace_back([=]() {
			setThreadName("txcheck" + toString(i));
			this->verifierBody();
		});

		m_clearTimer = std::make_unique<ba::deadline_timer>(io_service_a);
		m_processSuperfluousThread = std::thread([this]() { this->processSuperfluous(); });
	}

	TransactionQueue::~TransactionQueue()
	{
		DEV_GUARDED(x_queue)
			m_aborting = true;
		m_queueReady.notify_all();
		for (auto& i : m_verifiers)
			i.join();

		if (m_clearTimer)
			m_clearTimer->cancel();
	}

	ImportResult TransactionQueue::check_WITH_LOCK(h256 const& _h)
	{
		if (m_known.count(_h) )
			return ImportResult::AlreadyKnown;

		if (m_dropped.contains(_h))
			return ImportResult::AlreadyInChain;
		mcp::db::db_transaction t(m_store.create_transaction());
		if (m_cache->transaction_exists(t, _h))
			return ImportResult::AlreadyInChain;

		return ImportResult::Success;
	}

	ImportResult TransactionQueue::import(std::shared_ptr<Transaction> _transaction, source _in)
	{
		validateTx(_transaction);
		// Check if we already know this transaction.
		h256 h = _transaction->sha3();
		//LOG(m_log.debug) << "import transaction:" << h.hex();

		ImportResult ret;
		{
			UpgradableGuard l(m_lock);
			auto ir = check_WITH_LOCK(h);
			if (ir != ImportResult::Success)
				return ir;

			_transaction->sender();
			if (_in != source::request && _in != source::sync)///block lined,do not checkout balance and nonce
				checkTx(*_transaction); ///check balance and nonce
			UpgradeGuard ul(l);
			ret = manageImport_WITH_LOCK(_transaction, _in);

			//LOG(m_log.debug) << "import.......";
			//prinf();
		}

		if (ImportResult::Success == ret && _in != source::sync)/// first import && successed,broadcast it
		{
			m_async_task->sync_async([this, _transaction]() {
				m_capability->broadcast_transaction(*_transaction);
			});
		}
		return ret;
	}

	ImportResult TransactionQueue::importLocal(std::shared_ptr<Transaction> _transaction)
	{
		return import(_transaction, source::local);
	}

	h256s TransactionQueue::topTransactions(unsigned _limit) const
	{
		/// todo: link accounts randomly and limit the maximum number of transactions in a single block links 
		ReadGuard l(m_lock);
		h256s ret;
		for (auto cs = queue.begin(); cs != queue.end(); ++cs)
		{
			auto r = cs->second.transactionHashs(_limit - ret.size());
			ret.insert(ret.end(), r.begin(), r.end());
			if (ret.size() == _limit)
				break;
		}
		return ret;
	}

	bool TransactionQueue::exist(h256 const& _hash)
	{
		ReadGuard l(m_lock);
		return all.count(_hash);
	}

	h256Hash TransactionQueue::knownTransactions() const
	{
		ReadGuard l(m_lock);
		return m_known;
	}


	ImportResult TransactionQueue::manageImport_WITH_LOCK(std::shared_ptr<Transaction> _t, source _in)
	{
		try
		{
			/// If valid, append to transactions.
			auto r = isPending_WITH_LOCK(_t);
			if (NonceRange::TooSmall == r)///nonce too low, just request need insert.
			{
				if (_in == source::request)
					return insertQueue_WITH_LOCK(_t, false);
				else
					return ImportResult::InvalidNonce;
			}
			if (NonceRange::TooBig == r)
			{
				if (_in == source::request)/// block liked.
					r = NonceRange::Pending;
				else
					return ImportResult::FutureFull; ///account's transaction is full,the maximum cache size is c_maxAccountPendingSize.
			}
			if (NonceRange::Pending == r)/// insert to pending
			{
				//LOG(m_log.debug) << "Queued vaguely not legit-looking transaction " << _t->sha3().hex();
				return insertPending_WITH_LOCK(_t);
			}
			else ///insert to queue
			{
				//LOG(m_log.debug) << "Queued vaguely legit-looking transaction " << _t->sha3().hex();
				return insertQueue_WITH_LOCK(_t);
			}
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
	}

	u256 TransactionQueue::maxNonce(Address const& _a, BlockNumber const blockTag) const
	{
		ReadGuard l(m_lock);
		return maxNonce_WITH_LOCK(_a, blockTag);
	}

	/// return account's next nonce
	u256 TransactionQueue::maxNonce_WITH_LOCK(Address const& _a, BlockNumber const blockTag) const
	{
		u256 ret = 0;
		bool tqExist = false;
		if (blockTag == PendingBlock || blockTag == LatestBlock)
		{
			auto cs = queue.find(_a);
			if (cs != queue.end())
			{
				tqExist = true;
				ret = cs->second.maxNonce() + 1; ///next nonce
			}

			if (blockTag == PendingBlock) {
				auto fs = pending.find(_a);
				if (fs != pending.end())
				{
					tqExist = true;
					ret = std::max(ret, fs->second.maxNonce() + 1);
				}
			}
		}

		///stable nonce
		if (!tqExist)
		{
			mcp::db::db_transaction t(m_store.create_transaction());
			if (m_cache->account_nonce_get(t, _a, ret))
				ret++;
		}
		return ret;
	}


	ImportResult TransactionQueue::insertQueue_WITH_LOCK(std::shared_ptr<Transaction> _t, bool includeQueue)
	{
		if (all.count(_t->sha3()))
			assert_x(false);
		
		///If the nonce is too small, it is not inserted into the queue.just insert all, must be linked by a block.
		if (includeQueue)
		{
			auto from = _t->sender();
			if (!queue.count(from)) 
				queue[_t->sender()] = newTxList();
			auto r = queue[_t->sender()].add(_t);/// have transaction used nonce,replace it
			if (!r.first)///OverbidGasPrice
				return ImportResult::OverbidGasPrice;
			if (r.second)///replaced. remove replaced transaction from known and all.
			{
				all.erase(r.second->sha3());
				m_known.erase(r.second->sha3());
			}
		}
		
		all[_t->sha3()] = _t;
		m_known.insert(_t->sha3());
		m_onReady(_t->sha3());
		/// Move following transactions from pending to queue
		makeQueue_WITH_LOCK(_t);
		
		return ImportResult::Success;
	}

	ImportResult TransactionQueue::insertPending_WITH_LOCK(std::shared_ptr<Transaction> _t)
	{
		if (all.count(_t->sha3()))
			assert_x(false);

		/// find from pending. if nonce exist and it is not the same transaction,try to replaced it.
		if (!pending.count(_t->sender())) 
			pending[_t->sender()] = newTxList();
		auto r = pending[_t->sender()].add(_t);/// have transaction used nonce,replace it
		if (!r.first)///OverbidGasPrice
			return ImportResult::OverbidGasPrice;
		if (r.second)///replaced. remove replaced transaction from known.
			m_known.erase(r.second->sha3());
		else///not replaced, jsut insert.
			++m_pendingSize;
		m_known.insert(_t->sha3());

		/// exceed the maximum limit, half of the pending will be deleted, delete from each account in turn, from back to front.
		/// TODO: priority queue for future transactions
		if (m_pendingSize > m_pendingLimit)
		{
			while (m_pendingSize > m_pendingLimit / 2)
			{
				auto txl = pending.begin()->second;
				m_pendingSize -= txl.size();
				LOG(m_log.debug) << "Dropping out of bounds account transaction "
					<< pending.begin()->first.hex();
				for (auto t : txl.txs)
					m_known.erase(t.second->sha3());
				pending.erase(pending.begin());
			}
		}

		return ImportResult::Success;
	}

	bool TransactionQueue::remove_WITH_LOCK(h256 const& _txHash)
	{
		auto t = all.find(_txHash);
		if (t == all.end())
		{
			LOG(m_log.debug) << "remove_WITH_LOCK Transaction hash" << _txHash.hex() << "already in all?!";
			return false;
		}
			
		Address from = t->second->sender();
		u256 nonce = t->second->nonce();

		if (queue.count(from))
		{
			auto delt = queue[from].erase(nonce);
			if (delt->sha3() != _txHash)/// not the hash,but deleted from queue,put it to delete queue,delete it 2 minutes later
			{
				auto now = SteadyClock.now();
				if (!m_superfluous.count(now))
					m_superfluous.emplace(now, h256Set());
				m_superfluous[now].emplace(delt->sha3());
			}
			if (queue[from].empty())
				queue.erase(from);
		}
		all.erase(_txHash);
		m_known.erase(_txHash);

		//LOG(m_log.debug) << "remove.......";
		//prinf();
		
		return true;
	}


	void TransactionQueue::makeQueue_WITH_LOCK(std::shared_ptr<Transaction> _t)
	{
		if (pending.count(_t->from()))
		{
			auto cur = pending[_t->from()].release(_t->nonce() + 1);
			if (cur.size()) ///release all consecutive and compliant transactions from pending.
			{
				if (!pending[_t->from()].size()) ///if have no transactions in pending delete it.
					pending.erase(_t->from());
				m_pendingSize -= cur.size();
				for (auto td : cur) ///move to queue
				{
					queue[_t->sender()].add(td);
					all[td->sha3()] = td;
					m_onReady(td->sha3());
				}
			}
		}
	}

	NonceRange TransactionQueue::isPending_WITH_LOCK(std::shared_ptr<Transaction> _t)
	{
		/// account's first transaction
		if (_t->nonce() == 0)
			return NonceRange::Queue;

		/// not include pending transactions,larger than the largest nonce in the queue is the pending.
		u256 next = maxNonce_WITH_LOCK(_t->sender(), LatestBlock);
		if (_t->nonce() < next)
			return NonceRange::TooSmall;
		if (_t->nonce() > next)
		{
			if (_t->nonce() > next + c_maxAccountPendingSize - 1)
				return NonceRange::TooBig;
			return NonceRange::Pending;
		}
		return NonceRange::Queue;
	}

	void TransactionQueue::drop(h256s const& _txHashs)
	{
		UpgradableGuard l(m_lock);
		h256s dels;
		for (auto h : _txHashs)
		{
			//LOG(m_log.info) << "drop transaction,hash: " << h.hexPrefixed();

			///if not known,must be deleted
			if (m_known.count(h))
			{
				dels.push_back(h);
			}
		}

		UpgradeGuard ul(l);
		for (auto h : dels)
		{
			m_dropped.insert(h, true /* placeholder value */);
			remove_WITH_LOCK(h);
		}
	}

	std::shared_ptr<Transaction> TransactionQueue::get(h256 const& _txHash) const
	{
		UpgradableGuard l(m_lock);

		auto t = all.find(_txHash);
		if (t == all.end())
			return nullptr;

		return t->second;
	}


	void TransactionQueue::enqueue(std::shared_ptr<Transaction> _tx, p2p::node_id const& _nodeId, source _in)
	{
		bool queued = false;
		{
			Guard l(x_queue);
			if (m_unverified.size() >= c_maxVerificationQueueSize && source::broadcast == _in)
			{
				LOG(m_log.debug) << "Transaction verification queue is full. Dropping transactions";
				return;
			}
			m_unverified.emplace_back(UnverifiedTransaction(_tx, _nodeId, _in));
			queued = true;
		}
		if (queued)
			m_queueReady.notify_all();
	}

	void TransactionQueue::verifierBody()
	{
		while (!m_aborting)
		{
			std::deque<UnverifiedTransaction> works;

			{
				unique_lock<Mutex> l(x_queue);
				m_queueReady.wait(l, [&]() { return !m_unverified.empty() || m_aborting; });
				if (m_aborting)
					return;
				std::swap(works, m_unverified);
			}

			while (!works.empty())
			{
				UnverifiedTransaction work = std::move(works.front());
				works.pop_front();
				try
				{
					if (all.size() > c_maxReadyTransactionCount/2 && work.in == source::broadcast)///only process request or sync transactions.
					{
						continue;
					}
					auto ir = import(work.transaction, work.in);
					m_onImport(ir, work.nodeId);
				}
				catch (InvalidNonce)
				{
					///remote network is not very good,Disconnect the peer.todo
				}
				catch (NotEnoughCash)
				{
					///remote network is not very good,Disconnect the peer.todo
				}
				catch (...)
				{
					LOG(m_log.error) << "verifierBody Bad transaction:" << boost::current_exception_diagnostic_information();
					m_onImport(ImportResult::Malformed, work.nodeId);///  Notify capability and P2P to process peer. diconnect peer if bad transaction  
				}
			}
		}
	}

	void TransactionQueue::validateTx(std::shared_ptr<Transaction> _t)
	{
		if (_t->hasZeroSignature())
			BOOST_THROW_EXCEPTION(ZeroSignatureTransaction());
		_t->checkChainId(mcp::chain_id);
		_t->checkLowS();

		eth::EVMSchedule const& schedule = dev::eth::EVMSchedule();
		/// Pre calculate the gas needed for execution
		if (_t->baseGasRequired(schedule) > _t->gas())
			BOOST_THROW_EXCEPTION(OutOfGasIntrinsic() << RequirementError(
			(bigint)(_t->baseGasRequired(schedule)), (bigint)_t->gas()));

		/// Avoid transactions that would take us beyond the block gas limit.
		if ((uint256_t)_t->gas() > mcp::uint256_t(mcp::tx_max_gas))
			BOOST_THROW_EXCEPTION(BlockGasLimitReached() << RequirementErrorComment(
			(bigint)(mcp::tx_max_gas), (bigint)_t->gas(),
				std::string("_gasUsed + (bigint)_t.gas() > _header.gasLimit()")));

		/// Avoid transactions that are less than the lower gas price limit.
		if ((uint256_t)_t->gasPrice() < mcp::uint256_t(mcp::gas_price))
			BOOST_THROW_EXCEPTION(OutOfGasPriceIntrinsic() << RequirementErrorComment(
			(bigint)(mcp::tx_max_gas), (bigint)_t->gas(),
				std::string("_gasUsed + (bigint)_t.gas() < lower.gasLimit()")));
		
		//if ((uint256_t)_t->gas()*(uint256_t)_t->gasPrice() > mcp::uint256_t(tx_max_gas_fee))
		//	BOOST_THROW_EXCEPTION(BlockGasLimitReached() << RequirementErrorComment(
		//	(bigint)(mcp::tx_max_gas), (bigint)_t->gas(),
		//		std::string("_t->gas() * t->gasPrice() > tx_max_gas_fee")));
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

	void TransactionQueue::processSuperfluous()
	{
		{
			UpgradableGuard l(m_lock);
			if (m_superfluous.size())
			{
				auto now = SteadyClock.now();
				UpgradeGuard ul(l);
				auto ft = m_superfluous.begin();
				while (ft != m_superfluous.end())
				{
					if (now - ft->first < m_clear_time)
						break;
					for (auto h : ft->second)
					{
						all.erase(h);
						m_known.erase(h);
					}
					ft++;
				}
				if (ft != m_superfluous.begin())
				{
					m_superfluous.erase(m_superfluous.begin(), ft);
				}
			}
			//LOG(m_log.debug) << "processSuperfluous.......";
			//prinf();
		}

		m_clearTimer->expires_from_now(boost::posix_time::seconds(180));
		m_clearTimer->async_wait([this](boost::system::error_code const & error)
		{
			processSuperfluous();
		});
	}

	std::string TransactionQueue::getInfo()
	{
		int allSize = 0;
		int queueSize = 0;
		int pendingSize = 0;
		int queueAccountSize = 0;
		int pendingAccountSize = 0;
		int knownSize = 0;
		int dropSize = 0;
		{
			ReadGuard l(m_lock);
			allSize = all.size();
			queueAccountSize = queue.size();
			pendingAccountSize = pending.size();
			knownSize = m_known.size();
			dropSize = m_dropped.size();
			for (auto it = queue.begin(); it != queue.end(); it++)
			{
				queueSize += it->second.size();
			}
			for (auto it = pending.begin(); it != pending.end(); it++)
			{
				pendingSize += it->second.size();
			}
		}

		std::string str = "transactionQueue all txs:" + std::to_string(allSize)
			+ " ,queue  account:" + std::to_string(queueAccountSize)
			+ " ,pending account:" + std::to_string(pendingAccountSize)
			+ " ,queue txs:" + std::to_string(queueSize)
			+ " ,pending txs:" + std::to_string(pendingSize)
			+ " ,m_known:" + std::to_string(knownSize)
			+ " ,m_dropped:" + std::to_string(dropSize)
			+ " ,m_pendingSize:" + std::to_string(m_pendingSize)
			;

		return str;
	}

	void TransactionQueue::prinf()
	{
		LOG(m_log.debug) << getInfo();
		LOG(m_log.debug) << "--------------all-------------";
		for (auto it = all.begin(); it != all.end(); it++)
		{
			LOG(m_log.debug) << it->first.hex() << " ,nonce:" << it->second->nonce();
		}
		LOG(m_log.debug) << "--------------all end-------------";

		LOG(m_log.debug) << "--------------queue-------------";
		for (auto it = queue.begin(); it != queue.end(); it++)
		{
			LOG(m_log.debug) << "address:" << it->first.hex();
			auto m = it->second;
			for (auto at = m.txs.begin(); at != m.txs.end(); at++)
			{
				LOG(m_log.debug) << "--nonce:" << at->first << " ,hash:" << at->second->sha3().hex();
			}
		}
		LOG(m_log.debug) << "--------------queue end-------------";

		LOG(m_log.debug) << "--------------pending-------------";
		for (auto it = pending.begin(); it != pending.end(); it++)
		{
			LOG(m_log.debug) << "address:" << it->first.hex();
			auto m = it->second;
			for (auto at = m.txs.begin(); at != m.txs.end(); at++)
			{
				LOG(m_log.debug) << "--nonce:" << at->first << " ,hash:" << at->second->sha3().hex();
			}
		}
		LOG(m_log.debug) << "--------------pending end-------------";

		LOG(m_log.debug) << "--------------known-------------";
		for (auto it = m_known.begin(); it != m_known.end(); it++)
		{
			LOG(m_log.debug) << "hash:" << (*it).hex();
		}
		LOG(m_log.debug) << "--------------known end-------------";
		LOG(m_log.debug) << "--------------superfluous-------------";
		for (auto it = m_superfluous.begin(); it != m_superfluous.end(); it++)
		{
			for (auto h : it->second)
			{
				LOG(m_log.debug) << "hash:" << h.hex();
			}
		}
		LOG(m_log.debug) << "--------------superfluous end-------------";
		LOG(m_log.debug) << "-------------------------------------------------------";
	}

	std::pair<bool, std::shared_ptr<Transaction>> TransactionQueue::txList::add(std::shared_ptr<Transaction> _t)
	{
		if (txs.count(_t->nonce()))
		{
			auto old = txs[_t->nonce()];
			if (_t->gasPrice() <= old->gasPrice())
			{
				return std::make_pair(false, nullptr);
			}
			/// replace
			txs[_t->nonce()] = _t;
			return std::make_pair(true, old);
		}
		///insert
		txs[_t->nonce()] = _t;
		return std::make_pair(true, nullptr);
	}

	std::deque<std::shared_ptr<Transaction>> TransactionQueue::txList::release(u256 const& nonce)
	{
		std::deque<std::shared_ptr<Transaction>> ret;
		u256 _n = nonce;
		auto ft = txs.find(_n);
		if (ft != txs.end())
		{
			while (ft != txs.end() && ft->second->nonce() == _n)
			{
				ret.push_back(ft->second);
				++ft;
				++_n;
			}
			if (ret.size())
				txs.erase(txs.begin(), ft);
		}

		return ret;
	}

	std::shared_ptr<Transaction> TransactionQueue::txList::erase(u256 const & _n)
	{
		std::shared_ptr<Transaction> t = nullptr;
		if (txs.count(_n))
		{
			t = txs[_n];
			txs.erase(_n);
		}
		return t;
	}

	h256s TransactionQueue::txList::transactionHashs(int _limit) const
	{
		h256s ret;
		auto it = txs.begin();
		while (it != txs.end() && ret.size() < _limit)
		{
			ret.push_back(it->second->sha3());
			it++;
		}
		return ret;
	}

}
