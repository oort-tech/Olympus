#include "CallTracer.hpp"
#include <mcp/rpc/jsonHelper.hpp>

using namespace dev;
using namespace dev::eth;

namespace mcp
{

void CallTracer::CaptureState(uint64_t /*PC*/, Instruction /*inst*/, uint64_t /*gasCost*/, uint64_t /*gas*/,
                             VMFace const* /*vm*/, ExtVMFace const* /*extVM*/) noexcept
{
    // opcode-level state is ignored by call tracer
}


void CallTracer::CaptureStart(ExtVMFace const* /*ext*/, Address const& from, Address const& to,
                             bool create, bytes const& input, uint64_t gas, u256 value)
{
    while (!m_callStack.empty())
        m_callStack.pop();
    m_rootCall = CallFrame{};
    m_hasRootCall = false;

    CallFrame frame;
    frame.from = from.hexPrefixed();
    frame.to = to.hexPrefixed();
    frame.value = dev::toJS(value);
    frame.gas = dev::toHexQuantity(gas);
    frame.input = dev::toHexBytes(input);
    frame.type = create ? "CREATE" : "CALL";

    pushFrame(std::move(frame));
}

void CallTracer::CaptureEnter(Instruction op, Address const& from, Address const& to,
                             bytes const& input, uint64_t gas, std::shared_ptr<u256> value)
{
    CallFrame frame;
    frame.type = getCallType(op);
    frame.from = from.hexPrefixed();
    frame.to = to.hexPrefixed();
    frame.gas = dev::toHexQuantity(gas);
    frame.input = dev::toHexBytes(input);
    
    if (value)
        frame.value = dev::toJS(*value);
    else
        frame.value = "0x0";

    pushFrame(std::move(frame));
}

void CallTracer::CaptureExit(bytes const& output, uint64_t gasUsed, mcp::TransactionException const excepted)
{
    std::string error = excepted == mcp::TransactionException::None ? std::string() : mcp::to_transaction_exception_messge(excepted);
    finishFrame(output, gasUsed, error);
}

void CallTracer::CaptureEnd(bytes const& output, uint64_t gasUsed, mcp::TransactionException const excepted)
{
    std::string error;
    if (excepted != mcp::TransactionException::None)
        error = mcp::to_transaction_exception_messge(excepted);
    finishFrame(output, gasUsed, error);
}

void CallTracer::CaptureFault(uint64_t /*PC*/, Instruction /*op*/, uint64_t /*gasCost*/, uint64_t /*gas*/,
                             VMFace const* /*vm*/, ExtVMFace const* /*scope*/) noexcept
{
    // faults are handled when CaptureExit/CaptureEnd is invoked with the exception
}

mcp::json CallTracer::GetResult()
{
    if (!m_hasRootCall)
        return mcp::json::object();
    return callFrameToJson(m_rootCall);
}

void CallTracer::SetConfig(mcp::json const& options)
{
    m_config = options;
    // CallTracer typically doesn't need many configuration options
    // But we store them in case future enhancements need them
}

std::string CallTracer::getCallType(Instruction inst) const
{
    switch (inst) {
        case Instruction::CALL: return "CALL";
        case Instruction::CALLCODE: return "CALLCODE";
        case Instruction::DELEGATECALL: return "DELEGATECALL";
        case Instruction::STATICCALL: return "STATICCALL";
        case Instruction::CREATE: return "CREATE";
        case Instruction::CREATE2: return "CREATE2";
        default: return "CALL";
    }
}

void CallTracer::pushFrame(CallFrame frame)
{
    if (!m_hasRootCall)
    {
        m_rootCall = std::move(frame);
        m_hasRootCall = true;
        return;
    }

    m_callStack.push(std::move(frame));
}

void CallTracer::finishFrame(dev::bytes const& output, uint64_t gasUsed, std::string const& error)
{
    CallFrame* current = nullptr;

    if (m_callStack.empty())
    {
        if (!m_hasRootCall)
            return;
        current = &m_rootCall;
    }
    else
    {
        CallFrame frame = std::move(m_callStack.top());
        m_callStack.pop();
        frame.output = dev::toHexBytes(output);
        frame.gasUsed = dev::toHexQuantity(gasUsed);
        frame.error = error;
        frame.reverted = !error.empty();

        if (m_callStack.empty())
            m_rootCall.calls.push_back(std::move(frame));
        else
            m_callStack.top().calls.push_back(std::move(frame));
        return;
    }

    current->output = dev::toHexBytes(output);
    current->gasUsed = dev::toHexQuantity(gasUsed);
    current->error = error;
    current->reverted = !error.empty();
}

mcp::json CallTracer::callFrameToJson(CallFrame const& frame) const
{
    mcp::json result;
    
    result["type"] = frame.type;
    result["from"] = frame.from;
    result["to"] = frame.to;
    result["value"] = frame.value;
    result["gas"] = frame.gas;
    result["gasUsed"] = frame.gasUsed;
    result["input"] = frame.input;
    result["output"] = frame.output;
    
    if (!frame.error.empty()) {
        result["error"] = frame.error;
        result["revertReason"] = frame.error;
    }
    
    if (!frame.calls.empty()) {
        mcp::json calls = mcp::json::array();
        for (auto const& call : frame.calls) {
            calls.push_back(callFrameToJson(call));
        }
        result["calls"] = calls;
    }
    
    return result;
}

}  // namespace mcp