#include "rpc.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

#include <mcp/node/composer.hpp>
#include <mcp/common/pwd.hpp>
#include <mcp/core/genesis.hpp>
#include <mcp/node/evm/Executive.hpp>
#include <libdevcore/CommonJS.h>
#include <libdevcore/CommonData.h>

#include "exceptions.hpp"
#include "jsonHelper.hpp"

mcp::rpc_config::rpc_config() : rpc_config(false)
{
}

mcp::rpc_config::rpc_config(bool enable_control_a) :
	address(boost::asio::ip::address_v4::loopback()),
	port(8765),
	enable_control(enable_control_a),
	rpc_enable(false)
{
}

void mcp::rpc_config::serialize_json(mcp::json &json_a) const
{
	json_a["rpc"] = rpc_enable ? "true" : "false";
	json_a["rpc_addr"] = address.to_string();
	json_a["rpc_port"] = port;
	json_a["rpc_control"] = enable_control ? "true" : "false";
}

bool mcp::rpc_config::deserialize_json(mcp::json const &json_a)
{
	auto error(false);
	try
	{
		if (!error)
		{
			if (json_a.count("rpc") && json_a["rpc"].is_string())
			{
				rpc_enable = (json_a["rpc"].get<std::string>() == "true" ? true : false);
			}
			else
			{
				error = true;
			}

			if (json_a.count("rpc_addr") && json_a["rpc_addr"].is_string())
			{
				std::string address_text = json_a["rpc_addr"].get<std::string>();
				boost::system::error_code ec;
				address = boost::asio::ip::address::from_string(address_text, ec);
				if (ec)
				{
					error = true;
				}
			}
			else
			{
				error = true;
			}

			if (json_a.count("rpc_port") && json_a["rpc_port"].is_number_unsigned())
			{
				uint64_t port_l = json_a["rpc_port"].get<uint64_t>();
				if (port_l <= std::numeric_limits<uint16_t>::max())
				{
					port = port_l;
				}
				else
				{
					error = true;
				}
			}
			else
			{
				error = true;
			}

			if (json_a.count("rpc_control") && json_a["rpc_control"].is_string())
			{
				enable_control = (json_a["rpc_control"].get<std::string>() == "true" ? true : false);
			}
			else
			{
				error = true;
			}
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

bool mcp::rpc_config::parse_old_version_data(mcp::json const &json_a, uint64_t const &version)
{
	auto error(false);
	try
	{
		if (version < 2)
		{
			if (json_a.count("rpc_enable") && json_a["rpc_enable"].is_string())
			{
				rpc_enable = (json_a["rpc_enable"].get<std::string>() == "true" ? true : false);
			}

			if (json_a.count("rpc") && json_a["rpc"].is_object())
			{
				mcp::json j_rpc_l = json_a["rpc"].get<mcp::json>();
				;

				std::string address_text;
				if (j_rpc_l.count("address") && j_rpc_l["address"].is_string())
				{
					address_text = j_rpc_l["address"].get<std::string>();
				}

				std::string port_text;
				if (j_rpc_l.count("port") && j_rpc_l["port"].is_string())
				{
					port_text = j_rpc_l["port"].get<std::string>();
				}

				if (j_rpc_l.count("enable_control") && j_rpc_l["enable_control"].is_string())
				{
					enable_control = (j_rpc_l["enable_control"].get<std::string>() == "true" ? true : false);
				}

				try
				{
					auto port_l = std::stoul(port_text);
					if (port_l <= std::numeric_limits<uint16_t>::max())
					{
						port = port_l;
					}
					else
					{
						error = true;
					}
				}
				catch (std::logic_error const &)
				{
					error = true;
				}
				boost::system::error_code ec;
				address = boost::asio::ip::address::from_string(address_text, ec);
				if (ec)
				{
					error = true;
				}
			}
		}
		else
		{
			if (json_a.count("rpc") && json_a["rpc"].is_object())
			{
				mcp::json j_rpc_l = json_a["rpc"].get<mcp::json>();
				error |= deserialize_json(j_rpc_l);
			}
			else
				error = true;
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

mcp::rpc::rpc(mcp::block_store &store_a, std::shared_ptr<mcp::chain> chain_a,
			  std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
			  std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
			  std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::composer> composer_a,
			  boost::asio::io_service &service_a, mcp::rpc_config const &config_a) : m_store(store_a),
																					 m_chain(chain_a),
																					 m_cache(cache_a),
																					 m_key_manager(key_manager_a),
																					 m_wallet(wallet_a),
																					 m_host(host_a),
																					 m_background(background_a),
																					 m_composer(composer_a),
																					 io_service(service_a),
																					 acceptor(service_a),
																					 config(config_a)
{
}

void mcp::rpc::start()
{
	auto endpoint(bi::tcp::endpoint(config.address, config.port));
	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

	boost::system::error_code ec;
	acceptor.bind(endpoint, ec);
	if (ec)
	{
		LOG(m_log.error) << boost::str(boost::format("Error while binding for HTTP RPC on port %1%: %2%") % endpoint.port() % ec.message());
		throw std::runtime_error(ec.message());
	}

	acceptor.listen();

	LOG(m_log.info) << "HTTP RPC started, http://" << endpoint;
	LOG(m_log.info) << "HTTP RPC control is " << (config.enable_control ? "enabled" : "disabled");

	accept();
}

void mcp::rpc::accept()
{
	auto connection(std::make_shared<mcp::rpc_connection>(*this));
	acceptor.async_accept(connection->socket, [this, connection](boost::system::error_code const &ec)
						  {
		if (!ec)
		{
			accept();
			connection->parse_connection();
		}
		else
		{
            LOG(this->m_log.error) << "Error accepting HTTP RPC connections:" << ec.message();
		} });
}

void mcp::rpc::stop()
{
	acceptor.close();
}

mcp::rpc_handler::rpc_handler(mcp::rpc &rpc_a, std::string const &body_a, std::function<void(mcp::json const &)> const &response_a, int m_cap) : body(body_a),
																																				 rpc(rpc_a),
																																				 response(response_a),
																																				 m_chain(rpc_a.m_chain),
																																				 m_cache(rpc_a.m_cache),
																																				 m_key_manager(rpc_a.m_key_manager),
																																				 m_wallet(rpc_a.m_wallet),
																																				 m_host(rpc_a.m_host),
																																				 m_composer(rpc_a.m_composer),
																																				 m_background(rpc_a.m_background),
																																				 m_store(rpc.m_store)
{
	m_rpcMethods["account_list"] = &mcp::rpc_handler::account_list;
	m_rpcMethods["account_validate"] = &mcp::rpc_handler::account_validate;
	m_rpcMethods["account_create"] = &mcp::rpc_handler::account_create;
	m_rpcMethods["account_remove"] = &mcp::rpc_handler::account_remove;
	m_rpcMethods["account_unlock"] = &mcp::rpc_handler::account_unlock;
	m_rpcMethods["account_lock"] = &mcp::rpc_handler::account_lock;
	m_rpcMethods["account_export"] = &mcp::rpc_handler::account_export;
	m_rpcMethods["account_import"] = &mcp::rpc_handler::account_import;
	m_rpcMethods["account_password_change"] = &mcp::rpc_handler::account_password_change;
	m_rpcMethods["account_code"] = &mcp::rpc_handler::account_code;
	m_rpcMethods["account_balance"] = &mcp::rpc_handler::account_balance;
	m_rpcMethods["accounts_balances"] = &mcp::rpc_handler::accounts_balances;
	m_rpcMethods["account_block_list"] = &mcp::rpc_handler::account_block_list;
	m_rpcMethods["account_state_list"] = &mcp::rpc_handler::account_state_list;
	m_rpcMethods["block"] = &mcp::rpc_handler::block;
	m_rpcMethods["blocks"] = &mcp::rpc_handler::blocks;
	m_rpcMethods["block_state"] = &mcp::rpc_handler::block_state;
	m_rpcMethods["block_states"] = &mcp::rpc_handler::block_states;
	m_rpcMethods["block_traces"] = &mcp::rpc_handler::block_traces;
	m_rpcMethods["stable_blocks"] = &mcp::rpc_handler::stable_blocks;
	m_rpcMethods["send_block"] = &mcp::rpc_handler::send_block;
	m_rpcMethods["generate_offline_block"] = &mcp::rpc_handler::generate_offline_block;
	m_rpcMethods["send_offline_block"] = &mcp::rpc_handler::send_offline_block;
	m_rpcMethods["block_summary"] = &mcp::rpc_handler::block_summary;
	m_rpcMethods["sign_msg"] = &mcp::rpc_handler::sign_msg;
	m_rpcMethods["version"] = &mcp::rpc_handler::version;
	m_rpcMethods["status"] = &mcp::rpc_handler::status;
	m_rpcMethods["peers"] = &mcp::rpc_handler::peers;
	m_rpcMethods["nodes"] = &mcp::rpc_handler::nodes;
	m_rpcMethods["witness_list"] = &mcp::rpc_handler::witness_list;
	m_rpcMethods["estimate_gas"] = &mcp::rpc_handler::estimate_gas;
	m_rpcMethods["logs"] = &mcp::rpc_handler::logs;
	m_rpcMethods["debug_trace_transaction"] = &mcp::rpc_handler::debug_trace_transaction;
	m_rpcMethods["debug_storage_range_at"] = &mcp::rpc_handler::debug_storage_range_at;
	
	m_rpcMethods["net_version"] = &mcp::rpc_handler::net_version;
	m_rpcMethods["net_listening"] = &mcp::rpc_handler::net_listening;
	m_rpcMethods["net_peerCount"] = &mcp::rpc_handler::net_peerCount;
	m_rpcMethods["web3_clientVersion"] = &mcp::rpc_handler::web3_clientVersion;
	m_rpcMethods["web3_sha3"] = &mcp::rpc_handler::web3_sha3;

	m_rpcMethods["eth_blockNumber"] = &mcp::rpc_handler::eth_blockNumber;
	m_rpcMethods["eth_getTransactionCount"] = &mcp::rpc_handler::eth_getTransactionCount;
	m_rpcMethods["eth_chainId"] = &mcp::rpc_handler::eth_chainId;
	m_rpcMethods["eth_gasPrice"] = &mcp::rpc_handler::eth_gasPrice;
	m_rpcMethods["eth_estimateGas"] = &mcp::rpc_handler::eth_estimateGas;
	m_rpcMethods["eth_getBlockByNumber"] = &mcp::rpc_handler::eth_getBlockByNumber;
	m_rpcMethods["eth_getBlockTransactionCountByHash"] = &mcp::rpc_handler::eth_getBlockTransactionCountByHash;
	m_rpcMethods["eth_getBlockTransactionCountByNumber"] = &mcp::rpc_handler::eth_getBlockTransactionCountByNumber;
	m_rpcMethods["eth_sendRawTransaction"] = &mcp::rpc_handler::eth_sendRawTransaction;
	m_rpcMethods["eth_sendTransaction"] = &mcp::rpc_handler::eth_sendTransaction;
	m_rpcMethods["eth_call"] = &mcp::rpc_handler::eth_call;
	
	m_rpcMethods["eth_protocolVersion"] = &mcp::rpc_handler::eth_protocolVersion;
	m_rpcMethods["eth_syncing"] = &mcp::rpc_handler::eth_syncing;
	m_rpcMethods["eth_getLogs"] = &mcp::rpc_handler::eth_getLogs;
	m_rpcMethods["eth_getCode"] = &mcp::rpc_handler::eth_getCode;
	m_rpcMethods["eth_getStorageAt"] = &mcp::rpc_handler::eth_getStorageAt;
	m_rpcMethods["eth_getTransactionByHash"] = &mcp::rpc_handler::eth_getTransactionByHash;
	m_rpcMethods["eth_getTransactionByBlockHashAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex;
	m_rpcMethods["eth_getTransactionByBlockNumberAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex;
	m_rpcMethods["eth_getTransactionReceipt"] = &mcp::rpc_handler::eth_getTransactionReceipt;
	m_rpcMethods["eth_getBalance"] = &mcp::rpc_handler::eth_getBalance;
	m_rpcMethods["eth_getBlockByHash"] = &mcp::rpc_handler::eth_getBlockByHash;
	m_rpcMethods["eth_accounts"] = &mcp::rpc_handler::eth_accounts;
	m_rpcMethods["eth_sign"] = &mcp::rpc_handler::eth_sign;
	m_rpcMethods["eth_signTransaction"] = &mcp::rpc_handler::eth_signTransaction;

	m_rpcMethods["personal_importRawKey"] = &mcp::rpc_handler::personal_importRawKey;
	m_rpcMethods["personal_listAccounts"] = &mcp::rpc_handler::personal_listAccounts;
	m_rpcMethods["personal_lockAccount"] = &mcp::rpc_handler::personal_lockAccount;
	m_rpcMethods["personal_newAccount"] = &mcp::rpc_handler::personal_newAccount;
	m_rpcMethods["personal_unlockAccount"] = &mcp::rpc_handler::personal_unlockAccount;
	m_rpcMethods["personal_sendTransaction"] = &mcp::rpc_handler::personal_sendTransaction;
	m_rpcMethods["personal_sign"] = &mcp::rpc_handler::personal_sign;
	m_rpcMethods["personal_ecRecover"] = &mcp::rpc_handler::personal_ecRecover;
}

void mcp::rpc_response(std::function<void(mcp::json const &)> response_a, std::string const &message_a)
{
	mcp::json j_response;
	j_response["msg"] = message_a;
	response_a(j_response);
}

void mcp::rpc_response(std::function<void(mcp::json const &)> response_a, int const &error_code, std::string const &message_a, mcp::json &json_a)
{
	mcp::json j_response;
	j_response["code"] = error_code;
	j_response["msg"] = message_a;
	if (!json_a.is_null())
	{
		j_response.update(json_a);
	}
	response_a(j_response);
}

void mcp::rpc_response(std::function<void(mcp::json const &)> response_a, int const &error_code, std::string const &message_a)
{
	mcp::json j_response;
	mcp::rpc_response(response_a, error_code, message_a, j_response);
}

namespace
{
	bool decode_unsigned(std::string const &text, uint64_t &number)
	{
		bool result;
		size_t end;
		try
		{
			number = std::stoull(text, &end);
			result = false;
		}
		catch (std::invalid_argument const &)
		{
			result = true;
		}
		catch (std::out_of_range const &)
		{
			result = true;
		}
		result = result || end != text.size();
		return result;
	}
}

bool mcp::rpc_handler::try_get_bool_from_json(std::string const &field_name_a, bool &value_a)
{
	bool nret(true);
	auto it = request.find(field_name_a);
	if (it != request.end())
	{
		if (it->is_string())
		{
			std::string value_field_text = it->get<std::string>();
			if (value_field_text == "1" || value_field_text == "0")
			{
				value_a = value_field_text == "1" ? true : false;
			}
			else
			{
				nret = false;
			}
		}
		else if (it->is_number_unsigned())
		{
			uint64_t value_field_text = it->get<uint64_t>();
			if (value_field_text == 0 || value_field_text == 1)
			{
				value_a = value_field_text;
			}
			else
			{
				nret = false;
			}
		}
		else
		{
			nret = false;
		}
	}
	else
	{
		nret = false;
	}
	return nret;
}

bool mcp::rpc_handler::try_get_uint64_t_from_json(std::string const &field_name_a, uint64_t &value_a)
{
	bool nret(true);
	auto it = request.find(field_name_a);
	if (it != request.end())
	{
		if (it->is_number_unsigned())
		{
			value_a = it->get<int64_t>();
		}
		else if (it->is_string())
		{
			try
			{
				value_a = std::stoull(it->get<std::string>());
			}
			catch (const std::exception &)
			{
				nret = false;
			}
		}
		else
		{
			nret = false;
		}
	}
	else
	{
		nret = false;
	}

	return nret;
}

bool mcp::rpc_handler::try_get_mc_info(dev::eth::McInfo &mc_info_a)
{
	std::string mci_str = "latest";
	if (request.count("mci"))
	{
		if (!request["mci"].is_string())
			return false;
		std::string str = request["mci"];
		mci_str = str;
	}

	uint64_t mci;
	if (mci_str == "latest")
		mci = m_chain->last_stable_mci();
	else if (mci_str == "earliest")
		mci = 0;
	else
	{
		if (!try_get_uint64_t_from_json("mci", mci))
			return false;

		if (mci > m_chain->last_stable_mci())
			return false;
	}

	return try_get_mc_info(mc_info_a, mci);
}

bool mcp::rpc_handler::try_get_mc_info(dev::eth::McInfo &mc_info_a, uint64_t &mci)
{
	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash mc_hash;
	bool exists(!m_store.main_chain_get(transaction, mci, mc_hash));
	assert_x(exists);
	std::shared_ptr<mcp::block_state> mc_state(m_cache->block_state_get(transaction, mc_hash));
	assert_x(mc_state);
	assert_x(mc_state->is_stable);
	assert_x(mc_state->main_chain_index);
	assert_x(mc_state->mc_timestamp > 0);

	uint64_t last_summary_mci(0);
	if (mc_hash != mcp::genesis::block_hash)
	{
		std::shared_ptr<mcp::block> mc_block(m_cache->block_get(transaction, mc_hash));
		assert_x(mc_block);
		std::shared_ptr<mcp::block_state> last_summary_state(m_cache->block_state_get(transaction, mc_block->last_summary_block()));
		assert_x(last_summary_state);
		assert_x(last_summary_state->is_stable);
		assert_x(last_summary_state->is_on_main_chain);
		assert_x(last_summary_state->main_chain_index);
		last_summary_mci = *last_summary_state->main_chain_index;
	}

	mc_info_a = dev::eth::McInfo(*mc_state->main_chain_index, mc_state->mc_timestamp, last_summary_mci);

	return true;
}

void mcp::rpc_handler::account_list(mcp::json & j_response)
{
	mcp::json j_accounts = mcp::json::array();
	std::list<dev::Address> account_list(m_key_manager->list());
	for (auto account : account_list)
	{
		j_accounts.push_back(account.hexPrefixed());
	}
	j_response["accounts"] = j_accounts;
}

void mcp::rpc_handler::account_validate(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	j_response["valid"] = mcp::isAddress(account_text) ? 1 : 0;
}

void mcp::rpc_handler::account_create(mcp::json & j_response)
{
	if (!request.count("password") || !request["password"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidPassword());
	}

	std::string password = request["password"];
	if (password.empty()) {
		BOOST_THROW_EXCEPTION(RPC_Error_EmptyPassword());
	}

	if (!mcp::validatePasswordSize(password)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLengthPassword());
	}

	if (!mcp::validatePassword(password)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidCharactersPassword());
	}

	bool backup_l(true);
	if (request.count("backup") && try_get_bool_from_json("backup", backup_l)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidGenNextWorkValue());
	}

	bool gen_next_work_l(false);
	dev::Address new_account = m_key_manager->create(password, gen_next_work_l, backup_l);
	j_response["account"] = new_account.hexPrefixed();
	
	/*if (rpc.config.enable_control)
	{
		if (request.count("password") && request["password"].is_string())
		{
			std::string password = request["password"];
			if (!password.empty())
			{
				if (mcp::validatePasswordSize(password))
				{
					if (mcp::validatePassword(password))
					{
						bool gen_next_work_l(false);

						bool backup_l(true);
						if (request.count("backup"))
						{
							if (!try_get_bool_from_json("backup", backup_l))
							{
								error_code_l = mcp::rpc_account_create_error_code::invalid_gen_next_work_value;
								rpc_response(response, int(error_code_l), err.msg(error_code_l));
								return;
							}
						}
						dev::Address new_account = m_key_manager->create(password, gen_next_work_l, backup_l);
						j_response["account"] = new_account.hexPrefixed();
						error_code_l = mcp::rpc_account_create_error_code::ok;
						rpc_response(response, int(error_code_l), err.msg(error_code_l), j_response);
					}
					else
					{
						error_code_l = mcp::rpc_account_create_error_code::invalid_characters_password;
						rpc_response(response, int(error_code_l), err.msg(error_code_l));
					}
				}
				else
				{
					error_code_l = mcp::rpc_account_create_error_code::invalid_length_password;
					rpc_response(response, int(error_code_l), err.msg(error_code_l));
				}
			}
			else
			{
				error_code_l = mcp::rpc_account_create_error_code::empty_password;
				rpc_response(response, int(error_code_l), err.msg(error_code_l));
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_create_error_code::invalid_password;
			rpc_response(response, int(error_code_l), err.msg(error_code_l));
			return;
		}
	}
	else
	{
		rpc_response(response, "HTTP RPC control is disabled");
	}*/
}

void mcp::rpc_handler::account_remove(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account)) {
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	if (!request.count("password") || !request["password"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidPassword());
	}

	std::string password_text = request["password"];
	if (password_text.empty()) {
		BOOST_THROW_EXCEPTION(RPC_Error_EmptyPassword());
	}

	if (m_key_manager->remove(account, password_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
	}

	/*if (rpc.config.enable_control)
	{
		mcp::rpc_account_remove_error_code error_code_l;
		bool error(true);
		if (request.count("account") && request["account"].is_string())
		{
			error = !mcp::isAddress(request["account"]);
		}
		if (!error)
		{
			std::string account_text = request["account"];
			dev::Address account(account_text);
			bool exists(m_key_manager->exists(account));
			if (exists)
			{
				if (request.count("password") && request["password"].is_string())
				{
					std::string password_text = request["password"];
					bool error(m_key_manager->remove(account, password_text));
					if (!error)
					{
						error_code_l = mcp::rpc_account_remove_error_code::ok;
						rpc_response(response, int(error_code_l), err.msg(error_code_l));
					}
					else
					{
						error_code_l = mcp::rpc_account_remove_error_code::wrong_password;
						rpc_response(response, int(error_code_l), err.msg(error_code_l));
					}
				}
				else
				{
					error_code_l = mcp::rpc_account_remove_error_code::invalid_password;
					rpc_response(response, int(error_code_l), err.msg(error_code_l));
					return;
				}
			}
			else
			{
				error_code_l = mcp::rpc_account_remove_error_code::account_not_exisit;
				rpc_response(response, int(error_code_l), err.msg(error_code_l));
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_remove_error_code::invalid_account;
			rpc_response(response, int(error_code_l), err.msg(error_code_l));
		}
	}
	else
	{
		rpc_response(response, "HTTP RPC control is disabled");
	}*/
}

void mcp::rpc_handler::account_password_change(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account)) {
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	if (!request.count("old_password") || !request["old_password"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidOldPassword());
	}
	std::string old_password_text = request["old_password"];

	if (!request.count("new_password") || (!request["new_password"].is_string()))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidNewPassword());
	}
	std::string new_password_text = request["new_password"];

	if (!mcp::validatePasswordSize(new_password_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLengthPassword());
	}

	if (!mcp::validatePassword(new_password_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidCharactersPassword());
	}

	if (m_key_manager->change_password(account, old_password_text, new_password_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
	}
	
	/*if (rpc.config.enable_control)
	{
		mcp::rpc_account_password_change_error_code error_code_l;
		bool error(true);
		if (request.count("account") && request["account"].is_string())
		{
			error = !mcp::isAddress(request["account"]);
		}
		if (!error)
		{
			std::string account_text = request["account"];
			dev::Address account(account_text);
			auto exists(m_key_manager->exists(account));
			if (exists)
			{
				if (!request.count("old_password") || (!request["old_password"].is_string()))
				{
					error_code_l = mcp::rpc_account_password_change_error_code::invalid_old_password;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				std::string old_password_text = request["old_password"];

				if (!request.count("new_password") || (!request["new_password"].is_string()))
				{
					error_code_l = mcp::rpc_account_password_change_error_code::invalid_new_password;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				std::string new_password_text = request["new_password"];

				if (mcp::validatePasswordSize(new_password_text))
				{
					if (mcp::validatePassword(new_password_text))
					{
						auto error(m_key_manager->change_password(account, old_password_text, new_password_text));
						if (!error)
						{
							error_code_l = mcp::rpc_account_password_change_error_code::ok;
							rpc_response(response, (int)error_code_l, err.msg(error_code_l));
						}
						else
						{
							error_code_l = mcp::rpc_account_password_change_error_code::wrong_password;
							rpc_response(response, (int)error_code_l, err.msg(error_code_l));
						}
					}
					else
					{
						error_code_l = mcp::rpc_account_password_change_error_code::invalid_characters_password;
						rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					}
				}
				else
				{
					error_code_l = mcp::rpc_account_password_change_error_code::invalid_length_password;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
				}
			}
			else
			{
				error_code_l = mcp::rpc_account_password_change_error_code::account_not_exisit;
				rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_password_change_error_code::invalid_account;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		}
	}
	else
	{
		rpc_response(response, "HTTP RPC control is disabled");
	}*/
}

void mcp::rpc_handler::account_unlock(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account)) {
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	if (!request.count("password") || !request["password"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidPassword());
	}

	std::string password_text = request["password"];
	if (password_text.empty()) {
		BOOST_THROW_EXCEPTION(RPC_Error_EmptyPassword());
	}

	if (m_key_manager->unlock(account, password_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
	}

	/*if (rpc.config.enable_control)
	{
		mcp::rpc_account_unlock_error_code error_code_l;
		if (request.count("account") && request["account"].is_string())
		{
			std::string account_text = request["account"];
			if (mcp::isAddress(account_text))
			{
				dev::Address account(account_text);
				auto exists(m_key_manager->exists(account));
				if (exists)
				{
					if (request.count("password") && request["password"].is_string())
					{
						std::string password_text = request["password"];
						auto error(m_key_manager->unlock(account, password_text));
						if (!error)
						{
							error_code_l = mcp::rpc_account_unlock_error_code::ok;
							rpc_response(response, (int)error_code_l, err.msg(error_code_l));
						}
						else
						{
							error_code_l = mcp::rpc_account_unlock_error_code::wrong_password;
							rpc_response(response, (int)error_code_l, err.msg(error_code_l));
						}
					}
					else
					{
						error_code_l = mcp::rpc_account_unlock_error_code::invalid_password;
						rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					}
				}
				else
				{
					error_code_l = mcp::rpc_account_unlock_error_code::account_not_exisit;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
				}
			}
			else
			{
				error_code_l = mcp::rpc_account_unlock_error_code::invalid_account;
				rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_unlock_error_code::invalid_account;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		}
	}
	else
	{
		rpc_response(response, "HTTP RPC control is disabled");
	}*/
}

void mcp::rpc_handler::account_lock(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account)) {
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	m_key_manager->lock(account);

	/*if (rpc.config.enable_control)
	{
		mcp::rpc_account_lock_error_code error_code_l;
		if (request.count("account") && request["account"].is_string())
		{
			std::string account_text = request["account"];
			if (mcp::isAddress(account_text))
			{
				dev::Address account(account_text);
				auto exists(m_key_manager->exists(account));
				if (exists)
				{
					m_key_manager->lock(account);
					error_code_l = mcp::rpc_account_lock_error_code::ok;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
				}
				else
				{
					error_code_l = mcp::rpc_account_lock_error_code::account_not_exisit;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
				}
			}
			else
			{
				error_code_l = mcp::rpc_account_lock_error_code::invalid_account;
				rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_lock_error_code::invalid_account;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		}
	}
	else
	{
		rpc_response(response, "HTTP RPC control is disabled");
	}*/
}

void mcp::rpc_handler::account_export(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	mcp::key_content kc;
	if (!m_key_manager->find(account, kc)) {
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}
	
	j_response["json"] = kc.to_json();

/*
	mcp::rpc_account_export_error_code error_code_l;
	if (request.count("account") && request["account"].is_string())
	{
		std::string account_text = request["account"];
		if (mcp::isAddress(account_text))
		{
			dev::Address account(account_text);
			mcp::key_content kc;
			auto exists(m_key_manager->find(account, kc));
			if (exists)
			{
				std::string const &json(kc.to_json());
				j_response["json"] = json;

				error_code_l = mcp::rpc_account_export_error_code::ok;
				rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
			}
			else
			{
				error_code_l = mcp::rpc_account_export_error_code::account_not_exisit;
				rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_export_error_code::invalid_account;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		}
	}
	else
	{
		error_code_l = mcp::rpc_account_export_error_code::invalid_account;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	}*/
}

void mcp::rpc_handler::account_import(mcp::json & j_response)
{
	if (!request.count("json") || !request["json"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidJson());
	}

	std::string json_text = request["json"];
	bool gen_next_work_l(false);
	mcp::key_content kc;

	if (m_key_manager->import(json_text, kc, gen_next_work_l)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidJson());
	}
	
	//if (rpc.config.enable_control)
	//{
	//	mcp::rpc_account_import_error_code error_code_l;

	//	if (request.count("json") && request["json"].is_string())
	//	{
	//		std::string json_text = request["json"];
	//		bool gen_next_work_l(false);

	//		mcp::key_content kc;
	//		auto error(m_key_manager->import(json_text, kc, gen_next_work_l));
	//		if (!error)
	//		{
	//			j_response["account"] = kc.account.hexPrefixed();
	//			error_code_l = mcp::rpc_account_import_error_code::ok;
	//			//rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
	//		}
	//		else
	//		{
	//			error_code_l = mcp::rpc_account_import_error_code::invalid_json;
	//			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		}
	//	}
	//	else
	//	{
	//		error_code_l = mcp::rpc_account_import_error_code::invalid_json;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	}
	//}
	//else
	//{
	//	rpc_response(response, "HTTP RPC control is disabled");
	//}
}

void mcp::rpc_handler::account_code(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	Address account(account_text);
	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
	
	j_response["account_code"] = bytes_to_hex(c_state.code(account));
}

void mcp::rpc_handler::account_balance(mcp::json & j_response)
{
	if (!request.count("account") || !request["account"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text)) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

	auto balance(c_state.balance(dev::Address(account_text)));
	j_response["balance"] = balance.convert_to<std::string>();
}

void mcp::rpc_handler::accounts_balances(mcp::json & j_response)
{
	mcp::rpc_accounts_balances_error_code error_code_l;

	mcp::json j_balances = mcp::json::array();
	if (request.count("accounts") == 0)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	for (mcp::json const &j_account : request["accounts"])
	{
		std::string account_text = j_account;
		if (!mcp::isAddress(account_text))
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
		}
		
		dev::Address account(account_text);
		mcp::db::db_transaction transaction(m_store.create_transaction());
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
		auto balance(c_state.balance(account));
		j_balances.push_back(balance.convert_to<std::string>());
	}

	j_response["balances"] = j_balances;
}

void mcp::rpc_handler::account_block_list(mcp::json & j_response)
{
	/*mcp::rpc_account_block_list_error_code error_code_l;

	if (!request.count("account") || (!request["account"].is_string()))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	dev::Address account(account_text);

	uint64_t limit_l(0);
	if (!request.count("limit"))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (!try_get_uint64_t_from_json("limit", limit_l))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (limit_l > list_max_limit)
	{
		error_code_l = mcp::rpc_account_block_list_error_code::limit_too_large;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::account_state_hash search_hash(0);

	if (request.count("index"))
	{
		if (request["index"].is_string())
		{
			std::string index = request["index"];
			if (!index.empty())
			{
				mcp::account_state_hash last_state_hash;
				if (last_state_hash.decode_hex(index))
				{
					error_code_l = mcp::rpc_account_block_list_error_code::invalid_index;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, last_state_hash));
				if (!(acc_state && acc_state->account == account))
				{
					error_code_l = mcp::rpc_account_block_list_error_code::index_not_exsist;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				search_hash = last_state_hash;
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_block_list_error_code::invalid_index;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	else
	{
		std::shared_ptr<mcp::account_state> acc_state(m_cache->latest_account_state_get(transaction, account));
		if (acc_state)
			search_hash = acc_state->hash();
	}

	mcp::json resp_l;
	mcp::json blocks_l = mcp::json::array();

	int i = 0;
	while (i < limit_l && !search_hash.is_zero())
	{
		std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, search_hash));
		if (!acc_state)
			break;
		mcp::block_hash b_hash(acc_state->block_hash);
		auto block = m_cache->block_get(transaction, b_hash);
		assert_x(block);

		mcp::json block_l;
		block->serialize_json(block_l);
		blocks_l.push_back(block_l);

		search_hash = acc_state->previous;
		i++;
	}
	resp_l["blocks"] = blocks_l;

	std::string index;
	if (!search_hash.is_zero())
		resp_l["next_index"] = search_hash.to_string();
	else
		resp_l["next_index"] = nullptr;

	error_code_l = mcp::rpc_account_block_list_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), resp_l);*/
}

void mcp::rpc_handler::account_state_list(mcp::json & j_response)
{
	/*mcp::rpc_account_block_list_error_code error_code_l;

	if (!request.count("account") || (!request["account"].is_string()))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	dev::Address account(account_text);

	uint64_t limit_l(0);
	if (!request.count("limit"))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (!try_get_uint64_t_from_json("limit", limit_l))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (limit_l > list_max_limit)
	{
		error_code_l = mcp::rpc_account_block_list_error_code::limit_too_large;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::account_state_hash search_hash(0);

	if (request.count("index"))
	{
		if (request["index"].is_string())
		{
			std::string index = request["index"];
			if (!index.empty())
			{
				mcp::account_state_hash last_state_hash;
				if (last_state_hash.decode_hex(index))
				{
					error_code_l = mcp::rpc_account_block_list_error_code::invalid_index;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, last_state_hash));
				if (!(acc_state && acc_state->account == account))
				{
					error_code_l = mcp::rpc_account_block_list_error_code::index_not_exsist;
					rpc_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				search_hash = last_state_hash;
			}
		}
		else
		{
			error_code_l = mcp::rpc_account_block_list_error_code::invalid_index;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	else
	{
		std::shared_ptr<mcp::account_state> acc_state(m_cache->latest_account_state_get(transaction, account));
		if (acc_state)
			search_hash = acc_state->hash();
	}

	mcp::json resp_l;
	mcp::json acc_states_l = mcp::json::array();

	int i = 0;
	while (i < limit_l && !search_hash.is_zero())
	{
		std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, search_hash));
		if (!acc_state)
			break;

		mcp::json acc_state_l;
		acc_state_l["hash"] = acc_state->hash().to_string();
		acc_state_l["account"] = acc_state->account.hexPrefixed();
		acc_state_l["block_hash"] = acc_state->block_hash.to_string();
		acc_state_l["previous"] = acc_state->previous.to_string();
		acc_state_l["balance"] = acc_state->balance.str();
		acc_state_l["nonce"] = acc_state->nonce().str();
		acc_state_l["storage_root"] = acc_state->baseRoot().hex();
		acc_state_l["code_hash"] = acc_state->codeHash().hex();
		acc_state_l["is_alive"] = acc_state->isAlive();

		acc_states_l.push_back(acc_state_l);

		search_hash = acc_state->previous;
		i++;
	}
	resp_l["account_states"] = acc_states_l;

	std::string index;
	if (!search_hash.is_zero())
		resp_l["next_index"] = search_hash.to_string();
	else
		resp_l["next_index"] = nullptr;

	error_code_l = mcp::rpc_account_block_list_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), resp_l);*/
}

void mcp::rpc_handler::block(mcp::json & j_response)
{
	if (!request.count("hash") || !request["hash"].is_string()) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}
	std::string hash_text = request["hash"];
	mcp::uint256_union hash;
	bool error(hash.decode_hex(hash_text));

	if (error) {
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, hash));

	if (block != nullptr)
	{
		mcp::json block_l;
		block->serialize_json(block_l);
		j_response["block"] = block_l;
	}
	else
	{
		j_response["block"] = nullptr;
	}

	//mcp::rpc_block_error_code error_code_l;

	//if (!request.count("hash") || (!request["hash"].is_string()))
	//{
	//	error_code_l = mcp::rpc_block_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//std::string hash_text = request["hash"];
	//mcp::uint256_union hash;
	//auto error(hash.decode_hex(hash_text));
	//if (!error)
	//{
	//	mcp::json response_l;
	//	mcp::db::db_transaction transaction(m_store.create_transaction());
	//	auto block(m_cache->block_get(transaction, hash));
	//	//if (block == nullptr)
	//	//{
	//	//	auto unlink_block(m_cache->unlink_block_get(transaction, hash));
	//	//	if (unlink_block)
	//	//		block = unlink_block->block;
	//	//}

	//	if (block != nullptr)
	//	{
	//		mcp::json block_l;
	//		block->serialize_json(block_l);
	//		response_l["block"] = block_l;
	//		error_code_l = mcp::rpc_block_error_code::ok;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	//	}
	//	else
	//	{
	//		response_l["block"] = nullptr;
	//		error_code_l = mcp::rpc_block_error_code::ok;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	//	}
	//}
	//else
	//{
	//	error_code_l = mcp::rpc_block_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//}
}

void mcp::rpc_handler::blocks(mcp::json & j_response)
{
	/*mcp::rpc_blocks_error_code error_code_l;

	std::vector<std::string> hashes;
	mcp::json response_l;
	mcp::json blocks_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	if (!request.count("hashes") || (!request["hashes"].is_array()))
	{
		error_code_l = mcp::rpc_blocks_error_code::invalid_hash;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::vector<std::string> hashes_l = request["hashes"];
	for (std::string const &hash_text : hashes_l)
	{
		mcp::uint256_union hash;
		auto error(hash.decode_hex(hash_text));
		if (!error)
		{
			auto block(m_cache->block_get(transaction, hash));
			mcp::json block_l;
			if (block != nullptr)
				block->serialize_json(block_l);
			else
				block_l = nullptr;
			blocks_l.push_back(block_l);
		}
		else
		{
			error_code_l = mcp::rpc_blocks_error_code::invalid_hash;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l) + "," + hash_text);
			return;
		}
	}
	response_l["blocks"] = blocks_l;
	error_code_l = mcp::rpc_blocks_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);*/
}

void mcp::rpc_handler::block_state(mcp::json & j_response)
{
	mcp::rpc_block_error_code error_code_l;

	if (!request.count("hash") || (!request["hash"].is_string()))
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string hash_text = request["hash"];
	mcp::uint256_union hash;
	auto error(hash.decode_hex(hash_text));
	if (!error)
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());
		std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, hash));
		if (state != nullptr)
		{
			auto block(m_cache->block_get(transaction, hash));
			assert_x(block);

			mcp::json block_state_l;
			block_state_l["hash"] = hash.to_string();
			//dev::Address contract_account(0);
			//if (block->type() == mcp::block_type::light && block->isCreation() && state->is_stable && (state->status == mcp::block_status::ok))
			//{
			//	std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, state->receipt->from_state));
			//	assert_x(acc_state);
			//	contract_account = toAddress(block->from(), acc_state->nonce() - 1);
			//}

			state->serialize_json(block_state_l);

			j_response["block_state"] = block_state_l;
			error_code_l = mcp::rpc_block_error_code::ok;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
		}
		else
		{
			j_response["block_state"] = nullptr;
			error_code_l = mcp::rpc_block_error_code::ok;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
		}
	}
	else
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	}
}

void mcp::rpc_handler::block_states(mcp::json & j_response)
{
	//mcp::rpc_blocks_error_code error_code_l;

	//std::vector<std::string> hashes;
	//mcp::json response_l;
	//mcp::json states_l = mcp::json::array();
	//mcp::db::db_transaction transaction(m_store.create_transaction());

	//if (!request.count("hashes") || (!request["hashes"].is_array()))
	//{
	//	error_code_l = mcp::rpc_blocks_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//std::vector<std::string> hashes_l = request["hashes"];
	//for (std::string const &hash_text : hashes_l)
	//{
	//	mcp::uint256_union hash;
	//	auto error(hash.decode_hex(hash_text));
	//	if (!error)
	//	{
	//		auto state(m_cache->block_state_get(transaction, hash));
	//		mcp::json state_l;
	//		if (state != nullptr)
	//		{
	//			auto block(m_cache->block_get(transaction, hash));
	//			assert_x(block);

	//			state_l["hash"] = hash.to_string();
	//			//dev::Address contract_address;
	//			//if (block->type() == mcp::block_type::light && block->isCreation() && state->is_stable && (state->status == mcp::block_status::ok))
	//			//{
	//			//	std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, state->receipt->from_state));
	//			//	assert_x(acc_state);
	//			//	contract_address = toAddress(block->from(), acc_state->nonce() - 1);
	//			//}
	//			state->serialize_json(state_l);
	//		}
	//		else
	//			state_l = nullptr;

	//		states_l.push_back(state_l);
	//	}
	//	else
	//	{
	//		error_code_l = mcp::rpc_blocks_error_code::invalid_hash;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l) + "," + hash_text);
	//		return;
	//	}
	//}
	//response_l["block_states"] = states_l;
	//error_code_l = mcp::rpc_blocks_error_code::ok;
	//rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::block_traces(mcp::json & j_response)
{
	/*mcp::rpc_block_error_code error_code_l;

	if (!request.count("hash") || (!request["hash"].is_string()))
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string hash_text = request["hash"];
	mcp::uint256_union hash;
	auto error(hash.decode_hex(hash_text));
	if (!error)
	{
		mcp::json response_l;
		mcp::db::db_transaction transaction(m_store.create_transaction());
		std::list<std::shared_ptr<mcp::trace>> traces;
		m_store.traces_get(transaction, hash, traces);

		mcp::json traces_l = mcp::json::array();
		std::deque<uint32_t> trace_address;
		for (auto it(traces.begin()); it != traces.end(); it++)
		{
			std::shared_ptr<mcp::trace> trace(*it);
			mcp::json trace_l;
			trace->serialize_json(trace_l);

			uint32_t const &depth(trace->depth);

			uint32_t sub_traces(0);
			auto it_temp = it;
			while (++it_temp != traces.end())
			{
				std::shared_ptr<mcp::trace> t(*it_temp);
				if (t->depth <= depth)
					break;

				if (t->depth == depth + 1)
					sub_traces++;
			}
			trace_l["subtraces"] = sub_traces;

			if (depth > 0)
			{
				if (trace_address.size() < depth)
				{
					assert_x(trace_address.size() == depth - 1);
					trace_address.push_back(0);
				}
				else
				{
					trace_address[depth - 1] += 1;
					while (trace_address.size() > depth)
						trace_address.pop_back();
				}
			}
			trace_l["trace_address"] = trace_address;

			traces_l.push_back(trace_l);
		}

		response_l["block_traces"] = traces_l;
		error_code_l = mcp::rpc_block_error_code::ok;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	}
	else
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	}*/
}

void mcp::rpc_handler::stable_blocks(mcp::json & j_response)
{
	/*bool error(false);
	mcp::rpc_stable_blocks_error_code error_code_l;

	uint64_t index(0);
	if (request.count("index"))
	{
		if (!try_get_uint64_t_from_json("index", index))
		{
			error_code_l = mcp::rpc_stable_blocks_error_code::invalid_index;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	uint64_t limit_l(0);
	if (!request.count("limit"))
	{
		error_code_l = mcp::rpc_stable_blocks_error_code::invalid_limit;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	if (!try_get_uint64_t_from_json("limit", limit_l))
	{
		error_code_l = mcp::rpc_stable_blocks_error_code::invalid_limit;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (limit_l > list_max_limit)
	{
		error_code_l = mcp::rpc_stable_blocks_error_code::limit_too_large;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	uint64_t last_stable_index(m_chain->last_stable_index());

	mcp::json response_l;
	mcp::json block_list_l = mcp::json::array();
	int blocks_count(0);
	for (uint64_t stable_index = index; stable_index <= last_stable_index; stable_index++)
	{
		mcp::block_hash block_hash_l;
		bool exists(!m_store.stable_block_get(transaction, stable_index, block_hash_l));
		assert_x(exists);

		auto block = m_cache->block_get(transaction, block_hash_l);
		assert_x(block);

		mcp::json block_l;
		block->serialize_json(block_l);
		block_list_l.push_back(block_l);

		blocks_count++;
		if (blocks_count == limit_l)
			break;
	}

	response_l["blocks"] = block_list_l;

	uint64_t next_index = index + limit_l;
	if (next_index <= last_stable_index)
		response_l["next_index"] = next_index;
	else
		response_l["next_index"] = nullptr;

	error_code_l = mcp::rpc_stable_blocks_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);*/
}

void mcp::rpc_handler::estimate_gas(mcp::json & j_response)
{
	//mcp::rpc_estimate_gas_error_code error_code_l;
	//if (!rpc.config.enable_control)
	//{
	//	rpc_response(response, "RPC control is disabled");
	//	return;
	//}

	//// sichaoy: remove the "from" field, and to pick it by default
	//dev::Address from(0);
	//if (request.count("from"))
	//{
	//	if (!request["from"].is_string())
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_from;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}

	//	std::string from_text = request["from"];
	//	bool error(from.decode_account(from_text));
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_from;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//dev::Address to(0);
	//if (request.count("to"))
	//{
	//	if (!request["to"].is_string())
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_to;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}

	//	std::string to_text = request["to"];
	//	bool error = to.decode_account(to_text);
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_to;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//mcp::amount amount(0);
	//if (request.count("amount"))
	//{
	//	if (!request["amount"].is_string())
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_amount;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//	std::string amount_text = request["amount"];
	//	bool error = !boost::conversion::try_lexical_convert(amount_text, amount);
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_amount;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//uint64_t gas(0);
	//if (request.count("gas"))
	//{
	//	if (!try_get_uint64_t_from_json("gas", gas))
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_gas;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//uint256_t gas_price(0);
	//if (request.count("gas_price"))
	//{
	//	uint256_t gas_price;
	//	if (!request["gas_price"].is_string() || !boost::conversion::try_lexical_convert(request["gas_price"].get<std::string>(), gas_price))
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_gas_price;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//dev::bytes data;
	//if (request.count("data"))
	//{
	//	std::string data_text = request["data"];
	//	bool error = mcp::hex_to_bytes(data_text, data);
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_data;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}

	//	if (data.size() > mcp::max_data_size)
	//	{
	//		error_code_l = mcp::rpc_estimate_gas_error_code::data_size_too_large;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//dev::eth::McInfo mc_info;
	//if (!try_get_mc_info(mc_info))
	//{
	//	error_code_l = mcp::rpc_estimate_gas_error_code::invalid_mci;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//mcp::db::db_transaction transaction(m_store.create_transaction());
	//std::pair<u256, bool> result = m_chain->estimate_gas(transaction, m_cache, from, amount, to, data, gas, gas_price, mc_info);

	//mcp::json response_l;
	//if (!result.second)
	//{
	//	error_code_l = mcp::rpc_estimate_gas_error_code::gas_not_enough_or_fail;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	//	return;
	//}

	//response_l["gas"] = result.first;
	//error_code_l = mcp::rpc_estimate_gas_error_code::ok;
	//rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::call(mcp::json & j_response)
{
	//mcp::rpc_call_error_code error_code_l;
	//if (!rpc.config.enable_control)
	//{
	//	rpc_response(response, "RPC control is disabled");
	//	return;
	//}

	//dev::Address from(0);
	//if (request.count("from"))
	//{
	//	if (!request["from"].is_string())
	//	{
	//		error_code_l = mcp::rpc_call_error_code::invalid_account_from;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}

	//	// sichaoy: remove the "from" field, and to pick it by default
	//	std::string from_text = request["from"];
	//	auto error(from.decode_account(from_text));
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_call_error_code::invalid_account_from;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//if (!request.count("to") || (!request["to"].is_string()))
	//{
	//	error_code_l = mcp::rpc_call_error_code::invalid_account_to;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//std::string to_text = request["to"];
	//dev::Address to;
	//bool error = to.decode_account(to_text);
	//if (error)
	//{
	//	error_code_l = mcp::rpc_call_error_code::invalid_account_to;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//dev::bytes data;
	//if (request.count("data"))
	//{
	//	std::string data_text = request["data"];
	//	error = mcp::hex_to_bytes(data_text, data);
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_call_error_code::invalid_data;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}

	//	if (data.size() > mcp::max_data_size)
	//	{
	//		error_code_l = mcp::rpc_call_error_code::data_size_too_large;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//dev::eth::McInfo mc_info;
	//if (!try_get_mc_info(mc_info))
	//{
	//	error_code_l = mcp::rpc_call_error_code::invalid_mci;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//std::shared_ptr<mcp::block> block = std::make_shared<mcp::block>(
	//	mcp::block_type::light,														// mcp::block_type type_a,
	//	from,																		// dev::Address const & from_a,
	//	to,																			// dev::Address const & to_a,
	//	0,																			// mcp::amount const & amount_a,
	//	0,																			// mcp::block_hash const & previous_a,
	//	std::vector<mcp::block_hash>{},												// std::vector<mcp::block_hash> const & parents_a,
	//	std::make_shared<std::list<mcp::block_hash>>(std::list<mcp::block_hash>{}), // std::shared_ptr<std::list<mcp::block_hash>> links_a,
	//	0,																			// mcp::summary_hash const & last_summary_a,
	//	0,																			// mcp::block_hash const & last_summary_block_a,
	//	0,																			// mcp::block_hash const & last_stable_block_a,
	//	mcp::uint256_t(mcp::block_max_gas),											// uint256_t gas_a,
	//	0,																			// uint256_t gas_price_a,
	//	data,																		// std::vector<uint8_t> const & data_a,
	//	0																			// uint64_t const & exec_timestamp_a,
	//);

	//mcp::db::db_transaction transaction(m_store.create_transaction());
	//std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = m_chain->execute(transaction, m_cache, block, mc_info, Permanence::Reverted, dev::eth::OnOpFunc());

	//mcp::json response_l;
	//response_l["output"] = bytes_to_hex(result.first.output);

	//error_code_l = mcp::rpc_call_error_code::ok;
	//rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::logs(mcp::json & j_response)
{
	//mcp::rpc_logs_error_code error_code_l;

	//uint64_t from_stable_block_index(0);
	//if (request.count("from_stable_block_index") && !request["from_stable_block_index"].is_null())
	//{
	//	if (!try_get_uint64_t_from_json("from_stable_block_index", from_stable_block_index))
	//	{
	//		error_code_l = mcp::rpc_logs_error_code::invalid_from_stable_block_index;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}

	//uint64_t to_stable_block_index;
	//if (request.count("to_stable_block_index") && !request["to_stable_block_index"].is_null())
	//{
	//	if (!try_get_uint64_t_from_json("to_stable_block_index", to_stable_block_index))
	//	{
	//		error_code_l = mcp::rpc_logs_error_code::invalid_to_stable_block_index;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}
	//else
	//{
	//	to_stable_block_index = m_chain->last_stable_index();
	//}

	//boost::optional<dev::Address> search_account;
	//if (request.count("account") && !request["account"].is_null())
	//{
	//	if (!request["account"].is_string())
	//	{
	//		error_code_l = mcp::rpc_logs_error_code::invalid_account;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}

	//	std::string account_text = request["account"];
	//	dev::Address account;
	//	bool error = account.decode_account(account_text);
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_logs_error_code::invalid_account;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//	search_account = account;
	//}

	//std::vector<dev::h256> search_topics;
	//if (request.count("topics") && !request["topics"].is_null())
	//{
	//	if (!request["topics"].is_array())
	//	{
	//		error_code_l = mcp::rpc_logs_error_code::invalid_topics;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}

	//	std::vector<std::string> topics_l = request["topics"];
	//	for (std::string const &topic_text : topics_l)
	//	{
	//		mcp::uint256_union topic;
	//		auto error(topic.decode_hex(topic_text));
	//		if (error)
	//		{
	//			error_code_l = mcp::rpc_logs_error_code::invalid_topics;
	//			rpc_response(response, (int)error_code_l, err.msg(error_code_l) + "," + topic_text);
	//			return;
	//		}

	//		search_topics.push_back(dev::h256(topic.ref()));
	//	}
	//}

	//mcp::json response_l;
	//mcp::json logs_l = mcp::json::array();
	//mcp::db::db_transaction transaction(m_store.create_transaction());

	///*for (uint64_t i(from_stable_block_index); i <= to_stable_block_index; i++)
	//{
	//	mcp::block_hash block_hash;
	//	bool exists(!m_store.stable_block_get(transaction, i, block_hash));
	//	assert_x(exists);
	//	std::shared_ptr<mcp::block_state> block_state(m_cache->block_state_get(transaction, block_hash));
	//	assert_x(block_state);

	//	if (block_state->block_type != mcp::block_type::light)
	//		continue;

	//	if (!block_state->receipt)
	//		continue;

	//	if (search_account && !block_state->receipt->contains_bloom(search_account->ref()))
	//		continue;

	//	std::unordered_set<dev::h256> existed_topics;
	//	for (dev::h256 const &topic : search_topics)
	//	{
	//		if (block_state->receipt->contains_bloom(topic))
	//			existed_topics.insert(topic);
	//	}

	//	if (search_topics.size() > 0 && existed_topics.size() == 0)
	//		continue;

	//	for (auto const &log : block_state->receipt->log)
	//	{
	//		if (!search_account || log.acct == *search_account)
	//		{
	//			for (dev::h256 const &t : log.topics)
	//			{
	//				if (search_topics.size() == 0 || existed_topics.count(t))
	//				{
	//					mcp::json log_l;
	//					log.serialize_json(log_l);
	//					log_l["block_hash"] = block_hash.to_string();

	//					uint32_t log_id;
	//					blake2b_state hash_l;
	//					auto status(blake2b_init(&hash_l, sizeof(log_id)));
	//					assert_x(status == 0);
	//					blake2b_update(&hash_l, block_hash.bytes.data(), sizeof(block_hash.bytes));
	//					log.hash(hash_l);
	//					status = blake2b_final(&hash_l, &log_id, sizeof(log_id));
	//					assert_x(status == 0);
	//					std::stringstream stream;
	//					stream << std::hex << log_id;
	//					log_l["id"] = stream.str();

	//					logs_l.push_back(log_l);
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}*/

	//response_l["logs"] = logs_l;
	//error_code_l = mcp::rpc_logs_error_code::ok;
	//rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::send_block(mcp::json & j_response)
{
	try
	{
		// optional, account locked but input password will unlock.or else return account locked.
		boost::optional<std::string> password;
		if (request.count("password") && request["password"].is_string()) {
			password = request["password"].get<std::string>();
		}

		auto rpc_l(shared_from_this());
		auto fun = [rpc_l, j_response, this](h256 h)
		{
			mcp::json j_res = j_response;
			j_res["result"] = toJS(h);
			response(j_res);
		};

		TransactionSkeleton t = mcp::toTransactionSkeleton(request);
		m_wallet->send_async(t, fun, password);
	}
	catch (...)
	{
		//throw JsonRpcException(exceptionToErrorMessage());
	}
}

void mcp::rpc_handler::generate_offline_block(mcp::json & j_response)
{
	//if (!rpc.config.enable_control)
	//{
	//	rpc_response(response, "RPC control is disabled");
	//	return;
	//}

	//mcp::rpc_generate_offline_block_error_code error_code_l;

	//boost::optional<mcp::block_hash> previous_opt;
	//if (request.count("previous"))
	//{
	//	if (!request["previous"].is_string())
	//	{
	//		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_previous;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//	std::string previous_text = request["previous"];
	//	mcp::block_hash previous;
	//	auto error(previous.decode_hex(previous_text));
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_previous;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//	previous_opt = previous;
	//}

	//if (!request.count("from") || (!request["from"].is_string()))
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_from;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}
	//std::string from_text = request["from"];
	//dev::Address from;
	//auto error(from.decode_account(from_text));
	//if (error)
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_from;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//if (!request.count("to") || (!request["to"].is_string()))
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_to;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}
	//std::string to_text = request["to"];
	//dev::Address to;
	//error = to.decode_account(to_text);
	//if (error)
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_to;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//if (!request.count("amount") || (!request["amount"].is_string()))
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_amount;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}
	//std::string amount_text = request["amount"];
	//mcp::amount amount;
	//error = !boost::conversion::try_lexical_convert(amount_text, amount);
	//if (error)
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_amount;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//uint64_t gas_u64;
	//if (!try_get_uint64_t_from_json("gas", gas_u64))
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_gas;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}
	//uint256_t gas(gas_u64);

	//uint256_t gas_price;
	//if (!request.count("gas_price") || !request["gas_price"].is_string() || !boost::conversion::try_lexical_convert(request["gas_price"].get<std::string>(), gas_price))
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_gas_price;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//dev::bytes data;
	//if (request.count("data"))
	//{
	//	bool error(false);
	//	if (request["data"].is_string())
	//	{
	//		std::string data_text = request["data"];
	//		error = mcp::hex_to_bytes(data_text, data);
	//	}
	//	else
	//	{
	//		error = true;
	//	}
	//	if (error)
	//	{
	//		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_data;
	//		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//		return;
	//	}
	//}
	//if (data.size() > mcp::max_data_size)
	//{
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::data_size_too_large;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//mcp::db::db_transaction transaction(m_store.create_transaction());
	//std::shared_ptr<mcp::block> block;
	///*mcp::compose_result_codes compose_block_result(m_wallet->composer->compose_block(transaction, mcp::block_type::light, previous_opt, from, to, amount, gas, gas_price, data, block));
	//switch (compose_block_result)
	//{
	//case mcp::compose_result_codes::ok:
	//{
	//	mcp::json response_l;

	//	response_l["hash"] = block->hash().to_string();
	//	response_l["from"] = block->from().hexPrefixed();
	//	response_l["to"] = block->hashables->to.hexPrefixed();
	//	response_l["amount"] = block->hashables->amount.str();
	//	response_l["previous"] = block->hashables->previous.to_string();
	//	response_l["gas"] = block->hashables->gas.str();
	//	response_l["gas_price"] = block->hashables->gas_price.str();
	//	response_l["data"] = mcp::bytes_to_hex(block->hashables->data);

	//	error_code_l = mcp::rpc_generate_offline_block_error_code::ok;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	//	break;
	//}
	//case mcp::compose_result_codes::insufficient_balance:
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::insufficient_balance;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	break;
	//case mcp::compose_result_codes::data_size_too_large:
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::data_size_too_large;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	break;
	//case mcp::compose_result_codes::validate_error:
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::validate_error;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	break;
	//case mcp::compose_result_codes::error:
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::compose_error;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	break;
	//default:
	//	error_code_l = mcp::rpc_generate_offline_block_error_code::compose_unknown_error;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	break;
	//}*/
}

void mcp::rpc_handler::send_offline_block(mcp::json & j_response)
{
	if (!rpc.config.enable_control)
	{
		rpc_response(response, "RPC control is disabled");
		return;
	}
	mcp::rpc_send_offline_block_error_code error_code_l;

	if (!request.count("from") || (!request["from"].is_string()))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_from;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	std::string from_text = request["from"];
	auto error(!mcp::isAddress(from_text));
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_from;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	dev::Address from(from_text);

	if (!request.count("to") || (!request["to"].is_string()))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_to;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	std::string to_text = request["to"];
	error = !mcp::isAddress(to_text);
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_to;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	dev::Address to(to_text);

	if (!request.count("amount") || (!request["amount"].is_string()))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_amount;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string amount_text = request["amount"];
	mcp::amount amount;
	error = !boost::conversion::try_lexical_convert(amount_text, amount);
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_amount;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	uint64_t gas_u64;
	if (!try_get_uint64_t_from_json("gas", gas_u64))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_gas;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	uint256_t gas(gas_u64);

	uint256_t gas_price;
	if (!request.count("gas_price") || !request["gas_price"].is_string() || !boost::conversion::try_lexical_convert(request["gas_price"].get<std::string>(), gas_price))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_gas_price;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	dev::bytes data;
	if (request.count("data"))
	{
		bool error(false);
		if (request["data"].is_string())
		{
			std::string data_text = request["data"];
			error = mcp::hex_to_bytes(data_text, data);
		}
		else
		{
			error = true;
		}
		if (error)
		{
			error_code_l = mcp::rpc_send_offline_block_error_code::invalid_data;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	if (data.size() > mcp::max_data_size)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::data_size_too_large;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (!request.count("previous") || (!request["previous"].is_string()))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_previous;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	std::string previous_text = request["previous"];
	mcp::block_hash previous;
	error = previous.decode_hex(previous_text);
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_previous;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (!request.count("signature") || (!request["signature"].is_string()))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_signature;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string signature_text = request["signature"];
	dev::Signature signature(signature_text);

	std::vector<mcp::block_hash> parents;
	std::shared_ptr<std::list<mcp::block_hash>> links = std::make_shared<std::list<mcp::block_hash>>();
	mcp::block_hash last_summary(0);
	mcp::block_hash last_summary_block(0);
	mcp::block_hash last_stable_block(0);
	uint64_t exec_timestamp(0);

	//std::shared_ptr<mcp::block> p_block = std::make_shared<mcp::block>(mcp::block_type::light, from, to, amount, previous, parents, links,
	//																   last_summary, last_summary_block, last_stable_block, gas, gas_price, data, exec_timestamp);

	//assert_x(p_block != nullptr);

	bool async(false);
	if (request.count("async"))
	{
		if (!try_get_bool_from_json("async", async))
		{
			error_code_l = mcp::rpc_send_offline_block_error_code::invalid_async;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	/*bool gen_next_work_l(false);
	auto rpc_l(shared_from_this());
	m_wallet->send_async(
		p_block, signature, [rpc_l, this](mcp::send_result result)
		{
		mcp::rpc_send_offline_block_error_code error_code_l;
		switch (result.code)
		{
		case mcp::send_result_codes::ok:
		{
			mcp::uint256_union hash(result.block->hash());
			mcp::json response_l;
			response_l["hash"] = hash.to_string();

			error_code_l = mcp::rpc_send_offline_block_error_code::ok;
			rpc_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l), response_l);
			break;
		}
		case mcp::send_result_codes::insufficient_balance:
			error_code_l = mcp::rpc_send_offline_block_error_code::insufficient_balance;
			rpc_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::data_size_too_large:
			error_code_l = mcp::rpc_send_offline_block_error_code::data_size_too_large;
			rpc_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::validate_error:
			error_code_l = mcp::rpc_send_offline_block_error_code::validate_error;
			if (result.msg != "")
				rpc_response(response, (int)error_code_l, result.msg);
			else
				rpc_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::error:
			error_code_l = mcp::rpc_send_offline_block_error_code::send_block_error;
			rpc_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		default:
			error_code_l = mcp::rpc_send_offline_block_error_code::send_unknown_error;
			rpc_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		} },
		gen_next_work_l, async);*/
}

void mcp::rpc_handler::block_summary(mcp::json & j_response)
{
	//mcp::rpc_block_error_code error_code_l;

	//if (!request.count("hash") || (!request["hash"].is_string()))
	//{
	//	error_code_l = mcp::rpc_block_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//std::string hash_text = request["hash"];
	//mcp::uint256_union hash;
	//auto error(hash.decode_hex(hash_text));
	//if (!error)
	//{
	//	mcp::json response_l;
	//	mcp::db::db_transaction transaction(m_store.create_transaction());
	//	mcp::summary_hash summary;
	//	bool exists(!m_cache->block_summary_get(transaction, hash, summary));
	//	if (!exists)
	//	{
	//		response_l["summary"] = nullptr;
	//	}
	//	else
	//	{
	//		response_l["summary"] = summary.to_string();

	//		auto block(m_cache->block_get(transaction, hash));
	//		assert_x(block);
	//		auto block_state(m_cache->block_state_get(transaction, hash));
	//		assert_x(block_state);

	//		// previous summary hash
	//		mcp::summary_hash previous_summary_hash(0);
	//		if (!block->previous().is_zero())
	//		{
	//			bool previous_summary_hash_error(m_cache->block_summary_get(transaction, block->previous(), previous_summary_hash));
	//			assert_x(!previous_summary_hash_error);
	//		}
	//		response_l["previous_summary"] = previous_summary_hash.to_string();

	//		// parent summary hashs
	//		mcp::json parent_summaries_l = mcp::json::array();
	//		for (mcp::block_hash const &pblock_hash : block->parents())
	//		{
	//			mcp::summary_hash p_summary_hash;
	//			bool p_summary_hash_error(m_cache->block_summary_get(transaction, pblock_hash, p_summary_hash));
	//			assert_x(!p_summary_hash_error);

	//			parent_summaries_l.push_back(p_summary_hash.to_string());
	//		}
	//		response_l["parent_summaries"] = parent_summaries_l;

	//		// link summary hashs
	//		std::shared_ptr<std::list<mcp::block_hash>> links(block->links());
	//		mcp::json link_summaries_l = mcp::json::array();
	//		for (auto it(links->begin()); it != links->end(); it++)
	//		{
	//			mcp::block_hash const &link_hash(*it);
	//			mcp::summary_hash l_summary_hash;
	//			bool l_summary_hash_error(m_cache->block_summary_get(transaction, link_hash, l_summary_hash));
	//			assert_x(!l_summary_hash_error);

	//			link_summaries_l.push_back(l_summary_hash.to_string());
	//		}
	//		response_l["link_summaries"] = link_summaries_l;

	//		// skip list
	//		mcp::json skiplist_summaries_l = mcp::json::array();
	//		if (block_state->is_on_main_chain)
	//		{
	//			std::set<mcp::summary_hash> summary_skiplist;
	//			std::vector<uint64_t> skip_list_mcis = m_chain->cal_skip_list_mcis(*block_state->main_chain_index);
	//			for (uint64_t &mci : skip_list_mcis)
	//			{
	//				mcp::block_hash sl_block_hash;
	//				bool sl_block_hash_error(m_store.main_chain_get(transaction, mci, sl_block_hash));
	//				assert_x(!sl_block_hash_error);

	//				mcp::summary_hash sl_summary_hash;
	//				bool sl_summary_hash_error(m_cache->block_summary_get(transaction, sl_block_hash, sl_summary_hash));
	//				assert_x(!sl_summary_hash_error);
	//				summary_skiplist.insert(sl_summary_hash);
	//			}

	//			for (mcp::summary_hash s : summary_skiplist)
	//				skiplist_summaries_l.push_back(s.to_string());
	//		}
	//		response_l["skiplist_summaries"] = skiplist_summaries_l;

	//		response_l["status"] = (uint64_t)block_state->status;

	//		//if (block_state->receipt)
	//		//{
	//		//	response_l["from_state"] = block_state->receipt->from_state.to_string();
	//		//	mcp::json to_states_l = mcp::json::array();
	//		//	for (auto sh : block_state->receipt->to_state)
	//		//	{
	//		//		to_states_l.push_back(sh.to_string());
	//		//	}
	//		//	response_l["to_states"] = to_states_l;
	//		//}
	//		//else
	//		{
	//			response_l["from_state"] = nullptr;
	//			response_l["to_states"] = nullptr;
	//		}
	//	}

	//	error_code_l = mcp::rpc_block_error_code::ok;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	//}
	//else
	//{
	//	error_code_l = mcp::rpc_block_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//}
}

void mcp::rpc_handler::sign_msg(mcp::json & j_response)
{
	/*mcp::rpc_sign_msg_error_code error_code_l;

	if (!request.count("account") || (!request["account"].is_string()))
	{
		error_code_l = mcp::rpc_sign_msg_error_code::invalid_public_key;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string account_text = request["account"];
	auto error(!mcp::isAddress(account_text));
	if (error)
	{
		error_code_l = mcp::rpc_sign_msg_error_code::invalid_public_key;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	dev::Address account(account_text);

	if (!request.count("msg") || (!request["msg"].is_string()))
	{
		error_code_l = mcp::rpc_sign_msg_error_code::invalid_msg;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	std::string sign_msg_text = request["msg"];
	mcp::uint256_union sign_msg;
	error = sign_msg.decode_hex(sign_msg_text);
	if (error)
	{
		error_code_l = mcp::rpc_sign_msg_error_code::invalid_msg;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (!request.count("password") || (!request["password"].is_string()))
	{
		error_code_l = mcp::rpc_sign_msg_error_code::invalid_password;
		rpc_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	std::string password_a = request["password"];
	mcp::raw_key prv;
	if (password_a.empty())
	{
		bool exists(m_key_manager->find_unlocked_prv(account, prv));
		if (!exists)
		{
			error_code_l = mcp::rpc_sign_msg_error_code::wrong_password;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	else
	{
		bool error(m_key_manager->decrypt_prv(account, password_a, prv));
		if (error)
		{
			error_code_l = mcp::rpc_sign_msg_error_code::wrong_password;
			rpc_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	mcp::json response_l;
	dev::Signature sig(mcp::sign_message(prv, sign_msg));
	response_l["signature"] = sig.to_string();
	error_code_l = mcp::rpc_sign_msg_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);*/
}

void mcp::rpc_handler::version(mcp::json & j_response)
{
	mcp::rpc_version_error_code error_code_l = mcp::rpc_version_error_code::ok;
	mcp::json response_l;
	j_response["version"] = STR(MCP_VERSION);
	j_response["rpc_version"] = "1";
	j_response["store_version"] = std::to_string(m_store.version_get());

	rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
}

void mcp::rpc_handler::status(mcp::json & j_response)
{
	// stable_mci
	mcp::rpc_status_error_code error_code_l;
	uint64_t last_stable_mci(m_chain->last_stable_mci());
	uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());
	
	j_response["syncing"] = mcp::node_sync::is_syncing() ? 1 : 0;
	j_response["last_stable_mci"] = last_stable_mci;
	j_response["last_mci"] = last_mci;
	j_response["last_stable_block_index"] = last_stable_index;
	
	error_code_l = mcp::rpc_status_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
}

void mcp::rpc_handler::peers(mcp::json & j_response)
{
	mcp::rpc_peers_error_code error_code_l;

	mcp::json response_l;
	mcp::json peers_l = mcp::json::array();
	std::unordered_map<p2p::node_id, bi::tcp::endpoint> peers(m_host->peers());
	for (auto i : peers)
	{
		p2p::node_id id(i.first);
		bi::tcp::endpoint endpoint(i.second);

		mcp::json peer_l;
		peer_l["id"] = id.hex();
		std::stringstream ss_endpoint;
		ss_endpoint << endpoint;
		peer_l["endpoint"] = ss_endpoint.str();
		peers_l.push_back(peer_l);
	}
	j_response["peers"] = peers_l;

	error_code_l = mcp::rpc_peers_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
}

void mcp::rpc_handler::nodes(mcp::json & j_response)
{
	mcp::rpc_nodes_error_code error_code_l;

	mcp::json response_l;
	mcp::json nodes_l = mcp::json::array();
	std::list<p2p::node_info> nodes(m_host->nodes());
	for (p2p::node_info node : nodes)
	{
		mcp::json node_l;
		node_l["id"] = node.id.hex();
		std::stringstream ss_endpoint;
		ss_endpoint << (bi::tcp::endpoint)node.endpoint;
		node_l["endpoint"] = ss_endpoint.str();
		nodes_l.push_back(node_l);
	}
	j_response["nodes"] = nodes_l;

	error_code_l = mcp::rpc_nodes_error_code::ok;
	rpc_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
}

void mcp::rpc_handler::witness_list(mcp::json & j_response)
{
	mcp::rpc_witness_list_error_code error_code_l;
	mcp::witness_param const &w_param(mcp::param::curr_witness_param());
	mcp::json witness_list_l = mcp::json::array();
	for (auto i : w_param.witness_list)
	{
		witness_list_l.push_back(i.hexPrefixed());
	}
	j_response["witness_list"] = witness_list_l;

	error_code_l = mcp::rpc_witness_list_error_code::ok;
	rpc_response(response, int(error_code_l), err.msg(error_code_l), j_response);
}

void mcp::rpc_handler::debug_trace_transaction(mcp::json & j_response)
{
	//mcp::rpc_debug_trace_transaction_error_code error_code_l;

	//if (!request.count("hash") || !request["hash"].is_string())
	//{
	//	error_code_l = mcp::rpc_debug_trace_transaction_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//std::string hash_text = request["hash"];
	//mcp::uint256_union hash;
	//auto error(hash.decode_hex(hash_text));

	//if (error)
	//{
	//	error_code_l = mcp::rpc_debug_trace_transaction_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l) + "," + hash_text);
	//	return;
	//}

	//mcp::db::db_transaction transaction(m_store.create_transaction());
	//dev::eth::McInfo mc_info;
	//if (!m_chain->get_mc_info_from_block_hash(transaction, m_cache, hash, mc_info))
	//{
	//	error_code_l = mcp::rpc_debug_trace_transaction_error_code::invalid_mci;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//mcp::json options;
	//options["disable_storage"] = true;
	//options["disable_memory"] = false;
	//options["disable_stack"] = false;
	//options["full_storage"] = false;
	//if (request.count("options"))
	//{
	//	mcp::json options_json = request["options"];
	//	if (options_json.count("disable_storage"))
	//		options["disable_storage"] = options_json["disable_storage"];
	//	if (options_json.count("disable_memory"))
	//		options["disable_memory"] = options_json["disable_memory"];
	//	if (options_json.count("disable_stack"))
	//		options["disable_stack"] = options_json["disable_stack"];
	//	if (options_json.count("full_storage"))
	//		options["full_storage"] = options_json["full_storage"];
	//}

	//try
	//{
	//	mcp::json response_l;
	//	dev::eth::EnvInfo env(transaction, m_store, m_cache, mc_info);
	//	auto block(m_cache->block_get(transaction, hash));
	//	assert_x(block);
	//	chain_state c_state(transaction, 0, m_store, m_chain, m_cache, block);
	//	mcp::ExecutionResult er;
	//	std::list<std::shared_ptr<mcp::trace>> traces;
	//	mcp::Executive e(c_state, env, true, traces);
	//	e.setResultRecipient(er);

	//	mcp::json trace = m_chain->traceTransaction(e, options);
	//	//response_l["gas"] = block->hashables->gas.str();
	//	response_l["return_value"] = er.output.hexPrefixed();
	//	response_l["struct_logs"] = trace;

	//	error_code_l = mcp::rpc_debug_trace_transaction_error_code::ok;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	//}
	//catch (Exception const &_e)
	//{
	//	cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
	//		   << diagnostic_information(_e);
	//	exit(1);
	//}
	//catch (std::exception const &_e)
	//{
	//	std::cerr << _e.what() << std::endl;
	//	throw;
	//}
}

void mcp::rpc_handler::debug_storage_range_at(mcp::json & j_response)
{
	//mcp::rpc_debug_storage_range_at_error_code error_code_l;

	//mcp::block_hash hash;
	//if (!request.count("hash") || !request["hash"].is_string() || hash.decode_hex(request["hash"]))
	//{
	//	error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_hash;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//dev::Address acct(0);
	//if (!request.count("account") || !request["account"].is_string() || acct.decode_account(request["account"]))
	//{
	//	error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_account;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//mcp::uint256_union begin_u;
	//if (!request.count("begin") || !request["begin"].is_string() || begin_u.decode_hex(request["begin"]))
	//{
	//	error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_begin;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}
	//h256 begin = h256(begin_u.ref());

	//uint64_t max_results(0);
	//if (!request.count("max_results") || !try_get_uint64_t_from_json("max_results", max_results))
	//{
	//	error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_max_results;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l));
	//	return;
	//}

	//mcp::json ret = mcp::json::object();
	//ret["storage"] = mcp::json::object();

	//try
	//{
	//	mcp::db::db_transaction transaction(m_store.create_transaction());
	//	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

	//	std::map<h256, std::pair<u256, u256>> const storage(c_state.storage(acct));

	//	// begin is inclusive
	//	auto itBegin = storage.lower_bound(begin);
	//	for (auto it = itBegin; it != storage.end(); ++it)
	//	{
	//		if (ret["storage"].size() == static_cast<unsigned>(max_results))
	//		{
	//			ret["next_key"] = toCompactHexPrefixed(it->first, 32);
	//			break;
	//		}

	//		mcp::json keyValue = mcp::json::object();
	//		std::string hashedKey = toCompactHexPrefixed(it->first, 32);
	//		keyValue["key"] = toCompactHexPrefixed(it->second.first, 32);
	//		keyValue["value"] = toCompactHexPrefixed(it->second.second, 32);

	//		ret["storage"][hashedKey] = keyValue;
	//	}

	//	error_code_l = mcp::rpc_debug_storage_range_at_error_code::ok;
	//	rpc_response(response, (int)error_code_l, err.msg(error_code_l), ret);
	//}
	//catch (Exception const &_e)
	//{
	//	cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
	//		   << diagnostic_information(_e);
	//	exit(1);
	//}
}

mcp::rpc_connection::rpc_connection(mcp::rpc &rpc_a) : rpc(rpc_a),
													   socket(rpc_a.io_service)
{
	responded.clear();
}

void mcp::rpc_connection::parse_connection()
{
	read();
}

void mcp::rpc_connection::write_result(std::string body, unsigned version)
{
	if (!responded.test_and_set())
	{
		res.set("Content-Type", "application/json");
		res.set("Access-Control-Allow-Origin", "*");
		res.set("Access-Control-Allow-Headers", "Accept, Accept-Language, Content-Language, Content-Type");
		res.set("Connection", "close");
		res.result(boost::beast::http::status::ok);
		res.body() = body;
		res.version(version);
		res.prepare_payload();
	}
	else
	{
		assert_x(false && "HTTP RPC already responded and should only respond once");
		// Guards `res' from being clobbered while async_write is being serviced
	}
}

void mcp::rpc_connection::read()
{
	auto this_l(shared_from_this());
	boost::beast::http::async_read(socket, buffer, request, [this_l](boost::system::error_code const &ec, size_t bytes_transferred)
								   {
		if (!ec)
		{
			this_l->rpc.m_background->sync_async([this_l]() {
				auto start(std::chrono::steady_clock::now());
				auto version(this_l->request.version());
                auto response_handler([this_l, version, start](mcp::json const & js)
				{
					try
					{
                        std::string body = js.dump();
						LOG(this_l->m_log.debug) << "RESPONSE:" << body;
						this_l->write_result(body, version);
						boost::beast::http::async_write(this_l->socket, this_l->res, [this_l](boost::system::error_code const & e, size_t size)
						{
						});
					}
					catch (std::exception const & e)
					{
                        LOG(this_l->m_log.error) << "rpc http write error:" << e.what() << "," << boost::stacktrace::stacktrace();
						throw;
					}
				});
				if (this_l->request.method() == boost::beast::http::verb::post)
				{
					auto handler(std::make_shared<mcp::rpc_handler>(this_l->rpc, this_l->request.body(), response_handler,0));
					handler->process_request();
				}
				else
				{
					rpc_response(response_handler, "Can only POST requests");
				}
			});
		}
		else
		{
            LOG(this_l->m_log.error) << "HTTP RPC read error: " << ec.message();
		} });
}

namespace
{
	void reprocess_body(std::string &body, mcp::json &json_a)
	{
		body = json_a.dump();
	}
}

void mcp::rpc_handler::process_request()
{
	if (!rpc.config.enable_control)
	{
		rpc_response(response, "RPC control is disabled");
		return;
	}

	try
	{
		request = mcp::json::parse(body);
		
		LOG(m_log.debug) << "REQUEST:" << request;

		std::string action = request.count("action") > 0 ? request["action"] : request["method"];
		auto pointer = m_rpcMethods.find(action);

		if (pointer != m_rpcMethods.end()) {
			mcp::json j_response;
			(this->*(pointer->second))(j_response);
			rpc_response(response, 0, "OK", j_response);
		}
		/*
		bool handled = false;
		if (action == "account_create")
		{
			account_create();
			request.erase("password");
			reprocess_body(body, request);
			handled = true;
		}
		else if (action == "account_remove")
		{
			account_remove();
			request.erase("password");
			reprocess_body(body, request);
			handled = true;
		}
		else if (action == "account_unlock")
		{
			account_unlock();
			request.erase("password");
			reprocess_body(body, request);
			handled = true;
		}
		else if (action == "account_password_change")
		{
			account_password_change();
			request.erase("old_password");
			request.erase("new_password");
			reprocess_body(body, request);
			handled = true;
		}
		else if (action == "send_block")
		{
			send_block();
			request.erase("password");
			reprocess_body(body, request);
			handled = true;
		}

		if (handled)
			return;

		if (action == "account_list")
		{
			account_list();
		}
		else if (action == "account_validate")
		{
			account_validate();
		}
		else if (action == "account_lock")
		{
			account_lock();
		}
		else if (action == "account_export")
		{
			account_export();
		}

		else if (action == "account_import")
		{
			account_import();
		}
		else if (action == "account_code")
		{
			account_code();
		}
		else if (action == "account_balance")
		{
			account_balance();
		}
		else if (action == "accounts_balances")
		{
			accounts_balances();
		}
		else if (action == "account_block_list")
		{
			account_block_list();
		}
		else if (action == "account_state_list")
		{
			account_state_list();
		}
		else if (action == "block")
		{
			block();
		}
		else if (action == "blocks")
		{
			blocks();
		}
		else if (action == "block_state")
		{
			block_state();
		}
		else if (action == "block_states")
		{
			block_states();
		}
		else if (action == "block_traces")
		{
			block_traces();
		}
		else if (action == "stable_blocks")
		{
			stable_blocks();
		}
		else if (action == "estimate_gas")
		{
			estimate_gas();
		}
		else if (action == "call")
		{
			call();
		}
		else if (action == "logs")
		{
			logs();
		}
		else if (action == "generate_offline_block")
		{
			generate_offline_block();
		}
		else if (action == "send_offline_block")
		{
			send_offline_block();
		}
		else if (action == "block_summary")
		{
			block_summary();
		}
		else if (action == "sign_msg")
		{
			sign_msg();
		}
		else if (action == "version")
		{
			version();
		}
		else if (action == "status")
		{
			status();
		}
		else if (action == "peers")
		{
			peers();
		}
		else if (action == "nodes")
		{
			nodes();
		}
		else if (action == "witness_list")
		{
			witness_list();
		}
		else if (action == "debug_trace_transaction")
		{
			debug_trace_transaction();
		}
		else if (action == "debug_storage_range_at")
		{
			debug_storage_range_at();
		}
		else if (action == "get_sync_status")
		{
			rpc_response(response, mcp::node_sync::get_syncing_status());
		}
		// added by michael
		else if (action == "eth_blockNumber")
		{
			eth_blockNumber();
		}
		else if (action == "eth_getTransactionCount")
		{
			eth_getTransactionCount();
		}
		else if (action == "eth_chainId")
		{
			eth_chainId();
		}
		else if (action == "eth_gasPrice")
		{
			eth_gasPrice();
		}
		else if (action == "eth_estimateGas")
		{
			eth_estimateGas();
		}
		else if (action == "eth_getBlockByNumber")
		{
			eth_getBlockByNumber();
		}
		else if (action == "eth_sendRawTransaction")
		{
			eth_sendRawTransaction();
		}
		else if (action == "eth_sendTransaction")
		{
			eth_sendTransaction();
		}
		else if (action == "eth_call")
		{
			eth_call();
		}
		else if (action == "net_version")
		{
			net_version();
		}
		else if (action == "net_listening")
		{
			net_listening();
		}
		else if (action == "net_peerCount") {
			net_peerCount();
		}
		else if (action == "web3_clientVersion")
		{
			web3_clientVersion();
		}
		else if (action == "web3_sha3") {
			web3_sha3();
		}
		else if (action == "eth_getCode")
		{
			eth_getCode();
		}
		else if (action == "eth_getStorageAt")
		{
			eth_getStorageAt();
		}
		else if (action == "eth_getTransactionByHash")
		{
			eth_getTransactionByHash();
		}
		else if (action == "eth_getTransactionByBlockHashAndIndex")
		{
			eth_getTransactionByBlockHashAndIndex();
		}
		else if (action == "eth_getTransactionByBlockNumberAndIndex")
		{
			eth_getTransactionByBlockNumberAndIndex();
		}
		else if (action == "eth_getTransactionReceipt")
		{
			eth_getTransactionReceipt();
		}
		else if (action == "eth_getBalance")
		{
			eth_getBalance();
		}
		else if (action == "eth_getBlockByHash")
		{
			eth_getBlockByHash();
		}
		else if (action == "eth_getBlockTransactionCountByHash")
		{
			eth_getBlockTransactionCountByHash();
		}
		else if (action == "eth_getBlockTransactionCountByNumber")
		{
			eth_getBlockTransactionCountByNumber();
		}
		else if (action == "eth_accounts")
		{
			eth_accounts();
		}
		else if (action == "eth_sign")
		{
			eth_sign();
		}
		else if (action == "eth_signTransaction")
		{
			eth_signTransaction();
		}
		else if (action == "eth_syncing")
		{
			eth_syncing();
		}
		else if (action == "eth_protocolVersion")
		{
			eth_protocolVersion();
		}
		else if (action == "eth_getLogs")
		{
			eth_getLogs();
		}
		else if (action == "personal_importRawKey")
		{
			personal_importRawKey();
		}
		else if (action == "personal_listAccounts")
		{
			personal_listAccounts();
		}
		else if (action == "personal_lockAccount")
		{
			personal_lockAccount();
		}
		else if (action == "personal_newAccount")
		{
			personal_newAccount();
		}
		else if (action == "personal_unlockAccount")
		{
			personal_unlockAccount();
		}
		else if (action == "personal_sendTransaction")
		{
			personal_sendTransaction();
		}
		else if (action == "personal_sign")
		{
			personal_sign();
		}
		else if (action == "personal_ecRecover")
		{
			personal_ecRecover();
		}
		*/
		//
		else
		{
			rpc_response(response, "Unknown command");
		}
	}
	catch (mcp::RpcException const &err)
	{
		rpc_response(response, err.code(), err.what());
	}
	catch (std::exception const &err)
	{
		LOG(m_log.error) << "rpc runtime_error : error_response = Unable to parse JSON or " << err.what();
		rpc_response(response, "Unable to parse JSON");
	}
	catch (...)
	{
		LOG(m_log.error) << "Internal server error in HTTP RPC ";
		rpc_response(response, "Internal server error in HTTP RPC");
	}
}

std::shared_ptr<mcp::rpc> mcp::get_rpc(mcp::block_store &store_a, std::shared_ptr<mcp::chain> chain_a,
									   std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
									   std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
									   std::shared_ptr<mcp::async_task> background_a, std::shared_ptr<mcp::composer> composer_a,
									   boost::asio::io_service &service_a, mcp::rpc_config const &config_a)
{
	std::shared_ptr<rpc> impl(new rpc(store_a, chain_a, cache_a, key_manager_a, wallet_a, host_a, background_a, composer_a, service_a, config_a));
	return impl;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_create_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_create_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_create_error_code::empty_password:
		error_msg = "Password can not be empty";
		break;
	case mcp::rpc_account_create_error_code::invalid_length_password:
		error_msg = "Invalid password! A valid password length must between 8 and 100";
		break;
	case mcp::rpc_account_create_error_code::invalid_characters_password:
		error_msg = "Invalid password! A valid password must contain characters from letters (a-Z, A-Z), digits (0-9) and special characters (!@#$%^&*)";
		break;
	case mcp::rpc_account_create_error_code::invalid_gen_next_work_value:
		error_msg = "Invalid gen_next_work format";
		break;
	case mcp::rpc_account_create_error_code::invalid_password:
		error_msg = "Invalid password";
		break;
	case mcp::rpc_account_create_error_code::invalid_backup:
		error_msg = "Invalid backup";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_remove_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_remove_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_remove_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_account_remove_error_code::account_not_exisit:
		error_msg = "Account not found";
		break;
	case mcp::rpc_account_remove_error_code::wrong_password:
		error_msg = "Wrong password";
		break;
	case mcp::rpc_account_remove_error_code::invalid_password:
		error_msg = "Invalid password";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_unlock_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_unlock_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_unlock_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_account_unlock_error_code::account_not_exisit:
		error_msg = "Account not found";
		break;
	case mcp::rpc_account_unlock_error_code::wrong_password:
		error_msg = "Wrong password";
		break;
	case mcp::rpc_account_unlock_error_code::invalid_password:
		error_msg = "Invalid password";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_lock_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_lock_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_lock_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_account_lock_error_code::account_not_exisit:
		error_msg = "Account not found";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_import_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_import_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_import_error_code::invalid_gen_next_work_value:
		error_msg = "Invalid gen_next_work format";
		break;
	case mcp::rpc_account_import_error_code::invalid_json:
		error_msg = "Invalid json";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_export_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_export_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_export_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_account_export_error_code::account_not_exisit:
		error_msg = "Account not found";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_validate_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_validate_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_validate_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_password_change_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_password_change_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_password_change_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_account_password_change_error_code::account_not_exisit:
		error_msg = "Account not found";
		break;
	case mcp::rpc_account_password_change_error_code::invalid_length_password:
		error_msg = "Invalid new password! A valid password length must between 8 and 100";
		break;
	case mcp::rpc_account_password_change_error_code::invalid_characters_password:
		error_msg = "Invalid new password! A valid password must contain characters from letters (a-Z, A-Z), digits (0-9) and special characters (!@#$%^&*)";
		break;
	case mcp::rpc_account_password_change_error_code::wrong_password:
		error_msg = "Wrong old password";
		break;
	case mcp::rpc_account_password_change_error_code::invalid_old_password:
		error_msg = "Invalid old password";
		break;
	case mcp::rpc_account_password_change_error_code::invalid_new_password:
		error_msg = "Invalid new password";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_list_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_list_error_code::ok:
		error_msg = "OK";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_block_list_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_block_list_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_block_list_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_account_block_list_error_code::invalid_limit:
		error_msg = "Invalid limit";
		break;
	case mcp::rpc_account_block_list_error_code::limit_too_large:
		error_msg = "Limit too large, it can not be large than " + std::to_string(mcp::rpc_handler::list_max_limit);
		break;
	case mcp::rpc_account_block_list_error_code::invalid_index:
		error_msg = "Invalid index";
		break;
	case mcp::rpc_account_block_list_error_code::index_not_exsist:
		error_msg = "Index not found";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_balance_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_balance_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_balance_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_accounts_balances_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_accounts_balances_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_accounts_balances_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_code_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_account_code_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_account_code_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_generate_offline_block_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_generate_offline_block_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_generate_offline_block_error_code::invalid_account_from:
		error_msg = "Invalid from account";
		break;
	case mcp::rpc_generate_offline_block_error_code::invalid_account_to:
		error_msg = "Invalid to account";
		break;
	case mcp::rpc_generate_offline_block_error_code::invalid_amount:
		error_msg = "Invalid amount format";
		break;
	case mcp::rpc_generate_offline_block_error_code::invalid_gas:
		error_msg = "Invalid gas format";
		break;
	case mcp::rpc_generate_offline_block_error_code::invalid_data:
		error_msg = "Invalid data format";
		break;
	case mcp::rpc_generate_offline_block_error_code::data_size_too_large:
		error_msg = "Data size too large";
		break;
	case mcp::rpc_generate_offline_block_error_code::insufficient_balance:
		error_msg = "Insufficient balance";
		break;
	case mcp::rpc_generate_offline_block_error_code::validate_error:
		error_msg = "Validate error";
		break;
	case mcp::rpc_generate_offline_block_error_code::compose_error:
		error_msg = "Compose error";
		break;
	case mcp::rpc_generate_offline_block_error_code::compose_unknown_error:
		error_msg = "Compose unknown error";
		break;
	case mcp::rpc_generate_offline_block_error_code::invalid_gas_price:
		error_msg = "Invalid gas price format";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_send_offline_block_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_send_offline_block_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_account_from:
		error_msg = "Invalid from account";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_account_to:
		error_msg = "Invalid to account";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_amount:
		error_msg = "Invalid amount format";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_gas:
		error_msg = "Invalid gas format";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_data:
		error_msg = "Invalid data format";
		break;
	case mcp::rpc_send_offline_block_error_code::data_size_too_large:
		error_msg = "Data size too large";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_gen_next_work_value:
		error_msg = "Invalid gen_next_work format";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_previous:
		error_msg = "Invalid previous format";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_exec_timestamp:
		error_msg = "Invalid exec timestamp format";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_work:
		error_msg = "Invalid work format";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_signature:
		error_msg = "Invalid signature format";
		break;
	case mcp::rpc_send_offline_block_error_code::insufficient_balance:
		error_msg = "Insufficient balance";
		break;
	case mcp::rpc_send_offline_block_error_code::validate_error:
		error_msg = "Generate block fail, please retry later";
		break;
	case mcp::rpc_send_offline_block_error_code::send_block_error:
		error_msg = "Send block error";
		break;
	case mcp::rpc_send_offline_block_error_code::send_unknown_error:
		error_msg = "Send block unkonw error";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_password:
		error_msg = "Invalid password";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_id:
		error_msg = "Invalid id";
		break;
	case mcp::rpc_send_offline_block_error_code::invalid_gas_price:
		error_msg = "Invalid gas price format";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_sign_msg_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_sign_msg_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_sign_msg_error_code::invalid_public_key:
		error_msg = "Invalid public key format";
		break;
	case mcp::rpc_sign_msg_error_code::invalid_msg:
		error_msg = "Invalid msg format";
		break;
	case mcp::rpc_sign_msg_error_code::wrong_password:
		error_msg = "Wrong password";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_block_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_block_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_block_error_code::invalid_hash:
		error_msg = "Invalid hash format";
		break;
	case mcp::rpc_block_error_code::block_not_exsist:
		error_msg = "Hash not found";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_blocks_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_blocks_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_blocks_error_code::invalid_hash:
		error_msg = "Invalid hash format";
		break;
	case mcp::rpc_blocks_error_code::block_not_exsist:
		error_msg = "Hash not found";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_stable_blocks_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_stable_blocks_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_stable_blocks_error_code::invalid_index:
		error_msg = "Invalid index format";
		break;
	case mcp::rpc_stable_blocks_error_code::invalid_limit:
		error_msg = "Invalid limit format";
		break;
	case mcp::rpc_stable_blocks_error_code::limit_too_large:
		error_msg = "Limit too large, it can not be large than " + std::to_string(mcp::rpc_handler::list_max_limit);
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_status_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_status_error_code::ok:
		error_msg = "OK";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_witness_list_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_witness_list_error_code::ok:
		error_msg = "OK";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_work_get_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_work_get_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_work_get_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_work_get_error_code::account_not_exisit:
		error_msg = "Account not found";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_version_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_version_error_code::ok:
		error_msg = "OK";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_peers_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_peers_error_code::ok:
		error_msg = "OK";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_nodes_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_nodes_error_code::ok:
		error_msg = "OK";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_stop_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_stop_error_code::ok:
		error_msg = "OK";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_send_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_send_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_send_error_code::invalid_account_from:
		error_msg = "Invalid from account";
		break;
	case mcp::rpc_send_error_code::account_not_exisit:
		error_msg = "From account not found";
		break;
	case mcp::rpc_send_error_code::invalid_account_to:
		error_msg = "Invalid to account";
		break;
	case mcp::rpc_send_error_code::invalid_amount:
		error_msg = "Invalid amount format";
		break;
	case mcp::rpc_send_error_code::invalid_gas:
		error_msg = "Invalid gas format";
		break;
	case mcp::rpc_send_error_code::invalid_data:
		error_msg = "Invalid data format";
		break;
	case mcp::rpc_send_error_code::data_size_too_large:
		error_msg = "Data size too large";
		break;
	case mcp::rpc_send_error_code::invalid_gen_next_work_value:
		error_msg = "Invalid gen_next_work format";
		break;
	case mcp::rpc_send_error_code::account_locked:
		error_msg = "Account locked";
		break;
	case mcp::rpc_send_error_code::wrong_password:
		error_msg = "Wrong password";
		break;
	case mcp::rpc_send_error_code::insufficient_balance:
		error_msg = "Insufficient balance";
		break;
	case mcp::rpc_send_error_code::validate_error:
		error_msg = "Generate block fail, please retry later";
		break;
	case mcp::rpc_send_error_code::send_block_error:
		error_msg = "Send block error";
		break;
	case mcp::rpc_send_error_code::send_unknown_error:
		error_msg = "Send block unkonw error";
		break;
	case mcp::rpc_send_error_code::invalid_password:
		error_msg = "Invalid password";
		break;
	case mcp::rpc_send_error_code::invalid_id:
		error_msg = "Invalid id";
		break;
	case mcp::rpc_send_error_code::invalid_gas_price:
		error_msg = "Invalid gas price format";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_estimate_gas_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_estimate_gas_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_estimate_gas_error_code::invalid_account_from:
		error_msg = "Invalid from account";
		break;
	case mcp::rpc_estimate_gas_error_code::invalid_account_to:
		error_msg = "Invalid to account";
		break;
	case mcp::rpc_estimate_gas_error_code::invalid_amount:
		error_msg = "Invalid amount format";
		break;
	case mcp::rpc_estimate_gas_error_code::invalid_gas:
		error_msg = "Invalid gas format";
		break;
	case mcp::rpc_estimate_gas_error_code::invalid_data:
		error_msg = "Invalid data format";
		break;
	case mcp::rpc_estimate_gas_error_code::data_size_too_large:
		error_msg = "Data size too large";
		break;
	case mcp::rpc_estimate_gas_error_code::invalid_gas_price:
		error_msg = "Invalid gas price format";
		break;
	case mcp::rpc_estimate_gas_error_code::invalid_mci:
		error_msg = "Invalid mci";
		break;
	case mcp::rpc_estimate_gas_error_code::gas_not_enough_or_fail:
		error_msg = "Gas not enough or execute fail";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_call_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_call_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_call_error_code::invalid_account_from:
		error_msg = "Invalid from account";
		break;
	case mcp::rpc_call_error_code::invalid_account_to:
		error_msg = "Invalid to account";
		break;
	case mcp::rpc_call_error_code::invalid_amount:
		error_msg = "Invalid amount format";
		break;
	case mcp::rpc_call_error_code::invalid_data:
		error_msg = "Invalid data format";
		break;
	case mcp::rpc_call_error_code::data_size_too_large:
		error_msg = "Data size too large";
		break;
	case mcp::rpc_call_error_code::invalid_mci:
		error_msg = "Invalid mci";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_logs_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_logs_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_logs_error_code::invalid_from_stable_block_index:
		error_msg = "Invalid from stable block index";
		break;
	case mcp::rpc_logs_error_code::invalid_to_stable_block_index:
		error_msg = "Invalid to stable block index";
		break;
	case mcp::rpc_logs_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_logs_error_code::invalid_topics:
		error_msg = "Invalid topics";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_debug_trace_transaction_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_debug_trace_transaction_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_debug_trace_transaction_error_code::invalid_hash:
		error_msg = "Invalid hash format";
		break;
	case mcp::rpc_debug_trace_transaction_error_code::invalid_mci:
		error_msg = "Invalid mci info from the block";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_debug_storage_range_at_error_code const &err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_debug_storage_range_at_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_debug_storage_range_at_error_code::invalid_hash:
		error_msg = "Invalid hash format";
		break;
	case mcp::rpc_debug_storage_range_at_error_code::invalid_account:
		error_msg = "Invalid account";
		break;
	case mcp::rpc_debug_storage_range_at_error_code::invalid_begin:
		error_msg = "Invalid begin address";
		break;
	case mcp::rpc_debug_storage_range_at_error_code::invalid_max_results:
		error_msg = "Invalid value of max results ";
		break;
	}
	return error_msg;
}

// added by michael
void mcp::error_eth_response(std::function<void(mcp::json const &)> response_a, mcp::rpc_eth_error_code error_code, mcp::json& json_a)
{
	std::string sub_error_msg;
	switch (error_code)
	{
	case mcp::rpc_eth_error_code::ok:
		sub_error_msg = "";
		break;
	case mcp::rpc_eth_error_code::invalid_params:
		sub_error_msg = "";
		break;
	case mcp::rpc_eth_error_code::invalid_account:
		sub_error_msg = "Invalid account";
		break;
	case mcp::rpc_eth_error_code::invalid_password:
		sub_error_msg = "Invalid password";
		break;
	case mcp::rpc_eth_error_code::locked_account:
		sub_error_msg = "Locked account";
		break;
	case mcp::rpc_eth_error_code::invalid_signature:
		sub_error_msg = "Invalid signature";
		break;
	case mcp::rpc_eth_error_code::invalid_value:
		sub_error_msg = "Invalid value";
		break;
	case mcp::rpc_eth_error_code::invalid_gas:
		sub_error_msg = "Invalid gas amount";
		break;
	case mcp::rpc_eth_error_code::invalid_gas_price:
		sub_error_msg = "Invalid gas price";
		break;
	case mcp::rpc_eth_error_code::invalid_data:
		sub_error_msg = "Invalid data";
		break;
	case mcp::rpc_eth_error_code::invalid_block_number:
		sub_error_msg = "Invalid block number";
		break;
	case mcp::rpc_eth_error_code::invalid_from_account:
		sub_error_msg = "Invalid sender account";
		break;
	case mcp::rpc_eth_error_code::invalid_to_account:
		sub_error_msg = "Invalid receiver account";
		break;
	case mcp::rpc_eth_error_code::invalid_hash:
		sub_error_msg = "Invalid hash";
		break;
	case mcp::rpc_eth_error_code::insufficient_balance:
		sub_error_msg = "Insufficient balance";
		break;
	case mcp::rpc_eth_error_code::data_size_too_large:
		sub_error_msg = "Data size is too large";
		break;
	case mcp::rpc_eth_error_code::validate_error:
		sub_error_msg = "Validation error";
		break;
	case mcp::rpc_eth_error_code::block_error:
		sub_error_msg = "Block error";
		break;
	default:
		sub_error_msg = "Unknown error";
		break;
	}

	mcp::json error;
	if (error_code >= mcp::rpc_eth_error_code::invalid_params && error_code <= mcp::rpc_eth_error_code::data_size_too_large) {
		error_code = mcp::rpc_eth_error_code::INVALID_PARAMS;
	}
	else if (error_code >= mcp::rpc_eth_error_code::validate_error && error_code <= mcp::rpc_eth_error_code::unknown_error) {
		error_code = mcp::rpc_eth_error_code::INTERNAL_ERROR;
	}

	std::string error_msg;
	// https://github.com/ethereum/EIPs/blob/master/EIPS/eip-1474.md#error-codes
	switch (error_code)
	{
	case mcp::rpc_eth_error_code::PARSE_ERROR:
		error_msg = "Parse error";
		break;
	case mcp::rpc_eth_error_code::INVALID_REQUEST:
		error_msg = "Invalid request";
		break;
	case mcp::rpc_eth_error_code::METHOD_NOT_FOUND:
		error_msg = "Method not found";
		break;
	case mcp::rpc_eth_error_code::INVALID_PARAMS:
		error_msg = "Invalid params";
		break;
	case mcp::rpc_eth_error_code::INTERNAL_ERROR:
		error_msg = "Internal error";
		break;
	case mcp::rpc_eth_error_code::METHOD_NOT_SUPPORTED:
		error_msg = "Method not supported.";
		break;
	case mcp::rpc_eth_error_code::INVALID_INPUT:
		error_msg = "Invalid input";
		break;
	case mcp::rpc_eth_error_code::TRANSACTION_REJECTED:
		error_msg = "Transaction rejected";
		break;
	default:
		error_msg = "Unknown error";
		break;
	}

	error["code"] = error_code;
	error["message"] = error_msg + (sub_error_msg.empty() ? "" : (" (" + sub_error_msg + ")"));
	json_a["error"] = error;
	response_a(json_a);
}

bool mcp::rpc_handler::is_eth_rpc(mcp::json &response_l)
{
	if (!request.count("id") ||
		!request.count("jsonrpc") ||
		!request.count("params") ||
		!request["params"].is_array())
	{
		error_eth_response(response, rpc_eth_error_code::INVALID_REQUEST, response_l);
		return false;
	}

	response_l["id"] = request["id"];
	response_l["jsonrpc"] = request["jsonrpc"];
	response_l["result"] = nullptr;

	return true;
}

void mcp::rpc_handler::eth_blockNumber(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	j_response["result"] = toJS(m_chain->last_stable_index());
	response(j_response);
}

void mcp::rpc_handler::eth_getTransactionCount(mcp::json & j_response)
{
	try
	{
		std::string str = request["params"][0];
		j_response["result"] = toJS(m_wallet->getTransactionCount(jsToAddress(str)));
		response(j_response);
	}
	catch (...)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		//BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

void mcp::rpc_handler::eth_chainId(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	j_response["result"] = toJS(mcp::chain_id);
	response(j_response);
}

void mcp::rpc_handler::eth_gasPrice(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}
	j_response["result"] = toJS(mcp::gas_price);
	response(j_response);
}

void mcp::rpc_handler::eth_estimateGas(mcp::json & j_response)
{
	try
	{
		mcp::json params = request["params"][0];
		TransactionSkeleton t = mcp::toTransactionSkeleton(params);
		int64_t gas = static_cast<int64_t>(t.gas);

		dev::eth::McInfo mc_info;
		uint64_t block_number = m_chain->last_stable_mci();
		if (!try_get_mc_info(mc_info, block_number))
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}

		mcp::db::db_transaction transaction(m_store.create_transaction());
		std::pair<u256, bool> result = m_chain->estimate_gas(transaction, m_cache, t.from, t.value, t.to, t.data, gas, t.gasPrice, mc_info);

		/// this error is reported if the gas less than 21000, the logic has not been confirmed, response other code ?
		if (!result.second)
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}

		j_response["result"] = toJS(result.first);
		response(j_response);
	}
	catch (Exception const&)
	{
		//throw JsonRpcException(exceptionToErrorMessage());
	}
}

void mcp::rpc_handler::eth_getBlockByNumber(mcp::json & j_response)
{
	try
	{
		uint64_t block_number = 0;
		std::string str = request["params"][0];
		if (str == "latest")
		{
			block_number = m_chain->last_stable_index();
		}
		else if (str == "earliest")
		{///used default zero
		}
		else
		{
			block_number = jsToInt(str);
		}

		mcp::db::db_transaction transaction(m_store.create_transaction());
		mcp::block_hash block_hash;
		if (m_store.stable_block_get(transaction, block_number, block_hash))
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}

		auto block(m_cache->block_get(transaction, block_hash));
		mcp::json block_l;
		if (block != nullptr)
		{
			block->serialize_json_eth(block_l);
			block_l["number"] = uint64_to_hex_nofill(block_number);
		}
		j_response["result"] = block_l;

		response(j_response);
	}
	catch (...)
	{
		mcp::json j_response;
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		//BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

void mcp::rpc_handler::eth_sendRawTransaction(mcp::json & j_response)
{
	try
	{
		std::string _rlp = request["params"][0];
		Transaction t(jsToBytes(_rlp, OnFailed::Throw), CheckTransaction::None);

		mcp::json response_l;
		j_response["result"] = toJS(m_wallet->importTransaction(t));
		response(j_response);
	}
	catch (Exception const& e)
	{
		LOG(m_log.debug) << "eth_sendRawTransaction error:" << e.what();//delete if response
		//throw JsonRpcException(exceptionToErrorMessage());
	}
}

void mcp::rpc_handler::eth_sendTransaction(mcp::json & j_response)
{
	try
	{
		mcp::json params = request["params"][0];
		auto rpc_l(shared_from_this());
		auto fun = [rpc_l, &j_response, this](h256 h)
		{
			j_response["result"] = toJS(h);
			response(j_response);
		};

		TransactionSkeleton t = mcp::toTransactionSkeleton(params);
		m_wallet->send_async(t, fun);
	}
	catch (Exception const& e)
	{
		LOG(m_log.debug) << "eth_sendRawTransaction error:" << e.what();//delete if response
		//throw JsonRpcException(exceptionToErrorMessage());
	}
}

void mcp::rpc_handler::eth_call(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() == 0)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	dev::Address from;

	if (params[0].count("from"))
	{
		if (!params[0]["from"].is_string())
		{
			error_eth_response(response, rpc_eth_error_code::invalid_from_account, j_response);
			return;
		}
		std::string from_text = params[0]["from"];
		if (!mcp::isAddress(from_text))
		{
			error_eth_response(response, rpc_eth_error_code::invalid_from_account, j_response);
			return;
		}
		from = dev::Address(from_text);
	}
	
	if (!params[0].count("to") || (!params[0]["to"].is_string()))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_to_account, j_response);
		return;
	}

	std::string to_text = params[0]["to"];
	if (!mcp::isAddress(to_text))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_to_account, j_response);
		return;
	}

	dev::Address to(to_text);

	dev::bytes data;
	if (params[0].count("data"))
	{
		std::string data_text = params[0]["data"];
		if (mcp::hex_to_bytes(data_text, data))
		{
			error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
			return;
		}
		if (data.size() > mcp::max_data_size)
		{
			error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
			return;
		}
	}

	uint64_t block_number;
	if (params.size() == 2 && params[1].is_string())
	{
		std::string block_numberText = params[1];
		if (block_numberText == "latest")
		{
			block_number = m_chain->last_stable_mci();
		}
		else if (block_numberText == "earliest")
		{
			block_number = 0;
		}
		else if (block_numberText.find("0x") == 0)
		{
			if (hex_to_uint64(block_numberText, block_number, true))
			{
				error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
				return;
			}
		}
		else
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}
	}
	else
	{
		block_number = m_chain->last_stable_mci();
	}

	dev::eth::McInfo mc_info;
	if (!try_get_mc_info(mc_info, block_number))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
		return;
	}

	//std::shared_ptr<mcp::block> block = std::make_shared<mcp::block>(
	//	mcp::block_type::light,														// mcp::block_type type_a,
	//	from,																		// dev::Address const & from_a,
	//	to,																			// dev::Address const & to_a,
	//	0,																			// mcp::amount const & amount_a,
	//	0,																			// mcp::block_hash const & previous_a,
	//	std::vector<mcp::block_hash>{},												// std::vector<mcp::block_hash> const & parents_a,
	//	std::make_shared<std::list<mcp::block_hash>>(std::list<mcp::block_hash>{}), // std::shared_ptr<std::list<mcp::block_hash>> links_a,
	//	0,																			// mcp::summary_hash const & last_summary_a,
	//	0,																			// mcp::block_hash const & last_summary_block_a,
	//	0,																			// mcp::block_hash const & last_stable_block_a,
	//	mcp::uint256_t(mcp::block_max_gas),											// uint256_t gas_a,
	//	0,																			// uint256_t gas_price_a,
	//	data,																		// std::vector<uint8_t> const & data_a,
	//	0																			// uint64_t const & exec_timestamp_a,
	//);

	//mcp::db::db_transaction transaction(m_store.create_transaction());
	//std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = m_chain->execute(transaction, m_cache, block, mc_info, Permanence::Reverted, dev::eth::OnOpFunc());

	//response_l["result"] = "0x" + bytes_to_hex(result.first.output);

	response(j_response);
}

void mcp::rpc_handler::net_version(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	j_response["result"] = (int)mcp::mcp_network + 800;
	response(j_response);
}

void mcp::rpc_handler::net_listening(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}
	j_response["result"] = m_host->is_started();
	response(j_response);
}

void mcp::rpc_handler::net_peerCount(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}
	j_response["result"] = m_host->get_peers_count();
	response(j_response);
}

void mcp::rpc_handler::web3_clientVersion(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}
	j_response["result"] = STR(MCP_VERSION);
	response(j_response);
}

void mcp::rpc_handler::web3_sha3(mcp::json & j_response) {
	if (!is_eth_rpc(j_response))
	{
		return;
	}
	mcp::json params = request["params"];
	if (params.size() != 1)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}
	if (!params[0].is_string()) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	dev::bytes digest;
	CryptoPP::Keccak_256 hash;
	dev::bytes msg = jsToBytes(params[0]);
	hash.Update((const byte*)msg.data(), msg.size());
	digest.resize(hash.DigestSize());
	hash.Final(digest.data());

	j_response["result"] = "0x" + mcp::bytes_to_hex(digest);
	response(j_response);
}

void mcp::rpc_handler::eth_getCode(mcp::json & j_response)
{
	try
	{
		std::string str = request["params"][0];
		mcp::db::db_transaction transaction(m_store.create_transaction());
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

		j_response["result"] = toJS(c_state.code(jsToAddress(str)));
		response(j_response);
	}
	catch (...)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		//BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

void mcp::rpc_handler::eth_getStorageAt(mcp::json & j_response)
{
	/*mcp::json response_l;
	if (!is_eth_rpc(response_l))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() < 2 ||
		!params[0].is_string() ||
		!params[1].is_string()) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, response_l);
		return;
	}

	if (!mcp::isAddress(params[0])) {
		error_eth_response(response, rpc_eth_error_code::invalid_account, response_l);
		return;
	}

	dev::Address account(params[0]);

	uint256_t position;
	if (hex_to_uint256(params[1], position, true)) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, response_l);
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
	response_l["result"] = uint256_to_hex_nofill(c_state.storage(account, position));

	response(response_l);*/
}

void mcp::rpc_handler::eth_getTransactionByHash(mcp::json & j_response)
{
	try
	{
		//todo localisedTransaction, have block hash and block number. but how to associate blocks before witness
		std::string str = request["params"][0];
		h256 h = jsToFixed<32>(str);

		//todo used cache
		auto transaction = m_store.create_transaction();
		auto t = m_store.transaction_get(transaction, h);
		if (t == nullptr)//todo throw and catch
		{
			error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
			return;
		}

		j_response["result"] = toJson(*t);
		response(j_response);
	}
	catch (Exception const&)
	{
		//throw JsonRpcException(exceptionToErrorMessage());
	}
}

void mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 2) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	if (params[1] != "0x0") {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::block_hash block_hash;
	if (block_hash.decode_hex(params[0], true)) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, block_hash));
	if (block_hash != mcp::genesis::block_hash && state != nullptr)
	{
		auto block(m_cache->block_get(transaction, block_hash));
		if (block != nullptr) {
			if (state->is_stable)
			{
				/*if (state->status == mcp::block_status::ok) {
					mcp::json json_receipt;
					json_receipt["hash"] = block_hash.to_string(true);
					json_receipt["nonce"] = 0;
					if (state->receipt != boost::none) {
						std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, state->receipt->from_state));
						assert_x(acc_state);
						json_receipt["nonce"] = uint256_to_hex_nofill(acc_state->nonce() - 1);
					}
					json_receipt["blockHash"] = block_hash.to_string(true);
					json_receipt["blockNumber"] = uint64_to_hex_nofill(state->main_chain_index.get());
					json_receipt["transactionIndex"] = "0x0";
					json_receipt["from"] = block->hashables->from.to_account();
					if (block->isCreation()) {
						json_receipt["to"] = nullptr;
					}
					else {
						json_receipt["to"] = block->hashables->to.to_account();
					}
					json_receipt["value"] = uint256_to_hex_nofill(block->hashables->amount);
					json_receipt["gas"] = uint256_to_hex_nofill(block->hashables->gas);
					json_receipt["input"] = "0x" + bytes_to_hex(block->data);
					json_receipt["gasPrice"] = uint64_to_hex_nofill(1000000000);
					json_receipt["v"] = block->signature.v;
					json_receipt["r"] = block->signature.r.to_string_no_fill(true);
					json_receipt["s"] = block->signature.s.to_string_no_fill(true);
					j_response["result"] = json_receipt;
				}
				else if (state->status == mcp::block_status::fail) {
					error_eth_response(response, rpc_eth_error_code::TRANSACTION_REJECTED, j_response);
					return;
				}*/
			}
		}
	}

	response(j_response);
}

void mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 2)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	if (params[1] != "0x0") {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	uint64_t block_number;
	if (params[0].is_string())
	{
		std::string block_numberText = params[0];
		if (block_numberText == "latest")
		{
			block_number = m_chain->last_stable_mci();
		}
		else if (block_numberText == "earliest")
		{
			block_number = 0;
		}
		else if (block_numberText.find("0x") == 0)
		{
			if (hex_to_uint64(block_numberText, block_number, true))
			{
				error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
				return;
			}
		}
		else
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}
	}
	else
	{
		error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash block_hash;
	if (m_store.main_chain_get(transaction, block_number, block_hash))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
		return;
	}

	std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, block_hash));

	if (block_hash != mcp::genesis::block_hash && state != nullptr)
	{
		auto block(m_cache->block_get(transaction, block_hash));
		if (block != nullptr) {
			if (state->is_stable)
			{
				/*if (state->status == mcp::block_status::ok) {
					mcp::json json_receipt;
					json_receipt["hash"] = block_hash.to_string(true);
					json_receipt["nonce"] = 0;
					if (state->receipt != boost::none) {
						std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, state->receipt->from_state));
						assert_x(acc_state);
						json_receipt["nonce"] = uint256_to_hex_nofill(acc_state->nonce() - 1);
					}
					json_receipt["blockHash"] = block_hash.to_string(true);
					json_receipt["blockNumber"] = uint64_to_hex_nofill(state->main_chain_index.get());
					json_receipt["transactionIndex"] = "0x0";
					json_receipt["from"] = block->hashables->from.to_account();
					if (block->isCreation()) {
						json_receipt["to"] = nullptr;
					}
					else {
						json_receipt["to"] = block->hashables->to.to_account();
					}
					json_receipt["value"] = uint256_to_hex_nofill(block->hashables->amount);
					json_receipt["gas"] = uint256_to_hex_nofill(block->hashables->gas);
					json_receipt["input"] = "0x" + bytes_to_hex(block->data);
					json_receipt["gasPrice"] = uint64_to_hex_nofill(1000000000);
					json_receipt["v"] = block->signature.v;
					json_receipt["r"] = block->signature.r.to_string_no_fill(true);
					json_receipt["s"] = block->signature.s.to_string_no_fill(true);
					j_response["result"] = json_receipt;
				}
				else if (state->status == mcp::block_status::fail) {
					error_eth_response(response, rpc_eth_error_code::TRANSACTION_REJECTED, j_response);
					return;
				}*/
			}
		}
	}

	response(j_response);
}

void mcp::rpc_handler::eth_getBalance(mcp::json & j_response) {
	try
	{
		std::string str = request["params"][0];
		mcp::db::db_transaction transaction(m_store.create_transaction());
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

		j_response["result"] = toJS(c_state.balance(jsToAddress(str)));
		response(j_response);
	}
	catch (...)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		//BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INVALID_PARAMS));
	}
}

void mcp::rpc_handler::eth_getTransactionReceipt(mcp::json & j_response)
{
	try
	{
		//todo localisedTransaction, have block hash and block number. but how to associate blocks before witness
		std::string str = request["params"][0];
		h256 h = jsToFixed<32>(str);

		//todo used cache
		auto transaction = m_store.create_transaction();
		auto t = m_store.transaction_get(transaction, h);
		auto tr = m_store.transaction_receipt_get(transaction, h);
		if (nullptr == t || nullptr == tr)
			return response("{}");

		auto lt = dev::eth::LocalisedTransactionReceipt(
			*tr,
			t->sha3(),//transaction hash
			t->from(),
			t->to(),
			toAddress(t->from(), t->nonce()));

		j_response["result"] = toJson(lt);
		response(j_response);
	}
	catch (Exception const&)
	{
		//throw JsonRpcException(exceptionToErrorMessage());
	}
}

void mcp::rpc_handler::eth_getBlockByHash(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 2)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::block_hash block_hash;
	if (block_hash.decode_hex(params[0], true)) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, block_hash));
	if (state != nullptr)
	{
		auto block(m_cache->block_get(transaction, block_hash));
		mcp::json block_l;
		if (block != nullptr)
		{
			bool is_full = params[1];
			block->serialize_json_eth(block_l);
			block_l["number"] = uint64_to_hex_nofill(state->main_chain_index.get());
		}
		j_response["result"] = block_l;
	}

	response(j_response);
}

void mcp::rpc_handler::eth_getBlockTransactionCountByHash(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 1)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::block_hash block_hash;
	if (block_hash.decode_hex(params[0], true)) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, block_hash));
	if (state != nullptr)
	{
		auto block(m_cache->block_get(transaction, block_hash));
		mcp::json block_l;
		j_response["result"] = 0;
		if (block != nullptr)
		{
			if (!block->hash().is_zero()) {
				j_response["result"] = 1;
			}
		}
	}

	response(j_response);
}

void mcp::rpc_handler::eth_getBlockTransactionCountByNumber(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 1)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	uint64_t block_number;
	if (params[0].is_string())
	{
		std::string block_numberText = params[0];
		if (block_numberText == "latest")
		{
			block_number = m_chain->last_stable_mci();
		}
		else if (block_numberText == "earliest")
		{
			block_number = 0;
		}
		else if (block_numberText.find("0x") == 0)
		{
			if (hex_to_uint64(block_numberText, block_number, true))
			{
				error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
				return;
			}
		}
		else
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}
	}
	else
	{
		error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::block_hash block_hash;
	if (m_store.main_chain_get(transaction, block_number, block_hash))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
		return;
	}

	std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, block_hash));
	if (state != nullptr)
	{
		auto block(m_cache->block_get(transaction, block_hash));
		if (block != nullptr)
		{
			j_response["result"] = 0;
			if (block != nullptr)
			{
				if (!block->hash().is_zero()) {
					j_response["result"] = 1;
				}
			}
		}
	}

	response(j_response);
}

void mcp::rpc_handler::eth_accounts(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json j_accounts = mcp::json::array();
	std::list<dev::Address> account_list(m_key_manager->list());
	for (auto account : account_list)
	{
		j_accounts.push_back(account.hexPrefixed());
	}
	j_response["result"] = j_accounts;

	response(j_response);
}

void mcp::rpc_handler::eth_sign(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 2) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	std::string account_text = params[0];
	if (!mcp::isAddress(account_text))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_account, j_response);
		return;
	}
	
	dev::Address account(account_text);
	if (!m_key_manager->exists(account))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_account, j_response);
		return;
	}

	dev::bytes data;
	std::string data_text = params[1];
	if (mcp::hex_to_bytes(data_text, data))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
		return;
	}
	if (data.size() > mcp::max_data_size)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
		return;
	}

	mcp::Secret prv;
	if (!m_key_manager->find_unlocked_prv(account, prv)) {
		error_eth_response(response, rpc_eth_error_code::locked_account, j_response);
		return;
	}

	dev::h256 hash;
	get_eth_signed_msg(data, hash);
	
	dev::Signature signature = dev::sign(prv, hash);
	j_response["result"] = signature.hexPrefixed();

	response(j_response);
}

void mcp::rpc_handler::eth_signTransaction(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"][0];
	if (!params.is_object())
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	if (!params.count("from") ||
		!params["from"].is_string() ||
		!mcp::isAddress(params["from"]))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_from_account, j_response);
		return;
	}

	std::string from_account = params["from"];
	dev::Address from(from_account);
	if (!m_key_manager->exists(from))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_from_account, j_response);
		return;
	}

	if (params.count("to") && !params["to"].is_string())
	{
		error_eth_response(response, rpc_eth_error_code::invalid_to_account, j_response);
		return;
	}

	std::string to_account = params["to"];
	dev::Address to(to_account);
	if (!m_key_manager->exists(to))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_to_account, j_response);
		return;
	}

	uint256_t amount(0);
	if (params.count("value") &&
		(!params["value"].is_string() ||
		hex_to_uint256(params["value"], amount, true)))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_value, j_response);
		return;
	}

	uint256_t gas(0);
	if (params.count("gas") &&
		(!params["gas"].is_string() ||
		hex_to_uint256(params["gas"], gas, true)))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_gas, j_response);
		return;
	}

	uint256_t gas_price(0);
	if (params.count("gasPrice") &&
		(!params["gasPrice"].is_string() ||
		hex_to_uint256(params["gasPrice"], gas_price, true)))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_gas_price, j_response);
		return;
	}

	dev::bytes data;
	if (params.count("data"))
	{
		std::string data_text = params["data"];
		if (mcp::hex_to_bytes(data_text, data))
		{
			error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
			return;
		}
	}
	else
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}
	if (data.size() > mcp::max_data_size)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
		return;
	}

	uint256_t nonce(0);
	if (params.count("nonce") &&
		(!params["nonce"].is_string() ||
		hex_to_uint256(params["nonce"], nonce, true)))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_nonce, j_response);
		return;
	}

	dev::Secret prv;
	if (!m_key_manager->find_unlocked_prv(from, prv)) {
		error_eth_response(response, rpc_eth_error_code::locked_account, j_response);
		return;
	}

	RLPStream rlpStream;
	rlpStream.appendList(9);
	rlpStream << nonce;
	rlpStream << gas_price;
	rlpStream << gas;
	if (to == dev::Address(0)) {
		rlpStream << "";
	}
	else {
		rlpStream << to;
	}
	rlpStream << amount;
	rlpStream << data;
	rlpStream << (uint64_t)((int)mcp::mcp_network + 800) << 0 << 0;
	
	dev::Signature signature = dev::sign(prv, dev::sha3(rlpStream.out()));
	j_response["result"] = signature.hexPrefixed();

	response(j_response);
}

void mcp::rpc_handler::eth_protocolVersion(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	j_response["result"] = STR(MCP_VERSION);
	response(j_response);
}

void mcp::rpc_handler::eth_syncing(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	uint64_t last_stable_mci(m_chain->last_stable_mci());
	uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());

	mcp::json result;
	result["startingBlock"] = uint64_to_hex_nofill(last_stable_mci);
	result["currentBlock"] = uint64_to_hex_nofill(last_mci);
	result["highestBlock"] = uint64_to_hex_nofill(last_stable_index);

	j_response["result"] = result;
	response(j_response);
}

void mcp::rpc_handler::eth_getLogs(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"][0];
	if (!params.is_object())
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	uint64_t fromBlock = 0;
	if (params.count("fromBlock"))
	{
		std::string fromBlockText = params["fromBlock"];
		if (fromBlockText == "latest")
		{
			fromBlock = m_chain->last_stable_index();
		}
		else if (fromBlockText == "earliest")
		{
			fromBlock = 0;
		}
		else if (fromBlockText.find("0x") == 0)
		{
			if (hex_to_uint64(fromBlockText, fromBlock, true))
			{
				error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
				return;
			}
		}
		else
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}
	}

	uint64_t toBlock = 0;
	if (params.count("toBlock"))
	{
		std::string toBlockText = params["toBlock"];
		if (toBlockText == "latest")
		{
			toBlock = m_chain->last_stable_index();
		}
		else if (toBlockText == "earliest")
		{
			toBlock = 0;
		}
		else if (toBlockText.find("0x") == 0)
		{
			if (hex_to_uint64(toBlockText, toBlock, true))
			{
				error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
				return;
			}
		}
		else
		{
			error_eth_response(response, rpc_eth_error_code::invalid_block_number, j_response);
			return;
		}
	}

	mcp::block_hash block_hash;
	block_hash.clear();
	if (params.count("blockhash"))
	{
		if (block_hash.decode_hex(params["blockhash"], true)) {
			error_eth_response(response, rpc_eth_error_code::invalid_hash, j_response);
			return;
		}
	}

	std::vector<dev::Address> search_address;
	if (params.count("address") && !params["address"].is_null())
	{
		if (request["address"].is_string())
		{
			std::string account_text = params["account"];
			bool error = !mcp::isAddress(account_text);
			if (error)
			{
				error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
				return;
			}
			dev::Address account(account_text);
			search_address.push_back(account);
		}
		else if (params["address"].is_array())
		{
			std::vector<std::string> address_l = params["address"];
			for (std::string const &address_text : address_l)
			{
				auto error(!mcp::isAddress(address_text));
				if (error)
				{
					error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
					return;
				}
				dev::Address address(address_text);
				search_address.push_back(address);
			}
		}
	}

	std::vector<dev::h256> search_topics;
	if (params.count("topics") && !params["topics"].is_null())
	{
		if (!params["topics"].is_array())
		{
			error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
			return;
		}

		std::vector<std::string> topics_l = params["topics"];
		for (std::string const &topic_text : topics_l)
		{
			mcp::uint256_union topic;
			auto error(topic.decode_hex(topic_text, true));
			if (error)
			{
				error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
				return;
			}

			search_topics.push_back(dev::h256(topic.ref()));
		}
	}
	
	int logi = 0;
	mcp::json logs_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	if (!block_hash.is_zero()) {
		std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, block_hash));
		fromBlock = toBlock = state->stable_index;
	}

	for (uint64_t i(fromBlock); i <= toBlock; i++)
	{
		mcp::block_hash block_hash;
		bool exists(!m_store.stable_block_get(transaction, i, block_hash));
		assert_x(exists);
		std::shared_ptr<mcp::block_state> block_state(m_cache->block_state_get(transaction, block_hash));
		assert_x(block_state);

		//if (block_state->block_type != mcp::block_type::light)
		//	continue;

		//if (!block_state->receipt)
		//	continue;

		std::unordered_set<dev::Address> existed_addresses;
		for (dev::Address const &address : search_address)
		{
			//if (block_state->receipt->contains_bloom(address.ref()))
			//	existed_addresses.insert(address);
		}
		if (search_address.size() > 0 && existed_addresses.size() == 0)
			continue;

		std::unordered_set<dev::h256> existed_topics;
		for (dev::h256 const &topic : search_topics)
		{
			//if (block_state->receipt->contains_bloom(topic))
			//	existed_topics.insert(topic);
		}

		if (search_topics.size() > 0 && existed_topics.size() == 0)
			continue;

		/*for (auto const &log : block_state->receipt->log)
		{
			if (search_address.empty() || existed_addresses.count(log.acct))
			{
				for (dev::h256 const &t : log.topics)
				{
					if (search_topics.size() == 0 || existed_topics.count(t))
					{
						mcp::json log_l;
						log_l["transactionIndex"] = "0x0";
						log_l["blockNumber"] = uint64_to_hex_nofill(block_state->main_chain_index.get());
						log_l["transactionHash"] = block_hash.to_string(true);
						logi++;
						log_l["logIndex"] = logi;
						log_l["blockHash"] = block_hash.to_string(true);
						log.serialize_json_eth(log_l);

						logs_l.push_back(log_l);
						break;
					}
				}
			}
		}*/
	}

	j_response["result"] = logs_l;
	response(j_response);
}

void mcp::rpc_handler::personal_importRawKey(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 2)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	if (!params[0].is_string())
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::Secret prv(params[0].get<std::string>());
	std::string password = params[1];
	if (password.empty() ||
		!mcp::validatePasswordSize(password) ||
		!mcp::validatePassword(password))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_password, j_response);
		return;
	}

	mcp::key_content kc = m_key_manager->importRawKey(prv, password);
	j_response["result"] = kc.account.hexPrefixed();
	response(j_response);
}

void mcp::rpc_handler::personal_listAccounts(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json j_accounts = mcp::json::array();
	std::list<dev::Address> account_list(m_key_manager->list());
	for (auto account : account_list)
	{
		j_accounts.push_back(account.hexPrefixed());
	}
	j_response["result"] = j_accounts;

	response(j_response);
}

void mcp::rpc_handler::personal_lockAccount(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	std::string account_text = params[0];
	if (!mcp::isAddress(account_text))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account)) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	m_key_manager->lock(account);
	j_response["result"] = true;

	response(j_response);
}

void mcp::rpc_handler::personal_newAccount(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 1)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	std::string password = params[0];
	if (password.empty() ||
		!mcp::validatePasswordSize(password) ||
		!mcp::validatePassword(password))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_password, j_response);
		return;
	}

	dev::Address account = m_key_manager->create(password, false, true);

	j_response["result"] = account.hexPrefixed();
	response(j_response);
}

void mcp::rpc_handler::personal_unlockAccount(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 3)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	std::string account_text = params[0];
	if (!mcp::isAddress(account_text))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account)) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	std::string password_text = params[1];
	auto error(m_key_manager->unlock(account, password_text));
	if (error)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_password, j_response);
		return;
	}
	
	j_response["result"] = true;

	response(j_response);
}

void mcp::rpc_handler::personal_sendTransaction(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	mcp::json tx = params[0];
	if (!tx.is_object() || !params[1].is_string())
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}
	std::string password = params[1];

	if (!tx.count("from") ||
		!tx["from"].is_string() ||
		!mcp::isAddress(tx["from"]))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_from_account, j_response);
		return;
	}

	std::string from_account = tx["from"];
	dev::Address from(from_account);
	if (!m_key_manager->exists(from))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_from_account, j_response);
		return;
	}

	dev::Address to;
	if (tx.count("to"))
	{
		if (!tx["to"].is_string() || !mcp::isAddress(tx["to"])) {
			error_eth_response(response, rpc_eth_error_code::invalid_to_account, j_response);
			return;
		}
		std::string to_account = tx["to"];
		to = dev::Address(to_account);
	}

	uint256_t amount(0);
	if (tx.count("value") &&
		(!tx["value"].is_string() ||
			hex_to_uint256(tx["value"], amount, true)))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_value, j_response);
		return;
	}

	uint256_t gas(0);
	if (tx.count("gas") &&
		(!tx["gas"].is_string() ||
			hex_to_uint256(tx["gas"], gas, true)))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_gas, j_response);
		return;
	}

	uint256_t gas_price(0);
	if (tx.count("gasPrice") &&
		(tx["gasPrice"].is_string() ||
			hex_to_uint256(tx["gasPrice"], gas_price, true)))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_gas_price, j_response);
		return;
	}

	dev::bytes data;
	if (tx.count("data"))
	{
		std::string data_text = tx["data"];
		if (mcp::hex_to_bytes(data_text, data))
		{
			error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
			return;
		}
	}
	else
	{
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}
	if (data.size() > mcp::max_data_size)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
		return;
	}
	
	boost::optional<mcp::block_hash> previous_opt;
	bool gen_next_work_l(false);
	bool async(false);
	auto rpc_l(shared_from_this());
	/*m_wallet->send_async(
		mcp::block_type::light, previous_opt, from, to, amount, gas, gas_price, data, password, [response_l, rpc_l, this](mcp::send_result result)
	{
		mcp::json response_j = response_l;
		switch (result.code)
		{
		case mcp::send_result_codes::ok:
		{
			response_j["result"] = result.block->hash().to_string(true);
			response(response_j);
			break;
		}
		case mcp::send_result_codes::from_not_exists:
			error_eth_response(response, rpc_eth_error_code::invalid_from_account, response_j);
			break;
		case mcp::send_result_codes::account_locked:
			error_eth_response(response, rpc_eth_error_code::locked_account, response_j);
			break;
		case mcp::send_result_codes::wrong_password:
			error_eth_response(response, rpc_eth_error_code::invalid_password, response_j);
			break;
		case mcp::send_result_codes::insufficient_balance:
			error_eth_response(response, rpc_eth_error_code::insufficient_balance, response_j);
			break;
		case mcp::send_result_codes::data_size_too_large:
			error_eth_response(response, rpc_eth_error_code::data_size_too_large, response_j);
			break;
		case mcp::send_result_codes::validate_error:
			error_eth_response(response, rpc_eth_error_code::validate_error, response_j);
			break;
		case mcp::send_result_codes::error:
			error_eth_response(response, rpc_eth_error_code::block_error, response_j);
			break;
		default:
			error_eth_response(response, rpc_eth_error_code::unknown_error, response_j);
			break;
		} },
		gen_next_work_l, async);*/
}

void mcp::rpc_handler::personal_sign(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 3) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}
	
	dev::bytes data;
	std::string data_text = params[0];
	if (mcp::hex_to_bytes(data_text, data))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
		return;
	}
	if (data.size() > mcp::max_data_size)
	{
		error_eth_response(response, rpc_eth_error_code::invalid_data, j_response);
		return;
	}
	
	std::string account_text = params[1];
	if (!mcp::isAddress(account_text))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_account, j_response);
		return;
	}
	
	dev::Address account(account_text);
	if (!m_key_manager->exists(account))
	{
		error_eth_response(response, rpc_eth_error_code::invalid_account, j_response);
		return;
	}

	std::string password_text = params[2];

	mcp::Secret prv;
	if (m_key_manager->decrypt_prv(account, password_text, prv)) {
		error_eth_response(response, rpc_eth_error_code::invalid_password, j_response);
		return;
	}

	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Signature signature = dev::sign(prv, hash);
	j_response["result"] = signature.hexPrefixed();

	response(j_response);
}

void mcp::rpc_handler::personal_ecRecover(mcp::json & j_response)
{
	if (!is_eth_rpc(j_response))
	{
		return;
	}

	mcp::json params = request["params"];
	if (params.size() != 2) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	mcp::bytes data;
	if (mcp::hex_to_bytes(params[0], data)) {
		error_eth_response(response, rpc_eth_error_code::invalid_params, j_response);
		return;
	}

	dev::Signature sig(params[1].get<std::string>());
	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Address from = dev::toAddress(dev::recover(sig, hash));
	j_response["result"] = from.hexPrefixed();

	response(j_response);
}

void mcp::rpc_handler::get_eth_signed_msg(dev::bytes & data, dev::h256 & hash)
{
	dev::bytes msg;
	std::string prefix = "Ethereum Signed Message:\n" + std::to_string(data.size());
	msg.resize(prefix.size() + data.size() + 1);

	msg[0] = 0x19;
	dev::bytesRef((unsigned char*)prefix.data(), prefix.size()).copyTo(dev::bytesRef(msg.data() + 1, prefix.size()));
	dev::bytesRef(data.data(), data.size()).copyTo(dev::bytesRef(msg.data() + prefix.size() + 1, data.size()));

	CryptoPP::Keccak_256 kHash;
	kHash.Update((const byte*)msg.data(), msg.size());
	kHash.Final(hash.data());
}