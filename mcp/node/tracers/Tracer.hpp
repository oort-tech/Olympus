#pragma once
#include <libevm/ExtVMFace.h>
#include <libevm/VMFace.h>
#include <libevm/Logger.h>
#include <mcp/core/common.hpp>
#include <string>

namespace dev { namespace eth { class VM; } }

namespace mcp
{
        class Tracer : public dev::eth::EVMLogger
        {
		public:
			explicit Tracer() {};
			virtual ~Tracer();
			struct DebugOptions
		{
			bool enableMemory = true;// enable memory capture
			bool disableStorage = false;// disable stack capture
			bool disableStack = false;// disable storage capture
			bool debug = false; // print output during capture end. for expand.
			int limit = 0;// maximum length of output, but zero means unlimited
		};

		virtual void SetCurrentVM(dev::eth::VM const* /*_vm*/) {}

		void CaptureTxStart(uint64_t _gasLimit) override {}
		void CaptureTxEnd(uint64_t _restGas) override {}

		void CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
			bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value) override {}
		void CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override {}

		void CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to, 
			dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value) override {}
		void CaptureExit(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override {}

		void CaptureState(uint64_t PC, dev::eth::Instruction inst,
			uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) noexcept override {}
		void CaptureFault(uint64_t _PC, dev::eth::Instruction _inst,
			uint64_t _gasCost, uint64_t _gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* _voidExt) noexcept override {}

		virtual mcp::json GetResult() { return mcp::json::object(); }
		virtual std::string GetTracerName() const { return ""; }
		virtual void SetConfig(mcp::json const& /*options*/) {}
		DebugOptions debugOptions(mcp::json const& _json);

	};
	std::shared_ptr<Tracer> NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er);

} // namespace mcp