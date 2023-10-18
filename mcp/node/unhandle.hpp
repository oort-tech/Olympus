#pragma once

#include <mcp/node/message.hpp>
#include <mcp/common/log.hpp>

#include <unordered_set>
#include <unordered_map>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include "transaction_queue.hpp"
#include "approve_queue.hpp"

namespace mcp
{
	enum unhandle_add_result : int
	{
		/// clear cache, go to sync
		Nothing,

		/// block in unhandle,need request exist missing,not block missing
		Exist,

		/// first import,request block missing
		Success,

		/// just dependence on transaction,and transaction in the cache
		Retry,
	};

class unhandle_item
{
  public:
	unhandle_item() = default;
	unhandle_item(std::shared_ptr<mcp::block_processor_item> item_a,
		h256Hash const &bks_a, h256Hash const &txs_a, h256Hash const &aps_a
	):item(item_a), bks(bks_a), txs(txs_a), aps(aps_a) {}

	bool ready() { return bks.empty() && txs.empty() && aps.empty(); }
	std::shared_ptr<mcp::block_processor_item> item;
	h256Hash bks;
	h256Hash txs;
	h256Hash aps;
};

class unhandle_cache
{
  public:
	  unhandle_cache(std::shared_ptr<TransactionQueue> tq, std::shared_ptr<ApproveQueue> aq):
		  m_tq(tq),m_aq(aq) {}

	unhandle_add_result add(std::shared_ptr<mcp::block_processor_item> item_a, h256Hash const &bks, h256Hash const &transactions, h256Hash const &approves);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> release_dependency(mcp::block_hash const &dependency_hash_a);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> release_transaction_dependency(h256Hash const &hashs);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> release_approve_dependency(h256Hash const &hashs);
	void get_missings(size_t const & missing_limit_a, std::vector<mcp::block_hash>& missings_a, std::vector<h256>& light_missings_a, std::vector<h256>& approve_missings_a);

	bool exists(mcp::block_hash const & block_hash_a);
	bool is_full();
	std::string getInfo();
  private:
	/// unhandle hash -> unhandle item
	std::unordered_map<mcp::block_hash, mcp::unhandle_item> m_pending;
	/// dependency hash -> unhandle block hashs
	/// All blocks that depend on the block. such as, the parent of block C is block A, and the previous of block D is block A too.
	std::unordered_map<mcp::block_hash, h256Hash> m_dependencies;
	/// dependency hash -> unhandle transaction hashs
	/// All blocks that depend on the transaction. maybe block A depend on T1,and block B depend on T1 too.
	std::unordered_map<h256, h256Hash> m_transactions;
	/// dependency hash -> unhandle approve hashs
	/// All blocks that depend on the approve. maybe block A depend on approve1,and block B depend on approve1 too.
	std::unordered_map<h256, h256Hash> m_approves;

	h256Hash m_missings;
	h256Hash m_light_missings;
	h256Hash m_approve_missings;

	std::mutex m_mutux;
	std::shared_ptr<TransactionQueue> m_tq;                  ///< Maintains a list of incoming transactions not yet in a block on the blockchain.
	std::shared_ptr<ApproveQueue> m_aq;                  ///< Maintains a list of incoming approves not yet in a block on the blockchain.

	uint64_t add_unhandle_ok_count = 0;
	uint64_t unhandle_full_count = 0;
	uint64_t unhandle_exist_count = 0;
    mcp::log m_log = { mcp::log("node") };
};

} // namespace mcp
