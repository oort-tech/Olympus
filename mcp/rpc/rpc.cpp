#include "rpc.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

#include <mcp/node/composer.hpp>
#include <mcp/common/pwd.hpp>
#include <mcp/core/genesis.hpp>
#include <mcp/node/evm/Executive.hpp>
#include <libdevcore/CommonJS.h>

mcp::rpc_config::rpc_config() :
	rpc_config(false)
{
}

mcp::rpc_config::rpc_config(bool enable_control_a) :
	address(boost::asio::ip::address_v4::loopback()),
	port(8765),
	enable_control(enable_control_a),
    rpc_enable(false)
{
}

void mcp::rpc_config::serialize_json(mcp::json & json_a) const
{
    json_a["rpc"] = rpc_enable ? "true" : "false";
    json_a["rpc_addr"] =  address.to_string();
    json_a["rpc_port"] = port;
    json_a["rpc_control"] =  enable_control ? "true": "false";
}

bool mcp::rpc_config::deserialize_json(mcp::json const & json_a)
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

bool mcp::rpc_config::parse_old_version_data(mcp::json const & json_a, uint64_t const& version)
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
				mcp::json j_rpc_l = json_a["rpc"].get<mcp::json>();;

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
				mcp::json j_rpc_l = json_a["rpc"].get<mcp::json>();;
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

mcp::rpc::rpc(mcp::block_store & store_a, std::shared_ptr<mcp::chain> chain_a,
	std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
	std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
	std::shared_ptr<mcp::async_task> background_a,
	boost::asio::io_service & service_a, mcp::rpc_config const& config_a) :
	m_store(store_a),
	m_chain(chain_a),
	m_cache(cache_a),
	m_key_manager(key_manager_a),
	m_wallet(wallet_a),
	m_host(host_a),
	m_background(background_a),
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
	acceptor.async_accept(connection->socket, [this, connection](boost::system::error_code const & ec) {
		if (!ec)
		{
			accept();
			connection->parse_connection();
		}
		else
		{
            LOG(this->m_log.error) << "Error accepting HTTP RPC connections:" << ec.message();
		}
	});
}

void mcp::rpc::stop()
{
	acceptor.close();
}

mcp::rpc_handler::rpc_handler(mcp::rpc &rpc_a, std::string const & body_a, std::function<void(mcp::json const &)>const & response_a, int m_cap):
body(body_a),
rpc(rpc_a),
response(response_a),
m_chain(rpc_a.m_chain),
m_cache(rpc_a.m_cache),
m_key_manager(rpc_a.m_key_manager),
m_wallet(rpc_a.m_wallet),
m_host(rpc_a.m_host),
m_background(rpc_a.m_background),
m_store(rpc.m_store)
{
}

void mcp::error_response(std::function<void(mcp::json const &)> response_a, std::string const & message_a)
{
    mcp::json j_response;
    j_response["msg"] = message_a;
    response_a(j_response);
}

void mcp::error_response(std::function<void(mcp::json const &)> response_a, int const &  error_code, std::string const& message_a, mcp::json& json_a)
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

void mcp::error_response(std::function<void(mcp::json const &)> response_a, int const &  error_code, std::string const & message_a)
{
    mcp::json j_response;
    mcp::error_response(response_a, error_code, message_a, j_response);
}

namespace
{
	bool decode_unsigned(std::string const & text, uint64_t & number)
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

bool mcp::rpc_handler::try_get_bool_from_json(std::string const& field_name_a, bool & value_a)
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

bool mcp::rpc_handler::try_get_uint64_t_from_json(std::string const& field_name_a, uint64_t & value_a)
{
    bool nret(true);
    auto it = request.find(field_name_a);
    if (it != request.end())
    {
        if (it->is_number_unsigned())
        {
            value_a = it->get<int64_t>();
        }
        else if(it->is_string())
        {
            try
            {
                value_a = std::stoull(it->get<std::string>());
            }
            catch (const std::exception&)
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

bool mcp::rpc_handler::try_get_mc_info(dev::eth::McInfo & mc_info_a)
{
	std::string mci_str = "latest";
	if (request.count("mci"))
	{
		if (!request["mci"].is_string())
			return false;

		mci_str = request["mci"];
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
		std::shared_ptr<mcp::block_state> last_summary_state(m_cache->block_state_get(transaction, mc_block->hashables->last_summary_block));
		assert_x(last_summary_state);
		assert_x(last_summary_state->is_stable);
		assert_x(last_summary_state->is_on_main_chain);
		assert_x(last_summary_state->main_chain_index);
		last_summary_mci = *last_summary_state->main_chain_index;
	}

	mc_info_a = dev::eth::McInfo(*mc_state->main_chain_index, mc_state->mc_timestamp, last_summary_mci);

	return true;
}

void mcp::rpc_handler::account_list()
{
    mcp::rpc_account_list_error_code error_code_l;

	mcp::json j_response;
    mcp::json j_accounts = mcp::json::array();

	std::list<mcp::public_key> pubkey_list(m_key_manager->list());
    for (auto pubkey : pubkey_list)
    {
		mcp::account account(pubkey);
        j_accounts.push_back(account.to_account());
    }
    j_response["accounts"] = j_accounts;
    error_code_l = mcp::rpc_account_list_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
}

void mcp::rpc_handler::account_validate()
{
    mcp::rpc_account_validate_error_code error_code_l;
    if (request.count("account") && request["account"].is_string())
    {
        std::string account_text = request["account"];
        mcp::account account;
        bool error = account.decode_account(account_text);

        mcp::json j_response;
        j_response["valid"] = error ? 0 : 1;

        error_code_l = mcp::rpc_account_validate_error_code::ok;
        error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
    }
    else
    {
        error_code_l = mcp::rpc_account_validate_error_code::invalid_account;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
    }
}

void mcp::rpc_handler::account_create()
{
    mcp::rpc_account_create_error_code error_code_l;
	if (rpc.config.enable_control)
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
								error_response(response, int(error_code_l), err.msg(error_code_l));
								return;
							}
						}
						mcp::public_key public_key = m_key_manager->create(password, gen_next_work_l, backup_l);
                        mcp::account new_account(public_key);
                        mcp::json j_response;
                        j_response["account"] = new_account.to_account();
						// added by michael at 1/18
						j_response["public_key"] = public_key.to_string();
						//
                        error_code_l = mcp::rpc_account_create_error_code::ok;
                        error_response(response, int(error_code_l), err.msg(error_code_l), j_response); 
                    }
                    else
                    {
                        error_code_l = mcp::rpc_account_create_error_code::invalid_characters_password;
                        error_response(response, int(error_code_l), err.msg(error_code_l));
                    }

                }
                else
                {
                    error_code_l = mcp::rpc_account_create_error_code::invalid_length_password;
                    error_response(response, int(error_code_l), err.msg(error_code_l));
                }

            }
            else
            {
                error_code_l = mcp::rpc_account_create_error_code::empty_password;
                error_response(response, int(error_code_l), err.msg(error_code_l));
            }
        }
        else
        {
            error_code_l = mcp::rpc_account_create_error_code::invalid_password;
            error_response(response, int(error_code_l), err.msg(error_code_l));
            return;
        }
	}
	else
	{
		error_response(response, "HTTP RPC control is disabled");
	}
}

void mcp::rpc_handler::account_remove()
{
	if (rpc.config.enable_control)
	{
        mcp::rpc_account_remove_error_code error_code_l;
		mcp::account account;
        bool error(false);
        if (request.count("account") && request["account"].is_string())
        {
            std::string account_text = request["account"];
            error = account.decode_account(account_text);
        }
		if (!error)
		{
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
                        error_response(response, int(error_code_l), err.msg(error_code_l));
                    }
                    else
                    {
                        error_code_l = mcp::rpc_account_remove_error_code::wrong_password;
                        error_response(response, int(error_code_l), err.msg(error_code_l));
                    }
                }
                else
                {
                    error_code_l = mcp::rpc_account_remove_error_code::invalid_password;
                    error_response(response, int(error_code_l), err.msg(error_code_l));
                    return;
                }
			}
			else
			{
                error_code_l = mcp::rpc_account_remove_error_code::account_not_exisit;
                error_response(response, int(error_code_l), err.msg(error_code_l));
			}
		}
		else
		{
            error_code_l = mcp::rpc_account_remove_error_code::invalid_account;
            error_response(response, int(error_code_l), err.msg(error_code_l));
		}
	}
	else
	{
		error_response(response, "HTTP RPC control is disabled");
	}
}

void mcp::rpc_handler::account_password_change()
{
	if (rpc.config.enable_control)
	{
        mcp::rpc_account_password_change_error_code error_code_l;		
        bool error(false);
        mcp::account account;
        if (request.count("account") && request["account"].is_string())
        {
            std::string account_text = request["account"];
            error = account.decode_account(account_text);
        }
        else
        {
            error = true;
        }
		if (!error)
		{
			auto exists(m_key_manager->exists(account));
			if (exists)
			{             
                if (!request.count("old_password") || (!request["old_password"].is_string()))
                {
                    error_code_l = mcp::rpc_account_password_change_error_code::invalid_old_password;
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
                    return;
                }
                std::string old_password_text = request["old_password"];

                if (!request.count("new_password") || (!request["new_password"].is_string()))
                {
                    error_code_l = mcp::rpc_account_password_change_error_code::invalid_new_password;
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
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
                            error_response(response, (int)error_code_l, err.msg(error_code_l));
                        }
                        else
                        {
                            error_code_l = mcp::rpc_account_password_change_error_code::wrong_password;
                            error_response(response, (int)error_code_l, err.msg(error_code_l));
                        }

                    }
                    else
                    {
                        error_code_l = mcp::rpc_account_password_change_error_code::invalid_characters_password;
                        error_response(response, (int)error_code_l, err.msg(error_code_l));
                    }
                }
                else
                {
                    error_code_l = mcp::rpc_account_password_change_error_code::invalid_length_password;
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
                }
			}
			else
			{
                error_code_l = mcp::rpc_account_password_change_error_code::account_not_exisit;
                error_response(response, (int)error_code_l, err.msg(error_code_l));
			}
		}
		else
		{
            error_code_l = mcp::rpc_account_password_change_error_code::invalid_account;
			error_response(response,(int)error_code_l, err.msg(error_code_l));
		}
	}
	else
	{
		error_response(response, "HTTP RPC control is disabled");
	}
}

void mcp::rpc_handler::account_unlock()
{
	if (rpc.config.enable_control)
	{
        mcp::rpc_account_unlock_error_code error_code_l;
        if (request.count("account") && request["account"].is_string())
		{
            std::string account_text = request["account"];
            mcp::account account;
            if (!account.decode_account(account_text))
            {
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
                            error_response(response, (int)error_code_l, err.msg(error_code_l));
                        }
                        else
                        {
                            error_code_l = mcp::rpc_account_unlock_error_code::wrong_password;
                            error_response(response, (int)error_code_l, err.msg(error_code_l));
                        }
                    }
                    else
                    {
                        error_code_l = mcp::rpc_account_unlock_error_code::invalid_password;
                        error_response(response, (int)error_code_l, err.msg(error_code_l));
                    }

                }
                else
                {
                    error_code_l = mcp::rpc_account_unlock_error_code::account_not_exisit;
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
                }
            }
            else
            {
                error_code_l = mcp::rpc_account_unlock_error_code::invalid_account;
                error_response(response, (int)error_code_l, err.msg(error_code_l));
            }

		}
		else
		{
            error_code_l = mcp::rpc_account_unlock_error_code::invalid_account;
			error_response(response, (int)error_code_l,err.msg(error_code_l));
		}
	}
	else
	{
		error_response(response, "HTTP RPC control is disabled");
	}
}

void mcp::rpc_handler::account_lock()
{
	if (rpc.config.enable_control)
	{
        mcp::rpc_account_lock_error_code error_code_l;
        if (request.count("account") && request["account"].is_string())
		{
            std::string account_text = request["account"];
            mcp::account account;
            if (!account.decode_account(account_text))
            {
                auto exists(m_key_manager->exists(account));
                if (exists)
                {
					m_key_manager->lock(account);
                    error_code_l = mcp::rpc_account_lock_error_code::ok;
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
                }
                else
                {
                    error_code_l = mcp::rpc_account_lock_error_code::account_not_exisit;
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
                }
            }
            else
            {
                error_code_l = mcp::rpc_account_lock_error_code::invalid_account;
                error_response(response, (int)error_code_l, err.msg(error_code_l));
            }
		}
		else
		{
            error_code_l = mcp::rpc_account_lock_error_code::invalid_account;
			error_response(response,(int)error_code_l,err.msg(error_code_l));
		}
	}
	else
	{
		error_response(response, "HTTP RPC control is disabled");
	}
}

void mcp::rpc_handler::account_export()
{
    mcp::rpc_account_export_error_code error_code_l;
    if (request.count("account") && request["account"].is_string())
    {
        std::string account_text = request["account"];
        mcp::account account;
        if (!account.decode_account(account_text))
        {
            mcp::key_content kc;
            auto exists(m_key_manager->find(account, kc));
            if (exists)
            {
                mcp::json j_response;
                std::string const & json(kc.to_json());
                j_response["json"] = json;

                error_code_l = mcp::rpc_account_export_error_code::ok;
                error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
            }
            else
            {
                error_code_l = mcp::rpc_account_export_error_code::account_not_exisit;
                error_response(response, (int)error_code_l, err.msg(error_code_l));
            }
        }
        else
        {
            error_code_l = mcp::rpc_account_export_error_code::invalid_account;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
        }	
	}
	else
	{
        error_code_l = mcp::rpc_account_export_error_code::invalid_account;
		error_response(response, (int)error_code_l,err.msg(error_code_l));
	}
}

void mcp::rpc_handler::account_import()
{
	if (rpc.config.enable_control)
	{
        mcp::rpc_account_import_error_code error_code_l;

        if (request.count("json") && request["json"].is_string())
        {
            std::string json_text = request["json"];
            bool gen_next_work_l(false);

            mcp::key_content kc;
            auto error(m_key_manager->import(json_text, kc, gen_next_work_l));
            if (!error)
            {
                mcp::json j_response;
                j_response["account"] = kc.account.to_account();
				// added by michael at 1/18
				j_response["public_key"] = kc.public_key.to_string();
				//
                error_code_l = mcp::rpc_account_import_error_code::ok;
                error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
            }
            else
            {
                error_code_l = mcp::rpc_account_import_error_code::invalid_json;
                error_response(response, (int)error_code_l, err.msg(error_code_l));
            }
        }
        else
        {
            error_code_l = mcp::rpc_account_import_error_code::invalid_json;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
        }
	}
	else
	{
		error_response(response, "HTTP RPC control is disabled");
	}
}

void mcp::rpc_handler::account_code()
{
    mcp::rpc_account_code_error_code error_code_l;
    if (request.count("account") && request["account"].is_string())
    {
        std::string account_text = request["account"];
        mcp::account account;
        if (!account.decode_account(account_text))
        {
			
            mcp::db::db_transaction transaction(m_store.create_transaction()); 
            chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
		    auto code(c_state.code(account));

            mcp::json j_response;
            j_response["account_code"] = bytes_to_hex(code);

            error_code_l = mcp::rpc_account_code_error_code::ok;
            error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
        }
        else
        {
            error_code_l = mcp::rpc_account_code_error_code::invalid_account;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
        }
	}
	else
	{
        error_code_l = mcp::rpc_account_code_error_code::invalid_account;
		error_response(response, (int)error_code_l,err.msg(error_code_l));
	}
}

void mcp::rpc_handler::account_balance()
{
    mcp::rpc_account_balance_error_code error_code_l;
    if (request.count("account") && request["account"].is_string())
    {
        std::string account_text = request["account"];
        mcp::account account;
        if (!account.decode_account(account_text))
        {
            mcp::db::db_transaction transaction(m_store.create_transaction()); 
			chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
            auto balance(c_state.balance(account));

            mcp::json j_response;
            j_response["balance"] = balance.convert_to<std::string>();

            error_code_l = mcp::rpc_account_balance_error_code::ok;
            error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
        }
        else
        {
            error_code_l = mcp::rpc_account_balance_error_code::invalid_account;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
        }
	}
	else
	{
        error_code_l = mcp::rpc_account_balance_error_code::invalid_account;
		error_response(response, (int)error_code_l,err.msg(error_code_l));
	}
}

void mcp::rpc_handler::accounts_balances()
{
    mcp::rpc_accounts_balances_error_code error_code_l;

	mcp::json j_response;
    mcp::json j_balances = mcp::json::array();
    if (request.count("accounts") == 0)
    {
        error_code_l = mcp::rpc_accounts_balances_error_code::invalid_account;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

	for (mcp::json const & j_account : request["accounts"])
	{
		mcp::account account;
        std::string account_text = j_account;
		auto error(account.decode_account(account_text));
        if (!error)
		{
			mcp::db::db_transaction transaction(m_store.create_transaction()); 
			chain_state c_state(transaction, 0, m_store, m_chain, m_cache);
			auto balance(c_state.balance(account));
			j_balances.push_back(balance.convert_to<std::string>());
		}
		else
		{
            error_code_l = mcp::rpc_accounts_balances_error_code::invalid_account;
			error_response(response, (int)error_code_l,err.msg(error_code_l)+ ":" + account_text);
            return;
		}
	}
    j_response["balances"] = j_balances;
    error_code_l = mcp::rpc_accounts_balances_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
}

void mcp::rpc_handler::account_block_list()
{
	mcp::rpc_account_block_list_error_code error_code_l;

    mcp::account account;
    if (!request.count("account") || (!request["account"].is_string()))
    {
        error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

    std::string account_text = request["account"];
    if (account.decode_account(account_text))
    {
        error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

	uint64_t limit_l(0);
    if (!request.count("limit"))
    {
        error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

    if (!try_get_uint64_t_from_json("limit", limit_l))
    {
        error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

    if (limit_l > list_max_limit)
    {
		error_code_l = mcp::rpc_account_block_list_error_code::limit_too_large;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
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
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
                    return;
                }
                std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, last_state_hash));
                if (!(acc_state && acc_state->account == account))
                {
                    error_code_l = mcp::rpc_account_block_list_error_code::index_not_exsist;
                    error_response(response, (int)error_code_l, err.msg(error_code_l));
                    return;
                }
                search_hash = last_state_hash;

            }

        }
        else
        {
            error_code_l = mcp::rpc_account_block_list_error_code::invalid_index;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
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
    error_response(response, (int)error_code_l, err.msg(error_code_l), resp_l);
}

void mcp::rpc_handler::account_state_list()
{
	mcp::rpc_account_block_list_error_code error_code_l;

	mcp::account account;
	if (!request.count("account") || (!request["account"].is_string()))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string account_text = request["account"];
	if (account.decode_account(account_text))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_account;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	uint64_t limit_l(0);
	if (!request.count("limit"))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (!try_get_uint64_t_from_json("limit", limit_l))
	{
		error_code_l = mcp::rpc_account_block_list_error_code::invalid_limit;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if (limit_l > list_max_limit)
	{
		error_code_l = mcp::rpc_account_block_list_error_code::limit_too_large;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
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
					error_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, last_state_hash));
				if (!(acc_state && acc_state->account == account))
				{
					error_code_l = mcp::rpc_account_block_list_error_code::index_not_exsist;
					error_response(response, (int)error_code_l, err.msg(error_code_l));
					return;
				}
				search_hash = last_state_hash;

			}

		}
		else
		{
			error_code_l = mcp::rpc_account_block_list_error_code::invalid_index;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
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
		acc_state_l["account"] = acc_state->account.to_account();
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
	error_response(response, (int)error_code_l, err.msg(error_code_l), resp_l);
}

void mcp::rpc_handler::block()
{
    mcp::rpc_block_error_code error_code_l;

    if (!request.count("hash") || (!request["hash"].is_string()))
    {
        error_code_l = mcp::rpc_block_error_code::invalid_hash;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

	std::string hash_text = request["hash"];
	mcp::uint256_union hash;
	auto error(hash.decode_hex(hash_text));
	if (!error)
	{
		mcp::json response_l;
		mcp::db::db_transaction transaction(m_store.create_transaction()); 
		auto block(m_cache->block_get(transaction, hash));
		if (block == nullptr)
		{
			auto unlink_block(m_cache->unlink_block_get(transaction, hash));
			if (unlink_block)
				block = unlink_block->block;
		}

		if (block != nullptr)
		{
			mcp::json block_l;
			block->serialize_json(block_l);
			response_l["block"] = block_l;
            error_code_l = mcp::rpc_block_error_code::ok;
            error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
		}
        else
        {
			response_l["block"] = nullptr;
            error_code_l = mcp::rpc_block_error_code::ok;
            error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
        }

	}
	else
	{
        error_code_l = mcp::rpc_block_error_code::invalid_hash;
		error_response(response,(int)error_code_l,err.msg(error_code_l));
	}
}

void mcp::rpc_handler::blocks()
{
	mcp::rpc_blocks_error_code error_code_l;

	std::vector<std::string> hashes;
	mcp::json response_l;
	mcp::json blocks_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	if (!request.count("hashes") || (!request["hashes"].is_array()))
	{
		error_code_l = mcp::rpc_blocks_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::vector<std::string> hashes_l = request["hashes"];
	for (std::string const & hash_text : hashes_l)
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
			error_response(response, (int)error_code_l, err.msg(error_code_l) + "," + hash_text);
			return;
		}
	}
	response_l["blocks"] = blocks_l;
	error_code_l = mcp::rpc_blocks_error_code::ok;
	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::block_state()
{
	mcp::rpc_block_error_code error_code_l;

	if (!request.count("hash") || (!request["hash"].is_string()))
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string hash_text = request["hash"];
	mcp::uint256_union hash;
	auto error(hash.decode_hex(hash_text));
	if (!error)
	{
		mcp::json response_l;
		mcp::db::db_transaction transaction(m_store.create_transaction());
		std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, hash));
		if (state != nullptr)
		{
			auto block(m_cache->block_get(transaction, hash));
			assert_x(block);

			mcp::json block_state_l;
			block_state_l["hash"] = hash.to_string();
			mcp::account contract_account;
			if (block->hashables->type == mcp::block_type::light 
				&& block->isCreation() 
				&& state->is_stable 
				&& (state->status == mcp::block_status::ok))
			{
				std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, state->receipt->from_state));
				assert_x(acc_state);
				contract_account = toAddress(block->hashables->from, acc_state->nonce() - 1);
			}

			state->serialize_json(block_state_l, contract_account);

			response_l["block_state"] = block_state_l;
			error_code_l = mcp::rpc_block_error_code::ok;
			error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
		}
		else
		{
			response_l["block_state"] = nullptr;
			error_code_l = mcp::rpc_block_error_code::ok;
			error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
		}
	}
	else
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
	}
}

void mcp::rpc_handler::block_states()
{
	mcp::rpc_blocks_error_code error_code_l;

	std::vector<std::string> hashes;
	mcp::json response_l;
	mcp::json states_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	if (!request.count("hashes") || (!request["hashes"].is_array()))
	{
		error_code_l = mcp::rpc_blocks_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::vector<std::string> hashes_l = request["hashes"];
	for (std::string const & hash_text : hashes_l)
	{
		mcp::uint256_union hash;
		auto error(hash.decode_hex(hash_text));
		if (!error)
		{
			auto state(m_cache->block_state_get(transaction, hash));
			mcp::json state_l;
			if (state != nullptr)
			{
				auto block(m_cache->block_get(transaction, hash));
				assert_x(block);

				state_l["hash"] = hash.to_string();
				mcp::account contract_address;
				if (block->hashables->type == mcp::block_type::light
					&& block->isCreation()
					&& state->is_stable
					&& (state->status == mcp::block_status::ok))
				{
					std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, state->receipt->from_state));
					assert_x(acc_state);
					contract_address = toAddress(block->hashables->from, acc_state->nonce() - 1);
				}
				state->serialize_json(state_l, contract_address);
			}
			else
				state_l = nullptr;

			states_l.push_back(state_l);
		}
		else
		{
			error_code_l = mcp::rpc_blocks_error_code::invalid_hash;
			error_response(response, (int)error_code_l, err.msg(error_code_l) + "," + hash_text);
			return;
		}
	}
	response_l["block_states"] = states_l;
	error_code_l = mcp::rpc_blocks_error_code::ok;
	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::block_traces()
{
	mcp::rpc_block_error_code error_code_l;

	if (!request.count("hash") || (!request["hash"].is_string()))
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
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
			
			uint32_t const & depth(trace->depth);

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
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);

	}
	else
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
	}
}

void mcp::rpc_handler::stable_blocks()
{
	bool error(false);
	mcp::rpc_stable_blocks_error_code error_code_l;

	uint64_t index(0);
    if (request.count("index"))
    {
		if (!try_get_uint64_t_from_json("index", index))
		{
			error_code_l = mcp::rpc_stable_blocks_error_code::invalid_index;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
    }

	uint64_t limit_l(0);
    if (!request.count("limit"))
    {
        error_code_l = mcp::rpc_stable_blocks_error_code::invalid_limit;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
    if (!try_get_uint64_t_from_json("limit", limit_l))
    {
        error_code_l = mcp::rpc_stable_blocks_error_code::invalid_limit;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

	if (limit_l > list_max_limit)
	{
		error_code_l = mcp::rpc_stable_blocks_error_code::limit_too_large;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	uint64_t last_stable_index(m_chain->last_stable_index());

	mcp::json response_l;
	mcp::json block_list_l = mcp::json::array();
	int blocks_count(0);
	for(uint64_t stable_index = index; stable_index <= last_stable_index; stable_index++)
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
	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::estimate_gas()
{
    mcp::rpc_estimate_gas_error_code error_code_l;
    if (!rpc.config.enable_control)
    {
        error_response(response, "RPC control is disabled");
        return;
    }

	// sichaoy: remove the "from" field, and to pick it by default
	mcp::account from(0);
	if (request.count("from"))
	{
		if (!request["from"].is_string())
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_from;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}

		std::string from_text = request["from"];
		bool error(from.decode_account(from_text));
		if (error)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_from;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	mcp::account to(0);
	if (request.count("to"))
	{
		if (!request["to"].is_string())
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_to;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}

		std::string to_text = request["to"];
		bool error = to.decode_account(to_text);
		if (error)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_to;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	mcp::amount amount(0);
    if (request.count("amount"))
    {
		if (!request["amount"].is_string())
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_amount;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		std::string amount_text = request["amount"];
		bool error = !boost::conversion::try_lexical_convert(amount_text, amount);
		if (error)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_amount;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
    }

	uint64_t gas(0);
	if (request.count("gas"))
	{
		if (!try_get_uint64_t_from_json("gas", gas))
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_gas;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	uint256_t gas_price(0);
	if (request.count("gas_price"))
	{
		uint256_t gas_price;
		if (!request["gas_price"].is_string()
			|| !boost::conversion::try_lexical_convert(request["gas_price"].get<std::string>(), gas_price))
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_gas_price;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	dev::bytes data;
	if (request.count("data"))
	{
		std::string data_text = request["data"];
        bool error = mcp::hex_to_bytes(data_text, data);
        if (error)
        {
            error_code_l = mcp::rpc_estimate_gas_error_code::invalid_data;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
            return;
        }

		if (data.size() > mcp::max_data_size)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::data_size_too_large;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	dev::eth::McInfo mc_info;
	if (!try_get_mc_info(mc_info))
	{
		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_mci;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    mcp::db::db_transaction transaction(m_store.create_transaction());
    std::pair<u256, bool> result = m_chain->estimate_gas(transaction, m_cache, from, amount, to, data, gas, gas_price, mc_info);

    mcp::json response_l;
	if (!result.second)
	{
		error_code_l = mcp::rpc_estimate_gas_error_code::gas_not_enough_or_fail;
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
		return;
	}

    response_l["gas"] = result.first;
    error_code_l = mcp::rpc_estimate_gas_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::call()
{
    mcp::rpc_call_error_code error_code_l;
    if (!rpc.config.enable_control)
    {
        error_response(response, "RPC control is disabled");
        return;
    }

	mcp::account from(0);
	if (request.count("from"))
	{
		if (!request["from"].is_string())
		{
			error_code_l = mcp::rpc_call_error_code::invalid_account_from;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}

		// sichaoy: remove the "from" field, and to pick it by default
		std::string from_text = request["from"];
		auto error(from.decode_account(from_text));
		if (error)
		{
			error_code_l = mcp::rpc_call_error_code::invalid_account_from;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

    if (!request.count("to") || (!request["to"].is_string()) )
    {
        error_code_l = mcp::rpc_call_error_code::invalid_account_to;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

	std::string to_text = request["to"];
	mcp::account to;
	bool error = to.decode_account(to_text);
	if (error)
	{
        error_code_l = mcp::rpc_call_error_code::invalid_account_to;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    dev::bytes data;
	if (request.count("data"))
	{
		std::string data_text = request["data"];
        error = mcp::hex_to_bytes(data_text, data);
        if (error)
        {
            error_code_l = mcp::rpc_call_error_code::invalid_data;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
            return;
        }

		if (data.size() > mcp::max_data_size)
		{
			error_code_l = mcp::rpc_call_error_code::data_size_too_large;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
    }	

	dev::eth::McInfo mc_info;
	if (!try_get_mc_info(mc_info))
	{
		error_code_l = mcp::rpc_call_error_code::invalid_mci;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    std::shared_ptr<mcp::block> block = std::make_shared<mcp::block>(
        mcp::block_type::light,             //mcp::block_type type_a,
        from,                               // mcp::account const & from_a, 
        to,                                 // mcp::account const & to_a, 
        0,                                  // mcp::amount const & amount_a,
	    0,                                  // mcp::block_hash const & previous_a, 
        std::vector<mcp::block_hash>{},     // std::vector<mcp::block_hash> const & parents_a,
        std::make_shared<std::list<mcp::block_hash>>(std::list<mcp::block_hash>{}), // std::shared_ptr<std::list<mcp::block_hash>> links_a,
	    0,                                  // mcp::summary_hash const & last_summary_a, 
        0,                                  // mcp::block_hash const & last_summary_block_a, 
        0,                                  // mcp::block_hash const & last_stable_block_a, 
		mcp::uint256_t(mcp::block_max_gas),					// uint256_t gas_a,
        0,                                  // uint256_t gas_price_a,
		mcp::block::data_hash(data),        // mcp::data_hash const & data_hash_a, 
        data,                               // std::vector<uint8_t> const & data_a, 
        0,                                  // uint64_t const & exec_timestamp_a, 
        mcp::uint64_union(0)                // mcp::uint64_union const & work_a
        );

	mcp::db::db_transaction transaction(m_store.create_transaction());
    std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> result = m_chain->execute(transaction, m_cache, block, mc_info, Permanence::Reverted, dev::eth::OnOpFunc());

    mcp::json response_l;
    response_l["output"] = bytes_to_hex(result.first.output);

    error_code_l = mcp::rpc_call_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::logs()
{
	mcp::rpc_logs_error_code error_code_l;

	uint64_t from_stable_block_index(0);
	if (request.count("from_stable_block_index") && !request["from_stable_block_index"].is_null())
	{
		if (!try_get_uint64_t_from_json("from_stable_block_index", from_stable_block_index))
		{
			error_code_l = mcp::rpc_logs_error_code::invalid_from_stable_block_index;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	uint64_t to_stable_block_index;
	if (request.count("to_stable_block_index") && !request["to_stable_block_index"].is_null())
	{
		if (!try_get_uint64_t_from_json("to_stable_block_index", to_stable_block_index))
		{
			error_code_l = mcp::rpc_logs_error_code::invalid_to_stable_block_index;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	else
	{
		to_stable_block_index = m_chain->last_stable_index();
	}

	boost::optional<mcp::account> search_account;
	if (request.count("account") && !request["account"].is_null())
	{
		if (!request["account"].is_string())
		{
			error_code_l = mcp::rpc_logs_error_code::invalid_account;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}

		std::string account_text = request["account"];
		mcp::account account;
		bool error = account.decode_account(account_text);
		if (error)
		{
			error_code_l = mcp::rpc_logs_error_code::invalid_account;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		search_account = account;
	}

	std::vector<dev::h256> search_topics;
	if (request.count("topics") && !request["topics"].is_null())
	{
		if (!request["topics"].is_array())
		{
			error_code_l = mcp::rpc_logs_error_code::invalid_topics;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}

		std::vector<std::string> topics_l = request["topics"];
		for (std::string const & topic_text : topics_l)
		{
			mcp::uint256_union topic;
			auto error(topic.decode_hex(topic_text));
			if (error)
			{
				error_code_l = mcp::rpc_logs_error_code::invalid_topics;
				error_response(response, (int)error_code_l, err.msg(error_code_l) + "," + topic_text);
				return;
			}

			search_topics.push_back(dev::h256(topic.ref()));
		}
	}

	mcp::json response_l;
	mcp::json logs_l = mcp::json::array();
	mcp::db::db_transaction transaction(m_store.create_transaction());

	for (uint64_t i(from_stable_block_index); i <= to_stable_block_index; i++)
	{
		mcp::block_hash block_hash;
		bool exists(!m_store.stable_block_get(transaction, i, block_hash));
		assert_x(exists);
		std::shared_ptr<mcp::block_state> block_state(m_cache->block_state_get(transaction, block_hash));
		assert_x(block_state);

		if (block_state->block_type != mcp::block_type::light)
			continue;

		if (!block_state->receipt)
			continue;

		if (search_account && !block_state->receipt->contains_bloom(dev::h256(search_account->ref())))
			continue;

		std::unordered_set<dev::h256> existed_topics;
		for (dev::h256 const & topic : search_topics)
		{
			if (block_state->receipt->contains_bloom(topic))
				existed_topics.insert(topic);
		}

		if (search_topics.size() > 0  && existed_topics.size() == 0)
			continue;

		for (auto const & log : block_state->receipt->log)
		{
			if (!search_account || log.acct == *search_account)
			{
				for (dev::h256 const & t : log.topics)
				{
					if (search_topics.size() == 0 || existed_topics.count(t))
					{
						mcp::json log_l;
						log.serialize_json(log_l);
						log_l["block_hash"] = block_hash.to_string();

						uint32_t log_id;
						blake2b_state hash_l;
						auto status(blake2b_init(&hash_l, sizeof(log_id)));
						assert_x(status == 0);
						blake2b_update(&hash_l, block_hash.bytes.data(), sizeof(block_hash.bytes));
						log.hash(hash_l);
						status = blake2b_final(&hash_l, &log_id, sizeof(log_id));
						assert_x(status == 0);
						std::stringstream stream;
						stream << std::hex << log_id;
						log_l["id"] = stream.str();

						logs_l.push_back(log_l);
						break;
					}
				}
			}
		}
	}

	response_l["logs"] = logs_l;
	error_code_l = mcp::rpc_logs_error_code::ok;
	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::send_block()
{
    mcp::rpc_send_error_code error_code_l;
    if (!rpc.config.enable_control)
    {
        error_response(response, "RPC control is disabled");
        return;
    }

	boost::optional<mcp::block_hash> previous_opt;
	if (request.count("previous"))
	{
		if (!request["previous"].is_string())
		{
			error_code_l = mcp::rpc_send_error_code::invalid_previous;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		std::string previous_text = request["previous"];
		mcp::block_hash previous;
		auto error(previous.decode_hex(previous_text));
		if (error)
		{
			error_code_l = mcp::rpc_send_error_code::invalid_previous;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		previous_opt = previous;
	}

    if (!request.count("from") || (!request["from"].is_string()))
    {
        error_code_l = mcp::rpc_send_error_code::invalid_account_from;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }

    // sichaoy: remove the "from" field, and to pick it by default
    std::string from_text = request["from"];
	mcp::account from;
	auto error(from.decode_account(from_text));
	if (error)
	{
        error_code_l = mcp::rpc_send_error_code::invalid_account_from;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	if(!m_key_manager->exists(from))
	{
        error_code_l = mcp::rpc_send_error_code::account_not_exisit;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("to") || (!request["to"].is_string()) )
    {
        error_code_l = mcp::rpc_send_error_code::invalid_account_to;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string to_text = request["to"];
	mcp::account to;
	error = to.decode_account(to_text);
	if (error)
	{
        error_code_l = mcp::rpc_send_error_code::invalid_account_to;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("amount") || (!request["amount"].is_string()))
    {
        error_code_l = mcp::rpc_send_error_code::invalid_amount;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string amount_text = request["amount"];
	mcp::amount amount;
	error = !boost::conversion::try_lexical_convert(amount_text, amount);
	if (error)
	{
        error_code_l = mcp::rpc_send_error_code::invalid_amount;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	uint64_t gas_u64;
	if (!try_get_uint64_t_from_json("gas", gas_u64))
	{
        error_code_l = mcp::rpc_send_error_code::invalid_gas;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	uint256_t gas(gas_u64);

	uint256_t gas_price;
	if (!request.count("gas_price")
		|| !request["gas_price"].is_string()
		|| !boost::conversion::try_lexical_convert(request["gas_price"].get<std::string>(), gas_price))
	{
		error_code_l = mcp::rpc_send_error_code::invalid_gas_price;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    dev::bytes data;
	if (request.count("data"))
	{
		std::string data_text = request["data"];
        error = mcp::hex_to_bytes(data_text, data);
        if (error)
        {
            error_code_l = mcp::rpc_send_error_code::invalid_data;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
            return;
        }
    }	
	if (data.size() > mcp::max_data_size)
	{
        error_code_l = mcp::rpc_send_error_code::data_size_too_large;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	boost::optional<std::string> password;
	if (request.count("password"))
	{
		if (!request["password"].is_string())
		{
			error_code_l = mcp::rpc_send_error_code::invalid_password;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		std::string password_str = request["password"];
		password = password_str;
	}

    bool gen_next_work_l(false);

	bool async(false);
	if (request.count("async"))
	{
		if (!try_get_bool_from_json("async", async))
		{
			error_code_l = mcp::rpc_send_error_code::invalid_async;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	auto rpc_l(shared_from_this());
	m_wallet->send_async(mcp::block_type::light, previous_opt, from, to, amount, gas, gas_price, data, password, [from, rpc_l, this](mcp::send_result result)
	{
		mcp::rpc_send_error_code error_code_l;
		switch (result.code)
		{
		case mcp::send_result_codes::ok:
		{
			error_code_l = mcp::rpc_send_error_code::ok;
			mcp::uint256_union hash(result.block->hash());
			mcp::json response_value_p;
			response_value_p["hash"] = hash.to_string();
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l), response_value_p);
			break;
		}
		case mcp::send_result_codes::from_not_exists:
			error_code_l = mcp::rpc_send_error_code::account_not_exisit;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::account_locked:
			error_code_l = mcp::rpc_send_error_code::account_locked;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::wrong_password:
			error_code_l = mcp::rpc_send_error_code::wrong_password;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::insufficient_balance:
			error_code_l = mcp::rpc_send_error_code::insufficient_balance;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::data_size_too_large:
			error_code_l = mcp::rpc_send_error_code::data_size_too_large;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::validate_error:
			error_code_l = mcp::rpc_send_error_code::validate_error;
			if (result.msg != "")
				error_response(response, (int)error_code_l, result.msg);
			else
				error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::error:
			error_code_l = mcp::rpc_send_error_code::send_block_error;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		default:
			error_code_l = mcp::rpc_send_error_code::send_unknown_error;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		}
	}, gen_next_work_l, async);
}

void mcp::rpc_handler::generate_offline_block()
{
	if (!rpc.config.enable_control)
	{
		error_response(response, "RPC control is disabled");
		return;
	}

	mcp::rpc_generate_offline_block_error_code error_code_l;

	boost::optional<mcp::block_hash> previous_opt;
	if (request.count("previous"))
	{
		if (!request["previous"].is_string())
		{
			error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_previous;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		std::string previous_text = request["previous"];
		mcp::block_hash previous;
		auto error(previous.decode_hex(previous_text));
		if (error)
		{
			error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_previous;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		previous_opt = previous;
	}

    if (!request.count("from") || (!request["from"].is_string()))
    {
        error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_from;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string from_text = request["from"];
	mcp::account from;
	auto error(from.decode_account(from_text));
	if (error)
	{
		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_from;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("to") || (!request["to"].is_string()))
    {
        error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_to;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string to_text = request["to"];
	mcp::account to;
	error = to.decode_account(to_text);
	if (error)
	{
		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_account_to;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("amount") || (!request["amount"].is_string()))
    {
        error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_amount;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string amount_text = request["amount"];
	mcp::amount amount;
	error = !boost::conversion::try_lexical_convert(amount_text, amount);
	if (error)
	{
		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_amount;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	uint64_t gas_u64;
	if (!try_get_uint64_t_from_json("gas", gas_u64))
	{
		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_gas;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	uint256_t gas(gas_u64);

	uint256_t gas_price;
	if (!request.count("gas_price") 
		|| !request["gas_price"].is_string()
		|| !boost::conversion::try_lexical_convert(request["gas_price"].get<std::string>(), gas_price))
	{
		error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_gas_price;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	dev::bytes data;
    if(request.count("data"))
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
			error_code_l = mcp::rpc_generate_offline_block_error_code::invalid_data;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	if (data.size() > mcp::max_data_size)
	{
		error_code_l = mcp::rpc_generate_offline_block_error_code::data_size_too_large;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	std::shared_ptr<mcp::block> block;
	mcp::compose_result_codes compose_block_result(m_wallet->composer->compose_block(transaction, mcp::block_type::light, previous_opt, from, to, amount, gas, gas_price, data, block));
	switch (compose_block_result)
	{
	case mcp::compose_result_codes::ok:
	{
		mcp::json response_l;

		response_l["hash"] = block->hash().to_string();
		response_l["from"] = block->hashables->from.to_account();
		response_l["to"] = block->hashables->to.to_account();
		response_l["amount"] = block->hashables->amount.str();
		response_l["previous"] = block->hashables->previous.to_string();
		response_l["gas"] = block->hashables->gas.str();
		response_l["gas_price"] = block->hashables->gas_price.str();
		response_l["data"] = mcp::bytes_to_hex(block->data);

		error_code_l = mcp::rpc_generate_offline_block_error_code::ok;
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
		break;
	}
	case mcp::compose_result_codes::insufficient_balance:
		error_code_l = mcp::rpc_generate_offline_block_error_code::insufficient_balance;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		break;
	case mcp::compose_result_codes::data_size_too_large:
		error_code_l = mcp::rpc_generate_offline_block_error_code::data_size_too_large;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		break;
	case mcp::compose_result_codes::validate_error:
		error_code_l = mcp::rpc_generate_offline_block_error_code::validate_error;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		break;
	case mcp::compose_result_codes::error:
		error_code_l = mcp::rpc_generate_offline_block_error_code::compose_error;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		break;
	default:
		error_code_l = mcp::rpc_generate_offline_block_error_code::compose_unknown_error;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		break;
	}
}

void mcp::rpc_handler::send_offline_block()
{
	if (!rpc.config.enable_control)
	{
		error_response(response, "RPC control is disabled");
		return;
	}
	mcp::rpc_send_offline_block_error_code error_code_l;

    if (!request.count("from") || (!request["from"].is_string()))
    {
        error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_from;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string from_text = request["from"];
	mcp::account from;
	auto error(from.decode_account(from_text));
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_from;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("to") || (!request["to"].is_string()))
    {
        error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_to;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string to_text = request["to"];
	mcp::account to;
	error = to.decode_account(to_text);
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_account_to;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("amount") || (!request["amount"].is_string()))
    {
        error_code_l = mcp::rpc_send_offline_block_error_code::invalid_amount;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string amount_text = request["amount"];
	mcp::amount amount;
	error = !boost::conversion::try_lexical_convert(amount_text, amount);
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_amount;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	uint64_t gas_u64;
	if (!try_get_uint64_t_from_json("gas", gas_u64))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_gas;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	uint256_t gas(gas_u64);

	uint256_t gas_price;
	if (!request.count("gas_price")
		|| !request["gas_price"].is_string()
		|| !boost::conversion::try_lexical_convert(request["gas_price"].get<std::string>(), gas_price))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_gas_price;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
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
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	if (data.size() > mcp::max_data_size)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::data_size_too_large;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("previous") || (!request["previous"].is_string()))
    {
        error_code_l = mcp::rpc_send_offline_block_error_code::invalid_previous;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string previous_text = request["previous"];
	mcp::block_hash previous;
	error = previous.decode_hex(previous_text);
	if (error)
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_previous;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("signature") || (!request["signature"].is_string()))
    {
        error_code_l = mcp::rpc_send_offline_block_error_code::invalid_signature;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string signature_text = request["signature"];
	mcp::signature signature;
	if (signature.decode_hex(signature_text))
	{
		error_code_l = mcp::rpc_send_offline_block_error_code::invalid_signature;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::vector<mcp::block_hash> parents;
	std::shared_ptr<std::list<mcp::block_hash>> links = std::make_shared<std::list<mcp::block_hash>>();
	mcp::block_hash last_summary(0);
	mcp::block_hash last_summary_block(0);
	mcp::block_hash last_stable_block(0);
	uint64_t exec_timestamp(0);
	mcp::uint64_union work(0);

    std::shared_ptr<mcp::block> p_block = std::make_shared<mcp::block>(mcp::block_type::light, from, to, amount, previous, parents, links,
        last_summary, last_summary_block, last_stable_block, gas, gas_price, mcp::block::data_hash(data), data, exec_timestamp, work);

	assert_x(p_block != nullptr);

	bool async(false);
	if (request.count("async"))
	{
		if (!try_get_bool_from_json("async", async))
		{
			error_code_l = mcp::rpc_send_offline_block_error_code::invalid_async;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	bool gen_next_work_l(false);
	auto rpc_l(shared_from_this());
	m_wallet->send_async(p_block, signature, [rpc_l, this](mcp::send_result result) {
		mcp::rpc_send_offline_block_error_code error_code_l;
		switch (result.code)
		{
		case mcp::send_result_codes::ok:
		{
			mcp::uint256_union hash(result.block->hash());
			mcp::json response_l;
			response_l["hash"] = hash.to_string();

			error_code_l = mcp::rpc_send_offline_block_error_code::ok;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l), response_l);
			break;
		}
		case mcp::send_result_codes::insufficient_balance:
			error_code_l = mcp::rpc_send_offline_block_error_code::insufficient_balance;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::data_size_too_large:
			error_code_l = mcp::rpc_send_offline_block_error_code::data_size_too_large;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::validate_error:
			error_code_l = mcp::rpc_send_offline_block_error_code::validate_error;
			if (result.msg != "")
				error_response(response, (int)error_code_l, result.msg);
			else
				error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		case mcp::send_result_codes::error:
			error_code_l = mcp::rpc_send_offline_block_error_code::send_block_error;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		default:
			error_code_l = mcp::rpc_send_offline_block_error_code::send_unknown_error;
			error_response(response, (int)error_code_l, rpc_l->err.msg(error_code_l));
			break;
		}
	}, gen_next_work_l, async);
}

void mcp::rpc_handler::block_summary()
{
	mcp::rpc_block_error_code error_code_l;

	if (!request.count("hash") || (!request["hash"].is_string()))
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string hash_text = request["hash"];
	mcp::uint256_union hash;
	auto error(hash.decode_hex(hash_text));
	if (!error)
	{
		mcp::json response_l;
		mcp::db::db_transaction transaction(m_store.create_transaction());
		mcp::summary_hash summary;
		bool exists(!m_cache->block_summary_get(transaction, hash, summary));
		if (!exists)
		{
			response_l["summary"] = nullptr;
		}
		else
		{
			response_l["summary"] = summary.to_string();

			auto block(m_cache->block_get(transaction, hash));
			assert_x(block);
			auto block_state(m_cache->block_state_get(transaction, hash));
			assert_x(block_state);

			//previous summary hash
			mcp::summary_hash previous_summary_hash(0);
			if (!block->previous().is_zero())
			{
				bool previous_summary_hash_error(m_cache->block_summary_get(transaction, block->previous(), previous_summary_hash));
				assert_x(!previous_summary_hash_error);
			}
			response_l["previous_summary"] = previous_summary_hash.to_string();

			//parent summary hashs
			mcp::json parent_summaries_l = mcp::json::array();
			for (mcp::block_hash const & pblock_hash : block->parents())
			{
				mcp::summary_hash p_summary_hash;
				bool p_summary_hash_error(m_cache->block_summary_get(transaction, pblock_hash, p_summary_hash));
				assert_x(!p_summary_hash_error);

				parent_summaries_l.push_back(p_summary_hash.to_string());
			}
			response_l["parent_summaries"] = parent_summaries_l;

			//link summary hashs
			std::shared_ptr<std::list<mcp::block_hash>> links(block->links());
			mcp::json link_summaries_l = mcp::json::array();
			for (auto it(links->begin()); it != links->end(); it++)
			{
				mcp::block_hash const & link_hash(*it);
				mcp::summary_hash l_summary_hash;
				bool l_summary_hash_error(m_cache->block_summary_get(transaction, link_hash, l_summary_hash));
				assert_x(!l_summary_hash_error);

				link_summaries_l.push_back(l_summary_hash.to_string());
			}
			response_l["link_summaries"] = link_summaries_l;

			//skip list
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
					skiplist_summaries_l.push_back(s.to_string());
			}
			response_l["skiplist_summaries"] = skiplist_summaries_l;

			response_l["status"] = (uint64_t)block_state->status;

            if (block_state->receipt)
            {
			    response_l["from_state"] = block_state->receipt->from_state.to_string();
                mcp::json to_states_l = mcp::json::array();
			    for (auto sh : block_state->receipt->to_state)
			    {
				    to_states_l.push_back(sh.to_string());
			    }
			    response_l["to_states"] = to_states_l;
            }
            else
            {
                response_l["from_state"] = nullptr;
                response_l["to_states"] = nullptr;
            }
		}

		error_code_l = mcp::rpc_block_error_code::ok;
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	}
	else
	{
		error_code_l = mcp::rpc_block_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
	}
}


void mcp::rpc_handler::sign_msg()
{
	mcp::rpc_sign_msg_error_code error_code_l;

    if (!request.count("public_key") || (!request["public_key"].is_string()))
    {
        error_code_l = mcp::rpc_sign_msg_error_code::invalid_public_key;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string public_key_text = request["public_key"];
	mcp::public_key public_key;
	auto error(public_key.decode_hex(public_key_text));
	if (error)
	{
		error_code_l = mcp::rpc_sign_msg_error_code::invalid_public_key;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("msg") || (!request["msg"].is_string()))
    {
        error_code_l = mcp::rpc_sign_msg_error_code::invalid_msg;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string sign_msg_text = request["msg"];
	mcp::uint256_union sign_msg;
	error = sign_msg.decode_hex(sign_msg_text);
	if (error)
	{
		error_code_l = mcp::rpc_sign_msg_error_code::invalid_msg;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    if (!request.count("password") || (!request["password"].is_string()))
    {
        error_code_l = mcp::rpc_sign_msg_error_code::invalid_password;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
	std::string password_a = request["password"];
	mcp::raw_key prv;
	if (password_a.empty())
	{
		bool exists(m_key_manager->find_unlocked_prv(public_key, prv));
		if (!exists)
		{
			error_code_l = mcp::rpc_sign_msg_error_code::wrong_password;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	else
	{
		bool error(m_key_manager->decrypt_prv(public_key, password_a, prv));
		if (error)
		{
			error_code_l = mcp::rpc_sign_msg_error_code::wrong_password;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	mcp::json response_l;
	mcp::signature sig(mcp::sign_message(prv, public_key, sign_msg));
	response_l["signature"] = sig.to_string();
	error_code_l = mcp::rpc_sign_msg_error_code::ok;
	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::version()
{
	mcp::rpc_version_error_code error_code_l = mcp::rpc_version_error_code::ok;
	mcp::json response_l;
	response_l["version"] = STR(MCP_VERSION);
	response_l["rpc_version"] = "1";
	response_l["store_version"] = std::to_string(m_store.version_get());

	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::status()
{	
    //stable_mci
    mcp::rpc_status_error_code error_code_l;
    uint64_t last_stable_mci(m_chain->last_stable_mci());
    uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());
    mcp::json js;
	js["syncing"] = mcp::node_sync::is_syncing() ? 1 : 0;
    js["last_stable_mci"] = last_stable_mci;
    js["last_mci"] = last_mci;
	js["last_stable_block_index"] = last_stable_index;
    error_code_l = mcp::rpc_status_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), js);
}

void mcp::rpc_handler::peers()
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
		peer_l["id"] = id.to_string();
		std::stringstream ss_endpoint;
		ss_endpoint << endpoint;
		peer_l["endpoint"] = ss_endpoint.str();
		peers_l.push_back(peer_l);
	}
	response_l["peers"] = peers_l;

	error_code_l = mcp::rpc_peers_error_code::ok;
	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::nodes()
{
	mcp::rpc_nodes_error_code error_code_l;

	mcp::json response_l;
	mcp::json nodes_l = mcp::json::array();
	std::list<p2p::node_info> nodes(m_host->nodes());
	for (p2p::node_info node : nodes)
	{
		mcp::json node_l;
		node_l["id"] = node.id.to_string();
		std::stringstream ss_endpoint;
		ss_endpoint << (bi::tcp::endpoint)node.endpoint;
		node_l["endpoint"] = ss_endpoint.str();
		nodes_l.push_back(node_l);
	}

	error_code_l = mcp::rpc_nodes_error_code::ok;
	error_response(response, (int)error_code_l, err.msg(error_code_l), nodes_l);

}

void mcp::rpc_handler::work_get()
{
    if (rpc.config.enable_control)
    {
        mcp::rpc_work_get_error_code error_code_l;

        if (!request.count("account") || (!request["account"].is_string()))
        {
            error_code_l = mcp::rpc_work_get_error_code::invalid_account;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
            return;
        }
        std::string account_text = request["account"];
        mcp::account account;
        auto error(account.decode_account(account_text));
        if (!error)
        {
            mcp::uint64_union work(0);
            mcp::block_hash root_l(0);
            if (!m_key_manager->work_account_get(account, root_l, work))
            {
                mcp::json response_l;
                response_l["root"] = root_l.to_string();
                std::string work_text = work.to_string();
                response_l["work"] = work_text;
                error_code_l = mcp::rpc_work_get_error_code::ok;
                error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
            }
            else
            {
                error_code_l = mcp::rpc_work_get_error_code::account_not_exisit;
                error_response(response, (int)error_code_l, err.msg(error_code_l));
            }
        }
        else
        {
            error_code_l = mcp::rpc_work_get_error_code::invalid_account;
            error_response(response, (int)error_code_l,err.msg(error_code_l));
        }
    }
    else
    {
        error_response(response, "RPC control is disabled");
    }

}

void mcp::rpc_handler::witness_list()
{
    mcp::rpc_witness_list_error_code error_code_l;
	mcp::witness_param const & w_param(mcp::param::curr_witness_param());
	mcp::json response_l;
	mcp::json witness_list_l = mcp::json::array();
	for (auto i: w_param.witness_list)
	{
		witness_list_l.push_back(i.to_account());
	}
	response_l["witness_list"] = witness_list_l;

    error_code_l = mcp::rpc_witness_list_error_code::ok;
    error_response(response,int(error_code_l),err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::debug_trace_transaction()
{
	mcp::rpc_debug_trace_transaction_error_code error_code_l;

	if (!request.count("hash") || !request["hash"].is_string())
	{
		error_code_l = mcp::rpc_debug_trace_transaction_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	std::string hash_text = request["hash"];
	mcp::uint256_union hash;
	auto error(hash.decode_hex(hash_text));

	if (error)
	{
		error_code_l = mcp::rpc_debug_trace_transaction_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l) + "," + hash_text);
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());
	dev::eth::McInfo mc_info;
	if (!m_chain->get_mc_info_from_block_hash(transaction, m_cache, hash, mc_info))
	{
		error_code_l = mcp::rpc_debug_trace_transaction_error_code::invalid_mci;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
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
		mcp::json response_l;
		dev::eth::EnvInfo env(transaction, m_store, m_cache, mc_info);
		auto block(m_cache->block_get(transaction, hash));
		assert_x(block);
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache, block);
		mcp::ExecutionResult er;
		std::list<std::shared_ptr<mcp::trace>> traces;
		mcp::Executive e(c_state, env, true, traces);
		e.setResultRecipient(er);

		mcp::json trace = m_chain->traceTransaction(e, options);
		response_l["gas"] = block->hashables->gas.str();
		response_l["return_value"] = toHexPrefixed(er.output);
		response_l["struct_logs"] = trace;

		error_code_l = mcp::rpc_debug_trace_transaction_error_code::ok;
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	}
	catch (Exception const& _e)
	{
		cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
			<< diagnostic_information(_e);
		exit(1);
	}
	catch (std::exception const& _e)
	{
		std::cerr << _e.what() << std::endl;
		throw;
	}
}

void mcp::rpc_handler::debug_storage_range_at()
{
	mcp::rpc_debug_storage_range_at_error_code error_code_l;

	mcp::block_hash hash;
	if (!request.count("hash") || !request["hash"].is_string() || hash.decode_hex(request["hash"]))
	{
		error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_hash;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::account acct(0);
	if (!request.count("account") || !request["account"].is_string() || acct.decode_account(request["account"]))
	{
		error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_account;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::uint256_union begin_u;
	if (!request.count("begin") || !request["begin"].is_string() || begin_u.decode_hex(request["begin"]))
	{
		error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_begin;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}
	h256 begin = h256(begin_u.ref());

	uint64_t max_results(0);
	if (!request.count("max_results") || !try_get_uint64_t_from_json("max_results", max_results))
	{
		error_code_l = mcp::rpc_debug_storage_range_at_error_code::invalid_max_results;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

	mcp::json ret = mcp::json::object();
	ret["storage"] = mcp::json::object();

	try
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());
		chain_state c_state(transaction, 0, m_store, m_chain, m_cache);

		std::map<h256, std::pair<u256, u256>> const storage(c_state.storage(acct));

		// begin is inclusive
		auto itBegin = storage.lower_bound(begin);
		for (auto it = itBegin; it != storage.end(); ++it)
		{
			if (ret["storage"].size() == static_cast<unsigned>(max_results))
			{
				ret["next_key"] = toCompactHexPrefixed(it->first, 32);
				break;
			}

			mcp::json keyValue = mcp::json::object();
			std::string hashedKey = toCompactHexPrefixed(it->first, 32);
			keyValue["key"] = toCompactHexPrefixed(it->second.first, 32);
			keyValue["value"] = toCompactHexPrefixed(it->second.second, 32);

			ret["storage"][hashedKey] = keyValue;
		}

		error_code_l = mcp::rpc_debug_storage_range_at_error_code::ok;
		error_response(response, (int)error_code_l, err.msg(error_code_l), ret);
	}
	catch (Exception const& _e)
	{
		cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
			<< diagnostic_information(_e);
		exit(1);
	}
}

void mcp::rpc_handler::web3_clientVersion() {
	mcp::rpc_version_error_code error_code_l = mcp::rpc_version_error_code::ok;
	mcp::json response_l;
	response_l["version"] = STR(MCP_VERSION);
	response_l["rpc_version"] = "1";
	response_l["store_version"] = std::to_string(m_store.version_get());

	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::web3_sha3() {
	mcp::rpc_web3_sha3_error_code error_code_l = mcp::rpc_web3_sha3_error_code::ok;
	mcp::json response_l;
	
	if (!request.count("params") || (!request["params"].is_array()))
    {
        error_code_l = mcp::rpc_web3_sha3_error_code::invalid_params;
        error_response(response, (int)error_code_l, err.msg(error_code_l));
        return;
    }
    std::string s = request["params"][0].get<std::string>();
    std::string mHashKey;
    if (dev::isHex(s) && s.compare(0, 2, "0x") == 0) {
    	mHashKey = toJS(sha3(jsToBytes(s)));
    } else {
    	mHashKey = toJS(dev::sha3(s));
    }
    std::uint64_t a;
    hex_to_uint64(s, a);
    response_l["a"] = a;
	response_l["id"] = request["id"];
    response_l["jsonrpc"] = request["jsonrpc"];
	response_l["result"] = mHashKey;

	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

/**
 * 
 * 
 * 
 * */
void mcp::rpc_handler::eth_accounts(){
	mcp::rpc_account_list_error_code error_code_l;

	mcp::json j_response;
    mcp::json j_accounts = mcp::json::array();
	std::list<mcp::public_key> pubkey_list(m_key_manager->list());

	//std::list<mcp::account> account_list(m_key_manager->list());
    for (auto public_key : pubkey_list)
    {
		mcp::account account(public_key);
        j_accounts.push_back(account.to_account());
    }
    j_response["result"] = j_accounts;
    j_response["id"] = request["id"];
    j_response["jsonrpc"] = request["jsonrpc"];
    error_code_l = mcp::rpc_account_list_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), j_response);
}

/**
 *  //Result 
  	{
	  	"id":83,
	  	"jsonrpc": "2.0",
	  	"result": "0x4b7" // 1207
	}
 * 
 * */
void mcp::rpc_handler::eth_blockNumber() {
	//stable_mci
    mcp::rpc_status_error_code error_code_l;
    uint64_t last_stable_mci(m_chain->last_stable_mci());
    uint64_t last_mci(m_chain->last_mci());
	uint64_t last_stable_index(m_chain->last_stable_index());
    mcp::json js;
    js["id"] = request["id"];
    js["jsonrpc"] = request["jsonrpc"];
    js["result"] = uint64_to_hex_nofill(last_mci);
	js["syncing"] = mcp::node_sync::is_syncing() ? 1 : 0;
    js["last_stable_mci"] = last_stable_mci;
    js["last_mci"] = last_mci;
	js["last_stable_block_index"] = last_stable_index;
    error_code_l = mcp::rpc_status_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), js);
}

void mcp::rpc_handler::eth_estimateGas() {
	mcp::rpc_estimate_gas_error_code error_code_l;
    if (!rpc.config.enable_control)
    {
        error_response(response, "RPC control is disabled");
        return;
    }

	if (!request.count("params") || (!request["params"].is_array()))
    {
        error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_from;
        error_response(response, (int)error_code_l, err.msg(error_code_l) );
        return;
    }

	mcp::account from(0);
	if (!request["params"][0]["from"].is_null() && false)
	{
		if (!request["params"][0]["from"].is_string())
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_from;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}

		std::string from_text = request["params"][0]["from"];
		bool error(from.decode_account(from_text));
		if (error)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_from;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}
	
	mcp::account to(0);
	if (!request["params"][0]["to"].is_null())
	{
		if (!request["params"][0]["to"].is_string())
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_to;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}

		std::string to_text = request["params"][0]["to"];
		bool error = to.decode_account(to_text);
		if (error)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_account_to;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	mcp::amount amount(0);
    if (!request["params"][0]["value"].is_null())
    {
		if (!request["params"][0]["value"].is_string())
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_amount;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		std::string amount_text = request["params"][0]["value"];
		bool error = !boost::conversion::try_lexical_convert(amount_text, amount);
		if (error)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_amount;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
    }

	uint64_t gas(0);
	if (!request["params"][0]["gas"].is_null())
	{
		if (!try_get_uint64_t_from_json("gas", gas))
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_gas;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	uint64_t gas_price(0);
	if (!request["params"][0]["gasPrice"].is_null())
	{
		// || !boost::conversion::try_lexical_convert(request["params"][0]["gasPrice"].get<std::string>(), gas_price)
		if (!request["params"][0]["gasPrice"].is_string()
			)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::invalid_gas_price;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
		hex_to_uint64(request["params"][0]["gasPrice"].get<std::string>(), gas_price);
		LOG(m_log.error) << "LLLLLL OOOOOO GGGGGG" << gas_price;
	}

	dev::bytes data;
	if (request.count("data"))
	{
		std::string data_text = request["data"];
        bool error = mcp::hex_to_bytes(data_text, data);
        if (error)
        {
            error_code_l = mcp::rpc_estimate_gas_error_code::invalid_data;
            error_response(response, (int)error_code_l, err.msg(error_code_l));
            return;
        }

		if (data.size() > mcp::max_data_size)
		{
			error_code_l = mcp::rpc_estimate_gas_error_code::data_size_too_large;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		}
	}

	dev::eth::McInfo mc_info;
	if (!try_get_mc_info(mc_info))
	{
		error_code_l = mcp::rpc_estimate_gas_error_code::invalid_mci;
		error_response(response, (int)error_code_l, err.msg(error_code_l));
		return;
	}

    mcp::db::db_transaction transaction(m_store.create_transaction());
    std::pair<u256, bool> result = m_chain->estimate_gas(transaction, m_cache, from, amount, to, data, gas, gas_price, mc_info);

    mcp::json response_l;
	if (!result.second)
	{
		error_code_l = mcp::rpc_estimate_gas_error_code::gas_not_enough_or_fail;
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
		return;
	}
    response_l["result"] = uint256_to_hex_nofill(result.first);
    error_code_l = mcp::rpc_estimate_gas_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}

void mcp::rpc_handler::eth_chainId() {
    mcp::rpc_status_error_code error_code_l;
    mcp::json js;
    js["id"] = request["id"];
    js["jsonrpc"] = request["jsonrpc"];
    js["result"] = "0x33c";
    error_code_l = mcp::rpc_status_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), js);
}

void mcp::rpc_handler::eth_gasPrice() {
	mcp::rpc_status_error_code error_code_l;
    mcp::json js;
    js["id"] = request["id"];
    js["jsonrpc"] = request["jsonrpc"];
    js["result"] = uint64_to_hex_nofill(1000000);
    error_code_l = mcp::rpc_status_error_code::ok;
    error_response(response, (int)error_code_l, err.msg(error_code_l), js);
}

void mcp::rpc_handler::eth_getTransactionCount() {
	mcp::rpc_status_error_code error_code_l;
	mcp::json js;
	js["id"] = request["id"];
	js["jsonrpc"] = request["jsonrpc"];
	js["result"] = "0x0"; // temp
	error_code_l = mcp::rpc_status_error_code::ok;
	error_response(response, (int)error_code_l, err.msg(error_code_l), js);
}

void mcp::rpc_handler::eth_getBlockByNumber() {

    mcp::rpc_block_error_code error_code_l = mcp::rpc_block_error_code::ok;
    mcp::json response_l;
  	response_l["id"] = request["id"];
    response_l["jsonrpc"] = request["jsonrpc"];

	if (!request.count("params") || (!request["params"].is_array()))
    {
        error_code_l = mcp::rpc_block_error_code::invalid_hash;
        error_response(response, (int)error_code_l, err.msg(error_code_l), response_l );
        return;
    }
    uint64_t block_number;
    if (request["params"][0].is_string()){
    	dev::eth::McInfo mc_info;
		if (!try_get_mc_info(mc_info))
		{
			error_code_l = mcp::rpc_block_error_code::invalid_hash;
			error_response(response, (int)error_code_l, err.msg(error_code_l));
			return;
		} else{
			block_number = mc_info.mci;
		}
    }else {
    	block_number = request["params"][0].get<uint64_t>();	
    }
    
    // uint64_t block_number;
    // std::string s = request["params"][0].get<std::string>();
    bool full_transaction = request["params"][1];
    // hex_to_uint64(s, block_number);
    mcp::db::db_transaction transaction(m_store.create_transaction());

	mcp::block_hash block_hash_l;
	bool exists(!m_store.stable_block_get(transaction, block_number, block_hash_l));
	assert_x(exists);

	std::shared_ptr<mcp::block_state> state(m_cache->block_state_get(transaction, block_hash_l));
	if (state != nullptr)
	{
		auto block(m_cache->block_get(transaction, block_hash_l));
		assert_x(block);
		mcp::json block_state_l, block_result;
		
		block_state_l["hash"] = block_hash_l.to_string();
		block_result["hash"] = "0x" + block_state_l["hash"].get<std::string>();

		mcp::account contract_account(0);
		if (block->hashables->type == mcp::block_type::light 
			&& block->isCreation() 
			&& state->is_stable 
			&& (state->status == mcp::block_status::ok))
		{
			std::shared_ptr<mcp::account_state> acc_state(m_store.account_state_get(transaction, state->receipt->from_state));
			assert_x(acc_state);
			contract_account = toAddress(block->hashables->from, acc_state->nonce() - 1);
		}

		state->serialize_json(block_state_l, contract_account);
		if (!block_state_l["stable_content"].is_null()) {
			block_result["number"] = uint64_to_hex_nofill(block_state_l["stable_content"]["mci"].get<uint64_t>());

			if (!block_state_l["stable_content"]["log_bloom"].is_null()) {
				block_result["logsBloom"] = block_state_l["stable_content"]["log_bloom"];
			}

			block_result["gasLimit"] = "0x0";
			if (!block_state_l["stable_content"]["gas_used"].is_null()) {
				uint64_t gasUsed;
				hex_to_uint64(block_state_l["stable_content"]["gas_used"].get<std::string>(), gasUsed);
				block_result["gasUsed"] = uint64_to_hex_nofill(gasUsed);
				block_result["gasLimit"] = uint64_to_hex_nofill(gasUsed + 1000); // fake data
			}
			block_result["timestamp"] = uint64_to_hex_nofill(block_state_l["stable_content"]["mc_timestamp"].get<uint64_t>());
		}



		// Need to update these mockup data
		block_result["difficulty"] = "0x2";
		block_result["extraData"] = "0xd983010000846765746889676f312e31322e3137856c696e7578000000000000735424e16996bb366d0247a8c359eba969d3b51f64ab942a6a0e3e632d2e19c933db8b32d62950183a6799f5d6f6ac07bf448086c1bdc7b726a0842863bc807701";
		
		
		
		block_result["miner"] = "0x980a75ecd1309ea12fa2ed87a8744fbfc9b863d5";
		block_result["mixHash"] = "0x0000000000000000000000000000000000000000000000000000000000000000";
		block_result["nonce"] = "0x0000000000000000";
		
		block_result["parentHash"] = "0x50f93abc42d5ad0876a0768490ba7858239b6e38c5e066612cab9d7ab72ecfc2";
		block_result["receiptsRoot"] = "0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421";
		block_result["sha3Uncles"] = "0x1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347";
		block_result["size"] = "0x25f";
		block_result["stateRoot"] = "0x0b9279d6596c22b580a56e87110ab3f78a3dce913ffb7a2b157e2ed7b7146859";
		block_result["totalDifficulty"] = "0x29";
		block_result["transactions"] = mcp::json::array();
		block_result["transactionsRoot"] = "0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421";
		block_result["uncles"] = mcp::json::array();

		response_l["result"] = block_result;

		// response_l["block_state"] = block_state_l;
		error_code_l = mcp::rpc_block_error_code::ok;
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	}
	else
	{
		response_l["block_state"] = nullptr;
		error_code_l = mcp::rpc_block_error_code::block_not_exsist;
		error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
	}
	
}


void mcp::rpc_handler::eth_sendRawTransaction() {

}
/**
*	// Result
*	{
*	  "id":67,
*	  "jsonrpc": "2.0",
*	  "result": "59"
*	}
**/

void mcp::rpc_handler::net_version() {
	mcp::rpc_version_error_code error_code_l = mcp::rpc_version_error_code::ok;
	mcp::json response_l;
	response_l["id"] = request["id"];
	response_l["jsonrpc"] = request["jsonrpc"];
	response_l["version"] = STR(MCP_VERSION);
	// response_l["result"] = STR(MCP_VERSION);
	response_l["result"] = "828";
	response_l["rpc_version"] = "1";
	response_l["store_version"] = std::to_string(m_store.version_get());

	error_response(response, (int)error_code_l, err.msg(error_code_l), response_l);
}


mcp::rpc_connection::rpc_connection(mcp::rpc & rpc_a) :
	rpc(rpc_a),
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
	boost::beast::http::async_read(socket, buffer, request, [this_l](boost::system::error_code const & ec, size_t bytes_transferred) {
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
					error_response(response_handler, "Can only POST requests");
				}
			});
		}
		else
		{
            LOG(this_l->m_log.error) << "HTTP RPC read error: " << ec.message();
		}
	});
}

namespace
{
	void reprocess_body(std::string & body, mcp::json & json_a)
	{
		body = json_a.dump();
	}
}

void mcp::rpc_handler::process_request()
{
	try
	{
        request = mcp::json::parse(body);
		LOG(m_log.error) << "Request Parse" << request;
        std::string action;
        if (request["action"].is_string()) {
        	action = request["action"];
        }
        if (request["method"].is_string()) {
        	action = request["method"];
        }

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

        LOG(m_log.debug) << body;

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
		else if (action == "work_get")
		{
			work_get();
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
			error_response(response, mcp::node_sync::get_syncing_status());
		}
		else if (action == "web3_clientVersion") {
			web3_clientVersion();
		}
		else if (action == "eth_accounts") {
			eth_accounts();
		}
		else if (action == "eth_blockNumber") {
			eth_blockNumber();
		}
		else if (action == "net_version") {
			net_version();
		}
		else if (action == "web3_sha3") {
			web3_sha3();
		}
		else if (action == "eth_chainId") {
			eth_chainId();
		}
		else if (action == "eth_getBlockByNumber") {
			eth_getBlockByNumber();
		}
		else if (action == "eth_gasPrice") {
			eth_gasPrice();
		}
		else if (action == "eth_estimateGas") {
			eth_estimateGas();
		}
		else if (action == "eth_getTransactionCount") {
			eth_getTransactionCount();
		}
        else
		{
			error_response(response, "Unknown command");
		}
	}
	catch (std::exception const & err)
	{
        LOG(m_log.error) << "rpc runtime_error : error_response = Unable to parse JSON or " << err.what();
        error_response(response, "Unable to parse JSON");
	}
	catch (...)
	{
        LOG(m_log.error) << "Internal server error in HTTP RPC ";
        error_response(response, "Internal server error in HTTP RPC");
	}
}

std::shared_ptr<mcp::rpc> mcp::get_rpc(mcp::block_store & store_a, std::shared_ptr<mcp::chain> chain_a,
	std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
	std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::p2p::host> host_a,
	std::shared_ptr<mcp::async_task> background_a,
	boost::asio::io_service & service_a, mcp::rpc_config const& config_a)
{
	std::shared_ptr<rpc> impl(new rpc(store_a, chain_a, cache_a, key_manager_a, wallet_a, host_a, background_a, service_a, config_a));
	return impl;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_create_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_remove_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_unlock_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_lock_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_import_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_export_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_validate_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_password_change_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_list_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_block_list_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_balance_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_accounts_balances_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_account_code_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_generate_offline_block_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_send_offline_block_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_sign_msg_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_block_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_blocks_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_stable_blocks_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_status_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_witness_list_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_work_get_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_version_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_peers_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_nodes_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_stop_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_web3_sha3_error_code const & err_a)
{
	std::string error_msg;
	switch (err_a)
	{
	case mcp::rpc_web3_sha3_error_code::ok:
		error_msg = "OK";
		break;
	case mcp::rpc_web3_sha3_error_code::invalid_params:
		error_msg = "Invalid Params";
		break;
	default:
		error_msg = "Unkonw error";
		break;
	}
	return error_msg;
}

std::string mcp::rpc_error_msg::msg(mcp::rpc_send_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_estimate_gas_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_call_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_logs_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_debug_trace_transaction_error_code const & err_a)
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

std::string mcp::rpc_error_msg::msg(mcp::rpc_debug_storage_range_at_error_code const & err_a)
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
