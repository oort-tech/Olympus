#pragma once

#include <libdevcore/Address.h>
#include <libdevcore/Common.h>
#include <mcp/core/common.hpp>

namespace mcp
{
	class Staking
	{
		
	public:
		Staking() {}
		void update(StakingList const& _v)
		{
			for (auto v : _v)
			{
				m[v.account] = v.balance;
			}
		}

		bool count(dev::Address const& _a)
		{
			return m.count(_a);
		}

		void clear()
		{
			m.clear();
		}
	private:
		std::map<dev::Address, dev::u256> m;
	};

	extern Staking StakingInstance;
}