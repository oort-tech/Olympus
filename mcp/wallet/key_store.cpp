#include "key_store.hpp"
#include <libdevcore/CommonJS.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

mcp::cipherparamsJSON::cipherparamsJSON(dev::RLP const & _r)
{
	if (!_r.isList())
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("Key RLP must be a list"));
	if (_r.itemCount() != 1)
		BOOST_THROW_EXCEPTION(InvalidBlockFormat() << errinfo_comment("too many or to low fields in the Key RLP"));

	IV = (dev::h128)_r[0];
}

mcp::cipherparamsJSON::cipherparamsJSON(mcp::json const & json_a)
{
	if (!json_a.count("iv"))
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("The required fields are missing"));

	IV = dev::h128(json_a["iv"].get<std::string>());
}

void mcp::cipherparamsJSON::streamRLP(dev::RLPStream & s) const
{
	s.appendList(1);
	s << IV;
}

mcp::json mcp::cipherparamsJSON::to_json() const
{
	mcp::json res;
	res["iv"] = IV.hex();
	return res;
}

mcp::scryptParamsJSON::scryptParamsJSON(dev::RLP const & _r)
{
	if (!_r.isList())
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("Key RLP must be a list"));
	if (_r.itemCount() != 5)
		BOOST_THROW_EXCEPTION(InvalidBlockFormat() << errinfo_comment("too many or to low fields in the Key RLP"));

	N = _r[0].toInt();
	R = _r[1].toInt();
	P = _r[2].toInt();
	DKLen = _r[3].toInt();
	Salt = (dev::h256)(_r[4]);
}

mcp::scryptParamsJSON::scryptParamsJSON(mcp::json const & json_a)
{
	if (!json_a.count("n") || !json_a.count("r") ||
		!json_a.count("p") || !json_a.count("dklen") ||
		!json_a.count("salt"))
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("The required fields are missing"));

	N = json_a["n"].get<int>();
	R = json_a["r"].get<int>();
	P = json_a["p"].get<int>();
	DKLen = json_a["dklen"].get<int>();
	Salt = dev::h256(json_a["salt"].get<std::string>());
}

void mcp::scryptParamsJSON::streamRLP(dev::RLPStream & s) const
{
	s.appendList(5);
	s << N << R << P << DKLen << Salt;
}

mcp::json mcp::scryptParamsJSON::to_json() const
{
	mcp::json res;
	res["salt"] = Salt.hex();
	res["n"] = N;
	res["dklen"] = DKLen;
	res["p"] = P;
	res["r"] = R;
	return res;
}

mcp::CryptoJSON::CryptoJSON(dev::RLP const & _r)
{
	if (!_r.isList())
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("Key RLP must be a list"));
	if (_r.itemCount() != 6)
		BOOST_THROW_EXCEPTION(InvalidBlockFormat() << errinfo_comment("too many or to low fields in the Key RLP"));

	Cipher = (std::string)_r[0];
	CipherText = (dev::h256)_r[1];
	KDF = (std::string)_r[2];
	MAC = (dev::h256)(_r[3]);
	CipherParams = cipherparamsJSON(_r[4]);
	KDFParams = scryptParamsJSON(_r[5]);
}

mcp::CryptoJSON::CryptoJSON(mcp::json const & json_a)
{
	if (!json_a.count("cipher") || !json_a.count("ciphertext") ||
		!json_a.count("kdf") || !json_a.count("mac") ||
		!json_a.count("cipherparams") || !json_a.count("kdfparams"))
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("The required fields are missing"));

	Cipher = json_a["cipher"].get<std::string>();
	CipherText = dev::h256(json_a["ciphertext"].get<std::string>());
	KDF = json_a["kdf"].get<std::string>();
	MAC = dev::h256(json_a["mac"].get<std::string>());
	CipherParams = cipherparamsJSON(json_a["cipherparams"]);
	KDFParams = scryptParamsJSON(json_a["kdfparams"]);
}

void mcp::CryptoJSON::streamRLP(dev::RLPStream & s) const
{
	s.appendList(6);
	s << Cipher << CipherText << KDF << MAC;
	CipherParams.streamRLP(s);
	KDFParams.streamRLP(s);
}

mcp::json mcp::CryptoJSON::to_json() const
{
	mcp::json res;
	res["cipher"] = Cipher;
	res["cipherparams"] = CipherParams.to_json();
	res["ciphertext"] = CipherText.hex();
	res["kdf"] = KDF;
	res["kdfparams"] = KDFParams.to_json();
	res["mac"] = MAC.hex();
	return res;
}

mcp::key_content::key_content(dev::RLP const & _r)
{
	if (!_r.isList())
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("Key RLP must be a list"));
	if (_r.itemCount() != 4)
		BOOST_THROW_EXCEPTION(InvalidBlockFormat() << errinfo_comment("too many or to low fields in the Key RLP"));

	address = (Address)_r[0];
	Id = boost::uuids::string_generator()(_r[1].toString());
	Version = _r[2].toInt();
	Crypto = CryptoJSON(_r[3]);
}

mcp::key_content::key_content(mcp::json const & json_a)
{
	if (!json_a.count("address") || !json_a.count("id") || 
		!json_a.count("version") || !json_a.count("crypto"))
		BOOST_THROW_EXCEPTION(InvalidKeyContentFormat() << errinfo_comment("The required fields are missing"));

	address = dev::Address(json_a["address"].get<std::string>());
	Id = boost::uuids::string_generator()(json_a["id"].get<std::string>());
	Version = json_a["version"].get<int>();
	Crypto = CryptoJSON(json_a["crypto"]);
}

void mcp::key_content::streamRLP(dev::RLPStream & s) const
{
	s.appendList(4);
	s << address << boost::uuids::to_string(Id) << Version;
	Crypto.streamRLP(s);
}

mcp::json mcp::key_content::to_json() const
{
	mcp::json js;
	js["address"] = address.hex();
	js["id"] = boost::uuids::to_string(Id);
	js["version"] = Version;
	js["crypto"] = Crypto.to_json();
	return js;
}

//store
mcp::key_store::key_store(bool & error_a, boost::filesystem::path const& _path) :
	m_database(std::make_shared<mcp::db::database>(_path))
{
	if (error_a)
		return;

	auto tbops = mcp::db::db_column::default_table_options(mcp::db::database::get_table_cache());
	auto cfops = mcp::db::db_column::default_column_family_options(tbops);
	cfops->OptimizeForSmallDb();
	cfops->prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(1));

	int default_col = m_database->create_column_family(rocksdb::kDefaultColumnFamilyName, cfops);
	m_keys = m_database->set_column_family(default_col, "k");
	error_a = !m_database->open();
	if(error_a)
		std::cerr << "Key store db open error" << std::endl;
}

//keys
void mcp::key_store::keys_put(mcp::db::db_transaction& transaction, dev::Address const& _k, mcp::key_content const& _v)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		_v.streamRLP(s);
		s.swapOut(b_value);
	}
	dev::Slice s_value((char *)b_value.data(), b_value.size());

	transaction.put(m_keys, mcp::account_to_slice(_k), s_value);
}

bool mcp::key_store::keys_get(mcp::db::db_transaction& transaction, dev::Address const& _k, mcp::key_content& _v)
{
	std::string result;
	bool ret = transaction.get(m_keys, mcp::account_to_slice(_k), result);
	if (ret)
	{
		dev::RLP r(result);
		_v = mcp::key_content(r);
	}
	return !ret;
}

void mcp::key_store::keys_del(mcp::db::db_transaction& transaction, dev::Address const & _k)
{
	transaction.del(m_keys, mcp::account_to_slice(_k));
}

bool mcp::key_store::keys_exists(mcp::db::db_transaction& transaction, dev::Address const & _k)
{
	return transaction.exists(m_keys, mcp::account_to_slice(_k));;
}

mcp::db::forward_iterator mcp::key_store::keys_begin(mcp::db::db_transaction& transaction)
{
	return transaction.begin(m_keys);
}

mcp::db::forward_iterator mcp::key_store::keys_begin(mcp::db::db_transaction& transaction, dev::Address const & _k)
{
	return transaction.begin(m_keys, mcp::account_to_slice(_k));
}

mcp::db::backward_iterator mcp::key_store::keys_rbegin(mcp::db::db_transaction& transaction)
{
	return transaction.rbegin(m_keys);
}

mcp::db::backward_iterator mcp::key_store::keys_rbegin(mcp::db::db_transaction& transaction, dev::Address const & _k)
{
	return transaction.rbegin(m_keys, mcp::account_to_slice(_k));
}


