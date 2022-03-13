#include "key_manager.hpp"
#include <mcp/common/common.hpp>

void mcp::kdf::phs(mcp::raw_key & result_a, std::string const & password_a, mcp::uint128_union const & salt_a)
{
	std::lock_guard<std::mutex> lock(mutex);

	uint32_t memory_cost = 16 * 1024 * 1024;
	auto success = crypto_pwhash(result_a.data.bytes.data(),
		result_a.data.bytes.size(),
		password_a.data(),
		password_a.size(),
		salt_a.bytes.data(),
		1, memory_cost, crypto_pwhash_ALG_ARGON2ID13);
	assert_x(success == 0);
	(void)success;
}

mcp::key_manager::key_manager(boost::filesystem::path const & application_path_a, mcp::key_store& store_a):
	m_backup_path(application_path_a / "backup"),
	m_store(store_a)
{
	auto transaction = m_store.create_transaction();
	auto it = m_store.keys_begin(transaction);
	for (; it.valid(); ++it)
	{
		mcp::account account(mcp::slice_to_account(it.key()));
		mcp::key_content key_content(it.value());
		m_key_contents[account] = key_content;
	}

	boost::filesystem::create_directories(m_backup_path);
}

bool mcp::key_manager::exists(mcp::account const & account_a)
{
	std::lock_guard<std::mutex> lock(m_key_contents_mutex);
	return m_key_contents.count(account_a) > 0;
}

bool mcp::key_manager::work_account_exisit(mcp::account const & account_a)
{
	mcp::db::db_transaction transaction = m_store.create_transaction();
	auto result(m_store.work_exists(transaction, account_a));
	return result;
}

bool mcp::key_manager::work_account_get(mcp::account const & account_a, mcp::block_hash &  previous_a, mcp::uint64_union & work_a)
{
	auto result(false);
	mcp::value_previous_work previous_work_l;
	mcp::db::db_transaction transaction = m_store.create_transaction();
	auto exists(!m_store.work_get(transaction, account_a, previous_work_l));
	if (exists)
	{
		previous_a = previous_work_l.previous;
		work_a = previous_work_l.work;
	}
	else
		result = true;
	return result;
}

bool mcp::key_manager::work_get(mcp::account const & account_a, mcp::block_hash const&  previous_a, mcp::uint64_union & work_a)
{
    auto result(false);
	mcp::value_previous_work previous_work_l;
	mcp::db::db_transaction transaction = m_store.create_transaction();
	auto exists(!m_store.work_get(transaction, account_a, previous_work_l));

	if (exists)
	{
		if (previous_work_l.previous == previous_a)
		{
			work_a = previous_work_l.work;
			assert_x(!result);
		}
		else
			result = true;
	}
	else
		result = true;

	return result;
}

void mcp::key_manager::work_put(mcp::account const & account_a, mcp::block_hash const&  previous_a, mcp::uint64_union const & work_a)
{
    mcp::value_previous_work previous_work_l(previous_a, work_a);

	mcp::db::db_transaction transaction = m_store.create_transaction();
	m_store.work_put(transaction, account_a, previous_work_l);
}

bool mcp::key_manager::find(mcp::account const & account_a, mcp::key_content & kc_a)
{
	bool exists(true);
	std::lock_guard<std::mutex> lock(m_key_contents_mutex);
	if (m_key_contents.count(account_a))
	{
		kc_a = m_key_contents[account_a];
	}
	else
	{
		exists = false;
	}
	return exists;
}

std::list<mcp::account> mcp::key_manager::list()
{
	std::list<mcp::account> accounts;
	std::lock_guard<std::mutex> lock(m_key_contents_mutex);
	for (auto pair : m_key_contents)
		accounts.push_back(pair.first);
	return accounts;
}

mcp::account mcp::key_manager::create(std::string const & password_a, bool generate_work_a, bool const & is_backup_a)
{
	mcp::raw_key prv;
	random_pool.GenerateBlock(prv.data.bytes.data(), prv.data.bytes.size());

	mcp::key_content kc(gen_key_content(prv, password_a));
	add_or_update_key(kc, is_backup_a);

	return kc.account;
}

bool mcp::key_manager::change_password(mcp::account const & account_a, std::string const & old_password_a, std::string const & new_password_a)
{
	mcp::raw_key prv;
	bool error(decrypt_prv(account_a, old_password_a, prv));
	if (!error)
	{
		mcp::key_content kc(gen_key_content(prv, new_password_a));
		add_or_update_key(kc);
	}
	return error;
}

bool mcp::key_manager::remove(mcp::account const & account_a, std::string const & password_a)
{
	mcp::raw_key prv;
	bool error(decrypt_prv(account_a, password_a, prv));
	if (!error)
	{
		{
			std::lock_guard<std::mutex> lock(m_unlocked_mutex);
			m_unlocked.erase(account_a);
		}
		{
			std::lock_guard<std::mutex> lock(m_key_contents_mutex);
			m_key_contents.erase(account_a);
		}
		mcp::db::db_transaction transaction = m_store.create_transaction();
		m_store.keys_del(transaction, account_a);
	}

	return error;
}

bool mcp::key_manager::import(std::string const & json_a, key_content & kc_a, bool generate_work_a)
{
	bool error(false);
	mcp::key_content kc(error, json_a);
	if (!error)
	{
		add_or_update_key(kc);
		kc_a = kc;
	}
	return error;
}

bool mcp::key_manager::decrypt_prv(mcp::account const & account, std::string const & password_a, mcp::raw_key & prv)
{
	bool error(false);
	mcp::key_content kc;
	{
		std::lock_guard<std::mutex> lock(m_key_contents_mutex);
		if (m_key_contents.count(account))
			kc = m_key_contents[account];
		else
			error = true;
	}

	if (!error)
		error = decrypt_prv(kc, password_a, prv);

	return error;
}

bool mcp::key_manager::decrypt_prv(mcp::key_content const & kc_a, std::string const & password_a, mcp::raw_key & prv)
{
	bool error(false);

	mcp::raw_key derive_pwd;
	m_kdf.phs(derive_pwd, password_a, kc_a.kdf_salt);

	prv.decrypt(kc_a.ciphertext, derive_pwd, kc_a.iv);

	mcp::public_key compare;
	mcp::encry::generate_public_from_secret(prv.data, compare);

	if (kc_a.account != mcp::account(compare))
	{
		error = true;
	}

	return error;
}

bool mcp::key_manager::is_locked(mcp::account const & account_a)
{
	std::lock_guard<std::mutex> lock(m_unlocked_mutex);
	return m_unlocked.count(account_a) > 0;
}

bool mcp::key_manager::find_unlocked_prv(mcp::account const & account_a, mcp::raw_key & prv)
{
	bool exists(true);
	std::lock_guard<std::mutex> lock(m_unlocked_mutex);
	if (m_unlocked.count(account_a))
		prv.data = m_unlocked[account_a];
	else
		exists = false;
	return exists;
}

void mcp::key_manager::lock(mcp::account const & account_a)
{
	std::lock_guard<std::mutex> lock(m_unlocked_mutex);
	m_unlocked.erase(account_a);
}

bool mcp::key_manager::unlock(mcp::account const & account_a, std::string const & password_a)
{
	mcp::raw_key prv;
	bool error(decrypt_prv(account_a, password_a, prv));
	if (!error)
	{
		std::lock_guard<std::mutex> lock(m_unlocked_mutex);
		m_unlocked[account_a] = prv.data;
	}
	
	return error;
}

void mcp::key_manager::write_backup(mcp::account const & account, std::string const & json)
{
	std::ofstream backup_file;
	std::string file_name((m_backup_path / (account.to_account() + ".json")).string());
	backup_file.open(file_name);
	if (!backup_file.fail())
	{
		backup_file << json;
	}
}

mcp::key_content mcp::key_manager::gen_key_content(mcp::raw_key const & prv, std::string const & password_a)
{
	mcp::uint128_union kdf_salt;
	random_pool.GenerateBlock(kdf_salt.bytes.data(), kdf_salt.bytes.size());

	mcp::raw_key derive_pwd;
	m_kdf.phs(derive_pwd, password_a, kdf_salt);

	mcp::uint128_union iv;
	random_pool.GenerateBlock(iv.bytes.data(), iv.bytes.size());

	mcp::uint256_union ciphertext;
	ciphertext.encrypt(prv, derive_pwd, iv);

	mcp::public_key pub;
	mcp::encry::generate_public_from_secret(prv.data, pub);

	mcp::key_content kc(mcp::account(pub), kdf_salt, iv, ciphertext);
	return kc;
}

void mcp::key_manager::add_or_update_key(mcp::key_content const & kc, bool const & is_backup_a)
{
	{
		std::lock_guard<std::mutex> lock(m_key_contents_mutex);
		m_key_contents[kc.account] = kc;
	}
	mcp::db::db_transaction transaction = m_store.create_transaction();
	m_store.keys_put(transaction, kc.account, kc);

	if(is_backup_a)
		write_backup(kc.account, kc.to_json());
}
