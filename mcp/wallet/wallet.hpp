#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>

#include <mcp/core/block_store.hpp>
#include <mcp/common/log.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/node/block_processor.hpp>
#include <mcp/node/composer.hpp>
#include <mcp/node/transaction_queue.hpp>

namespace mcp
{
enum class send_result_codes
{
	ok,
	account_locked,
	from_not_exists,
	wrong_password,
	insufficient_balance,
	data_size_too_large,
	validate_error,
	dag_no_links,
	error
};

class wallet : public std::enable_shared_from_this<mcp::wallet>
{
public:
	wallet (
		mcp::block_store& block_store_a, std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
		std::shared_ptr<TransactionQueue> tq
	);
	~wallet() { stop(); }
	void send_async(TransactionSkeleton t, std::function<void(h256 &, boost::optional<dev::Exception const &>)> const & action_a, boost::optional<std::string> const & password = boost::none);
	h256 send_action(TransactionSkeleton t, boost::optional<std::string> const & password);
	/// Imports the given transaction into the transaction queue
	h256 importTransaction(Transaction const& _t);
	u256 getTransactionCount(Address const& from, BlockNumber const blockTag = PendingBlock);
	void stop();

	/// Populate the uninitialized fields in the supplied transaction with default values
	void populateTransactionWithDefaults(TransactionSkeleton& _t);

private:
	void do_wallet_actions();
	void queue_wallet_action(std::function<void()> const & action_a);

	std::deque<std::function<void()>> m_actions;
	std::mutex m_mutex;
	std::condition_variable m_condition;
	bool m_stopped;
	std::thread m_thread;
	mcp::block_store m_block_store;
	std::shared_ptr<mcp::key_manager> m_key_manager;
	std::shared_ptr<TransactionQueue> m_tq;                  ///< Maintains a list of incoming transactions not yet in a block on the blockchain.
	std::shared_ptr<mcp::iblock_cache> m_cache;

    mcp::log m_log_pow = { mcp::log("pow") };
    mcp::log m_log = { mcp::log("node") };
};
}
