#pragma once

#include <mcp/node/message.hpp>
#include <mcp/consensus/ledger.hpp>
#include <mcp/core/block_store.hpp>
#include <mcp/node/chain.hpp>

namespace mcp
{
	enum class compose_result_codes
	{
		ok,
		insufficient_balance,
		data_size_too_large,
		validate_error,
		dag_no_links,
		error
	};

	class compose_result
	{
	public:
		compose_result(mcp::compose_result_codes const & code_a, std::shared_ptr<mcp::joint_message> message);
		mcp::compose_result_codes code;
		std::shared_ptr<mcp::joint_message> joint;
	};

	class composer
	{
	public:
		composer(mcp::block_store& store_a, std::shared_ptr<mcp::block_cache> cache_a,
			mcp::ledger& ledger_a, std::shared_ptr<mcp::chain> chain_a,
			uint256_t const& gas_price_a
		);
		~composer();
		mcp::compose_result compose_joint(mcp::db::db_transaction & transaction_a, mcp::block_type const & type_a, boost::optional<mcp::block_hash> const & previous_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a, uint256_t const & gas_a, uint256_t const & gas_price_a, std::vector<uint8_t> const & data_a, mcp::raw_key const & prv_a, /*mcp::public_key const & pub_a,*/ bool generate_work_a);
		mcp::compose_result_codes compose_block(mcp::db::db_transaction &  transaction_a, mcp::block_type const & type_a, boost::optional<mcp::block_hash> const & previous_a, mcp::account const & from_a, mcp::account const & to_a, mcp::amount const & amount_a, uint256_t const & gas_a, uint256_t const & gas_price_a, std::vector<uint8_t> const & data_a, std::shared_ptr<mcp::block>& block_a);
		mcp::compose_result sign_and_compose_joint(std::shared_ptr<mcp::block> block_a, mcp::signature const & signature_a, bool generate_work_a);
		void pick_parents_and_last_summary_and_wl_block(mcp::error_message & err_msg, mcp::db::db_transaction &  transaction_a, mcp::block_type const & block_type_a, mcp::block_hash const & previous_a, mcp::account const & from_a, std::vector<mcp::block_hash>& parents, std::shared_ptr<std::list<mcp::block_hash>> links, mcp::block_hash & last_summary_block, mcp::summary_hash & last_summary, mcp::block_hash & last_stable_block, uint256_t const& gas_a, uint256_t const& gas_price_a);
		
		mcp::block_hash get_latest_block(mcp::db::db_transaction &  transaction_a, mcp::block_type const & type_a, mcp::account const & account_a);
		uint256_t get_gas_price() { return m_gas_price; }
		std::shared_ptr<std::list<mcp::block_hash>> random_get_links(mcp::db::db_transaction &  transaction_a, size_t const & limit_a, std::shared_ptr<rocksdb::ManagedSnapshot> snapshot_a = nullptr);

		mcp::ledger & m_ledger;
		mcp::block_store & m_store;
		std::shared_ptr<mcp::iblock_cache> m_cache;
		std::shared_ptr<mcp::chain> m_chain;
		uint256_t	m_gas_price;
        mcp::log m_log = { mcp::log("node") };
		void stop() { m_stopped = true; }
		bool m_stopped = false;
	};
}