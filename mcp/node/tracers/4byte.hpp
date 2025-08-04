#pragma once
#include "Tracer.hpp"

namespace mcp
{
	class FourByteTracer: public Tracer
	{
	public:
		explicit FourByteTracer() noexcept {}

		void CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
			bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value) override;
		void CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to, 
			dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value) override;
		
		mcp::json GetResult() override;

	private:
		void store(dev::bytes const& _id, int _size);
		std::map<std::string, int> ids;
	};
}