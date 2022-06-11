#pragma once

#include "transaction.hpp"

namespace mcp
{
class iTransactionQueue
{
public:
	virtual bool exist(Address const&_acco, u256 const& _nonce) = 0;
	virtual bool exist(h256 const& _hash) = 0;
	virtual std::shared_ptr<Transaction> get(h256 const& _txHash) const = 0;
};
} // namespace mcp
