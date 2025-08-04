#pragma once

#include "chain.hpp"
#include <mcp/p2p/host.hpp>
#include <mcp/rpc/LogFilter.hpp>

namespace mcp
{
	class Client
	{
	public:
		Client(mcp::block_store& store_a, 
			std::shared_ptr<mcp::chain> chain_a,
			std::shared_ptr<mcp::block_cache> cache_a, 
			std::shared_ptr<mcp::p2p::host> host_a);
		~Client() {};

		/// Estimate gas usage for call/create.
		/// @param _maxGas An upper bound value for estimation, if not provided default value of c_maxGasEstimate will be used.
		/// @param _callback Optional callback function for progress reporting
		std::pair<u256, ExecutionResult> estimateGas(Address const& _from, u256 _value, Address _dest, bytes const& _data, int64_t _maxGas, u256 _gasPrice, BlockNumber _blockNumber, GasEstimationCallback const& _callback = GasEstimationCallback());


		u256 balanceAt(Address _a, BlockNumber _block) const;
		u256 countAt(Address _a, BlockNumber _block) const;
		u256 stateAt(Address _a, u256 _l, BlockNumber _block) const;
		dev::bytes codeAt(Address _a, BlockNumber _block) const;
		localised_log_entries logs(LogFilter const& _filter) const;

		/// Makes the given call. Nothing is recorded into the state.
		ExecutionResult call(Address const& _from, u256 _value, Address _dest, bytes const& _data, u256 _gas, u256 _gasPrice, BlockNumber _blockNumber);
		/// for syatem contract
		dev::bytes callSystem(dev::Address const& _from, dev::Address const& _contractAddress, dev::bytes const& _data);

		/// Get the object representing the current canonical blockchain.
		chain const& blockChain() const { return bc(); }

		LocalisedTransaction localisedTransaction(h256 const& _transactionHash) const;
		LocalisedTransaction localisedTransaction(h256 const& _blockHash, unsigned _i) const;
		LocalisedTransaction localisedTransaction(BlockNumber const& _block, unsigned _i) const;
		
		LocalisedBlock localisedBlock(BlockNumber const& _block) const;
		LocalisedBlock localisedBlock(h256 const& _block) const;

		block blockInfo(h256 const& _block) const;
		block blockInfo(BlockNumber const& _block) const;
		block_state blockState(h256 const& _block) const;
		h256 blockSummary(h256 const& _block) const;
		h256 mciHash(uint64_t const& _mci) /*const*/;

		dev::eth::LocalisedTransactionReceipt localisedTransactionReceipt(h256 const& _transactionHash) const;

		unsigned transactionCount(h256 _blockHash) const;
		unsigned transactionCount(BlockNumber _block) const;

		Approves epochApproves(Epoch _epoch);
		ApproveReceipt approveReceipt(dev::h256 const& _h) const;

		uint64_t number() const;
		uint64_t lastStableMci() const;
		uint64_t lastMci() const;
		mcp::Epoch lastEpoch() const;

		Block block(h256 const& _block, bool _debug=false) const;

		Block blockByNumber(BlockNumber _h) const;

		h256 workTransactionHash(Epoch _epoch);
		//json status() const;
		WitnessList witnessList(Epoch _epoch) const;
		std::unordered_map<p2p::node_id, bi::tcp::endpoint> peers() const;
		std::list<p2p::node_info> nodes() const;
		bool netListening() const;
		size_t peersCount() const;
		int storeVersion();
	private:
		bool getMcInfo(dev::eth::McInfo& mc_info_a, uint64_t& block_number);
		/// InterfaceStub methods
		chain& bc() { return *m_chain; }
		chain const& bc() const { return *m_chain; }

		std::shared_ptr<mcp::chain> m_chain;
		std::shared_ptr<mcp::block_cache> m_cache;
		//std::shared_ptr<mcp::key_manager> m_key_manager;
		//std::shared_ptr<mcp::wallet> m_wallet;
		std::shared_ptr<mcp::p2p::host> m_host;
		mcp::block_store m_store;

		mcp::log m_log = { mcp::log("rpc") };
	};
}
