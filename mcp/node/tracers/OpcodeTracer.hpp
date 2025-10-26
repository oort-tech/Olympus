#pragma once
#include "Tracer.hpp"

namespace mcp
{
	class OpCodeTracer: public Tracer
	{
	public:
	
		explicit OpCodeTracer(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept :
                        m_res{ &_er },
                        m_options(debugOptions(_param)) {}

                void SetCurrentVM(dev::eth::VM const* _vm) override { m_currentInterpreterVm = _vm; }

                void CaptureState(uint64_t PC, dev::eth::Instruction inst,
                        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) noexcept override;

                mcp::json GetResult() override;

                std::string GetTracerName() const override { return "opCodeTracer"; }

        private:
                DebugOptions m_options;
                mcp::json m_outValue{ mcp::json::array() };
                ExecutionResult* m_res = nullptr;
                dev::eth::VM const* m_currentInterpreterVm = nullptr;
        };
} // namespace mcp
