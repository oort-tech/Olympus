#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>

#include <mcp/core/block_store.hpp>
#include <mcp/common/log.hpp>
#include <mcp/wallet/wallet_store.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/node/block_processor.hpp>
#include <mcp/node/composer.hpp>

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

class send_result
{
public:
	send_result(mcp::send_result_codes const & code_a, std::shared_ptr<mcp::block> block_a, std::string const& msg_a = "");
	mcp::send_result_codes code;
	std::shared_ptr<mcp::block> block;
	std::string msg;
};

class wallet : public std::enable_shared_from_this<mcp::wallet>
{
public:
	wallet (
		mcp::wallet_store& store_a, mcp::block_store& block_store_a,
		std::shared_ptr<mcp::key_manager> key_manager_a, std::shared_ptr<mcp::composer> composer_a,
		std::shared_ptr<mcp::block_processor> block_processor_a
	);
	~wallet() { stop(); }
	void send_async(mcp::block_type const & type_a, boost::optional<mcp::block_hash> const & previous_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a, uint256_t const & gas_a, uint256_t const & gas_price_a, std::vector<uint8_t> const & data_a, boost::optional<std::string> const & password_a, std::function<void(mcp::send_result)> const & action_a, bool const & async_a = false);
	mcp::send_result send_action(mcp::block_type const & type_a, boost::optional<mcp::block_hash> const & previous_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a, uint256_t gas_a, uint256_t gas_price_a, std::vector<uint8_t> const & data_a, boost::optional<std::string> const & password_a, bool const & async_a);
	void send_async(std::shared_ptr<mcp::block> const p_block, mcp::signature const & signature_a, std::function<void(mcp::send_result)> const & action_a, bool const & async_a);
	mcp::send_result send_action(std::shared_ptr<mcp::block> const p_block, mcp::signature const & signature_a, std::function<void(mcp::send_result)> const & action_a, bool const & async_a);
	void stop();

	std::shared_ptr<mcp::composer> composer;
private:
	mcp::send_result handle_compose_result(mcp::compose_result const & compose_result_a, bool const & async_a);
	void do_wallet_actions();
	void queue_wallet_action(std::function<void()> const & action_a);

	std::deque<std::function<void()>> m_actions;
	std::mutex m_mutex;
	std::condition_variable m_condition;
	bool m_stopped;
	std::thread m_thread;
	mcp::wallet_store m_store;
	mcp::block_store m_block_store;
	std::shared_ptr<mcp::key_manager> m_key_manager;
	std::shared_ptr<mcp::block_processor> m_block_processor;
    mcp::log m_log_pow = { mcp::log("pow") };
    mcp::log m_log = { mcp::log("node") };
};
}
