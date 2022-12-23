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
	if (!r.isList() || r.itemCount() != 2)
		BOOST_THROW_EXCEPTION(eth::InvalidTransactionReceiptFormat());

	m_from = r[0].isEmpty() ? Address() : r[0].toHash<Address>(RLP::VeryStrict);;
	m_output = r[1].toHash<h256>();
}

ApproveReceipt::ApproveReceipt(Address _from, h256 _output):
	m_from(_from),
	m_output(_output)
{}

void ApproveReceipt::streamRLP(RLPStream& _s) const
{
	_s.appendList(2);
	_s << m_from << m_output;
}

std::ostream& dev::operator<<(std::ostream& _out, ApproveReceipt const& _r)
{
	_out << "From: " << _r.from().hex() << std::endl;
	_out << "Output: " << toHex(_r.output()) << std::endl;
	return _out;
}
