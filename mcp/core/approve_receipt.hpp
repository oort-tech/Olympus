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
	ApproveReceipt(Address _from, uint64_t _epoch, std::vector<uint8_t> _output, h256 _approve_hash);

	Address from() const { return m_from; }
	uint64_t epoch() const { return m_epoch; }
	std::vector<uint8_t> output() const { return m_output; }
	h256 approve_hash() const { return m_approve_hash; }

	void streamRLP(dev::RLPStream & s) const;
	bytes rlp() const { RLPStream s; streamRLP(s); return s.out(); }
private:
	Address m_from;
	uint64_t m_epoch;
	std::vector<uint8_t> m_output;
	h256 m_approve_hash; //hash corresponding to approve.
};

using ApproveReceipts = std::vector<ApproveReceipt>;

std::ostream& operator<<(std::ostream& _out, dev::ApproveReceipt const& _r);

}
