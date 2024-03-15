#pragma once
#include <string>
#include <mcp/common/mcp_json.hpp>
#include <mcp/core/common.hpp>
#include <boost/optional.hpp>

namespace mcp
{
	struct jsonrpcMessage
	{
		bool isCall() const;
		bool hasValidVersion() const;
		bool hasValidID() const;
		void SetResponse(mcp::json& _res) const;///set rpc version(jsonrpc), request id

		std::string Version = "";	//"jsonrpc"
		nlohmann::json ID;			//"id"
		std::string Method = "";	//"method"
		nlohmann::json Params;		//"params"
	};

	void from_json(const nlohmann::json& j, jsonrpcMessage& p);
	using jsonrpcMessages = std::vector<jsonrpcMessage>;

	std::pair<jsonrpcMessages,bool> readBatch(std::string const& reader);

	/// only set rpc version(jsonrpc)
	void SetResponse(mcp::json& _res);


	/// eth_call params
	struct BlockNumberOrHash
	{
		boost::optional<mcp::BlockNumber> Number() { return _blockNumber; }
		boost::optional<dev::h256> Hash() { return _blockHash; }

		boost::optional<mcp::BlockNumber> _blockNumber;
		boost::optional<dev::h256> _blockHash;
		bool RequireCanonical; ///not used yet
	};

}