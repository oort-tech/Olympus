#pragma once

#include <libdevcore/RLP.h>
#include <libdevcore/Address.h>
#include <mcp/common/EVMSchedule.h>
#include <libdevcrypto/Common.h>
#include <boost/optional.hpp>

using namespace dev;
namespace mcp
{
	struct TransactionSkeleton
	{
		Address from;
		Address to;
		u256 value;
		bytes data;
		u256 nonce = Invalid256;
		u256 gas = Invalid256;
		u256 gasPrice = Invalid256;
	};

	/// Named-boolean type to encode whether a signature be included in the serialisation process.
	/// hash not include vrs,broadcast inlcude vrs
	enum IncludeSignature
	{
		WithoutSignature = 0,	///< Do not include a signature.
		WithSignature = 1,		///< Do include a signature.
	};

	enum class CheckTransaction
	{
		None,
		Cheap,
		Everything
	};

	class Transaction
	{
	public:
		Transaction() {}
		/// Constructs a transaction from a transaction skeleton & optional secret.
		Transaction(TransactionSkeleton const& ts, Secret const& _s);

		/// Constructs an unsigned message-call transaction.
		Transaction(u256 const& _value, u256 const& _gasPrice, u256 const& _gas, Address const& _dest, bytes const& _data, u256 const& _nonce = 0) :
			m_nonce(_nonce), 
			m_value(_value), 
			m_receiveAddress(_dest), 
			m_gasPrice(_gasPrice), 
			m_gas(_gas), 
			m_data(_data) 
		{}

		/// Constructs an unsigned contract-creation transaction.
		Transaction(u256 const& _value, u256 const& _gasPrice, u256 const& _gas, bytes const& _data, u256 const& _nonce = 0) :
			m_nonce(_nonce), 
			m_value(_value), 
			m_gasPrice(_gasPrice), 
			m_gas(_gas), 
			m_data(_data) 
		{}

		/// Constructs a transaction from the given RLP.
		Transaction(dev::RLP const & r, CheckTransaction _checkSig);

		/// Constructs a transaction from the given RLP.
		explicit Transaction(bytesConstRef _rlp, CheckTransaction _checkSig) : Transaction(RLP(_rlp), _checkSig) {}

		/// Constructs a Transaction from the given RLP.
		explicit Transaction(bytes const& _rlp, CheckTransaction _checkSig) : Transaction(&_rlp, _checkSig) {}

		///// Checks equality of transactions.
		//bool operator==(Transaction const& _c) const { return m_receiveAddress == _c.m_receiveAddress && m_value == _c.m_value && m_data == _c.m_data; }
		///// Checks inequality of transactions.
		//bool operator!=(Transaction const& _c) const { return !operator==(_c); }

		/// @returns sender of the transaction from the signature (and hash).
		/// @throws TransactionIsUnsigned if signature was not initialized
		Address const& sender() const;

		/// Like sender() but will never throw. @returns a null Address if the signature is invalid.
		Address const& safeSender() const noexcept;

		/// Force the sender to a particular value. This will result in an invalid transaction RLP.used for estimate_gas
		void forceSender(Address const& _a) { m_sender = _a; }

		/// @throws TransactionIsUnsigned if signature was not initialized
		/// @throws InvalidSValue if the signature has an invalid S value.
		void checkLowS() const;

		/// @throws InvalidSignature if the transaction is replay protected
		/// and chain id is not equal to @a _chainId
		void checkChainId(uint64_t _chainId) const;

		/// @returns true if transaction is non-null.
		explicit operator bool() const { return true; }

		/// @returns true if transaction is contract-creation.
		bool isCreation() const { return m_receiveAddress == ZeroAddress; }

		/// Serialises this transaction to an RLPStream.
		/// @throws TransactionIsUnsigned if including signature was requested but it was not initialized
		void streamRLP(RLPStream& s, IncludeSignature sig = WithSignature) const;

		/// @returns the RLP serialisation of this transaction.
		bytes rlp(IncludeSignature _sig = WithSignature) const { RLPStream s; streamRLP(s, _sig); return s.out(); }

		/// @returns the SHA3 hash of the RLP serialisation of this transaction.
		/// queue used WithSignature hash inlcude sinature,if sinature error,can resend it again with the correct signature
		h256 sha3(IncludeSignature _sig = WithSignature) const;

		/// @returns the amount of ETH to be transferred by this (message-call) transaction, in Wei. Synonym for endowment().
		u256 value() const { return m_value; }

		/// @returns the base fee and thus the implied exchange rate of ETH to GAS.
		u256 gasPrice() const { return m_gasPrice; }

		/// @returns the total gas to convert, paid for from sender's account. Any unused gas gets refunded once the contract is ended.
		u256 gas() const { return m_gas; }

		/// @returns the receiving address of the message-call transaction (undefined for contract-creation transactions).
		Address receiveAddress() const { return m_receiveAddress; }

		/// Synonym for receiveAddress().
		Address to() const { return m_receiveAddress; }

		/// Synonym for safeSender().
		Address from() const { return safeSender(); }

		/// @returns the data associated with this (message-call) transaction. Synonym for initCode().
		bytes const& data() const { return m_data; }

		/// @returns the transaction-count of the sender.
		u256 nonce() const { return m_nonce; }

		/// @returns true if the transaction was signed
		bool hasSignature() const { return m_vrs.is_initialized(); }

		/// @returns true if the transaction was signed with zero signature
		bool hasZeroSignature() const { return m_vrs && isZeroSignature(m_vrs->r, m_vrs->s); }

		///// @returns true if the transaction uses EIP155 replay protection
		//bool isReplayProtected() const { return m_chainId.is_initialized(); }

		//test
		uint64_t chainID() const { return *m_chainId; }

		/// @returns the signature of the transaction (the signature has the sender encoded in it)
		/// @throws TransactionIsUnsigned if signature was not initialized
		SignatureStruct const& signature() const;

		/// @returns v value of the transaction (has chainID and recoveryID encoded in it)
		/// @throws TransactionIsUnsigned if signature was not initialized
		u256 rawV() const;

		void sign(Secret const& _priv);			///< Sign the transaction.

		void setSinature(h256 const& _r, h256 const& _s, byte _v) { m_vrs = SignatureStruct(_r, _s, _v); }

		/// @returns amount of gas required for the basic payment.
		int64_t baseGasRequired(dev::eth::EVMSchedule const& _es) const { return baseGasRequired(isCreation(), &m_data, _es); }

		/// Get the fee associated for a transaction with the given data.
		static int64_t baseGasRequired(bool _contractCreation, dev::bytesConstRef _data, dev::eth::EVMSchedule const& _es);
	private:
		static bool isZeroSignature(u256 const& _r, u256 const& _s) { return !_r && !_s; }

		u256 m_nonce;						///< The transaction-count of the sender.
		u256 m_value;						///< The amount of ETH to be transferred by this transaction. Called 'endowment' for contract-creation transactions.
		Address m_receiveAddress;			///< The receiving address of the transaction.
		u256 m_gasPrice;					///< The base fee and thus the implied exchange rate of ETH to GAS.
		u256 m_gas;							///< The total gas to convert, paid for from sender's account. Any unused gas gets refunded once the contract is ended.
		bytes m_data;						///< The data associated with the transaction, or the initialiser if it's a creation transaction.
		boost::optional<SignatureStruct> m_vrs;	///< The signature of the transaction. Encodes the sender.
		boost::optional<uint64_t> m_chainId;

		mutable h256 m_hashWith;			///< Cached hash of transaction with signature.
		mutable boost::optional<Address> m_sender;  ///< Cached sender, determined from signature.
	};

	/// Nice name for vector of Transaction.
	using Transactions = std::vector<Transaction>;

	class LocalisedTransaction : public Transaction
	{
	public:
		LocalisedTransaction(
			Transaction const& _t,
			mcp::block_hash const& _blockHash,
			unsigned _transactionIndex,
			uint64_t _blockNumber = 0
		) :
			Transaction(_t),
			m_blockHash(_blockHash),
			m_transactionIndex(_transactionIndex),
			m_blockNumber(_blockNumber)
		{}

		mcp::block_hash const& blockHash() const { return m_blockHash; }
		unsigned transactionIndex() const { return m_transactionIndex; }
		uint64_t blockNumber() const { return m_blockNumber; }

	private:
		mcp::block_hash m_blockHash;
		unsigned m_transactionIndex;
		uint64_t m_blockNumber;
	};
}
