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

		if (!error_a)
		{
			std::string account_text = js["account"];
			error_a = account.decode_account(account_text);

			if (!error_a)
			{
				std::string kdf_salt_text = js["kdf_salt"];
				error_a = kdf_salt.decode_hex(kdf_salt_text);

				if (!error_a)
				{
					std::string iv_text = js["iv"];
					error_a = iv.decode_hex(iv_text);

					if (!error_a)
					{
						std::string ciphertext_text = js["ciphertext"];
						error_a = ciphertext.decode_hex(ciphertext_text);
					}
				}
			}
		}
	}
	catch (std::exception const &e)
	{
		error_a = true;
	}
}

mcp::key_content::key_content(mcp::account const & account_a, mcp::uint128_union const & kdf_salt_a,
	mcp::uint128_union const & iv_a, mcp::secret_ciphertext const & ciphertext_a) :
	account(account_a),
	kdf_salt(kdf_salt_a),
	iv(iv_a),
	ciphertext(ciphertext_a)
{
}

dev::Slice mcp::key_content::val() const
{
	//return mcp::mdb_val(sizeof(*this), const_cast<mcp::key_content *> (this));
	return dev::Slice((char*)this, sizeof(*this));
}

std::string mcp::key_content::to_json() const
{
	mcp::json js;
	js["account"] = account.to_account();
	js["kdf_salt"] = kdf_salt.to_string();
	js["iv"] = iv.to_string();
	js["ciphertext"] = ciphertext.to_string();

	std::string str_json = js.dump();
	return str_json;
}

mcp::value_previous_work::value_previous_work(dev::Slice const & val_a)
{
	assert_x(val_a.size() == sizeof(*this));
	std::copy(reinterpret_cast<uint8_t const *> (val_a.data()), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(previous), previous.chars.begin());
	std::copy(reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(previous), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(previous) + sizeof(work), work.bytes.begin());
}

mcp::value_previous_work::value_previous_work(mcp::uint256_union const & previous_a, mcp::uint64_union const & work_a) :
	previous(previous_a),
	work(work_a)
{
}

dev::Slice mcp::value_previous_work::val() const
{
	static_assert (sizeof(*this) == sizeof(previous) + sizeof(work), "Class not packed");
	return dev::Slice((char*)this, sizeof(*this));
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
	m_work = m_database->set_column_family(default_col, "w");
	error_a = !m_database->open();
	if(error_a)
		std::cerr << "Key store db open error" << std::endl;
}

//keys
void mcp::key_store::keys_put(mcp::db::db_transaction& transaction, mcp::public_key const& _k, mcp::key_content const& _v)
{
	transaction.put(m_keys, mcp::uint256_to_slice(_k), _v.val());
}

bool mcp::key_store::keys_get(mcp::db::db_transaction& transaction, mcp::public_key const& _k, mcp::key_content& _v)
{
	std::string result;
	bool ret = transaction.get(m_keys, mcp::uint256_to_slice(_k), result);
	if (ret)
		_v = mcp::key_content(dev::Slice(result));
	return !ret;
}

void mcp::key_store::keys_del(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	transaction.del(m_keys, mcp::uint256_to_slice(_k));
}

bool mcp::key_store::keys_exists(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	return transaction.exists(m_keys, mcp::uint256_to_slice(_k));;
}

mcp::db::forward_iterator mcp::key_store::keys_begin(mcp::db::db_transaction& transaction)
{
	return transaction.begin(m_keys);
}

mcp::db::forward_iterator mcp::key_store::keys_begin(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	return transaction.begin(m_keys, mcp::uint256_to_slice(_k));
}

mcp::db::backward_iterator mcp::key_store::keys_rbegin(mcp::db::db_transaction& transaction)
{
	return transaction.rbegin(m_keys);
}

mcp::db::backward_iterator mcp::key_store::keys_rbegin(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	return transaction.rbegin(m_keys, mcp::uint256_to_slice(_k));
}

//work
void mcp::key_store::work_put(mcp::db::db_transaction& transaction, mcp::public_key const & _k, mcp::value_previous_work const & _v)
{
	transaction.put(m_work, mcp::uint256_to_slice(_k), _v.val());
}

bool mcp::key_store::work_get(mcp::db::db_transaction& transaction, mcp::public_key const & _k, mcp::value_previous_work & _v)
{
	std::string result;
	bool ret = transaction.get(m_work, mcp::uint256_to_slice(_k), result);
	if (ret)
		_v = mcp::value_previous_work(dev::Slice(result));
	return !ret;
}

void mcp::key_store::work_del(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	transaction.del(m_work, mcp::uint256_to_slice(_k));
}

bool mcp::key_store::work_exists(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	return transaction.exists(m_work, mcp::uint256_to_slice(_k));;
}

mcp::db::forward_iterator mcp::key_store::work_begin(mcp::db::db_transaction& transaction)
{
	return transaction.begin(m_work);
}

mcp::db::forward_iterator mcp::key_store::work_begin(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	return transaction.begin(m_work, mcp::uint256_to_slice(_k));
}

mcp::db::backward_iterator mcp::key_store::work_rbegin(mcp::db::db_transaction& transaction)
{
	return transaction.rbegin(m_work);
}

mcp::db::backward_iterator mcp::key_store::work_rbegin(mcp::db::db_transaction& transaction, mcp::public_key const & _k)
{
	return transaction.rbegin(m_work, mcp::uint256_to_slice(_k));
}


