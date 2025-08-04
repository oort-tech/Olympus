#pragma once
#include "Tracer.hpp"

namespace mcp
{
	class CallTracer: public Tracer
	{
	public:
		struct DebugOptions
		{
			bool OnlyTopCall = false;// If true, call tracer won't collect any subcalls
			bool WithLog = false;// If true, call tracer will collect event logs
		};

		struct callLog
		{
			dev::Address Address;
			dev::h256s Topics;
			dev::bytes Data;
		};

		struct callFrame
		{
			callFrame() {};
			callFrame(dev::eth::Instruction _inst, dev::Address const& _from, std::shared_ptr<dev::Address> _to, 
				dev::bytes const& _input, uint64_t _gas, std::shared_ptr <dev::u256> _value):
				Type(_inst), From(_from), To(_to), Input(_input), Gas(_gas), Value(_value) {};
			
			dev::eth::Instruction Type = dev::eth::Instruction::STOP;
			dev::Address From;
			uint64_t Gas = 0;
			uint64_t GasUsed = 0;
			std::shared_ptr<dev::Address> To = nullptr;
			dev::bytes Input;
			dev::bytes Output;
			std::string Error;
			std::string RevertReason;
			std::vector<callFrame> Calls;
			std::vector<callLog> Logs;
			std::shared_ptr<dev::u256> Value;

			bool failed() { return Error.size() > 0; }
			void processOutput(dev::bytes const& _output, TransactionException const _excepted);
		};


		explicit CallTracer(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept :
			m_res{ &_er },
			m_options(debugOptions(_param)) {}

		void CaptureTxStart(uint64_t _gasLimit) override;
		void CaptureTxEnd(uint64_t _restGas) override;

		void CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
			bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value) override;
		void CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override;
		
		void CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to,
			dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value) override;
		void CaptureExit(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override;

		void CaptureState(uint64_t PC, dev::eth::Instruction inst,
			uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;


		mcp::json GetResult() override;

	private:
		void clearFailedLogs(callFrame& _cf, bool _parentFailed);
		CallTracer::DebugOptions debugOptions(mcp::json const& _json);
		mcp::json toJson(callFrame const&);
		DebugOptions m_options;
		ExecutionResult* m_res = nullptr;
		std::vector<callFrame> callstack{ callFrame()};
		uint64_t gasLimit = 0;
	};
}