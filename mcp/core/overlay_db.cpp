#include <thread>
#include <libdevcore/db.h>
#include <libdevcore/Common.h>
#include <libdevcore/SHA3.h>
#include "overlay_db.hpp"
#include <libdevcore/TrieDB.h>
#include <mcp/core/block_store.hpp>

//namespace mcp
//{
//namespace
//{
//inline dev::db::Slice toSlice(dev::h256 const& _h)
//{
//    return dev::db::Slice(reinterpret_cast<char const*>(_h.data()), _h.size);
//}
//
//inline dev::db::Slice toSlice(std::string const& _str)
//{
//    return dev::db::Slice(_str.data(), _str.size());
//}
//
//inline dev::db::Slice toSlice(bytes const& _b)
//{
//    return dev::db::Slice(reinterpret_cast<char const*>(&_b[0]), _b.size());
//}
//
//}  // namespace
//
//overlay_db::~overlay_db() = default;
//
//void overlay_db::commit()
//{
//    for (auto const& i: m_main)
//    {
//        if (i.second.second)
//        {
//            store.contract_main_trie_node_put(transaction, mcp::code_hash(i.first), i.second.first);
//            //LOG(g_log.info) << "commit main: " << i.first.hexPrefixed();
//        }
//    }
//
//    for (auto const& i: m_aux)
//    {
//        if (i.second.second)
//        {
//            bytes b = i.first.asBytes();
//            b.push_back(255);   // for aux
//            store.contract_aux_state_key_put(transaction, b, i.second.first);
//            //LOG(g_log.info) << "commit aux: " << i.first.hexPrefixed();
//        }
//    }
//
//    m_aux.clear();
//    m_main.clear();
//}
//
//bytes overlay_db::lookupAux(dev::h256 const& _h) const
//{
//    bytes ret = dev::StateCacheDB::lookupAux(_h);
//    if (!ret.empty())
//        return ret;
//
//    bytes b = _h.asBytes();
//    b.push_back(255);   // for aux
//
//    bytes value;
//    bool error = store.contract_aux_state_key_get(transaction, b, value);
//    return value;
//}
//
//void overlay_db::rollback()
//{
//#if DEV_GUARDED_DB
//    WriteGuard l(x_this);
//#endif
//    m_main.clear();
//}
//
//std::string overlay_db::lookup(dev::h256 const& _h) const
//{
//    std::string ret = StateCacheDB::lookup(_h);
//    if (!ret.empty())
//        return ret;
//
//    std::string value;
//    bool error = store.contract_main_trie_node_get(transaction, mcp::code_hash(_h), value);
//    //std::cout << "looktup: " << mcp::uint256_union(_h).to_string() << " string: " << value.size() << std::endl;
//    return value;
//}
//
//bool overlay_db::exists(dev::h256 const& _h) const
//{
//    if (StateCacheDB::exists(_h))
//        return true;
//
//    std::string value;
//    return !store.contract_main_trie_node_get(transaction, mcp::code_hash(_h), value);
//
//    /*
//    return m_db && m_db->exists(toSlice(_h));
//    */
//}
//
//void overlay_db::kill(dev::h256 const& _h)
//{
///*
//#if ETH_PARANOIA || 1
//    if (!StateCacheDB::kill(_h))
//    {
//        if (m_db)
//        {
//            if (!m_db->exists(toSlice(_h)))
//            {
//                // No point node ref decreasing for EmptyTrie since we never bother incrementing it
//                // in the first place for empty storage tries.
//                if (_h != EmptyTrie)
//                    cnote << "Decreasing DB node ref count below zero with no DB node. Probably "
//                             "have a corrupt Trie."
//                          << _h;
//                // TODO: for 1.1: ref-counted triedb.
//            }
//        }
//    }
//#else
//    StateCacheDB::kill(_h);
//#endif
//*/
//}
//
//}
