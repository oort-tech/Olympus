# JSON-RPC API
In order for a software application to interact with the mcp blockchain - either by reading blockchain data or sending transactions to the network - it must connect to an mcp node.

JSON-RPC is a stateless, light-weight remote procedure call (RPC) protocol. It defines several data structures and the rules around their processing. It is transport agnostic in that the concepts can be used within the same process, over sockets, over HTTP, or in many various message passing environments. It uses JSON (RFC 4627) as data format.

# CONVENIENCE LIBRARIES
While you may choose to interact directly with mcp clients via the JSON-RPC API, there are often easier options for dapp developers. Many JavaScript and backend API libraries exist to provide wrappers on top of the JSON-RPC API. With these libraries, developers can write intuitive, one-line methods in the programming language of their choice to initialize JSON-RPC requests (under the hood) that interact with mcp.

# CONVENTIONS
## The default block parameter
The following methods have an extra default block parameter:
* [eth_getTransactionCount](#eth_getTransactionCount)
* [eth_call](#eth_call)

When requests are made that act on the state of mcp, the last default block parameter determines the height of the block.

The following options are possible for the defaultBlock parameter:
* HEX String - an integer block number.
* String "earliest" for the earliest/genesis block.
* String "latest" - for the latest mined block.
* String "pending" - for the pending state/transactions.


# JSON-RPC API METHODS

## account_remove
Remove the keystore file of an account previously imported through `account_import` or `personal_importRawKey`.

### **Parameters**
1. `DATA`, 20 Bytes - Address.
2. `String` - Password of the address.

### **Returns**
`Boolean` - `true` if the account was successfully removed, otherwise return error message.

### **Example**
```js
// Request
curl --data '{"method":"account_remove","params":["0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28", "12345678"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": true
}
```



## account_import
Imports the given keystore into the key store.

### **Parameters**
1. `String` - Keystore file(V3).

### **Returns**
`DATA`, 20 Bytes - The account address.

### **Example**
```js
// Request
curl --data '{"method":"account_import","params":["{"address":"8bea69e42045e162ccfed67ecb78d513a6be2eb3","id":"137a236e-9740-477b-affb-921efdf922a1","version":3,"crypto":{"cipher":"aes-128-ctr","cipherparams":{"iv":"c16adf7a5520dbdc08d7e08984451e0f"},"ciphertext":"79cb955240fa62deae7c05c6fed77e55cef7dfa7f70db49e4979c0f3403742bf","kdf":"scrypt","kdfparams":{"salt":"f7833b3bf2ef37426141e09bc7d5437521253736a4aac6bd83fe586507b841dc","n":131072,"dklen":32,"p":1,"r":8},"mac":"79740d2707f4af646915fbd14f9127de6e8ec8fb6d62dc3f78ab3aece05f7305"}}"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": "0x8bea69e42045e162ccfed67ecb78d513a6be2eb3"
}
```



## accounts_balances
Returns the decimalism balance of given addresses.

### **Parameters**
1. `Array` of `DATA`, 20 Bytes - The addresses to get the balance of.

### **Returns**
`Array` - Array of balance matching all givening addresses.

### **Example**
```js
// Request
curl --data '{"method":"accounts_balances","params":["0xa19B8dB625f0f43f2817aa455E646BE6db85f204","0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": [
        {"0xa19B8dB625f0f43f2817aa455E646BE6db85f204": "0"},
        {"0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28": "0"}
    ]
}
```



## block
Returns a block by block hash.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a block.

### **Returns**
`Object` - A block object, or *null* when no block was found:
  * `hash`: `DATA`, 32 Bytes - Hash of the block.
  * `from`: `DATA`, 20 Bytes - Sender's address.
  * `previous`: `DATA`, 32 Bytes - Hash of the latest block of sender's account before the current block. The value is 0 for the first block of the account.
  * `parents`: `Array` - Array of the 32 Bytes hash of parent blocks on the DAG.
  * `links`: `Array` - Array of the 32 Bytes hash of the transactions that the witness block referenced.
  * `approves`: `Array` - Array of the 32 Bytes hash of the approves that the witness block referenced.
  * `last_stable_block`:  `DATA`, 32 Bytes - The last stable block of this block on the DAG.
  * `last_summary_block`:  `DATA`, 32 Bytes - The `last_stable_block` of the best parent of this block on the DAG.
  * `last_summary`:  `DATA`, 32 Bytes - The summary of `last_summary_block`.
  * `timestamp`: `QUANTITY` - The unix timestamp when the block is generated.
  * `gasLimit`: `QUANTITY` - Uplimit of the gas for the block.
  * `signature`: `DATA`, 65 Bytes - Signature.

### **Example**
```js
// Request
curl --data '{"method":"block","params":["0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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
```



## block_state
Returns block state by block hash.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a block.

### **Returns**
`Object` - A block state object, or *null* when no block state was found:
  * `content`: `Object` - Object of the witness.
    * `level`: `QUANTITY` - The level of the block.
    * `witnessed_level`: `QUANTITY` - The witness level of the block.
    * `best_parent`: `DATA`, 32 Bytes - The best parent block hash of the block.
  * `is_stable`: `Boolean` - Is the block stable or not. *0*: not stable, *1*: stable.
  * `stable_content`: `Object` - Stable block state contents. *empty* if the block is not stable.
    * `status`: `QUANTITY` - Block status. *0*: success, *1*: double spending, *2*: invalid, *3*: contract execution failed. 
    * `stable_index`: `QUANTITY` - Stable block index which indicates the order of blocks on DAG. The value starts from genesis block as 0, and keeps increasing.
    * `stable_timestamp`: `QUANTITY` - The Stable unix timestamp.
    * `mci`：`QUANTITY` - Main chain index.
    * `mc_timestamp`: `QUANTITY` - Main chain timestamp.
    * `is_on_mc`: `Boolean` - Is the block on main chain. *0*: not on main chain, *1*: on main chain.
    * `is_free`: `Boolean` - Does the block have children. *0*: no children, *1*: has children.

### **Example**
```js
// Request
curl --data '{"method":"block_state","params":["0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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
```



## block_states
Returns block states of given hashes.

### **Parameters**
1. `Array` of `DATA`, 32 Bytes - Hashes of the blocks.

### **Returns**
`Array` - Array of block state contents givening hashes.See block state contents in the [block_state](#block_state) RPC method. If any of the block hashes doesn't exist, the corresponding list element is *null*.

### **Example**
```js
// Request
curl --data '{"method":"block_states","params":["0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8","0x7c5f7b04fe000788e9fec981137c782e11aa8422b015cfe01503c1e43bf8a8b4"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 161,
    "jsonrpc": "2.0",
    "result": [
        {
            "0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8": {
                "content": {
                    "level": 5,
                    "witnessed_level": 5,
                    "best_parent": "0x16705b18a048ded431598c82bce96f10213fb14e31c77e5748ab573ddf01125c"
                },
                "is_stable": 1,
                "stable_content": {
                    "status": 0,
                    "stable_index": 5,
                    "stable_timestamp": 1701438293,
                    "mci": 5,
                    "mc_timestamp": 1701438293,
                    "is_on_mc": 1,
                    "is_free": 0
                }
            }
        },
        {
            "0x7c5f7b04fe000788e9fec981137c782e11aa8422b015cfe01503c1e43bf8a8b4": null
        }
    ]
}
```



## block_traces
Get the trace of internal transactions in a smart contract.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a block.

### **Returns**
`Array `- Array of trace objects, or empty when no block was found, trace object:
  * `type`: `QUANTITY` - The value of the method such as call or create.
  * `action`: The action to be performed on the receiver id.
    * `call`: Type is call(0).
      * `call_type`: `String` - The type of method such as call, delegatecall. 
      * `from`: `DATA`, 20 Bytes - The address of the sender.
      * `to`: `DATA`, 20 Bytes - The address of the receiver.
      * `gas`: `QUANTITY` - The gas provided by the sender, encoded as decimalism. 
      * `data`: `DATA` - The data sent along with the transaction.
      * `amount`: `QUANTITY` - The integer of the value sent with this transaction, encoded as decimalism.
    * `create`: Type is create(1).
      * `from`: `DATA`, 20 Bytes - The address of the sender. 
      * `gas`: `QUANTITY` - The gas provided by the sender, encoded as decimalism.  
      * `init`: `DATA` - The code that creates the contract.
      * `amount`: `QUANTITY` - The integer of the value sent with this transaction, encoded as decimalism.
    * `suicide`: Type is suicide(2).
      * `contract_account`: `DATA`, 20 Bytes - Contract account.
      * `refund_account`: `DATA`, 20 Bytes - Refund account after suicide.
      * `balance`: `QUANTITY` - The total amount that is refunded in suicide.
  * `result`: Subjective to the type of a trace. If the execution of the contract failed，this field is empty.
    * `call`:
      * `gas_used`: `QUANTITY` - Used gas.
      * `output`: `DATA` - The value returned by the contract call, and it only contains the actual value sent by the RETURN method. If the RETURN method was not executed, the output is empty bytes.
    * `create`:
      * `gas_used`: `QUANTITY` - Used gas.
      * `contract_account`: `DATA`, 20 Bytes - Address of the contract created.
      * `code`: `DATA` - Code of the contract created.
    * `suicide`: Result field is *null*.
  * `error`: `String` - Error message. This field is *null* if the contract execution is successful.
  * `subtraces`: `QUANTITY` - The traces of contract calls made by the transaction.
  * `trace_address`: `Array` of `DATA`, 20 Bytes - The list of addresses where the call was executed, the address of the parents, and the order of the current sub call.

### **Example**
```js
// Request
curl --data '{"method":"block_traces","params":["0x412254AB895FD2E6ADE6F9076CA8297516F2845C989A13AC008CD5D70157AFFB"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id":1,
    "jsonrpc":"2.0",
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
```



## stable_blocks
Return the stable blocks by giving the index of the first block to retrieve and the uplimit of the number of blocks to return.

### **Parameters**
1. `QUANTITY` - The index of the first block to retrieve. It can be the value of `next_index` from the result of previous `stable_blocks` call. 
2. `QUANTITY` - Uplimit of the number of blocks to return. The maximum value is 100.

### **Returns**
`Object` - Include array of stable blocks and next_index:
  * `blocks`: `Array` - Array of stable blocks object. See block object in the [block](#block) RPC method. Object on each index is a stable block object.
  * `next_index`: `QUANTITY` - The index of next stable block. *null* if there is no subsequent block.

### **Example**
```js
// Request
curl --data '{"method":"stable_blocks","params":["0","100"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id":1,
    "jsonrpc":"2.0",
    "result":[{
        "blocks": [{...}, {...}, ...],
        "next_index": 15677
    }]
}
```



## block_summary
Return the summary of a stable block.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a block.

### **Returns**
`Object` - A block summary object, or *null* when no block was found or not stable.
  * `summeries`: `DATA`, 32 Bytes - Summary hash value.
  * `previous_summary`: `DATA`, 32 Bytes - Previous summary hash.
  * `parent_summaries`: `Array` - Array of the 32 Bytes hashes of parent summary.
  * `skiplist_summaries`: `Array` - Array of the 32 Bytes hashes of skipped summary.
  * `status`: `QUANTITY` - The status of the block state.

### **Example**
```js
// Request
curl --data '{"method":"block_summary","params":["0x7763f7c4ebb4301a71ab27ba5d42cbe4cb7eb71eaba479a50f9b888c8017ced8"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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
```



## version
Return the current node version, rpc interface version, and database version.

### **Parameters**
None

### **Returns**
`Object`
  * `version`: `String` - Current node version.
  * `rpc_version`: `String` - RPC interface version.
  * `store_version`: `String` - Database version.

### **Example**
```js
// Request
curl --data '{"method":"version","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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
Return the current status of DAG on the node.

### **Parameters**
None

### **Returns**
`Object`
  * `syncing`: `Boolean` - If the node is syncing to the other nodes，0：not syncing，1：syncing.
  * `last_stable_mci`: `QUANTITY` - The `mci` of the last stable block.
  * `last_mci`: `QUANTITY` - The `mci` of the last block on the `main chain`.
  * `last_stable_block_index`：`QUANTITY` - The `stable index` of the last stable block. `stable index` starts from value 0 and keep increasing. It indicates the order of stable blocks on DAG.
  * `epoch`: `QUANTITY` - The current epoch number of mcp.
  * `epoch_period`: `QUANTITY` - The number of blocks in main chain included in each epoch.

### **Example**
```js
// Request
curl --data '{"method":"status","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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

### **Parameters**
None

### **Returns**
`Array` - Array of peer object. Object:
  * `id`: `DATA`, 64 Bytes - Remote id.
  * `endpoint`: `String` - Remote ip address.

### **Example**
```js
// Request
curl --data '{"method":"peers","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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

### **Parameters**
None

### **Returns**
`Array` - Array of peer object. Object:
  * `id`: `DATA`, 64 Bytes - Remote id.
  * `endpoint`: `String` - Remote ip address.

### **Example**
```js
// Request
curl --data '{"method":"nodes","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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
Return the witness list by epoch.

### **Parameters**
1. `QUANTITY` - Epoch number.

### **Returns**
`Array` - Array of 20 Bytes `DATA` witness address.

### **Example**
```js
// Request
curl --data '{"method":"witness_list","params":["0"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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
```



## epoch_approves
Return all approve messages that have been processed in the specified epoch.

### **Parameters**
1. `QUANTITY` - Epoch number.

### **Returns**
`Array` - Array of approve object. Object:
  * `hash`: `DATA`, 32 Bytes - Approve hash.
  * `from`: `DATA`, 20 Bytes - Sender, stand for election.
  * `proof`: `DATA`, 81 Bytes - Election proof message.

### **Example**
```js
// Request
curl --data '{"method":"epoch_approves","params":["2"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
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
```



## approve_receipt
Returns the approve receipt by hash.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a approve.

### **Returns**
`Object` - A approve receipt object, or *null* when no block state was found.
  * `from`: `DATA`, 20 Bytes - Sender, stand for election.
  * `output`: `DATA` - Receipt output.
  * `status`: `Boolean` - Approve execution status. `true` if successfully, otherwise `false`.

### **Example**
```js
// Request
curl --data '{"method":"approve_receipt","params":["0x01b46344d3e21800e43fbc0da883fbb110b39826e49657150f7c728ae9a6a5d3"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "from": "0xf0821dc4ba9419b865aa412170377ca3b44cdb58",
        "output": "0xd5e023cc427b924072e7694fe0d29d14336cf47c486401655b405e9fcc712cfc",
        "status": "0x1"
    }
}
```



## epoch_work_transaction
Returns epoch finalized transaction hash.

### **Parameters**
1. `QUANTITY` - Epoch number.

### **Returns**
`DATA`, 32 Bytes - Hash of the epoch finalized transaction.

### **Example**
```js
// Request
curl --data '{"method":"epoch_work_transaction","params":["2"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": "0x6588d572085f0ed4c771ab5286b2c53a9ea78f6a799360f9a3abebb34332e5f2"
}
```



## web3_clientVersion
Returns the current client version.

### **Parameters**
None

### **Returns**
`String` - The current client version.

### **Example**
```js
// Request
curl --data '{"method":"web3_clientVersion","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "1.0.10"
}
```



## web3_sha3
Returns Keccak-256 (*not* the standardized SHA3-256) of the given data.

### **Parameters**
1. `DATA` - The data to convert into a SHA3 hash.

### **Returns**
`DATA`, 32Bytes - The Keccak-256 hash of the given string.

### **Example**
```js
// Request
curl --data '{"method":"web3_sha3","params":["0x68656c6c6f20776f726c64"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x47173285a8d7341e5e972fc677286384f802f8ef42a5ec5f03bbfa254cb01fad"
}
```



## eth_blockNumber
Returns the number of most recent block.

### **Parameters**
None

### **Returns**
`QUANTITY` - Integer of the current block number the client is on.

### **Example**
```js
// Request
curl --data '{"method":"eth_blockNumber","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x4b7" // 1207
}
```



## eth_getTransactionCount
Returns the number of transactions *sent* from an address.

### **Parameters**
1. `DATA`, 20 Bytes - Address.
2. `QUANTITY`|`TAG` - (default: `latest`) Integer block number, or the string `'latest'`, `'earliest'` or `'pending'`, see the [default block parameter](#the-default-block-parameter).

### **Returns**
`QUANTITY` - Integer of the number of transactions send from this address.

### **Example**
```js
// Request
curl --data '{"method":"eth_getTransactionCount","params":["0x407d73d8a49eeb85d32cf465507dd71d507100c1","latest"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": "0x4" //4
}
```



## eth_chainId
Returns the chain ID used for signing replay-protected transactions.

### **Parameters**
None

### **Returns**
`QUANTITY` - Hexadecimal value as a string representing the integer of the current chain id.

### **Example**
```js
// Request
curl --data '{"method":"eth_chainId","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x25e4" //9700
}
```



## eth_gasPrice
Returns the current price per gas in wei.

### **Parameters**
None

### **Returns**
`QUANTITY` - Integer of the current gas price in wei.

### **Example**
```js
// Request
curl --data '{"method":"eth_gasPrice","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": "0x989680" //10,000,000 Wei
}
```



## eth_estimateGas
Generates and returns an estimate of how much gas is necessary to allow the transaction to complete. The transaction will not be added to the blockchain. Note that the estimate may be significantly more than the amount of gas actually used by the transaction, for a variety of reasons including EVM mechanics and node performance.

### **Parameters**
See [eth_call](#eth_call) parameters, except that all properties are optional. If no gas limit is specified mcp uses the max gas limit(50000000) as an upper bound. 

### **Returns**
`QUANTITY` - The amount of gas used.

### **Example**
```js
// Request
curl --data '{"method":"eth_estimateGas","params":[{"from": "0x9b1d35635cc34752ca54713bb99d38614f63c955", "data": "0x6060604052341561000f57600080fd5b60eb8061001d6000396000f300606060405260043610603f576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff168063c6888fa1146044575b600080fd5b3415604e57600080fd5b606260048080359060200190919050506078565b6040518082815260200191505060405180910390f35b60007f24abdb5865df5079dcc5ac590ff6f01d5c16edbc5fab4e195d9febd1114503da600783026040518082815260200191505060405180910390a16007820290509190505600a165627a7a7230582040383f19d9f65246752244189b02f56e8d0980ed44e7a56c0b200458caad20bb0029"}], "id": 1, "jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x1c31e"
}
```



## eth_getBlockByNumber
Returns information about a block by block number.

### **Parameters**
1. `QUANTITY`|`TAG` - Integer of a block number, or the string "earliest", "latest" or "pending", as in the [default block parameter](#the-default-block-parameter).
2. `Boolean` - If `true` it returns the full transaction objects, if `false` only the hashes of the transactions.

### **Returns**
See [eth_getBlockByHash](#eth_getBlockByHash).

### **Example**
```js
// Request
curl --data '{"method":"eth_getBlockByNumber","params":["0x1b4",true],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
```



## eth_getBlockTransactionCountByHash
Returns the number of transactions in a block from a block matching the given block hash.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a block.

### **Returns**
`QUANTITY` - Integer of the number of transactions in this block.

### **Example**
```js
// Request
curl --data '{"method":"eth_getBlockTransactionCountByHash","params":["0xb903239f8543d04b5dc1ba6579132b143087c68db1b2168786408fcbce568238"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0xb" // 11
}
```



## eth_getBlockTransactionCountByNumber
Returns the number of transactions in a block from a block matching the given block number.

### **Parameters**
1. `QUANTITY`|`TAG` - Integer of a block number, or the string `'earliest'`, `'latest'` or `'pending'`, as in the [default block parameter](#the-default-block-parameter).

### **Returns**
`QUANTITY` - Integer of the number of transactions in this block.

### **Example**
```js
// Request
curl --data '{"method":"eth_getBlockTransactionCountByNumber","params":["0xe8"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0xa" // 10
}
```



## eth_sendRawTransaction
Creates new message call transaction or a contract creation for signed transactions.

### **Parameters**
1. `DATA` - The signed transaction data.

### **Returns**
`DATA`, 32 Bytes - The transaction hash, or the zero hash if the transaction is not yet available. 

Use [eth_getTransactionReceipt](#eth_getTransactionReceipt) to get the contract address, after the transaction was mined, when you created a contract.

### **Example**
```js
// Request
curl --data '{"method":"eth_sendRawTransaction","params":["0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0xe670ec64341771606e55d6b4ca35a1a6b75ee3d5145a99d05921026d1527331"
}
```



## eth_sendTransaction
Creates new message call transaction or a contract creation, if the data field contains code, and signs it using the account specified in from.

### **Parameters**
1. `Object` - The transaction object.
   * `from`: `DATA`, 20 Bytes - The address the transaction is sent from.
   * `to`: `DATA`, 20 Bytes - (optional when creating new contract) The address the transaction is directed to.
   * `gas`: `QUANTITY` - (optional, default: 21000) Integer of the gas provided for the transaction execution. It will return unused gas.
   * `gasPrice`: `QUANTITY` - (optional, default: To-Be-Determined) Integer of the gas price used for each paid gas.
   * `value`: `QUANTITY` - (optional) Integer of the value sent with this transaction.
   * `data`: `DATA` - The compiled code of a contract OR the hash of the invoked method signature and encoded parameters.
   * `nonce`: `QUANTITY` - (optional) Integer of a nonce. This allows you to overwrite your own pending transactions that use the same nonce.
```js
params: [{
  "from": "0xb60e8dd61c5d32be8058bb8eb970870f07233155",
  "to": "0xd46e8dd67c5d32be8058bb8eb970870f07244567",
  "gas": "0x76c0", // 30400
  "gasPrice": "0x9184e72a000", // 10000000000000
  "value": "0x9184e72a", // 2441406250
  "data": "0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675"
}]
```
### **Returns**
`DATA`, 32 Bytes - The transaction hash, or the zero hash if the transaction is not yet available.

Use [eth_getTransactionReceipt](#eth_getTransactionReceipt) to get the contract address, after the transaction was mined, when you created a contract.

**Example**
```js
// Request
curl --data '{"method":"eth_sendTransaction","params":[{see above}],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0xe670ec64341771606e55d6b4ca35a1a6b75ee3d5145a99d05921026d1527331"
}
```



## eth_call
Executes a new message call immediately without creating a transaction on the block chain. Often used for executing read-only smart contract functions, for example the balanceOf for an ERC-20 contract.

### **Parameters**
1. `Object` - The transaction call object.
   * `from`: `DATA`, 20 Bytes - (optional) The address the transaction is sent from.
   * `to`: `DATA`, 20 Bytes - The address the transaction is directed to.
   * `gas`: `QUANTITY` - (optional) Integer of the gas provided for the transaction execution. eth_call consumes zero gas, but this parameter may be needed by some executions.
   * `gasPrice`: `QUANTITY` - (optional) Integer of the gas price used for each paid gas.
   * `value`: `QUANTITY` - (optional) Integer of the value sent with this transaction.
   * `data`: `DATA` - (optional) Hash of the method signature and encoded parameters. For details see [Contract ABI in the Solidity documentation](https://docs.soliditylang.org/en/latest/abi-spec.html).
2. `QUANTITY`|`TAG` - Integer block number, or the string `'latest'`, `'earliest'` or `'pending'`, see the [default block parameter](#the-default-block-parameter).

### **Returns**
`DATA` - The return value of executed contract.

### **Example**
```js
// Request
curl --data '{"method":"eth_call","params":[{see above}],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x"
}
```



## net_version
Returns the current network version.

### **Parameters**
None

### **Returns**
`String` - The current network protocol version.

The full list of current network IDs is available at chainlist.org(https://chainlist.org/). Some common ones are:
* 0x3ca: mcp Mainnet //970
* 0x25e4: mcp dev //9700

### **Example**
```js
// Request
curl --data '{"method":"net_version","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x3ca" //970
}
```



## net_listening
Returns `true` if client is actively listening for network connections.

### **Parameters**
None

### **Returns**
`Boolean` - `true` when listening, otherwise `false`.

### **Example**
```js
// Request
curl --data '{"method":"net_listening","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```



## net_peerCount
Returns number of peers currently connected to the client.

### **Parameters**
None

### **Returns**
`QUANTITY` - Integer of the number of connected peers.

**Example**
```js
// Request
curl --data '{"method":"net_peerCount","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x2" // 2
}
```



## eth_protocolVersion
Returns the current protocol version.

### **Parameters**
None

### **Returns**
`String` - The current mcp protocol version.

### **Example**
```js
// Request
curl --data '{"method":"eth_protocolVersion","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x63" // 99
}
```



## eth_syncing
Returns an object with data about the sync status or `false`.

### **Parameters**
None

### **Returns**
Return `False` when the node is not syncing, and return the following fields.

`Object`|`Boolean`, An object with sync status data or `False`, when not syncing:
* `startingBlock`: `QUANTITY` - The block at which the import started (will only be reset, after the sync reached this head).
* `currentBlock`: `QUANTITY` - The current block, same as eth_blockNumber.
* `highestBlock`: `QUANTITY` - The estimated highest block.

### **Example**
```js
// Request
curl --data '{"method":"eth_syncing","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": {
    "startingBlock": "0x384", // 900
    "currentBlock": "0x386", // 902
    "highestBlock": "0x454" // 1108
  }
}
// Or when not syncing
{
  "id":1,
  "jsonrpc": "2.0",
  "result": false
}
```



## eth_getLogs
Returns an array of all logs matching a given filter object.

### **Parameters**
1. `Object` - The filter options:
  * `fromBlock`: `QUANTITY`|`TAG` - (optional, default: `'latest'`) Integer block number, or `'latest'` for the last mined block or `'pending'`, `'earliest'` for not yet mined transactions.
  * `toBlock`: `QUANTITY`|`TAG` - (optional, default: `'latest'`) Integer block number, or `'latest'` for the last mined block or `'pending'`, `'earliest'` for not yet mined transactions.
  * `address`: `DATA`|`Array`, 20 Bytes - (optional) Contract address or a list of addresses from which logs should originate.
  * `topics`: `Array` of `DATA` - (optional) Array of 32 Bytes `DATA` topics. Topics are order-dependent. Each topic can also be an array of `DATA` with "or" options.

### **Returns**
`Array` - Array of log objects, or an empty array if nothing. Log objects contain the following keys and their values:
  * `logIndex`: `QUANTITY` - Integer of the log index position in the block. *null* when its pending log.
  * `transactionIndex`: `QUANTITY` - Integer of the transactions index position log was created from. *null* when its pending log.
  * `transactionHash`: `DATA`, 32 Bytes - Hash of the transactions this log was created from. *null* when its pending log.
  * `blockHash`: `DATA`, 32 Bytes - Hash of the block where this log was in. *null* when its pending. *null* when its pending log.
  * `blockNumber`: `QUANTITY` - The block number where this log was in. *null* when its pending. *null* when its pending log.
  * `address`: `DATA`, 20 Bytes - Address from which this log originated.
  * `data`: `DATA` - Contains zero or more 32 Bytes non-indexed arguments of the log.
  * `topics`: `Array` of `DATA` - Array of 0 to 4 32 Bytes DATA of indexed log arguments. (In solidity: The first topic is the hash of the signature of the event (e.g. Deposit(address,bytes32,uint256)), except you declared the event with the anonymous specifier.)

### **Example**
```js
// Request
curl --data '{"method":"eth_getLogs","params":[{"topics":["0x000000000000000000000000a94f5374fce5edbc8e2a8697c15331677e6ebf0b"]}],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": [
    {
      "logIndex": "0x1", // 1
      "blockNumber": "0x1b4", // 436
      "blockHash": "0x8216c5785ac562ff41e2dcfdf5785ac562ff41e2dcfdf829c5a142f1fccd7d",
      "transactionHash": "0xdf829c5a142f1fccd7d8216c5785ac562ff41e2dcfdf5785ac562ff41e2dcf",
      "transactionIndex": "0x0", // 0
      "address": "0x16c5785ac562ff41e2dcfdf829c5a142f1fccd7d",
      "data": "0x0000000000000000000000000000000000000000000000000000000000000000",
      "topics": ["0x59ebeb90bc63057b6515673c3ecf9438e5058bca0f92585014eced636878c9a5"]
    },
    ...
  ]
}
```



## eth_getCode
Returns code at a given address.

### **Parameters**
1. `DATA`, 20 Bytes - Address.

### **Returns**
`DATA` - The code from the given address.

### **Example**
```js
// Request
curl --data '{"method":"eth_getCode","params":["0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x600160008035811a818181146012578301005b601b6001356025565b8060005260206000f25b600060078202905091905056"
}
```



## eth_getStorageAt
Returns the value from a storage position at a given address.

### **Parameters**
1. `DATA`, 20 Bytes - Address of the storage.
2. `QUANTITY` - Integer of the position in the storage.

### **Returns**
`DATA` - The value at this storage position.

### **Example**
```js
// Request
curl --data '{"method":"eth_getStorageAt","params":["0x407d73d8a49eeb85d32cf465507dd71d507100c1","0x0"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x0000000000000000000000000000000000000000000000000000000000000003"
}
```



## eth_getTransactionByHash
Returns the information about a transaction requested by transaction hash.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a transaction.

### **Returns**
`Object` - A transaction object, or *null* when no transaction was found.
  * `hash`: `DATA`, 32 Bytes - Hash of the transaction.
  * `input`: `DATA` - The data send along with the transaction.
  * `to`: `DATA`, 20 Bytes - Address of the receiver. *null* when its a contract creation transaction.
  * `from`: `DATA`, 20 Bytes - Address of the sender.
  * `gas`: `QUANTITY` - Gas provided by the sender.
  * `gasPrice`: `QUANTITY` - Gas price provided by the sender in Wei.
  * `nonce`: `QUANTITY` - The number of transactions made by the sender prior to this one.
  * `value`: `QUANTITY` - Value transferred in Wei.
  * `blockHash`: `DATA`, 32 Bytes - Hash of the block where this transaction was in. *null* when its pending.
  * `transactionIndex`: `QUANTITY` - Integer of the transactions index position in the block. *null* when its pending.
  * `blockNumber`: `QUANTITY` - Block number where this transaction was in. *null* when its pending.
  * `v`: `QUANTITY` - ECDSA recovery id.
  * `r`: `QUANTITY` - ECDSA signature r.
  * `s`: `QUANTITY` - ECDSA signature s.

### **Example**
```js
// Request
curl --data '{"method":"eth_getTransactionByHash","params":["0xbf2d4552fba50efcf467da369b2f36596edfc337cb17ff7c9fe9431548231fe3"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "hash": "0xbf2d4552fba50efcf467da369b2f36596edfc337cb17ff7c9fe9431548231fe3",
        "input": "0xf305d7190000000000000000000000005e1a3ca002d04b3cfedb705320d6cfdf52d7fcf00000000000000000000000000000000000000000000000056bc75e2d63100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001144b522f45265c2dfdbaee8e324719e63a1694cffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
        "to": "0x3a71241d2d880e0e09642db7b5b69277c0e1d455",
        "from": "0x1144b522f45265c2dfdbaee8e324719e63a1694c",
        "gas": "0x1fd7c8",
        "gasPrice": "0x989680",
        "nonce": "0xe",
        "value": "0xde0b6b3a7640000",
        "blockHash": "0xcad8c320cae32037415aab16ace767ee163a0eaedb29f2282e5acd2596f5d55f",
        "transactionIndex": "0x0",
        "blockNumber": "0x1b",
        "r": "0x2fbfb3ded113cbdb579c1bdbe252c3b30a40c5e7c5a644668f87cd889f154eed",
        "s": "0x165d8e90a47f3f1e095ae59f0688f38e89fba27912c75647534da4cc68c22d4c",
        "v": "0x66c"
    }
}
```



## eth_getTransactionByBlockHashAndIndex
Returns information about a transaction by block hash and transaction index position.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a block.
2. `QUANTITY` - Integer of the transaction index position.

### **Returns**
See [eth_getTransactionByHash](#eth_getTransactionByHash).

### **Example**
```js
// Request
curl --data '{"method":"eth_getTransactionByBlockHashAndIndex","params":["0xe670ec64341771606e55d6b4ca35a1a6b75ee3d5145a99d05921026d1527331","0x0"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
```



## eth_getTransactionByBlockNumberAndIndex
Returns information about a transaction by block number and transaction index position.

### **Parameters**
1. `QUANTITY`|`TAG` - A block number, or the string `'earliest'`, `'latest'` or `'pending'`, as in the [default block parameter](#the-default-block-parameter).
2. `QUANTITY` - The transaction index position.

### **Returns**
See [eth_getTransactionByHash](#eth_getTransactionByHash).

### **Example**
```js
// Request
curl --data '{"method":"eth_getTransactionByBlockNumberAndIndex","params":["0x29c","0x1"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
```



## eth_getTransactionReceipt
Returns the receipt of a transaction by transaction hash.

**Note** That the receipt is not available for pending transactions.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a transaction.

### **Returns**
`Object` - A transaction receipt object, or *null* when no receipt was found:
  * `transactionHash`: `DATA`, 32 Bytes - Hash of the transaction.
  * `transactionIndex`: `QUANTITY` - Integer of the transaction’s index position in the block.
  * `blockHash`: `DATA`, 32 Bytes - Hash of the block this transaction was in.
  * `blockNumber`: `QUANTITY` - Block number this transaction was in.
  * `from`: `DATA`, 20 Bytes - Address of the sender.
  * `to`: `DATA`, 20 Bytes - Address of the receiver. `null` when it’s a contract creation transaction.
  * `cumulativeGasUsed`: `QUANTITY` - The total amount of gas used when this transaction was executed in the block.
  * `gasUsed`: `QUANTITY` - The amount of gas used by this specific transaction alone.
  * `contractAddress`: `DATA`, 20 Bytes - The contract address created, if the transaction was a contract creation, otherwise `null`.
  * `logs`: `Array` - Array of log objects, which this transaction generated.
  * `logsBloom`: `DATA`, 256 Bytes - Bloom filter for light clients to quickly retrieve related logs.
  * `status`: `QUANTITY` - either `0x1` (success) or `0x0` (failure).

### **Example**
```js
// Request
curl --data '{"method":"eth_getTransactionReceipt","params":["0x444172bef57ad978655171a8af2cfd89baa02a97fcb773067aef7794d6913374"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": {
    "blockHash": "0x67c0303244ae4beeec329e0c66198e8db8938a94d15a366c7514626528abfc8c",
    "blockNumber": "0x6914b0",
    "contractAddress": "0x471a8bf3fd0dfbe20658a97155388cec674190bf", // or null, if none was created
    "from": "0xc931d93e97ab07fe42d923478ba2465f2",
    "to": null, // value is null because this example transaction is a contract creation
    "cumulativeGasUsed": "0x158e33",
    "gasUsed": "0xba2e6",
    "logs": [], // logs as returned by eth_getFilterLogs, etc.
    "logsBloom": "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    "status": "0x1",
    "transactionHash": "0x444172bef57ad978655171a8af2cfd89baa02a97fcb773067aef7794d6913374",
    "transactionIndex": "0x4"
  }
}
```



## eth_getBalance
Returns the balance of the account of given address.

### **Parameters**
1. `DATA`, 20 Bytes - Address to check for balance.

### **Returns**
`QUANTITY` - Integer of the current balance in wei.

### **Example**
```js
// Request
curl --data '{"method":"eth_getBalance","params":["0x407d73d8a49eeb85d32cf465507dd71d507100c1"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x0234c8a3397aab58" // 158972490234375000
}
```



## eth_getBlockByHash
Returns information about a block by hash.

### **Parameters**
1. `DATA`, 32 Bytes - Hash of a block.
2. `Boolean` - If `true` it returns the full transaction objects, if `false` only the hashes of the transactions.

### **Returns**
`Object` - A block object, or `null` when no block was found.
  * `number`: `QUANTITY` - The block number. `null` when its pending block.
  * `nonce`: `DATA`, 8 Bytes - `null`.
  * `extraData`: `DATA` -  The `'extra data'` field of this block.
  * `hash`: `DATA`, 32 Bytes - Hash of the block. `null` when its pending block.
  * `parentHash`: `DATA`, 32 Bytes - Hash of the latest block of sender's account before the current block. The value is 0 for the first block of the account..
  * `gasLimit`: `QUANTITY` - The maximum gas allowed in this block.
  * `timestamp`: `QUANTITY` -  The unix timestamp for when the block was collated.
  * `transactions`: `Array` - Array of transaction objects, or 32 Bytes transaction hashes depending on the last given parameter.
  * `miner`: `DATA`, 20 Bytes - The address of the beneficiary to whom the mining rewards were given.

### **Example**
```js
// Request
curl --data '{"method":"eth_getBlockByHash","params":["0xcad8c320cae32037415aab16ace767ee163a0eaedb29f2282e5acd2596f5d55f",true],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "number": "0x1b",
        "nonce": null,
        "difficulty": "0x0",
        "extraData": "0x00",
        "hash": "0xcad8c320cae32037415aab16ace767ee163a0eaedb29f2282e5acd2596f5d55f",
        "parentHash": "0x8afcf2f16afdd9b8d67ac646213d064f8ce65180192bb678fb5fb3d168ab82a8",
        "gasUsed": "0x1fd7c8",
        "minGasPrice": "0x989680",
        "gasLimit": "0x7a1200",
        "timestamp": "0x62b9f1af",
        "transactions": [
            "0xbf2d4552fba50efcf467da369b2f36596edfc337cb17ff7c9fe9431548231fe3"
        ]
    }
}
```



## eth_accounts
Returns a list of addresses owned by client.

### **Parameters**
None

### **Returns**
`Array` of `DATA`, 20 Bytes - Addresses owned by the client.

**Example**
```js
// Request
curl --data '{"method":"eth_accounts","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": ["0x407d73d8a49eeb85d32cf465507dd71d507100c1"]
}
```



## eth_sign
The sign method calculates an mcp specific signature with: `sign(keccak256("\x19Ethereum Signed Message:\n" + len(message) + message)))`.

**Note**: the address to sign with must be unlocked.
### **Parameters**
1. `DATA`, 20 Bytes - address.
2. `DATA`, N Bytes - message to sign.

### **Returns**
`DATA` - Signature.

### **Example**
```js
// Request
curl --data '{"method":"eth_sign","params":["0xcd2a3d9f938e13cd947ec05abc7fe734df8dd826","0x5363686f6f6c627573"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0xb1092cb5b23c2aa55e5b5787729c6be812509376de99a52bea2b41e5a5f8601c5641e74d01e4493c17bf1ef8b179c49362b2c721222128d58422a539310c6ecd1b"
}
```



## eth_signTransaction
Signs a transaction that can be submitted to the network at a later time using with [eth_sendRawTransaction](#eth_sendRawTransaction).

### **Parameters**
1. `Object` - The transaction object.
   * `from`: `DATA`, 20 Bytes - The address the transaction is sent from.
   * `to`: `DATA`, 20 Bytes - (optional when creating new contract) The address the transaction is directed to.
   * `gas`: `QUANTITY` - (optional, default: 21000) Integer of the gas provided for the transaction execution. It will return unused gas.
   * `gasPrice`: `QUANTITY` - (optional, default: To-Be-Determined) Integer of the gas price used for each paid gas, in Wei.
   * `value`: `QUANTITY` - (optional) Integer of the value sent with this transaction, in Wei.
   * `data`: `DATA` - The compiled code of a contract OR the hash of the invoked method signature and encoded parameters.
   * `nonce`: `QUANTITY` - (optional) Integer of a nonce. This allows you to overwrite your own pending transactions that use the same nonce.

### **Returns**
`DATA` - The RLP-encoded transaction object signed by the specified account.

### **Example**
```js
// Request
curl --data '{"id": 1,"jsonrpc": "2.0","method": "eth_signTransaction","params": [{"data":"0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675","from": "0x613a091b978848d48dabc74a4c4487389e2e0d8c","gas": "0x76c0","gasPrice": "0x9184e72a000","to": "0x07f332f5a5ade86babdf1f8a43fbca9691ac46ed","value": "0x9184e72a"}]}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": {
    "raw": "0xf894808609184e72a0008276c09407f332f5a5ade86babdf1f8a43fbca9691ac46ed849184e72aa9d46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675824d7ba06b0e804be5e96e65c2456b45af1ffaf30e053340b2499b3f859af4a0b729ce51a073fa85578b05e85cc5cbd0fe58d8636ccf8db2b2370956b78a72528a0c4e6e4a",
    "tx": {
      "hash": "0x44782fac5905d0f81d983043867d99a96ad12cd9c957ec71fe417c684cd4e997",
      "nonce": "0x0", // 0
      "from": "0x613a091b978848d48dabc74a4c4487389e2e0d8c",
      "to": "0x07f332f5a5ade86babdf1f8a43fbca9691ac46ed",
      "value": "0x9184e72a",
      "gas": "0x76c0", 
      "gasPrice": "0x9184e72a000",
      "input": "0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675"
    }
  }
}
```



## personal_importRawKey
Imports the given private key into the key store, encrypting it with the passphrase.

### **Parameters**
1. `DATA`, 32 Bytes - An unencrypted private key.
2. `String` -  The password of the account.

### **Returns**
`DATA`, 20 Bytes - The address of the account.

### **Example**
```js
// Request
curl --data '{"method":"personal_importRawKey","params":["0x7aa3b91561fceae29a8cb1affb3a9d9b3dec7a720685678ae890df0b6e6eed79", "12345678"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": "0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28"
}
```



## personal_listAccounts
Returns a list of addresses owned by client.

### **Parameters**
None

### **Returns**
`Array` of `DATA`, 20 Bytes - Addresses owned by the client.

### **Example**
```js
// Request
curl --data '{"method":"personal_listAccounts","id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": [
    "0x7bf87721a96849d168de02fd6ea5986a3a147383",
    "0xca807a90fd64deed760fb98bf0869b475c469348"
  ]
}
```



## personal_lockAccount
Locks the given account.

### **Parameters**
1. `DATA`, 20 Bytes - The account address.

### **Returns**
`Boolean` - `true` if the account was successfully locked, otherwise `false`.

### **Example**
```js
// Request
curl --data '{"method":"personal_lockAccount","params":["0x1144b522f45265c2dfdbaee8e324719e63a1694c"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```



## personal_newAccount
Creates new account.

**Note:** it becomes the new current locked account.

### **Parameters**
1. `String` - Password for the new account.

### **Returns**
`DATA`, 20 Bytes - The identifier of the new account.

### **Example**
```js
// Request
curl --data '{"method":"personal_newAccount","params":["hunter2"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x8f0227d45853a50eefd48dd4fec25d5b3fd2295e"
}
```



## personal_unlockAccount
Decrypts the key with the given address from the key store. The unencrypted key will be held in memory until the unlock duration expires.

### **Parameters**
1. `DATA`, 20 Bytes - The address of the account to unlock.
2. `String` - The password to unlock the account.

### **Returns**
`Boolean` - `true` if the account was successfully unlocked, otherwise `false`.

```js
// Request
curl --data '{"method":"personal_unlockAccount","params":["0x8f0227d45853a50eefd48dd4fec25d5b3fd2295e","hunter2"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```



## personal_sendTransaction
Sends transaction and signs it in a single call. The account does not need to be unlocked to make this call, and will not be left unlocked after.

### **Parameters**
1. `Object` - The transaction object. see [eth_sendTransaction](#eth_sendTransaction).
2. `String` - Passphrase to unlock the `from` account.

### **Returns**
`DATA`, 32 Bytes - The transaction hash, or the zero hash if the transaction is not yet available.

### **Example**
```js
// Request
curl --data '{"method":"personal_sendTransaction","params":[{"from":"0x407d73d8a49eeb85d32cf465507dd71d507100c1","to":"0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b","value":"0x186a0"},"hunter2"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0x62e05075829655752e146a129a044ad72e95ce33e48ff48118b697e15e7b41e4"
}
```



## personal_sign
Calculates an mcp specific signature with: `sign(keccak256("\x19Ethereum Signed Message:\n" + len(message) + message)))`.

### **Parameters**
1. `DATA`, N Bytes - The data to sign.
2. `DATA`, 20 Bytes - The address of the account to sign with.
3. `String` - Passphrase to unlock the `from` account.

### **Returns**
`DATA` - Signature.

**Example**
```js
// Request
curl --data '{"method":"personal_sign","params":["0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675","0xb60e8dd61c5d32be8058bb8eb970870f07233155","hunter"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0xe7225f986f192f859a9bf84e34b2b7001dfa11aeb5c7164f81a2bee0d79943e2587be1faa11502eba0f803bb0ee071a082b6fe40fba025f3309263a1eef52c711c"
}
```



## personal_ecRecover
Returns the address associated with the private key that was used to calculate the signature in [personal_sign](#personal_sign).

### **Parameters**
1. `DATA` - The data which hash was signed.
2. `DATA` - Signature.

### **Returns**
`DATA`, 20 Bytes - Address of the signer of the message.

### **Example**
```js
// Request
curl --data '{"method":"personal_ecRecover","params":["0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675","0xe7225f986f192f859a9bf84e34b2b7001dfa11aeb5c7164f81a2bee0d79943e2587be1faa11502eba0f803bb0ee071a082b6fe40fba025f3309263a1eef52c711c"],"id":1,"jsonrpc":"2.0"}' -H "Content-Type: application/json" -X POST localhost:8765
// Result
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "0xb60e8dd61c5d32be8058bb8eb970870f07233155"
}
```
