#pragma once

#include <mcp/core/common.hpp>


namespace mcp
{
	class key_content
	{
	public:
		key_content();
		key_content(dev::Slice const & val_a);
		key_content(bool & error_a, std::string const & json_a);
		key_content(mcp::account const & account, mcp::uint128_union const & kdf_salt_a, mcp::uint128_union const & iv_a, mcp::secret_ciphertext const & ciphertext_a);
		dev::Slice val() const;
		std::string to_json() const;

		mcp::account account;
		// mcp::public_key public_key;
		mcp::uint128_union kdf_salt;
		mcp::uint128_union iv;
		mcp::secret_ciphertext ciphertext;
	};

	class value_previous_work
	{
	public:
		value_previous_work() = default;
		value_previous_work(dev::Slice const & val_a);
		value_previous_work(mcp::uint256_union const &, mcp::uint64_union const &);
		dev::Slice val() const;
		mcp::block_hash previous;
		mcp::uint64_union work;
	};

	class key_store
	{
	public:
		key_store(bool & error_a, boost::filesystem::path const& _path);

		//keys
		void keys_put(mcp::db::db_transaction& transaction, mcp::account const& _k, mcp::key_content const& _v);
		bool keys_get(mcp::db::db_transaction& transaction, mcp::account const& _k, mcp::key_content& _v);
		void keys_del(mcp::db::db_transaction& transaction, mcp::account const& _k);
		bool keys_exists(mcp::db::db_transaction& transaction, mcp::account const& _k);
		mcp::db::forward_iterator keys_begin(mcp::db::db_transaction& transaction);
		mcp::db::forward_iterator keys_begin(mcp::db::db_transaction& transaction, mcp::account const& _k);
		mcp::db::backward_iterator keys_rbegin(mcp::db::db_transaction& transaction);
		mcp::db::backward_iterator keys_rbegin(mcp::db::db_transaction& transaction, mcp::account const& _k);

		//work
		void work_put(mcp::db::db_transaction& transaction, mcp::account const& _k, mcp::value_previous_work const& _v);
		bool work_get(mcp::db::db_transaction& transaction, mcp::account const& _k, mcp::value_previous_work& _v);
		void work_del(mcp::db::db_transaction& transaction, mcp::account const& _k);
		bool work_exists(mcp::db::db_transaction& transaction, mcp::account const& _k);
		mcp::db::forward_iterator work_begin(mcp::db::db_transaction& transaction);
		mcp::db::forward_iterator work_begin(mcp::db::db_transaction& transaction, mcp::account const& _k);
		mcp::db::backward_iterator work_rbegin(mcp::db::db_transaction& transaction);
		mcp::db::backward_iterator work_rbegin(mcp::db::db_transaction& transaction, mcp::account const& _k);

		mcp::db::db_transaction create_transaction() { return m_database->create_transaction(); };
		
	private:
		std::shared_ptr<mcp::db::database> m_database;
		int m_keys;
		int m_work;
	};
}