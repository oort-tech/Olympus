#include "numbers.hpp"

#include <blake2/blake2.h>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

#include <mcp/common/base58.h>
#include <mcp/common/common.hpp>

// added by michael at 1/24
#include <libdevcore/CommonData.h>

thread_local CryptoPP::AutoSeededRandomPool mcp::random_pool;

// void mcp::uint256_union::encode_account(std::string & destination_a) const
// {
// 	if (is_zero())
// 	{
// 		destination_a = "";
// 		return;
// 	}

// 	std::vector<uint8_t> vch;
// 	vch.reserve(33);
// 	vch.push_back(0x01); //version
// 	vch.insert(vch.end(), bytes.begin(), bytes.end());
// 	std::string encoded = EncodeBase58Check(vch);
// 	destination_a = "mcp" + encoded;
// }

// bool mcp::uint256_union::decode_account(std::string const & source_a)
// {
// 	bool error(false);
// 	if (source_a.empty())
// 	{
// 		bytes.fill(0);
// 	}
// 	else if (source_a[0] == 'm' && source_a[1] == 'c' && source_a[2] == 'p')
// 	{
// 		std::vector<uint8_t> vch;
// 		error = !DecodeBase58Check(source_a.substr(3), vch);
// 		if (!error)
// 		{
// 			if (vch.size() == 33) 
// 			{
// 				if (vch[0] == 0x01) // check version
// 					std::copy(vch.begin() + 1, vch.end(), bytes.begin());
// 				else
// 					error = true;
// 			}
// 			else
// 			{
// 				error = true;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		error = true;
// 	}
// 	return error;
// }

// std::string mcp::uint256_union::to_account_split() const
// {
// 	auto result(to_account());
// 	assert_x(result.size() == 64);
// 	result.insert(32, "\n");
// 	return result;
// }

// std::string mcp::uint256_union::to_account() const
// {
// 	std::string result;
// 	encode_account(result);
// 	return result;
// }

mcp::uint256_union::uint256_union(uint64_t value0)
{
	*this = mcp::uint256_t(value0);
}

mcp::uint256_union::uint256_union(mcp::uint256_t const & number_a)
{
	if (number_a == 0)
	{
		memset(bytes.data(), 0, bytes.size());
	}
	else
	{
		mcp::uint256_t number_l(number_a);
		for (auto i(bytes.rbegin()), n(bytes.rend()); i != n; ++i)
		{
			*i = ((number_l) & 0xff).convert_to<uint8_t>();
			number_l >>= 8;
		}
	}
}

// added by michael at 1/14
mcp::uint256_union::uint256_union(mcp::account20_struct const & account) {
	*this = account.number();
}
//

bool mcp::uint256_union::operator== (mcp::uint256_union const & other_a) const
{
	return bytes == other_a.bytes;
}

// Construct a uint256_union = AES_ENC_CTR (cleartext, key, iv)
void mcp::uint256_union::encrypt(mcp::raw_key const & cleartext, mcp::raw_key const & key, uint128_union const & iv)
{
	CryptoPP::AES::Encryption alg(key.data.bytes.data(), sizeof(key.data.bytes));
	CryptoPP::CTR_Mode_ExternalCipher::Encryption enc(alg, iv.bytes.data());
	enc.ProcessData(bytes.data(), cleartext.data.bytes.data(), sizeof(cleartext.data.bytes));
}

bool mcp::uint256_union::is_zero() const
{
	return qwords[0] == 0 && qwords[1] == 0 && qwords[2] == 0 && qwords[3] == 0;
}

std::string mcp::uint256_union::to_string() const
{
	std::string result;
	encode_hex(result);
	return result;
}

bool mcp::uint256_union::operator< (mcp::uint256_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::uint256_union::operator> (mcp::uint256_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::uint256_union::operator<= (mcp::uint256_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return true;
}

bool mcp::uint256_union::operator>= (mcp::uint256_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return true;
}

mcp::uint256_union & mcp::uint256_union::operator^= (mcp::uint256_union const & other_a)
{
	auto j(other_a.qwords.begin());
	for (auto i(qwords.begin()), n(qwords.end()); i != n; ++i, ++j)
	{
		*i ^= *j;
	}
	return *this;
}

mcp::uint256_union mcp::uint256_union::operator^ (mcp::uint256_union const & other_a) const
{
	mcp::uint256_union result;
	auto k(result.qwords.begin());
	for (auto i(qwords.begin()), j(other_a.qwords.begin()), n(qwords.end()); i != n; ++i, ++j, ++k)
	{
		*k = *i ^ *j;
	}
	return result;
}

mcp::uint256_union::uint256_union(std::string const & hex_a)
{
	decode_hex(hex_a);
}

void mcp::uint256_union::clear()
{
	qwords.fill(0);
}

mcp::uint256_t mcp::uint256_union::number() const
{
	mcp::uint256_t result;
	auto shift(0);
	for (auto i(bytes.begin()), n(bytes.end()); i != n; ++i)
	{
		result <<= shift;
		result |= *i;
		shift = 8;
	}
	return result;
}

void mcp::uint256_union::encode_hex(std::string & text) const
{
	assert_x(text.empty());
	std::stringstream stream;
	stream << std::hex << std::noshowbase << std::setw(64) << std::setfill('0');
	stream << number();
	text = stream.str();
}

bool mcp::uint256_union::decode_hex(std::string const & text)
{
	auto error(false);
	if (!text.empty() && text.size() <= 64)
	{
		std::stringstream stream(text);
		stream << std::hex << std::noshowbase;
		mcp::uint256_t number_l;
		try
		{
			stream >> number_l;
			*this = number_l;
			if (!stream.eof())
			{
				error = true;
			}
		}
		catch (std::runtime_error &)
		{
			error = true;
		}
	}
	else
	{
		error = true;
	}
	return error;
}

void mcp::uint256_union::encode_dec(std::string & text) const
{
	assert_x(text.empty());
	std::stringstream stream;
	stream << std::dec << std::noshowbase;
	stream << number();
	text = stream.str();
}

bool mcp::uint256_union::decode_dec(std::string const & text)
{
	auto error(text.size() > 78 || (text.size() > 1 && text[0] == '0') || (text.size() > 0 && text[0] == '-'));
	if (!error)
	{
		std::stringstream stream(text);
		stream << std::dec << std::noshowbase;
		mcp::uint256_t number_l;
		try
		{
			stream >> number_l;
			*this = number_l;
			if (!stream.eof())
			{
				error = true;
			}
		}
		catch (std::runtime_error &)
		{
			error = true;
		}
	}
	return error;
}

bool mcp::uint256_union::operator!= (mcp::uint256_union const & other_a) const
{
	return !(*this == other_a);
}

std::string mcp::uint256_union::to_string_dec() const
{
	std::string result;
	encode_dec(result);
	return result;
}


mcp::raw_key::raw_key(mcp::private_key const & prv_a) :
	data(prv_a)
{
}

mcp::raw_key::~raw_key()
{
	data.clear();
}

bool mcp::raw_key::operator== (mcp::raw_key const & other_a) const
{
	return data == other_a.data;
}

bool mcp::raw_key::operator!= (mcp::raw_key const & other_a) const
{
	return !(*this == other_a);
}

// This this = AES_DEC_CTR (ciphertext, key, iv)
void mcp::raw_key::decrypt(mcp::uint256_union const & ciphertext, mcp::raw_key const & key_a, uint128_union const & iv)
{
	CryptoPP::AES::Encryption alg(key_a.data.bytes.data(), sizeof(key_a.data.bytes));
	CryptoPP::CTR_Mode_ExternalCipher::Decryption dec(alg, iv.bytes.data());
	dec.ProcessData(data.bytes.data(), ciphertext.bytes.data(), sizeof(ciphertext.bytes));
}


bool mcp::uint512_union::operator== (mcp::uint512_union const & other_a) const
{
	return bytes == other_a.bytes;
}

// Added by Daniel
bool mcp::uint512_union::operator< (mcp::uint512_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::uint512_union::operator> (mcp::uint512_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::uint512_union::operator<= (mcp::uint512_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return true;
}

bool mcp::uint512_union::operator>= (mcp::uint512_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return true;
}

mcp::uint512_union mcp::uint512_union::operator^ (mcp::uint512_union const & other_a) const
{
	mcp::uint512_union result;
	auto k(result.qwords.begin());
	for (auto i(qwords.begin()), j(other_a.qwords.begin()), n(qwords.end()); i != n; ++i, ++j, ++k)
	{
		*k = *i ^ *j;
	}
	return result;
}

mcp::uint512_union::uint512_union(mcp::uint512_t const & number_a)
{
	mcp::uint512_t number_l(number_a);
	for (auto i(bytes.rbegin()), n(bytes.rend()); i != n; ++i)
	{
		*i = ((number_l) & 0xff).convert_to<uint8_t>();
		number_l >>= 8;
	}
}

void mcp::uint512_union::clear()
{
	bytes.fill(0);
}

mcp::uint512_t mcp::uint512_union::number() const
{
	mcp::uint512_t result;
	auto shift(0);
	for (auto i(bytes.begin()), n(bytes.end()); i != n; ++i)
	{
		result <<= shift;
		result |= *i;
		shift = 8;
	}
	return result;
}

void mcp::uint512_union::encode_hex(std::string & text) const
{
	assert_x(text.empty());
	std::stringstream stream;
	stream << std::hex << std::noshowbase << std::setw(128) << std::setfill('0');
	stream << number();
	text = stream.str();
}

bool mcp::uint512_union::decode_hex(std::string const & text)
{
	auto error(text.size() > 128 || text.empty());
	if (!error)
	{
		std::stringstream stream(text);
		stream << std::hex << std::noshowbase;
		mcp::uint512_t number_l;
		try
		{
			stream >> number_l;
			*this = number_l;
			if (!stream.eof())
			{
				error = true;
			}
		}
		catch (std::runtime_error &)
		{
			error = true;
		}
	}
	return error;
}

bool mcp::uint512_union::is_zero() const
{
	return qwords[0] == 0 && qwords[1] == 0 && qwords[2] == 0 && qwords[3] == 0 && qwords[4] == 0 && qwords[5] == 0 && qwords[6] == 0 && qwords[7] == 0;
}

bool mcp::uint512_union::operator!= (mcp::uint512_union const & other_a) const
{
	return !(*this == other_a);
}

mcp::uint512_union & mcp::uint512_union::operator^= (mcp::uint512_union const & other_a)
{
	uint256s[0] ^= other_a.uint256s[0];
	uint256s[1] ^= other_a.uint256s[1];
	return *this;
}

std::string mcp::uint512_union::to_string() const
{
	std::string result;
	encode_hex(result);
	return result;
}

mcp::signature mcp::sign_message(mcp::raw_key const & private_key, mcp::public_key const & public_key, mcp::uint256_union const & message)
{
	mcp::signature result;
	mcp::encry::sign(private_key.data, public_key, message.ref(), result);
	return result;
}

bool mcp::validate_message(mcp::public_key const & public_key, mcp::uint256_union const & message, mcp::signature const & signature)
{
	return mcp::encry::verify(public_key, signature, message.ref());
}

mcp::uint128_union::uint128_union(std::string const & string_a)
{
	decode_hex(string_a);
}

mcp::uint128_union::uint128_union(uint64_t value_a)
{
	*this = mcp::uint128_t(value_a);
}

mcp::uint128_union::uint128_union(mcp::uint128_t const & value_a)
{
	mcp::uint128_t number_l(value_a);
	for (auto i(bytes.rbegin()), n(bytes.rend()); i != n; ++i)
	{
		*i = ((number_l) & 0xff).convert_to<uint8_t>();
		number_l >>= 8;
	}
}

bool mcp::uint128_union::operator== (mcp::uint128_union const & other_a) const
{
	return qwords[0] == other_a.qwords[0] && qwords[1] == other_a.qwords[1];
}

bool mcp::uint128_union::operator!= (mcp::uint128_union const & other_a) const
{
	return !(*this == other_a);
}

bool mcp::uint128_union::operator< (mcp::uint128_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::uint128_union::operator> (mcp::uint128_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return false;
}

mcp::uint128_t mcp::uint128_union::number() const
{
	mcp::uint128_t result;
	auto shift(0);
	for (auto i(bytes.begin()), n(bytes.end()); i != n; ++i)
	{
		result <<= shift;
		result |= *i;
		shift = 8;
	}
	return result;
}

void mcp::uint128_union::encode_hex(std::string & text) const
{
	assert_x(text.empty());
	std::stringstream stream;
	stream << std::hex << std::noshowbase << std::setw(32) << std::setfill('0');
	stream << number();
	text = stream.str();
}

bool mcp::uint128_union::decode_hex(std::string const & text)
{
	auto error(text.size() > 32);
	if (!error)
	{
		std::stringstream stream(text);
		stream << std::hex << std::noshowbase;
		mcp::uint128_t number_l;
		try
		{
			stream >> number_l;
			*this = number_l;
			if (!stream.eof())
			{
				error = true;
			}
		}
		catch (std::runtime_error &)
		{
			error = true;
		}
	}
	return error;
}

void mcp::uint128_union::encode_dec(std::string & text) const
{
	assert_x(text.empty());
	std::stringstream stream;
	stream << std::dec << std::noshowbase;
	stream << number();
	text = stream.str();
}

bool mcp::uint128_union::decode_dec(std::string const & text)
{
	auto error(text.size() > 39 || (text.size() > 1 && text[0] == '0') || (text.size() > 0 && text[0] == '-'));
	if (!error)
	{
		std::stringstream stream(text);
		stream << std::dec << std::noshowbase;
		boost::multiprecision::checked_uint128_t number_l;
		try
		{
			stream >> number_l;
			mcp::uint128_t unchecked(number_l);
			*this = unchecked;
			if (!stream.eof())
			{
				error = true;
			}
		}
		catch (std::runtime_error &)
		{
			error = true;
		}
	}
	return error;
}

void format_frac(std::ostringstream & stream, mcp::uint128_t value, mcp::uint128_t scale, int precision)
{
	auto reduce = scale;
	auto rem = value;
	while (reduce > 1 && rem > 0 && precision > 0)
	{
		reduce /= 10;
		auto val = rem / reduce;
		rem -= val * reduce;
		stream << val;
		precision--;
	}
}

void format_dec(std::ostringstream & stream, mcp::uint128_t value, char group_sep, const std::string & groupings)
{
	auto largestPow10 = mcp::uint256_t(1);
	int dec_count = 1;
	while (1)
	{
		auto next = largestPow10 * 10;
		if (next > value)
		{
			break;
		}
		largestPow10 = next;
		dec_count++;
	}

	if (dec_count > 39)
	{
		// Impossible.
		return;
	}

	// This could be cached per-locale.
	bool emit_group[39];
	if (group_sep != 0)
	{
		int group_index = 0;
		int group_count = 0;
		for (int i = 0; i < dec_count; i++)
		{
			group_count++;
			if (group_count > groupings[group_index])
			{
				group_index = std::min(group_index + 1, (int)groupings.length() - 1);
				group_count = 1;
				emit_group[i] = true;
			}
			else
			{
				emit_group[i] = false;
			}
		}
	}

	auto reduce = mcp::uint128_t(largestPow10);
	mcp::uint128_t rem = value;
	while (reduce > 0)
	{
		auto val = rem / reduce;
		rem -= val * reduce;
		stream << val;
		dec_count--;
		if (group_sep != 0 && emit_group[dec_count] && reduce > 1)
		{
			stream << group_sep;
		}
		reduce /= 10;
	}
}

std::string format_balance(mcp::uint128_t balance, mcp::uint128_t scale, int precision, bool group_digits, char thousands_sep, char decimal_point, std::string & grouping)
{
	std::ostringstream stream;
	auto int_part = balance / scale;
	auto frac_part = balance % scale;
	auto prec_scale = scale;
	for (int i = 0; i < precision; i++)
	{
		prec_scale /= 10;
	}
	if (int_part == 0 && frac_part > 0 && frac_part / prec_scale == 0)
	{
		// Display e.g. "< 0.01" rather than 0.
		stream << "< ";
		if (precision > 0)
		{
			stream << "0";
			stream << decimal_point;
			for (int i = 0; i < precision - 1; i++)
			{
				stream << "0";
			}
		}
		stream << "1";
	}
	else
	{
		format_dec(stream, int_part, group_digits && grouping.length() > 0 ? thousands_sep : 0, grouping);
		if (precision > 0 && frac_part > 0)
		{
			stream << decimal_point;
			format_frac(stream, frac_part, scale, precision);
		}
	}
	return stream.str();
}

std::string mcp::uint128_union::format_balance(mcp::uint128_t scale, int precision, bool group_digits)
{
	auto thousands_sep = std::use_facet<std::numpunct<char>>(std::locale()).thousands_sep();
	auto decimal_point = std::use_facet<std::numpunct<char>>(std::locale()).decimal_point();
	std::string grouping = "\3";
	return ::format_balance(number(), scale, precision, group_digits, thousands_sep, decimal_point, grouping);
}

std::string mcp::uint128_union::format_balance(mcp::uint128_t scale, int precision, bool group_digits, const std::locale & locale)
{
	auto thousands_sep = std::use_facet<std::moneypunct<char>>(locale).thousands_sep();
	auto decimal_point = std::use_facet<std::moneypunct<char>>(locale).decimal_point();
	std::string grouping = std::use_facet<std::moneypunct<char>>(locale).grouping();
	return ::format_balance(number(), scale, precision, group_digits, thousands_sep, decimal_point, grouping);
}

void mcp::uint128_union::clear()
{
	qwords.fill(0);
}

bool mcp::uint128_union::is_zero() const
{
	return qwords[0] == 0 && qwords[1] == 0;
}

std::string mcp::uint128_union::to_string() const
{
	std::string result;
	encode_hex(result);
	return result;
}

std::string mcp::uint128_union::to_string_dec() const
{
	std::string result;
	encode_dec(result);
	return result;
}

mcp::uint64_union::uint64_union(uint64_t const & value_a)
{
	uint64_t number_l(value_a);
	for (auto i(bytes.rbegin()), n(bytes.rend()); i != n; ++i)
	{
		*i = (uint8_t)(number_l & 0xff);
		number_l >>= 8;
	}
}

bool mcp::uint64_union::operator==(mcp::uint64_union const & other_a) const
{
	return qwords[0] == other_a.qwords[0];
}

bool mcp::uint64_union::operator!=(mcp::uint64_union const & other_a) const
{
	return qwords[0] != other_a.qwords[0];
}

bool mcp::uint64_union::operator<(mcp::uint64_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::uint64_union::operator>(mcp::uint64_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::uint64_union::operator<=(mcp::uint64_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return true;
}

bool mcp::uint64_union::operator>=(mcp::uint64_union const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return true;
}

uint64_t mcp::uint64_union::number() const
{
	uint64_t result(0);
	auto shift(0);
	for (auto i(bytes.begin()), n(bytes.end()); i != n; ++i)
	{
		result <<= shift;
		result |= *i;
		shift = 8;
	}
	return result;
}

void mcp::uint64_union::encode_hex(std::string & text) const
{
	assert_x(text.empty());
	std::stringstream stream;
	stream << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << std::uppercase;
	stream << number();
	text = stream.str();
}

bool mcp::uint64_union::decode_hex(std::string const & text)
{
	auto error(text.size() > 16);
	if (!error)
	{
		std::stringstream stream(text);
		stream << std::hex << std::noshowbase;
		uint64_t number_l;
		try
		{
			stream >> number_l;
			*this = mcp::uint64_union(number_l);
			if (!stream.eof())
			{
				error = true;
			}
		}
		catch (std::runtime_error &)
		{
			error = true;
		}
	}
	return error;
}

std::string mcp::uint64_union::to_string() const
{
	std::string result;
	encode_hex(result);
	return result;
}

mcp::signature_struct::signature_struct(uint256_union const& _r, uint256_union const& _s, byte _v): r(_r), s(_s), v(_v) {

}

mcp::signature_struct::signature_struct(uint64_t value0) {
	if (value0 == 0) {
		r.clear();
		s.clear();
		v = 0;
	}
}

bool mcp::signature_struct::is_valid() const noexcept {
	static const uint256_union s_max("0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
	static const uint256_union s_zero(0);
	return (v <= 1 && r > s_zero && s > s_zero && r < s_max && s < s_max);
}

bool mcp::signature_struct::operator== (signature_struct const& other_a) const {
	return r == other_a.r && s == other_a.s && v == other_a.v;
}
bool mcp::signature_struct::operator!= (signature_struct const & other_a) const {
	return !(*this == other_a);
}

bool mcp::signature_struct::decode_hex(std::string const & text) {
	bool error(text.size() != 130 || text.empty());
	if (!error) {
		r.decode_hex(text.substr(0, r.size * 2));
		s.decode_hex(text.substr(r.size * 2, s.size * 2));
		
		std::stringstream stream(text.substr((r.size + s.size) * 2, 2));
		uint v_l;
		stream << std::hex << std::noshowbase;
		stream >> v_l;
		v = static_cast<byte>(v_l);
	}
	return error;
}

std::string mcp::signature_struct::to_string() const {
	std::stringstream stream;
	stream << std::hex << std::uppercase << std::noshowbase << std::setw(2) << std::setfill('0') << (uint) v;
	return r.to_string() + s.to_string() + stream.str();
}

dev::bytesRef mcp::signature_struct::ref() {
	return dev::bytesRef(r.data(), size);
}

dev::bytesConstRef mcp::signature_struct::ref() const {
	return dev::bytesConstRef(r.data(), size);
}

mcp::account20_struct::account20_struct(mcp::public_key const& pubkey) {
	dev::bytesConstRef bRef = sha3(pubkey.ref()).ref();
	dev::bytesConstRef(bRef.data() + 12, size).copyTo(ref());
}

mcp::account20_struct::account20_struct(uint64_t value0) {
	*this = mcp::uint256_t(value0);
}

mcp::account20_struct::account20_struct (mcp::uint256_t const & number_a) {
	if(number_a == 0) {
		memset(bytes.data(), 0, bytes.size());
	} else {
		mcp::uint256_t number_l(number_a);
		for (auto i(bytes.rbegin()), n(bytes.rend()); i != n; ++i) {
			*i = ((number_l) & 0xff).convert_to<uint8_t>();
			number_l >>= 8;
		}
	}
}

mcp::account20_struct::account20_struct (mcp::uint256_union const & uint256_a) {
	*this = uint256_a.number();
}

bool mcp::account20_struct::operator== (account20_struct const& other_a) const {
	return bytes == other_a.bytes;
}
bool mcp::account20_struct::operator!= (account20_struct const & other_a) const {
	return !(*this == other_a);
}
bool mcp::account20_struct::operator< (account20_struct const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return false;
}
bool mcp::account20_struct::operator> (account20_struct const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return false;
}

bool mcp::account20_struct::operator<= (account20_struct const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] < other_a.bytes[i])
			return true;
		else if (bytes[i] > other_a.bytes[i])
			return false;
	}
	return true;
}
bool mcp::account20_struct::operator>= (account20_struct const & other_a) const
{
	for (int i = 0; i < size; i++)
	{
		if (bytes[i] > other_a.bytes[i])
			return true;
		else if (bytes[i] < other_a.bytes[i])
			return false;
	}
	return true;
}

std::string mcp::account20_struct::to_account() const {
	std::stringstream stream;
	stream << "0x" << std::setw(40) << std::setfill('0') << std::uppercase << std::hex << number();
	return "0x" + stream.str();
}

bool mcp::account20_struct::is_zero() const {
	bool is_zero(true);
	for (auto i(bytes.begin()), n(bytes.end()); i != n; ++i) {
		if (*i != 0) {
			is_zero = false;
			break;
		}
	}
	return is_zero;
}

bool mcp::account20_struct::decode_account(std::string const & text) {
	auto error(text.size() != 42 || text.empty());
	if (!error) {
		std::stringstream stream(text);
		stream << std::hex << std::showbase;
		mcp::uint256_t number_l;
		try
		{
			stream >> number_l;
			*this = number_l;
			if (!stream.eof())
			{
				error = true;
			}
		}
		catch (std::runtime_error &)
		{
			error = true;
		}
	}
	return error;
}

mcp::uint256_t mcp::account20_struct::number() const {
	mcp::uint256_t result;
	auto shift(0);
	for (auto i(bytes.begin()), n(bytes.end()); i != n; ++i) {
		result <<= shift;
		result |= *i;
		shift = 8;
	}
	return result;
}

byte* mcp::account20_struct::data() {
	return bytes.data();
}

byte const* mcp::account20_struct::data() const {
	return bytes.data();
}

void mcp::account20_struct::clear() {
	bytes.fill(0);
}

dev::bytesRef mcp::account20_struct::ref() {
	return dev::bytesRef(bytes.data(), size);
}

dev::bytesConstRef mcp::account20_struct::ref() const {
	return dev::bytesConstRef(bytes.data(), size);
}
