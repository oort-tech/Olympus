#include "peer_store.hpp"

mcp::p2p::peers_content::peers_content()
{
	m_last_connected = std::chrono::system_clock::now();
	m_last_attempted = std::chrono::system_clock::now();
	m_last_disconnect = disconnect_reason::no_disconnect;
	m_fail_attempts = 0;
}


mcp::p2p::peers_content::peers_content(dev::Slice const & val_a)
{
	assert_x(val_a.size() == sizeof(*this));
	std::copy(reinterpret_cast<uint8_t const *> (val_a.data()), reinterpret_cast<uint8_t const *> (val_a.data()) + sizeof(*this), reinterpret_cast<uint8_t *> (this));
}

dev::Slice mcp::p2p::peers_content::val() const
{
	return dev::Slice((char*)this, sizeof(*this));
}

mcp::p2p::peer_store::peer_store(bool & error_a, boost::filesystem::path const& _path) :
	m_database(std::make_shared<mcp::db::database>(_path))
{
	if (error_a)
		return;

	auto tbops = mcp::db::db_column::default_table_options(mcp::db::database::get_table_cache());
	auto cfops = mcp::db::db_column::default_column_family_options(tbops);
	cfops->OptimizeForSmallDb();
	cfops->prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(1));

	int default_col = m_database->create_column_family(rocksdb::kDefaultColumnFamilyName, cfops);
	m_peers = m_database->set_column_family(default_col, "p");
	m_nodes = m_database->set_column_family(default_col, "n");
	error_a = !m_database->open();
	if (error_a)
		std::cerr << "peer store db open error" << std::endl;
}

bool mcp::p2p::peer_store::peer_get(mcp::db::db_transaction & transaction, node_id const & node_id_a, peers_content & content_a)
{
	std::string result;
	bool ret = transaction.get(m_peers, dev::Slice((char*)node_id_a.data(), node_id_a.size), result);
	if (ret)
		content_a = peers_content(dev::Slice(result));
	return ret;
}

void mcp::p2p::peer_store::peer_put(mcp::db::db_transaction & transaction, node_id const & node_id_a, peers_content const & content_a)
{
	transaction.put(m_peers, dev::Slice((char*)node_id_a.data(), node_id_a.size), content_a.val());
}

void mcp::p2p::peer_store::peer_del(mcp::db::db_transaction & transaction, node_id const & node_id_a)
{
	transaction.del(m_peers, dev::Slice((char*)node_id_a.data(), node_id_a.size));
}

bool mcp::p2p::peer_store::node_get(mcp::db::db_transaction & transaction, std::shared_ptr<node_info> nf_a, node_id & id_a)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		nf_a->endpoint.stream_RLP(s);
		s.swapOut(b_value);
	}
	dev::Slice key((char *)b_value.data(), b_value.size());

	std::string result;
	bool ret = transaction.get(m_nodes, key, result);
	if (ret)
		id_a = mcp::p2p::node_id(mcp::slice_to_h512(result));
	return ret;
}

void mcp::p2p::peer_store::node_put(mcp::db::db_transaction & transaction, std::shared_ptr<node_info> nf_a)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		nf_a->endpoint.stream_RLP(s);
		s.swapOut(b_value);
	}
	dev::Slice key((char *)b_value.data(), b_value.size());

	transaction.put(m_nodes, key, mcp::h512_to_slice(nf_a->id));
}

void mcp::p2p::peer_store::node_del(mcp::db::db_transaction & transaction, std::shared_ptr<node_info> nf_a)
{
	dev::bytes b_value;
	{
		dev::RLPStream s;
		nf_a->endpoint.stream_RLP(s);
		s.swapOut(b_value);
	}
	dev::Slice key((char *)b_value.data(), b_value.size());

	transaction.del(m_nodes, key);
}

mcp::db::forward_iterator mcp::p2p::peer_store::node_begin(mcp::db::db_transaction & transaction)
{
	return transaction.begin(m_nodes);
}
