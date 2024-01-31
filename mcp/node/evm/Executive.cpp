#include "Executive.hpp"
#include "ExtVM.h"

#include <libevm/LegacyVM.h>
#include <libevm/VMFactory.h>
#include <mcp/common/Exceptions.h>
#include <mcp/common/stopwatch.hpp>

#include <numeric>

using namespace std;
using namespace dev;
using namespace dev::eth;

namespace
{
	std::string dumpStackAndMemory(LegacyVM const& _vm)
	{
		ostringstream o;
		o << "\n    STACK\n";
		for (auto i : _vm.stack())
			o << (h256)i << "\n";
		o << "    MEMORY\n"
			<< ((_vm.memory().size() > 1000) ? " mem size greater than 1000 bytes " :
				memDump(_vm.memory()));
		return o.str();
	};

	std::string dumpStorage(ExtVM const& _ext)
	{
		ostringstream o;
		o << "    STORAGE\n";
		for (auto const& i : _ext.state().storage(_ext.myAddress))
			o << showbase << hex << i.second.first << ": " << i.second.second << "\n";
		return o.str();
	};

}  // namespace



void mcp::Executive::initialize(Transaction const& _transaction)
{
	m_t = _transaction;
	m_baseGasRequired = m_t.baseGasRequired(dev::eth::ConstantinopleSchedule);

	// Avoid unaffordable transactions.
	u256 nonceReq;
	nonceReq = m_s.getNonce(m_t.sender());
	if (m_t.nonce() != nonceReq)
	{
		LOG(m_log.debug) << "Sender: " << m_t.sender().hex() << " Invalid Nonce: Required "
			<< nonceReq << ", received " << m_t.nonce();
		m_excepted = TransactionException::InvalidNonce;
		BOOST_THROW_EXCEPTION(
			dev::eth::InvalidNonce() << RequirementError((bigint)nonceReq, (bigint)m_t.nonce()));
	}

	bigint gasCost = (bigint)m_t.gas() * m_t.gasPrice();
	bigint totalCost = m_t.value() + gasCost;

    if (m_s.balance(m_t.sender()) < totalCost)
    {
		LOG(m_log.debug) << "Not enough cash: Require > " << totalCost << " = " << m_t.gas()
			<< " * " << m_t.gasPrice() << " + " << m_t.value() << " Got"
			<< m_s.balance(m_t.sender()) << " for sender: " << m_t.sender().hexPrefixed();
		m_excepted = TransactionException::NotEnoughCash;
		m_s.incNonce(m_t.sender());
		BOOST_THROW_EXCEPTION(dev::eth::NotEnoughCash() << RequirementError(totalCost, (bigint)m_s.balance(m_t.sender())) << errinfo_comment(m_t.sender().hex()));
	}
    m_gasCost = (u256)gasCost;  // Convert back to 256-bit, safe now.
}

bool mcp::Executive::execute()
{
	//mcp::stopwatch_guard sw("Executive:execute");
	try
	{
		m_s.subBalance(m_t.sender(), m_gasCost);
	}
	catch (dev::eth::NotEnoughCash const&)
	{
		m_s.incNonce(m_t.sender());
		throw;
	}
	assert(m_t.gas() >= (u256)m_baseGasRequired);
    if (m_t.isCreation())
    {
        return create(m_t.sender(), m_t.value(), m_t.gasPrice(), m_t.gas() - (u256)m_baseGasRequired, &m_t.data(), m_t.sender());
    }
    else
    {
        return call(m_t.receiveAddress(), m_t.sender(), m_t.value(), m_t.gasPrice(), bytesConstRef(&m_t.data()), m_t.gas() - (u256)m_baseGasRequired);
    }
	return true;
}

bool mcp::Executive::create(Address const& _txSender, u256 const& _endowment, u256 const& _gasPrice, u256 const& _gas, bytesConstRef _init, Address const& _origin)
{
    // Contract creation by an external account is the same as CREATE opcode
    return createOpcode(_txSender, _endowment, _gasPrice, _gas, _init, _origin);
}

bool mcp::Executive::call(Address const& _receiveAddress, Address const& _senderAddress, u256 const& _value, u256 const& _gasPrice, bytesConstRef _data, u256 const& _gas)
{
    dev::eth::CallParameters params(_senderAddress, _receiveAddress, _receiveAddress, _value, _value, _gas, _data, {});
    return call(params, _gasPrice, _senderAddress);
}

bool mcp::Executive::call(dev::eth::CallParameters const& _p, u256 const& _gasPrice, Address const& _origin)
{
	// If external transaction.
    if (m_t)
    {
        m_s.incNonce(_p.senderAddress);
    }

    m_savepoint = m_s.savepoint();

	if (m_s.is_precompiled(_p.codeAddress, m_envInfo.mc_last_summary_mci()))
	{
		bigint g = m_s.cost_of_precompiled(_p.codeAddress, _p.data);
		if (_p.gas < g)
		{
			m_excepted = TransactionException::OutOfGasBase;
			return true;	// true actually means "all finished - nothing more to be done regarding go().
		}
		else
		{
			m_gas = (u256)(_p.gas - g);
			bytes output;
			bool success;
			tie(success, output) = m_s.execute_precompiled(_p.codeAddress, _p.data);
			size_t outputSize = output.size();
			m_output = owning_bytes_ref{ std::move(output), 0, outputSize };
			if (!success)
			{
				m_gas = 0;
				m_excepted = TransactionException::OutOfGas;
				return true;	// true means no need to run go().
			}
		}
	}
	else
	{
		m_gas = _p.gas;
		if (m_s.addressHasCode(_p.codeAddress))
		{
			bytes const& c = m_s.code(_p.codeAddress);
			h256 codeHash = m_s.codeHash(_p.codeAddress);
			m_ext = std::make_shared<ExtVM>(m_s, m_envInfo, _p.receiveAddress,
				_p.senderAddress, _origin, _p.apparentValue, _gasPrice, _p.data, &c, codeHash,
				0, m_depth + 1, false, _p.staticCall);
		}
	}

	//call trace action
	std::shared_ptr<mcp::call_trace_action> call_action(std::make_shared<mcp::call_trace_action>());
	std::string call_type;
	if (_p.op)
	{
		Instruction op = *_p.op;
		switch (op)
		{
		case Instruction::CALL:
			call_type = "call";
			break;
		case Instruction::CALLCODE:
			call_type = "callcode";
			break;
		case Instruction::DELEGATECALL:
			call_type = "delegatecall";
			break;
		case Instruction::STATICCALL:
			call_type = "staticcall";
			break;
		default:
			call_type = std::to_string((uint8_t)op);
			break;
		}
	}
	else
		call_type = "call";

	call_action->call_type = call_type;
	call_action->from = _p.senderAddress;
	call_action->to = _p.receiveAddress;
	call_action->amount = _p.valueTransfer;
	call_action->data = _p.data.toBytes();
	call_action->gas = _p.gas;
	if (m_depth == 0)
		call_action->gas += m_baseGasRequired;

	std::shared_ptr<mcp::trace> call_trace(std::make_shared<mcp::trace>());
	call_trace->type = mcp::trace_type::call;
	call_trace->action = call_action;
	call_trace->depth = m_depth;

	m_traces.push_back(call_trace);
	assert_x(!m_current_trace);
	m_current_trace = call_trace;

	mcp::uint256_t start_gas_used = gasUsed();

    /// Transfer balance
    m_s.transferBalance(_p.senderAddress, _p.receiveAddress, _p.valueTransfer);

	if (!m_ext)
	{
		std::shared_ptr<mcp::call_trace_result> call_result(std::make_shared<mcp::call_trace_result>());
		call_result->output = m_output.toVector();
		call_result->gas_used = gasUsed() - start_gas_used;

		assert_x(m_current_trace);
		assert_x(m_current_trace->type == mcp::trace_type::call);
		m_current_trace->result = call_result;
		m_current_trace = nullptr;
	}

    return !m_ext;
}

void mcp::Executive::accrueSubState(SubState& _parentContext)
{
    if (m_ext)
        _parentContext += m_ext->sub;
}

bool mcp::Executive::createOpcode(Address const& _sender, u256 const& _endowment, u256 const& _gasPrice, u256 const& _gas, bytesConstRef _init, Address const& _origin)
{
    u256 nonce = m_s.getNonce(_sender);
	m_newAddress = right160(sha3(rlpList(_sender, nonce)));
    return executeCreate(_sender, _endowment, _gasPrice, _gas, _init, _origin);
}

bool mcp::Executive::create2Opcode(Address const& _sender, u256 const& _endowment, u256 const& _gasPrice, u256 const& _gas, bytesConstRef _init, Address const& _origin, u256 const& _salt)
{
    m_newAddress = right160(sha3(bytes{0xff} +_sender.asBytes() + toBigEndian(_salt) + sha3(_init)));
    return executeCreate(_sender, _endowment, _gasPrice, _gas, _init, _origin);
}

bool mcp::Executive::executeCreate(Address const& _sender, u256 const& _endowment, u256 const& _gasPrice,
	u256 const& _gas, bytesConstRef _init, Address const& _origin)
{
    // sichaoy: why should _sender != MaxAddress?
    m_s.incNonce(_sender);

    m_savepoint = m_s.savepoint();

    m_isCreation = true;

    // We can allow for the reverted state (i.e. that with which m_ext is constructed) to contain the m_orig.address, since
    // we delete it explicitly if we decide we need to revert.

    m_gas = _gas;
    bool accountAlreadyExist = (m_s.addressHasCode(m_newAddress) || m_s.getNonce(m_newAddress) >0);
    if (accountAlreadyExist)
    {
		BOOST_LOG(m_log.debug) << "Address already used: " << m_newAddress.hexPrefixed();
        m_gas = 0;
        m_excepted = TransactionException::AddressAlreadyUsed;
        revert();
        m_ext = {}; // cancel the _init execution if there are any scheduled.
        return !m_ext;
    }

    // Transfer ether before deploying the code. This will also create new
    // account if it does not exist yet.
    m_s.transferBalance(_sender, m_newAddress, _endowment);

    m_s.clearStorage(m_newAddress);

    // Schedule _init execution if not empty.
	if (!_init.empty())
	{
		m_ext = std::make_shared<ExtVM>(m_s, m_envInfo, m_newAddress, _sender, _origin, _endowment, _gasPrice, 
			dev::bytesConstRef(), _init, sha3(_init), 0, m_depth + 1, true, false);
	}

	//create trace action
	std::shared_ptr<mcp::create_trace_action> create_action(std::make_shared<mcp::create_trace_action>());
	create_action->from = _sender;
	create_action->init = _init.toVector();
	create_action->amount = _endowment;
	create_action->gas = _gas;
	if (m_depth == 0)
		create_action->gas += m_baseGasRequired;

	std::shared_ptr<mcp::trace> create_trace(std::make_shared<mcp::trace>());
	create_trace->type = mcp::trace_type::create;
	create_trace->action = create_action;
	create_trace->depth = m_depth;

	m_traces.push_back(create_trace);
	assert_x(!m_current_trace);
	m_current_trace = create_trace;

    return !m_ext;
}

bool mcp::Executive::go(dev::eth::OnOpFunc const& _onOp)
{
	//mcp::stopwatch_guard sw("Executive:go");
    if (m_ext)
    {
#if ETH_TIMED_EXECUTIONS
        Timer t;
#endif
        try{
			mcp::uint256_t start_gas_used = gasUsed();
			int64_t start_refunds = m_ext->sub.refunds;

            // Create VM instance. Force Interpreter if tracing requested.
            auto vm = VMFactory::create();
            if (m_isCreation)
            {
                auto out = vm->exec(m_gas, *m_ext, _onOp);
                if (m_res)
                {
                    m_res->gasForDeposit = m_gas;
                    m_res->depositSize = out.size();
                }
                if (out.size() > m_ext->evmSchedule().maxCodeSize)
                    BOOST_THROW_EXCEPTION(OutOfGas());
                else if (out.size() * m_ext->evmSchedule().createDataGas <= m_gas)
                {
                    if (m_res)
                        m_res->codeDeposit = CodeDeposit::Success;
                    m_gas -= out.size() * m_ext->evmSchedule().createDataGas;
                }
                else
                {
                    if (m_ext->evmSchedule().exceptionalFailedCodeDeposit)
                        BOOST_THROW_EXCEPTION(OutOfGas());
                    else
                    {
                        if (m_res)
                            m_res->codeDeposit = CodeDeposit::Failed;
                        out = {};
                    }
                }
                if (m_res)
                    m_res->output = out.toVector(); // copy output to execution result
                m_s.setCode(m_ext->myAddress, out.toVector());

				//create trace result 
				std::shared_ptr<mcp::create_trace_result> create_result(std::make_shared<mcp::create_trace_result>());
				create_result->contract_account = m_ext->myAddress;
				create_result->code = out.toVector();
				create_result->gas_used = gasUsed() - start_gas_used - mcp::uint256_t(m_ext->sub.refunds - start_refunds);

				assert_x(m_current_trace);
				assert_x(m_current_trace->type == mcp::trace_type::create);
				m_current_trace->result = create_result;
				m_current_trace = nullptr;
            }
            else
            {
                m_output = vm->exec(m_gas, *m_ext, _onOp);

				//call trace result 
				std::shared_ptr<mcp::call_trace_result> call_result(std::make_shared<mcp::call_trace_result>());
				call_result->output = m_output.toVector();
				call_result->gas_used = gasUsed() - start_gas_used - mcp::uint256_t(m_ext->sub.refunds - start_refunds);

				assert_x(m_current_trace);
				assert_x(m_current_trace->type == mcp::trace_type::call);
				m_current_trace->result = call_result;
				m_current_trace = nullptr;
            }
        }
        catch (RevertInstruction& _e)
        {
            revert();
            m_output = _e.output();
            m_excepted = TransactionException::RevertInstruction;
        }
        catch (VMException const& _e)
        {
            BOOST_LOG(m_log.debug) << "Safe VM Exception. " << diagnostic_information(_e);
            m_gas = 0;
            m_excepted = toTransactionException(_e);
            revert();
        }
        catch (InternalVMError const& _e)
        {
            cerror << "Internal VM Error (EVMC status code: "
                 << *boost::get_error_info<errinfo_evmcStatusCode>(_e) << ")";
            revert();
            throw;
        }
        catch (Exception const& _e)
        {
            // TODO: AUDIT: check that this can never reasonably happen. Consider what to do if it does.
            cerror << "Unexpected exception in VM. There may be a bug in this implementation. "
                 << diagnostic_information(_e);
            exit(1);
            // Another solution would be to reject this transaction, but that also
            // has drawbacks. Essentially, the amount of ram has to be increased here.
        }
        catch (std::exception const& _e)
        {
            // TODO: AUDIT: check that this can never reasonably happen. Consider what to do if it does.
            cerror << "Unexpected std::exception in VM. Not enough RAM? " << _e.what();
            exit(1);
            // Another solution would be to reject this transaction, but that also
            // has drawbacks. Essentially, the amount of ram has to be increased here.
        }
        if (m_res && m_output)
            // Copy full output:
            m_res->output = m_output.toVector();
        //std::cout << m_res->output << std::endl;

		//error trace result 
		if (m_excepted != mcp::TransactionException::None)
		{
			assert_x(m_current_trace);
			m_current_trace->error_message = to_transaction_exception_messge(m_excepted);
			m_current_trace = nullptr;
		}

#if ETH_TIMED_EXECUTIONS
        cnote << "VM took:" << t.elapsed() << "; gas used: " << (sgas - m_endGas);
#endif
    }
    return true;
}

bool mcp::Executive::finalize()
{
	//mcp::stopwatch_guard sw("Executive:finalize");

    if (m_ext)
    {
         // Accumulate refunds for selfdestructs.
        m_ext->sub.refunds +=
            m_ext->evmSchedule().selfdestructRefundGas * m_ext->sub.selfdestructs.size();

        // Refunds must be applied before the miner gets the fees.
        assert_x(m_ext->sub.refunds >= 0);
		int64_t maxRefund = (static_cast<int64_t>(m_t.gas()) - static_cast<int64_t>(m_gas)) / 2;
		m_gas += min(maxRefund, m_ext->sub.refunds);
    }

    if (m_t)
    {
        m_s.addBalance(m_t.sender(), m_gas * m_t.gasPrice());

		u256 feesEarned = (m_t.gas() - m_gas) * m_t.gasPrice();
        // m_s.addBalance(m_envInfo.author(), feesEarned); //sichaoy: these fees should goes to witness
    }

    // Suicides
    if (m_ext)
        for (auto a: m_ext->sub.selfdestructs)
            m_s.kill(a);

    // Logs..
    if (m_ext)
        m_logs = m_ext->sub.logs;

    if (m_res) // Collect results
    {
        m_res->gasUsed = gasUsed();
        m_res->excepted = m_excepted; // TODO: m_except is used only in ExtVM::call
        m_res->newAddress = m_newAddress;
        m_res->gasRefunded = m_ext ? m_ext->sub.refunds : 0;
    }
    return (m_excepted == TransactionException::None);
}

void mcp::Executive::revert()
{
    if (m_ext)
        m_ext->sub.clear();

    // Set result address to the null one.
    m_newAddress = {};
    m_s.rollback(m_savepoint);
}

mcp::uint256_t mcp::Executive::gasUsed() const
{
    return m_t.gas() - m_gas;
}


mcp::log mcp::Executive::m_log = { mcp::log("vm") };
