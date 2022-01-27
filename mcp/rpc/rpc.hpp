#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <mcp/common/utility.hpp>
#include <unordered_map>
#include <mcp/common/log.hpp>
#include <mcp/common/mcp_json.hpp>
#include <mcp/db/database.hpp>
#include <libevm/ExtVMFace.h>
#include <mcp/node/chain.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/wallet/wallet.hpp>

namespace mcp
{
void error_response(std::function<void(mcp::json const &)> response_a, int const& error_code, std::string const & message_a, mcp::json& json_a);
void error_response(std::function<void(mcp::json const &)> response_a, int const& error_code, std::string const & message_a);
void error_response(std::function<void(mcp::json const &)> response_a, std::string const & message_a);


class rpc_config
{
public:
	rpc_config ();
	rpc_config (bool);
    void serialize_json(mcp::json &) const;
    bool deserialize_json(mcp::json const &);
    bool parse_old_version_data(mcp::json const &, uint64_t const&);

	boost::asio::ip::address address;
	uint16_t port;
	bool enable_control;
    bool rpc_enable;
};
enum class payment_status
{
	not_a_status,
	unknown,
	nothing, // Timeout and nothing was received
	//insufficient, // Timeout and not enough was received
	//over, // More than requested received
	//success_fork, // Amount received but it involved a fork
	success // Amount received
};

class rpc
{
public:
	rpc (mcp::block_store & store_a, std::shared_ptr<mcp::chain> chain_a,
		std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
		std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
		std::shared_ptr<mcp::async_task> background_a,
		boost::asio::io_service & service_a, mcp::rpc_config const& config_a);
	void start ();
	virtual void accept ();
	void stop ();
	boost::asio::io_service &io_service;
	boost::asio::ip::tcp::acceptor acceptor;
	std::mutex mutex;
	mcp::rpc_config config;
	std::shared_ptr<mcp::chain> m_chain;
	std::shared_ptr<mcp::block_cache> m_cache;
	std::shared_ptr<mcp::key_manager> m_key_manager;
	std::shared_ptr<mcp::wallet> m_wallet;
	std::shared_ptr<mcp::p2p::host> m_host;
	std::shared_ptr<mcp::async_task> m_background;
	mcp::block_store m_store;
    mcp::log m_log = { mcp::log("rpc") };
};
class rpc_connection : public std::enable_shared_from_this<mcp::rpc_connection>
{
public:
	rpc_connection (mcp::rpc &);
	virtual void parse_connection ();
	virtual void read ();
	virtual void write_result (std::string body, unsigned version);
	mcp::rpc & rpc;
	boost::asio::ip::tcp::socket socket;
	boost::beast::flat_buffer buffer;
	boost::beast::http::request<boost::beast::http::string_body> request;
	boost::beast::http::response<boost::beast::http::string_body> res;
	std::atomic_flag responded;
    mcp::log m_log = { mcp::log("rpc") };
};

enum class rpc_account_create_error_code
{
    ok = 0,
    empty_password = 1,
    invalid_length_password = 2,
    invalid_characters_password = 3,
    invalid_gen_next_work_value = 4,
    invalid_password = 5,
	invalid_backup = 6,
};

enum class rpc_account_remove_error_code
{
    ok = 0,
    invalid_account = 1,
    account_not_exisit = 2,
    wrong_password = 3,
    invalid_password = 4
};

enum class rpc_account_unlock_error_code
{
    ok = 0,
    invalid_account = 1,
    account_not_exisit = 2,
    wrong_password = 3,
    invalid_password = 4
};

enum class rpc_account_lock_error_code
{
    ok = 0,
    invalid_account = 1,
    account_not_exisit = 2,
};

enum class rpc_account_import_error_code
{
    ok = 0,
    invalid_gen_next_work_value = 1,
    invalid_json = 2
};

enum class rpc_account_export_error_code
{
    ok = 0,
    invalid_account = 1,
    account_not_exisit = 2
};

enum class rpc_account_validate_error_code
{
    ok = 0,
    invalid_account = 1
};

enum class rpc_account_password_change_error_code
{
    ok = 0,
    invalid_account = 1,
    account_not_exisit = 2,
    invalid_length_password = 3,
    invalid_characters_password = 4,
    wrong_password = 5,
    invalid_old_password = 6,
    invalid_new_password = 7

};

enum class rpc_account_list_error_code
{
    ok = 0
};

enum class rpc_account_block_list_error_code
{
    ok = 0,
    invalid_account = 1,
    invalid_limit = 2,
	limit_too_large = 3,
    invalid_index = 4,
    index_not_exsist= 5
};

enum class rpc_account_balance_error_code
{
    ok = 0,
    invalid_account = 1
};

enum class rpc_accounts_balances_error_code
{
    ok = 0,
    invalid_account = 1
};

enum class rpc_account_code_error_code
{
    ok = 0,
    invalid_account = 1
};

enum class rpc_send_error_code
{
    ok = 0,
    invalid_account_from = 1,
    account_not_exisit = 2,
    invalid_account_to = 3,
    invalid_amount = 4,
    invalid_gas = 5,
    invalid_data = 6,
    data_size_too_large = 7,
    invalid_gen_next_work_value = 8,
    account_locked = 9,
    wrong_password = 10,
    insufficient_balance = 11,
    validate_error = 12,
    send_block_error = 13,
    send_unknown_error = 14,
    invalid_password = 15,
    invalid_id = 16,
	invalid_async = 17,
	invalid_gas_price = 18,
	invalid_previous = 19
};

enum class rpc_generate_offline_block_error_code
{
    ok = 0,
    invalid_account_from = 1,
    invalid_account_to = 3,
    invalid_amount = 4,
    invalid_gas = 5,
    invalid_data = 6,
    data_size_too_large = 7,
    insufficient_balance = 8,
    validate_error = 9,
    compose_error = 10,
    compose_unknown_error = 11,
	invalid_gas_price = 12,
	invalid_previous = 13
};

enum class rpc_send_offline_block_error_code
{
    ok = 0,
    invalid_account_from = 1,
    invalid_account_to = 3,
    invalid_amount = 4,
    invalid_gas = 5,
    invalid_data = 6,
    data_size_too_large = 7,
    invalid_gen_next_work_value = 8,

    invalid_previous = 9,


    invalid_exec_timestamp = 10,
    invalid_work = 11,
    invalid_signature = 12,

    insufficient_balance = 13,
    validate_error = 14,
    send_block_error = 15,
    send_unknown_error = 16,
    invalid_password = 17,
    invalid_id = 18,
	invalid_gas_price = 19,
	invalid_async = 20
};

enum class rpc_sign_msg_error_code
{
    ok = 0,
    invalid_public_key = 1,
    invalid_msg = 2,
    wrong_password = 3,
    invalid_password = 4
};

enum class rpc_block_error_code
{
    ok = 0,
    invalid_hash = 1,
    block_not_exsist = 2
};

enum class rpc_blocks_error_code
{
    ok = 0,
    invalid_hash = 1,
    block_not_exsist = 2
};

enum class rpc_stable_blocks_error_code
{
    ok = 0,
	invalid_index = 1,
    invalid_limit = 2,
	limit_too_large = 3
};

enum class rpc_estimate_gas_error_code
{
	ok = 0,
	invalid_account_from = 1,
	invalid_account_to = 2,
	invalid_amount = 3,
	invalid_gas = 4,
	invalid_data = 5,
	data_size_too_large = 6,
	invalid_gas_price = 7,
	invalid_mci = 8,
	gas_not_enough_or_fail = 9
};

enum class rpc_call_error_code
{
	ok = 0,
	invalid_account_from = 1,
	invalid_account_to = 2,
	invalid_amount = 3,
	invalid_data = 4,
	data_size_too_large = 5,
	invalid_mci = 6
};

enum class rpc_debug_trace_transaction_error_code
{
	ok = 0,
	invalid_hash = 1,
	invalid_mci = 2
};

enum class rpc_debug_storage_range_at_error_code
{
	ok = 0,
	invalid_hash = 1,
	invalid_account = 2,
	invalid_begin = 3,
	invalid_max_results = 4
};

enum class rpc_logs_error_code
{
	ok = 0,
	invalid_from_stable_block_index = 1,
	invalid_to_stable_block_index = 2,
	invalid_account = 3,
	invalid_topics = 4
};

enum class rpc_status_error_code
{
    ok = 0
};

enum class rpc_witness_list_error_code
{
    ok = 0
};

enum class rpc_work_get_error_code
{
    ok = 0,
    invalid_account = 1,
    account_not_exisit = 2
};

enum class rpc_version_error_code
{
    ok = 0
};

enum class rpc_peers_error_code
{
    ok = 0
};

enum class rpc_nodes_error_code
{
    ok = 0
};

enum class rpc_stop_error_code
{
    ok = 0
};

enum class rpc_web3_sha3_error_code
{
	ok = 0,
	invalid_params = 1
};




class rpc_error_msg 
{
public:
    rpc_error_msg() = default;
    std::string msg(mcp::rpc_account_create_error_code const & err_a);
    std::string msg(mcp::rpc_account_remove_error_code const & err_a);
    std::string msg(mcp::rpc_account_unlock_error_code const & err_a);
    std::string msg(mcp::rpc_account_lock_error_code const & err_a);
    std::string msg(mcp::rpc_account_import_error_code const & err_a);
    std::string msg(mcp::rpc_account_export_error_code const & err_a);
    std::string msg(mcp::rpc_account_validate_error_code const & err_a);
    std::string msg(mcp::rpc_account_password_change_error_code const & err_a);
    std::string msg(mcp::rpc_account_list_error_code const & err_a);
    std::string msg(mcp::rpc_account_block_list_error_code const & err_a);
    std::string msg(mcp::rpc_account_balance_error_code const & err_a);
    std::string msg(mcp::rpc_accounts_balances_error_code const & err_a);
    std::string msg(mcp::rpc_account_code_error_code const & err_a);
    std::string msg(mcp::rpc_send_error_code const & err_a);
	std::string msg(mcp::rpc_estimate_gas_error_code const & err_a);
	std::string msg(mcp::rpc_call_error_code const & err_a);
	std::string msg(mcp::rpc_debug_trace_transaction_error_code const & err_a);
	std::string msg(mcp::rpc_debug_storage_range_at_error_code const & err_a);
	std::string msg(mcp::rpc_logs_error_code const & err_a);
    std::string msg(mcp::rpc_generate_offline_block_error_code const & err_a);
    std::string msg(mcp::rpc_send_offline_block_error_code const & err_a);
    std::string msg(mcp::rpc_sign_msg_error_code const & err_a);
    std::string msg(mcp::rpc_block_error_code const & err_a);
    std::string msg(mcp::rpc_blocks_error_code const & err_a);
    std::string msg(mcp::rpc_stable_blocks_error_code const & err_a);
    std::string msg(mcp::rpc_status_error_code const & err_a);
    std::string msg(mcp::rpc_witness_list_error_code const & err_a);
    std::string msg(mcp::rpc_work_get_error_code const & err_a);
    std::string msg(mcp::rpc_version_error_code const & err_a);
    std::string msg(mcp::rpc_peers_error_code const & err_a);
    std::string msg(mcp::rpc_nodes_error_code const & err_a);
    std::string msg(mcp::rpc_stop_error_code const & err_a);
	std::string msg(mcp::rpc_web3_sha3_error_code const & err_a);
};

class rpc_handler : public std::enable_shared_from_this<mcp::rpc_handler>
{
public:
    rpc_handler(mcp::rpc &, std::string const &, std::function<void(mcp::json const &)>const & ,int m_cap);
    void process_request ();

	void account_list();
	void account_validate();
	void account_create();
	void account_remove();
	void account_unlock();
	void account_lock();
	void account_export();
	void account_import();
	void account_password_change();
	void account_code();
	void account_balance();
	void accounts_balances();
	void account_block_list();
	void account_state_list();

	void block ();
	void blocks();
	void block_state();
	void block_states();
	void block_traces();
	void stable_blocks();

	void send_block();
	void generate_offline_block();
	void send_offline_block();
	void block_summary();
	void sign_msg();

	void version();
	void status();
	void peers();
	void nodes();
	void work_get();
	void witness_list();

    void estimate_gas();
    void call();

	void debug_trace_transaction();
	void debug_storage_range_at();

	void logs();

    /**
     *  compatible RPC with Ethereum
     * 
     * */

    void web3_clientVersion();
    void web3_sha3();
    void eth_accounts();
    void eth_blockNumber();
    void eth_chainId();
    void eth_estimateGas();
    void eth_getBlockByNumber();
    void eth_gasPrice();
	void eth_getTransactionCount();
    void eth_sendRawTransaction();
    void net_version();

	std::string body;
	mcp::rpc & rpc;

	static const uint32_t list_max_limit = 1000;

    mcp::json request;
    std::function<void(mcp::json const& )> response;

private:
	std::shared_ptr<mcp::chain> m_chain;
	std::shared_ptr<mcp::block_cache> m_cache;
	std::shared_ptr<mcp::key_manager> m_key_manager;
	std::shared_ptr<mcp::wallet> m_wallet;
	std::shared_ptr<mcp::p2p::host> m_host;
	std::shared_ptr<mcp::async_task> m_background;
	mcp::block_store m_store;
	
	bool try_get_bool_from_json(std::string const& field_name_a,bool & value_a );
    bool try_get_uint64_t_from_json(std::string const& field_name_a, uint64_t & value_a);
	bool try_get_mc_info(dev::eth::McInfo & mc_info_a);

    mcp::rpc_error_msg err;
    mcp::log m_log = { mcp::log("rpc") };
};
/** Returns the correct RPC implementation based on TLS configuration */
std::shared_ptr<mcp::rpc> get_rpc (mcp::block_store & store_a, std::shared_ptr<mcp::chain> chain_a,
	std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
	std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
	std::shared_ptr<mcp::async_task> background_a,
	boost::asio::io_service & service_a, mcp::rpc_config const& config_a);
}
