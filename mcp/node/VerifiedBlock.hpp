//#pragma once
//#include <mcp/core/blocks.hpp>
//
//namespace mcp
//{
//	class Transaction;
//
//	/// @brief Verified block info, does not hold block data, but a reference instead
//	//struct VerifiedBlockRef
//	//{
//	//	block info;							///< Prepopulated block info
//	//	std::vector<Transaction> transactions;	///< Verified list of block transactions
//	//};
//
//	/// @brief Verified block info, combines block data and verified info/transactions
//	struct VerifiedBlock
//	{
//		VerifiedBlock() {}
//
//		VerifiedBlock(block&& _bi)
//		{
//			//verified.info = std::move(_bi);
//			block = std::move(_bi);
//		}
//
//		VerifiedBlock(VerifiedBlock&& _other) :
//			//verified(std::move(_other.verified))
//			block(std::move(_other.block))
//		{
//		}
//
//		VerifiedBlock& operator=(VerifiedBlock&& _other)
//		{
//			assert(&_other != this);
//
//			//verified = std::move(_other.verified);
//			block = std::move(_other.block);
//			return *this;
//		}
//
//		//VerifiedBlockRef verified;				///< Verified block structures
//
//		block block;							///< Prepopulated block info
//		std::vector<Transaction> transactions;	///< Verified list of block transactions
//	private:
//		VerifiedBlock(VerifiedBlock const&) = delete;
//		VerifiedBlock operator=(VerifiedBlock const&) = delete;
//	};
//
//	using VerifiedBlocks = std::vector<VerifiedBlock>;
//}
