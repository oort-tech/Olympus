#include "key_store.hpp"


mcp::key_content::key_content()
{
}

mcp::key_content::key_content(dev::Slice const & val_a)
{
	assert_x(val_a.size() == sizeof(*this));
	std::copy(reinterpret_cast<uint8_t const *> (val_a.data()), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(*this), reinterpret_cast<uint8_t *> (this));
}

mcp::key_content::key_content(bool & error_a, std::string const & json_a)
{
	try
	{
		mcp::json js = mcp::json::parse(json_a);
		account = dev::Address(js["account"].get<std::string>());
		kdf_salt = dev::h128(js["kdf_salt"].get<std::string>());
		iv = dev::h128(js["iv"].get<std::string>());
		ciphertext = dev::h256(js["ciphertext"].get<std::string>());
	}
	catch (...)
	{
		error_a = true;
	}
}

mcp::key_content::key_content(
	dev::Address const & account,
	dev::h128 const & kdf_salt_a,
	dev::h128 const & iv_a,
	dev::h256 const & ciphertext_a) :
	account(account),
	kdf_salt(kdf_salt_a),
	iv(iv_a),
	ciphertext(ciphertext_a)
{
}

dev::Slice mcp::key_content::val() const
{
	return dev::Slice((char*)this, sizeof(*this));
}

std::string mcp::key_content::to_json() const
{
	mcp::json js;
	js["account"] = account.hexPrefixed();
	js["kdf_salt"] = kdf_salt.hex();
	js["iv"] = iv.hex();
	js["ciphertext"] = ciphertext.hex();
	return js.dump();
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
	transaction.put(m_keys, mcp::account_to_slice(_k), _v.val());
}

bool mcp::key_store::keys_get(mcp::db::db_transaction& transaction, dev::Address const& _k, mcp::key_content& _v)
{
	std::string result;
	bool ret = transaction.get(m_keys, mcp::account_to_slice(_k), result);
	if (ret)
		_v = mcp::key_content(dev::Slice(result));
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



