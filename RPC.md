# RPC Interfaces

## account_remove
Remove the keystore file of an account previously imported through account_import or personal_importRawKey.
### Parameters
1. DATA, 20 Bytes - Address.
2. STRING - Password of the address.

#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "account_remove",
    "params": ["0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28", "12345678"]
}
```
### Returns
* Boolean - True if successfully, otherwise return error.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": true
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error": {
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## account_import
Imports the given keystore into the key store.
### Parameters
1. JSON - Keystore file string(V3).
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "account_import",
    "params":["{"address":"8bea69e42045e162ccfed67ecb78d513a6be2eb3","id":"137a236e-9740-477b-affb-921efdf922a1","version":3,"crypto":{"cipher":"aes-128-ctr","cipherparams":{"iv":"c16adf7a5520dbdc08d7e08984451e0f"},"ciphertext":"79cb955240fa62deae7c05c6fed77e55cef7dfa7f70db49e4979c0f3403742bf","kdf":"scrypt","kdfparams":{"salt":"f7833b3bf2ef37426141e09bc7d5437521253736a4aac6bd83fe586507b841dc","n":131072,"dklen":32,"p":1,"r":8},"mac":"79740d2707f4af646915fbd14f9127de6e8ec8fb6d62dc3f78ab3aece05f7305"}}"]
}
```
### Returns
* DATA, 20 Bytes - The address of the keystore.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": "0x8bea69e42045e162ccfed67ecb78d513a6be2eb3"
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error":{
	"code": -32700,
	"message": "Cannot wrap string value as a json-rpc type; only the v3 keystore file."
	}
}
```

## accounts_balances
Returns the decimalism balance of given addresses.
### Parameters
1. Array of DATA, 20 Bytes - The addresses to get the balance of.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "accounts_balances",
    "params": [
        "0xa19B8dB625f0f43f2817aa455E646BE6db85f204",
        "0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28"
    ]
}
```
### Returns
* Array - Array of balance matching all givening addresses.
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": [
        {"0xa19B8dB625f0f43f2817aa455E646BE6db85f204": "0"},
        {"0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28": "0"}
    ]
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error":{
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## block
Returns a block by block hash.
### Parameters
1. DATA, 32 Bytes - Hash of a block.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "block",
    "params":["0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8"]
}
```
### Returns
* Object - A block object, or *null* when no block was found:
  * hash: DATA, 32 Bytes - Hash of the block.
  * from: DATA, 20 Bytes - Sender's address.
  * previous: DATA, 32 Bytes - Hash of the latest block of sender's account before the current block. The value is 0 for the first block of the account.
  * parents: Array - Array of the 32 Bytes hash of parent blocks on the DAG.
  * links: Array - Array of the 32 Bytes hash of the transactions that the witness block referenced.
  * approves: Array - Array of the 32 Bytes hash of the approves that the witness block referenced.
  * last_stable_block:  DATA, 32 Bytes - The last stable block of this block on the DAG.
  * last_summary_block:  DATA, 32 Bytes - The `last_stable_block` of the best parent of this block on the DAG.
  * last_summary:  DATA, 32 Bytes - The summary of `last_summary_block`.
  * timestamp: QUANTITY - The unix timestamp when the block is generated.
  * gasLimit: QUANTITY - Uplimit of the gas for the block.
  * signature: DATA, 65 Bytes - Signature.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "hash": "0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8",
        "from": "0x49a1b41e8ccb704f5c069ef89b08cd33f764e9b3",
        "previous": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "parents": [
            "0x515e804b8c449fa2972ad9f649b47b84b3e70e4f49514f65bccb9ee13feb4090"
        ],
        "links": [
            "0x87d21d3187b2e7a8ed85fb1a2e8f0f7b54ef8e02aa99fffbb7e7377e246bb7df"
        ],
        "approves": [],
        "last_summary": "0x37b7ce47c8af3f10922e687e514b673b46c1a5f6e05c01cc70ac338cf296b3b1",
        "last_summary_block": "0x515e804b8c449fa2972ad9f649b47b84b3e70e4f49514f65bccb9ee13feb4090",
        "last_stable_block": "0x515e804b8c449fa2972ad9f649b47b84b3e70e4f49514f65bccb9ee13feb4090",
        "timestamp": 1701072715,
        "gasLimit": "0x2faf080",
        "signature": "0xcd93ff58377425095e5caaad28bd25f93998dd5736580a80582921dce6a5f6044fc9946df0fd4e320b93ad60f7cbb2ddea4b90d8e993914f771edb9fe51d9fcb00"
    }
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error": {
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## block_state
Returns block state by block hash.
### Parameters
1. DATA, 32 Bytes - Hash of a block.
#### Example
```json
{
    "jsonrpc":"2.0",
    "id":1,
    "method": "block_state",
    "params":["0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8"]
}
```
### Returns
* Object - A block state object, or *null* when no block state was found:
  * content: Object - Object of the witness.
    * level: QUANTITY - The level of the block.
    * witnessed_level: QUANTITY - The witness level of the block.
    * best_parent: DATA, 32 Bytes - The best parent block hash of the block.
  * is_stable: Boolean - Is the block stable or not. *0*: not stable, *1*: stable.
  * stable_content: Object - Stable block state contents. *empty* if the block is not stable.
    * status: QUANTITY - Block status. *0*: success, *1*: double spending, *2*: invalid, *3*: contract execution failed. 
    * stable_index: QUANTITY - Stable block index which indicates the order of blocks on DAG. The value starts from genesis block as 0, and keeps increasing.
    * stable_timestamp: QUANTITY - The Stable unix timestamp.
    * mci：QUANTITY - Main chain index.
    * mc_timestamp: QUANTITY - Main chain timestamp.
    * is_on_mc: Boolean - Is the block on main chain. *0*: not on main chain, *1*: on main chain.
    * is_free: Boolean - Does the block have children. *0*: no children, *1*: has children.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "content": {
            "level": 1,
            "witnessed_level": 1,
            "best_parent": "0x515e804b8c449fa2972ad9f649b47b84b3e70e4f49514f65bccb9ee13feb4090"
        },
        "is_stable": 1,
        "stable_content": {
            "status": 0,
            "stable_index": 1,
            "stable_timestamp": 1701072715,
            "mci": 1,
            "mc_timestamp": 1701072715,
            "is_on_mc": 1,
            "is_free": 0
        }
    }
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error": {
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## block_states
Returns block states of given hashes.
### Parameters
1. Array of DATA, 32 Bytes - Hashes of the blocks.
#### Example
```json
{
    "jsonrpc":"2.0",
    "id":1,
    "method": "block_states",
    "params":[
        "0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8",
        "0x7c5f7b04fe000788e9fec981137c782e11aa8422b015cfe01503c1e43bf8a8b4"
    ]
}
```
### Returns
* Array - Array of block state contents givening hashes.See block state contents in the [`block_state`](RPC.md#block\_state) RPC method. If any of the block hashes doesn't exist, the corresponding list element is *null*.
#### Example
```json
// Success
{
    "result": [{"hashA":{stateA}}, {"hashA":null}, ...]
}
// Failed
{
    "jsonrpc":"2.0",
    "id":1,
    "error":{
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be 	prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## block_traces
Get the trace of internal transactions in a smart contract.
### Parameters
1. DATA, 32 Bytes - Hash of a block.
#### Example
```json
{
    "jsonrpc":"2.0",
    "id":1,
    "method":"block_traces",
    "params": "0x412254AB895FD2E6ADE6F9076CA8297516F2845C989A13AC008CD5D70157AFFB"
}
```
### Returns
* Array - array of trace objects, or empty when no block was found:
  * fields in a trace:
    * type: 0:call，1:create，2:suicide.
    * action: subjective to the type of a trace.
      * call: - call_type: type of call. - from: sender's account. - to: receiver's account. - gas: _string_，gas limit. - data: input data. - amount: _string_，amount in the unit of 10-18 oort.
      * create： - from: sender's account. - gas: _string_，gas limit. - init: the code that creates the contract. - amount: _string_，amount in the unit of 10-18 oort.
      * suicide： - contract_account: contract account. - refund_account: refund account after suicide. - balance: the total amount that is refunded in suicide.
    * result: subjective to the type of a trace. If the execution of the contract failed，this field is empty.
      * call：
        * gas_used：used gas.
        * output：output.
      * create：
        * gas_used：used gas.
        * contract_account: address of the contract created.
        * code：code of the contract created.
      * suicide：result field is *null*
    * error: error message. This field is *null* if the contract execution is successful.
    * subtraces：number of subtraces.
    * trace_address：the layer of trace.
#### Example
```json
// Success
{
    "jsonrpc":"2.0",
    "id":1,
    "result": [{
        "type": 0,  //call
        "action": {
            "call_type": "call",
            "from": "0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
            "to": "0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28",
            "gas": "25000",
            "data": "",
            "amount": "120000000000000000000"
        },
        "result": {
            "gas_used": "21000",
            "output": "",
        },
        "subtraces":0,
        "trace_address": []
    }, ...]
}
// Failed
{
    "jsonrpc":"2.0",
    "id":1,
    "error":{
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## stable_blocks
Return the stable blocks by giving the index of the first block to retrieve and the uplimit of the number of blocks to return.
### Parameters
1. QUANTITY - The index of the first block to retrieve. It can be the value of `next_index` from the result of previous `stable_blocks` call. 
2. QUANTITY - Uplimit of the number of blocks to return. The maximum value is 100.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method":"stable_blocks",
    "params":["0","100"]
}
```
### Returns
* Object - Include array of stable blocks and next_index:
  * blocks: Array - Array of stable blocks object. See block object in the [block](RPC.md#block) RPC method. Object on each index is a stable block object.
  * next_index: QUANTITY - The index of next stable block. *null* if there is no subsequent block.
#### Example
```json
// Success
{
    "result":[{
        "blocks": [{...}, {...}, ...],
        "next_index": 15677
    }]
}
// Failed
{
    "error":{
        "code": -32700,
        "message": "query returned more than 100 results or limit zero."
    }
}
```

## block_summary
Return the summary of a stable block.
### Parameters
1. DATA, 32 Bytes - Hash of a block.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "block_summary",
    "params":["0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8"]
}
```
### Returns
* Object - A block summary object, or *null* when no block was found or not stable:
    * summary: DATA, 32 Bytes - Summary hash value.
    * previous_summary: DATA, 32 Bytes - Previous summary hash.
    * parent_summaries: Array - Array of the 32 Bytes hashes of parent summary.
    * skiplist_summaries: Array - Array of the 32 Bytes hashes of skipped summary.
    * status: QUANTITY - The status of the block state.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "summeries": "0xb4f15ae3d10b1e3ebf742213113785a561ffc6ab737471ad6f9085e76a22be1f",
        "previous_summary": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "parent_summaries": [
            "0x37b7ce47c8af3f10922e687e514b673b46c1a5f6e05c01cc70ac338cf296b3b1"
        ],
        "skiplist_summaries": [],
        "status": 0
    }
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error":{
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## version
Acquire the current node version, rpc interface version, and database version.
### Parameters
None
#### Example
```json
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "version"
}
```
### Returns
* Object.
  * version: String - Current node version.
  * rpc_version: String - RPC interface version.
  * store_version: String - Database version.
#### Example
```json
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "version": "1.0.10",
        "rpc_version": "1",
        "store_version": "1"
    }
}
```

## status
Retrieve the current status of DAG on the node.
### Parameters
None
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "status"
}
```
### Returns
* Object.
    * syncing: Boolean - If the node is syncing to the other nodes，0：not syncing，1：syncing.
    * last_stable_mci: QUANTITY - The `mci` of the last stable block.
    * last_mci: QUANTITY - The `mci` of the last block on the `main chain`.
    * last_stable_block_index：QUANTITY - The `stable index` of the last stable block. `stable index` starts from value 0 and keep increasing. It indicates the order of stable blocks on DAG.
    * epoch: QUANTITY - The current epoch number of mcp.
    * epoch_period: QUANTITY - The number of blocks in main chain included in each epoch.
#### Example
```json
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "syncing": 0,
        "last_stable_mci": 7,
        "last_mci": 8,
        "last_stable_block_index": 7,
        "epoch": 0,
        "epoch_period": 10000
    }
}
```

## peers
List the peers connected to the node.
### Parameters
None
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "peers"
}
```
### Returns
* Array - array of peer object. Object:
  * id: DATA, 64 Bytes - Remote id.
  * endpoint: ip - Remote ip address.
#### Example
```json
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": [
        {
            "id": "0x9968eebe92c18e63a77e6e9861c28a506eb7ba1ff35276b3256018f3946812f16e0e8de1e63ee36613f279011393a4995e54bc6101575ae9d167b69e456f5571",
            "endpoint": "127.0.0.1:30607"
        },
        {
            "id": "0x569a3550a3163d301ae877efb5b58d062a58f833df497ddae657838558b5f09d466258ee2a972969f0c378e1b936763b4afcea76650f0a378821668793f8ec24",
            "endpoint": "127.0.0.1:51911"
        }
    ]
}
```

## nodes
List the nodes connected to the node.
### Parameters
None
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "nodes"
}
```
### Returns
* Array - array of peer object. Object:
  * id: DATA, 64 Bytes - Remote id.
  * endpoint: ip - Remote ip address.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": [
        {
            "id": "0x9968eebe92c18e63a77e6e9861c28a506eb7ba1ff35276b3256018f3946812f16e0e8de1e63ee36613f279011393a4995e54bc6101575ae9d167b69e456f5571",
            "endpoint": "127.0.0.1:30607"
        },
        {
            "id": "0x569a3550a3163d301ae877efb5b58d062a58f833df497ddae657838558b5f09d466258ee2a972969f0c378e1b936763b4afcea76650f0a378821668793f8ec24",
            "endpoint": "127.0.0.1:30608"
        }
    ]
}
```

## witness_list
Retrieve the list of witnesses.
### Parameters
1. QUANTITY - Epoch number.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method":"witness_list",
    "params":["0"]
}
```
### Returns
* Array - Array of witness address.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": [
        "0x05174fa7ab39a36391b17850a2db9afdcf57190e",
        "0x1895ac1edc15389b905bb19537eb0c5b33d8c77a",
        "0x329e6b5b8e59fc73d892958b2ff6a89474e3d067",
        "0x49a1b41e8ccb704f5c069ef89b08cd33f764e9b3",
        "0x827cce78dc6ec7051f2d7bb9e7adaefba7ca3248",
        "0x918d3fe1dbff02fc7521d4a04b50017ce1a7c2ea",
        "0x929f336edb0a39ad5532a462d4a84e1546c5e5de",
        "0xa11b98c54d4189adda8eda97e13c214fedaf0a0f",
        "0xa65ec5c65031d668094cb1b81bb8253ea64a23d7",
        "0xba618c1e3e90d16e6c15d92ed198780dc4ad39c2",
        "0xc2cf7b9eb048c34c2b00175a884543366bbcd029",
        "0xc543a3868f3613eecd109761f71e31832ecf51ba",
        "0xdab8a5fb82eb24ad321751bb2dd8e4cc9a4e45e5",
        "0xf0821dc4ba9419b865aa412170377ca3b44cdb58"
    ]
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error": {
        "code": -32602,
        "message": "The epoch has not yet completed."
    }
}
```

## epoch_approves
List the all approve messages that have been processed in the specified epoch.
### Parameters
1. QUANTITY - Epoch number.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method":"epoch_approves",
    "params":["2"]
}
```
### Returns
* Array - array of approve object. Object:
  * hash: DATA, 32 Bytes - Approve hash.
  * from: DATA, 20 Bytes - Sender, stand for election.
  * proof: DATA - Election proof message.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": [
        {
            "hash": "0x01b46344d3e21800e43fbc0da883fbb110b39826e49657150f7c728ae9a6a5d3",
            "from": "0xf0821dc4ba9419b865aa412170377ca3b44cdb58",
            "proof": "0x0230dcc71f2c751af057f8a3e339d382dadfb791a45dc55d6c8b6bd610b2c0b9f6a5546e9f06769f5e691543cad18a570cf2dadd1b448257ce536f7624033e814ffae81b189947d6086e0f1950363e7153"
        },
        {
            "hash": "0x13483b1e028088496f7fae84f2d3306023d5700d787e5f89938f66728c4c57a3",
            "from": "0x929f336edb0a39ad5532a462d4a84e1546c5e5de",
            "proof": "0x0365900ad931b888d4a45dc3ae62f75affdd3fe61ed75a90b01288ae3bf955b63d5ba9189ff4a624a199d00fe5fd65f2f559f7d9add7009ec7bd0b78c1d1f49436be989c0d795759067ba8b9ca9789e246"
        }
    ]
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error": {
        "code": -32602,
        "message": "The epoch has not yet completed."
    }
}
```

## approve_receipt
Returns receipt about a approve by hash.
### Parameters
1. DATA, 32 Bytes - Hash of a approve.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method":"approve_receipt",
    "params":["0x01b46344d3e21800e43fbc0da883fbb110b39826e49657150f7c728ae9a6a5d3"]
}
```
### Returns
* Object - A approve receipt object, or *null* when no block state was found, Object:
  * from: DATA, 20 Bytes - Sender, stand for election.
  * output: DATA - Receipt output.
  * status: Boolean - Approve execution status.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "from": "0xf0821dc4ba9419b865aa412170377ca3b44cdb58",
        "output": "0xd5e023cc427b924072e7694fe0d29d14336cf47c486401655b405e9fcc712cfc",
        "status": "0x1"
    }
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error": {
        "code": -32700,
        "message": "cannot wrap string value as a json-rpc type; strings must be prefixed with \"0x\", cannot contains invalid hex character, and must be of the correct length."
    }
}
```

## epoch_work_transaction
Returns epoch finalized transaction hash, for epoch reward.
### Parameters
1. QUANTITY - Epoch number.
#### Example
```json
{
    "id":1,
    "jsonrpc":"2.0",
    "method": "epoch_work_transaction",
    "params":["2"]
}
```
### Returns
* DATA, 32 Bytes - Hash of the epoch finalized transaction.
#### Example
```json
// Success
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": "0x6588d572085f0ed4c771ab5286b2c53a9ea78f6a799360f9a3abebb34332e5f2"
}
// Failed
{
    "id": 1,
    "jsonrpc": "2.0",
    "error": {
        "code": -32602,
        "message": "The epoch has not yet completed."
    }
}
```