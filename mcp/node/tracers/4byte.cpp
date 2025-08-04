#include "4byte.hpp"
#include <mcp/core/param.hpp>

using namespace dev::eth;

void mcp::FourByteTracer::CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to, bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value)
{
    if (_input.size() >= 4)
        store(dev::bytes(_input.begin(), _input.begin()+4), _input.size()-4);
}

void mcp::FourByteTracer::CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to, dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value)
{
    if (_input.size() < 4)
        return;

    // primarily we want to avoid CREATE/CREATE2/SELFDESTRUCT
    if (_inst != Instruction::DELEGATECALL && _inst != Instruction::STATICCALL &&
        _inst != Instruction::CALL && _inst != Instruction::CALLCODE)
        return;

    // Skip any pre-compile invocations, those are just fancy opcodes
    if (mcp::param::get()->precompiled.count(_to))
        return;

    store(dev::bytes(_input.begin(), _input.begin() + 4), _input.size() - 4);
}

mcp::json mcp::FourByteTracer::GetResult()
{
    mcp::json ret{ mcp::json::object() };
    for (auto const& it : ids)
        ret[it.first] = it.second;

    return ret;
}

void mcp::FourByteTracer::store(dev::bytes const& _id, int _size)
{
    auto _k = dev::toHexPrefixed(_id) + "-" + std::to_string(_size);
    ids[_k] += 1;
}
