#include "wallet.hpp"
#include <mcp/node/composer.hpp>
#include <future>

mcp::wallet::wallet(
	mcp::wallet_store& store_a, mcp::block_store& block_store_a,
	std::shared_ptr<mcp::key_manager> key_manager_a, std::shared_ptr<mcp::composer> composer_a,
	std::shared_ptr<mcp::block_processor> block_processor_a
) :
	composer(composer_a),
	m_store(store_a),
	m_block_store(block_store_a),
	m_key_manager(key_manager_a),
	m_block_processor(block_processor_a),
	m_stopped(false),
	m_thread([this]() { do_wallet_actions(); })
{
}

void mcp::wallet::send_async(mcp::block_type const & type_a, boost::optional<mcp::block_hash> const & previous_a, 
	mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a, uint256_t const & gas_a, uint256_t const & gas_price_a,
	std::vector<uint8_t> const & data_a, boost::optional<std::string> const & password_a, std::function<void(mcp::send_result)> const & action_a,
	bool const & gen_next_work_a, bool const & async_a)
{
	this->queue_wallet_action([this, type_a, previous_a, from_a, to_a, amount_a, gas_a, gas_price_a, data_a, action_a, password_a, gen_next_work_a, async_a]() 
	{
		auto result(send_action(type_a, previous_a, from_a, to_a, amount_a, gas_a, gas_price_a, data_a, password_a, gen_next_work_a, async_a));
		action_a(result);
	});
}

mcp::send_result mcp::wallet::send_action(mcp::block_type const & type_a, boost::optional<mcp::block_hash> const & previous_a, 
	mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a, uint256_t gas_a, uint256_t gas_price_a, 
	std::vector<uint8_t> const & data_a, boost::optional<std::string> const & password_a, bool const & gen_next_work_a, bool const & async_a)
{
	mcp::db::db_transaction transaction(m_block_store.create_transaction());
	bool exists(m_key_manager->exists(from_a));
	if (!exists)
		return mcp::send_result(mcp::send_result_codes::from_not_exists, nullptr);

	mcp::raw_key prv;
	if (!password_a)
	{
		bool exists(m_key_manager->find_unlocked_prv(from_a, prv));
		if (!exists)
			return mcp::send_result(mcp::send_result_codes::account_locked, nullptr);
	}
	else
	{
		bool error(m_key_manager->decrypt_prv(from_a, *password_a, prv));
		if (error)
			return mcp::send_result(mcp::send_result_codes::wrong_password, nullptr);
	}

	mcp::compose_result compose_result(composer->compose_joint(transaction, type_a, previous_a, from_a, to_a, amount_a, gas_a, gas_price_a, data_a, prv, gen_next_work_a));
	return handle_compose_result(compose_result, async_a);
}

void mcp::wallet::send_async(std::shared_ptr<mcp::block> const p_block, mcp::signature const & signature_a, std::function<void(mcp::send_result)> const & action_a, bool gen_next_work_a, bool const & async_a)
{
	this->queue_wallet_action([this, p_block, signature_a, action_a, gen_next_work_a, async_a]()
	{
		auto result(send_action(p_block, signature_a, action_a, gen_next_work_a, async_a));
		action_a(result);
	});
}

mcp::send_result mcp::wallet::send_action(std::shared_ptr<mcp::block> const p_block, mcp::signature const & signature_a, std::function<void(mcp::send_result)> const & action_a, bool const & gen_next_work_a, bool const & async_a)
{
	mcp::compose_result compose_result(composer->sign_and_compose_joint(p_block, signature_a, gen_next_work_a));
	return handle_compose_result(compose_result, async_a);
}

mcp::send_result mcp::wallet::handle_compose_result(mcp::compose_result const & compose_result_a, bool const & async_a)
{
	std::shared_ptr<mcp::joint_message> joint;
	switch (compose_result_a.code)
	{
	case mcp::compose_result_codes::ok:
		joint = compose_result_a.joint;
		break;
	case mcp::compose_result_codes::insufficient_balance:
		return mcp::send_result(mcp::send_result_codes::insufficient_balance, nullptr);
	case mcp::compose_result_codes::data_size_too_large:
		return mcp::send_result(mcp::send_result_codes::data_size_too_large, nullptr);
	case mcp::compose_result_codes::validate_error:
		return mcp::send_result(mcp::send_result_codes::validate_error, nullptr);
	case mcp::compose_result_codes::dag_no_links:
		return mcp::send_result(mcp::send_result_codes::dag_no_links, nullptr);
	case mcp::compose_result_codes::error:
		return mcp::send_result(mcp::send_result_codes::error, nullptr);
	default:
		LOG(m_log.error) << "invalid compose result codes";
		return mcp::send_result(mcp::send_result_codes::error, nullptr);
	}

	std::shared_ptr<std::promise<mcp::validate_status>> p(std::make_shared<std::promise<mcp::validate_status>>());
	auto fut = p->get_future();
	std::shared_ptr<mcp::block_processor_item> item(std::make_shared<mcp::block_processor_item>(*joint, p));
	m_block_processor->add_to_mt_process(item);
	if (!async_a)
	{
		mcp::validate_status ret = fut.get();
		if (!ret.ok)
			return mcp::send_result(mcp::send_result_codes::validate_error, nullptr, ret.msg);
	}

	assert_x(joint);
	return mcp::send_result(mcp::send_result_codes::ok, joint->block);
}

void mcp::wallet::do_wallet_actions()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_stopped)
	{
		if (!m_actions.empty())
		{
			auto first(m_actions.front());
			auto current(std::move(first));
			m_actions.pop_front();
			lock.unlock();
			current();
			lock.lock();
		}
		else
		{
			m_condition.wait(lock);
		}
	}
}

void mcp::wallet::queue_wallet_action(std::function<void()> const & action_a)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_actions.push_back(std::move(action_a));
	m_condition.notify_all();
}

void mcp::wallet::stop()
{
	LOG(m_log.info) << "Wallet stopped";
	composer->stop();
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_stopped = true;
        m_condition.notify_all();
	}

	if (m_thread.joinable())
		m_thread.join();
}


mcp::send_result::send_result(mcp::send_result_codes const & code_a, std::shared_ptr<mcp::block> block_a, std::string const& msg_a):
	code(code_a),
	block(block_a),
	msg(msg_a)
{
}

