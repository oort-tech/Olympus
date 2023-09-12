#pragma once

#include <mcp/wallet/key_store.hpp>

namespace mcp
{
	///Encryptdata encrypts the data given as 'data' with the password 'auth'.
	mcp::CryptoJSON EncryptDataV3(dev::Secret const& _data, std::string const& _auth, int _scryptN, int _scryptP);

	std::pair<bool, dev::Secret> DecryptDataV3(mcp::CryptoJSON const& _cryptoJson, std::string const& _auth);

	std::pair<bool, dev::Secret> GetKDFKey(mcp::CryptoJSON const& _cryptoJson, std::string const& _auth);

	class key_manager
	{
	public:
		key_manager(boost::filesystem::path const& _path, mcp::key_store& _store);
		bool exists(dev::Address const& _address);
		bool find(dev::Address const& _address, mcp::key_content & _kc);
		Addresses list();
		dev::Address create(std::string const& _auth, bool const& _backup = false);
		bool remove(dev::Address const& _address, std::string const& _auth);
		bool import(mcp::json const& _json, key_content & _kc);
		mcp::key_content importRawKey(dev::Secret & _prv, std::string const& _auth);
		std::pair<bool, Secret> DecryptKey(dev::Address const& _address, std::string const& _auth);
		bool unlock(dev::Address const& _address, std::string const& _auth);
		void write_backup(dev::Address const& _address, mcp::json const& _json);
		void lock(dev::Address const& _address);
		std::pair<bool, Secret> authenticate(dev::Address const& _address, boost::optional<std::string> const& _auth = boost::none);
	private:
		void add_or_update_key(mcp::key_content const& _kc, bool const& _backup = false);

		boost::filesystem::path m_backup_path;

		std::unordered_map<dev::Address, mcp::key_content> m_key_contents;
		std::mutex m_key_contents_mutex;

		std::unordered_map<dev::Address, dev::Secret> m_unlocked;
		std::mutex m_unlocked_mutex;
		mcp::key_store m_store;
	};
}
