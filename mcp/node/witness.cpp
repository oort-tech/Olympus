#include <mcp/node/witness.hpp>
#include <mcp/core/genesis.hpp>
#include <mcp/core/param.hpp>
#include <mcp/consensus/ledger.hpp>

mcp::witness::witness(mcp::error_message & error_msg,
	std::shared_ptr<mcp::key_manager> key_manager_a,
	mcp::block_store& store_a, std::shared_ptr<mcp::alarm> alarm_a,
	std::shared_ptr<mcp::composer> composer_a, std::shared_ptr<mcp::chain> chain_a,
	std::shared_ptr<mcp::block_processor> block_processor_a,
	std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<TransactionQueue> tq,
	std::shared_ptr<ApproveQueue> aq,
	std::string const & account_or_file_text, std::string const & password_a
) :
	m_store(store_a),
	m_alarm(alarm_a),
	m_composer(composer_a),
	m_chain(chain_a),
	m_block_processor(block_processor_a),
	m_cache(cache_a),
	m_tq(tq),
	m_aq(aq),
	m_last_witness_time(std::chrono::steady_clock::now()),
	m_witness_interval(std::chrono::milliseconds(m_max_witness_interval))
{
	m_chain->onMciStable([this](uint64_t const& mci) { try_create_approve(mci); });
	bool error(!mcp::isAddress(account_or_file_text));
	if (error) /// Specifies the keystore that needs to be imported. like: --witness_account=\home\0x1144B522F45265C2DFDBAEE8E324719E63A1694C.json
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
	else /// Specify an account(just address) that has been imported.like: --witness_account=0x1144B522F45265C2DFDBAEE8E324719E63A1694C
	{
		m_account = dev::Address(account_or_file_text);
	}

	error = key_manager_a->decrypt_prv(m_account, password_a, m_secret);
	if (error)
	{
		error_msg.error = true;
		LOG(m_log.error) << "Witness error: Account not exists, " << m_account.hexPrefixed();
		return;
	}
	m_rawPubkey = dev::toPublickey(m_secret);

    mcp::db::db_transaction transaction(m_store.create_transaction());

    //std::cout << "Witness start success.\n" << std::flush;
    LOG(m_log.info) << "witness account:" << m_account.hexPrefixed();


	///try send approves
	try_create_approve(m_chain->last_stable_mci());
}

void mcp::witness::start()
{
	std::weak_ptr<mcp::witness> this_w(shared_from_this());
	
	uint32_t ms = mcp::mcp_network == mcp::mcp_networks::mcp_mini_test_network ? 50 : mcp::random_pool.GenerateWord32(500, 1000);
	m_alarm->add(std::chrono::steady_clock::now() + std::chrono::milliseconds(ms), [this_w]() {
		if (auto this_l = this_w.lock())
		{
            //LOG(this_l->m_log.info) << "Witness :is time to check_and_witness";
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
		witness_interval_count++;
		m_is_witnessing.clear();
		return;
	}

	if (mcp::node_sync::is_syncing())
	{
		witness_syncing_count++;
        LOG(m_log.info) << "Not do witness when syncing";
		m_last_witness_time = std::chrono::steady_clock::now();
		m_is_witnessing.clear();
		return;
	}

	mcp::db::db_transaction transaction(m_store.create_transaction());

	if (m_tq->size() == 0)
	{
		size_t transaction_unstable_count(m_store.transaction_unstable_count(transaction));
		if (transaction_unstable_count == 0)
		{
			witness_transaction_count++;
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
		std::shared_ptr<mcp::block_state> last_summary_block_state(m_cache->block_state_get(transaction, mc_block->last_stable_block()));
		assert_x(last_summary_block_state
			&& last_summary_block_state->is_stable
			&& last_summary_block_state->is_on_main_chain
			&& last_summary_block_state->main_chain_index);

		last_summary_mci = *last_summary_block_state->main_chain_index;
	}

	Epoch epoch = mcp::epoch(last_summary_mci);
	mcp::witness_param const & w_param(mcp::param::witness_param(transaction, epoch));
	if (!mcp::param::is_witness(transaction, epoch, m_account))
	{
		witness_notwitness_count++;
		m_is_witnessing.clear();
		LOG(m_log.debug) << "Not do witness, account:" << m_account.hexPrefixed() << " is not witness, last_summary_mci:" << last_summary_mci;
		return;
	}

	//check majority different of witnesses
	bool is_diff_majority(Ledger.check_majority_witness(transaction, m_cache, mc_block_hash, m_account, w_param));
	if (!is_diff_majority)
	{
		witness_majority_count++;
		m_is_witnessing.clear();
		LOG(m_log.trace) << "Not do witness because check majority different of witnesses";
		return;
	}

	do_witness();
}

void mcp::witness::do_witness()
{
    LOG(m_log.info) << "Do witness";

	try
	{
		auto block = m_composer->compose_block(m_account, m_secret);
		std::shared_ptr<mcp::joint_message> joint(new mcp::joint_message(block));

		std::shared_ptr<std::promise<mcp::validate_status>> p(std::make_shared<std::promise<mcp::validate_status>>());
		auto fut = p->get_future();
		std::shared_ptr<mcp::block_processor_item> item(std::make_shared<mcp::block_processor_item>(*joint, p));
		m_block_processor->add_to_mt_process(item);
		mcp::validate_status ret = fut.get();
		if (!ret.ok)
		{
			LOG(m_log.error) << "invalid compose result codes:" << ret.msg;
		}
		else
		{
			LOG(m_log.info) << "Do witness ok";
		}
		m_last_witness_time = std::chrono::steady_clock::now();
		m_is_witnessing.clear();
		//LOG(m_log.info) << "witness hash:" << block->hash().hex() << " ,links:" << block->links().size();
	}
	catch (Exception& _e)
	{
		LOG(m_log.error) << "witness error," << _e.what();
		m_is_witnessing.clear();
	}
}

void mcp::witness::try_create_approve(uint64_t const& mci)
{
	if (mci == 0 || mcp::node_sync::is_syncing()) 
	{
		//LOG(m_log.debug) << "Needn't send approve when syncing";
		return;
	}
	static Epoch last_epoch = UINT64_MAX;
	Epoch epoch = mcp::epoch(mci);
	if (epoch != last_epoch &&
		mci%mcp::epoch_period > *(uint64_t *)m_account.data() % mcp::epoch_period / 10) 
	{
		mcp::db::db_transaction transaction(m_store.create_transaction());

		///send if staking finalized.
		if (!m_chain->IsStakingList(transaction, epoch, m_account))
		{
			last_epoch = epoch;
			LOG(m_log.info) << "[witness] epoch=" << epoch << " staking was not completed.";
			return;
		}
		mcp::block_hash hash;
		if (epoch <= 1) {
			hash = mcp::genesis::block_hash;
		}
		else {
			bool exists(!m_store.main_chain_get(transaction, (epoch - 1)*epoch_period, hash));
			assert_x(exists);
			///maybe not stable,But it doesn't matter
		}

		h648 proof;
		if (!dev::signProve(proof, m_secret, m_rawPubkey, hash)) {
			//LOG(m_log.debug) << "[send_approve] secp256k1_vrf_prove fail m_last_summary_mci=" << mci << " epoch=" << epoch;
			return;
		}

		auto ap = std::make_shared<approve>(epoch, proof, m_secret);
		if (!m_cache->approve_exists(transaction, ap->sha3()))///maybe send but not execute
		{
			LOG(m_log.info) << "[send_approve] m_last_summary_mci=" << mci << " epoch=" << epoch;
			m_aq->importLocal(ap);
		}
		
		last_epoch = epoch;
		approve_success_count++;
	}
	else
	{
		//LOG(m_log.debug) << "[send_approve] last_epoch existed m_last_summary_mci=" << mci << " epoch=" << epoch;
		approve_failed_count++;
	}
}

std::string mcp::witness::getInfo()
{
	std::string str = "lessInterval:" + std::to_string(witness_interval_count)
		+ " ,syncing:" + std::to_string(witness_syncing_count)
		+ " ,noTransaction:" + std::to_string(witness_transaction_count)
		+ " ,notWitness:" + std::to_string(witness_notwitness_count)
		+ " ,majority:" + std::to_string(witness_majority_count)
		+ " ,approveSuccessed:" + std::to_string(approve_success_count)
		+ " ,approveFailed:" + std::to_string(approve_failed_count);

	return str;
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
		/// parse json used low version
		switch (version)
		{
			//case 0:
			//{
			//	/// parse
			//	break;
			//}
			//case 1:
			//{
			//	/// parse
			//	break;
			//}
		default:
			error |= deserialize_json(json_a);
			break;
		}
	}
	catch (std::runtime_error const &)
	{
		error = true;
	}
	return error;
}

