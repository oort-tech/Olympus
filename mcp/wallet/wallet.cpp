#include "wallet.hpp"
#include <libdevcore/Common.h>
#include <mcp/node/composer.hpp>
#include <future>

////test
#include <libdevcore/CommonJS.h>


mcp::wallet::wallet(
	mcp::block_store& block_store_a, std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::key_manager> key_manager_a,
	std::shared_ptr<TransactionQueue> tq
) :
	m_block_store(block_store_a),
	m_cache(cache_a),
	m_key_manager(key_manager_a),
	m_tq(tq),
	m_stopped(false),
	m_thread([this]() { do_wallet_actions(); })
{
}

void mcp::wallet::send_async(TransactionSkeleton t, std::function<void(h256 &, boost::optional<dev::Exception const &>)> const & action_a, boost::optional<std::string> const & password)
{
	this->queue_wallet_action([this, t, action_a, password]()// put queue
	{
		try {
			h256 h = send_action(t, password);
			action_a(h, boost::none);
		}
		catch (dev::Exception const & e) {
			h256 h = h256(0);
			action_a(h, e);
		}
	});
}

h256 mcp::wallet::send_action(TransactionSkeleton t, boost::optional<std::string> const & password)
{
	std::pair<bool, Secret> ar = m_key_manager->authenticate(t.from, password);
	if (!ar.first)
	{
		populateTransactionWithDefaults(t);
		t.from = dev::toAddress(ar.second);
		Transaction ts(t, ar.second);
		return importTransaction(ts);
	}
	else
	{
		h256 emptyHash;
		return emptyHash; // TODO: give back something more useful than an empty hash.
	}
}

h256 mcp::wallet::importTransaction(Transaction const& _t)
{
	// Use the Executive to perform basic validation of the transaction
	// (e.g. transaction signature, account balance) using the state of
	// the pending block. This can throw but we'll catch the exception at the RPC level.
	//try
	//{
	//	
	//}
	//catch (InvalidNonce const& e)
	//{
	//	// Too low nonce is invalid for sure
	//	bigint const& req = *boost::get_error_info<errinfo_required>(e);
	//	bigint const& got = *boost::get_error_info<errinfo_got>(e);
	//	if (req > got)
	//		throw;

	//	// Checking against pending block doesn't take into account transactions from the same
	//	// sender, that are currently in Transaction Queue.
	//	// If nonce is too high, it could be that previous transactions are in TQ.
	//	// We'll let TQ deal with nonces, it will order pending transactions by nonce.
	//}

	ImportResult res = m_tq->importLocal(_t);
	switch (res)
	{
	case ImportResult::Success:
		break;
	case ImportResult::OverbidGasPrice:
		BOOST_THROW_EXCEPTION(GasPriceTooLow());
	case ImportResult::AlreadyKnown:
		BOOST_THROW_EXCEPTION(PendingTransactionAlreadyExists());
	case ImportResult::AlreadyInChain:
		BOOST_THROW_EXCEPTION(TransactionAlreadyInChain());
	case ImportResult::FutureTimeKnown:
		BOOST_THROW_EXCEPTION(PendingTransactionTooMuch());
	default:
		BOOST_THROW_EXCEPTION(UnknownTransactionValidationError());
	}

	return _t.sha3();
}

u256 mcp::wallet::getTransactionCount(Address const& from)
{
	mcp::db::db_transaction transaction(m_block_store.create_transaction());
	u256 tqNonce = m_tq->maxNonce(from);
	u256 accNonce = 0;
	if (!m_cache->account_nonce_get(transaction, from, accNonce))
		return tqNonce;
	accNonce++; /// next nonce
	return std::max<u256>(accNonce, tqNonce);
}

void mcp::wallet::populateTransactionWithDefaults(TransactionSkeleton& _t)
{
	// Default gas value meets the intrinsic gas requirements of both
	// send value and create contract transactions and is the same default
	// value used by geth and testrpc.
	//mcp::db::db_transaction transaction(m_block_store.create_transaction());
	//u256 ret = 0;
	//auto state = m_cache->latest_account_state_get(transaction, _t.from);
	//if (state)
	//{
	//	ret = state->nonce();
	//}

	const u256 defaultTransactionGas = 21000;
	if (_t.nonce == Invalid256)
		_t.nonce = getTransactionCount(_t.from);
	if (_t.gasPrice == Invalid256)
		_t.gasPrice = mcp::gas_price;
	if (_t.gas == Invalid256)
		_t.gas = defaultTransactionGas;

	//return ret;
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
	//composer->stop();
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_stopped = true;
        m_condition.notify_all();
	}

	if (m_thread.joinable())
		m_thread.join();
}


