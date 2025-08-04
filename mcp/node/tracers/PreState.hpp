#pragma once
#include "Tracer.hpp"
#include <mcp/node/evm/ExtVM.h>

namespace mcp
{
	class PreStateTracer: public Tracer
	{
	public:
		struct DebugOptions
		{
			bool DiffMode = false;// If true, this tracer will return state modifications
		};
		struct account
		{
			dev::u256 Balance;
			dev::bytes Code;
			dev::u256 Nonce;
			std::map<u256, u256> Storage;
			bool exists() {
				return Nonce > 0 || Code.size() > 0 || Storage.size() > 0 || Balance > 0;
			}
		};
		using state = std::map<dev::Address, account>;


		explicit PreStateTracer(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept :
			m_res{ &_er },
			m_options(debugOptions(_param)) {}

		void CaptureTxStart(uint64_t _gasLimit) override;
		void CaptureTxEnd(uint64_t _restGas) override;

		void CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
			bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value) override;
		void CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override;
		
		void CaptureState(uint64_t PC, dev::eth::Instruction inst,
			uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;


		mcp::json GetResult() override;

	private:
		PreStateTracer::DebugOptions debugOptions(mcp::json const& _json);
		void lookupAccount(dev::Address const& _address);
		void lookupStorage(dev::Address const& _address, dev::u256 _key);

		DebugOptions m_options;
		ExtVM* m_ext;
		ExecutionResult* m_res = nullptr;
		uint64_t gasLimit = 0;
		state pre;
		state post;
		std::unordered_map<dev::Address, bool> created;
		std::unordered_map<dev::Address, bool> deleted;
		bool create;
		dev::Address to;
	};
}