#pragma once

#include <mcp/wallet/key_store.hpp>

namespace mcp
{
	class kdf
	{
	public:
		void phs(mcp::raw_key &, std::string const &, mcp::uint128_union const &);
		std::mutex mutex;
	};

	class key_manager
	{
	public:
		key_manager(boost::filesystem::path const & application_path_a, mcp::key_store& store_a);
		// added by michael at 1/12
		bool exists(mcp::account const & account_a);
		//
		bool exists(mcp::public_key const & pub_a);
		// added by michael at 1/12
		bool find(mcp::account const & account_a, mcp::key_content & kc_a);
		//
		bool find(mcp::public_key const & pub_a, mcp::key_content & kc_a);
		std::list<mcp::public_key> list();
		mcp::public_key create(std::string const & password_a, bool gen_next_work_a, bool const & is_backup_a = true);
		// added by michael at 1/12
		bool change_password(mcp::account const & account_a, std::string const & old_password_a, std::string const & new_password_a);
		//
		bool change_password(mcp::public_key const & pub_a, std::string const & old_password_a, std::string const & new_password_a);
		// added by michael at 1/12
		bool remove(mcp::account const & account_a, std::string const & password_a);
		//
		bool remove(mcp::public_key const & pub_a, std::string const & password_a);
		bool import(std::string const & json_a, key_content & kc_a, bool gen_next_work_a);
		// added by michael at 1/13
		bool decrypt_prv(mcp::account const & account_a, std::string const & password_a, mcp::raw_key & prv);
		//
		bool decrypt_prv(mcp::public_key const & pub_a, std::string const & password_a, mcp::raw_key & prv);
		bool decrypt_prv(mcp::key_content const & kc, std::string const & password_a, mcp::raw_key & prv);
		bool is_locked(mcp::public_key const & pub_a);
		// added by michael at 1/13
		bool find_unlocked_prv(mcp::account const & account_a, mcp::raw_key & prv);
		//
		bool find_unlocked_prv(mcp::public_key const & pub_a, mcp::raw_key & prv);
		// added by michael at 1/12
		bool unlock(mcp::account const & account_a, std::string const & password_a);
		//
		bool unlock(mcp::public_key const & pub_a, std::string const & password_a);
		// updated by michael at 1/13
		void write_backup(mcp::account const & account, std::string const & json);
		// added by michael at 1/12
		void lock(mcp::account const & account_a);
		//
		void lock(mcp::public_key const & pub_a);
		bool work_account_exisit(mcp::public_key const & pub_a);
		// added by michael at 1/12
		bool work_account_get(mcp::account const & account_a, mcp::block_hash &  previous_a, mcp::uint64_union & work_a);
		//
		bool work_account_get(mcp::public_key const & pub_a, mcp::block_hash &  previous_a, mcp::uint64_union & work_a);
		bool work_get(mcp::public_key const &, mcp::block_hash const&  previous_a, mcp::uint64_union & work_a);
		void work_put(mcp::public_key const &, mcp::block_hash const&  previous_a, mcp::uint64_union const& work_a);
	private:
		mcp::key_content gen_key_content(mcp::raw_key const & prv, std::string const & password_a);
		void add_or_update_key(mcp::key_content const & kc, bool const & is_backup_a = true);

		mcp::kdf m_kdf;
		boost::filesystem::path m_backup_path;

		std::unordered_map<mcp::account, mcp::public_key> m_addr_lookup;
		std::unordered_map<mcp::public_key, mcp::key_content> m_key_contents;
		std::mutex m_key_contents_mutex;

		std::unordered_map<mcp::public_key, mcp::private_key> m_unlocked;
		std::mutex m_unlocked_mutex;
		mcp::key_store m_store;
	};
}
