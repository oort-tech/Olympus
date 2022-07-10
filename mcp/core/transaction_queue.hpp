#pragma once

#include "transaction.hpp"

namespace mcp
{
class iTransactionQueue
{
public:
	virtual std::shared_ptr<Transaction> get(h256 const& _txHash) const = 0;
};
} // namespace mcp
