#pragma once
#include "chain_state.hpp"
#include <mcp/core/timeout_db_transaction.hpp>
#include <mcp/node/process_block_cache.hpp>

namespace mcp
{
	//struct ChainEnv
	//{
	//	//mcp::timeout_db_transaction& timeout_tx;
	//	//std::shared_ptr<mcp::process_block_cache> cache;
	//	uint64_t mci;
	//	uint64_t blockNum;
	//	uint64_t mc_timestamp;
	//	//uint64_t stable_timestamp;
	//	uint64_t mc_last_summary_mci;
	//};

	/// @brief Verified block info, does not hold block data, but a reference instead
	struct VerifiedBlockRef
	{
		std::shared_ptr<mcp::block> info;		///< Prepopulated block info
		std::vector< std::shared_ptr<Transaction> > transactions;	///< Verified list of block transactions
	};

	class TransactionReceiptPlaceholder :public dev::eth::TransactionReceipt
	{
	public:
		TransactionReceiptPlaceholder():TransactionReceipt(0,0, mcp::log_entries()){m_Placeholder = true;}
	};

	class chain;

	class Block
	{
	public:
		/// Default constructor; creates with a blank database prepopulated with the genesis block.
		Block(u256 const& _accountStartNonce) : m_state(_accountStartNonce, OverlayDB(), BaseState::Empty) {}

		Block(/*mcp::block_store& store_a, */chain const& _bc, OverlayDB const& _db, BaseState _bs = BaseState::PreExisting);

		/// Basic state object from database.
		/// Use the default when you already have a database and you just want to make a Block object
		/// which uses it.
		/// Will throw InvalidRoot if the root passed is not in the database.
		Block(/*mcp::block_store& store_a, */chain const& _bc, OverlayDB const& _db, h256 const& _root);

		/// Copy state object.
		Block(Block const& _s);

		/// Copy state object.
		Block& operator=(Block const& _s);

		/// Get an account's balance.
		/// @returns 0 if the address has never been used.
		u256 balance(Address const& _address) const { return m_state.balance(_address); }

		/// Get the number of transactions a particular address has sent (used for the transaction nonce).
		/// @returns 0 if the address has never been used.
		u256 transactionsFrom(Address const& _address) const { return m_state.getNonce(_address); }

		/// Get the value of a storage position of an account.
		/// @returns 0 if no account exists at that address.
		u256 storage(Address const& _contract, u256 const& _memory) const { return m_state.storage(_contract, _memory); }

		/// Get the storage of an account.
		/// @note This is expensive. Don't use it unless you need to.
		/// @returns map of hashed keys to key-value pairs or empty map if no account exists at that address.
		std::map<h256, std::pair<u256, u256>> storage(Address const& _contract) const { return m_state.storage(_contract); }

		/// Get the code of an account.
		/// @returns bytes() if no account exists at that address.
		bytes const& code(Address const& _contract) const { return m_state.code(_contract); }

		/// Get the code hash of an account.
		/// @returns EmptySHA3 if no account exists at that address or if there is no code associated with the address.
		h256 codeHash(Address const& _contract) const { return m_state.codeHash(_contract); }

		/// Get the backing state object.
		chain_state const& state() const { return m_state; }

		/// Open a DB - useful for passing into the constructor & keeping for other states that are necessary.
		dev::OverlayDB const& db() const { return m_state.db(); }

		/// The hash of the root of our state tree.
		h256 rootHash() const { return m_state.rootHash(); }

		/// Get the list of pending transactions.
		Transactions const& pending() const { return m_transactions; }

		/// Get a mutable State object which is backing this block.
		/// @warning Only use this is you know what you're doing. If you use it while constructing a
		/// normal sealable block, don't expect things to work right.
		chain_state& mutableState() { return m_state; }

		/// Get the transaction receipt for the transaction of the given index.
		dev::eth::TransactionReceipt const& receipt(unsigned _i) const { return m_receipts.at(_i); }

		/// Returns zero hash if intermediate state root is not available in the receipt (the case after EIP98)
		h256 stateRootBeforeTx() const { return m_previousBlockState->m_stateRoot; };

		/// Construct state object from arbitrary point in blockchain.
		void populateFromChain(std::shared_ptr<mcp::block_state> _cstate, std::shared_ptr<mcp::block> _cblock, std::shared_ptr<mcp::block_state> _pstate, Transactions& _txs);

		/// Execute a given transaction.
		/// This will append @a _t to the transaction list and change the state accordingly.
		//ExecutionResult execute(LastBlockHashesFace const& _lh, Transaction const& _t, Permanence _p = Permanence::Committed, dev::eth::OnOpFunc const& _onOp = dev::eth::OnOpFunc());
		//ExecutionResult execute(dev::eth::EnvInfo const& _envInfo, mcp::Transaction const& _t, Permanence _p, dev::eth::OnOpFunc const& _onOp = dev::eth::OnOpFunc());
		mcp::ExecutionResult execute(/*mcp::db::db_transaction& transaction_a, *//*chain const& _bc,*/ /*std::shared_ptr<mcp::iblock_cache> cache_a,*/ Transaction const& _t, dev::eth::McInfo const& mc_info_a, Permanence _p/*, dev::eth::OnOpFunc const& _onOp*/);

		/// Execute all transactions within a given block.
		/// @returns the additional total difficulty.
		//u256 mcp::Block::enactOn(mcp::timeout_db_transaction& timeout_tx_a,
		//	std::shared_ptr<mcp::process_block_cache> cache_a,
		//	uint64_t const& mci, uint64_t const& _blockNum, uint64_t const& mc_timestamp, uint64_t const& stable_timestamp, uint64_t const& mc_last_summary_mci,
		//	h256 const& dag_stable_block_hash,
		//	chain const& _bc
		//);
		u256 enactOn(dev::eth::McInfo const& _mc, VerifiedBlockRef const& _block, chain const& _bc);

		/// Returns back to a pristine state after having done a playback.
		void cleanup();

		///// Sets m_currentBlock to a clean state, (i.e. no change from m_previousBlock) 
		//void resetCurrent();

		/// Get the header information on the present block.
		dev::eth::McInfo info() const;
	private:
		/// Execute the given block, assuming it corresponds to m_currentBlock.
		/// Throws on failure.
		u256 enact(mcp::block const& _block, chain const& _bc);
		//void set_block_stable(mcp::timeout_db_transaction& timeout_tx_a, std::shared_ptr<mcp::process_block_cache> cache_a, mcp::block_hash const& stable_block_hash, uint64_t const& mci, uint64_t const& mc_timestamp, uint64_t const& mc_last_summary_mci, uint64_t const& stable_timestamp, uint64_t const& stable_index, h256 receiptsRoot);

		chain_state m_state;						///< Our state tree, as an OverlayDB DB.
		Transactions m_transactions;				///< The current list of transactions that we've included in the state.
		dev::eth::TransactionReceipts m_receipts;	///< The corresponding list of transaction receipts.
		//h256Hash m_transactionSet;				///< The set of transaction hashes that we've included in the state.
		std::shared_ptr<mcp::block_state> m_previousBlockState;
		std::shared_ptr<mcp::block_state> m_currentBlockState;
		std::shared_ptr<mcp::block> m_currentBlock;

		SealEngineFace* m_sealEngine = nullptr;		///< The chain's seal engine.
		//mcp::block_store m_store;
		mcp::log m_log = { mcp::log("node") };
	};
}