#pragma once

#include <libdevcore/RLP.h>
#include <libdevcore/Address.h>
#include <array>

namespace dev
{

/// Approve receipt, constructed either from RLP representation or from individual values.
/// Either a state root or a status code is contained.  m_hasStatusCode is true when it contains a status code.
/// Empty state root is not included into RLP-encoding.
class ApproveReceipt
{
public:
	ApproveReceipt(RLP r);
	ApproveReceipt(Address _from, h256 _output);

	Address from() const { return m_from; }
	h256 output() const { return m_output; }

	void streamRLP(dev::RLPStream & s) const;
	bytes rlp() const { RLPStream s; streamRLP(s); return s.out(); }
private:
	Address m_from;
	h256 m_output;
};

using ApproveReceipts = std::vector<ApproveReceipt>;

std::ostream& operator<<(std::ostream& _out, dev::ApproveReceipt const& _r);

}
