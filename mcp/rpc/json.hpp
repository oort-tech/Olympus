#pragma once
#include <string>
#include <mcp/common/mcp_json.hpp>

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
}