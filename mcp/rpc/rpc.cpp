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

mcp::rpc_config::rpc_config(bool enable_control_a) : address(boost::asio::ip::address_v4::loopback()),
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
	m_mcpRpcMethods["account_list"] = &mcp::rpc_handler::account_list;
	m_mcpRpcMethods["account_validate"] = &mcp::rpc_handler::account_validate;
	m_mcpRpcMethods["account_create"] = &mcp::rpc_handler::account_create;
	m_mcpRpcMethods["account_remove"] = &mcp::rpc_handler::account_remove;
	m_mcpRpcMethods["account_unlock"] = &mcp::rpc_handler::account_unlock;
	m_mcpRpcMethods["account_lock"] = &mcp::rpc_handler::account_lock;
	m_mcpRpcMethods["account_export"] = &mcp::rpc_handler::account_export;
	m_mcpRpcMethods["account_import"] = &mcp::rpc_handler::account_import;
	m_mcpRpcMethods["account_password_change"] = &mcp::rpc_handler::account_password_change;
	m_mcpRpcMethods["account_code"] = &mcp::rpc_handler::account_code;
	m_mcpRpcMethods["account_balance"] = &mcp::rpc_handler::account_balance;
	m_mcpRpcMethods["accounts_balances"] = &mcp::rpc_handler::accounts_balances;
	m_mcpRpcMethods["account_block_list"] = &mcp::rpc_handler::account_block_list;
	m_mcpRpcMethods["account_state_list"] = &mcp::rpc_handler::account_state_list;
	m_mcpRpcMethods["block"] = &mcp::rpc_handler::block;
	m_mcpRpcMethods["block_state"] = &mcp::rpc_handler::block_state;
	m_mcpRpcMethods["block_states"] = &mcp::rpc_handler::block_states;
	m_mcpRpcMethods["block_traces"] = &mcp::rpc_handler::block_traces;
	m_mcpRpcMethods["stable_blocks"] = &mcp::rpc_handler::stable_blocks;
	m_mcpRpcMethods["send_block"] = &mcp::rpc_handler::send_block;
	m_mcpRpcMethods["send_offline_block"] = &mcp::rpc_handler::send_offline_block;
	m_mcpRpcMethods["block_summary"] = &mcp::rpc_handler::block_summary;
	m_mcpRpcMethods["sign_msg"] = &mcp::rpc_handler::sign_msg;
	m_mcpRpcMethods["version"] = &mcp::rpc_handler::version;
	m_mcpRpcMethods["status"] = &mcp::rpc_handler::status;
	m_mcpRpcMethods["peers"] = &mcp::rpc_handler::peers;
	m_mcpRpcMethods["nodes"] = &mcp::rpc_handler::nodes;
	m_mcpRpcMethods["witness_list"] = &mcp::rpc_handler::witness_list;
	m_mcpRpcMethods["estimate_gas"] = &mcp::rpc_handler::estimate_gas;
	m_mcpRpcMethods["logs"] = &mcp::rpc_handler::logs;
	m_mcpRpcMethods["debug_trace_transaction"] = &mcp::rpc_handler::debug_trace_transaction;
	m_mcpRpcMethods["debug_storage_range_at"] = &mcp::rpc_handler::debug_storage_range_at;

	m_ethRpcMethods["net_version"] = &mcp::rpc_handler::net_version;
	m_ethRpcMethods["net_listening"] = &mcp::rpc_handler::net_listening;
	m_ethRpcMethods["net_peerCount"] = &mcp::rpc_handler::net_peerCount;
	m_ethRpcMethods["web3_clientVersion"] = &mcp::rpc_handler::web3_clientVersion;
	m_ethRpcMethods["web3_sha3"] = &mcp::rpc_handler::web3_sha3;

	m_ethRpcMethods["eth_blockNumber"] = &mcp::rpc_handler::eth_blockNumber;
	m_ethRpcMethods["eth_getTransactionCount"] = &mcp::rpc_handler::eth_getTransactionCount;
	m_ethRpcMethods["eth_chainId"] = &mcp::rpc_handler::eth_chainId;
	m_ethRpcMethods["eth_gasPrice"] = &mcp::rpc_handler::eth_gasPrice;
	m_ethRpcMethods["eth_estimateGas"] = &mcp::rpc_handler::eth_estimateGas;
	m_ethRpcMethods["eth_getBlockByNumber"] = &mcp::rpc_handler::eth_getBlockByNumber;
	m_ethRpcMethods["eth_getBlockTransactionCountByHash"] = &mcp::rpc_handler::eth_getBlockTransactionCountByHash;
	m_ethRpcMethods["eth_getBlockTransactionCountByNumber"] = &mcp::rpc_handler::eth_getBlockTransactionCountByNumber;
	m_ethRpcMethods["eth_sendRawTransaction"] = &mcp::rpc_handler::eth_sendRawTransaction;
	m_ethRpcMethods["eth_sendTransaction"] = &mcp::rpc_handler::eth_sendTransaction;
	m_ethRpcMethods["eth_call"] = &mcp::rpc_handler::eth_call;

	m_ethRpcMethods["eth_protocolVersion"] = &mcp::rpc_handler::eth_protocolVersion;
	m_ethRpcMethods["eth_syncing"] = &mcp::rpc_handler::eth_syncing;
	m_ethRpcMethods["eth_getLogs"] = &mcp::rpc_handler::eth_getLogs;
	m_ethRpcMethods["eth_getCode"] = &mcp::rpc_handler::eth_getCode;
	m_ethRpcMethods["eth_getStorageAt"] = &mcp::rpc_handler::eth_getStorageAt;
	m_ethRpcMethods["eth_getTransactionByHash"] = &mcp::rpc_handler::eth_getTransactionByHash;
	m_ethRpcMethods["eth_getTransactionByBlockHashAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex;
	m_ethRpcMethods["eth_getTransactionByBlockNumberAndIndex"] = &mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex;
	m_ethRpcMethods["eth_getTransactionReceipt"] = &mcp::rpc_handler::eth_getTransactionReceipt;
	m_ethRpcMethods["eth_getBalance"] = &mcp::rpc_handler::eth_getBalance;
	m_ethRpcMethods["eth_getBlockByHash"] = &mcp::rpc_handler::eth_getBlockByHash;
	m_ethRpcMethods["eth_accounts"] = &mcp::rpc_handler::eth_accounts;
	m_ethRpcMethods["eth_sign"] = &mcp::rpc_handler::eth_sign;
	m_ethRpcMethods["eth_signTransaction"] = &mcp::rpc_handler::eth_signTransaction;

	m_ethRpcMethods["personal_importRawKey"] = &mcp::rpc_handler::personal_importRawKey;
	m_ethRpcMethods["personal_listAccounts"] = &mcp::rpc_handler::personal_listAccounts;
	m_ethRpcMethods["personal_lockAccount"] = &mcp::rpc_handler::personal_lockAccount;
	m_ethRpcMethods["personal_newAccount"] = &mcp::rpc_handler::personal_newAccount;
	m_ethRpcMethods["personal_unlockAccount"] = &mcp::rpc_handler::personal_unlockAccount;
	m_ethRpcMethods["personal_sendTransaction"] = &mcp::rpc_handler::personal_sendTransaction;
	m_ethRpcMethods["personal_sign"] = &mcp::rpc_handler::personal_sign;
	m_ethRpcMethods["personal_ecRecover"] = &mcp::rpc_handler::personal_ecRecover;

	m_ethRpcMethods["eth_requestAccounts"] = &mcp::rpc_handler::eth_accounts;
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

void mcp::rpc_handler::account_list(mcp::json &j_response, bool &)
{
	mcp::json j_accounts = mcp::json::array();
	std::list<dev::Address> account_list(m_key_manager->list());
	for (auto account : account_list)
	{
		j_accounts.push_back(account.hexPrefixed());
	}
	j_response["accounts"] = j_accounts;
}

void mcp::rpc_handler::account_validate(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	j_response["valid"] = mcp::isAddress(account_text) ? 1 : 0;
}

void mcp::rpc_handler::account_create(mcp::json &j_response, bool &)
{
	if (!request.count("password") || !request["password"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidPassword());
	}

	std::string password = request["password"];
	if (password.empty())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_EmptyPassword());
	}

	if (!mcp::validatePasswordSize(password))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLengthPassword());
	}

	if (!mcp::validatePassword(password))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidCharactersPassword());
	}

	if (request.count("backup") || !request["backup"].is_boolean())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidGenNextWorkValue());
	}
	bool backup_l = request["backup"];

	bool gen_next_work_l(false);
	dev::Address new_account = m_key_manager->create(password, gen_next_work_l, backup_l);
	j_response["account"] = new_account.hexPrefixed();
}

void mcp::rpc_handler::account_remove(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	if (!request.count("password") || !request["password"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidPassword());
	}

	std::string password_text = request["password"];
	if (password_text.empty())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_EmptyPassword());
	}

	if (m_key_manager->remove(account, password_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
	}
}

void mcp::rpc_handler::account_password_change(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	if (!request.count("old_password") || !request["old_password"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidOldPassword());
	}
	std::string old_password_text = request["old_password"];

	if (!request.count("new_password") || (!request["new_password"].is_string()))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidNewPassword());
	}
	std::string new_password_text = request["new_password"];

	if (!mcp::validatePasswordSize(new_password_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLengthPassword());
	}

	if (!mcp::validatePassword(new_password_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidCharactersPassword());
	}

	if (m_key_manager->change_password(account, old_password_text, new_password_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
	}
}

void mcp::rpc_handler::account_unlock(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	if (!request.count("password") || !request["password"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidPassword());
	}

	std::string password_text = request["password"];
	if (password_text.empty())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_EmptyPassword());
	}

	if (m_key_manager->unlock(account, password_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
	}
}

void mcp::rpc_handler::account_lock(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	if (!m_key_manager->exists(account))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	m_key_manager->lock(account);
}

void mcp::rpc_handler::account_export(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(account_text);
	mcp::key_content kc;
	if (!m_key_manager->find(account, kc))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_AccountNotExist());
	}

	j_response["json"] = kc.to_json();
}

void mcp::rpc_handler::account_import(mcp::json &j_response, bool &)
{
	if (!request.count("json") || !request["json"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidJson());
	}

	std::string json_text = request["json"];
	bool gen_next_work_l(false);
	mcp::key_content kc;

	if (m_key_manager->import(json_text, kc, gen_next_work_l))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidJson());
	}
}

void mcp::rpc_handler::account_code(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	Address account(account_text);
	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

	j_response["account_code"] = toJS(c_state.code(account));
}

void mcp::rpc_handler::account_balance(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	std::string account_text = request["account"];
	if (!mcp::isAddress(account_text))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

	auto balance(c_state.balance(dev::Address(account_text)));
	j_response["balance"] = balance.convert_to<std::string>();
}

void mcp::rpc_handler::accounts_balances(mcp::json &j_response, bool &)
{
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

void mcp::rpc_handler::account_block_list(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(0);
	try
	{
		account = jsToAddress(request["account"]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	if (!request.count("limit"))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLimit());
	}

	uint64_t limit_l = jsToInt(request["limit"]);
	if (limit_l > list_max_limit)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLimitTooLarge());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	dev::h256 search_hash(0);

	if (request.count("index"))
	{
		if (!request["index"].is_string())
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndex());

		try
		{
			search_hash = jsToHash(request["index"]);
		}
		catch (...)
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndex());
		}

		std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, search_hash));
		if (!(acc_state && acc_state->account() == account))
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndexNotExsist());
		}
	}
	else
	{
		std::shared_ptr<mcp::account_state> acc_state(m_cache->latest_account_state_get(transaction, account));
		if (acc_state)
			search_hash = acc_state->hash();
		else
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndexNotExsist());
	}
}

void mcp::rpc_handler::account_state_list(mcp::json &j_response, bool &)
{
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	dev::Address account(0);
	try
	{
		account = jsToAddress(request["account"]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	if (!request.count("limit"))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLimit());
	}

	uint64_t limit_l = jsToInt(request["limit"]);
	if (limit_l > list_max_limit)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLimitTooLarge());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	dev::h256 search_hash(0);

	if (request.count("index"))
	{
		if (!request["index"].is_string())
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndex());

		try
		{
			search_hash = jsToHash(request["index"]);
		}
		catch (...)
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndex());
		}

		std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, search_hash));
		if (!(acc_state && acc_state->account() == account))
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndexNotExsist());
		}
	}
	else
	{
		std::shared_ptr<mcp::account_state> acc_state(m_cache->latest_account_state_get(transaction, account));
		if (acc_state)
			search_hash = acc_state->hash();
		else
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndexNotExsist());
	}

	mcp::json acc_states_l = mcp::json::array();
	int i = 0;
	while (i < limit_l && search_hash != dev::h256(0))
	{
		std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, search_hash));
		if (!acc_state)
			break;

		mcp::json acc_state_l;
		acc_state_l["hash"] = acc_state->hash().hex();
		acc_state_l["account"] = acc_state->account().hexPrefixed();
		// acc_state_l["block_hash"] = acc_state->block_hash.to_string();
		// acc_state_l["previous"] = acc_state->previous.to_string();
		acc_state_l["balance"] = acc_state->balance().str();
		acc_state_l["nonce"] = acc_state->nonce().str();
		acc_state_l["storage_root"] = acc_state->baseRoot().hex();
		acc_state_l["code_hash"] = acc_state->codeHash().hex();
		acc_state_l["is_alive"] = acc_state->isAlive();

		acc_states_l.push_back(acc_state_l);

		search_hash = acc_state->previous();
		i++;
	}

	j_response["account_states"] = acc_states_l;
	if (search_hash != dev::h256(0))
		j_response["next_index"] = search_hash.hex();
	else
		j_response["next_index"] = nullptr;
}

void mcp::rpc_handler::block(mcp::json &j_response, bool &)
{
	if (!request.count("hash") || !request["hash"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	mcp::block_hash block_hash(0);
	try
	{
		block_hash = jsToHash(request["hash"]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, block_hash));
	uint64_t block_number;

	if (block == nullptr)
	{
		throw "";
	}

	mcp::json j_block = toJson(*block, true);
	if (!m_cache->block_number_get(transaction, block_hash, block_number))
	{
		j_block["number"] = toJS(block_number);
	}

	u256 gasUsed = 0;
	u256 minGasPrice = 0;
	for (auto &th : block->links())
	{
		auto t = m_cache->transaction_get(transaction, th);
		gasUsed += t->gas();
		minGasPrice = minGasPrice == 0 ? t->gasPrice() : std::min(minGasPrice, t->gasPrice());

		auto td = m_store.transaction_address_get(transaction, th);
		j_block["transactions"].push_back(
			toJson(LocalisedTransaction(*t, block_hash, td->index, block_number)));
	}
	j_block["gasUsed"] = toJS(gasUsed);
	j_block["minGasPrice"] = toJS(minGasPrice);

	j_response["result"] = j_block;
}

void mcp::rpc_handler::block_state(mcp::json &j_response, bool &)
{
	mcp::block_hash block_hash(0);
	try
	{
		block_hash = jsToHash(request["hash"]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	try
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());
		auto block = m_cache->block_get(transaction, block_hash);
		if (block == nullptr)
		{
			throw "";
		}
		std::shared_ptr<mcp::block_state> state(m_store.block_state_get(transaction, block_hash));
		mcp::json block_state_l;
		state->serialize_json(block_state_l);
		j_response["block_state"] = block_state_l;
	}
	catch (...)
	{
		j_response["block_state"] = nullptr;
	}
}

void mcp::rpc_handler::block_states(mcp::json &j_response, bool &)
{

	mcp::json states_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	std::vector<std::string> hashes_l;
	try
	{
		hashes_l = request["hashes"].get<std::vector<std::string>>();
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}
	for (std::string const &hash_text : hashes_l)
	{
		mcp::block_hash block_hash(0);
		try
		{
			block_hash = jsToHash(hash_text);
		}
		catch (...)
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
		}
		mcp::json state_l;

		auto block = m_cache->block_get(transaction, block_hash);
		if (block == nullptr)
		{
			throw "";
		}
		std::shared_ptr<mcp::block_state> state(m_store.block_state_get(transaction, block_hash));
		state->serialize_json(state_l);
		states_l.push_back(state_l);
	}
	j_response["block_states"] = states_l;
}

void mcp::rpc_handler::block_traces(mcp::json &j_response, bool &)
{
	mcp::block_hash block_hash(0);
	try
	{
		block_hash = jsToHash(request["hash"]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	mcp::json response_l;
	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::list<std::shared_ptr<mcp::trace>> traces;
	m_store.traces_get(transaction, block_hash, traces);

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

	j_response["block_traces"] = traces_l;
}

void mcp::rpc_handler::stable_blocks(mcp::json &j_response, bool &)
{
	bool error(false);

	uint64_t index(0);
	if (!request.count("index"))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidIndex());
	}
	index = jsToInt(request["index"]);

	uint64_t limit_l(0);
	if (!request.count("limit"))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLimit());
	}
	limit_l = jsToInt(request["limit"]);
	if (limit_l > list_max_limit)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidLimit());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	uint64_t last_stable_index(m_chain->last_stable_index());

	mcp::json block_list_l = mcp::json::array();
	int blocks_count(0);
	for (uint64_t stable_index = index; stable_index <= last_stable_index; stable_index++)
	{
		mcp::block_hash block_hash_l;
		bool exists(!m_store.stable_block_get(transaction, stable_index, block_hash_l));
		assert_x(exists);

		auto block = m_cache->block_get(transaction, block_hash_l);
		assert_x(block);

		mcp::json block_l = toJson(*block);
		block_list_l.push_back(block_l);

		blocks_count++;
		if (blocks_count == limit_l)
			break;
	}

	j_response["blocks"] = block_list_l;

	uint64_t next_index = index + limit_l;
	if (next_index <= last_stable_index)
		j_response["next_index"] = next_index;
	else
		j_response["next_index"] = nullptr;
}

void mcp::rpc_handler::estimate_gas(mcp::json &j_response, bool &)
{
	TransactionSkeleton t = mcp::toTransactionSkeletonForMcp(request);

	dev::eth::McInfo mc_info;
	uint64_t block_number = m_chain->last_stable_mci();
	if (!try_get_mc_info(mc_info, block_number))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidMci());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::pair<u256, bool> result = m_chain->estimate_gas(transaction, m_cache, t.from, t.value, t.to, t.data, static_cast<int64_t>(t.gas), t.gasPrice, mc_info);

	/// this error is reported if the gas less than 21000, the logic has not been confirmed, response other code ?
	if (!result.second)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidGas());
	}

	j_response["gas"] = result.first;
}

void mcp::rpc_handler::call(mcp::json &j_response, bool &)
{
	TransactionSkeleton ts = mcp::toTransactionSkeletonForMcp(request);
	ts.gasPrice = 0;
	ts.gas = mcp::block_max_gas;

	Transaction t(ts);
	t.setSignature(h256(0), h256(0), 0);

	dev::eth::McInfo mc_info;
	uint64_t block_number = m_chain->last_stable_mci();
	if (!try_get_mc_info(mc_info, block_number))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidMci());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = m_chain->execute(
		transaction,
		m_cache,
		t,
		mc_info,
		Permanence::Reverted,
		dev::eth::OnOpFunc());

	j_response["output"] = dev::toHex(result.first.output);
}

void mcp::rpc_handler::logs(mcp::json &j_response, bool &)
{
	uint64_t from_stable_block_index(0);
	if (request.count("from_stable_block_index"))
	{
		from_stable_block_index = jsToInt(request["from_stable_block_index"]);
	}

	uint64_t to_stable_block_index;
	if (request.count("to_stable_block_index"))
	{
		to_stable_block_index = jsToInt(request["to_stable_block_index"]);
	}
	else
	{
		to_stable_block_index = m_chain->last_stable_index();
	}

	std::unordered_set<dev::Address> search_account;
	if (request.count("account") && request["account"].is_string())
	{
		std::string account_text = request["account"];
		if (!mcp::isAddress(account_text))
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
		}
		search_account.insert(dev::Address(account_text));
	}

	std::unordered_set<dev::h256> search_topics;
	if (request.count("topics") && !request["topics"].is_null())
	{
		if (!request["topics"].is_array())
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidTopics());
		}

		std::vector<std::string> topics_l = request["topics"];
		for (std::string const &topic_text : topics_l)
		{
			dev::h256 topic(0);
			try
			{
				topic = jsToHash(topic_text);
			}
			catch (...)
			{
				BOOST_THROW_EXCEPTION(RPC_Error_InvalidTopics());
			}

			search_topics.insert(dev::h256(topic.ref()));
		}
	}

	mcp::json logs_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	for (uint64_t i(from_stable_block_index); i <= to_stable_block_index; i++)
	{
		mcp::block_hash block_hash;
		if (m_store.stable_block_get(transaction, i, block_hash))
		{
			continue;
		}

		auto block_state = m_cache->block_state_get(transaction, block_hash);
		if (!block_state || !block_state->is_stable)
		{
			continue;
		}

		auto block = m_cache->block_get(transaction, block_hash);
		for (auto &th : block->links())
		{
			auto t = m_cache->transaction_get(transaction, th);
			auto tr = m_store.transaction_receipt_get(transaction, th);
			auto td = m_cache->transaction_address_get(transaction, th);
			if (t == nullptr || tr == nullptr || td == nullptr)
				continue;

			auto lt = dev::eth::LocalisedTransactionReceipt(
				*tr,
				t->sha3(),
				block_hash,
				i,
				t->from(),
				t->to(),
				td->index,
				toAddress(t->from(), t->nonce()),
				search_account,
				search_topics);

			mcp::json logs = toJson(lt.localisedLogs());
			if (logs.size() > 0)
			{
				logs_l.push_back(logs);
			}
		}
	}

	j_response["logs"] = logs_l;
}

void mcp::rpc_handler::send_block(mcp::json &j_response, bool &async)
{
	TransactionSkeleton t = mcp::toTransactionSkeletonForMcp(request);

	boost::optional<std::string> password;
	if (request.count("password") && request["password"].is_string())
	{
		password = request["password"].get<std::string>();
	}

	auto rpc_l(shared_from_this());
	auto fun = [rpc_l, j_response, this](h256 &h, boost::optional<dev::Exception const &> e)
	{
		mcp::json j_resp = j_response;
		if (!e)
		{
			j_resp["hash"] = h.hex();
		}
		else
		{
			toRpcExceptionJson(*e, j_resp);
		}
		response(j_resp);
	};

	async = true;
	m_wallet->send_async(t, fun, password);
}

void mcp::rpc_handler::send_offline_block(mcp::json &j_response, bool &)
{
	TransactionSkeleton ts = mcp::toTransactionSkeletonForMcp(request);

	dev::Signature signature(0);
	try
	{
		signature = jsToSignature(request["signature"]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidSignature());
	}

	try
	{
		Transaction t(ts);
		dev::SignatureStruct *sig = (dev::SignatureStruct *)&signature;
		t.setSignature(sig->r, sig->s, sig->v);
		j_response["hash"] = m_wallet->importTransaction(t).hex();
	}
	catch (dev::Exception &e)
	{
		toRpcExceptionJson(e, j_response);
	}
}

void mcp::rpc_handler::block_summary(mcp::json &j_response, bool &)
{
	if (!request.count("hash") || (!request["hash"].is_string()))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	std::string hash_text = request["hash"];
	mcp::block_hash hash;
	try
	{
		hash = jsToHash(hash_text);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	mcp::summary_hash summary;
	bool exists(!m_cache->block_summary_get(transaction, hash, summary));
	if (!exists)
	{
		j_response["summary"] = nullptr;
	}
	else
	{
		j_response["summary"] = summary.hex();

		auto block(m_cache->block_get(transaction, hash));
		assert_x(block);
		auto block_state(m_cache->block_state_get(transaction, hash));
		assert_x(block_state);

		// previous summary hash
		mcp::summary_hash previous_summary_hash(0);
		if (block->previous() != dev::h256(0))
		{
			bool previous_summary_hash_error(m_cache->block_summary_get(transaction, block->previous(), previous_summary_hash));
			assert_x(!previous_summary_hash_error);
		}
		j_response["previous_summary"] = previous_summary_hash.hex();

		// parent summary hashs
		mcp::json parent_summaries_l = mcp::json::array();
		for (mcp::block_hash const &pblock_hash : block->parents())
		{
			mcp::summary_hash p_summary_hash;
			bool p_summary_hash_error(m_cache->block_summary_get(transaction, pblock_hash, p_summary_hash));
			assert_x(!p_summary_hash_error);

			parent_summaries_l.push_back(p_summary_hash.hex());
		}
		j_response["parent_summaries"] = parent_summaries_l;

		// link summary hashs
		// std::shared_ptr<std::list<mcp::block_hash>> links(block->links());
		mcp::json link_summaries_l = mcp::json::array();
		for (auto it(block->links().begin()); it != block->links().end(); it++)
		{
			mcp::block_hash const &link_hash(*it);
			mcp::summary_hash l_summary_hash;
			bool l_summary_hash_error(m_cache->block_summary_get(transaction, link_hash, l_summary_hash));
			assert_x(!l_summary_hash_error);

			link_summaries_l.push_back(l_summary_hash.hex());
		}
		j_response["link_summaries"] = link_summaries_l;

		// skip list
		mcp::json skiplist_summaries_l = mcp::json::array();
		if (block_state->is_on_main_chain)
		{
			std::set<mcp::summary_hash> summary_skiplist;
			std::vector<uint64_t> skip_list_mcis = m_chain->cal_skip_list_mcis(*block_state->main_chain_index);
			for (uint64_t &mci : skip_list_mcis)
			{
				mcp::block_hash sl_block_hash;
				bool sl_block_hash_error(m_store.main_chain_get(transaction, mci, sl_block_hash));
				assert_x(!sl_block_hash_error);

				mcp::summary_hash sl_summary_hash;
				bool sl_summary_hash_error(m_cache->block_summary_get(transaction, sl_block_hash, sl_summary_hash));
				assert_x(!sl_summary_hash_error);
				summary_skiplist.insert(sl_summary_hash);
			}

			for (mcp::summary_hash s : summary_skiplist)
				skiplist_summaries_l.push_back(s.hex());
		}
		j_response["skiplist_summaries"] = skiplist_summaries_l;

		j_response["status"] = (uint64_t)block_state->status;
	}
}

void mcp::rpc_handler::sign_msg(mcp::json &j_response, bool &)
{
	dev::Address account(0);
	try
	{
		std::string account_text = request["account"];
		if (!mcp::isAddress(account_text))
		{
			BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
		}
		account = dev::Address(account_text);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}

	h256 sign_msg(0);
	try
	{
		std::string sign_msg_text = request["msg"];
		sign_msg = jsToHash(sign_msg_text);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidMsg());
	}

	if (!request.count("password") || (!request["password"].is_string()))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidPassword());
	}
	std::string password_a = request["password"];
	dev::Secret prv;
	if (password_a.empty())
	{
		if (!m_key_manager->find_unlocked_prv(account, prv))
		{
			BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
		}
	}
	else
	{
		if (m_key_manager->decrypt_prv(account, password_a, prv))
		{
			BOOST_THROW_EXCEPTION(RPC_Error_WrongPassword());
		}
	}

	dev::Signature sig(dev::sign(prv, sign_msg));
	j_response["signature"] = sig.hex();
}

void mcp::rpc_handler::version(mcp::json &j_response, bool &)
{
	j_response["version"] = STR(MCP_VERSION);
	j_response["rpc_version"] = "1";
	j_response["store_version"] = std::to_string(m_store.version_get());
}

void mcp::rpc_handler::status(mcp::json &j_response, bool &)
{
	// stable_mci
	uint64_t last_stable_mci(m_chain->last_stable_mci());
	uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());

	j_response["syncing"] = mcp::node_sync::is_syncing() ? 1 : 0;
	j_response["last_stable_mci"] = last_stable_mci;
	j_response["last_mci"] = last_mci;
	j_response["last_stable_block_index"] = last_stable_index;
}

void mcp::rpc_handler::peers(mcp::json &j_response, bool &)
{
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
}

void mcp::rpc_handler::nodes(mcp::json &j_response, bool &)
{
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
}

void mcp::rpc_handler::witness_list(mcp::json &j_response, bool &)
{
	mcp::witness_param const &w_param(mcp::param::curr_witness_param());
	mcp::json witness_list_l = mcp::json::array();
	for (auto i : w_param.witness_list)
	{
		witness_list_l.push_back(i.hexPrefixed());
	}
	j_response["witness_list"] = witness_list_l;
}

void mcp::rpc_handler::debug_trace_transaction(mcp::json &j_response, bool &)
{
	if (!request.count("hash") || !request["hash"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	std::string hash_text = request["hash"];
	mcp::block_hash hash;
	try
	{
		hash = jsToHash(hash_text);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	dev::eth::McInfo mc_info;
	if (!m_chain->get_mc_info_from_block_hash(transaction, m_cache, hash, mc_info))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidMci());
	}

	mcp::json options;
	options["disable_storage"] = true;
	options["disable_memory"] = false;
	options["disable_stack"] = false;
	options["full_storage"] = false;
	if (request.count("options"))
	{
		mcp::json options_json = request["options"];
		if (options_json.count("disable_storage"))
			options["disable_storage"] = options_json["disable_storage"];
		if (options_json.count("disable_memory"))
			options["disable_memory"] = options_json["disable_memory"];
		if (options_json.count("disable_stack"))
			options["disable_stack"] = options_json["disable_stack"];
		if (options_json.count("full_storage"))
			options["full_storage"] = options_json["full_storage"];
	}

	try
	{
		dev::eth::EnvInfo env(transaction, m_store, m_cache, mc_info, mcp::chain_id);
		auto block(m_cache->block_get(transaction, hash));
		assert_x(block);
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
		mcp::ExecutionResult er;
		std::list<std::shared_ptr<mcp::trace>> traces;
		mcp::Executive e(c_state, env, traces);
		e.setResultRecipient(er);
	}
	catch (Exception const &_e)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_VMException());
	}
	catch (std::exception const &_e)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_UnknowError());
	}
}

void mcp::rpc_handler::debug_storage_range_at(mcp::json &j_response, bool &)
{
	if (!request.count("hash") || !request["hash"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	std::string hash_text = request["hash"];
	mcp::block_hash hash;
	try
	{
		hash = jsToHash(hash_text);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidHash());
	}

	dev::Address acct(0);
	if (!request.count("account") || !request["account"].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}
	std::string account_text = request["account"];
	if (!mcp::isAddress(request["account"]))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidAccount());
	}
	acct = dev::Address(account_text);

	h256 begin;
	try
	{
		begin = jsToHash(request["begin"]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidBegin());
	}

	uint64_t max_results(0);
	if (!request.count("max_results"))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_InvalidBegin());
	}
	max_results = jsToInt(request["max_results"]);

	j_response["storage"] = mcp::json::object();

	try
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

		std::map<h256, std::pair<u256, u256>> const storage(c_state.storage(acct));

		// begin is inclusive
		auto itBegin = storage.lower_bound(begin);
		for (auto it = itBegin; it != storage.end(); ++it)
		{
			if (j_response["storage"].size() == static_cast<unsigned>(max_results))
			{
				j_response["next_key"] = toCompactHexPrefixed(it->first, 32);
				break;
			}

			mcp::json keyValue = mcp::json::object();
			std::string hashedKey = toCompactHexPrefixed(it->first, 32);
			keyValue["key"] = toCompactHexPrefixed(it->second.first, 32);
			keyValue["value"] = toCompactHexPrefixed(it->second.second, 32);

			j_response["storage"][hashedKey] = keyValue;
		}
	}
	catch (Exception const &_e)
	{
		cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
			   << diagnostic_information(_e);
		exit(1);
	}
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
						LOG(this_l->m_log.error) << "RESPONSE:" << body;
						this_l->write_result(body, version);
						boost::beast::http::async_write(this_l->socket, this_l->res, [this_l](boost::system::error_code const & e, size_t size)
						{
						});
					}
					catch (std::exception const & e)
					{
                        LOG(this_l->m_log.error) << "rpc http write error:" << e.what() << "," << boost::stacktrace::stacktrace();
						throw "";
					}
				});
				if (this_l->request.method() == boost::beast::http::verb::post)
				{
					auto handler(std::make_shared<mcp::rpc_handler>(this_l->rpc, this_l->request.body(), response_handler,0));
					handler->process_request();
				}
				else
				{
					mcp::json j_response;
					j_response["msg"] = "Can only POST requests";
					response_handler(j_response);
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
	mcp::json j_response;
	bool async = false;

	try
	{
		if (!rpc.config.enable_control)
		{
			BOOST_THROW_EXCEPTION(RPC_Error_Disabled());
		}

		request = mcp::json::parse(body);

		LOG(m_log.error) << "REQUEST:" << request;

		if (request.count("action"))
		{
			auto pointer = m_mcpRpcMethods.find(request["action"]);
			if (pointer != m_mcpRpcMethods.end())
			{
				j_response["code"] = 0;
				j_response["msg"] = "OK";
				(this->*(pointer->second))(j_response, async);
			}
			else
			{
				BOOST_THROW_EXCEPTION(RPC_Error_UnknownCommand());
			}
		}
		else if (request.count("method"))
		{
			auto pointer = m_ethRpcMethods.find(request["method"]);
			if (pointer != m_ethRpcMethods.end())
			{
				if (!request.count("id") ||
					!request.count("jsonrpc") ||
					!request.count("params") ||
					!request["params"].is_array())
				{
					BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidRequest());
				}

				j_response["id"] = request["id"];
				j_response["jsonrpc"] = request["jsonrpc"];
				j_response["result"] = nullptr;
				(this->*(pointer->second))(j_response, async);
			}
			else
			{
				BOOST_THROW_EXCEPTION(RPC_Error_Eth_MethodNotFound());
			}
		}
		else
		{
			BOOST_THROW_EXCEPTION(RPC_Error_UnknownCommand());
		}
	}
	catch (mcp::RpcEthException const &err)
	{
		err.toJson(j_response);
	}
	catch (mcp::RpcException const &err)
	{
		err.toJson(j_response);
	}
	catch (std::exception const &err)
	{
		j_response["msg"] = "Unable to parse JSON";
	}
	catch (...)
	{
		j_response["msg"] = "Internal server error in HTTP RPC";
	}

	if (!async)
	{
		response(j_response);
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

void mcp::rpc_handler::eth_blockNumber(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(m_chain->last_stable_index());
}

void mcp::rpc_handler::eth_getTransactionCount(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}
	BlockNumber blockTag = LatestBlock;
	if (params.size() >= 2 && params[1].is_string())
	{
		blockTag = jsToBlockNumber(params[1]);
	}

	try
	{
		j_response["result"] = toJS(m_wallet->getTransactionCount(jsToAddress(params[0]), blockTag));
	}
	catch (...)
	{
		j_response["result"] = 0;
	}
}

void mcp::rpc_handler::eth_chainId(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(mcp::chain_id);
}

void mcp::rpc_handler::eth_gasPrice(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(mcp::gas_price);
}

void mcp::rpc_handler::eth_estimateGas(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_object())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	TransactionSkeleton ts = mcp::toTransactionSkeletonForEth(params[0]);

	dev::eth::McInfo mc_info;
	uint64_t block_number = m_chain->last_stable_mci();
	if (!try_get_mc_info(mc_info, block_number))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidBlock());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());

	std::pair<u256, bool> result = m_chain->estimate_gas(
		transaction,
		m_cache,
		ts.from,
		ts.value,
		ts.to,
		ts.data,
		static_cast<int64_t>(ts.gas),
		ts.gasPrice,
		mc_info);

	/// this error is reported if the gas less than 21000, the logic has not been confirmed, response other code ?
	if (!result.second)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidGas());
	}

	j_response["result"] = toJS(result.first);
}

void mcp::rpc_handler::eth_getBlockByNumber(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}
	bool is_full = params[1].is_null() ? false : (bool)params[1];

	uint64_t block_number = 0;
	std::string blockText = params[0];
	if (blockText == "latest" || blockText == "pending")
	{
		block_number = m_chain->last_stable_index();
	}
	else if (blockText == "earliest")
	{
		block_number = 0;
	}
	else
	{
		block_number = jsToInt(blockText);
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	auto block(m_cache->block_get(transaction, block_number));
	mcp::block_hash block_hash;
	if (block == nullptr || m_cache->block_number_get(transaction, block_number, block_hash))
	{
		return;
	}

	mcp::json j_block = toJson(*block, true);
	j_block["number"] = toJS(block_number);

	u256 gasUsed = 0;
	u256 minGasPrice = 0;

	for (auto &th : block->links())
	{
		auto t = m_cache->transaction_get(transaction, th);
		gasUsed += t->gas();
		minGasPrice = minGasPrice == 0 ? t->gasPrice() : std::min(minGasPrice, t->gasPrice());

		auto td = m_store.transaction_address_get(transaction, th);
		if (is_full)
		{
			j_block["transactions"].push_back(toJson(LocalisedTransaction(*t, block_hash, td->index, block_number)));
		}
		else
		{
			j_block["transactions"].push_back(th.hexPrefixed());
		}
	}
	j_block["gasUsed"] = toJS(gasUsed);
	j_block["minGasPrice"] = toJS(minGasPrice);

	j_response["result"] = j_block;
}

void mcp::rpc_handler::eth_getBlockByHash(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}
	bool is_full = params[1].is_null() ? false : (bool)params[1];

	try
	{
		mcp::block_hash block_hash = jsToHash(params[0]);

		mcp::db::db_transaction transaction(m_store.create_transaction());
		auto block = m_cache->block_get(transaction, block_hash);
		uint64_t block_number;
		if (block == nullptr)
		{
			throw "";
		}

		mcp::json j_block = toJson(*block, true);
		if (!m_cache->block_number_get(transaction, block_hash, block_number))
		{
			j_block["number"] = toJS(block_number);
		}

		u256 gasUsed = 0;
		u256 minGasPrice = 0;
		for (auto &th : block->links())
		{
			auto t = m_cache->transaction_get(transaction, th);
			gasUsed += t->gas();
			minGasPrice = minGasPrice == 0 ? t->gasPrice() : std::min(minGasPrice, t->gasPrice());

			auto td = m_store.transaction_address_get(transaction, th);
			if (is_full)
			{
				j_block["transactions"].push_back(toJson(LocalisedTransaction(*t, block_hash, td->index, block_number)));
			}
			else
			{
				j_block["transactions"].push_back(th.hexPrefixed());
			}
		}
		j_block["gasUsed"] = toJS(gasUsed);
		j_block["minGasPrice"] = toJS(minGasPrice);

		j_response["result"] = j_block;
	}
	catch (...)
	{
	}
}

void mcp::rpc_handler::eth_sendRawTransaction(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	try
	{
		Transaction t(jsToBytes(params[0], OnFailed::Throw), CheckTransaction::None);
		j_response["result"] = toJS(m_wallet->importTransaction(t));
	}
	catch (dev::Exception &e)
	{
		toRpcExceptionEthJson(e, j_response);
	}
}

void mcp::rpc_handler::eth_sendTransaction(mcp::json &j_response, bool &async)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_object())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	TransactionSkeleton t = mcp::toTransactionSkeletonForEth(params[0]);

	auto rpc_l(shared_from_this());
	auto fun = [rpc_l, j_response, this](h256 &h, boost::optional<dev::Exception const &> e)
	{
		mcp::json j_resp = j_response;
		if (!e)
		{
			j_resp["result"] = toJS(h);
		}
		else
		{
			toRpcExceptionEthJson(*e, j_resp);
		}
		response(j_resp);
	};

	async = true;
	m_wallet->send_async(t, fun);
}

void mcp::rpc_handler::eth_call(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2 || !params[0].is_object())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	TransactionSkeleton ts = mcp::toTransactionSkeletonForEth(params[0]);
	ts.gasPrice = 0;
	ts.gas = mcp::block_max_gas;

	Transaction t(ts);
	t.setSignature(h256(0), h256(0), 0);

	uint64_t block_number = 0;
	std::string blockText = params[1];
	if (blockText == "latest" || blockText == "pending")
	{
		block_number = m_chain->last_stable_index();
	}
	else if (blockText == "earliest")
	{
		block_number = 0;
	}
	else
	{
		block_number = jsToInt(blockText);
	}

	dev::eth::McInfo mc_info;
	if (!try_get_mc_info(mc_info, block_number))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidBlock());
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = m_chain->execute(
		transaction,
		m_cache,
		t,
		mc_info,
		Permanence::Uncommitted,
		dev::eth::OnOpFunc());

	j_response["result"] = toJS(result.first.output);
}

void mcp::rpc_handler::net_version(mcp::json &j_response, bool &)
{
	j_response["result"] = toJS(mcp::chain_id);
}

void mcp::rpc_handler::net_listening(mcp::json &j_response, bool &)
{
	j_response["result"] = m_host->is_started();
}

void mcp::rpc_handler::net_peerCount(mcp::json &j_response, bool &)
{
	j_response["result"] = m_host->get_peers_count();
}

void mcp::rpc_handler::web3_clientVersion(mcp::json &j_response, bool &)
{
	j_response["result"] = STR(MCP_VERSION);
}

void mcp::rpc_handler::web3_sha3(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	dev::bytes msg = jsToBytes(params[0]);
	j_response["result"] = toJS(dev::sha3(msg));
}

void mcp::rpc_handler::eth_getCode(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	mcp::db::db_transaction transaction(m_store.create_transaction());
	chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

	try
	{
		j_response["result"] = toJS(c_state.code(jsToAddress(params[0])));
	}
	catch (...)
	{
	}
}

void mcp::rpc_handler::eth_getStorageAt(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2 ||
		!params[0].is_string() ||
		!params[1].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	try
	{
		dev::Address account = jsToAddress(params[0]);
		uint256_t position = jsToU256(params[1]);

		mcp::db::db_transaction transaction(m_store.create_transaction());
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
		j_response["result"] = toJS(c_state.storage(account, position));
	}
	catch (...)
	{
	}
}

void mcp::rpc_handler::eth_getTransactionByHash(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	try
	{
		h256 hash = jsToHash(params[0]);

		auto transaction = m_store.create_transaction();
		auto t = m_cache->transaction_get(transaction, hash);
		if (t == nullptr)
		{
			throw "";
		}
		auto lt = LocalisedTransaction(*t, mcp::block_hash(0), 0, 0);
		mcp::json j_transaction = toJson(lt);

		auto td = m_cache->transaction_address_get(transaction, hash);
		if (td != nullptr)
		{
			j_transaction["blockHash"] = td->blockHash.hexPrefixed();
			j_transaction["transactionIndex"] = toJS(td->index);

			uint64_t block_number = 0;
			if (!m_cache->block_number_get(transaction, td->blockHash, block_number))
			{
				j_transaction["blockNumber"] = toJS(block_number);
			}
		}

		j_response["result"] = j_transaction;
	}
	catch (...)
	{
	}
}

void mcp::rpc_handler::eth_getTransactionByBlockHashAndIndex(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	try
	{
		mcp::block_hash block_hash = jsToHash(params[0]);
		uint64_t index = jsToInt(params[1]);

		auto transaction = m_store.create_transaction();
		auto block(m_cache->block_get(transaction, block_hash));
		uint64_t block_number;
		if (block == nullptr ||
			m_cache->block_number_get(transaction, block_hash, block_number) ||
			index >= block->links().size())
		{
			throw "";
		}

		dev::h256 hash = block->links().at(index);
		auto t = m_cache->transaction_get(transaction, hash);
		if (t == nullptr)
		{
			throw "";
		}

		auto lt = LocalisedTransaction(*t, block->hash(), index, block_number);
		j_response["result"] = toJson(lt);
	}
	catch (...)
	{
	}
}

void mcp::rpc_handler::eth_getTransactionByBlockNumberAndIndex(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	uint64_t block_number = 0;
	std::string blockText = params[0];
	if (blockText == "latest" || blockText == "pending")
	{
		block_number = m_chain->last_stable_index();
	}
	else if (blockText == "earliest")
	{
		block_number = 0;
	}
	else
	{
		block_number = jsToInt(blockText);
	}
	uint64_t index = jsToInt(params[1]);

	try
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());
		mcp::block_hash block_hash;
		if (m_store.main_chain_get(transaction, block_number, block_hash))
		{
			throw "";
		}

		auto block(m_cache->block_get(transaction, block_hash));
		if (block == nullptr || index >= block->links().size())
		{
			throw "";
		}

		dev::h256 hash = block->links().at(index);
		auto t = m_cache->transaction_get(transaction, hash);
		if (t == nullptr)
		{
			throw "";
		}

		auto lt = LocalisedTransaction(*t, block_hash, index, block_number);
		j_response["result"] = toJson(lt);
	}
	catch (...)
	{
	}
}

void mcp::rpc_handler::eth_getTransactionReceipt(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}
	try
	{
		h256 hash = jsToHash(params[0]);

		auto transaction = m_store.create_transaction();
		auto t = m_cache->transaction_get(transaction, hash);
		auto tr = m_store.transaction_receipt_get(transaction, hash);
		auto td = m_cache->transaction_address_get(transaction, hash);

		if (t == nullptr || tr == nullptr || td == nullptr)
			throw "";

		uint64_t block_number = 0;
		if (m_cache->block_number_get(transaction, td->blockHash, block_number))
		{
			throw "";
		}

		auto lt = dev::eth::LocalisedTransactionReceipt(
			*tr,
			t->sha3(),
			td->blockHash,
			block_number,
			t->from(),
			t->to(),
			td->index,
			toAddress(t->from(), t->nonce()));

		j_response["result"] = toJson(lt);
	}
	catch (...)
	{
	}
}

void mcp::rpc_handler::eth_getBlockTransactionCountByHash(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	try
	{
		mcp::block_hash block_hash = jsToHash(params[0]);

		mcp::db::db_transaction transaction(m_store.create_transaction());
		auto block(m_cache->block_get(transaction, block_hash));
		if (block == nullptr)
		{
			throw "";
		}

		j_response["result"] = toJS(block->links().size());
	}
	catch (...)
	{
		j_response["result"] = 0;
	}
}

void mcp::rpc_handler::eth_getBlockTransactionCountByNumber(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	uint64_t block_number = 0;
	std::string blockText = params[0];
	if (blockText == "latest" || blockText == "pending")
	{
		block_number = m_chain->last_stable_index();
	}
	else if (blockText == "earliest")
	{
		block_number = 0;
	}
	else
	{
		block_number = jsToInt(blockText);
	}

	try
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());
		mcp::block_hash block_hash;
		if (m_store.main_chain_get(transaction, block_number, block_hash))
		{
			throw "";
		}

		auto block(m_cache->block_get(transaction, block_hash));
		if (block == nullptr)
		{
			throw "";
		}

		j_response["result"] = toJS(block->links().size());
	}
	catch (...)
	{
		j_response["result"] = 0;
	}
}

void mcp::rpc_handler::eth_getBalance(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}
	try
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
		j_response["result"] = toJS(c_state.balance(jsToAddress(params[0])));
	}
	catch (...)
	{
		j_response["result"] = 0;
	}
}

void mcp::rpc_handler::eth_accounts(mcp::json &j_response, bool &)
{
	mcp::json j_accounts = mcp::json::array();
	std::list<dev::Address> account_list(m_key_manager->list());
	for (auto account : account_list)
	{
		j_accounts.push_back(account.hexPrefixed());
	}
	j_response["result"] = j_accounts;
}

void mcp::rpc_handler::eth_sign(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	if (!mcp::isAddress(params[0]))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidAccount());
	}

	dev::Address account = jsToAddress(params[0]);
	if (!m_key_manager->exists(account))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidAccount());
	}

	dev::bytes data = jsToBytes(params[1]);
	if (data.empty() || data.size() > mcp::max_data_size)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidData());
	}

	mcp::Secret prv;
	if (!m_key_manager->find_unlocked_prv(account, prv))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_LockedAccount());
	}

	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Signature signature = dev::sign(prv, hash);
	j_response["result"] = signature.hexPrefixed();
}

void mcp::rpc_handler::eth_signTransaction(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_object())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	TransactionSkeleton ts = mcp::toTransactionSkeletonForEth(params[0]);
	if (!m_key_manager->exists(ts.from))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidAccount());
	}

	dev::Secret prv;
	if (!m_key_manager->find_unlocked_prv(ts.from, prv))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_LockedAccount());
	}

	m_wallet->populateTransactionWithDefaults(ts);

	Transaction t(ts, prv);
	mcp::json result;

	RLPStream s;
	t.streamRLP(s);
	result["raw"] = toJS(s.out());
	result["tx"] = toJson(t);

	j_response["result"] = result;
}

void mcp::rpc_handler::eth_protocolVersion(mcp::json &j_response, bool &)
{
	j_response["result"] = STR(MCP_VERSION);
}

void mcp::rpc_handler::eth_syncing(mcp::json &j_response, bool &)
{
	uint64_t last_stable_mci(m_chain->last_stable_mci());
	uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());

	mcp::json result;
	result["startingBlock"] = toJS(last_stable_mci);
	result["currentBlock"] = toJS(last_mci);
	result["highestBlock"] = toJS(last_stable_index);

	j_response["result"] = result;
}

void mcp::rpc_handler::eth_getLogs(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1 || !params[0].is_object())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}
	params = params[0];

	try
	{
		uint64_t fromBlock = 0;
		if (params.count("fromBlock"))
		{
			std::string blockText = params["fromBlock"];
			if (blockText == "latest" || blockText == "pending")
			{
				fromBlock = m_chain->last_stable_index();
			}
			else if (blockText == "earliest")
			{
				fromBlock = 0;
			}
			else
			{
				fromBlock = jsToInt(blockText);
			}
		}

		uint64_t toBlock = 0;
		if (params.count("toBlock"))
		{
			std::string blockText = params["toBlock"];
			if (blockText == "latest" || blockText == "pending")
			{
				toBlock = m_chain->last_stable_index();
			}
			else if (blockText == "earliest")
			{
				toBlock = 0;
			}
			else
			{
				toBlock = jsToInt(blockText);
			}
		}

		mcp::block_hash block_hash(0);
		if (params.count("blockhash"))
		{
			block_hash = jsToHash(params["blockhash"]);
		}

		std::unordered_set<dev::Address> search_address;
		if (params.count("address") && !params["address"].is_null())
		{
			if (params["address"].is_string())
			{
				search_address.insert(jsToAddress(params["address"]));
			}
			else if (params["address"].is_array())
			{
				std::vector<std::string> address_l = params["address"];
				for (std::string const &address_text : address_l)
				{
					search_address.insert(jsToAddress(address_text));
				}
			}
		}

		std::unordered_set<dev::h256> search_topics;
		if (params.count("topics") && !params["topics"].is_null())
		{
			if (!params["topics"].is_array())
			{
				throw "";
			}

			std::vector<std::string> topics_l = params["topics"];
			for (std::string const &topic_text : topics_l)
			{
				search_topics.insert(jsToHash(topic_text));
			}
		}

		int logi = 0;
		mcp::json logs_l = mcp::json::array();
		mcp::db::db_transaction transaction(m_store.create_transaction());

		if (block_hash != mcp::block_hash(0))
		{
			auto state = m_cache->block_state_get(transaction, block_hash);
			if (state == nullptr)
			{
				throw "";
			}
			fromBlock = toBlock = state->stable_index;
		}

		for (uint64_t i(fromBlock); i <= toBlock; i++)
		{
			if (m_store.stable_block_get(transaction, i, block_hash))
			{
				continue;
			}

			auto block_state = m_cache->block_state_get(transaction, block_hash);
			if (!block_state || !block_state->is_stable)
			{
				continue;
			}

			auto block = m_cache->block_get(transaction, block_hash);
			for (auto &th : block->links())
			{
				auto t = m_cache->transaction_get(transaction, th);
				auto tr = m_store.transaction_receipt_get(transaction, th);
				auto td = m_cache->transaction_address_get(transaction, th);
				if (t == nullptr || tr == nullptr || td == nullptr)
					continue;

				auto lt = dev::eth::LocalisedTransactionReceipt(
					*tr,
					t->sha3(),
					block_hash,
					i,
					t->from(),
					t->to(),
					td->index,
					toAddress(t->from(), t->nonce()),
					search_address,
					search_topics);

				mcp::json logs = toJson(lt.localisedLogs());
				if (logs.size() > 0)
				{
					logs_l.push_back(logs);
				}
			}
		}

		j_response["result"] = logs_l;
	}
	catch (...)
	{
		j_response["result"] = mcp::json::array();
	}
}

void mcp::rpc_handler::personal_importRawKey(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2 || !params[0].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	dev::Secret prv;
	try
	{
		prv = dev::Secret(params[0].get<std::string>());
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	std::string password = params[1];
	if (password.empty() ||
		!mcp::validatePasswordSize(password) ||
		!mcp::validatePassword(password))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidPassword());
	}

	mcp::key_content kc = m_key_manager->importRawKey(prv, password);
	j_response["result"] = kc.account.hexPrefixed();
}

void mcp::rpc_handler::personal_listAccounts(mcp::json &j_response, bool &)
{
	mcp::json j_accounts = mcp::json::array();
	std::list<dev::Address> account_list(m_key_manager->list());
	for (auto account : account_list)
	{
		j_accounts.push_back(account.hexPrefixed());
	}
	j_response["result"] = j_accounts;
}

void mcp::rpc_handler::personal_lockAccount(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];

	j_response["result"] = false;

	if (mcp::isAddress(params[0]))
	{
		dev::Address account = jsToAddress(params[0]);
		if (m_key_manager->exists(account))
		{
			m_key_manager->lock(account);
			j_response["result"] = true;
		}
	}
}

void mcp::rpc_handler::personal_newAccount(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 1)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	std::string password = params[0];
	if (password.empty() ||
		!mcp::validatePasswordSize(password) ||
		!mcp::validatePassword(password))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidPassword());
	}

	dev::Address account = m_key_manager->create(password, false, true);
	j_response["result"] = account.hexPrefixed();
}

void mcp::rpc_handler::personal_unlockAccount(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	j_response["result"] = false;

	if (mcp::isAddress(params[0]))
	{
		dev::Address account = jsToAddress(params[0]);
		if (m_key_manager->exists(account) ||
			!m_key_manager->unlock(account, params[1]))
		{
			j_response["result"] = true;
		}
	}
}

void mcp::rpc_handler::personal_sendTransaction(mcp::json &j_response, bool &async)
{
	mcp::json params = request["params"];
	if (params.size() < 2 || !params[0].is_object() || !params[1].is_string())
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	TransactionSkeleton t = mcp::toTransactionSkeletonForEth(params[0]);
	std::string password = params[1];

	auto rpc_l(shared_from_this());
	auto fun = [rpc_l, j_response, this](h256 &h, boost::optional<dev::Exception const &> e)
	{
		mcp::json j_resp = j_response;
		if (!e)
		{
			j_resp["result"] = toJS(h);
		}
		else
		{
			toRpcExceptionEthJson(*e, j_resp);
		}
		response(j_resp);
	};

	async = true;
	m_wallet->send_async(t, fun, password);
}

void mcp::rpc_handler::personal_sign(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 3)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	dev::bytes data = jsToBytes(params[0]);
	if (data.empty() || data.size() > mcp::max_data_size)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidData());
	}

	if (!mcp::isAddress(params[1]))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidAccount());
	}

	dev::Address account = jsToAddress(params[1]);
	if (!m_key_manager->exists(account))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidAccount());
	}

	mcp::Secret prv;
	if (m_key_manager->decrypt_prv(account, params[2], prv))
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidPassword());
	}

	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Signature signature = dev::sign(prv, hash);
	j_response["result"] = signature.hexPrefixed();
}

void mcp::rpc_handler::personal_ecRecover(mcp::json &j_response, bool &)
{
	mcp::json params = request["params"];
	if (params.size() < 2)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidParams());
	}

	dev::bytes data = jsToBytes(params[0]);
	if (data.empty() || data.size() > mcp::max_data_size)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidData());
	}

	dev::Signature sig(0);
	try
	{
		sig = jsToSignature(params[1]);
	}
	catch (...)
	{
		BOOST_THROW_EXCEPTION(RPC_Error_Eth_InvalidSignature());
	}

	dev::h256 hash;
	get_eth_signed_msg(data, hash);

	dev::Address from = dev::toAddress(dev::recover(sig, hash));
	j_response["result"] = from.hexPrefixed();
}

void mcp::rpc_handler::get_eth_signed_msg(dev::bytes &data, dev::h256 &hash)
{
	dev::bytes msg;
	std::string prefix = "Ethereum Signed Message:\n" + std::to_string(data.size());
	msg.resize(prefix.size() + data.size() + 1);

	msg[0] = 0x19;
	dev::bytesRef((unsigned char *)prefix.data(), prefix.size()).copyTo(dev::bytesRef(msg.data() + 1, prefix.size()));
	dev::bytesRef(data.data(), data.size()).copyTo(dev::bytesRef(msg.data() + prefix.size() + 1, data.size()));

	hash = dev::sha3(msg);
}