#pragma once
#include <libevm/ExtVMFace.h>
#include <libevm/VMFace.h>

namespace mcp
{
	class StandardTrace
	{
	public:
		struct DebugOptions
		{
			bool disable_storage = false;
			bool disable_memory = false;
			bool disable_stack = false;
			bool full_storage = false;
		};

		StandardTrace();
		void operator()(uint64_t _steps, uint64_t _PC, dev::eth::Instruction _inst, bigint _newMemSize,
			bigint _gasCost, bigint _gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* _extVM);

		void setShowMnemonics() { m_showMnemonics = true; }
		void setOptions(DebugOptions _options) { m_options = _options; }

		mcp::json jsonValue() const { return m_trace; }
		std::string styledJson() const;
		std::string multilineTrace() const;

		dev::eth::OnOpFunc onOp()
		{
			return [=](uint64_t _steps, uint64_t _PC, dev::eth::Instruction _inst, bigint _newMemSize,
				bigint _gasCost, bigint _gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* _extVM) {
				(*this)(_steps, _PC, _inst, _newMemSize, _gasCost, _gas, _vm, _extVM);
			};
		}

	private:
		bool m_showMnemonics = false;
		std::vector<dev::eth::Instruction> m_lastInst;
		mcp::json m_trace;
		DebugOptions m_options;
	};

    StandardTrace::DebugOptions debugOptions(mcp::json const& _json);
}