#include "debug.hpp"
#include <libevm/LegacyVM.h>
#include <mcp/node/evm/ExtVM.h>

using namespace dev::eth;
mcp::StandardTrace::StandardTrace() :
	m_trace(mcp::json::array())
{}

bool changesMemory(dev::eth::Instruction _inst)
{
	return
		_inst == Instruction::MSTORE ||
		_inst == Instruction::MSTORE8 ||
		_inst == Instruction::MLOAD ||
		_inst == Instruction::CREATE ||
		_inst == Instruction::CALL ||
		_inst == Instruction::CALLCODE ||
		_inst == Instruction::SHA3 ||
		_inst == Instruction::CALLDATACOPY ||
		_inst == Instruction::CODECOPY ||
		_inst == Instruction::EXTCODECOPY ||
		_inst == Instruction::DELEGATECALL;
}

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

	mcp::json stack = mcp::json::array();
	if (vm && !m_options.disable_stack)
	{
		// Try extracting information about the stack from the VM is supported.
		for (auto const& i : vm->stack())
			stack.push_back(toCompactHex(i, 32));
		r["stack"] = stack;
	}

	bool newContext = false;
	Instruction lastInst = Instruction::STOP;

	assert_x(ext.depth > 0);
	if (m_lastInst.size() == ext.depth - 1)
	{
		// starting a new context
		assert(m_lastInst.size() == ext.depth - 1);
		m_lastInst.push_back(inst);
		newContext = true;
	}
	else if (m_lastInst.size() == ext.depth + 1)
	{
		m_lastInst.pop_back();
		lastInst = m_lastInst.back();
	}
	else if (m_lastInst.size() == ext.depth)
	{
		// continuing in previous context
		lastInst = m_lastInst.back();
		m_lastInst.back() = inst;
	}
	else
	{
		cwarn << "GAA!!! Tracing VM and more than one new/deleted stack frame between steps!";
		cwarn << "Attmepting naive recovery...";
		m_lastInst.resize(ext.depth + 1);
	}

	mcp::json memJson = mcp::json::array();
	if (vm && !m_options.disable_memory && (changesMemory(lastInst) || newContext))
	{
		for (unsigned i = 0; i < vm->memory().size(); i += 32)
		{
			bytesConstRef memRef(vm->memory().data() + i, 32);
			memJson.push_back(toHex(memRef));
		}
		r["memory"] = memJson;
	}

	if (!m_options.disable_storage && (m_options.full_storage || changesStorage(lastInst) || newContext))
	{
		mcp::json storage = mcp::json::object();
		for (auto const& i : ext.state().storage(ext.myAddress))
			storage[toCompactHexPrefixed(i.second.first, 1)] = toCompactHex(i.second.second, 32);
		r["storage"] = storage;
	}

	if (m_showMnemonics)
		r["op"] = instructionInfo(inst).name;
	r["pc"] = toString(PC);
	r["gas"] = toString(gas);
	r["gasCost"] = toString(gasCost);
	r["depth"] = toString(ext.depth);
	if (!!newMemSize)
		r["memexpand"] = toString(newMemSize);

	m_trace.push_back(r);
}

std::string mcp::StandardTrace::styledJson() const
{
	return m_trace.dump();
}

std::string mcp::StandardTrace::multilineTrace() const
{
	if (m_trace.empty())
		return{};

	// Each opcode trace on a separate line
	return std::accumulate(std::next(m_trace.begin()), m_trace.end(),
		m_trace[0].dump(),
		[](std::string a, mcp::json b) { return a + b.dump(); });
}

mcp::StandardTrace::DebugOptions mcp::debugOptions(mcp::json const& _json)
{
	mcp::StandardTrace::DebugOptions op;
    if (!_json.is_object() || _json.empty())
        return op;
    if (!_json["disable_storage"].empty())
        op.disable_storage = _json["disable_storage"].get<bool>();
    if (!_json["disable_memory"].empty())
        op.disable_memory = _json["disable_memory"].get<bool>();
    if (!_json["disable_stack"].empty())
        op.disable_stack =_json["disable_stack"].get<bool>();
    if (!_json["full_storage"].empty())
        op.full_storage = _json["full_storage"].get<bool>();
    return op;
}