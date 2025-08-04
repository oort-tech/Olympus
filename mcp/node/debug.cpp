#include "debug.hpp"
#include <libevm/LegacyVM.h>
#include <mcp/node/evm/ExtVM.h>

using namespace dev::eth;
//mcp::StandardTrace::StandardTrace() :
//	m_trace(mcp::json::array())
//{}

//bool changesMemory(dev::eth::Instruction _inst)
//{
//	return
//		_inst == Instruction::MSTORE ||
//		_inst == Instruction::MSTORE8 ||
//		_inst == Instruction::MLOAD ||
//		_inst == Instruction::CREATE ||
//		_inst == Instruction::CALL ||
//		_inst == Instruction::CALLCODE ||
//		_inst == Instruction::SHA3 ||
//		_inst == Instruction::CALLDATACOPY ||
//		_inst == Instruction::CODECOPY ||
//		_inst == Instruction::EXTCODECOPY ||
//		_inst == Instruction::DELEGATECALL;
//}

bool changesStorage(dev::eth::Instruction _inst)
{
	return _inst == Instruction::SSTORE;
}

void mcp::StandardTrace::operator()(uint64_t _steps, uint64_t PC, dev::eth::Instruction inst, bigint newMemSize,
	bigint gasCost, bigint gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
	(void)_steps;

	ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
	auto vm = dynamic_cast<LegacyVM const*>(_vm);

	mcp::json r = mcp::json::object();

	r["pc"] = PC;
	r["op"] = instructionInfo(inst).name;
	//r["op"] = static_cast<uint8_t>(inst);
	//if (m_showMnemonics)
	//	r["opName"] = instructionInfo(inst).name;
	r["gas"] = gas.convert_to<uint64_t>()/*toString(gas)*/;
	r["gasCost"] = gasCost.convert_to<uint64_t>()/*toString(gasCost)*/;
	r["depth"] = ext.depth + 1;  // depth in standard trace is 1-based
	//if (!!newMemSize)
	//	r["memexpand"] = toString(newMemSize);

	mcp::json stack = mcp::json::array();
	if (vm && !m_options.disableStack)
	{
		//mcp::log m_log = { mcp::log("vm") };
		// Try extracting information about the stack from the VM is supported.
		for (auto const& i : vm->stack())
		{
			//LOG(m_log.info) << i << " : " << toCompactHexPrefixed(i, 1);
			stack.push_back(toCompactHexPrefixedTrim(i));
		}
			
		r["stack"] = stack;
	}

	//bool newContext = false;
	//Instruction lastInst = Instruction::STOP;

	////assert_x(ext.depth > 0);
	//if (m_lastInst.size() == ext.depth /*- 1*/)
	//{
	//	// starting a new context
	//	assert(m_lastInst.size() == ext.depth/* - 1*/);
	//	m_lastInst.push_back(inst);
	//	newContext = true;
	//}
	//else if (m_lastInst.size() == ext.depth + 2)
	//{
	//	m_lastInst.pop_back();
	//	lastInst = m_lastInst.back();
	//}
	//else if (m_lastInst.size() == ext.depth + 1)
	//{
	//	// continuing in previous context
	//	lastInst = m_lastInst.back();
	//	m_lastInst.back() = inst;
	//}
	//else
	//{
	//	cwarn << "GAA!!! Tracing VM and more than one new/deleted stack frame between steps!";
	//	cwarn << "Attmepting naive recovery...";
	//	m_lastInst.resize(ext.depth + 1);
	//}

	if (vm)
	{
		bytes const& memory = vm->memory();

		mcp::json memJson(mcp::json::array());
		if (!m_options.disableMemory)
		{
			for (unsigned i = 0; i < memory.size(); i += 32)
			{
				bytesConstRef memRef(memory.data() + i, 32);
				memJson.push_back(toHex(memRef));
			}
			r["memory"] = memJson;
		}
		//r["memSize"] = static_cast<uint64_t>(memory.size());
	}

	if (!m_options.disableStorage &&
		(inst == Instruction::SLOAD || inst == Instruction::SSTORE)
		/*(m_options.fullStorage || changesStorage(lastInst) || newContext)*/)
	{
		mcp::json storage(mcp::json::object());
		for (auto const& i : ext.state().storage(ext.myAddress))
			storage[toCompactHex(i.second.first, 32)] =
			toCompactHex(i.second.second, 32);
		r["storage"] = storage;
	}

	if (m_outValue)
		m_outValue->push_back(r);
	else
		*m_outStream << r /*m_fastWriter.write(r)*/ << std::flush;
}

//std::string mcp::StandardTrace::styledJson() const
//{
//	return m_trace.dump();
//}
//
//std::string mcp::StandardTrace::multilineTrace() const
//{
//	if (m_trace.empty())
//		return{};
//
//	// Each opcode trace on a separate line
//	return std::accumulate(std::next(m_trace.begin()), m_trace.end(),
//		m_trace[0].dump(),
//		[](std::string a, mcp::json b) { return a + b.dump(); });
//}

//mcp::StandardTrace::DebugOptions mcp::debugOptions(mcp::json const& _json)
//{
//	mcp::StandardTrace::DebugOptions op;
//    if (!_json.is_object() || _json.empty())
//        return op;
//    if (_json.count("disableStorage") && !_json["disableStorage"].empty())
//        op.disableStorage = _json["disableStorage"].get<bool>();
//    if (_json.count("disableMemory") && !_json["disableMemory"].empty())
//        op.disableMemory = _json["disableMemory"].get<bool>();
//    if (_json.count("disableStack") && !_json["disableStack"].empty())
//        op.disableStack =_json["disableStack"].get<bool>();
//    if (_json.count("full_storage") && !_json["full_storage"].empty())
//        op.fullStorage = _json["full_storage"].get<bool>();
//    return op;
//}