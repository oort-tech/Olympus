# Using `debug_traceTransaction`

This guide shows how to exercise the refreshed tracing stack over the JSON-RPC interface. All examples use `curl` against a local node at `http://127.0.0.1:8765`

## Prerequisites

- The Olympus node is running with RPC enabled on port 8765.
- You have at least one transaction hash to inspect.
- `jq` for accutare overview

## Selecting a tracer

`debug_traceTransaction` accepts an optional tracer descriptor as the second parameter. When omitted, the opcode tracer is used by default.

| tracer         | Purpose                                                                 | Notes |
| -------------- | ------------------------------------------------------------------------ | ----- |
| `opCodeTracer` | Produces geth-style `structLogs` with per-opcode state                   | Respects `tracerConfig` toggles for memory, stack, storage, debug, and log limits |
| `callTracer`   | Builds a nested call tree with gas, value, I/O buffers, and revert flags | Ignores opcode-level state; maintains its own call stack |
| `4byteTracer`  | Counts function selectors seen in CALL-like opcodes                      | Skips contract creation and precompiles |

## Opcode tracer (default)

```bash
TX_HASH=0x4bc2cf8940e9031fb5587a0bc05810d1c68465f360800b90349c6d1b8ca698f1
curl -s -X POST http://127.0.0.1:8765 \
  -H 'Content-Type: application/json' \
  -d "{\"jsonrpc\":\"2.0\",\"method\":\"debug_traceTransaction\",\"params\":[\"${TX_HASH}\"],\"id\":1}" \
  | jq '.result.structLogs[]? | {pc, op, depth, gas, gasCost, stack}'
```

### Configuring the opcode tracer

`tracerConfig` keys map directly to `mcp::Tracer::DebugOptions`:

- `enableMemory` (bool, default `true`): include memory snapshots per step.
- `disableStack` (bool, default `false`): skip stack dumps to reduce payload size.
- `disableStorage` (bool, default `false`): skip storage snapshots even on SLOAD/SSTORE.
- `debug` (bool, default `false`): enables additional logging when future hooks are added.
- `limit` (integer, default `0` for unlimited): stop recording after this many `structLogs` entries.

Example request that disables memory and stack capture while keeping the first 250 steps:

```bash
curl -s -X POST http://127.0.0.1:8765 \
  -H 'Content-Type: application/json' \
  -d "{\"jsonrpc\":\"2.0\",\"method\":\"debug_traceTransaction\",\"params\":[\"${TX_HASH}\",{\"tracer\":\"opCodeTracer\",\"tracerConfig\":{\"enableMemory\":false,\"disableStack\":true,\"limit\":250}}],\"id\":2}"
```

## Call tracer

The call tracer reports a single JSON object representing the root call and its descendants. Each frame contains `type`, `from`, `to`, `value`, `gas`, `gasUsed`, `input`, `output`, optional `error`, and a `calls` array for nested invocations.

```bash
curl -s -X POST http://127.0.0.1:8765 \
  -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","method":"debug_traceTransaction","params":["0x4bc2cf8940e9031fb5587a0bc05810d1c68465f360800b90349c6d1b8ca698f1",{"tracer":"callTracer"}],"id":3}' \
  | jq '.result'
```

Sample output fragment:

```json
{
  "type": "CALL",
  "from": "0xB1472472F034dE08c25024Ae4Ba8d6e5121aD93b",
  "to": "0x9970b8A28F639a1Fe8734e952E2512304C64C292",
  "gas": "0x1dcd6500",
  "gasUsed": "0x5208",
  "value": "0x0",
  "input": "0x...",
  "output": "0x",
  "calls": [
    {
      "type": "DELEGATECALL",
      "from": "0x9970b8A28F639a1Fe8734e952E2512304C64C292",
      "to": "0x...",
      "gas": "0x1c9c3800",
      "gasUsed": "0x186a0",
      "value": "0x0",
      "input": "0x..."
    }
  ]
}
```

> Tip: Use `jq '.. | objects | select(has("calls")) | {type, from, to, gasUsed}'` to surface the nested frames.

## 4-byte tracer

The 4-byte tracer tallies function selectors encountered in contract calls. Keys follow the pattern `0x<selector>-<payload_length>` and values are counts.

```bash
curl -s -X POST http://127.0.0.1:8765 \
  -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","method":"debug_traceTransaction","params":["0x9970b8a28f639a1fe8734e952e2512304c64c2929511fadc8560ffb8baa327d1",{"tracer":"4byteTracer"}],"id":4}' \
  | jq '.result'
```

Example response:

```json
{
  "0x6057361d-32": 1 // store()
}
```

Selectors from precompiled contracts and contract-creation code are ignored. Repeated selectors across nested calls are accumulated in the final map.

