#include "approve_queue.hpp"
#include <mcp/core/genesis.hpp>
#include <thread>

namespace mcp
{
	using namespace std;
	using namespace dev;

	constexpr size_t c_maxVerificationQueueSizeApprove = 8192;

	ApproveQueue::ApproveQueue(
		mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
		std::shared_ptr<mcp::async_task> async_task_a
	):
		m_store(store_a),
		m_cache(cache_a),
		m_async_task(async_task_a),
		m_dropped(1000)
	{
		unsigned verifierThreads = std::max(thread::hardware_concurrency()/2, 3U) - 2U;
		for (unsigned i = 0; i < verifierThreads; ++i)
			m_verifiers.emplace_back([=]() {
			setThreadName("approveCheck" + toString(i));
			this->verifierBody();
		});
	}

	ApproveQueue::~ApproveQueue()
	{
		DEV_GUARDED(x_queue)
			m_aborting = true;
		m_queueReady.notify_all();
		for (auto& i : m_verifiers)
			i.join();
	}

	ImportResult ApproveQueue::check_WITH_LOCK(h256 const& _h)
	{
		if (m_known.count(_h) )
			return ImportResult::AlreadyKnown;

		if (m_dropped.contains(_h))
			return ImportResult::AlreadyInChain;
		mcp::db::db_transaction t(m_store.create_transaction());
		if (m_cache->approve_exists(t, _h))
			return ImportResult::AlreadyInChain;

		return ImportResult::Success;
	}

	ImportResult ApproveQueue::manageImport_WITH_LOCK(std::shared_ptr<approve> _approve)
	{
		try
		{	
			if (all.count(_approve->sha3()))
				assert_x(false);

			if (!queue.count(_approve->epoch()))
				queue[_approve->epoch()] = h256Hash();
			queue[_approve->epoch()].insert(_approve->sha3());
			all.insert(std::make_pair(_approve->sha3(), _approve));
			m_known.insert(_approve->sha3());

			m_onReady(_approve->sha3());
		}
		catch (Exception const& _e)
		{
			LOG(m_log.debug) << "Ignoring invalid approve: " << diagnostic_information(_e);
			return ImportResult::Malformed;
		}
		catch (std::exception const& _e)
		{
			LOG(m_log.debug) << "Ignoring invalid approve: " << _e.what();
			return ImportResult::Malformed;
		}

		return ImportResult::Success;
	}

	bool ApproveQueue::remove_WITH_LOCK(h256 const& _txHash)
	{
		auto ap = all.find(_txHash);
		if (ap == all.end())
		{
			LOG(m_log.debug) << "remove_WITH_LOCK Approve hash" << _txHash.hex() << "already in all?!";
			return false;
		}
		auto epoch = ap->second->epoch();
		if (queue.count(epoch))
		{
			queue[epoch].erase(_txHash);
			if (queue[epoch].empty())
				queue.erase(epoch);
		}

		all.erase(_txHash);
		m_known.erase(_txHash);
		
		return true;
	}


	ImportResult ApproveQueue::import(std::shared_ptr<approve> _approve, source _in)
	{
		//LOG(m_log.trace) << "[import] in";

		// Check if we already know this approve.
		h256 h = _approve->sha3();

		ImportResult ir;
		{
			UpgradableGuard l(m_lock);
			ir = check_WITH_LOCK(h);
			if (ir != ImportResult::Success)
				return ir;

			ir = validateApprove(*_approve);
			if (ir != ImportResult::Success)
				return ir;
			{
				UpgradeGuard ul(l);
				ir = manageImport_WITH_LOCK(_approve);
			}
		}

		if (ImportResult::Success == ir)/// first import && successed,broadcast it
		{
			m_async_task->sync_async([this, _approve]() {
				m_capability->broadcast_approve(*_approve);
			});
		}

		return ir;
	}

	void ApproveQueue::importLocal(std::shared_ptr<approve> _approve)
	{
		//LOG(m_log.trace) << "[importLocal] in";
		auto ret = import(_approve, source::local);
		if(ret != ImportResult::Success)
		{
			LOG(m_log.info) << "[importLocal] fail with ret = " << (uint32_t)ret;
			return;
		}
	}

	void ApproveQueue::drop(h256s const& _txHashs)
	{
		UpgradableGuard l(m_lock);
		h256s dels;
		for (auto h : _txHashs)
		{
			///if not known,must be deleted
			if (m_known.count(h))
			{
				dels.push_back(h);
			}
		}

		UpgradeGuard ul(l);
		for (auto h : dels)
		{
			m_dropped.insert(h, true /* placeholder value */);
			remove_WITH_LOCK(h);
		}
	}

	std::shared_ptr<approve> ApproveQueue::get(h256 const& _txHash) const
	{
		UpgradableGuard l(m_lock);
		auto t = all.find(_txHash);
		if (t == all.end())
			return nullptr;

		return t->second;
		return nullptr;
	}

	h256s ApproveQueue::topApproves(unsigned _limit, h256Hash const& _avoid) const
	{
		ReadGuard l(m_lock);
		h256s ret;
		for(auto hashs : queue)
		{
			for (auto cs = hashs.second.begin(); cs != hashs.second.end(); ++cs)
			{
				if (!_avoid.count(*cs))
					ret.push_back(*cs);
					if (ret.size() == _limit)
						return ret;
			}
		}
		return ret;
	}

	h256s ApproveQueue::topApproves(unsigned _limit, uint64_t _epoch, h256Hash const& _avoid) const
	{
		ReadGuard l(m_lock);
		h256s ret;
		if(queue.find(_epoch) == queue.end()) return ret;
		for (auto cs = queue.at(_epoch).begin(); cs != queue.at(_epoch).end(); ++cs)
		{
			if (!_avoid.count(*cs))
				ret.push_back(*cs);
				if (ret.size() == _limit)
					return ret;
		}
			
		return ret;
	}

	bool ApproveQueue::exist(h256 const& _hash)
	{
		ReadGuard l(m_lock);
		return all.count(_hash);
	}

	h256Hash ApproveQueue::knownApproves() const
	{
		ReadGuard l(m_lock);
		return m_known;
	}

	void ApproveQueue::enqueue(std::shared_ptr<approve> _ap, p2p::node_id const& _nodeId, source _in)
	{
		bool queued = false;
		{
			Guard l(x_queue);
			if (m_unverified.size() >= c_maxVerificationQueueSizeApprove)
			{
				LOG(m_log.debug) << "Approve verification queue is full. Dropping approve";
				return;
			}
			m_unverified.emplace_back(UnverifiedApprove(_ap, _nodeId, _in));
			queued = true;
		}
		
		if (queued)
			m_queueReady.notify_all();
	}

	void ApproveQueue::verifierBody()
	{
		while (!m_aborting)
		{
			std::deque<UnverifiedApprove> works;

			{
				unique_lock<Mutex> l(x_queue);
				m_queueReady.wait(l, [&]() { return !m_unverified.empty() || m_aborting; });
				if (m_aborting)
					return;
				std::swap(works, m_unverified);
			}

			while (!works.empty())
			{
				UnverifiedApprove work = std::move(works.front());
				try
				{
					auto ir = import(work.ap, work.in);
					m_onImport(ir, work.nodeId);
				}
				catch (InvalidNonce)
				{
					///remote network is not very good,Disconnect the peer.todo
				}
				catch (...)
				{
					LOG(m_log.error) << "Bad approve:" << boost::current_exception_diagnostic_information();
					m_onImport(ImportResult::Malformed, work.nodeId);///  Notify capability and P2P to process peer. diconnect peer if bad transaction 
				}
				
				works.pop_front();
			}
		}
	}

	ImportResult ApproveQueue::validateApprove(approve const& _t){
		_t.checkChainId(mcp::chain_id);
		_t.checkLowS();
		
		mcp::db::db_transaction transaction(m_store.create_transaction());
		mcp::block_hash hash;
		if(_t.epoch() <= 1){
			hash = mcp::genesis::block_hash;
		}
		else{
			bool ret = m_store.main_chain_get(transaction, (_t.epoch()-1)*epoch_period, hash);
			if(ret){
				LOG(m_log.debug) << "[validateApprove] epoch is too high";
				//LOG(m_log.debug) << "[validateApprove] hash=" << hash.hex();
				return ImportResult::EpochIsTooHigh;
			}
		}
		
		_t.vrf_verify(hash);
		return ImportResult::Success;
	}

	std::string ApproveQueue::getInfo()
	{
		UpgradableGuard l(m_lock);
		std::string str = "ApproveQueue all:" + std::to_string(all.size())
			+ " ,m_unverified:" + std::to_string(m_unverified.size())
			+ " ,m_known:" + std::to_string(m_known.size())
			+ " ,m_dropped:" + std::to_string(m_dropped.size());
		if(queue.size() > 0){
			str += " current[";
			for(auto current : queue){
				str += " epoch:" + std::to_string(current.first) + " size=" + std::to_string(current.second.size()); 
			}
			str += " ]";
		}
		

		return str;
		return "";
	}
}
