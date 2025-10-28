/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file TransactionReceipt.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <mcp/core/log_entry.hpp>
#include <libdevcore/RLP.h>
#include <libdevcore/Address.h>
#include <array>

namespace dev
{
namespace eth
{

class LocalTransactionReceipt;
/// Transaction receipt, constructed either from RLP representation or from individual values.
/// Either a state root or a status code is contained.  m_hasStatusCode is true when it contains a status code.
/// Empty state root is not included into RLP-encoding.
class TransactionReceipt
{
	friend LocalTransactionReceipt;
public:
	TransactionReceipt() {}
	TransactionReceipt(uint8_t _status, u256 const& _gasUsed, mcp::log_entries const& _log, bool _createBloom = false);

	/// @returns the status code.
	/// @throw TransactionReceiptVersionError when the receipt has a state root instead of a status code.
	uint8_t statusCode() const { return m_statusCode; };
	u256 const& cumulativeGasUsed() const { return m_gasUsed; }
	log_bloom const& bloom() const { return m_bloom; }
	mcp::log_entries const& log() const { return m_log; }
	u256 const& gasUsed() const { return m_gasUsed; }

	bytes rlp() const;
private:
	uint8_t m_statusCode;
	u256 m_gasUsed;
	log_bloom m_bloom;
	mcp::log_entries m_log;
};

using TransactionReceipts = std::vector<TransactionReceipt>;

std::ostream& operator<<(std::ostream& _out, eth::TransactionReceipt const& _r);

class LocalTransactionReceipt : public TransactionReceipt
{
public:
	LocalTransactionReceipt(
		TransactionReceipt const& _t,
		h256 const& _blockHash,
		unsigned _transactionIndex,
		unsigned _transactionExecIndex
	) :
		TransactionReceipt(_t),
		m_blockHash(_blockHash),
		m_transactionIndex(_transactionIndex),
		m_transactionExecIndex(_transactionExecIndex)
	{
	}
	LocalTransactionReceipt(RLP r);
	void streamRLP(dev::RLPStream& s) const;

	h256 const& blockHash() const { return m_blockHash; }
	unsigned transactionIndex() const { return m_transactionIndex; }
	unsigned const& transactionExecIndex() const { return m_transactionExecIndex; }

private:
	h256 m_blockHash;
	unsigned m_transactionIndex = 0;
	unsigned m_transactionExecIndex = 0;
};

class LocalisedTransactionReceipt: public LocalTransactionReceipt
{
public:
	LocalisedTransactionReceipt(
		LocalTransactionReceipt const& _t,
		h256 const& _hash,
		uint64_t _blockNumber,
		Address const& _from,
		Address const& _to,
		Address const& _contractAddress = Address()
	):
		LocalTransactionReceipt(_t),
		m_hash(_hash),
		m_blockNumber(_blockNumber),
		m_from(_from),
		m_to(_to),
		m_contractAddress(_contractAddress)
	{
		mcp::log_entries entries = log();
		for (unsigned i = 0; i < entries.size(); i++)
			m_localisedLogs.push_back(mcp::localised_log_entry(
				entries[i],
				blockHash(),
				m_blockNumber,
				m_hash,
				transactionIndex(),
				i
			));
	}

	h256 const& hash() const { return m_hash; }
	unsigned blockNumber() const { return m_blockNumber; }
	Address const& from() const { return m_from; }
	Address const& to() const { return m_to; }
	Address const& contractAddress() const { return m_contractAddress; }
	mcp::localised_log_entries const& localisedLogs() const { return m_localisedLogs; };

private:
	h256 m_hash;
	unsigned m_blockNumber;
	Address m_from;
	Address m_to;
	Address m_contractAddress;
	mcp::localised_log_entries m_localisedLogs;
};

}
}
