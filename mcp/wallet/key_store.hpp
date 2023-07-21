#pragma once

#include <mcp/core/common.hpp>
#include <boost/uuid/uuid.hpp>

namespace mcp
{
	namespace Key
	{
		const int version = 3;
		const std::string keyHeaderKDF = "scrypt";
		// StandardScryptN is the N parameter of Scrypt encryption algorithm, using 256MB
		// memory and taking approximately 1s CPU time on a modern processor.
		const int StandardScryptN = 1 << 18;

		// StandardScryptP is the P parameter of Scrypt encryption algorithm, using 256MB
		// memory and taking approximately 1s CPU time on a modern processor.
		const int StandardScryptP = 1;

		const int scryptR = 8;
	}

	class cipherparamsJSON
	{
	public:
		cipherparamsJSON(){}
		cipherparamsJSON(dev::RLP const & _r);
		cipherparamsJSON(mcp::json const & json_a);

		cipherparamsJSON(dev::h128 const& _iv):IV(_iv)
		{}
		void streamRLP(dev::RLPStream & s) const;
		mcp::json to_json() const;
	
		dev::h128 IV;
	};

	class scryptParamsJSON
	{
	public:
		scryptParamsJSON(){}
		scryptParamsJSON(dev::RLP const & _r);
		scryptParamsJSON(mcp::json const & json_a);

		scryptParamsJSON(int const& _n, int const& _r, int const& _p, int const& _dklen, dev::h256 const& _salt):
			N(_n), R(_r), P(_p), DKLen(_dklen), Salt(_salt)
		{}
		void streamRLP(dev::RLPStream & s) const;
		mcp::json to_json() const;

		int N;
		int R;
		int P;
		int DKLen;
		dev::h256 Salt;
	};

	class CryptoJSON
	{
	public:
		CryptoJSON(){}
		CryptoJSON(dev::RLP const & _r);
		CryptoJSON(mcp::json const & json_a);

		CryptoJSON(dev::h256 const& _CipherText, cipherparamsJSON const& _CipherParams, scryptParamsJSON const& _KDFParams, dev::h256 const& _mac):
			CipherText(_CipherText), CipherParams(_CipherParams), KDFParams(_KDFParams), MAC(_mac)
		{}
		void streamRLP(dev::RLPStream & s) const;
		mcp::json to_json() const;

		std::string Cipher = "aes-128-ctr";
		dev::h256 CipherText;
		std::string KDF = Key::keyHeaderKDF;
		h256 MAC;
		cipherparamsJSON CipherParams;
		scryptParamsJSON KDFParams;
	};

	class key_content
	{
	public:
		key_content() {}
		key_content(dev::RLP const & _r);
		key_content(mcp::json const & json_a);
		key_content(dev::Address const& _address, CryptoJSON const& _crypto, boost::uuids::uuid const& _id, int const& _version):
			address(_address), Crypto(_crypto), Id(_id), Version(_version)
		{}
		void streamRLP(dev::RLPStream & s) const;
		mcp::json to_json() const;

		dev::Address address;
		boost::uuids::uuid Id;
		int Version;
		CryptoJSON Crypto;
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