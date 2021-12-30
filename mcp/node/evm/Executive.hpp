#pragma once

#include <libevm/ExtVMFace.h>
#include <libdevcore/Common.h>
#include <mcp/common/numbers.hpp>
#include <mcp/node/chain_state.hpp>

namespace dev
{
namespace eth
{
    class ExtVM;
}
}

namespace mcp
{
    using namespace dev;
    using namespace dev::eth;

    class Executive
    {
    public:
        // Simple constructor; executive will operate on given state, with the given environment info.
        Executive(chain_state& _s, EnvInfo const& _envInfo, bool isTransaction, std::list<std::shared_ptr<mcp::trace>> & _traces, unsigned _level = 0)
      : m_s(_s),m_envInfo(_envInfo),m_isTransaction(isTransaction),m_traces(_traces), m_depth(_level)
        {
        };

        /// Collect execution results in the result storage provided.
        void setResultRecipient(ExecutionResult& _res) { m_res = &_res; }

        void initialize();
        bool finalize();
        bool execute();
        bool go(dev::eth::OnOpFunc const& _onOp = dev::eth::OnOpFunc());
        void revert();

        /// @returns the log entries created by this operation.
        /// @warning Only valid after finalise().
        log_entries const& logs() const { return m_logs; }
        
        /// @returns total gas used in the transaction/operation.
        /// @warning Only valid after finalise().
        mcp::uint256_t gasUsed() const;

        /// @returns gas remaining after the transaction/operation. Valid after the transaction has been executed.
        u256 gas() const { return m_gas; }

        /// @returns the new address for the created contract in the CREATE operation.
        mcp::account newAddress() const { return m_newAddress; }

        owning_bytes_ref takeOutput() { return std::move(m_output); }

        /// @returns The exception that has happened during the execution if any.
        TransactionException getException() const noexcept { return m_excepted; }

        bool createOpcode(mcp::account const& _sender, u256 const& _endowment, u256 const& _gasPrice, u256 const& _gas, bytesConstRef _init, mcp::account const& _origin);
        bool create2Opcode(mcp::account const& _sender, u256 const& _endowment, u256 const& _gasPrice, u256 const& _gas, bytesConstRef _init, mcp::account const& _origin, u256 const& _salt);
        bool executeCreate(account const & _sender, u256 const& _endowment, u256  const& _gasPrice, u256  const& _gas, dev::bytesConstRef _init, account const& _origin);

        bool create(mcp::account const& _txSender, u256  const& _endowment, u256  const& _gasPrice, u256  const& _gas, dev::bytesConstRef _init, mcp::account const& _origin);
        bool call(mcp::account const& _txReceiver, mcp::account const& _txSender, u256 const& _endowment, u256 const& _gasPrice, u256 const& _gas, dev::bytesConstRef _init);
        bool call(CallParameters const& _cp, u256 const& _gasPrice, mcp::account const& _origin);

        void accrueSubState(SubState& _parentContext);

    private:

        dev::eth::EnvInfo m_envInfo;        ///< Information on the runtime environment.
        std::shared_ptr<ExtVM> m_ext;      ///< The VM externality object for the VM execution or null if no VM is required. shared_ptr used only to allow ExtVM forward reference. This field does *NOT* survive this object.
        owning_bytes_ref m_output;			///< Execution output.
        ExecutionResult* m_res = nullptr;	///< Optional storage for execution results.

        unsigned m_depth = 0;               ///< The context's call-depth.
        TransactionException m_excepted = TransactionException::None;	///< Details if the VM's execution resulted in an exception.
        int64_t m_baseGasRequired = 0;			///< The base amount of gas requried for executing this transaction.
        u256 m_gas = 0;			///< The gas for EVM code execution. Initial amount before go() execution, final amount after go() execution.
        u256 m_refunded = 0;		///< The amount of gas refunded.

        mcp::chain_state &m_s;              ///< The state to which this operation/transaction is applied.

        log_entries m_logs;					///< The log entries created by this transaction. Set by finalize().

        u256 m_gasCost;

        bool m_isCreation = false;
        mcp::account m_newAddress;
        size_t m_savepoint = 0;

        bool m_isTransaction;               ///< to indicate if the Executive is created from a transaction

		std::list<std::shared_ptr<mcp::trace>> & m_traces;
		std::shared_ptr<mcp::trace> m_current_trace;

		static mcp::log m_log;
    };
}