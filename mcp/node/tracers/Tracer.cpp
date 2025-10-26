#include "Tracer.hpp"
#include "OpcodeTracer.hpp"
#include "CallTracer.hpp"
#include "4ByteTracer.hpp"


mcp::Tracer::~Tracer() = default;


std::shared_ptr<mcp::Tracer> mcp::NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er)
{
    // Check if a specific tracer type is requested
        if (_param.count("tracer") && !_param["tracer"].empty())
    {
        std::string tracerType = _param["tracer"].get<std::string>();
        
                if (tracerType == "opCodeTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<mcp::OpCodeTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<mcp::OpCodeTracer>(_er);
        }
                else if (tracerType == "callTracer")
                {
                        auto tracer = std::make_shared<mcp::CallTracer>();
                        if (_param.count("tracerConfig"))
                                tracer->SetConfig(_param["tracerConfig"]);
                        return tracer;
                }
                else if (tracerType == "4byteTracer")
                {
                        return std::make_shared<mcp::FourByteTracer>();
                }
                // If requested tracer type is unknown, fall back to OpCode tracer
    }

    // Default to OpCode tracer (structured logs like geth's debug_traceTransaction)
    return std::make_shared<mcp::OpCodeTracer>(_er, _param);
}

mcp::Tracer::DebugOptions mcp::Tracer::debugOptions(mcp::json const& _json)
{
        mcp::Tracer::DebugOptions op;
        if (!_json.is_object() || _json.empty())
                return op;
        if (_json.count("enableMemory") && !_json["enableMemory"].empty())
                op.enableMemory = _json["enableMemory"].get<bool>();
        if (_json.count("disableStorage") && !_json["disableStorage"].empty())
                op.disableStorage = _json["disableStorage"].get<bool>();
        //if (_json.count("full_storage") && !_json["full_storage"].empty())
        //      op.fullStorage = _json["full_storage"].get<bool>();
        if (_json.count("disableStack") && !_json["disableStack"].empty())
                op.disableStack = _json["disableStack"].get<bool>();
        //if (_json.count("full_storage") && !_json["full_storage"].empty())
        //      op.fullStorage = _json["full_storage"].get<bool>();
        if (_json.count("debug") && !_json["debug"].empty())
                op.debug = _json["debug"].get<bool>();
        if (_json.count("limit") && !_json["limit"].empty())
                op.limit = _json["limit"].get<int>();
        return op;
}
