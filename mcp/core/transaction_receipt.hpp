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

#include <boost/variant/variant.hpp>
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
	TransactionReceipt(uint8_t _status, u256 const& _gasUsed, mcp::log_entries const& _log);

	/// @returns the status code.
	/// @throw TransactionReceiptVersionError when the receipt has a state root instead of a status code.
	uint8_t statusCode() const;
	u256 const& cumulativeGasUsed() const { return m_gasUsed; }
	mcp::log_entries const& log() const { return m_log; }
private:
	boost::variant<uint8_t,h256> m_statusCodeOrStateRoot;
	u256 m_gasUsed;
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
		mcp::account const& _from,
		mcp::account const& _to, 
		mcp::account const& _contractAddress // = Address() sichaoy: where is default address generated
	):
		TransactionReceipt(_t),
		m_hash(_hash),
		m_from(_from),
		m_to(_to),
		m_contractAddress(_contractAddress)
	{
		mcp::log_entries entries = log();
		for (unsigned i = 0; i < entries.size(); i++)
			m_localisedLogs.push_back(mcp::localised_log_entry(
				entries[i],
				m_hash,
				i
			));
	}

	h256 const& hash() const { return m_hash; }
	mcp::account const& from() const { return m_from; }
	mcp::account const& to() const { return m_to; }
	mcp::account const& contractAddress() const { return m_contractAddress; }
	mcp::localised_log_entries const& localisedLogs() const { return m_localisedLogs; };

private:
	h256 m_hash;
	mcp::account m_from;
	mcp::account m_to;
	unsigned m_transactionIndex = 0;
	mcp::account m_contractAddress;
	mcp::localised_log_entries m_localisedLogs;
};

}
}
