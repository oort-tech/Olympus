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
		genesis = 0,
		dag = 1,
		light = 2
	};

	class block_hashables
	{
	public:
		block_hashables() = default;
		block_hashables(mcp::block_type type_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a,
			mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
			mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a, uint256_t const & gas, uint256_t const& gas_price_a,
			mcp::data_hash const & data_hash_a, uint64_t const & exec_timestamp_a, mcp::uint64_union const & work_a);
		block_hashables(bool & error_a, dev::RLP const & r);

		void init_from_genesis_json(bool & error_a, mcp::json const & genesis_json_a);
		void stream_RLP(dev::RLPStream & s) const;
		void serialize_json(mcp::json & json_a) const;
		// added by michael at 2/7
		void serialize_json_eth(mcp::json & json_a) const;
		//
		void hash(blake2b_state &) const;

		mcp::uint64_union block_work() const;
		void block_work_set(mcp::uint64_union const &);

		mcp::block_type type;
		mcp::account from;
		mcp::block_hash previous;
		mcp::uint64_union work = mcp::uint64_union(0);

		//dag
		std::vector<mcp::block_hash> parents; //for dag block
		std::shared_ptr<std::list<mcp::block_hash>> links = std::make_shared<std::list<mcp::block_hash>>();; //for light block
		mcp::summary_hash last_summary;
		mcp::block_hash last_summary_block;
		mcp::block_hash last_stable_block;
		uint64_t exec_timestamp;

		//light
		mcp::account to;
		mcp::amount amount;
		uint256_t gas;   		///< The total gas to convert, paid for from sender's account. Any unused gas gets refunded once the contract is ended.
		uint256_t gas_price;
		mcp::data_hash data_hash;
		uint8_t light_version = 1; // 0 for legacy light block, 1 for new light block 
	};

	class block
	{
	public:
		block();
		block(mcp::block_type type_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a,
			mcp::block_hash const & previous_a, std::vector<mcp::block_hash> const & parents_a, std::shared_ptr<std::list<mcp::block_hash>> links_a,
			mcp::summary_hash const & last_summary_a, mcp::block_hash const & last_summary_block_a, mcp::block_hash const & last_stable_block_a,
			uint256_t const & gas_a, uint256_t const & gas_price_a, mcp::data_hash const & data_hash_a, std::vector<uint8_t> const & data_a,
			uint64_t const & exec_timestamp_a, mcp::uint64_union const & work_a);
		block(bool & error_a, dev::RLP const & r, bool with_data);
		void stream_RLP(dev::RLPStream & s) const;
		void stream_RLP_nodata(dev::RLPStream & s) const;

		virtual ~block() = default;

		void init_from_genesis_json(bool & error_a, mcp::json const & genesis_json_a);

		static mcp::data_hash data_hash(std::vector<uint8_t> const & data_a);

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
		void set_signature(mcp::raw_key const & prv_a);

		bool operator== (mcp::block const &) const;

		void block_work_set(mcp::uint64_union const &);
		mcp::uint64_union block_work() const;

		/// @returns true if transaction is contract-creation.
		bool isCreation() const { return hashables->to.is_zero(); }

		/// @returns amount of gas required for the basic payment.
		int64_t baseGasRequired(dev::eth::EVMSchedule const& _es) const { return baseGasRequired(isCreation(), &data, _es); }

		/// Get the fee associated for a transaction with the given data.
		static int64_t baseGasRequired(bool _contractCreation, dev::bytesConstRef _data, dev::eth::EVMSchedule const& _es);

		std::unique_ptr<mcp::block_hashables> hashables;
		mcp::signature signature;
		dev::bytes data;	///< The data associated with the transaction, or the initialiser if it's a creation transaction.

	private:
		mutable mcp::block_hash m_hash = 0;
	};
}
