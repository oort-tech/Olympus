#include "config.hpp"

mcp::mcp_networks mcp::mcp_network = mcp::mcp_networks::mcp_live_network;

dev::u256 mcp::gas_price;
uint64_t mcp::chain_id;
//mcp::ChainOperationParams* mcp::ChainConfig = new mcp::ChainOperationParams();

mcp::uint256_t mcp::chainID()
{
	return mcp::chain_id;
}

mcp::Epoch mcp::epoch(uint64_t last_summary_mci)
{
	return last_summary_mci / mcp::epoch_period;
}

//mcp::ChainOperationParams* mcp::chainParams()
//{
//	return ChainConfig;
//}

mcp::witness_param::witness_param(dev::RLP const & rlp)
{
	if (!rlp.isList())
		BOOST_THROW_EXCEPTION(WitnessParam() << errinfo_comment("WitnessParam RLP must be a list"));

	if (rlp.itemCount() != 3)
		BOOST_THROW_EXCEPTION(WitnessParam() << errinfo_comment("too many or to low fields in the WitnessParam RLP"));

	witness_count = (size_t)rlp[0];
	majority_of_witnesses = (size_t)rlp[1];

	dev::RLP const & list_rlp = rlp[2];
	for (dev::RLP const & add : list_rlp)
	{
		witness_list.insert((Address)add);
	}
}

void mcp::witness_param::streamRLP(dev::RLPStream& _s) const
{
	_s.appendList(3);
	_s << witness_count << majority_of_witnesses;
	_s.appendList(witness_list.size());
	for (auto it : witness_list)
		_s << it;
}

