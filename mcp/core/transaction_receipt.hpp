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

/// Transaction receipt, constructed either from RLP representation or from individual values.
/// Either a state root or a status code is contained.  m_hasStatusCode is true when it contains a status code.
/// Empty state root is not included into RLP-encoding.
class TransactionReceipt
{
public:
	TransactionReceipt(RLP r);
	TransactionReceipt(uint8_t _status, u256 const& _gasUsed, mcp::log_entries const& _log, uint64_t const& _mci = 0);

	/// @returns the status code.
	/// @throw TransactionReceiptVersionError when the receipt has a state root instead of a status code.
	uint8_t statusCode() const { return m_statusCode; };
	u256 const& cumulativeGasUsed() const { return m_gasUsed; }
	log_bloom const& bloom() const { return m_bloom; }
	mcp::log_entries const& log() const { return m_log; }
	u256 const& gasUsed() const { return m_gasUsed; }

	void streamRLP(dev::RLPStream & s) const;
	bytes rlp() const { RLPStream s; streamRLP(s); return s.out(); }

	virtual bool const isPlaceholder() const { return m_Placeholder; }
	///just for Placeholder. do not streamRLP.
	bool m_Placeholder = false;
private:
	uint8_t m_statusCode;
	u256 m_gasUsed;
	log_bloom m_bloom;
	mcp::log_entries m_log;
};

using TransactionReceipts = std::vector<TransactionReceipt>;

std::ostream& operator<<(std::ostream& _out, eth::TransactionReceipt const& _r);

class LocalisedTransactionReceipt: public TransactionReceipt
{
public:
	LocalisedTransactionReceipt(
		TransactionReceipt const& _t,
		h256 const& _hash,
		mcp::block_hash const& _blockHash,
		uint64_t _blockNumber,
		Address const& _from,
		Address const& _to,
		unsigned _transactionIndex,
		//u256 const& _gasUsed,
		Address const& _contractAddress = Address()
	):
		TransactionReceipt(_t),
		m_hash(_hash),
		m_blockHash(_blockHash),
		m_blockNumber(_blockNumber),
		m_from(_from),
		m_to(_to),
		m_transactionIndex(_transactionIndex),
		//m_gasUsed(_gasUsed),
		m_contractAddress(_contractAddress)
	{
		mcp::log_entries entries = log();
		for (unsigned i = 0; i < entries.size(); i++)
			m_localisedLogs.push_back(mcp::localised_log_entry(
				entries[i],
				m_blockHash,
				m_blockNumber,
				m_hash,
				m_transactionIndex,
				i
			));
	}

	h256 const& hash() const { return m_hash; }
	mcp::block_hash const& blockHash() const { return m_blockHash; }
	unsigned blockNumber() const { return m_blockNumber; }
	Address const& from() const { return m_from; }
	Address const& to() const { return m_to; }
	unsigned transactionIndex() const { return m_transactionIndex; }
	//u256 const& gasUsed() const { return m_gasUsed; }
	Address const& contractAddress() const { return m_contractAddress; }
	mcp::localised_log_entries const& localisedLogs() const { return m_localisedLogs; };

private:
	h256 m_hash;
	mcp::block_hash m_blockHash;
	unsigned m_blockNumber;
	Address m_from;
	Address m_to;
	unsigned m_transactionIndex = 0;
	//u256 m_gasUsed;
	Address m_contractAddress;
	mcp::localised_log_entries m_localisedLogs;
};

}
}
