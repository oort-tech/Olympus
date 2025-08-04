#pragma once

#include "ChainOperationParams.hpp"

namespace mcp
{
    class SealEngineFace
    {
    public:
        virtual ~SealEngineFace() {}

        ChainOperationParams const& chainParams() const { return m_params; }
        void setChainParams(ChainOperationParams const& _params) { m_params = _params; }

        //virtual EVMSchedule const& evmSchedule(u256 const& _blockNumber) const = 0;

        virtual bool isPrecompiled(dev::Address const& _a, dev::u256 const& _mci) const///last summary mci
        {
            return m_params.precompiled.count(_a) != 0 && _mci >= m_params.precompiled.at(_a).startingMci();
        }
        virtual dev::bigint costOfPrecompiled(
            dev::Address const& _a, dev::bytesConstRef _in/*, dev::u256 const& _mci*/) const
        {
            return m_params.precompiled.at(_a).cost(_in);
        }
        virtual std::pair<bool, dev::bytes> executePrecompiled(dev::Address const& _a, dev::bytesConstRef _in/*, dev::u256 const&*/) const 
        { 
            return m_params.precompiled.at(_a).execute(_in); 
        }

    private:
        ChainOperationParams m_params;
    };
}
