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
		bool exists(mcp::public_key const & pub_a);
		bool find(mcp::public_key const & pub_a, mcp::key_content & kc_a);
		std::list<mcp::public_key> list();
		mcp::public_key create(std::string const & password_a, bool gen_next_work_a, bool const & is_backup_a = true);
		bool change_password(mcp::public_key const & pub_a, std::string const & old_password_a, std::string const & new_password_a);
		bool remove(mcp::public_key const & pub_a, std::string const & password_a);
		bool import(std::string const & json_a, key_content & kc_a, bool gen_next_work_a);
		bool decrypt_prv(mcp::public_key const & pub_a, std::string const & password_a, mcp::raw_key & prv);
		bool decrypt_prv(mcp::key_content const & kc, std::string const & password_a, mcp::raw_key & prv);
		bool is_locked(mcp::public_key const & pub_a);
		bool find_unlocked_prv(mcp::public_key const & pub_a, mcp::raw_key & prv);
		bool unlock(mcp::public_key const & pub_a, std::string const & password_a);
		void write_backup(mcp::public_key const & account, std::string const & json);
		void lock(mcp::public_key const & pub_a);
		bool work_account_exisit(mcp::public_key const & pub_a);
		bool work_account_get(mcp::public_key const & pub_a, mcp::block_hash &  previous_a, mcp::uint64_union & work_a);
		bool work_get(mcp::public_key const &, mcp::block_hash const&  previous_a, mcp::uint64_union & work_a);
		void work_put(mcp::public_key const &, mcp::block_hash const&  previous_a, mcp::uint64_union const& work_a);
	private:
		mcp::key_content gen_key_content(mcp::raw_key const & prv, std::string const & password_a);
		void add_or_update_key(mcp::key_content const & kc, bool const & is_backup_a = true);

		mcp::kdf m_kdf;
		boost::filesystem::path m_backup_path;

		std::unordered_map<mcp::public_key, mcp::key_content> m_key_contents;
		std::mutex m_key_contents_mutex;

		std::unordered_map<mcp::public_key, mcp::private_key> m_unlocked;
		std::mutex m_unlocked_mutex;
		mcp::key_store m_store;
	};
}
