#pragma once
#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <mcp/core/iapprove_queue.hpp>
#include <libdevcore/Guards.h>
#include <libdevcore/LruCache.h>
#include <mcp/node/chain.hpp>
#include <mcp/common/Exceptions.h>
#include <mcp/common/async_task.hpp>
#include <mcp/node/node_capability.hpp>


namespace mcp
{
	class chain;
	class node_capability;
	class ApproveQueue : public iApproveQueue
	{
	public:
		ApproveQueue(
			mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::chain> chain_a,
			std::shared_ptr<mcp::async_task> async_task_a
		);
		~ApproveQueue();

		/// Add approve to the queue to be verified and imported.
		/// @param _data RLP encoded transaction data.
		/// @param _nodeId Optional network identified of a node transaction comes from.
		void enqueue(RLP const& _data, p2p::node_id const& _nodeId);

		void set_capability(std::shared_ptr<mcp::node_capability> capability_a) { m_capability = capability_a; }

		size_t size(uint64_t _epoch);

		/// Verify and add approve to the queue synchronously.
		/// @param _tx Trasnaction data.
		/// @param _ik Set to Retry to force re-addinga transaction that was previously dropped.
		/// @returns Import result code.
		ImportApproveResult import(approve const& _tx);

		void importLocal(approve const& _tx);
		
		/// Determined approve exist.
		bool exist(h256 const& _hash);

		/// Get a hash set of approves in the queue
		/// @returns A hash set of all approves in the queue
		h256Hash knownApproves() const;

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onImport(std::function<void(ImportApproveResult, h256 const&, p2p::node_id const&)> const& _t){ m_onImport.add(_t);}

		/// Register a handler that will be called once asynchronous verification is comeplte an transaction has been imported
		void onImportProcessed(std::function<void(h256 const&)> const& _t) { m_onImportProcessed.add(_t); }

		/// get approve from the queue
		/// @param _txHash approve hash
		std::shared_ptr<approve> get(h256 const& _txHash, uint64_t _epoch) const;
		std::shared_ptr<approve> get(h256 const& _txHash) const;

		/// Remove approve from the queue
		/// @param _txHash approve hash
		void drop(std::map<uint64_t, h256s> const& _txHashs);

		/// Remove approves from the queue 
		/// @param _epoch Remove approves less than _cur_epoch
		void dropObsolete(uint64_t _cur_epoch);

		/// Get top approves from the queue. Returned approves are not removed from the queue automatically.
		/// @param _limit Max number of approves to return.
		/// @param _avoid approves to avoid returning.
		/// @returns up to _limit approves ordered
		h256s topApproves(unsigned _limit, uint64_t _epoch, h256Hash const& _avoid = h256Hash()) const;
		h256s topApproves(unsigned _limit, h256Hash const& _avoid = h256Hash()) const;

		/// Get transaction queue information
		std::string getInfo();

	private:
		/// Verified and imported approve
		struct VerifiedApprove
		{
			VerifiedApprove(approve const& _t) : m_approve(_t) {}
			VerifiedApprove(VerifiedApprove&& _t) : m_approve(std::move(_t.m_approve)) {}

			VerifiedApprove(VerifiedApprove const&) = delete;
			VerifiedApprove& operator=(VerifiedApprove const&) = delete;

			approve m_approve;  ///< approve data
		};

		/// Approve pending verification
		struct UnverifiedApprove
		{
			UnverifiedApprove() {}
			UnverifiedApprove(bytesConstRef const& _t, p2p::node_id const& _nodeId) : m_approve(_t.toBytes()), nodeId(_nodeId) {}
			UnverifiedApprove(UnverifiedApprove&& _t) : m_approve(std::move(_t.m_approve)), nodeId(std::move(_t.nodeId)) {}
			UnverifiedApprove& operator=(UnverifiedApprove&& _other)
			{
				assert(&_other != this);

				m_approve = std::move(_other.m_approve);
				nodeId = std::move(_other.nodeId);
				return *this;
			}

			UnverifiedApprove(UnverifiedApprove const&) = delete;
			UnverifiedApprove& operator=(UnverifiedApprove const&) = delete;

			bytes m_approve;  ///< RLP encoded approve data
			//h512 nodeId;        ///< Network Id of the peer approve comes from
			p2p::node_id nodeId;
		};
		ImportApproveResult check_WITH_LOCK(h256 const& _h);
		ImportApproveResult manageImport_WITH_LOCK(h256 const& _h, approve const& _approve);
		bool remove_WITH_LOCK(h256 const& _txHash, uint64_t _epoch);

		void verifierBody();

		ImportApproveResult validateApprove(approve const& _t);

		mutable SharedMutex m_lock;  ///< General lock.
		h256Hash m_known;            ///< Headers of transactions in both sets.
		
		///< Approve that have previously been dropped. We technically only need to store the tx
		///< hash, but we also store bool as a placeholder value so that we can use an LRU cache to cap
		///< the number of approve hashes stored.
		LruCache<h256, bool> m_dropped;

		std::map<uint64_t, std::unordered_map<h256, approve>> m_current;
		
		/// verified broadcast incoming approve
		std::condition_variable m_queueReady;
		Signal<ImportApproveResult, h256 const&, p2p::node_id const&> m_onImport;			///< Called for each import attempt. Arguments are result, transaction id an node id. Be nice and exit fast.
		Signal<h256 const&> m_onImportProcessed; ///< First import notification unhandle processing dependency.
		std::vector<std::thread> m_verifiers;
		std::deque<UnverifiedApprove> m_unverified;  ///< Pending verification queue
		mutable Mutex x_queue;                           ///< Verification queue mutex
		std::atomic<bool> m_aborting = { false };          ///< Exit condition for verifier.

		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::shared_ptr<mcp::chain> m_chain;
		std::shared_ptr<mcp::async_task> m_async_task;
		std::shared_ptr<mcp::node_capability> m_capability;

		mcp::log m_log = { mcp::log("node") };
	};
}