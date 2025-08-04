#include "chain_state.hpp"
#include "Block.hpp"
#include "chain.hpp"
#include <mcp/node/evm/Executive.hpp>
#include <mcp/common/Exceptions.h>
//#include <mcp/common/stopwatch.hpp>
using namespace std;

mcp::chain_state::chain_state(/*mcp::db::db_transaction& transaction_a, */u256 const& _accountStartNonce, /*mcp::block_store& store_a,*/
	/*std::shared_ptr<mcp::chain> chain_a,*/ /*std::shared_ptr<mcp::iblock_cache> cache_a,*/ OverlayDB const& _db, BaseState _bs):
    //transaction(transaction_a),
    //store(store_a),
	//chain(chain_a),
    //block_cache(cache_a),
    m_db(_db),
    m_state(&m_db),
    m_accountStartNonce(/*_accountStartNonce*/0)
{
    if (_bs != BaseState::PreExisting)
        // Initialise to the state entailed by the genesis block; this guarantees the trie is built correctly.
        m_state.init();
}

mcp::chain_state::chain_state(chain_state const& _s) :
    m_db(_s.m_db),
    m_state(&m_db, _s.m_state.root(), Verification::Skip),
    m_cache(_s.m_cache),
    m_unchangedCacheEntries(_s.m_unchangedCacheEntries),
    m_nonExistingAccountsCache(_s.m_nonExistingAccountsCache),
    m_touched(_s.m_touched),
    //m_unrevertablyTouched(_s.m_unrevertablyTouched),
    m_accountStartNonce(_s.m_accountStartNonce)
{
}

mcp::chain_state& mcp::chain_state::operator=(chain_state const& _s)
{
    if (&_s == this)
        return *this;

    m_db = _s.m_db;
    m_state.open(&m_db, _s.m_state.root(), Verification::Skip);
    m_cache = _s.m_cache;
    m_unchangedCacheEntries = _s.m_unchangedCacheEntries;
    m_nonExistingAccountsCache = _s.m_nonExistingAccountsCache;
    m_touched = _s.m_touched;
    //m_unrevertablyTouched = _s.m_unrevertablyTouched;
    m_accountStartNonce = _s.m_accountStartNonce;
    return *this;
}

void mcp::chain_state::incNonce(Address const& _addr)
{
    if (std::shared_ptr<mcp::account_state> a = account(_addr))
    {
        auto oldNonce = a->nonce();
        a->incNonce();
        m_changeLog.emplace_back(_addr, oldNonce);
    }
    else
        // This is possible if a transaction has gas price 0.
        //createAccount(_addr, std::make_shared<mcp::account_state>(_addr, ts.sha3(), h256(0), requireAccountStartNonce() + 1, 0));
        createAccount(_addr, std::make_shared<mcp::account_state>(requireAccountStartNonce() + 1, 0));
}

void mcp::chain_state::setNonce(Address const& _addr, mcp::uint256_t const& _newNonce)
{
    if (std::shared_ptr<mcp::account_state> a = account(_addr))
    {
        auto oldNonce = a->nonce();
        a->setNonce(_newNonce);
        m_changeLog.emplace_back(_addr, oldNonce);
    }
    else
        // This is possible when a contract is being created.
        //createAccount(_addr, std::make_shared<mcp::account_state>(_addr, ts.sha3(), h256(0), _newNonce, 0));
        createAccount(_addr, std::make_shared<mcp::account_state>(_newNonce, 0));
}

mcp::uint256_t mcp::chain_state::getNonce(Address const& _addr) const
{
    if (std::shared_ptr<mcp::account_state> a = account(_addr))
        return a->nonce();
    else
        return m_accountStartNonce;
}

u256 const& mcp::chain_state::requireAccountStartNonce() const
{
    if (m_accountStartNonce == Invalid256)
        BOOST_THROW_EXCEPTION(InvalidAccountStartNonceInState());
    return m_accountStartNonce;
}

void mcp::chain_state::noteAccountStartNonce(u256 const& _actual)
{
    if (m_accountStartNonce == Invalid256)
        m_accountStartNonce = _actual;
    else if (m_accountStartNonce != _actual)
        BOOST_THROW_EXCEPTION(IncorrectAccountStartNonceInState());
}

std::shared_ptr<mcp::account_state> mcp::chain_state::account(Address const& _addr) const
{
    // If the account is already modified, return immediately
    auto it = m_cache.find(_addr);
	if (it != m_cache.end())
		return it->second;  
    
    // If the account doesn't exist, return nullptr
    if (m_nonExistingAccountsCache.count(_addr))
        return nullptr;

    //// Populate basic info.
    //std::shared_ptr<mcp::account_state> as(block_cache->latest_account_state_get(transaction, _addr));
    //if (!as)
    //{
    //    m_nonExistingAccountsCache.insert(_addr);
    //    return nullptr;
    //}

    // Populate basic info.
    std::string stateBack = m_state.at(_addr);
    if (stateBack.empty())
    {
        m_nonExistingAccountsCache.insert(_addr);
        return nullptr;
    }

    clearCacheIfTooLarge();

    RLP state(stateBack);
    auto const nonce = state[0].toInt<u256>();
    auto const balance = state[1].toInt<u256>();
    auto const storageRoot = state[2].toHash<h256>();
    auto const codeHash = state[3].toHash<h256>();
    // version is 0 if absent from RLP
    auto const version = state[4] ? state[4].toInt<u256>() : 0;

    auto acco = std::make_shared<account_state>(nonce, balance, storageRoot, codeHash, version, account_state::Unchanged);
    auto i = m_cache.emplace(_addr, acco);
    //auto i = m_cache.emplace(piecewise_construct, forward_as_tuple(_addr),
    //    forward_as_tuple(nonce, balance, storageRoot, codeHash, version, account_state::Unchanged));
     
	//std::shared_ptr<mcp::account_state> as_copy(std::make_shared<mcp::account_state>(*as));
    //auto i = m_cache.emplace(_addr, as_copy);
    m_unchangedCacheEntries.push_back(_addr);
    return i.first->second;
}

void mcp::chain_state::clearCacheIfTooLarge() const
{
}

std::pair<mcp::ExecutionResult, dev::eth::TransactionReceipt> mcp::chain_state::execute(dev::eth::EnvInfo const& _envInfo, SealEngineFace const& _sealEngine, Permanence _p, mcp::Transaction const& _t/*, dev::eth::OnOpFunc const& _onOp*/)
{
	Executive e(*this, _envInfo, _sealEngine/*, traces*/);
    ExecutionResult res;
    e.setResultRecipient(res);

	//ts = _t;

    //auto onOp = _onOp;
//#if ETH_VMTRACE
//    if (!onOp)
//        onOp = e.simpleTrace();
//#endif
	
	uint256_t const startGasUsed = 0; // Only one transaction in a block, start gas is always zero 
	
	bool statusCode = executeTransaction(e, _t/*, onOp*/);

	switch (_p)
	{
	case Permanence::Reverted:
		m_cache.clear();
		break;
	case Permanence::Committed:
		for (auto const& i : m_cache)
		{
			res.modified_accounts.insert(i.first);
		}
		commit(); // Remove empty accounts
		break;
	case Permanence::Uncommitted:
		break;
	}

	TransactionReceipt const receipt = TransactionReceipt(statusCode, startGasUsed + e.gasUsed(), e.logs(), _envInfo.mci());
	return std::make_pair(res, receipt);
}

void mcp::chain_state::executeBlockTransactions(Block const& _block, unsigned _txCount, SealEngineFace const& _sealEngine)
{
    u256 gasUsed = 0;
    for (unsigned i = 0; i < _txCount; ++i)
    {
        EnvInfo envInfo(_block.info(), mcp::chainID());
        Executive e(*this, envInfo, _sealEngine);
        executeTransaction(e, _block.pending()[i]/*, OnOpFunc()*/);
        gasUsed += e.gasUsed();
    }
}

bool mcp::chain_state::addressInUse(Address const& _address) const
{
    return !!account(_address);
}

/// @returns true when normally halted; false when exceptionally halted; throws when internal VM
/// exception occurred.
bool mcp::chain_state::executeTransaction(Executive& _e, mcp::Transaction const& _t/*, dev::eth::OnOpFunc const& _onOp*/)
{
	size_t const savept = savepoint();
	try
	{
		_e.initialize(_t);

		if (!_e.execute())
			_e.go(/*_onOp*/);
		return _e.finalize();
	}
    catch (dev::eth::NotEnoughCash const&)
    {
        ///do not throw, used the transaction nonce
        LOG(m_log.debug) << "executeTransaction not enough cash,hash: " << _t.sha3().hexPrefixed()
            << ", from: " << _t.sender().hexPrefixed()
            << ", to: " << _t.to().hexPrefixed()
            << ", value: " << _t.value();
        return false;
    }
	catch (Exception const&)
	{
		rollback(savept);
		throw;
	}
}

size_t mcp::chain_state::savepoint() const
{
	return m_changeLog.size();
}

void mcp::chain_state::rollback(size_t _savepoint)
{
	while (_savepoint != m_changeLog.size())
    {
        auto& change = m_changeLog.back();
        std::shared_ptr<mcp::account_state> account = m_cache[change.address];

        // Public State API cannot be used here because it will add another
        // change log entry.
        switch (change.kind)
        {
        case Change::Storage:
            account->setStorage(change.key, change.value);
            break;
        case Change::StorageRoot:
            account->setStorageRoot(change.value);
            break;
        case Change::Balance:
            account->addBalance(0 - change.value);
            break;
        case Change::Nonce:
            account->setNonce(change.value);
            break;
        case Change::Create:
            m_cache.erase(change.address);
            break;
        case Change::Code:
            account->setCode(std::move(change.oldCode));
            break;
        case Change::Touch:
            account->untouch();
            m_unchangedCacheEntries.emplace_back(change.address);
            break;
        }
        m_changeLog.pop_back();
    }
}

h256 mcp::chain_state::blockHash(u256 _number)
{
    auto _prefix = static_cast<uint8_t>(StorePrefix::stable_block);
    auto _k = mcp::h64_to_slice(dev::h64(uint64_t(_number)));
    dev::Slicebytes key = dev::Slicebytes(1, _prefix) + _k;

    auto result = m_db.db()->lookup(dev::Slice(key.data(), key.size()));
    return mcp::slice_to_h256(result);
}

void mcp::chain_state::commit()
{
    //if (ts.isCreation() || addressHasCode(ts.receiveAddress()))
    //{
    //    save_previous_account_state();
    //}
    removeEmptyAccounts();
	//std::shared_ptr<mcp::process_block_cache> process_block_cache = std::dynamic_pointer_cast<mcp::process_block_cache>(block_cache);
    //m_touched += mcp::commit(transaction, m_cache, &m_db, process_block_cache, store, ts.sha3());
    m_touched += mcp::commit(m_cache, m_state);
    m_changeLog.clear();
    m_cache.clear();
    m_unchangedCacheEntries.clear();

	//save traces
	//store.traces_put(transaction, ts.sha3(), traces);
	//traces.clear();
}

void mcp::chain_state::setRoot(h256 const& _root)
{
    m_cache.clear();
    m_unchangedCacheEntries.clear();
    m_nonExistingAccountsCache.clear();
    //  m_touched.clear();
    m_state.setRoot(_root);
}

void mcp::chain_state::removeEmptyAccounts()
{
    for (auto& i: m_cache)
        if (i.second->isDirty() && i.second->isEmpty())
            i.second->kill();
}

//void mcp::chain_state::save_previous_account_state()
//{
//    std::vector<h256> hs;
//    for(auto& address : m_unchangedCacheEntries)
//    {
//        h256 hash;
//        bool exists = !store.latest_account_state_get(transaction, address, hash);
//        if (exists)
//        {
//            hs.emplace_back(hash);
//        }
//    }
//    store.transaction_previous_account_state_put(transaction, ts.sha3(), hs);
//}

bool mcp::chain_state::addressHasCode(Address const& _id) const
{
	std::shared_ptr<mcp::account_state> a = account(_id);
    if (a)
        return a->codeHash() != EmptySHA3;
    else
        return false;
}


mcp::uint256_t mcp::chain_state::balance(Address const& _id) const
{
	std::shared_ptr<mcp::account_state> a = account(_id);
    if (a)
        return a->balance();
    else
        return 0;
}

void mcp::chain_state::addBalance(Address const& _id, uint256_t const& _amount)
{
	std::shared_ptr<mcp::account_state> a = account(_id);
    if (a)
    {
        // Log empty account being touched. Empty touched accounts are cleared
        // after the transaction, so this event must be also reverted.
        // We only log the first touch (not dirty yet), and only for empty
        // accounts, as other accounts does not matter.
        // TODO: to save space we can combine this event with Balance by having
        //       Balance and Balance+Touch events.
        if (!a->isDirty() && a->isEmpty())
            m_changeLog.emplace_back(Change::Touch, _id);

        // Increase the account balance. This also is done for value 0 to mark
        // the account as dirty. Dirty account are not removed from the cache
        // and are cleared if empty at the end of the transaction.
        a->addBalance(_amount);
    }
    else					
        //createAccount(_id, std::make_shared<mcp::account_state>(_id, ts.sha3(), h256(0), requireAccountStartNonce(), _amount));
        createAccount(_id, std::make_shared<mcp::account_state>(requireAccountStartNonce(), _amount));

    if (_amount)
        m_changeLog.emplace_back(Change::Balance, _id, _amount);
}
    
void mcp::chain_state::subBalance(Address const& _id, uint256_t const& _amount)
{
    if (_amount == 0)
        return;

	std::shared_ptr<mcp::account_state> a = account(_id);
    if (!a || a->balance() < _amount)
        // TODO: I expect this never happens.
        BOOST_THROW_EXCEPTION(dev::eth::NotEnoughCash());

    // Fall back to addBalance().
    addBalance(_id, 0 - _amount);
}

void mcp::chain_state::setBalance(Address const& _id, uint256_t const& _value)
{
	std::shared_ptr<mcp::account_state> a = account(_id);
	u256 original = a ? a->balance() : 0;

	// Fall back to addBalance().
	addBalance(_id, _value - original);
}

void mcp::chain_state::transferBalance(Address const& _from, Address const& _to, uint256_t const& _value)
{
    subBalance(_from, _value);
    addBalance(_to, _value);
}

void mcp::chain_state::createAccount(Address const& _address, std::shared_ptr<mcp::account_state> _account)
{
    assert_x(!addressInUse(_address) && "Account already exists");
    m_cache[_address] = _account;
    m_nonExistingAccountsCache.erase(_address);
    m_changeLog.emplace_back(Change::Create, _address);
}

void mcp::chain_state::kill(Address _addr)
{
    if (auto a = account(_addr))
        a->kill();
    // If the account is not in the db, nothing to kill.
}

mcp::uint256_t mcp::chain_state::storage(Address const& _id, mcp::uint256_t const& _key) const
{
    if (std::shared_ptr<mcp::account_state> as = account(_id))
        return as->storageValue(_key, m_db);

    return 0;
}

void mcp::chain_state::setStorage(Address const& _contract, u256 const& _key, u256 const& _value)
{
    m_changeLog.emplace_back(_contract, _key, storage(_contract, _key));
    m_cache[_contract]->setStorage(_key, _value);
}

mcp::uint256_t mcp::chain_state::originalStorageValue(Address const& _contract, mcp::uint256_t const& _key) const
{
    if (std::shared_ptr<mcp::account_state> as = account(_contract))
        return as->originalStorageValue(_key, m_db);
    return 0;
}

void mcp::chain_state::clearStorage(Address const& _contract)
{
    h256 const& oldHash{m_cache[_contract]->baseRoot()};
    if (oldHash == EmptyTrie)
        return;
    m_changeLog.emplace_back(Change::StorageRoot, _contract, oldHash);
    m_cache[_contract]->clearStorage();
}

std::map<h256, std::pair<u256, u256>> mcp::chain_state::storage(Address const& _id) const
{
#if ETH_FATDB
    std::map<h256, std::pair<u256, u256>> ret;

    if (std::shared_ptr<mcp::account_state> a = account(_id))
    {
        // Pull out all values from trie storage.
        if (h256 root = a->baseRoot())
        {
            SecureTrieDB<h256, dev::OverlayDB> memdb(const_cast<dev::OverlayDB*>(&m_db), root);       // promise we won't alter the overlay! :)

            for (auto it = memdb.hashedBegin(); it != memdb.hashedEnd(); ++it)
            {
                h256 const hashedKey((*it).first);
                u256 const key = h256(it.key());
                u256 const value = RLP((*it).second).toInt<u256>();
                ret[hashedKey] = std::make_pair(key, value);
            }
        }

        // Then merge cached storage over the top.
        for (auto const& i : a->storageOverlay())
        {
            h256 const key = i.first;
            h256 const hashedKey = sha3(key);
            if (i.second)
                ret[hashedKey] = i;
            else
                ret.erase(hashedKey);
        }
    }
    return ret;
#else
    (void) _id;
    BOOST_THROW_EXCEPTION(InterfaceNotSupported() << errinfo_interface("State::storage(Address const& _id)"));
#endif
}

dev::bytes const& mcp::chain_state::code(Address const& _addr) const
{
    std::shared_ptr<mcp::account_state> a = account(_addr);
    if (!a || a->codeHash() == EmptySHA3)
        return NullBytes;

    if (a->code().empty())
    {
        // Load the code from the backend.
		std::shared_ptr<mcp::account_state> mutableAccount = a;
        mutableAccount->noteCode(m_db.lookup(a->codeHash()));
        
        ////test
        //auto _code = m_db.lookup(a->codeHash());
        //std::cout << "address:" << _addr.hexPrefixed() << ",codeHash:" << a->codeHash().hexPrefixed() << ",code:" << _code.size() << std::endl;
        //mutableAccount->noteCode(_code);


        CodeSizeCache::instance().store(a->codeHash(), a->code().size());
    }

    return a->code();
}

void mcp::chain_state::setCode(Address const& _address, dev::bytes&& _code)
{
    m_changeLog.emplace_back(_address, code(_address));
    m_cache[_address]->setCode(std::move(_code));
}

dev::h256 mcp::chain_state::codeHash(Address const& _a) const
{
    if (std::shared_ptr<mcp::account_state> a = account(_a))
        return a->codeHash();
    else
        return EmptySHA3;
}

size_t mcp::chain_state::codeSize(Address const& _a) const
{
    if (std::shared_ptr<mcp::account_state> a = account(_a))
    {
        if (a->hasNewCode())
            return a->code().size();
        auto& codeSizeCache = CodeSizeCache::instance();
        h256 codeHash = a->codeHash();
        if (codeSizeCache.contains(codeHash))
            return codeSizeCache.get(codeHash);
        else
        {
            size_t size = code(_a).size();
            codeSizeCache.store(codeHash, size);
            return size;
        }
    }
    else
        return 0;
}


//bool mcp::chain_state::is_precompiled(Address const& account_a, uint64_t const& last_summary_mci_a) const
//{
//	return chain->is_precompiled(account_a, last_summary_mci_a);
//}
//bigint mcp::chain_state::cost_of_precompiled(Address const& account_a, bytesConstRef in_a) const
//{
//	return chain->cost_of_precompiled(account_a, in_a);
//}
//std::pair<bool, bytes> mcp::chain_state::execute_precompiled(Address const& account_a, bytesConstRef in_a) const
//{
//	return chain->execute_precompiled(account_a, in_a);
//}

//void mcp::chain_state::set_defalut_account_state(std::vector<h256>& accout_state_hashs)
//{
//    for(auto hash : accout_state_hashs)
//    {
//        std::shared_ptr<mcp::account_state> acc_state(store.account_state_get(transaction, hash));
//        if (acc_state)
//        {
//            LOG(m_log.debug) << "[set_defalut_account_state] "<<toHexPrefixed(acc_state->account());
//            m_cache.insert(std::make_pair(acc_state->account(), acc_state));
//        }
//    }
//}

// Diff from commit in aleth, here we
// 1. insert code into db if it's available
// 2. commit storageDB to DB
// 3. commit the cached account_state to DB
template<class DB>
AddressHash mcp::commit(AccountMap const& _cache, dev::eth::SecureTrieDB<Address, DB>& _state)
{
    //mcp::stopwatch_guard sw("chain state:commit");

    AddressHash ret;
    for (auto const& i : _cache)
    {
        if (i.second->isDirty())
        {
            RLPStream s(4);
            s << i.second->nonce() << i.second->balance();

            std::shared_ptr<mcp::account_state> state(i.second);
            if (i.second->storageOverlay().empty())
            {
                assert_x(i.second->baseRoot());
                s.append(i.second->baseRoot());
            }
            else
            {
                dev::eth::SecureTrieDB<h256, DB> storageDB(_state.db(), i.second->baseRoot());
                for (auto const& j : i.second->storageOverlay())
                    if (j.second)
                    {
                        storageDB.insert(j.first, rlp(j.second));
                        //cnote << j.first << ":" << j.second;

                        //auto _kk = bytesConstRef(storageDB.at(j.first));
                        //cnote << j.first << " get:" << RLP(_kk).toInt<u256>();
                    }
                    else
                        storageDB.remove(j.first);
                assert_x(storageDB.root());
                s.append(storageDB.root());
                //cnote << "storage commit root:" << storageDB.root().hex();
            }

            if (i.second->hasNewCode())
            {
                h256 ch = i.second->codeHash();

                /////test
                //std::cout << i.first.hexPrefixed() << ",codeHash:" << ch.hexPrefixed() << ",code:" << i.second->code().size() << std::endl;

                // Store the size of the code
                dev::eth::CodeSizeCache::instance().store(ch, i.second->code().size());
                _state.db()->insert(ch, &i.second->code());
                s << ch;
            }
            else
                s << i.second->codeHash();

            {
                //mcp::stopwatch_guard sw("chain state:commit2");
                _state.insert(i.first, &s.out());

                //auto _kk = bytesConstRef(_state.at(i.first));
                //cnote << i.first.hex() << " account:" << toHex(_kk);

               //db->commit();
            }

            ret.insert(i.first);
        }
    }

    return ret;
}

mcp::chain_state& mcp::createIntermediateState(mcp::chain_state& o_s, mcp::Block const& _block, unsigned _txIndex, mcp::chain const& _bc)
{
    o_s = _block.state();
    o_s.setRoot(_block.stateRootBeforeTx());
    o_s.executeBlockTransactions(_block, _txIndex,  *_bc.sealEngine());
    return o_s;
}
