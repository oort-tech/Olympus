#pragma once

#include <libdevcore/RLP.h>
#include <libdevcore/Address.h>
#include <mcp/common/EVMSchedule.h>
#include <libdevcrypto/Common.h>
#include <boost/optional.hpp>
#include "transaction.hpp"
#include <secp256k1-vrf.h>

using namespace dev;
namespace mcp
{
	const uint64_t epoch_period = 100; //一个eopch包含的主链block数量

	struct ApproveSkeleton
	{
		uint64_t epoch;
		std::vector<uint8_t> proof;
	};

	class approve
	{
	public:
		approve() {}
		/// Constructs a transaction from a transaction skeleton & optional secret.
		approve(ApproveSkeleton const& ts, Secret const& _s);

		/// Constructs a approve from the given RLP.
		approve(dev::RLP const & r, CheckTransaction _checkSig);

		/// Constructs a transaction from the given RLP.
		explicit approve(bytesConstRef _rlp, CheckTransaction _checkSig) : approve(RLP(_rlp), _checkSig) {}

		/// Constructs a Transaction from the given RLP.
		explicit approve(bytes const& _rlp, CheckTransaction _checkSig) : approve(&_rlp, _checkSig) {}

		/// @returns sender of the transaction from the signature (and hash).
		/// @throws TransactionIsUnsigned if signature was not initialized
		Address const& sender() const;

		/// @throws TransactionIsUnsigned if signature was not initialized
		/// @throws InvalidSValue if the signature has an invalid S value.
		void checkLowS() const;

		/// @throws InvalidSignature if the transaction is replay protected
		/// and chain id is not equal to @a _chainId
		void checkChainId(uint64_t _chainId) const;

		void checkEpoch(uint64_t _epoch) const;

		/// Serialises this approve to an RLPStream.
		/// @throws TransactionIsUnsigned if including signature was requested but it was not initialized
		void streamRLP(RLPStream& s, IncludeSignature sig = WithSignature) const;

		/// @returns the RLP serialisation of this transaction.
		bytes rlp(IncludeSignature _sig = WithSignature) const { RLPStream s; streamRLP(s, _sig); return s.out(); }

		/// @returns the SHA3 hash of the RLP serialisation of this approve.
		/// queue used WithSignature hash inlcude sinature,if sinature error,can resend it again with the correct signature
		h256 sha3(IncludeSignature _sig = WithSignature) const;

		//test
		uint64_t chainID() const { return m_chainId; }

		/// @returns the signature of the transaction (the signature has the sender encoded in it)
		/// @throws TransactionIsUnsigned if signature was not initialized
		SignatureStruct const& signature() const;

		/// @returns v value of the transaction (has chainID and recoveryID encoded in it)
		/// @throws TransactionIsUnsigned if signature was not initialized
		u256 rawV() const;

		void sign(Secret const& _priv);			///< Sign the transaction.

		void setSinature(h256 const& _r, h256 const& _s, byte _v) { m_vrs = SignatureStruct(_r, _s, _v); }

		secp256k1_pubkey getPublicKey() const;

		int vrf_verify(std::vector<uint8_t>& output, std::string msg) const;

		static uint64_t calc_curr_epoch(uint64_t last_summary_mci);
		static uint64_t calc_elect_epoch(uint64_t last_summary_mci);
		
		void show() const;
		uint64_t epoch() { return m_epoch; }

		uint64_t m_epoch;
		std::vector<uint8_t> m_proof;


	private:
		static bool isZeroSignature(u256 const& _r, u256 const& _s) { return !_r && !_s; }

		// Address m_from;
		// uint64_t m_epoch;
		// std::vector<uint8_t> m_proof;

		SignatureStruct m_vrs;	///< The signature of the approve. Encodes the sender.
		uint64_t m_chainId;
		mutable h256 m_hashWith;			///< Cached hash of transaction with signature.
		mutable boost::optional<Address> m_sender;  ///< Cached sender, determined from signature.
	};
}
