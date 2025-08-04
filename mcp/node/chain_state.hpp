#pragma once

#include <mcp/core/block_store.hpp>
//#include <mcp/core/overlay_db.hpp>
#include <mcp/core/transaction_receipt.hpp>
//#include <mcp/node/process_block_cache.hpp>
#include <mcp/common/SecureTrieDB.h>
#include <libevm/ExtVMFace.h>
//#include <mcp/node/chain.hpp>
#include <mcp/core/transaction.hpp>
//#include <mcp/core/approve.hpp>
#include <mcp/core/SealEngine.h>
#include <mcp/common/log.hpp>
#include <mcp/common/CodeSizeCache.h>
#include <libdevcore/OverlayDB.h>
#include <set>
#include <unordered_set>

namespace mcp
{

DEV_SIMPLE_EXCEPTION(InvalidAccountStartNonceInState);
DEV_SIMPLE_EXCEPTION(IncorrectAccountStartNonceInState);

enum class BaseState
{
    PreExisting,
    Empty
};

/// An atomic state changelog entry.
struct Change
{
    enum Kind: int
    {
        /// Account balance changed. Change::value contains the amount the
        /// balance was increased by.
        Balance,

        /// Account storage was modified. Change::key contains the storage key,
        /// Change::value the storage value.
        Storage,

        /// Account storage root was modified.  Change::value contains the old
        /// account storage root.
        StorageRoot,

        /// Account nonce was changed.
        Nonce,

        /// Account was created (it was not existing before).
        Create,

        /// New code was added to an account (by "create" message execution).
        Code,

        /// Account was touched for the first time.
        Touch
    };

    Kind kind;        ///< The kind of the change.
	Address address;  ///< Changed account address.
    uint256_t value;       ///< Change value, e.g. balance, storage and nonce.
    uint256_t key;         ///< Storage key. Last because used only in one case.
    dev::bytes oldCode;    ///< Code overwritten by CREATE, empty except in case of address collision.

    /// Helper constructor to make change log update more readable.
    Change(Kind _kind, Address const& _addr, uint256_t const& _value = 0):
            kind(_kind), address(_addr), value(_value)
    {
        assert_x(_kind != Code); // For this the special constructor needs to be used.
    }

    /// Helper constructor especially for storage change log.
    Change(Address const& _addr, uint256_t const& _key, uint256_t const& _value):
            kind(Storage), address(_addr), value(_value), key(_key)
    {}

    /// Helper constructor for nonce change log.
    Change(Address const& _addr, uint256_t const& _value):
            kind(Nonce), address(_addr), value(_value)
    {}

    /// Helper constructor especially for new code change log.
    Change(Address const& _addr, dev::bytes const& _oldCode):
            kind(Code), address(_addr), oldCode(_oldCode)
    {}
};

using ChangeLog = std::vector<Change>;

class chain;
class Block;

class chain_state
{
    friend class chain;
public:

    /// Basic state object from database.
    /// Use the default when you already have a database and you just want to make a State object
    /// which uses it. If you have no preexisting database then set BaseState to something other
    /// than BaseState::PreExisting in order to prepopulate the Trie.
    explicit chain_state(/*mcp::db::db_transaction& transaction_a,*/ u256 const& _accountStartNonce, /*mcp::block_store& store_a,*/
		/*std::shared_ptr<mcp::chain> chain_a,*/ /*std::shared_ptr<mcp::iblock_cache> cache_a,*/ OverlayDB const& _db, BaseState _bs = BaseState::PreExisting);
    
    enum NullType { Null };
    chain_state(NullType) : chain_state(Invalid256, OverlayDB(), BaseState::Empty) {}

    /// Copy state object.
    chain_state(chain_state const& _s);

    /// Copy state object.
    chain_state& operator=(chain_state const& _s);

    OverlayDB const& db() const { return m_db; }
    OverlayDB& db() { return m_db; }

    std::pair<ExecutionResult, dev::eth::TransactionReceipt> execute(dev::eth::EnvInfo const& _envInfo, SealEngineFace const& _sealEngine, Permanence _p, mcp::Transaction const& _t/*, dev::eth::OnOpFunc const& _onOp = dev::eth::OnOpFunc()*/);

    /// @returns the account at the given address or a null pointer if it does not exist.
    /// The pointer is valid until the next access to the state or account.
	std::shared_ptr<mcp::account_state> account(Address const& _addr) const;

    /// Execute @a _txCount transactions of a given block.
    /// This will change the state accordingly.
    void executeBlockTransactions(Block const& _block, unsigned _txCount, SealEngineFace const& _sealEngine);

    /// Check if the address is in use.
    bool addressInUse(Address const& _address) const;

    /// Check if the address contains executable code.
    bool addressHasCode(Address const& _address) const;

    /// Get an account's balance.
    /// @returns 0 if the address has never been used.
    uint256_t balance(Address const& _id) const;

    /// Add some amount to balance.
    /// Will initialise the address if it has never been used.
    void addBalance(Address const& _id, uint256_t const& _amount);

    /// Subtract the @p _value amount from the balance of @p _addr account.
    /// @throws NotEnoughCash if the balance of the account is less than the
    /// amount to be subtrackted (also in case the account does not exist).
    void subBalance(Address const& _addr, uint256_t const& _value);

    /// Set the balance of @p _addr to @p _value.
    /// Will instantiate the address if it has never been used.
    void setBalance(Address const& _addr, uint256_t const& _value);

    /**
     * @brief Transfers "the balance @a _value between two accounts.
     * @param _from Account from which @a _value will be deducted.
     * @param _to Account to which @a _value will be added.
     * @param _value Amount to be transferred.
     */
    void transferBalance(Address const& _from, Address const& _to, uint256_t const& _value);

    /// Get the value of a storage position of an account.
    /// @returns 0 if no account exists at that address.
    mcp::uint256_t storage(Address const& _contract, mcp::uint256_t const& _memory) const;

    /// Get the storage of an account.
    /// @note This is expensive. Don't use it unless you need to.
    /// @returns map of hashed keys to key-value pairs or empty map if no account exists at that address.
    std::map<h256, std::pair<u256, u256>> storage(Address const& _id) const;

    /// Set the value of a storage position of an account.
    void setStorage(Address const& _contract, dev::u256 const& _location, dev::u256 const& _value);

    /// Get the original value of a storage position of an account (before modifications saved in
    /// account cache).
    /// @returns 0 if no account exists at that address.
    dev::u256 originalStorageValue(Address const& _contract, mcp::uint256_t const& _key) const;

    /// Clear the storage root hash of an account to the hash of the empty trie.
    void clearStorage(Address const& _contract);

    /// Get the code of an account.
    /// @returns bytes() if no account exists at that address.
    /// @warning The reference to the code is only valid until the access to
    ///          other account. Do not keep it.
    dev::bytes const& code(Address const& _addr) const;

    /// Get the code hash of an account.
    /// @returns EmptySHA3 if no account exists at that address or if there is no code associated with the address.
    dev::h256 codeHash(Address const& _contract) const;

    /// Get the byte-size of the code of an account.
    /// @returns code(_contract).size(), but utilizes CodeSizeHash.
    size_t codeSize(Address const& _contract) const;

    /// Increament the account nonce.
    void incNonce(Address const& _id);

    /// Set the account nonce.
    void setNonce(Address const& _addr, mcp::uint256_t const& _newNonce);

    /// Get the account nonce -- the number of transactions it has sent.
    /// @returns 0 if the address has never been used.
    u256 getNonce(Address const& _addr) const;

    /// Sets the code of the account. Must only be called during / after contract creation.
    void setCode(Address const& _address, dev::bytes&& _code);

    /// Delete an account (used for processing suicides).
    void kill(Address _a);

    /// The hash of the root of our state tree.
    h256 rootHash() const { return m_state.root(); }

    /// Commit all changes waiting in the address cache to the DB.
    /// @param _commitBehaviour whether or not to remove empty accounts during commit.
    void commit();

    /// Resets any uncommitted changes to the cache.
    void setRoot(h256 const& _root);

    /// Get the account start nonce. May be required.
    u256 const& accountStartNonce() const { return m_accountStartNonce; }
    u256 const& requireAccountStartNonce() const;
    void noteAccountStartNonce(u256 const& _actual);

	/// Create a savepoint in the state changelog.
    /// @return The savepoint index that can be used in rollback() function.
    size_t savepoint() const;

    /// Revert all recent changes up to the given @p _savepoint savepoint.
    void rollback(size_t _savepoint);

    ChangeLog const& changeLog() const { return m_changeLog; }

    /// Hash of a block if within the last 256 blocks, or h256() otherwise.
    h256 blockHash(u256 _number);

	//bool is_precompiled(Address const& account_a, uint64_t const& last_summary_mci_a) const;

	//bigint cost_of_precompiled(Address const& account_a, bytesConstRef in_a) const;

	//std::pair<bool, bytes> execute_precompiled(Address const& account_a, bytesConstRef in_a) const;

    //void set_defalut_account_state(std::vector<h256>& accout_state_hashs);

    /// transaction
    //mcp::db::db_transaction & transaction;
    /// store
	//mcp::block_store store;
	///chain
	//std::shared_ptr<mcp::chain> chain;
    /// database cache
    //std::shared_ptr<mcp::iblock_cache> block_cache;

	/// current block
	//mcp::Transaction ts;

	//std::list<std::shared_ptr<mcp::trace>> traces;

private:

    /// Turns all "touched" empty accounts into non-alive accounts.
    void removeEmptyAccounts();

    /// Purges non-modified entries in m_cache if it grows too large.
    void clearCacheIfTooLarge() const;

    void createAccount(Address const& _address, std::shared_ptr<mcp::account_state> _account);
	
    /// @returns true when normally halted; false when exceptionally halted; throws when internal VM
    /// exception occurred.
	bool executeTransaction(Executive& _e, mcp::Transaction const& _t/*, dev::eth::OnOpFunc const& _onOp*/);

    ////Save the account status before transaction execution for debug_traceTransaction
    //void save_previous_account_state();

    /// Our overlay for the state tree.
    //mcp::overlay_db m_db;
    dev::OverlayDB m_db;
    /// Our state tree, as an OverlayDB DB.
    dev::eth::SecureTrieDB<Address, dev::OverlayDB> m_state;

	/// Our address cache. This stores the states of each address that has (or at least might have)
    /// been changed.
    mutable std::unordered_map<Address, std::shared_ptr<mcp::account_state>> m_cache;
    /// Tracks entries in m_cache that can potentially be purged if it grows too large.
    mutable std::vector<Address> m_unchangedCacheEntries;
    /// Tracks addresses that are known to not exist.
    mutable std::set<Address> m_nonExistingAccountsCache;
    /// Tracks all addresses touched so far.
	AddressHash m_touched;

    u256 m_accountStartNonce;

    ChangeLog m_changeLog;
    mcp::log m_log = { mcp::log("node") };
};

// Diff from commit in aleth, here we
// 1. insert code into db if it's available
// 2. commit storageDB to DB
// 3. commit the cached account_state to DB

//template <class DB>
//AddressHash commit(mcp::db::db_transaction& transaction_a, AccountMap const& _cache, DB* db, std::shared_ptr<mcp::process_block_cache> block_cache, mcp::block_store& store, h256 const& ts)
//{
//    //mcp::stopwatch_guard sw("chain state:commit");
//
//    AddressHash ret;
//    for (auto const& i : _cache)
//    {
//        if (i.second->isDirty())
//        {
//            if (i.second->hasNewCode())
//            {
//                h256 ch = i.second->codeHash();
//                // sichaoy: why do we need CodeSizeCache?
//                // Store the size of the code
//                dev::eth::CodeSizeCache::instance().store(ch, i.second->code().size());
//                db->insert(ch, &i.second->code());
//            }
//
//            std::shared_ptr<mcp::account_state> state(i.second);
//            if (i.second->storageOverlay().empty())
//            {
//                assert_x(i.second->baseRoot());
//                state->setStorageRoot(i.second->baseRoot());
//            }
//            else
//            {
//                //mcp::stopwatch_guard sw("chain state:commit1");
//
//                dev::eth::SecureTrieDB<h256, DB> storageDB(db, i.second->baseRoot());
//                for (auto const& j : i.second->storageOverlay())
//                    if (j.second)
//                        storageDB.insert(j.first, rlp(j.second));
//                    else
//                        storageDB.remove(j.first);
//                assert_x(storageDB.root());
//                state->setStorageRoot(storageDB.root());
//            }
//
//            {
//                //mcp::stopwatch_guard sw("chain state:commit2");
//
//                // commit the account_state to DB
//                db->commit();
//
//                //// Update account_state  previous and block hash
//                state->setPrevious();
//                state->setTs(ts);
//                state->record_init_hash();
//                state->clear_temp_state();
//
//                block_cache->latest_account_state_put(transaction_a, i.first, state);
//            }
//
//            ret.insert(i.first);
//        }
//    }
//
//    return ret;
//}

chain_state& createIntermediateState(chain_state& o_s, Block const& _block, unsigned _txIndex, chain const& _bc);

template <class DB>
AddressHash commit(AccountMap const& _cache, dev::eth::SecureTrieDB<Address, DB>& _state);

}
