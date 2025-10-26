#pragma once

#include "Tracer.hpp"
#include <unordered_map>

namespace mcp
{
class FourByteTracer : public Tracer
{
public:
    FourByteTracer() = default;

    void CaptureStart(dev::eth::ExtVMFace const* ext, dev::Address const& from, dev::Address const& address_a,
                      bool create, dev::bytes const& input, uint64_t gas, dev::u256 value) override;

    void CaptureEnter(dev::eth::Instruction op, dev::Address const& from, dev::Address const& address_a,
                      dev::bytes const& input, uint64_t gas, std::shared_ptr<dev::u256> value) override;

    mcp::json GetResult() override;

    std::string GetTracerName() const override { return "4byteTracer"; }

private:
    void recordCallSelector(dev::Address const& address_a, dev::bytes const& input);
    bool isCallInstruction(dev::eth::Instruction op) const;

    std::unordered_map<std::string, uint64_t> m_selectors;
};

} // namespace mcp
