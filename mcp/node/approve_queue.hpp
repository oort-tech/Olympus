#pragma once
#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <mcp/core/iapprove_queue.hpp>
#include <libdevcore/Guards.h>
#include <libdevcore/LruCache.h>
#include <mcp/common/Exceptions.h>
#include <mcp/common/async_task.hpp>
#include <mcp/node/node_capability.hpp>


namespace mcp
{
	class node_capability;
	class ApproveQueue : public iApproveQueue
	{
	public:
		ApproveQueue(
			mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
			std::shared_ptr<mcp::chain> chain_a, std::shared_ptr<mcp::async_task> async_task_a
		);
		~ApproveQueue();

		/// Add approve to the queue to be verified and imported.
		/// @param _t approve ptr.
		/// @param _nodeId Optional network identified of a node transaction comes from.
		/// @param _in Optional network identified of a node transaction comes from type.
		void enqueue(std::shared_ptr<approve>, p2p::node_id const& _nodeId, source _in);

		void set_capability(std::shared_ptr<mcp::node_capability> capability_a) { m_capability = capability_a; }

		size_t size() { return all.size(); }

		/// Verify and add approve to the queue synchronously.
		/// @param _tx Trasnaction data.
		/// @param _ik Set to Retry to force re-addinga transaction that was previously dropped.
		/// @returns Import result code.
		ImportResult import(std::shared_ptr<approve>, source);

		void importLocal(std::shared_ptr<approve>);
		
		/// Determined approve exist.
		bool exist(h256 const& _hash);

		/// Get a hash set of approves in the queue
		/// @returns A hash set of all approves in the queue
		h256Hash knownApproves() const;

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onImport(std::function<void(ImportResult, p2p::node_id const&)> const& _t) { m_onImport.add(_t); }

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onReady(std::function<void(h256 const&)> const& _t) { m_onReady.add(_t); }

		/// get approve from the queue
		/// @param _txHash approve hash
		std::shared_ptr<approve> get(h256 const& _txHash) const;

		/// Remove approve from the queue
		/// @param _txHash approve hash
		void drop(h256s const& _txHashs);

		/// Get top approves from the queue. Returned approves are not removed from the queue automatically.
		/// @param _limit Max number of approves to return.
		/// @param _avoid approves to avoid returning.
		/// @returns up to _limit approves ordered
		h256s topApproves(unsigned _limit, uint64_t _epoch, h256Hash const& _avoid = h256Hash()) const;
		h256s topApproves(unsigned _limit, h256Hash const& _avoid = h256Hash()) const;

		/// Get transaction queue information
		std::string getInfo();

	private:
		/// Approve pending verification
		struct UnverifiedApprove
		{
			UnverifiedApprove() {}
			UnverifiedApprove(std::shared_ptr<approve> _p, p2p::node_id const& _nodeId, source _in) : ap(_p), nodeId(std::move(_nodeId)), in(std::move(_in)) {}
			UnverifiedApprove(UnverifiedApprove&& _p) : ap(std::move(_p.ap)), nodeId(std::move(_p.nodeId)) {}
			UnverifiedApprove& operator=(UnverifiedApprove&& _other)
			{
				assert(&_other != this);

				ap = std::move(_other.ap);
				nodeId = std::move(_other.nodeId);
				in = std::move(_other.in);
				return *this;
			}

			UnverifiedApprove(UnverifiedApprove const&) = delete;
			UnverifiedApprove& operator=(UnverifiedApprove const&) = delete;

			std::shared_ptr<approve> ap;  ///< approve ptr
			source in;
			p2p::node_id nodeId;	///< Network Id of the peer transaction comes from
		};
		ImportResult check_WITH_LOCK(h256 const& _h);
		ImportResult manageImport_WITH_LOCK(std::shared_ptr<approve>);
		bool remove_WITH_LOCK(h256 const& _txHash);

		void verifierBody();

		ImportResult validateApprove(approve const& _t, source _in);

		mutable SharedMutex m_lock;  ///< General lock.
		h256Hash m_known;            ///< Headers of transactions in both sets.
		
		///< Approve that have previously been dropped. We technically only need to store the tx
		///< hash, but we also store bool as a placeholder value so that we can use an LRU cache to cap
		///< the number of approve hashes stored.
		LruCache<h256, bool> m_dropped;

		std::unordered_map<h256, std::shared_ptr<approve>> all;///approve hash -> approve
		std::map<Epoch, h256Hash> queue;
		
		/// verified broadcast incoming approve
		std::condition_variable m_queueReady;
		Signal<ImportResult, p2p::node_id const&> m_onImport;			///< Called for each import attempt. Arguments are result, transaction id an node id. Be nice and exit fast.
		Signal<h256 const&> m_onReady; ///< First import notification unhandle processing dependency.
		std::vector<std::thread> m_verifiers;
		std::deque<UnverifiedApprove> m_unverified;  ///< Pending verification queue
		mutable Mutex x_queue;                           ///< Verification queue mutex
		std::atomic<bool> m_aborting = { false };          ///< Exit condition for verifier.

		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::shared_ptr<mcp::async_task> m_async_task;
		std::shared_ptr<mcp::chain> m_chain;
		std::shared_ptr<mcp::node_capability> m_capability;

		mcp::log m_log = { mcp::log("node") };
	};
}