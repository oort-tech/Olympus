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
		TooSmall = 0, ///Less than the account stable nonce, cannot be linked.
		Queue = 1,///Eq next nonce.
		Pending = 2,///larger than the largest nonce in the queue, there are nonce smaller transactions missing.
		TooBig = 3,///Much larger than the largest nonce in the queue.
	};

	class chain;
	class node_capability;
	class TransactionQueue : public iTransactionQueue
	{
	public:
		TransactionQueue(
			boost::asio::io_service& io_service_a, mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,std::shared_ptr<mcp::chain> chain_a,
			std::shared_ptr<mcp::async_task> async_task_a
		);
		~TransactionQueue();

		/// Add transaction to the queue to be verified and imported.
		/// @param _t transaction ptr.
		/// @param _nodeId Optional network identified of a node transaction comes from.
		/// @param _in Optional network identified of a node transaction comes from type.
		void enqueue(std::shared_ptr<Transaction>, p2p::node_id const&, source);

		void set_capability(std::shared_ptr<mcp::node_capability> capability_a) { m_capability = capability_a; }

		size_t size() { return queue.size(); }

		/// Verify and add transaction to the queue synchronously.
		/// @param _tx Trasnaction data.
		/// @param _in transaction from(broadcast,sync,request).
		/// @returns Import result code.
		ImportResult import(std::shared_ptr<Transaction>, source);

		ImportResult importLocal(std::shared_ptr<Transaction> _tx);

		/// get transaction from the queue
		/// @param _txHash Trasnaction hash
		std::shared_ptr<Transaction> get(h256 const& _txHash) const;

		/// Remove transaction from the queue
		/// @param _txHash Trasnaction hashs
		void drop(h256s const& _txHash);

		/// Get top transactions from the queue. Returned transactions are not removed from the queue automatically.
		/// @param _limit Max number of transactions to return.
		/// @returns up to _limit transactions ordered by nonce and gas price.
		/// @returns account A : b2, b3, b4, b5
		///          account B : b1, b2, b3
		///          account c : c2, c3
		/// @returns accounts disorder,but transactions of account is order by nonce
		h256s topTransactions(unsigned _limit) const;

		/// Determined transaction exist.
		/// @param Address.
		/// @param nonce.
		bool exist(h256 const& _hash);

		/// Get a hash set of transactions in the queue
		/// @returns A hash set of all transactions in the queue
		h256Hash knownTransactions() const;

		/// Get max nonce for an account
		/// @returns Max transaction nonce for account in the queue
		u256 maxNonce(Address const& _a, BlockNumber const blockTag = PendingBlock) const;

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onImport(std::function<void(ImportResult, p2p::node_id const&)> const& _t){ m_onImport.add(_t);}

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onReady(std::function<void(h256 const&)> const& _t) { m_onReady.add(_t); }

		/// Get transaction queue information
		std::string getInfo();

		void prinf();

	private:
		/// Verified and imported transaction
		struct txList
		{
			txList(){}
			/// returning whether the transaction was accepted, and if yes, any previous transaction it replaced. 
			/// return true if insert success. return true and replaced transaction if replaced a transaction. 
			std::pair<bool, std::shared_ptr<Transaction>> add(std::shared_ptr<Transaction> _t);
			///release all consecutive and compliant transactions
			std::deque<std::shared_ptr<Transaction>> release(u256 const& nonce);
			///Deletes the transaction for the specified nonce and returns the transaction 
			std::shared_ptr<Transaction> erase(u256 const& _n); 
			h256s transactionHashs(int _limit) const;
			/// return txs max nonce
			u256 maxNonce() const { assert_x(txs.size()); return txs.rbegin()->first; }
			u256 minNonce() const { assert_x(txs.size()); return txs.begin()->first; }
			int size() { return txs.size(); }
			bool empty() { return txs.empty(); }
			std::map<u256, std::shared_ptr<Transaction>> txs;
		};
		txList newTxList() { return txList(); }

		/// Transaction pending verification
		struct UnverifiedTransaction
		{
			UnverifiedTransaction() {}
			UnverifiedTransaction(std::shared_ptr<Transaction> _t, p2p::node_id const& _nodeId, source _in) : transaction(_t), nodeId(_nodeId), in(_in) {}
			UnverifiedTransaction(UnverifiedTransaction&& _t) : transaction(_t.transaction), nodeId(std::move(_t.nodeId)), in(std::move(_t.in)) {}
			UnverifiedTransaction& operator=(UnverifiedTransaction&& _other)
			{
				assert(&_other != this);

				transaction = std::move(_other.transaction);
				nodeId = std::move(_other.nodeId);
				in = std::move(_other.in);
				return *this;
			}

			UnverifiedTransaction(UnverifiedTransaction const&) = delete;
			UnverifiedTransaction& operator=(UnverifiedTransaction const&) = delete;

			std::shared_ptr<Transaction> transaction;  ///< Transaction data
			source in;
			p2p::node_id nodeId;	///< Network Id of the peer transaction comes from
		};

		ImportResult check_WITH_LOCK(h256 const& _h);
		ImportResult manageImport_WITH_LOCK(std::shared_ptr<Transaction> _t, source _in);

		ImportResult insertQueue_WITH_LOCK(std::shared_ptr<Transaction> _t, bool includeQueue = true);
		ImportResult insertPending_WITH_LOCK(std::shared_ptr<Transaction>);
		void makeQueue_WITH_LOCK(std::shared_ptr<Transaction> _t);
		bool remove_WITH_LOCK(h256 const& _txHash);
		u256 maxNonce_WITH_LOCK(Address const& _a, BlockNumber const blockTag = PendingBlock) const;
		NonceRange isPending_WITH_LOCK(std::shared_ptr<Transaction>);
		void verifierBody();

		void validateTx(std::shared_ptr<Transaction>);/// Base format check
		void checkTx(Transaction const& _t);/// nonce and balance check

		void processSuperfluous();

		mutable SharedMutex m_lock;  ///< General lock.
		h256Hash m_known;            ///< Headers of transactions in both sets.
		
		///< Transactions that have previously been dropped. We technically only need to store the tx
		///< hash, but we also store bool as a placeholder value so that we can use an LRU cache to cap
		///< the number of transaction hashes stored.
		LruCache<h256, bool> m_dropped;

		std::unordered_map<h256, std::shared_ptr<Transaction>> all;///All transactions to allow lookups
		std::unordered_map<Address, txList> queue;///< ready Transactions grouped by account and nonce
		std::unordered_map<Address, txList> pending;///< pending Transactions grouped by account and nonce,there are nonce smaller transactions missing its nonce.

		unsigned m_pendingLimit;													///< Max number of pending transactions
		unsigned m_pendingSize = 0;													///< number of pending transactions

		/// verified broadcast incoming transaction
		std::condition_variable m_queueReady;
		Signal<ImportResult, p2p::node_id const&> m_onImport;			///< Called for each import attempt. Arguments are result, transaction id an node id. Be nice and exit fast.
		Signal<h256 const&> m_onReady; ///<  Called when a subsequent call to import transactions and ready.
		std::vector<std::thread> m_verifiers;
		std::deque<UnverifiedTransaction> m_unverified;  ///< Pending verification queue
		mutable Mutex x_queue;                           ///< Verification queue mutex
		std::atomic<bool> m_aborting = { false };          ///< Exit condition for verifier.

		///clear superfluous transaction
		std::map<std::chrono::steady_clock::time_point, h256Set> m_superfluous;
		std::unique_ptr<boost::asio::deadline_timer> m_clearTimer;
		std::thread m_processSuperfluousThread;
		std::chrono::minutes m_clear_time = std::chrono::minutes(1);

		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::shared_ptr<mcp::chain> m_chain;
		std::shared_ptr<mcp::async_task> m_async_task;
		std::shared_ptr<mcp::node_capability> m_capability;

		mcp::log m_log = { mcp::log("node") };
	};
}