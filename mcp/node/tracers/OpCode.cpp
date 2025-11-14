#include "OpCode.hpp"
#include <libinterpreter/VM.h>
#include <mcp/node/evm/ExtVM.h>

using namespace dev::eth;
void mcp::OpCode::CaptureState(uint64_t PC, dev::eth::Instruction inst,
	uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
	// check if already accumulated the specified number of logs
	if (m_options.limit != 0 && m_options.limit <= m_outValue.size())
		return;

	ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
	auto vm = dynamic_cast<VM const*>(_vm);

	mcp::json r = mcp::json::object();

	r["pc"] = PC;
	r["op"] = instructionInfo(inst).name;
	r["gas"] = gas;
	r["gasCost"] = gasCost;
	r["depth"] = ext.depth + 1;  // depth in standard trace is 1-based

	mcp::json stack = mcp::json::array();
	if (vm && !m_options.disableStack)
	{
		// Try extracting information about the stack from the VM is supported.
		for (auto const& i : vm->stack())
			stack.push_back("0x" + intx::hex(i));
		r["stack"] = stack;
	}

	if (vm && m_options.enableMemory)
	{
		bytes const& memory = vm->memory();
		mcp::json memJson(mcp::json::array());
		for (unsigned i = 0; i < memory.size(); i += 32)
		{
			bytesConstRef memRef(memory.data() + i, 32);
			memJson.push_back(toHex(memRef));
		}
		r["memory"] = memJson;
	}

	if (vm && !m_options.disableStorage &&
		(inst == Instruction::SLOAD || inst == Instruction::SSTORE))
	{
		auto stackData = vm->stack();
		auto stackLen = stackData.size();

		std::map<u256, u256> storage;			
		// capture SLOAD opcodes and record the read entry in the local storage
		if (inst == Instruction::SLOAD && stackLen >= 1)
		{
			auto _key = dev::eth::fromEvmC(intx::be::store<evmc_uint256be>(stackData[stackLen - 1]));
			auto value = ext.state().storage(ext.myAddress, _key);
			m_storage[ext.myAddress][_key] = value;
			storage = m_storage[ext.myAddress];
		}
		else if (inst == Instruction::SSTORE && stackLen >= 2)
		{
			auto _key = dev::eth::fromEvmC(intx::be::store<evmc_uint256be>(stackData[stackLen - 1]));
			auto value = dev::eth::fromEvmC(intx::be::store<evmc_uint256be>(stackData[stackLen - 2]));
			m_storage[ext.myAddress][_key] = value;
			storage = m_storage[ext.myAddress];
		}


		mcp::json _r(mcp::json::object());
		for (auto const& it : storage)
			_r[toCompactHex(it.first, 32)] = toCompactHex(it.second, 32);
		r["storage"] = _r;
	}

	m_outValue.push_back(r);
}

mcp::json mcp::OpCode::GetResult()
{
	mcp::json ret;
	ret["gas"] = m_res->gasUsed.convert_to<uint64_t>();
	ret["failed"] = m_res->Failed();
	ret["returnValue"] = toHex(m_res->output);
	ret["structLogs"] = m_outValue;
	return ret;
}

mcp::OpCode::DebugOptions mcp::OpCode::debugOptions(mcp::json const& _json)
{
	mcp::OpCode::DebugOptions op;
	if (!_json.is_object() || _json.empty())
		return op;
	if (_json.count("enableMemory") && !_json["enableMemory"].empty())
		op.enableMemory = _json["enableMemory"].get<bool>();
	if (_json.count("disableStorage") && !_json["disableStorage"].empty())
		op.disableStorage = _json["disableStorage"].get<bool>();
	if (_json.count("disableStack") && !_json["disableStack"].empty())
		op.disableStack = _json["disableStack"].get<bool>();
	if (_json.count("debug") && !_json["debug"].empty())
		op.debug = _json["debug"].get<bool>();
	if (_json.count("limit") && !_json["limit"].empty())
		op.limit = _json["limit"].get<int>();
	return op;
}
