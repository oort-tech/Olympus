
#include "message.hpp"

mcp::joint_message::joint_message(std::shared_ptr<mcp::block> block_a) :
	request_id(0),
	block(block_a),
    summary_hash(0)
{
}

mcp::joint_message::joint_message(std::shared_ptr<mcp::block> block_a, mcp::summary_hash const & summary_a) :
	request_id(0),
	block(block_a),
	summary_hash(summary_a)
{
}

mcp::joint_message::joint_message(bool & error_a, dev::RLP const & r)
{
	if (error_a)
		return;

	error_a = r.itemCount() != 2 && r.itemCount() != 3;
	if (error_a)
		return;

	request_id = (mcp::sync_request_hash)r[0];
	try
	{
		block = std::make_shared<mcp::block>(r[1]);
	}
	catch (Exception& _e)
	{
		error_a = true;
	}
	if (error_a)
		return;

	if (r.itemCount() > 2)
	{
		summary_hash = (mcp::summary_hash)r[2];
	}
}

void mcp::joint_message::stream_RLP(dev::RLPStream & s) const
{
	summary_hash == mcp::summary_hash(0) ? s.appendList(2) : s.appendList(3);
	s << request_id;
	block->streamRLP(s);
	if (summary_hash != mcp::summary_hash(0))
	{
		s << summary_hash;
	}
}

mcp::joint_request_message::joint_request_message(mcp::sync_request_hash const& request_id_a, mcp::block_hash const & block_hash_a)
	:request_id(request_id_a), 
	block_hash(block_hash_a)
{
}

void mcp::joint_request_message::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(2);
	s << request_id << block_hash;
}


mcp::joint_request_message::joint_request_message(bool & error_a, dev::RLP const & r)
{
	error_a = r.itemCount() != 2;
	if (error_a)
		return;

	request_id = (mcp::sync_request_hash)r[0];
	block_hash = (mcp::block_hash)r[1];
}

mcp::catchup_request_message::catchup_request_message(bool & error_a, dev::RLP const & r)
{
	error_a = r.itemCount() != 8;
	if (error_a)
		return;
	last_stable_mci = (uint64_t)r[0];
	last_stable_mc_hash = (mcp::block_hash)r[1];
	last_known_mci = (uint64_t)r[2];
	unstable_mc_joints_tail = (mcp::block_hash)r[3];
	first_catchup_chain_summary = (mcp::summary_hash)r[4];
	for (auto witness : r[5])
		arr_witnesses.insert((dev::Address)witness);
	distinct_witness_size = (uint64_t)r[6];
    request_id = (mcp::sync_request_hash)r[7];
}

mcp::catchup_request_message::catchup_request_message()
{
}

void mcp::catchup_request_message::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(8);
	s << last_stable_mci << last_stable_mc_hash << last_known_mci << unstable_mc_joints_tail << first_catchup_chain_summary;
	s.appendList(arr_witnesses.size());
	for (auto witness : arr_witnesses)
		s << witness;
    s << distinct_witness_size << request_id;
}

mcp::catchup_response_message::catchup_response_message()
{
}

mcp::catchup_response_message::catchup_response_message(bool & error_a, dev::RLP const & r)
{
	error_a = r.itemCount() != 5;
	if (error_a)
		return;

	status = (mcp::sync_response_status)r[0].toInt<uint8_t>();

	for (auto joint : r[1])
	{
		mcp::joint_message msg(error_a, joint);
		if (error_a)
			return;
		unstable_mc_joints.push_back(msg);
	}

	for (auto joint : r[2])
	{
		mcp::joint_message msg(error_a, joint);
		if (error_a)
			return;
		stable_last_summary_joints.push_back(msg);
	}

	is_catchup_chain_complete = r[3].toInt();

    request_id =  (mcp::sync_request_hash)r[4];
}

void mcp::catchup_response_message::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(5);

	s << (uint8_t)status;
	s.appendList(unstable_mc_joints.size());
	for (auto joint : unstable_mc_joints)
			joint.stream_RLP(s);

	s.appendList(stable_last_summary_joints.size());
	for (auto joint : stable_last_summary_joints)
			joint.stream_RLP(s);

	s << is_catchup_chain_complete<< request_id;
}

mcp::hash_tree_request_message::hash_tree_request_message(mcp::summary_hash from, mcp::summary_hash to)
	:from_summary(from),to_summary(to)
{
}

mcp::hash_tree_request_message::hash_tree_request_message(bool & error_a, dev::RLP const &r)
{
	error_a = r.itemCount() != 4;
	if (error_a)
		return;

	from_summary = (mcp::summary_hash)r[0];
	to_summary = (mcp::summary_hash)r[1];

	next_start_index = (uint64_t)r[2];
    request_id = (mcp::sync_request_hash)r[3];
}

void mcp::hash_tree_request_message::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(4);
	s << from_summary << to_summary << next_start_index << request_id;
}

mcp::hash_tree_response_message::hash_tree_response_message()
{
}

mcp::hash_tree_response_message::hash_tree_response_message(bool & error_a, dev::RLP const &r)
{
	error_a = r.itemCount() != 3;
	if (error_a)
		return;

	for (auto i : r[0])
	{
		mcp::hash_tree_response_message::summary_items items(error_a, i);
		if (error_a)
			return;
		arr_summaries.insert(items);
	}

    next_start_index = (uint64_t)r[1];
    request_id = (mcp::sync_request_hash)r[2];
}

mcp::hash_tree_response_message::summary_items::summary_items(mcp::block_hash const & bh, mcp::summary_hash const & sh,
	mcp::summary_hash const & previous_summary_a, std::list<mcp::summary_hash> const & p_summary, std::list<mcp::summary_hash> const & l_summary, std::set<mcp::summary_hash> const & s_summary,
	mcp::block_status const & status_a, uint64_t const& stable_index_a, uint64_t const& mc_timestamp_a,
	uint64_t level_a, std::shared_ptr<mcp::block> const & block_a, uint64_t mci_a, std::set<mcp::summary_hash> const & skiplist_block_a) :
	block_hash(bh), 
	summary(sh),
	previous_summary(previous_summary_a),
	parent_summaries(p_summary), 
	link_summaries(l_summary),
	skiplist_summaries(s_summary),
	status(status_a),
	stable_index(stable_index_a),
	mc_timestamp(mc_timestamp_a),
	//receipt(receipt_a),
	level(level_a), 
	block(block_a),
	mci(mci_a), 
	skiplist_block(skiplist_block_a)
{
}

mcp::hash_tree_response_message::summary_items::summary_items(bool & error_a, dev::RLP const &r)
{
	if (error_a)
		return;

	error_a = r.itemCount() != 14;
	if (error_a)
		return;

	block_hash = (mcp::block_hash)r[0];
	summary = (mcp::summary_hash)r[1];

	previous_summary = (mcp::summary_hash)r[2];

	dev::RLP const & parent_rlp = r[3];
	for(dev::RLP const & p_summary : parent_rlp)
		parent_summaries.push_back((mcp::summary_hash)p_summary);

	dev::RLP const & link_rlp = r[4];
	for (dev::RLP const & l_summary : link_rlp)
		link_summaries.push_back((mcp::summary_hash)l_summary);

	dev::RLP const & skiplist_rlp = r[5];
	for(dev::RLP const & s_summary : skiplist_rlp)
		skiplist_summaries.insert((mcp::summary_hash)s_summary);

	status = mcp::block_status(r[6].toInt<uint8_t>());
	stable_index = r[7].toInt<uint64_t>();
	mc_timestamp = r[8].toInt<uint64_t>();

	//dev::RLP const & receipt_rlp = r[9];
	//if (receipt_rlp.itemCount() == 1)
	//{
	//	receipt = transaction_receipt(error_a, receipt_rlp[0]);
	//	assert_x(!error_a);
	//}
	//else
	//	receipt = boost::none;

	level = (uint64_t)r[10];

	try
	{
		block = std::make_shared<mcp::block>(r[11]);
	}
	catch (Exception& _e)
	{
		error_a = true;
	}

    mci = (uint64_t)r[12];

    dev::RLP const & skiplist_block_rlp = r[13];
    for (dev::RLP const & bh : skiplist_block_rlp)
        skiplist_block.insert((mcp::block_hash)bh);
}

void mcp::hash_tree_response_message::summary_items::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(13);
	s << block_hash << summary << previous_summary;

	s.appendList(parent_summaries.size());
	for (auto p_summary : parent_summaries)
		s << p_summary;

	s.appendList(link_summaries.size());
	for (auto l_summary : link_summaries)
		s << l_summary;

	s.appendList(skiplist_summaries.size());
	for (auto s_summary: skiplist_summaries)
		s << s_summary;

	s << (uint8_t)status;
	s << stable_index;
	s << mc_timestamp;

	//if (receipt)
	//{
	//	s.appendList(1);
	//	receipt->stream_RLP(s);
	//}
	//else
	//{
	//	s.appendList(0);
	//}
	
	s << level;

    block->streamRLP(s);
    s << mci;

    s.appendList(skiplist_block.size());
    for (auto bh : skiplist_block)
        s << bh;
}

void mcp::hash_tree_response_message::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(3);
	s.appendList(arr_summaries.size());
	for (auto summary : arr_summaries)
		summary.stream_RLP(s);
    s << next_start_index << request_id;
}

mcp::peer_info_message::peer_info_message()
{
}

mcp::peer_info_message::peer_info_message(bool & error_a, dev::RLP const &r)
{
	if (error_a)
		return;

	error_a = r.itemCount() != 3;
	if (error_a)
		return;

	min_retrievable_mci = (uint64_t)r[0];
	for (auto r_hash : r[1])
		arr_tip_blocks.push_back((mcp::block_hash)r_hash);
	for (auto r_account_hash : r[2])
		arr_light_tip_blocks.push_back((h256)r_account_hash);
}

void mcp::peer_info_message::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(3);
	s << min_retrievable_mci;
	s.appendList(arr_tip_blocks.size());
	for (auto hash : arr_tip_blocks)
		s << hash;
	s.appendList(arr_light_tip_blocks.size());
	for (auto hash : arr_light_tip_blocks)
		s << hash;
}

mcp::peer_info_request_message::peer_info_request_message()
{
}

mcp::peer_info_request_message::peer_info_request_message(bool & error_a, dev::RLP const &r) 
{
	if (error_a)
		return;
	error_a = r.itemCount() != 0;
}

void mcp::peer_info_request_message::stream_RLP(dev::RLPStream & s) const
{
	s.appendList(0);
}

bool mcp::parse_port(std::string const & string_a, uint16_t & port_a)
{
	bool result;
	size_t converted;
	port_a = std::stoul(string_a, &converted);
	result = converted != string_a.size() || converted > std::numeric_limits<uint16_t>::max();
	return result;
}
