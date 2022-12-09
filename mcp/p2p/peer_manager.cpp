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

bool peer_manager::should_reconnect(node_id const& id, PeerType type)
{
	peers_content _peers_content;
	mcp::db::db_transaction transaction(store.create_transaction());
	bool ret = store.peer_get(transaction, id, _peers_content);
	if (!ret)
		return true;

	if (std::chrono::system_clock::now() >= _peers_content.m_last_attempted + std::chrono::seconds(fall_back_seconds(_peers_content, type)))
	{
		return true;
	}
	else
	{
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

void peer_manager::onHandshakeFailed(node_id const& _n, HandshakeFailureReason _r)
{
	peers_content _peers_content;
	mcp::db::db_transaction transaction(store.create_transaction());
	bool ret = store.peer_get(transaction, _n, _peers_content);

	//_peers_content.m_last_connected = std::chrono::system_clock::now();
	_peers_content.m_last_attempted = std::chrono::system_clock::now();
	_peers_content.m_fail_attempts++;
	_peers_content.m_lastHandshakeFailure = _r;

	store.peer_put(transaction, _n, _peers_content);
}

void peer_manager::clear_disconnect(node_id const& id)
{
	mcp::db::db_transaction transaction(store.create_transaction());
	store.peer_del(transaction, id);
}

unsigned defaultFallbackSeconds(unsigned _failedAttempts)
{
	if (_failedAttempts < 5)
		return _failedAttempts ? _failedAttempts * 5 : 5;
	else if (_failedAttempts < 15)
		return 25 + (_failedAttempts - 5) * 10;
	else
		return 25 + 100 + (_failedAttempts - 15) * 20;
}


unsigned peer_manager::fall_back_seconds(peers_content const& _p, PeerType type) const
{
	constexpr unsigned oneMonthInSeconds{ 30 * 24 * 3600 };
	constexpr unsigned oneDayInSeconds{ 1 * 24 * 3600 };

	if (type == PeerType::Required)
		return 5;

	switch (_p.m_lastHandshakeFailure)
	{
	case HandshakeFailureReason::FrameDecryptionFailure:
	case HandshakeFailureReason::ProtocolError:
		return oneMonthInSeconds;
	default:
		break;
	}

	switch (_p.m_last_disconnect)
	{
	case disconnect_reason::bad_protocol:
	case disconnect_reason::too_large_packet_size:
	case disconnect_reason::useless_peer:
	case disconnect_reason::malformed:
		return oneMonthInSeconds;
	case disconnect_reason::network_error:
		return oneDayInSeconds;
	case disconnect_reason::tcp_error:
	case disconnect_reason::too_many_peers:
	case disconnect_reason::duplicate_peer:
	{
		if (_p.m_fail_attempts > 25)/// try to connect 25 times one day
			return oneDayInSeconds;
		return 10 * (_p.m_fail_attempts + 1);
	}
	case disconnect_reason::client_quit:
	{
		if (_p.m_fail_attempts > 10)
			return oneDayInSeconds;
		return 25 * (_p.m_fail_attempts + 1);
	}
	default:
		if (_p.m_fail_attempts > 15)/// try to connect 15 times one day
			return oneDayInSeconds;
		return defaultFallbackSeconds(_p.m_fail_attempts);
	}
}

void peer_manager::add_score(node_id const& id, disconnect_reason reason)
{
	peers_content _peers_content;
	mcp::db::db_transaction transaction(store.create_transaction());
	bool ret = store.peer_get(transaction, id, _peers_content);

	//_peers_content.m_last_connected = std::chrono::system_clock::now();
	_peers_content.m_last_attempted = std::chrono::system_clock::now();
	_peers_content.m_fail_attempts++;
	_peers_content.m_last_disconnect = reason;

	store.peer_put(transaction, id, _peers_content);
}



