/*
 * public class for sync,processor,capability.decoupling
 */
#pragma once

#include "message.hpp"
#include <mcp/common/numbers.hpp>

namespace mcp
{
	enum class sub_packet_type
	{
		joint = 0,
		joint_request, //1
		transaction, //2
		transaction_request, //3
		approve, //4
		approve_request, //5
		catchup_request, //6
		catchup_response, //7
		hash_tree_request, //8
		hash_tree_response, //9
		peer_info, //10
		peer_info_request, //11
		hello_info, //12
		hello_info_request, //13
		hello_info_ack, //14
		packet_count = 0x10
	};

	class requesting_item
	{
	public:
		requesting_item() = default;
		/// for transaction
		/// Manager can only use one, so H256 does the conversion
		requesting_item(mcp::p2p::node_id const & node_id_a, h256 const& hash_a, mcp::requesting_block_cause const& cause_a,
			uint64_t const& time_a, mcp::sub_packet_type const& type_a = mcp::sub_packet_type::joint_request) :
			m_node_id(node_id_a),
			m_request_hash(hash_a),
			m_cause(cause_a),
			m_time(time_a),
			m_type(type_a),
			m_request_id(0),
			m_request_count(1)
		{
		}

		mcp::p2p::node_id		m_node_id;
		mcp::block_hash			m_request_hash;
		mcp::requesting_block_cause m_cause;
		mcp::sync_request_hash	m_request_id;

		uint64_t				m_time;
		mcp::sub_packet_type	m_type;
		uint8_t					m_request_count;
	};


	///
	class capability_metrics
	{
	public:
		uint64_t  broadcast_joint = 0;		/// just send

		uint64_t  joint_request = 0;
		uint64_t  send_joint = 0;
		uint64_t  joint = 0;
		
		uint64_t  broadcast_transaction = 0;	/// just send

		uint64_t  transaction_request = 0;
		uint64_t  send_transaction = 0;
		uint64_t  transaction = 0;

		uint64_t  broadcast_approve = 0;
		uint64_t  approve_request = 0;
		uint64_t  send_approve = 0;
		uint64_t  approve = 0;
		
		uint64_t  catchup_request = 0;
		uint64_t  send_catchup = 0;
		uint64_t  catchup_response = 0;

		uint64_t  hash_tree_request = 0;
		uint64_t  send_hash_tree = 0;
		uint64_t  hash_tree_response = 0;

		uint64_t  peer_info_request = 0;
		uint64_t  send_peer_info = 0;
		uint64_t  peer_info = 0;
		
		uint64_t  hello_info_request = 0;
		uint64_t  send_hello_info = 0;
		uint64_t  hello_info = 0;
		
		uint64_t  hello_info_ack = 0;	
	};

	extern capability_metrics CapMetricsRecieved;
	extern capability_metrics CapMetricsSend;

	mcp::sync_request_hash gen_sync_request_hash(p2p::node_id const & id, uint64_t random, mcp::sub_packet_type & request_type_a);
}