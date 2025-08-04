//#pragma once
//
//#include <memory>
//#include <libdevcore/db.h>
//#include <libdevcore/Common.h>
//#include <libdevcore/Log.h>
//#include <libdevcore/StateCacheDB.h>
//#include <mcp/db/database.hpp>
//using namespace dev;
//
//namespace mcp
//{
//    class block_store;
//
//	class overlay_db : public dev::StateCacheDB
//	{
//	public:
//		explicit overlay_db(mcp::db::db_transaction & transaction_a, mcp::block_store &_store)
//			: transaction(transaction_a), store(_store)
//		{}
//
//		~overlay_db();
//
//		// Copyable
//		overlay_db(overlay_db const&) = default;
//		overlay_db& operator=(overlay_db const&) = default;
//		// Movable
//		overlay_db(overlay_db&&) = default;
//		overlay_db& operator=(overlay_db&&) = default;
//
//		void commit();
//		void rollback();
//
//		std::string lookup(h256 const& _h) const;
//		bool exists(h256 const& _h) const;
//		void kill(h256 const& _h);
//
//		bytes lookupAux(h256 const& _h) const;
//
//	private:
//		using StateCacheDB::clear;
//
//		mcp::block_store &store;
//		mcp::db::db_transaction &transaction;
//	};
//}
