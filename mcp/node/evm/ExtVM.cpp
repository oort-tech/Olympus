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
/** @file ExtVM.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#include "ExtVM.h"
// #include "LastBlockHashesFace.h"
#include <boost/thread.hpp>
#include <exception>

using namespace dev;
using namespace dev::eth;
using namespace mcp;

namespace // anonymous
{

static unsigned const c_depthLimit = 1024;

/// Upper bound of stack space needed by single CALL/CREATE execution. Set experimentally.
static size_t const c_singleExecutionStackSize = 100 * 1024;

/// Standard thread stack size.
static size_t const c_defaultStackSize =
#if defined(__linux)
     8 * 1024 * 1024;
#elif defined(_WIN32)
    16 * 1024 * 1024;
#else
    512 * 1024; // OSX and other OSs
#endif

/// Stack overhead prior to allocation.
static size_t const c_entryOverhead = 128 * 1024;

/// On what depth execution should be offloaded to additional separated stack space.
static unsigned const c_offloadPoint = (c_defaultStackSize - c_entryOverhead) / c_singleExecutionStackSize;

void goOnOffloadedStack(Executive& _e, OnOpFunc const& _onOp)
{
    // Set new stack size enouth to handle the rest of the calls up to the limit.
    boost::thread::attributes attrs;
    attrs.set_stack_size((c_depthLimit - c_offloadPoint) * c_singleExecutionStackSize);

    // Create new thread with big stack and join immediately.
    // TODO: It is possible to switch the implementation to Boost.Context or similar when the API is stable.
    boost::exception_ptr exception;
    boost::thread{attrs, [&]{
        try
        {
            _e.go(_onOp);
        }
        catch (...)
        {
            exception = boost::current_exception(); // Catch all exceptions to be rethrown in parent thread.
        }
    }}.join();
    if (exception)
        boost::rethrow_exception(exception);
}

void go(unsigned _depth, Executive& _e, OnOpFunc const& _onOp)
{
    // If in the offloading point we need to switch to additional separated stack space.
    // Current stack is too small to handle more CALL/CREATE executions.
    // It needs to be done only once as newly allocated stack space it enough to handle
    // the rest of the calls up to the depth limit (c_depthLimit).

    if (_depth == c_offloadPoint + 1)
    {
        cnote << "Stack offloading (depth: " << c_offloadPoint << ")";
        goOnOffloadedStack(_e, _onOp);
    }
    else
        _e.go(_onOp);
}

evmc_status_code transactionExceptionToEvmcStatusCode(TransactionException ex) noexcept
{
    switch (ex)
    {
    case TransactionException::None:
        return EVMC_SUCCESS;

    case TransactionException::RevertInstruction:
        return EVMC_REVERT;

    case TransactionException::OutOfGas:
        return EVMC_OUT_OF_GAS;

    case TransactionException::BadInstruction:
        return EVMC_UNDEFINED_INSTRUCTION;

    case TransactionException::OutOfStack:
        return EVMC_STACK_OVERFLOW;

    case TransactionException::StackUnderflow:
        return EVMC_STACK_UNDERFLOW;

    case TransactionException ::BadJumpDestination:
        return EVMC_BAD_JUMP_DESTINATION;

    default:
        return EVMC_FAILURE;
    }
}

} // anonymous namespace


CallResult ExtVM::call(CallParameters& _p)
{   
    Executive e(m_s, envInfo(), m_s.traces, depth);
    if (!e.call(_p, 1, origin))
    {
        go(depth, e, _p.onOp);
        e.accrueSubState(sub);
    }
    _p.gas = e.gas();

    return {transactionExceptionToEvmcStatusCode(e.getException()), e.takeOutput()};
}

size_t ExtVM::codeSizeAt(Address _a)
{
    return m_s.codeSize(_a);
}

h256 ExtVM::codeHashAt(Address _a)
{    
    return exists(_a) ? m_s.codeHash(_a) : h256{};
}

void ExtVM::setStore(u256 _n, u256 _v)
{
    m_s.setStorage(myAddress, _n, _v);
}

CreateResult ExtVM::create(u256 _endowment, u256& io_gas, bytesConstRef _code, Instruction _op, u256 _salt, OnOpFunc const& _onOp)
{
    Executive e(m_s, envInfo(), m_s.traces, depth);
    bool result = false;
    if (_op == Instruction::CREATE)
        result = e.createOpcode(myAddress, _endowment, 1, io_gas, _code, origin);
    else
    {
        assert_x(_op == Instruction::CREATE2);
        result = e.create2Opcode(myAddress, _endowment, 1, io_gas, _code, origin, _salt);
    }

    if (!result)
    {
        go(depth, e, _onOp);
        e.accrueSubState(sub);
    }
    io_gas = e.gas();
    return {transactionExceptionToEvmcStatusCode(e.getException()), e.takeOutput(), e.newAddress()};
}

void ExtVM::selfdestruct(Address _a)
{
    // Why transfer is not used here? That caused a consensus issue before (see Quirk #2 in
    // http://martin.swende.se/blog/Ethereum_quirks_and_vulns.html). There is one test case
    // witnessing the current consensus
    // 'GeneralStateTests/stSystemOperationsTest/suicideSendEtherPostDeath.json'.
	mcp::uint256_t balance(m_s.balance(myAddress));
    m_s.addBalance(_a, balance);
    m_s.setBalance(myAddress, 0);
    ExtVMFace::selfdestruct(_a);

	//suicide trace action
	std::shared_ptr<mcp::suicide_trace_action> suicide_action(std::make_shared<mcp::suicide_trace_action>());
	suicide_action->contract_account = myAddress;
	suicide_action->refund_account = _a;
	suicide_action->balance = balance;

	std::shared_ptr<mcp::trace> suicide_trace(std::make_shared<mcp::trace>());
	suicide_trace->type = mcp::trace_type::suicide;
	suicide_trace->action = suicide_action;
	suicide_trace->depth = depth;

	m_s.traces.push_back(suicide_trace);
}

h256 ExtVM::blockHash(u256 _number)
{
    u256 const currentNumber = envInfo().number();
    if (_number >= currentNumber || _number < (std::max<u256>(256, currentNumber) - 256))
        return h256();

    mcp::block_store& store(envInfo().store);
    mcp::db::db_transaction& transaction(envInfo().transaction);
    h256 _h(0);
    store.stable_block_get(transaction, uint64_t(_number), _h);
    return _h;
}
