#include "OpcodeTracer.hpp"
#include <libinterpreter/VM.h>
#include <mcp/node/evm/ExtVM.h>
#include <libdevcore/HexUtils.h>
#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace dev::eth;
using namespace dev::eth;

void mcp::OpCodeTracer::CaptureState(uint64_t PC, dev::eth::Instruction inst,
        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* /*_vm*/, dev::eth::ExtVMFace const* voidExt) noexcept
{
        // check if already accumulated the specified number of logs
        if (m_options.limit != 0 && m_options.limit <= m_outValue.size())
                return;

        ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
        auto interpreterVm = m_currentInterpreterVm;

        mcp::json r = mcp::json::object();

        r["pc"] = PC;
        r["op"] = instructionInfo(inst).name;
        r["gas"] = gas;
        r["gasCost"] = gasCost;
        r["depth"] = ext.depth + 1;  // depth in standard trace is 1-based

        mcp::json stack = mcp::json::array();
        if (interpreterVm && !m_options.disableStack)
        {
                for (auto const& value : interpreterVm->stackIntx())
                        stack.push_back(dev::toCompactHexFromIntx(value));

                r["stack"] = stack;
        }

        if (interpreterVm)
{
    // Get memory size
    uint64_t memSize = interpreterVm->memory();

    if (m_options.enableMemory)
    {
        mcp::json memJson(mcp::json::array());
        // Access memory contents via m_mem
        const auto& mem = interpreterVm->m_mem;
        for (unsigned i = 0; i < memSize; i += 32)
        {
            std::vector<uint8_t> memRef(mem.begin() + i, mem.begin() + std::min<uint64_t>(i + 32, memSize));
            memJson.push_back(toHex(memRef));
        }
        r["memory"] = memJson;
    }
}

        if (!m_options.disableStorage &&
                (inst == Instruction::SLOAD || inst == Instruction::SSTORE))
        {
                mcp::json storage(mcp::json::object());
                for (auto const& i : ext.state().storage(ext.myAddress))
                        storage[dev::toCompactHex(i.second.first, 32)] =
                        dev::toCompactHex(i.second.second, 32);
                r["storage"] = storage;
        }

        m_outValue.push_back(r);
}

mcp::json mcp::OpCodeTracer::GetResult()
{
        mcp::json ret;
        ret["gas"] = m_res->gasUsed.convert_to<uint64_t>() /*toJS(t.gas())*/;
        ret["failed"] = m_res->Failed();
        ret["returnValue"] = dev::toHex(m_res->output);
        ret["structLogs"] = m_outValue;
        return ret;
}
