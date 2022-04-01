#pragma once

#include <libdevcore/RLP.h>
#include <mcp/common/numbers.hpp>
#include <mcp/common/mcp_json.hpp>
#include <mcp/common/EVMSchedule.h>
#include <blake2/blake2.h>
#include <mcp/core/config.hpp>

namespace mcp
{
	enum class block_type : uint8_t
	{
		light = 0,
		dag = 254,
		genesis = 255
	};

	class block_hashables
	{
	public:
		block_hashables() = default;
		block_hashables(mcp::account const & to_a, mcp::amount const & amount_a,
			mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
			mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a, uint256_t const & gas, uint256_t const& gas_price_a,
			dev::bytes const & data_a, uint64_t const & exec_timestamp_a, uint256_t chainId_a);
		block_hashables(bool & error_a, mcp::block_type type, dev::RLP const & r);

		void init_from_genesis_json(bool & error_a, mcp::json const & genesis_json_a);
		void stream_RLP(mcp::block_type type, dev::RLPStream & s) const;
		void serialize_json(mcp::block_type type, mcp::json & json_a) const;
		// added by michael at 2/7
		void serialize_json_eth(mcp::json & json_a) const;

		mcp::block_hash previous;

		//dag
		std::vector<mcp::block_hash> parents; //for dag block
		std::shared_ptr<std::list<mcp::block_hash>> links = std::make_shared<std::list<mcp::block_hash>>();; //for light block
		mcp::summary_hash last_summary;
		mcp::block_hash last_summary_block;
		mcp::block_hash last_stable_block;
		uint64_t exec_timestamp;

		//light
		uint256_t gas_price;
		uint256_t gas;   		///< The total gas to convert, paid for from sender's account. Any unused gas gets refunded once the contract is ended.
		mcp::account to;
		mcp::amount amount;
		dev::bytes data;	///< The data associated with the transaction, or the initialiser if it's a creation transaction.
		uint256_t  chainID;
	};

	class block
	{
	public:
		block();
		block(mcp::block_type type_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a,
			mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
			mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a,
			uint256_t const & gas_a, uint256_t const & gas_price_a, dev::bytes const & data_a,
			uint64_t const & exec_timestamp_a, uint256_t chainId_a = mcp::mcp_network);
		block(bool & error_a, dev::RLP const & r, bool with_data);
		void stream_RLP(dev::RLPStream & s) const;

		virtual ~block() = default;

		void init_from_genesis_json(bool & error_a, mcp::json const & genesis_json_a);

		uint256_t chain_id() { return hashables->chainID; }
		mcp::block_type type() const { return m_type; }
		mcp::account & from() { return m_from; }
		mcp::block_hash & hash() const;
		std::string to_json();
		mcp::block_hash const & previous() const;
		std::vector<mcp::block_hash> const & parents() const;
		std::shared_ptr<std::list<mcp::block_hash>> links() const;
		mcp::block_hash root() const;
		void serialize_json(std::string &) const;
		void serialize_json(mcp::json & json_a) const;

		// added by michael at 2/7
		void serialize_json_eth(std::string &) const;
		void serialize_json_eth(mcp::json & json_a) const;
		//

		void set_signature(mcp::signature signature_a);
		void set_signature(mcp::raw_key const & prv_a/*, mcp::public_key const & pub_a*/);

		bool operator== (mcp::block const &) const;

		/// @returns true if transaction is contract-creation.
		bool isCreation() const { return hashables->to.is_zero(); }

		/// @returns amount of gas required for the basic payment.
		int64_t baseGasRequired(dev::eth::EVMSchedule const& _es) const { return baseGasRequired(isCreation(), &hashables->data, _es); }

		/// Get the fee associated for a transaction with the given data.
		static int64_t baseGasRequired(bool _contractCreation, dev::bytesConstRef _data, dev::eth::EVMSchedule const& _es);

		std::unique_ptr<mcp::block_hashables> hashables;
		mcp::signature signature;

	private:
		mcp::block_type m_type;
		mcp::account m_from;
		mutable mcp::block_hash m_hash = 0;
	};
}
