#include <mcp/node/witness.hpp>
#include <mcp/core/genesis.hpp>

mcp::witness::witness(mcp::error_message & error_msg,
	mcp::ledger& ledger_a, std::shared_ptr<mcp::key_manager> key_manager_a,
	mcp::block_store& store_a, std::shared_ptr<mcp::alarm> alarm_a,
	std::shared_ptr<mcp::wallet> wallet_a, std::shared_ptr<mcp::chain> chain_a,
	std::shared_ptr<mcp::block_cache> cache_a,
	std::string const & account_or_file_text, std::string const & password_a,
	mcp::block_hash const & last_witness_block_hash_a, uint256_t const& gas_price_a
) :
	m_ledger(ledger_a),
	m_store(store_a),
	m_alarm(alarm_a),
	m_wallet(wallet_a),
	m_chain(chain_a),
	m_cache(cache_a),
	m_last_witness_time(std::chrono::steady_clock::now()),
	m_witness_interval(std::chrono::milliseconds(m_max_witness_interval)),
    m_witness_get_current_chain(true),
    m_last_witness_block_hash(last_witness_block_hash_a),
	gas_price(gas_price_a)
{
	bool error(m_account.decode_account(account_or_file_text));
	if (error)
	{
		mcp::key_content kc;
		bool error(key_manager_a->import(account_or_file_text, kc, false));
		if (error)
		{
			error_msg.error = true;
			error_msg.message = "Invalid account or json file";
			return;
		}
		m_account = kc.account;
	}

	error = key_manager_a->unlock(m_account, password_a);
	if (error)
	{
		error_msg.error = true;
		error_msg.message = "Account not exists or password wrong";
		return;
	}

    if (!m_last_witness_block_hash.is_zero())
    {
        mcp::db::db_transaction transaction(m_store.create_transaction());
        if (!m_store.block_exists(transaction, m_last_witness_block_hash))
        {
            m_witness_get_current_chain = false;
            LOG(m_log.info) << "witness account cannot do witness cause: " << m_last_witness_block_hash.to_string() << " not exsist.";
        }
    }

    //std::cout << "Witness start success.\n" << std::flush;
    LOG(m_log.info) << "witness account:" << m_account.to_account();
}

void mcp::witness::start()
{
	std::weak_ptr<mcp::witness> this_w(shared_from_this());
	
	uint32_t ms = mcp::mcp_network == mcp::mcp_networks::mcp_mini_test_network ? 50 : mcp::random_pool.GenerateWord32(500, 1000);
	m_alarm->add(std::chrono::steady_clock::now() + std::chrono::milliseconds(ms), [this_w]() {
		if (auto this_l = this_w.lock())
		{
            //LOG(this_l->node->log_node.debug) << "Witness :is time to check_and_witness";
			this_l->check_and_witness();
			this_l->start();
		}
	});
}

void mcp::witness::check_and_witness()
{
    if (m_is_witnessing.test_and_set())
        return;

	if (mcp::mcp_network != mcp::mcp_networks::mcp_mini_test_network 
		&& std::chrono::steady_clock::now() - m_last_witness_time < m_witness_interval)
	{
		m_is_witnessing.clear();
		return;
	}

	if (mcp::node_sync::is_syncing())
	{
        LOG(m_log.info) << "Not do witness when syncing";
		m_last_witness_time = std::chrono::steady_clock::now();
		m_is_witnessing.clear();
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());

    //can send witness
    if (!m_witness_get_current_chain)
    {
        if (!m_store.block_exists(transaction, m_last_witness_block_hash))
        {
            m_is_witnessing.clear();
            LOG(m_log.info) << "Not do witness, last_witness_block_hash:" << m_last_witness_block_hash.to_string() << " not exsist.";
            return;
        }
        else
        {
            m_witness_get_current_chain = true;
        }
    }

	size_t light_unstable_count(m_store.light_unstable_count(transaction));
	if (light_unstable_count == 0)
	{
		size_t unlink_info_count(m_store.unlink_info_count(transaction));
		if (unlink_info_count == 0)
		{
			m_is_witnessing.clear();
			return;
		}
	}

	mcp::block_hash mc_block_hash;
	while (true)
	{
		uint64_t last_mci = m_chain->last_mci();
		bool exists(!m_store.main_chain_get(transaction, last_mci, mc_block_hash));
		if (exists)
			break;
	}

	uint64_t last_summary_mci(0);
	if (mc_block_hash != mcp::genesis::block_hash)
	{
		std::shared_ptr<mcp::block> mc_block(m_cache->block_get(transaction, mc_block_hash));
		std::shared_ptr<mcp::block_state> last_summary_block_state(m_cache->block_state_get(transaction, mc_block->hashables->last_summary_block));
		assert_x(last_summary_block_state
			&& last_summary_block_state->is_stable
			&& last_summary_block_state->is_on_main_chain
			&& last_summary_block_state->main_chain_index);

		last_summary_mci = *last_summary_block_state->main_chain_index;
	}

	mcp::witness_param const & w_param(mcp::param::witness_param(last_summary_mci));

	if (!mcp::param::is_witness(last_summary_mci, m_account))
	{
		m_is_witnessing.clear();
		LOG(m_log.info) << "Not do witness, account:" << m_account.to_account() << " is not witness, last_summary_mci:" << last_summary_mci;
		return;
	}

	//check majority different of witnesses
	bool is_diff_majority(m_ledger.check_majority_witness(transaction, m_cache, mc_block_hash, m_account, w_param));
	if (!is_diff_majority)
	{
		m_is_witnessing.clear();
		LOG(m_log.info) << "Not do witness because check majority different of witnesses";
		return;
	}

	do_witness();
}

void mcp::witness::do_witness()
{
    LOG(m_log.info) << "Do witness";

	boost::optional<mcp::block_hash> previous = boost::none;
	auto from(m_account);
	auto to(0);
	uint128_t amount(0);
	mcp::uint256_t gas(0);
	mcp::uint256_t gas_price(0);
	std::vector<uint8_t> data;
	boost::optional<std::string> password = boost::none;
	bool gen_work = true;
	bool async = false;

	std::weak_ptr<mcp::witness> this_w(shared_from_this());
	m_wallet->send_async(mcp::block_type::dag, previous, from, to, amount, gas, gas_price, data, password, [from, this_w](mcp::send_result result) {
		if (auto this_l = this_w.lock())
		{
			switch (result.code)
			{
			case mcp::send_result_codes::ok:
			{
                LOG(this_l->m_log.info) << "Do witness ok";
				uint32_t r_interval = mcp::random_pool.GenerateWord32(this_l->m_min_witness_interval, this_l->m_max_witness_interval);
				this_l->m_witness_interval = std::chrono::milliseconds(r_interval);
				break;
			}
			case mcp::send_result_codes::from_not_exists:
                LOG(this_l->m_log.error) << "Witness error: Account not exists, " << from.to_account();
				break;
			case mcp::send_result_codes::account_locked:
                LOG(this_l->m_log.error) << "Witness error: Account locked";
				break;
			case mcp::send_result_codes::wrong_password:
                LOG(this_l->m_log.error) << "Witness error: Wrong password";
				break;
			case mcp::send_result_codes::insufficient_balance:
                LOG(this_l->m_log.error) << "Witness error: Insufficient balance";
				break;
			case mcp::send_result_codes::data_size_too_large:
                LOG(this_l->m_log.error) << "Witness error: Data size to large";
				break;
			case mcp::send_result_codes::validate_error:
			{
                LOG(this_l->m_log.error) << "Witness error: Validate error";
				break;
			}
			case mcp::send_result_codes::dag_no_links:
				LOG(this_l->m_log.error) << "Witness error: no links";
				break;
			case mcp::send_result_codes::error:
                LOG(this_l->m_log.error) << "Witness error: Generate block error";
				break;
			default:
                LOG(this_l->m_log.error) << "Unknown error";
				break;
			}

			this_l->m_last_witness_time = std::chrono::steady_clock::now();
			this_l->m_is_witnessing.clear();
		}
	}, gen_work, async);
}

std::atomic_flag mcp::witness::m_is_witnessing = ATOMIC_FLAG_INIT;
uint32_t const mcp::witness::m_max_do_witness_interval(1000);
uint64_t const mcp::witness::m_threshold_distance(50);

mcp::witness_config::witness_config():
    is_witness(false)
{
}

void mcp::witness_config::serialize_json(mcp::json &json_a) const
{
    json_a["witness"] = is_witness ? "true":"false";
    json_a["witness_account"] = account_or_file;
    json_a["password"] = password;
    json_a["last_block"] = last_block;
	json_a["gas_price"] = gas_price.str();
}

bool mcp::witness_config::deserialize_json(mcp::json const & json_a)
{
    auto error(false);
    try
    {
        if (json_a.count("witness") && json_a["witness"].is_string())
        {
            is_witness = (json_a["witness"].get<std::string>() == "true" ? true : false);
        }
        else
        {
            error = true;
        }
  
        if (json_a.count("witness_account") && json_a["witness_account"].is_string())
        {
            account_or_file = json_a["witness_account"].get<std::string>();
        }
        else
        {
            error = true;
        }

        if (json_a.count("password") && json_a["password"].is_string())
        {
            password = json_a["password"].get<std::string>();
        }
        else
        {
            error = true;
        }

        if (json_a.count("last_block") && json_a["last_block"].is_string())
        {
            last_block = json_a["last_block"].get<std::string>();
        }
        else
        {
            error = true;
        }

		if (json_a.count("gas_price") && json_a["gas_price"].is_string())
		{
			mcp::uint256_union uint;
			error = uint.decode_dec(json_a["gas_price"].get<std::string>());
			if (uint > 0)
				gas_price = uint.number();
			else
			{
				if (mcp::mcp_network == mcp::mcp_networks::mcp_live_network)
					gas_price = (uint256_t)5e13; //mcp 0.08
				else
					gas_price = 10000000;
			}
		}
    }
    catch (std::runtime_error const &)
    {
        error = true;
    }
    return error;
}

bool mcp::witness_config::parse_old_version_data(mcp::json const & json_a, uint64_t const& version)
{
	auto error(false);
	try
	{
		if (version < 3) //version 3 add gas price
		{
			if (json_a.count("witness") && json_a["witness"].is_string())
				is_witness = (json_a["witness"].get<std::string>() == "true" ? true : false);

			if (json_a.count("witness_account") && json_a["witness_account"].is_string())
				account_or_file = json_a["witness_account"].get<std::string>();

			if (json_a.count("password") && json_a["password"].is_string())
				password = json_a["password"].get<std::string>();

			if (json_a.count("last_block") && json_a["last_block"].is_string())
				last_block = json_a["last_block"].get<std::string>();
		}
		
		if (version == 3)
		{
			if (json_a.count("gas_price") && json_a["gas_price"].is_string())
			{
				mcp::uint256_union uint;
				error = uint.decode_dec(json_a["gas_price"].get<std::string>());
				gas_price = uint.number();
			}
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

