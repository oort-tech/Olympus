#pragma once

#include <mcp/wallet/key_store.hpp>

namespace mcp
{
	class kdf
	{
	public:
		void phs(dev::Secret &, std::string const &, dev::h128 const &);
		std::mutex mutex;
	};

	class key_manager
	{
	public:
		key_manager(boost::filesystem::path const & application_path_a, mcp::key_store& store_a);
		bool exists(dev::Address const & account_a);
		bool find(dev::Address const & account_a, mcp::key_content & kc_a);
		std::list<dev::Address> list();
		dev::Address create(std::string const & password_a, bool gen_next_work_a, bool const & is_backup_a = true);
		bool change_password(dev::Address const & account_a, std::string const & old_password_a, std::string const & new_password_a);
		bool remove(dev::Address const & account_a, std::string const & password_a);
		bool import(std::string const & json_a, key_content & kc_a, bool gen_next_work_a);
		mcp::key_content importRawKey(dev::Secret & prv, std::string const & password);
		bool decrypt_prv(dev::Address const & account_a, std::string const & password_a, dev::Secret & prv);
		bool decrypt_prv(mcp::key_content const & kc, std::string const & password_a, dev::Secret & prv);
		bool is_locked(dev::Address const & pub_a);
		bool find_unlocked_prv(dev::Address const & account_a, dev::Secret & prv);
		bool unlock(dev::Address const & account_a, std::string const & password_a);
		void write_backup(dev::Address const & account, std::string const & json);
		void lock(dev::Address const & account_a);
		std::pair<bool, Secret> authenticate(dev::Address, boost::optional<std::string> const & password);
	private:
		mcp::key_content gen_key_content(dev::Secret const & prv, std::string const & password_a);
		void add_or_update_key(mcp::key_content const & kc, bool const & is_backup_a = true);

		mcp::kdf m_kdf;
		boost::filesystem::path m_backup_path;

		std::unordered_map<dev::Address, mcp::key_content> m_key_contents;
		std::mutex m_key_contents_mutex;

		std::unordered_map<dev::Address, dev::Secret> m_unlocked;
		std::mutex m_unlocked_mutex;
		mcp::key_store m_store;
	};
}
