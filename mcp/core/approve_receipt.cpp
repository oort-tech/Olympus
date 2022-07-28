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
/** @file ApproveReceipt.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#include "approve_receipt.hpp"
#include <mcp/common/Exceptions.h>

#include <boost/variant/get.hpp>

using namespace std;
using namespace dev;

ApproveReceipt::ApproveReceipt(RLP r)
{
	if (!r.isList() || r.itemCount() != 4)
		BOOST_THROW_EXCEPTION(eth::InvalidTransactionReceiptFormat());

	m_from = r[0].isEmpty() ? Address() : r[0].toHash<Address>(RLP::VeryStrict);;
	m_epoch = r[1].toInt<uint64_t>();
	m_output = r[2].toBytes();
	m_approve_hash = (h256)r[3];
}

ApproveReceipt::ApproveReceipt(Address _from, uint64_t _epoch, std::vector<uint8_t> _output, h256 _approve_hash):
	m_from(_from),
	m_epoch(_epoch),
	m_output(_output),
	m_approve_hash(_approve_hash)
{}

void ApproveReceipt::streamRLP(RLPStream& _s) const
{
	_s.appendList(4);
	_s << m_from << m_epoch << m_output << m_approve_hash;
}

std::ostream& dev::operator<<(std::ostream& _out, ApproveReceipt const& _r)
{
	_out << "From: " << _r.from().hex() << std::endl;
	_out << "Epoch: " << _r.epoch() << std::endl;
	_out << "Output: " << toHex(_r.output()) << std::endl;
	return _out;
}
