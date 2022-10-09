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
		key_content(dev::Address const & account, dev::h128 const & kdf_salt_a, dev::h128 const & iv_a, dev::h256 const & ciphertext_a);
		dev::Slice val() const;
		std::string to_json() const;

		dev::Address account;
		dev::h128 kdf_salt;
		dev::h128 iv;
		dev::h256 ciphertext;
	};

	class value_previous_work
	{
	public:
		value_previous_work() = default;
		value_previous_work(dev::Slice const & val_a);
		value_previous_work(mcp::block_hash const &, dev::h64 const &);
		dev::Slice val() const;
		mcp::block_hash previous;
		dev::h64 work;
	};

	class key_store
	{
	public:
		key_store(bool & error_a, boost::filesystem::path const& _path);

		//keys
		void keys_put(mcp::db::db_transaction& transaction, dev::Address const& _k, mcp::key_content const& _v);
		bool keys_get(mcp::db::db_transaction& transaction, dev::Address const& _k, mcp::key_content& _v);
		void keys_del(mcp::db::db_transaction& transaction, dev::Address const& _k);
		bool keys_exists(mcp::db::db_transaction& transaction, dev::Address const& _k);
		mcp::db::forward_iterator keys_begin(mcp::db::db_transaction& transaction);
		mcp::db::forward_iterator keys_begin(mcp::db::db_transaction& transaction, dev::Address const& _k);
		mcp::db::backward_iterator keys_rbegin(mcp::db::db_transaction& transaction);
		mcp::db::backward_iterator keys_rbegin(mcp::db::db_transaction& transaction, dev::Address const& _k);

		mcp::db::db_transaction create_transaction() { return m_database->create_transaction(); };
		
	private:
		std::shared_ptr<mcp::db::database> m_database;
		int m_keys;
	};
}