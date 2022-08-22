#include "approve_queue.hpp"
#include <thread>

namespace mcp
{
	using namespace std;
	using namespace dev;

	constexpr size_t c_maxVerificationQueueSizeApprove = 8192;

	ApproveQueue::ApproveQueue(
		mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a, std::shared_ptr<mcp::chain> chain_a,
		std::shared_ptr<mcp::async_task> async_task_a
	):
		m_store(store_a),
		m_cache(cache_a),
		m_chain(chain_a),
		m_async_task(async_task_a),
		m_dropped(100000)
	{
		unsigned verifierThreads = std::max(thread::hardware_concurrency(), 3U) - 2U;
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

	ImportApproveResult ApproveQueue::check_WITH_LOCK(h256 const& _h, IfDropped _ik)
	{
		if (m_known.count(_h) )
			return ImportApproveResult::AlreadyKnown;

		mcp::db::db_transaction t(m_store.create_transaction());
		if (_ik == IfDropped::Ignore && (m_dropped.contains(_h) || m_cache->approve_exists(t, _h)))
			return ImportApproveResult::AlreadyInChain;

		return ImportApproveResult::Success;
	}

	ImportApproveResult ApproveQueue::manageImport_WITH_LOCK(h256 const& _h, approve const& _approve, bool isLocal)
	{
		try
		{
			assert(_h == _approve.sha3());
			
			m_known.insert(_h);
			if(m_current.find(_approve.m_epoch) == m_current.end()){
				m_current.insert(std::make_pair(_approve.m_epoch, std::unordered_map<h256, approve>()));
			}
			m_current[_approve.m_epoch].insert(std::make_pair(_h, _approve));
		}
		catch (Exception const& _e)
		{
			LOG(m_log.debug) << "Ignoring invalid approve: " << diagnostic_information(_e);
			return ImportApproveResult::Malformed;
		}
		catch (std::exception const& _e)
		{
			LOG(m_log.debug) << "Ignoring invalid approve: " << _e.what();
			return ImportApproveResult::Malformed;
		}

		return ImportApproveResult::Success;
	}

	bool ApproveQueue::remove_WITH_LOCK(h256 const& _txHash, uint64_t _epoch)
	{
		if(m_current.find(_epoch) == m_current.end()){
			return false;
		}

		auto t = m_current.at(_epoch).find(_txHash);
		if (t == m_current.at(_epoch).end())
			return false;

		m_current.at(_epoch).erase(t);
		if(m_current[_epoch].empty()){
			m_current.erase(m_current.find(_epoch));
		}
		m_known.erase(_txHash);
		
		return true;
	}


	ImportApproveResult ApproveQueue::import(approve const& _approve, bool isLoccal, IfDropped _ik)
	{
		LOG(m_log.trace) << "[import] in";

		// Check if we already know this approve.
		h256 h = _approve.sha3();

		ImportApproveResult ret;
		{
			UpgradableGuard l(m_lock);
			auto ir = check_WITH_LOCK(h, _ik);
			if (ir != ImportApproveResult::Success)
				return ir;

			{
				UpgradeGuard ul(l);
				ret = manageImport_WITH_LOCK(h, _approve, isLoccal);
			}
		}

		if (ImportApproveResult::Success == ret)/// first import && successed,broadcast it
		{
			m_onImportProcessed(h);
		}

		LOG(m_log.debug) << "[import] out";
		return ret;
	}

	void ApproveQueue::importLocal(approve const& _approve)
	{
		LOG(m_log.trace) << "[importLocal] in";
		auto ret = import(_approve,true);
		if(ret != ImportApproveResult::Success)
		{
			LOG(m_log.info) << "[importLocal] fail with ret = " << (uint32_t)ret;
			return;
		}
		
		m_async_task->sync_async([this, _approve]() {
			m_capability->broadcast_approve(_approve);
		});
	}

	void ApproveQueue::drop(std::map<uint64_t, h256s> const& _mapHashs)
	{
		try
		{
			UpgradableGuard l(m_lock);
			for(auto hashs : _mapHashs){
				h256s dels;
				auto _txHashs = hashs.second;
				auto epoch = hashs.first;
				for (auto h : _txHashs)
				{
					if (m_known.count(h))
					{
						dels.push_back(h);
					}
				}

				UpgradeGuard ul(l);
				for (auto h : dels)
				{
					m_dropped.insert(h, true /* placeholder value */);
					remove_WITH_LOCK(h, epoch);
				}
			}
		}
		catch(const std::exception& e)
		{
			LOG(m_log.error) << "[drop] error: " << e.what();
			assert_x(false);
		}
	}

	void ApproveQueue::dropObsolete(uint64_t _cur_epoch)
	{
		try
		{
			UpgradableGuard l(m_lock);
			for(auto it=m_current.begin(); it!=m_current.end();){
				auto epoch = it->first;
				if(it->first >= _cur_epoch)
				{
					return;
				}
				LOG(m_log.debug) << "[dropObsolete] drop epoch" << epoch << " size="<<it->second.size();
				h256s dels;
				for(auto ap : it->second)
				{
					h256 h = ap.first;
					if (m_known.count(h))
					{
						dels.push_back(h);
					}
				}

				UpgradeGuard ul(l);
				for (auto h : dels)
				{
					m_dropped.insert(h, true /* placeholder value */);
					remove_WITH_LOCK(h, epoch);
				}
				if(m_current.find(epoch) != m_current.end()){
					m_current.erase(epoch);
				}
				it = m_current.begin();
			}
		}
		catch(const std::exception& e)
		{
			LOG(m_log.error) << "[dropObsolete] fail in epoch " << _cur_epoch << " error: " << e.what();
			assert_x(false);
		}
	}

	std::shared_ptr<approve> ApproveQueue::get(h256 const& _txHash, uint64_t _epoch) const
	{
		UpgradableGuard l(m_lock);
		if(m_current.find(_epoch) == m_current.end()){
			return nullptr;
		}

		auto t = m_current.at(_epoch).find(_txHash);
		if (t == m_current.at(_epoch).end())
			return nullptr;

		return std::make_shared<approve>(t->second);
	}

	std::shared_ptr<approve> ApproveQueue::get(h256 const& _txHash) const
	{
		UpgradableGuard l(m_lock);
		if(m_current.empty()){
			return nullptr;
		}

		for(auto hashs : m_current){
			auto t = hashs.second.find(_txHash);
			if (t != hashs.second.end())
				return std::make_shared<approve>(t->second);
		}
		return nullptr;
	}

	h256s ApproveQueue::topApproves(unsigned _limit, h256Hash const& _avoid) const
	{
		ReadGuard l(m_lock);
		h256s ret;
		for(auto hashs : m_current)
		{
			for (auto cs = hashs.second.begin(); cs != hashs.second.end(); ++cs)
			{
				auto hash = cs->first;
				if (!_avoid.count(hash))
					ret.push_back(hash);
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
		if(m_current.find(_epoch) == m_current.end()) return ret;
		for (auto cs = m_current.at(_epoch).begin(); cs != m_current.at(_epoch).end(); ++cs)
		{
			auto hash = cs->first;
			if (!_avoid.count(hash))
				ret.push_back(hash);
				if (ret.size() == _limit)
					return ret;
		}
			
		return ret;
	}

	bool ApproveQueue::exist(h256 const& _hash)
	{
		ReadGuard l(m_lock);
		if(m_current.empty()) return false;
		for(auto hashs : m_current)
		{
			if(hashs.second.find(_hash) != hashs.second.end()){
				return true;
			}
		}
		return false;
	}

	h256Hash ApproveQueue::knownApproves() const
	{
		ReadGuard l(m_lock);
		return m_known;
	}

	void ApproveQueue::enqueue(RLP const& _data, p2p::node_id const& _nodeId)
	{
		bool queued = false;
		{
			Guard l(x_queue);
			if (m_unverified.size() >= c_maxVerificationQueueSizeApprove)
			{
				LOG(m_log.debug) << "Approve verification queue is full. Dropping approve";
				return;
			}
			m_unverified.emplace_back(UnverifiedApprove(_data.data(), _nodeId));
			queued = true;
		}
		
		m_queueReady.notify_all();
	}

	void ApproveQueue::verifierBody()
	{
		while (!m_aborting)
		{
			UnverifiedApprove work;

			{
				unique_lock<Mutex> l(x_queue);
				m_queueReady.wait(l, [&]() { return !m_unverified.empty() || m_aborting; });
				if (m_aborting)
					return;
				work = move(m_unverified.front());
				m_unverified.pop_front();
			}

			try
			{
				approve t(work.m_approve, CheckTransaction::Everything);
				ImportApproveResult vr = validateApprove(t);
				if(ImportApproveResult::Success != vr){
					m_onImport(vr, t.sha3(), work.nodeId);
					continue;
				}

				ImportApproveResult ir = import(t,false);
				m_onImport(ir, t.sha3(), work.nodeId);

				if (ImportApproveResult::Success == ir)/// first import && successed,broadcast it
				{
					m_async_task->sync_async([this, t]() {
						m_capability->broadcast_approve(t);
					});
				}
			}
			catch (...)
			{
				m_onImport(ImportApproveResult::BadChain, h256(0), work.nodeId);///  Notify capability and P2P to process peer. diconnect peer if bad transaction  
				// should not happen as exceptions are handled in import.
				LOG(m_log.error) << "Bad approve:" << boost::current_exception_diagnostic_information();
			}
		}
	}

	ImportApproveResult ApproveQueue::validateApprove(approve const& _t){
		_t.checkChainId(mcp::chain_id);
		_t.checkLowS();
		
		mcp::db::db_transaction transaction(m_store.create_transaction());
		mcp::block_hash hash;
		if(_t.m_epoch <= 2){
			hash = mcp::block_hash(0);
		}
		else{
			bool ret = m_store.stable_block_get(transaction, (_t.m_epoch-2)*epoch_period, hash);
			if(ret){
				LOG(m_log.info) << "[validateApprove] epoch is too high";
				return ImportApproveResult::EpochIsTooHigh;
			}
		}
		
		std::vector<uint8_t> output;
		_t.vrf_verify(output, hash.hex());
		return ImportApproveResult::Success;
	}

	size_t ApproveQueue::size(uint64_t _epoch)
	{
		if(m_current.find(_epoch) == m_current.end())
		{
			return 0;
		}
		return m_current[_epoch].size();
	}

	std::string ApproveQueue::getInfo()
	{
		UpgradableGuard l(m_lock);
		std::string str = "ApproveQueue current:" + std::to_string(m_current.size())
			+ " ,m_unverified:" + std::to_string(m_unverified.size())
			+ " ,m_known:" + std::to_string(m_known.size())
			+ " ,m_dropped:" + std::to_string(m_dropped.size());
		if(m_current.size() > 0){
			str += " current[";
			for(auto current : m_current){
				str += " epoch" + std::to_string(current.first) + " size=" + std::to_string(current.second.size()); 
			}
			str += " ]";
		}
		

		return str;
	}
}
