#include "requesting.hpp"

namespace mcp 
{
	requesting_mageger RequestingMageger;
}

mcp::requesting_mageger::requesting_mageger()
{
	counts[sub_packet_type::joint_request][requesting_block_cause::new_unknown] = 0;
	counts[sub_packet_type::joint_request][requesting_block_cause::existing_unknown] = 0;
	counts[sub_packet_type::joint_request][requesting_block_cause::request_peer_info] = 0;

	counts[sub_packet_type::transaction_request][requesting_block_cause::new_unknown] = 0;
	counts[sub_packet_type::transaction_request][requesting_block_cause::existing_unknown] = 0;
	counts[sub_packet_type::transaction_request][requesting_block_cause::request_peer_info] = 0;

	counts[sub_packet_type::approve_request][requesting_block_cause::new_unknown] = 0;
	counts[sub_packet_type::approve_request][requesting_block_cause::existing_unknown] = 0;
	counts[sub_packet_type::approve_request][requesting_block_cause::request_peer_info] = 0;
}

///return: true:successed, false: requesting in the way
bool mcp::requesting_mageger::add(mcp::requesting_item& item_a, bool const& count_a)
{
	bool ret = true;
	UpgradableGuard l(m_lock);
	counts[item_a.m_type][item_a.m_cause]++;
	auto it(m_request_info.get<0>().find(item_a.m_request_hash));
	if (it != m_request_info.get<0>().end())
	{
		if (item_a.m_time > it->m_time + mcp::requesting_mageger::STALLED_TIMEOUT) ///allow request again
		{
			item_a.m_request_id = it->m_request_id; ///used same request id as before
			///upgrade request time
			mcp::requesting_item item(*it);
			item.m_time = item_a.m_time;
			if (count_a)
				item.m_request_count++;

			UpgradeGuard ul(l);
			m_request_info.get<0>().replace(it, item);
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		item_a.m_request_id = mcp::gen_sync_request_hash(item_a.m_node_id, m_random_uint, item_a.m_type);
		m_random_uint++;

		UpgradeGuard ul(l);
		m_request_info.insert(item_a);
	}
	return ret;
}

bool mcp::requesting_mageger::try_erase(h256 const & _h)
{
	UpgradableGuard l(m_lock);
	auto it(m_request_info.get<0>().find(_h));
	if (it != m_request_info.get<0>().end())
	{
		UpgradeGuard ul(l);
		m_request_info.get<0>().erase(it);
		return true;
	}
	return false;
}

std::list<mcp::requesting_item> mcp::requesting_mageger::clear_by_time(uint64_t const& time_a)
{
	WriteGuard ul(m_lock);
	std::list<mcp::requesting_item> result;
	for (auto it = m_request_info.get<1>().begin(); it != m_request_info.get<1>().end();)
	{
		if (time_a > it->m_time + STALLED_TIMEOUT)
		{
			if (it->m_request_count > RETYR_TIMES)
			{
				it = m_request_info.get<1>().erase(it);
			}
			else
			{
				result.push_back(*it);
				it++;
			}
		}
		else
			break;
	}
	return result;
}

std::string mcp::requesting_mageger::get_info()
{
	std::string ret = "size:" + std::to_string(size())
		+ " ,joint new_unknown:" + std::to_string(counts[sub_packet_type::joint_request][requesting_block_cause::new_unknown])
		+ " ,joint existing_unknown:" + std::to_string(counts[sub_packet_type::joint_request][requesting_block_cause::existing_unknown])
		+ " ,joint peer_info:" + std::to_string(counts[sub_packet_type::joint_request][requesting_block_cause::request_peer_info])
		+ " ,joint new_unknown:" + std::to_string(counts[sub_packet_type::transaction_request][requesting_block_cause::new_unknown])
		+ " ,joint existing_unknown:" + std::to_string(counts[sub_packet_type::transaction_request][requesting_block_cause::existing_unknown])
		+ " ,joint peer_info:" + std::to_string(counts[sub_packet_type::transaction_request][requesting_block_cause::request_peer_info])
		+ " ,joint new_unknown:" + std::to_string(counts[sub_packet_type::approve_request][requesting_block_cause::new_unknown])
		+ " ,joint existing_unknown:" + std::to_string(counts[sub_packet_type::approve_request][requesting_block_cause::existing_unknown])
		+ " ,joint peer_info:" + std::to_string(counts[sub_packet_type::approve_request][requesting_block_cause::request_peer_info])
		;
	return ret;
}



