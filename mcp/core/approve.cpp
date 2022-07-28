#include "approve.hpp"
#include <mcp/common/utility.hpp>
#include <libdevcore/CommonIO.h>
#include <boost/endian/conversion.hpp>
#include <mcp/common/common.hpp>
#include <mcp/common/log.hpp>
#include "config.hpp"


mcp::approve::approve(ApproveSkeleton const& ts, Secret const& s) :
	m_epoch(ts.epoch),
	m_proof(ts.proof),
	m_chainId(mcp::chain_id)
{
	if (s)
		sign(s);
}

mcp::approve::approve(dev::RLP const & rlp, CheckTransaction _checkSig)
{
    try
	{
		if (!rlp.isList())
			BOOST_THROW_EXCEPTION(InvalidTransactionFormat() << errinfo_comment("approve RLP must be a list"));

		if (rlp.itemCount() != 5)
			BOOST_THROW_EXCEPTION(InvalidTransactionFormat() << errinfo_comment("too many or to low fields in the transaction RLP"));

        m_epoch = rlp[0].toInt<uint64_t>();
        m_proof = rlp[1].toBytes();

		u256 const v = rlp[2].toInt<u256>();
		h256 const r = rlp[3].toInt<u256>();
		h256 const s = rlp[4].toInt<u256>();

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

        auto const recoveryID = byte(v - (u256(m_chainId) *2 + 35));
        m_vrs = SignatureStruct{ r, s, recoveryID };

        if (_checkSig == CheckTransaction::Everything)
			m_sender = sender();
	}
	catch (Exception& _e)
	{
		_e << errinfo_name("invalid approve format: " + toString(rlp) + " RLP: " + toHex(rlp.data()));
		throw;
	}
}

Address const& mcp::approve::sender() const
{
	if (!m_sender.is_initialized())
	{
		//if (hasZeroSignature())
		//	m_sender = MaxAddress;
		//else
		//{

			auto p = recover(m_vrs, sha3(WithoutSignature));
			if (!p)
				BOOST_THROW_EXCEPTION(InvalidSignature());
			m_sender = right160(dev::sha3(bytesConstRef(p.data(), sizeof(p))));
		//}
	}
	return *m_sender;
}

SignatureStruct const& mcp::approve::signature() const
{
	return m_vrs;
}

u256 mcp::approve::rawV() const
{
	int const vOffset = m_chainId * 2 + 35;
	return m_vrs.v + vOffset;
}

void mcp::approve::sign(Secret const& priv)
{
	auto sig = dev::sign(priv, sha3(WithoutSignature));
	SignatureStruct sigStruct = *(SignatureStruct const*)&sig;
	if (sigStruct.isValid())
		m_vrs = sigStruct;
	else
		BOOST_THROW_EXCEPTION(InvalidSignature() << errinfo_comment("signatue invalid"));
}

secp256k1_pubkey mcp::approve::getPublicKey() const
{
	auto p = toPubkey(m_vrs, sha3(WithoutSignature));
	return p;
}

void mcp::approve::streamRLP(RLPStream& s, IncludeSignature sig) const
{
	s.appendList(5);
    s << m_epoch << m_proof;
    if (sig == IncludeSignature::WithSignature) //rlp for p2p and storage
	{
		//if (hasZeroSignature())
		//	s << *m_chainId;
		//else
			s << rawV();

		s << (u256)m_vrs.r << (u256)m_vrs.s;
	}
	else  ///rlp for hash and verify sinature
		s << m_chainId << 0 << 0;
}

static const u256 c_secp256k1n("115792089237316195423570985008687907852837564279074904382605163141518161494337");

void mcp::approve::checkLowS() const
{
	if (m_vrs.s > c_secp256k1n / 2)
		BOOST_THROW_EXCEPTION(InvalidSignature());
}

void mcp::approve::checkChainId(uint64_t _chainId) const
{
	if (m_chainId != _chainId)
		BOOST_THROW_EXCEPTION(InvalidSignature());
}

void mcp::approve::checkEpoch(uint64_t _epoch) const
{
	if (m_epoch != _epoch)
	{
		LOG(g_log.error) << "[checkEpoch] m_epoch = " << m_epoch << " _epoch = " <<_epoch;
		BOOST_THROW_EXCEPTION(InvalidSignature());
	}
}

h256 mcp::approve::sha3(IncludeSignature _sig) const
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

int mcp::approve::vrf_verify(std::vector<uint8_t>& output, std::string msg) const
{
	auto* ctx = mcp::encry::get_secp256k1_ctx();
	secp256k1_pubkey rawPubkey = getPublicKey();
	std::array<byte, 65> serializedPubkey;
	auto serializedPubkeySize = serializedPubkey.size();
	output.resize(32, 0);
	secp256k1_ec_pubkey_serialize(
		ctx,
		serializedPubkey.data(),
		&serializedPubkeySize,
		&rawPubkey,
		SECP256K1_EC_COMPRESSED
	);

	if(secp256k1_vrf_verify(output.data(), m_proof.data(), serializedPubkey.data(), msg.data(), msg.size()) == 1){
		LOG(g_log.debug) << "[vrf_verify] secp256k1_vrf_verify ok";
	}
	else{
		LOG(g_log.debug) << "[vrf_verify] secp256k1_vrf_verify fail ";
		BOOST_THROW_EXCEPTION(ZeroSignatureTransaction());
	}
}

uint64_t mcp::approve::calc_curr_epoch(uint64_t last_summary_mci)
{
	if(last_summary_mci < 2 * mcp::epoch_period) return 0;
	
	return last_summary_mci / mcp::epoch_period - 1;
}

uint64_t mcp::approve::calc_elect_epoch(uint64_t last_summary_mci)
{
	return last_summary_mci / mcp::epoch_period + 1;
}

void mcp::approve::show() const
{
    LOG(g_log.info) << "[approve show] sender="<<sender().hex();
    LOG(g_log.info) << "m_epoch="<<m_epoch;
    LOG(g_log.info) << "m_proof="<<dev::toHex(m_proof);
	LOG(g_log.info) << "m_vrs r=" << m_vrs.r.hex() << " s=" << m_vrs.s.hex() << " v=" << (uint32_t)m_vrs.v;
	LOG(g_log.info) << "m_chainId = " << m_chainId;
    LOG(g_log.info) << "m_hashWith="<<m_hashWith.hex();
	LOG(g_log.info) << "sha3(WithoutSignature) = " << sha3(WithoutSignature).hex();
	secp256k1_pubkey rawPubkey = getPublicKey();
	LOG(g_log.info) << "rawPubkey = " << chars_to_hex(rawPubkey.data, 64);
}
