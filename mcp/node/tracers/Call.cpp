#include "Call.hpp"
#include <libevm/LegacyVM.h>
#include <mcp/node/evm/ExtVM.h>
#include <libdevcore/CommonJS.h>
#include <account/abi.hpp>

using namespace dev::eth;

void mcp::CallTracer::callFrame::processOutput(dev::bytes const& _output, TransactionException const _excepted)
{
    if (_excepted == TransactionException::None)
    {
        Output = _output;
        return;
    }
    Error = mcp::to_transaction_exception_messge(_excepted);

    if (Type == Instruction::CREATE || Type == Instruction::CREATE2)
        To = nullptr;

    if (_excepted != TransactionException::RevertInstruction || _output.size() == 0)
        return;

    Output = _output;
    if (_output.size() < 4)
        return;

    std::string unpacked;
    if (dev::UnpackRevert(_output, unpacked))
        RevertReason = unpacked;
}

void mcp::CallTracer::CaptureTxStart(uint64_t _gasLimit)
{
    gasLimit = _gasLimit;
}

void mcp::CallTracer::CaptureTxEnd(uint64_t _restGas)
{
    callstack[0].GasUsed = uint64_t(m_res->gasUsed);

    if (m_options.WithLog)
    {
        // Logs are not emitted when the call fails
        clearFailedLogs(callstack[0], false);
    }
}

void mcp::CallTracer::CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to, bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value)
{
    callstack[0].Type = _create ? dev::eth::Instruction::CREATE : dev::eth::Instruction::CALL;
    callstack[0].From = _from;
    callstack[0].To = std::make_shared<dev::Address>(_to);
    callstack[0].Input = _input;
    callstack[0].Gas = gasLimit;
    callstack[0].Value = std::make_shared<dev::u256>(_value);
}

void mcp::CallTracer::CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted)
{
    callstack[0].processOutput(_output, _excepted);
}

void mcp::CallTracer::CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to, dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value)
{
    if (m_options.OnlyTopCall)
        return;

    callstack.push_back(callFrame(_inst, _from, std::make_shared<dev::Address>(_to), _input, _gas, _value));
}

void mcp::CallTracer::CaptureExit(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted)
{
    if (m_options.OnlyTopCall)
        return;

    auto size = callstack.size();
    if (size <= 1)
        return;

    // pop call
    callFrame call = callstack.back();
    callstack.pop_back();
    size -= 1;

    call.GasUsed = _gasUsed;
    call.processOutput(_output, _excepted);
    callstack[size - 1].Calls.push_back(call);
}

void mcp::CallTracer::CaptureState(uint64_t PC, dev::eth::Instruction inst, uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
    // Only logs need to be captured via opcode processing
    if (!m_options.WithLog)
        return;

    ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);

    // Avoid processing nested calls when only caring about top call
    if (m_options.OnlyTopCall && ext.depth > 0)
        return;

    if (inst != Instruction::LOG0 && inst != Instruction::LOG1 &&
        inst != Instruction::LOG2 && inst != Instruction::LOG3 &&
        inst != Instruction::LOG4)
        return;

    auto vm = dynamic_cast<LegacyVM const*>(_vm);
    int size = (uint8_t)inst - (uint8_t)Instruction::LOG0;
    u256s stackData = vm->stack();
    int64_t mStart = stackData[stackData.size() - 1].convert_to<int64_t>();
    int64_t mSize = stackData[stackData.size() - 2].convert_to<int64_t>();

    h256s topics(size);
    for (size_t i = 0; i < size; i++)
    {
        h256 topic = stackData[stackData.size() - 2 - (i + 1)];
        topics[i] = topic;
    }

    if (mStart < 0 || mSize < 0)
        return;

    bytesConstRef _data;
    bytes const& memory = vm->memory();
    if (mStart + mSize < memory.size())// slice fully inside memory
        _data = bytesConstRef(memory.data() + mStart, mSize);
    else
    {
        int64_t overlap = memory.size() - mStart;
        if (overlap > 0)
            _data = bytesConstRef(memory.data() + mStart, overlap);
    }

    callstack[callstack.size() - 1].Logs.push_back(callLog{ ext.myAddress, topics, _data.toBytes() });
}

mcp::json mcp::CallTracer::GetResult()
{
    if (callstack.size() != 1)
    {
        mcp::json ret{ mcp::json::object() };
        return ret;
    }

    return toJson(callstack[0]);
}

void mcp::CallTracer::clearFailedLogs(callFrame& _cf, bool _parentFailed)
{
    bool failed = _cf.failed() || _parentFailed;
    // Clear own logs
    if (failed)
        _cf.Logs.clear();
    for (auto& it : _cf.Calls)
    {
        clearFailedLogs(it, failed);
    }
}

mcp::CallTracer::DebugOptions mcp::CallTracer::debugOptions(mcp::json const& _json)
{
    mcp::CallTracer::DebugOptions op;
    if (_json.count("onlyTopCall") && !_json["onlyTopCall"].empty())
        op.OnlyTopCall = _json["onlyTopCall"].get<bool>();
    if (_json.count("withLog") && !_json["withLog"].empty())
        op.WithLog = _json["withLog"].get<bool>();

    return op;
}

mcp::json mcp::CallTracer::toJson(callFrame const& _call)
{
    mcp::json ret{ mcp::json::object() };

    ret["from"] = dev::toJS(_call.From);
    ret["gas"] = dev::toJS(_call.Gas);
    ret["gasUsed"] = dev::toJS(_call.GasUsed);
    if (_call.To)
        ret["to"] = dev::toJS(*_call.To);
    ret["input"] = dev::toJS(_call.Input);
    if (_call.Output.size())
        ret["output"] = dev::toJS(_call.Output);
    if (_call.Error.size())
        ret["error"] = _call.Error;
    if (_call.RevertReason.size())
        ret["revertReason"] = _call.RevertReason;
    if (_call.Calls.size())
    {
        mcp::json callArray{ mcp::json::array() };
        for (auto const& it : _call.Calls) 
        {
            callArray.push_back(toJson(it));
        }
        ret["calls"] = callArray;
    }
    if (_call.Logs.size())
    {
        mcp::json logsArray{ mcp::json::array() };
        for (auto const& it : _call.Logs)
        {
            mcp::json topics = mcp::json::array();
            for (auto const& _topic : it.Topics)
                topics.push_back(dev::toJS(_topic));

            mcp::json logsObj{ mcp::json::object() };
            logsObj["address"] = dev::toJS(it.Address);
            logsObj["topics"] = topics;
            logsObj["data"] = dev::toJS(it.Data);

            logsArray.push_back(logsObj);
        }
        ret["logs"] = logsArray;
    }
    if (_call.Value)
        ret["value"] = dev::toJS(*_call.Value);
    ret["type"] = instructionInfo(_call.Type).name;

    return ret;
}
