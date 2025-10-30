#include "4ByteTracer.hpp"

#include <libdevcore/CommonJS.h>
#include <libevm/VMFace.h>

namespace mcp
{
namespace
{
inline bool isTracedCall(dev::eth::Instruction op)
{
    using dev::eth::Instruction;
    return op == Instruction::CALL || op == Instruction::CALLCODE ||
           op == Instruction::DELEGATECALL || op == Instruction::STATICCALL;
}
}

void FourByteTracer::CaptureStart(dev::eth::ExtVMFace const* /*ext*/, dev::Address const& /*from*/, dev::Address const& address_a,
                                  bool create, dev::bytes const& input, uint64_t /*gas*/, dev::u256 /*value*/)
{
    m_selectors.clear();

    if (create)
        return;

    recordCallSelector(address_a, input);
}

void FourByteTracer::CaptureEnter(dev::eth::Instruction op, dev::Address const& /*from*/, dev::Address const& address_a,
                                  dev::bytes const& input, uint64_t /*gas*/, std::shared_ptr<dev::u256> /*value*/)
{
    if (!isCallInstruction(op))
        return;

    recordCallSelector(address_a, input);
}

mcp::json FourByteTracer::GetResult()
{
    mcp::json result = mcp::json::object();
    for (auto const& entry : m_selectors)
        result[entry.first] = entry.second;
    return result;
}

void FourByteTracer::recordCallSelector(dev::Address const& address_a, dev::bytes const& input)
{
    if (input.size() < 4)
        return;

    if (dev::eth::isPrecompiledContract(address_a))
        return;

    dev::bytes selector(input.begin(), input.begin() + 4);
    std::string key = dev::toJS(selector) + "-" +
        std::to_string(static_cast<uint64_t>(input.size() - 4));
    ++m_selectors[key];

}

bool FourByteTracer::isCallInstruction(dev::eth::Instruction op) const
{
    return isTracedCall(op);
}
}
