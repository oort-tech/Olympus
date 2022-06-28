#pragma once
#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <mcp/core/transaction_queue.hpp>
#include <libdevcore/Guards.h>
#include <libdevcore/LruCache.h>
#include <mcp/node/chain.hpp>
#include <mcp/common/Exceptions.h>
#include <mcp/common/async_task.hpp>
#include <mcp/node/node_capability.hpp>


namespace mcp
{
	enum NonceRange
	{
		Current = 0,
		Future = 1,
		FutureTooBig = 2,
	};

	class chain;
	class node_capability;
	class TransactionQueue : public iTransactionQueue
	{
	public:
		TransactionQueue(
			mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,std::shared_ptr<mcp::chain> chain_a,
			std::shared_ptr<mcp::async_task> async_task_a
		);
		~TransactionQueue();

		/// Add transaction to the queue to be verified and imported.
		/// @param _data RLP encoded transaction data.
		/// @param _nodeId Optional network identified of a node transaction comes from.
		void enqueue(RLP const& _data, p2p::node_id const& _nodeId);

		void set_capability(std::shared_ptr<mcp::node_capability> capability_a) { m_capability = capability_a; }

		size_t size() { return m_currentByAddressAndNonce.size(); }

		/// Verify and add transaction to the queue synchronously.
		/// @param _tx Trasnaction data.
		/// @param _ik Set to Retry to force re-addinga transaction that was previously dropped.
		/// @returns Import result code.
		ImportResult import(Transaction const& _tx, bool isLoccal, bool ignoreFuture = false, IfDropped _ik = IfDropped::Ignore);

		ImportResult importLocal(Transaction const& _tx);

		/// get transaction from the queue
		/// @param _txHash Trasnaction hash
		std::shared_ptr<Transaction> get(h256 const& _txHash) const;

		/// Remove transaction from the queue
		/// @param _txHash Trasnaction hash
		void drop(h256 const& _txHash);

		/// Get top transactions from the queue. Returned transactions are not removed from the queue automatically.
		/// @param _limit Max number of transactions to return.
		/// @param _avoid Transactions to avoid returning.
		/// @returns up to _limit transactions ordered by nonce and gas price.
		/// @returns account A : b2, b3, b4, b5
		///          account B : b1, b2, b3
		///          account c : c2, c3
		/// @returns accounts disorder,but transactions of account is order by nonce
		h256s topTransactions(unsigned _limit, h256Hash const& _avoid = h256Hash()) const;

		/// Determined transaction exist.
		/// @param Address.
		/// @param nonce.
		bool exist(h256 const& _hash);

		/// Get a hash set of transactions in the queue
		/// @returns A hash set of all transactions in the queue
		h256Hash knownTransactions() const;

		/// Get max nonce for an account
		/// @returns Max transaction nonce for account in the queue
		u256 maxNonce(Address const& _a) const;

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onImport(std::function<void(ImportResult, h256 const&, p2p::node_id const&)> const& _t){ m_onImport.add(_t);}

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onImportProcessed(std::function<void(h256 const&)> const& _t) { m_onImportProcessed.add(_t); }

		/// Get transaction queue information
		std::string getInfo();

	private:
		/// Verified and imported transaction
		struct VerifiedTransaction
		{
			VerifiedTransaction(Transaction const& _t) : transaction(_t) {}
			VerifiedTransaction(VerifiedTransaction&& _t) : transaction(std::move(_t.transaction)) {}

			VerifiedTransaction(VerifiedTransaction const&) = delete;
			VerifiedTransaction& operator=(VerifiedTransaction const&) = delete;

			Transaction transaction;  ///< Transaction data
		};

		/// Transaction pending verification
		struct UnverifiedTransaction
		{
			UnverifiedTransaction() {}
			UnverifiedTransaction(bytesConstRef const& _t, p2p::node_id const& _nodeId) : transaction(_t.toBytes()), nodeId(_nodeId) {}
			UnverifiedTransaction(UnverifiedTransaction&& _t) : transaction(std::move(_t.transaction)), nodeId(std::move(_t.nodeId)) {}
			UnverifiedTransaction& operator=(UnverifiedTransaction&& _other)
			{
				assert(&_other != this);

				transaction = std::move(_other.transaction);
				nodeId = std::move(_other.nodeId);
				return *this;
			}

			UnverifiedTransaction(UnverifiedTransaction const&) = delete;
			UnverifiedTransaction& operator=(UnverifiedTransaction const&) = delete;

			bytes transaction;  ///< RLP encoded transaction data
			p2p::node_id nodeId;	///< Network Id of the peer transaction comes from
		};

		struct PriorityCompare
		{
			TransactionQueue& queue;
			/// Compare transaction by nonce height and gas price.
			bool operator()(VerifiedTransaction const& _first, VerifiedTransaction const& _second) const
			{
				u256 const& height1 = _first.transaction.nonce() - queue.m_currentByAddressAndNonce[_first.transaction.sender()].begin()->first;
				u256 const& height2 = _second.transaction.nonce() - queue.m_currentByAddressAndNonce[_second.transaction.sender()].begin()->first;
				return height1 < height2 || (height1 == height2 && _first.transaction.gasPrice() > _second.transaction.gasPrice());
			}
		};

		// Use a set with dynamic comparator for minmax priority queue. The comparator takes into account min account nonce. Updating it does not affect the order.
		using PriorityQueue = std::multiset<VerifiedTransaction, PriorityCompare>;

		ImportResult check_WITH_LOCK(h256 const& _h, IfDropped _ik);
		ImportResult manageImport_WITH_LOCK(h256 const& _h, Transaction const& _transaction, bool isLocal, bool ignoreFuture);

		ImportResult insertCurrent_WITH_LOCK(std::pair<h256, Transaction> const& _p, bool isLocal);
		ImportResult insertFuture_WITH_LOCK(std::pair<h256, Transaction> const& _p, bool isLocal);
		void makeCurrent_WITH_LOCK(Transaction const& _t);
		bool remove_WITH_LOCK(h256 const& _txHash);
		u256 maxNonce_WITH_LOCK(Address const& _a) const;
		NonceRange isFuture_WITH_LOCK(Transaction const& _transaction);
		void verifierBody();

		void validateTx(Transaction const& _t);/// Base format check
		void checkTx(Transaction const& _t);/// nonce and balance check

		mutable SharedMutex m_lock;  ///< General lock.
		h256Hash m_known;            ///< Headers of transactions in both sets.
		
		///< Transactions that have previously been dropped. We technically only need to store the tx
		///< hash, but we also store bool as a placeholder value so that we can use an LRU cache to cap
		///< the number of transaction hashes stored.
		LruCache<h256, bool> m_dropped;

		PriorityQueue m_current;
		std::unordered_map<h256, PriorityQueue::iterator> m_currentByHash;			///< Transaction hash to set ref
		std::unordered_map<Address, std::map<u256, PriorityQueue::iterator>> m_currentByAddressAndNonce; ///< Transactions grouped by account and nonce
		std::unordered_map<Address, std::map<u256, VerifiedTransaction>> m_future;	/// Future transactions

		std::unordered_map<Address, std::map<u256, std::map<h256, PriorityQueue::iterator>>> m_sameCurrentByAddressAndNonce; ///< Transactions that exist in parallel, transaction maybe have same from and nonce,but have different hash. 

		unsigned m_futureLimit;														///< Max number of future transactions
		unsigned m_futureSize = 0;													///< Current number of future transactions

		/// verified broadcast incoming transaction
		std::condition_variable m_queueReady;
		Signal<ImportResult, h256 const&, p2p::node_id const&> m_onImport;			///< Called for each import attempt. Arguments are result, transaction id an node id. Be nice and exit fast.
		Signal<h256 const&> m_onImportProcessed; ///< First import notification unhandle processing dependency.
		std::vector<std::thread> m_verifiers;
		std::deque<UnverifiedTransaction> m_unverified;  ///< Pending verification queue
		mutable Mutex x_queue;                           ///< Verification queue mutex
		std::atomic<bool> m_aborting = { false };          ///< Exit condition for verifier.

		// recieve count
		std::atomic<uint64_t> receive_transaction_count = { 0 };

		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::shared_ptr<mcp::chain> m_chain;
		std::shared_ptr<mcp::async_task> m_async_task;
		std::shared_ptr<mcp::node_capability> m_capability;

		mcp::log m_log = { mcp::log("node") };
	};
}