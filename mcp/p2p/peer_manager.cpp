/*
 *peer_manager.cpp
 *data 2018.8
 *manager the peers
 */

#include <mcp/p2p/peer_manager.hpp>
using namespace mcp::p2p;

mcp::p2p::peer_manager::peer_manager(bool & error_a, boost::filesystem::path const & application_path_a):
	store(error_a, application_path_a / "p2pdb")
{
	if (error_a)
		return;
}

bool peer_manager::should_reconnect(node_id const& id, PeerType type, disconnect_reason & reason)
{
	peers_content _peers_content;
	mcp::db::db_transaction transaction(store.create_transaction());
	bool ret = store.peer_get(transaction, id, _peers_content);
	if (!ret)
		return true;

	if (std::chrono::system_clock::now() >= _peers_content.m_last_connected + std::chrono::seconds(fall_back_seconds(_peers_content, type)))
	{
		return true;
	}
	else
	{
		reason = _peers_content.m_last_disconnect;
		return false;
	}
}

void peer_manager::record_connect(node_id const& id, disconnect_reason const& reason)
{
	if (reason == disconnect_reason::no_disconnect)
	{
		clear_disconnect(id);
	}
	else
	{
		add_score(id, reason);
	}
}

void peer_manager::clear_disconnect(node_id const& id)
{
	mcp::db::db_transaction transaction(store.create_transaction());
	store.peer_del(transaction, id);
}

unsigned peer_manager::fall_back_seconds(peers_content const& _p, PeerType type) const
{
	unsigned score = 0;
	if (_p.m_fail_attempts < 3)
		score = 0;
	else if (_p.m_fail_attempts < 10)
		score = _p.m_score * _p.m_fail_attempts * 5 / 10;
	else if (_p.m_fail_attempts < 15)
		score = _p.m_score * (50 + (_p.m_fail_attempts - 10) * 10) / 10;
	else
		score = _p.m_score * (100 + (_p.m_fail_attempts - 15) * 20 ) / 10;

	if (type == PeerType::Required && score > 0)
	{
		score = 30;
	}

	if (score > 6 * 60 * 60)//eq 1 day,then 1 day
	{
		score = 6 * 60 * 60;
	}

	return score;
}

void peer_manager::add_score(node_id const& id, disconnect_reason reason)
{
	peers_content _peers_content;
	{
		mcp::db::db_transaction transaction(store.create_transaction());
		bool ret = store.peer_get(transaction, id, _peers_content);
	}

	unsigned ret = get_score_by_reason(reason);
	_peers_content.m_score += ret;
	_peers_content.m_last_connected = std::chrono::system_clock::now();
	if (ret != 0)//malevolence action
	{
		_peers_content.m_last_attempted = std::chrono::system_clock::now();
		_peers_content.m_fail_attempts++;
		_peers_content.m_last_disconnect = reason;
	}

	mcp::db::db_transaction transaction(store.create_transaction());
	store.peer_put(transaction, id, _peers_content);
}

unsigned peer_manager::get_score_by_reason(disconnect_reason reason) const
{
	unsigned score = 0;

	switch (reason)
	{
	case disconnect_reason::disconnect_requested:
		break;
	case disconnect_reason::tcp_error:
		break;
	case disconnect_reason::bad_protocol:
		score = 30;
		break;
	case disconnect_reason::useless_peer:
		break;
	case disconnect_reason::too_many_peers:
		//score = 25;
		break;
	case disconnect_reason::duplicate_peer:
		break;
	case disconnect_reason::self_connect:
		break;
	case disconnect_reason::client_quit:
		break;
	case disconnect_reason::too_large_packet_size:
		score = 50;
		break;
	case disconnect_reason::no_disconnect:
		break;
	default:
		score = 100;
		break;
	}
	return score;
}

