#include "json.hpp"

namespace mcp
{
    const std::string vsn = "2.0";///jsonrpc version

    bool jsonrpcMessage::isCall() const
    {
        return hasValidVersion() && hasValidID() && !Method.empty();
    }

    bool jsonrpcMessage::hasValidVersion() const
    {
        return Version == vsn;
    }

    bool jsonrpcMessage::hasValidID() const
    {
        if (ID.empty() || ID.is_object() || ID.is_array())
            return false;
        return true;
    }

    void jsonrpcMessage::SetResponse(mcp::json& _res) const
    {
        mcp::SetResponse(_res);
        _res["id"] = ID;
    }

    void from_json(const nlohmann::json& j, jsonrpcMessage& p)
    {
        if (j.count("jsonrpc"))
            j.at("jsonrpc").get_to(p.Version);

        if (j.count("id"))
            p.ID = j.at("id");

        if (j.count("method"))
            j.at("method").get_to(p.Method);

        if (j.count("params"))
            p.Params = j.at("params");
    }


    /// return requests and is batch or false
    std::pair<jsonrpcMessages, bool> readBatch(std::string const& reader)
    {
        jsonrpcMessages messages;
        bool batch = false;
        nlohmann::json r = nlohmann::json::parse(reader);
        if (r.is_array())
        {
            messages = r.get<jsonrpcMessages>();
            batch = true;
        }
        else
        {
            jsonrpcMessage _m = r.get<jsonrpcMessage>();
            messages.push_back(_m);
        }
        return std::make_pair(messages, batch);
    }

    void SetResponse(mcp::json& _res)
    {
        _res["jsonrpc"] = vsn;
        _res["id"] = nullptr;
    }
}
