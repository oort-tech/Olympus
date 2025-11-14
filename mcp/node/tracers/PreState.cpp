#include "PreState.hpp"
#include <libinterpreter/VM.h>
#include <libdevcore/CommonJS.h>

using namespace dev::eth;

void mcp::PreStateTracer::CaptureTxStart(uint64_t _gasLimit)
{
    gasLimit = _gasLimit;
}

void mcp::PreStateTracer::CaptureTxEnd(uint64_t _restGas)
{
    if (!m_options.DiffMode)
        return;

    for (auto it = pre.begin(); it != pre.end();)
    {
        dev::Address addr = it->first;
        account state = it->second;
        // The deleted account's state is pruned from `post` but kept in `pre`
        if (deleted.count(addr) && deleted[addr])
        {
            ++it;
            continue;
        }

        bool modified = false;
        account postAccount;
        dev::u256 newBalance = m_ext->balance(addr);
        dev::u256 newNonce = m_ext->getNonce(addr);
        dev::bytes newCode = m_ext->codeAt(addr);

        if (newBalance != *pre[addr].Balance)
        {
            modified = true;
            postAccount.Balance = newBalance;
        }
        if (newNonce != *pre[addr].Nonce)
        {
            modified = true;
            postAccount.Nonce = newNonce;
        }
        if (newCode != pre[addr].Code)
        {
            modified = true;
            postAccount.Code = newCode;
        }

        for (auto const& st : state.Storage)
        {
            dev::u256 key = st.first;
            dev::u256 val = st.second;
            // don't include the empty slot
            if (val == 0)
                pre[addr].Storage.erase(key);

            dev::u256 newVal = m_ext->store(addr, key);
            if (val == newVal)
                pre[addr].Storage.erase(key);
            else
            {
                modified = true;
                if (newVal != 0)
                    postAccount.Storage[key] = newVal;
            }
        }
        if (modified)
        {
            post[addr] = postAccount;
            ++it;
        }
        else
            // if state is not modified, then no need to include into the pre state
            pre.erase(it++);
    }

    // the new created contracts' prestate were empty, so delete them
    for (auto const& a : created)
    {
        // the created contract maybe exists in statedb before the creating tx
        if (pre.count(a.first) && !pre[a.first].exists())
            pre.erase(a.first);
    }
}

void mcp::PreStateTracer::CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to, bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value)
{
    m_ext = const_cast<ExtVM*>(dynamic_cast<ExtVM const*>(_voidExt));
    create = _create;
    to = _to;

    lookupAccount(_from);
    lookupAccount(_to);
    //lookupAccount(m_ext->envInfo().author());//not support Coinbase

    ///The recipient balance includes the value transferred.
    pre[_to].Balance = *pre[_to].Balance - _value;

    // The sender balance is after reducing: value and gasLimit.
    // We need to re-add them to get the pre-tx balance.
    auto consumedGas = _voidExt->gasPrice * gasLimit;
    pre[_from].Balance = *pre[_from].Balance + _value + consumedGas;
    pre[_from].Nonce = *pre[_from].Nonce - 1;

    if (_create && m_options.DiffMode)
        created[_to] = true;
}

void mcp::PreStateTracer::CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted)
{
    if (m_options.DiffMode)
        return;

    // Keep existing account prior to contract creation at that address
    if (create && pre.count(to) && !pre[to].exists())
        // Exclude newly created contract.
        pre.erase(to);
}

void mcp::PreStateTracer::CaptureState(uint64_t PC, dev::eth::Instruction inst, uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
    auto vm = dynamic_cast<VM const*>(_vm);
    auto stackData = vm->stack();
    auto stackLen = stackData.size();
    auto caller = voidExt->myAddress;

    if (stackLen >= 1 &&
        (inst == Instruction::SLOAD || inst == Instruction::SSTORE))
    {
        intx::uint256 _tmp = stackData[stackLen - 1];
        h256 slot(fromEvmC(intx::be::store<evmc_uint256be>(_tmp)));
        lookupStorage(caller, slot);
    }
    else if (stackLen >= 1 &&
        (inst == Instruction::EXTCODECOPY || inst == Instruction::EXTCODEHASH ||
            inst == Instruction::EXTCODESIZE || inst == Instruction::BALANCE || inst == Instruction::SELFDESTRUCT))
    {
        intx::uint256 _tmp = stackData[stackLen - 1];
        dev::Address addr(fromEvmC(intx::be::trunc<evmc::address>(_tmp)));
        lookupAccount(addr);
        if (inst == Instruction::SELFDESTRUCT)
            deleted[caller] = true;
    }
    else if (stackLen >= 5 &&
        (inst == Instruction::DELEGATECALL || inst == Instruction::CALL ||
            inst == Instruction::STATICCALL || inst == Instruction::CALLCODE))
    {
        intx::uint256 _tmp = stackData[stackLen - 2];
        dev::Address addr(fromEvmC(intx::be::trunc<evmc::address>(_tmp)));
        lookupAccount(addr);
    }
    else if (inst == Instruction::CREATE)
    {
        auto nonce = m_ext->getNonce(caller);
        dev::Address addr = right160(sha3(rlpList(caller, nonce)));
        lookupAccount(addr);
        created[addr] = true;
    }
    else if (stackLen >= 4 && inst == Instruction::CREATE2)
    {
        int64_t offset = int64_t(stackData[stackLen - 2]);
        int64_t size = int64_t(stackData[stackLen - 3]);
        bytes const& memory = vm->memory();
        bytesConstRef init = bytesConstRef(memory.data() + offset, size);
        intx::uint256 _tmpSalt = stackData[stackLen - 4];
        h256 salt(fromEvmC(intx::be::store<evmc_uint256be>(_tmpSalt)));
        dev::Address addr = right160(sha3(bytes{ 0xff } + caller.asBytes() + toBigEndian(salt) + sha3(init)));
        lookupAccount(addr);
        created[addr] = true;
    }
}

mcp::json toJson(std::map<dev::u256, dev::u256>const& _storage)
{
    mcp::json ret{ mcp::json::object() };
    for (auto const& it : _storage)
        ret[toCompactHexPrefixed(it.first, 32)] = toCompactHexPrefixed(it.second, 32);

    return ret;
}
mcp::json toJson(mcp::PreStateTracer::account const& _account)
{
    mcp::json ret{ mcp::json::object() };
    if (_account.Balance)
        ret["balance"] = dev::toJS(*_account.Balance);
    if (_account.Code.size())
        ret["code"] = dev::toJS(_account.Code);
    if (_account.Nonce)
        ret["nonce"] = (*_account.Nonce).convert_to<uint64_t>();
    if (_account.Storage.size())
        ret["storage"] = toJson(_account.Storage);
    return ret;
}
mcp::json toJson(mcp::PreStateTracer::state const& _state)
{
    mcp::json ret{ mcp::json::object() };
    for (auto const& it : _state)
    {
        Address addr = it.first;
        ret[addr.hexPrefixed()] = toJson(it.second);
    }
    return ret;
}

mcp::json mcp::PreStateTracer::GetResult()
{
    mcp::json ret{ mcp::json::object() };
    if (m_options.DiffMode)
    {
        ret["post"] = toJson(post);
        ret["pre"] = toJson(pre);
    }
    else
        ret = toJson(pre);

    return ret;
}

mcp::PreStateTracer::DebugOptions mcp::PreStateTracer::debugOptions(mcp::json const& _json)
{
    mcp::PreStateTracer::DebugOptions op;
    if (_json.count("diffMode") && !_json["diffMode"].empty())
        op.DiffMode = _json["diffMode"].get<bool>();

    return op;
}

// lookupAccount fetches details of an account and adds it to the prestate
// if it doesn't exist there.
void mcp::PreStateTracer::lookupAccount(dev::Address const& _address)
{
    if (pre.count(_address))
        return;
   
    pre[_address] = account{
        m_ext->balance(_address),
        m_ext->codeAt(_address),
        m_ext->getNonce(_address)
    };
}

// lookupStorage fetches the requested storage slot and adds
// it to the prestate of the given contract. It assumes `lookupAccount`
// has been performed on the contract before.
void mcp::PreStateTracer::lookupStorage(dev::Address const& _address, dev::u256 _key)
{
    if (pre.count(_address) && pre[_address].Storage.count(_key))
        return;    
    pre[_address].Storage[_key] = m_ext->store(_address, _key);
}
