#pragma once

namespace mcp
{
class iApproveQueue
{
public:
	virtual std::shared_ptr<approve> get(h256 const& _txHash) const = 0;
};
} // namespace mcp
