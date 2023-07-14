#include "key_manager.hpp"
#include <mcp/common/common.hpp>

#include <cryptopp/scrypt.h>
#include <cryptopp/sha.h>
#include <cryptopp/hkdf.h>
#include <boost/uuid/uuid_generators.hpp>

mcp::key_manager::key_manager(boost::filesystem::path const& _path, mcp::key_store& _store):
	m_backup_path(_path / "backup"),
	m_store(_store)
{
	auto transaction = m_store.create_transaction();
	auto it = m_store.keys_begin(transaction);
	for (; it.valid(); ++it)
	{
		dev::Address account(mcp::slice_to_account(it.key()));
		mcp::key_content key_content(dev::RLP(bytesConstRef((byte const*)it.value().data(), it.value().size())));
		m_key_contents[account] = key_content;
	}

	boost::filesystem::create_directories(m_backup_path);
}

bool mcp::key_manager::exists(dev::Address const& _address)
{
	std::lock_guard<std::mutex> lock(m_key_contents_mutex);
	return m_key_contents.count(_address);
}

bool mcp::key_manager::find(dev::Address const& _address, mcp::key_content & _kc)
{
	std::lock_guard<std::mutex> lock(m_key_contents_mutex);
	if (m_key_contents.count(_address))
	{
		_kc = m_key_contents[_address];
		return true;
	}
	return false;
}

Addresses mcp::key_manager::list()
{
	Addresses accounts;
	std::lock_guard<std::mutex> lock(m_key_contents_mutex);
	for (auto pair : m_key_contents)
		accounts.push_back(pair.first);
	return accounts;
}

dev::Address mcp::key_manager::create(std::string const& _auth, bool const& _backup)
{
	dev::Secret prv;
	random_pool.GenerateBlock((byte*)prv.data(), prv.size);
	CryptoJSON cryptoStruct = EncryptDataV3(prv, _auth, Key::StandardScryptN, Key::StandardScryptP);

	boost::uuids::uuid id = boost::uuids::random_generator()();
	mcp::key_content kc(dev::toAddress(dev::toPublic(prv)), cryptoStruct, id, Key::version);
	add_or_update_key(kc, _backup);

	return kc.address;
}

bool mcp::key_manager::remove(dev::Address const& _address, std::string const& _auth)
{
	std::pair<bool, Secret> _k = DecryptKey(_address, _auth);
	if (_k.first)
	{
		{
			std::lock_guard<std::mutex> lock(m_unlocked_mutex);
			m_unlocked.erase(_address);
		}
		{
			std::lock_guard<std::mutex> lock(m_key_contents_mutex);
			m_key_contents.erase(_address);
		}
		mcp::db::db_transaction transaction = m_store.create_transaction();
		m_store.keys_del(transaction, _address);
	}

	return _k.first;
}

bool mcp::key_manager::import(mcp::json const& _json, key_content & _kc)
{
	try
	{
		mcp::key_content kc(_json);
		if (kc.Version != Key::version)
			return false;
		add_or_update_key(kc);
		_kc = kc;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

mcp::key_content mcp::key_manager::importRawKey(dev::Secret & _prv, std::string const& _auth)
{
	CryptoJSON cryptoStruct = EncryptDataV3(_prv, _auth, Key::StandardScryptN, Key::StandardScryptP);
	boost::uuids::uuid id = boost::uuids::random_generator()();
	mcp::key_content kc(dev::toAddress(dev::toPublic(_prv)), cryptoStruct, id, Key::version);
	add_or_update_key(kc, true);
	return kc;
}

std::pair<bool, Secret> mcp::key_manager::DecryptKey(dev::Address const& _address, std::string const& _auth)
{
	mcp::key_content kc;
	{
		std::lock_guard<std::mutex> lock(m_key_contents_mutex);
		if (m_key_contents.count(_address))
			kc = m_key_contents[_address];
		else
			return std::make_pair(false, dev::Secret());
	}

	std::pair<bool, Secret> _k = DecryptDataV3(kc.Crypto, _auth);
	if (!_k.first)
		return std::make_pair(false, dev::Secret());

	if (kc.address != dev::toAddress(dev::toPublic(_k.second)))
		return std::make_pair(false, dev::Secret());

	return std::make_pair(true, _k.second);;
}

void mcp::key_manager::lock(dev::Address const& _address)
{
	std::lock_guard<std::mutex> lock(m_unlocked_mutex);
	m_unlocked.erase(_address);
}

bool mcp::key_manager::unlock(dev::Address const& _address, std::string const& _auth)
{
	std::pair<bool, Secret> _k = DecryptKey(_address, _auth);
	if (_k.first)
	{
		std::lock_guard<std::mutex> lock(m_unlocked_mutex);
		m_unlocked[_address] = _k.second;
	}

	return _k.first;
}

void mcp::key_manager::write_backup(dev::Address const& _address, mcp::json const& _json)
{
	std::ofstream backup_file;
	std::string file_name((m_backup_path / (_address.hexPrefixed() + ".json")).string());
	backup_file.open(file_name);
	if (!backup_file.fail())
	{
		backup_file << _json.dump();
	}
}


void mcp::key_manager::add_or_update_key(mcp::key_content const& _kc, bool const& _backup)
{
	{
		std::lock_guard<std::mutex> lock(m_key_contents_mutex);
		m_key_contents[_kc.address] = _kc;
	}

	mcp::db::db_transaction transaction = m_store.create_transaction();
	m_store.keys_put(transaction, _kc.address, _kc);

	if (_backup)
		write_backup(_kc.address, _kc.to_json());
}

std::pair<bool, Secret> mcp::key_manager::authenticate(dev::Address const& _address, boost::optional<std::string> const& _auth)
{
	std::pair<bool, Secret> ret;
	
	if (m_key_contents.count(_address))
	{
		if (m_unlocked.count(_address))
		{
			ret = std::make_pair(true, m_unlocked[_address]);
		}
		else if (_auth)
		{
			std::pair<bool, Secret> _k = DecryptKey(_address, *_auth);
			if (!_k.first)
				BOOST_THROW_EXCEPTION(AccountLocked());
			ret = std::make_pair(true, _k.second);
		}
		else
			BOOST_THROW_EXCEPTION(AccountLocked());
	}
	else
		BOOST_THROW_EXCEPTION(UnknownAccount());
	
	return ret;
}

mcp::CryptoJSON mcp::EncryptDataV3(dev::Secret const& _data, std::string const& _auth, int _scryptN, int _scryptP)
{
	///KDF salt
	dev::h256 salt;
	random_pool.GenerateBlock(salt.data(), salt.size);
	/// RFC 7914. 
	dev::Secret derivedKey;
	CryptoPP::Scrypt scrypt;
	scrypt.DeriveKey((byte*)derivedKey.data(), derivedKey.size,
		(byte*)_auth.data(), _auth.length(),
		salt.data(), salt.size,
		_scryptN,
		Key::scryptR,
		_scryptP);
	scryptParamsJSON spj(_scryptN, Key::scryptR, _scryptP, derivedKey.size, salt);

	///aes encrypt key
	dev::h128 encryptKey(derivedKey.ref().cropped(0, dev::h128::size));
	///aes iv
	dev::h128 iv;
	random_pool.GenerateBlock(iv.data(), iv.size);
	cipherparamsJSON cpj(iv);
	/// aes encrypt
	dev::h256 ciphertext;
	bytesConstRef _dataRef = _data.ref();
	mcp::encry::aesCTRXOR(ciphertext, encryptKey, iv, _dataRef);
	///
	dev::h256 mac = dev::sha3(dev::h128(derivedKey.ref().cropped(dev::h128::size)).asBytes() + ciphertext.asBytes());

	return mcp::CryptoJSON(ciphertext, cpj, spj, mac);
}

std::pair<bool, dev::Secret> mcp::DecryptDataV3(mcp::CryptoJSON const& _cryptoJson, std::string const& _auth)
{
	if (_cryptoJson.Cipher != "aes-128-ctr")
		return std::make_pair(false, dev::Secret());
	std::pair<bool, dev::Secret> _k = GetKDFKey(_cryptoJson, _auth);
	if (!_k.first)
		return std::make_pair(false, dev::Secret());
	dev::Secret derivedKey = _k.second;
	dev::h256 calculatedMAC = dev::sha3(dev::h128(derivedKey.ref().cropped(dev::h128::size)).asBytes() + _cryptoJson.CipherText.asBytes());
	if (calculatedMAC != _cryptoJson.MAC)
		return std::make_pair(false, dev::Secret());

	///aes encrypt key
	dev::h128 encryptKey(derivedKey.ref().cropped(0, dev::h128::size));
	dev::h256 plainText;
	bytesConstRef ciphertextRef = _cryptoJson.CipherText.ref();
	mcp::encry::aesCTRXOR(plainText, encryptKey, _cryptoJson.CipherParams.IV, ciphertextRef);

	return std::make_pair(true, dev::Secret(plainText.ref()));
}

std::pair<bool, dev::Secret> mcp::GetKDFKey(mcp::CryptoJSON const& _cryptoJson, std::string const& _auth)
{
	if (_cryptoJson.KDF == Key::keyHeaderKDF)///rfc7914, Scrypt
	{
		dev::Secret derivedKey;
		CryptoPP::Scrypt scrypt;
		scrypt.DeriveKey((byte*)derivedKey.data(), derivedKey.size,
			(byte*)_auth.data(), _auth.length(),
			_cryptoJson.KDFParams.Salt.data(), _cryptoJson.KDFParams.Salt.size,
			_cryptoJson.KDFParams.N,
			_cryptoJson.KDFParams.R,
			_cryptoJson.KDFParams.P);

		return std::make_pair(true, derivedKey);
	}

	return std::make_pair(false, dev::Secret());
}
