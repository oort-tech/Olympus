/** @file ApproveReceipt.cpp
 * @date 2014
 */

#include "approve_receipt.hpp"
#include <mcp/common/Exceptions.h>

#include <boost/variant/get.hpp>

using namespace std;
using namespace dev;

ApproveReceipt::ApproveReceipt(RLP r)
{
	if (!r.isList() || r.itemCount() != 3)
		BOOST_THROW_EXCEPTION(eth::InvalidTransactionReceiptFormat());

	m_statusCode = (uint8_t)r[0];
	m_from = r[1].isEmpty() ? Address() : r[1].toHash<Address>(RLP::VeryStrict);;
	m_output = r[2].toHash<h256>();
}

ApproveReceipt::ApproveReceipt(uint8_t _status, Address _from, h256 _output):
	m_statusCode(_status),
	m_from(_from),
	m_output(_output)
{}

void ApproveReceipt::streamRLP(RLPStream& _s) const
{
	_s.appendList(3);
	_s << statusCode() << from() << output();
}

std::ostream& dev::operator<<(std::ostream& _out, ApproveReceipt const& _r)
{
	_out << "Status: " << _r.statusCode() << std::endl;
	_out << "From: " << _r.from().hex() << std::endl;
	_out << "Output: " << toHex(_r.output()) << std::endl;
	return _out;
}
