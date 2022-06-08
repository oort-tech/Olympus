#pragma once

#include <libdevcore/RLP.h>
#include <mcp/common/common.hpp>
#include <mcp/core/common.hpp>

#include <boost/asio.hpp>

#include <bitset>

namespace mcp
{

bool parse_port(std::string const & string_a, uint16_t & port_a);

enum requesting_block_cause
{
	new_unknown = 0,
	existing_unknown,
	request_peer_info
};

class validate_status
{
public:
	validate_status() = default;
	validate_status(bool ok_a, std::string msg_a): ok(ok_a), msg(msg_a){}
	bool ok;
	std::string msg="";
};

enum joint_processor_level
{
	broadcast = 0,
	request
};

class joint_message
{
  public:
	joint_message() = default;
	joint_message(std::shared_ptr<mcp::block>);
	joint_message(std::shared_ptr<mcp::block>, mcp::summary_hash const &);
	joint_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;

	mcp::sync_request_hash request_id = 0;
	std::shared_ptr<mcp::block> block;
	mcp::summary_hash summary_hash = 0;
	mcp::joint_processor_level level = mcp::joint_processor_level::broadcast;
};

enum class remote_type
{
	none = 0,
	sync = 1,
	missing = 2
};

class block_processor_item
{
  public:
	block_processor_item() = default;
	block_processor_item(mcp::joint_message const &joint_a, std::shared_ptr<std::promise<mcp::validate_status>> local_promise_a) :
		joint(joint_a),
		block_hash(joint_a.block->hash()),
		m_remote_node_id(0),
		m_remote_type(mcp::remote_type::none),
		m_local_promise(local_promise_a)
	{
	}

	block_processor_item(mcp::joint_message const &joint_a, p2p::node_id const &remote_node_id_a, mcp::remote_type const & remote_type_a = mcp::remote_type::none) :
		joint(joint_a),
		block_hash(joint_a.block->hash()),
		m_remote_node_id(remote_node_id_a),
		m_remote_type(remote_type_a)
	{
	}

	bool is_sync() const
	{
		return m_remote_type == mcp::remote_type::sync;
	}

    void set_missing()
    {
        m_remote_type = mcp::remote_type::missing;
    }

	bool is_missing() const
	{
		return m_remote_type == mcp::remote_type::missing;
	}

	bool is_local() const
	{
		return m_remote_node_id.is_zero();
	}

	mcp::p2p::node_id remote_node_id() const
	{
		assert_x(!is_local());
		return m_remote_node_id;
	}

	std::shared_ptr<std::promise<mcp::validate_status>> get_local_promise()
	{
		assert_x(is_local());
		return m_local_promise;
	}

	void set_local_promise(mcp::validate_status const &ok)
	{
		assert_x(is_local());
		m_local_promise->set_value(ok);
	}

	mcp::joint_message joint;
	mcp::block_hash block_hash;
	bool is_successor = false;
private:
	p2p::node_id m_remote_node_id;
	mcp::remote_type m_remote_type;
	std::shared_ptr<std::promise<mcp::validate_status>> m_local_promise;
};

class joint_request_message
{
  public:
	joint_request_message(mcp::sync_request_hash const& request_id_a, mcp::block_hash const & block_hash_a);
	joint_request_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;

	mcp::sync_request_hash request_id;
	mcp::block_hash block_hash;
};

class transaction_request_message
{
public:
	transaction_request_message(mcp::sync_request_hash const& _request_id, h256 const & _hash):request_id(_request_id), hash(_hash){}
	transaction_request_message(bool &error_a, dev::RLP const &r) { error_a = r.itemCount() != 2; if (error_a) return; request_id = (mcp::sync_request_hash)r[0]; hash = (h256)r[1];}
	void stream_RLP(dev::RLPStream &s) const { s.appendList(2); s << request_id << hash; }

	mcp::sync_request_hash request_id;
	h256 hash;
};

//syncing status
enum sync_response_status
{
	ok = 0,
	stable_mci_error,
	last_stable_mci_error,
	main_chain_error,
	last_stable_mc_hash_error,
	joints_empty_error
};

class catchup_request_message
{
  public:
	catchup_request_message();
	catchup_request_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;

	uint64_t last_stable_mci;
	mcp::block_hash last_stable_mc_hash;
	uint64_t last_known_mci;
	mcp::block_hash unstable_mc_joints_tail;
	mcp::summary_hash first_catchup_chain_summary;
	std::set<mcp::account> arr_witnesses;
	uint64_t distinct_witness_size;
    mcp::sync_request_hash request_id;
};

class catchup_response_message
{
  public:
	catchup_response_message();
	catchup_response_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;

	mcp::sync_response_status status = mcp::sync_response_status::ok;
	std::list<joint_message> unstable_mc_joints;
	std::list<joint_message> stable_last_summary_joints;
	bool is_catchup_chain_complete;
    mcp::sync_request_hash request_id;
};

class hash_tree_request_message
{
  public:
	hash_tree_request_message(mcp::summary_hash from, mcp::summary_hash to);
	hash_tree_request_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;

	mcp::summary_hash from_summary;
	mcp::summary_hash to_summary;

	uint64_t next_start_index = 0;
	mcp::sync_request_hash request_id;
};

class hash_tree_response_message
{
  public:
	class summary_items
	{
	  public:
		mcp::block_hash block_hash;
		mcp::summary_hash summary;
		mcp::summary_hash previous_summary;
		std::list<mcp::summary_hash> parent_summaries;
		std::list<mcp::summary_hash> link_summaries;
		std::set<mcp::summary_hash> skiplist_summaries;
		mcp::block_status status;
		uint64_t stable_index;
		uint64_t mc_timestamp;
		//boost::optional<transaction_receipt> receipt;
		uint64_t level;
		std::shared_ptr<mcp::block> block;
		uint64_t mci;
		std::set<mcp::block_hash> skiplist_block;

		summary_items(mcp::block_hash const & bh, mcp::summary_hash const & sh, mcp::summary_hash const & previous_summary_a,
			std::list<mcp::summary_hash> const & p_summary, std::list<mcp::summary_hash> const & l_summary, 
			std::set<mcp::summary_hash> const & s_summary, mcp::block_status const & status_a, uint64_t const& stable_index_a, uint64_t const& mc_timestamp_a,
			//boost::optional<transaction_receipt> receipt_a,
			uint64_t level_a, std::shared_ptr<mcp::block> const & block_a, uint64_t mci_a, std::set<mcp::summary_hash> const & skiplist_block_a);

		summary_items(bool &error_a, dev::RLP const &r);
		void stream_RLP(dev::RLPStream &s) const;
	};

	struct level_compare
	{
		bool operator()(summary_items const &a, summary_items const &b) const
		{
			return a.mci < b.mci && a.level < b.level;
		}
	};

	hash_tree_response_message();
	hash_tree_response_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;

	std::multiset<summary_items, level_compare> arr_summaries;

	uint64_t          next_start_index = 0;
	mcp::sync_request_hash  request_id;

};

class peer_info_message
{
  public:
	peer_info_message();
	peer_info_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;
	uint64_t min_retrievable_mci;
	std::vector<mcp::block_hash> arr_tip_blocks;
	std::vector<h256> arr_light_tip_blocks; /// account -> nonce
};

class peer_info_request_message
{
  public:
	peer_info_request_message();
	peer_info_request_message(bool &error_a, dev::RLP const &r);
	void stream_RLP(dev::RLPStream &s) const;
};

enum class base_validate_result_codes
{
    ok,
	old,
	invalid_signature,
    invalid_block,
	known_invalid_block
};

class base_validate_result
{
  public:
	mcp::base_validate_result_codes code;
	std::string err_msg;
};

enum class validate_result_codes
{
	ok,
	old,
	missing_parents_and_previous,
	invalid_block,
	known_invalid_block,
	parents_and_previous_include_invalid_block,
};

class validate_result
{
public:
	mcp::validate_result_codes code;
	std::string err_msg;
	std::unordered_set<mcp::block_hash> missing_parents_and_previous;
	h256Hash missing_links;
};

/**
	* Returns seconds passed since unix epoch (posix time)
	*/
inline uint64_t seconds_since_epoch()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline uint64_t future_from_epoch(std::chrono::seconds sec)
{
	return std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() + sec).time_since_epoch()).count();
}

} // namespace mcp
