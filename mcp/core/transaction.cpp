#include "transaction.hpp"
#include <mcp/common/utility.hpp>
#include <libdevcore/CommonIO.h>
#include <boost/endian/conversion.hpp>
#include <mcp/common/common.hpp>
#include <mcp/common/log.hpp>

mcp::Transaction::Transaction(TransactionSkeleton const& ts, boost::optional<Secret> const& s) :
	m_nonce(ts.nonce),
	m_value(ts.value),
	m_receiveAddress(ts.to),
	m_gasPrice(ts.gasPrice),
	m_gas(ts.gas),
	m_data(ts.data),
	m_sender(ts.from),
	m_chainId(mcp::chain_id)
{
	if (s)
		sign(*s);
}

mcp::Transaction::Transaction(dev::RLP const & rlp, CheckTransaction _checkSig)
{
	try
	{
		if (!rlp.isList())
			BOOST_THROW_EXCEPTION(InvalidTransactionFormat() << errinfo_comment("transaction RLP must be a list"));

		if (rlp.itemCount() != 9)
			BOOST_THROW_EXCEPTION(InvalidTransactionFormat() << errinfo_comment("too many or to low fields in the transaction RLP"));

		m_nonce = rlp[0].toInt<u256>();
		m_gasPrice = rlp[1].toInt<u256>();
		m_gas = rlp[2].toInt<u256>();
		if (!rlp[3].isData())
			BOOST_THROW_EXCEPTION(InvalidTransactionFormat()
				<< errinfo_comment("recepient RLP must be a byte array"));
		m_receiveAddress = rlp[3].isEmpty() ? Address() : rlp[3].toHash<Address>(RLP::VeryStrict);
		m_value = rlp[4].toInt<u256>();

		if (!rlp[5].isData())
			BOOST_THROW_EXCEPTION(InvalidTransactionFormat()
				<< errinfo_comment("transaction data RLP must be a byte array"));

		m_data = rlp[5].toBytes();
		u256 const v = rlp[6].toInt<u256>();
		h256 const r = rlp[7].toInt<u256>();
		h256 const s = rlp[8].toInt<u256>();

		if (v > 36)
		{
			auto const chainId = (v - 35) / 2;
			if (chainId > std::numeric_limits<uint64_t>::max())
				BOOST_THROW_EXCEPTION(InvalidSignature());
			m_chainId = static_cast<uint64_t>(chainId);
		}
		// only values 27 and 28 are allowed for non-replay protected transactions
		else if (v != 27 && v != 28)
			BOOST_THROW_EXCEPTION(InvalidSignature());

		auto const recoveryID =
			m_chainId.is_initialized() ? byte(v - (u256(*m_chainId) * 2 + 35)) : byte(v - 27);
		m_vrs = SignatureStruct{ r, s, recoveryID };

		//mcp::log m_log = { mcp::log("node") };
		//LOG(m_log.info) << "v:" << toHex(bytes(recoveryID));
		//LOG(m_log.info) << "r:" << r.hex();
		//LOG(m_log.info) << "s:" << s.hex();
		//auto aa = *m_vrs;
		//LOG(m_log.info) << "sig:" << ((Signature)aa).hex();
		

		if (_checkSig >= CheckTransaction::Cheap && !m_vrs->isValid())
			BOOST_THROW_EXCEPTION(InvalidSignature());

		if (_checkSig == CheckTransaction::Everything)
			m_sender = sender();
	}
	catch (Exception& _e)
	{
		_e << errinfo_name("invalid transaction format: " + toString(rlp) + " RLP: " + toHex(rlp.data()));
		throw;
	}
}

Address const& mcp::Transaction::safeSender() const noexcept
{
	try
	{
		return sender();
	}
	catch (...)
	{
		return ZeroAddress;
	}
}

Address const& mcp::Transaction::sender() const
{
	if (!m_sender.is_initialized())
	{
		if (!m_vrs)
			BOOST_THROW_EXCEPTION(TransactionIsUnsigned());

		auto p = recover(*m_vrs, sha3(WithoutSignature));
		if (!p)
			BOOST_THROW_EXCEPTION(InvalidSignature());
		m_sender = right160(dev::sha3(bytesConstRef(p.data(), sizeof(p))));
	}
	return *m_sender;
}

SignatureStruct const& mcp::Transaction::signature() const
{
	if (!m_vrs)
		BOOST_THROW_EXCEPTION(TransactionIsUnsigned());

	return *m_vrs;
}

u256 mcp::Transaction::rawV() const
{
	if (!m_vrs)
		BOOST_THROW_EXCEPTION(TransactionIsUnsigned());

	int const vOffset = m_chainId.is_initialized() ? *m_chainId * 2 + 35 : 27;
	return m_vrs->v + vOffset;
}

void mcp::Transaction::sign(Secret const& priv)
{
	auto sig = dev::sign(priv, sha3(WithoutSignature));
	SignatureStruct sigStruct = *(SignatureStruct const*)&sig;
	if (sigStruct.isValid())
		m_vrs = sigStruct;
	else
		BOOST_THROW_EXCEPTION(InvalidSignature() << errinfo_comment("signature invalid"));
}

void mcp::Transaction::streamRLP(RLPStream& s, IncludeSignature sig) const
{
	s.appendList(9);
	s << m_nonce << m_gasPrice << m_gas;
	if (m_receiveAddress)
		s << m_receiveAddress;
	else
		s << "";
	s << m_value << m_data;

	if (sig == IncludeSignature::WithSignature) //rlp for p2p and storage
	{
		if (!m_vrs)
			BOOST_THROW_EXCEPTION(TransactionIsUnsigned());

		s << rawV() << (u256)m_vrs->r << (u256)m_vrs->s;
	}
	else  ///rlp for hash and verify sinature
		s << *m_chainId << 0 << 0;
}

static const u256 c_secp256k1n("115792089237316195423570985008687907852837564279074904382605163141518161494337");

void mcp::Transaction::checkLowS() const
{
	if (!m_vrs)
		BOOST_THROW_EXCEPTION(TransactionIsUnsigned());

	if (m_vrs->s > c_secp256k1n / 2)
		BOOST_THROW_EXCEPTION(InvalidSignature());
}

void mcp::Transaction::checkChainId(uint64_t _chainId) const
{
	if (m_chainId.is_initialized() && *m_chainId != _chainId)
		BOOST_THROW_EXCEPTION(InvalidSignature());
}

int64_t mcp::Transaction::baseGasRequired(bool _contractCreation, dev::bytesConstRef _data, dev::eth::EVMSchedule const& _es)
{
	int64_t g = _contractCreation ? _es.txCreateGas : _es.txGas;

	// Calculate the cost of input data.
	// No risk of overflow by using int64 until txDataNonZeroGas is quite small
	// (the value not in billions).
	for (auto i : _data)
		g += i ? _es.txDataNonZeroGas : _es.txDataZeroGas;
	return g;
}

h256 mcp::Transaction::sha3(IncludeSignature _sig) const
{
	if (_sig == WithSignature && m_hashWith)
		return m_hashWith;

	RLPStream s;
	streamRLP(s, _sig);

	auto ret = dev::sha3(s.out());
	if (_sig == WithSignature)
		m_hashWith = ret;
	return ret;
}
