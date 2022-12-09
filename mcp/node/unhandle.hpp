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
	unhandle_item(mcp::block_hash const &unhandle_hash_a, std::shared_ptr<mcp::block_processor_item> item_a, std::unordered_set<mcp::block_hash> const &dependency_hashs_a, h256Hash const &transactions_a, h256Hash const &approves_a);
	mcp::block_hash unhandle_hash;
	std::shared_ptr<mcp::block_processor_item> item;
	std::unordered_set<mcp::block_hash> dependency_hashs;
	h256Hash transactions;
	h256Hash approves;
};

class unhandle_cache
{
  public:
	unhandle_cache(std::shared_ptr<TransactionQueue> tq, std::shared_ptr<ApproveQueue> aq, size_t const &capacity_a = 1000);

	unhandle_add_result add(mcp::block_hash const &hash_a, std::unordered_set<mcp::block_hash> const &dependency_hashs_a, h256Hash const &transactions, h256Hash const &approves, std::shared_ptr<mcp::block_processor_item> item_a);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> release_dependency(mcp::block_hash const &dependency_hash_a);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> release_transaction_dependency(h256Hash const &hashs);
	std::unordered_set<std::shared_ptr<mcp::block_processor_item>> release_approve_dependency(h256 const &h);
	void get_missings(size_t const & missing_limit_a, std::vector<mcp::block_hash>& missings_a, std::vector<h256>& light_missings_a, std::vector<h256>& approve_missings_a);

	bool exists(mcp::block_hash const & block_hash_a);

	size_t unhandlde_size() const;
	size_t dependency_size() const;
    size_t missing_size() const;
	size_t light_missing_size() const;
	size_t approve_missing_size() const;
    size_t tips_size() const;
   
    uint64_t add_unhandle_ok_count = 0;
    uint64_t unhandle_full_count = 0;
    uint64_t unhandle_exist_count = 0;

  private:
	void del_unhandle_in_dependencies(mcp::block_hash const &unhandle_a);

	/// unhandle hash -> unhandle item
	std::unordered_map<mcp::block_hash, mcp::unhandle_item> m_unhandles;
	/// dependency hash -> unhandle block hashs
	/// All blocks that depend on the block. such as, the parent of block C is block A, and the previous of block D is block A too.
	std::unordered_map<mcp::block_hash, std::shared_ptr<std::unordered_set<mcp::block_hash>>> m_dependencies;
	/// dependency hash -> unhandle transaction hashs
	/// All blocks that depend on the transaction. maybe block A depend on T1,and block B depend on T1 too.
	std::unordered_map<h256, std::shared_ptr<std::unordered_set<mcp::block_hash>>> m_transactions;
	/// dependency hash -> unhandle approve hashs
	/// All blocks that depend on the approve. maybe block A depend on approve1,and block B depend on approve1 too.
	std::unordered_map<h256, std::shared_ptr<std::unordered_set<mcp::block_hash>>> m_approves;

	std::unordered_set<mcp::block_hash> m_missings;
	std::unordered_set<h256> m_light_missings;
	std::unordered_set<h256> m_approve_missings;
	std::unordered_set<mcp::block_hash> m_tips;/// No other blocks depend on this block ,like free.

	size_t m_capacity;
    const int m_max_search_count = 100;
	std::mutex m_mutux;
	std::shared_ptr<TransactionQueue> m_tq;                  ///< Maintains a list of incoming transactions not yet in a block on the blockchain.
	std::shared_ptr<ApproveQueue> m_aq;                  ///< Maintains a list of incoming approves not yet in a block on the blockchain.

    mcp::log m_log = { mcp::log("node") };
};

} // namespace mcp
