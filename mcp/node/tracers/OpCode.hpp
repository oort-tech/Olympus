#pragma once
#include "Tracer.hpp"

namespace mcp
{
	class OpCode: public Tracer
	{
	public:
		struct DebugOptions
		{
			bool enableMemory = false;// enable memory capture
			bool disableStorage = false;// disable stack capture
			//bool disableMemory = false;
			bool disableStack = false;// disable storage capture
			//bool fullStorage = false;
			bool debug = false; // print output during capture end. for expand.
			int limit = 0;// maximum length of output, but zero means unlimited
		};

		explicit OpCode(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept :
			//Tracer(_er),
			m_res{ &_er },
			m_options(debugOptions(_param)) {}

		void CaptureState(uint64_t PC, dev::eth::Instruction inst,
			uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;

		mcp::json GetResult() override;

	private:
		OpCode::DebugOptions debugOptions(mcp::json const& _json);
		DebugOptions m_options;
		mcp::json m_outValue{ mcp::json::array() };
		ExecutionResult* m_res = nullptr;
	};
}