# RPC Interfaces

## account_import
Import an account into the node, the private key will be managed by the node, unlock the account with personal_unlockAccount to send online transactions and the node will sign the transaction with managed private key.
### Parameters
- **action** - String: "account_import".
- **json** - String: Keystore file of the account to be imported.
#### POST Request Body
```json
{
    "action": "account_import",
    "json": "{\"account\":\"<account>\",\"kdf_salt\":\"<kdf_salt>\",\"iv\":\"<iv>\",\"ciphertext\":\"<ciphertext>\"}"
}
```
#### web3-olympus.js
```javascript
olympusRequest.accountImport(keystore)
```
### Returns
- **code** - Integer: Error code. *0*: success, *9*: invalid json.
- **msg** - String: Error message.
- **account** - String: Imported account.
#### Example
```javascript
olympusRequest.accountImport("{\"account\":\"0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28\",\"kdf_salt\":\"175DCAF994E6992AAD1369014670C086\",\"iv\":\"F6054D9B144A254D3D4EAB78C95F21B6\",\"ciphertext\":\"2A943F3A7316C33B16374D9076FEF5BA7770C2A0424A08501D3663A1467DEDD7\"}")

// Success
> {
    "code": 0,
    "msg": "OK",
    "account": "0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28"
}

// Failed
> {
    "code": 9,
    "msg": "Invalid json"
}
```

## account_export
Export the keystore file of an imported account.
### Parameters
- **action** - String: "account_export".
- **account** - String: Account to be exported.
#### POST Request Body
```json
{
    "action": "account_export",
    "account": <account>
}
```
#### web3-olympus.js
```javascript
olympusRequest.accountExport(account)
```
### Returns
- **code** - Integer: Error code. *0*: success, *1*: invalid account, *7*: account not found.
- **msg** - String: Error message.
- **json** - String: Keystore file of the exported account.
#### Example
```javascript
olympusRequest.accountExport("0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28")

// Success
> {
    "code": 0,
    "msg": "OK",
    "json": "{\"account\":\"0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28\",\"kdf_salt\":\"175dcaf994e6992aad1369014670c086\",\"iv\":\"f6054d9b144a254d3d4eab78c95f21b6\",\"ciphertext\":\"2a943f3a7316c33b16374d9076fef5ba7770c2a0424a08501d3663a1467dedd7\"}"
}

// Failed
> {
    "code": 1,
    "msg": "Invalid account"
}
// or
{
    "code": 7,
    "msg": "Account not found"
}
```

## account_remove
Remove the keystore file of an account previously imported through account_import or personal_importRawKey, the node will no longer manager this account.
### Parameters
- **action** - String: "account_remove".
- account - String: Account to be removed.
- password - String: Account password.

#### POST Request Body
```json
{
    "action": "account_remove",
    "account": <account>,
    "password": <password>
}
```
#### web3-olympus.js
```javascript
olympusRequest.accountRemove(account, password)
```
### Returns
- **code** - Integer: Error code. *0*: success, *1*: invalid account, *5*: invalid password, *7*: account not found, *8*: wrong password, *46*: empty password.
- **msg** - String: Error message.
#### Example
```javascript
olympusRequest.accountRemove("0xa9d8863d0bf68dbaaacacad4ee0e865a0cc59f28", "s4iH1t@hBFtymA")

// Success
> {
    "code": 0,
    "msg": "OK"
}

// Failed
> {
    "code": 1,
    "msg": "Invalid account"
}
// or
{
    "code": 5,
    "msg": "Invalid password"
}
// or
{
    "code": 7,
    "msg": "Account not found"
}
// or
{
    "code": 8,
    "msg": "Wrong password"
}
// or
{
    "code": 46,
    "msg": "Password can not be empty"
}
```

## block_state
Return the state of a block with its block hash.
### Parameters
- **action** - String: "block_state".
- **hash** - String: Block hash.
#### POST Request Body
```json
{
    "action": "block_state",
    "hash": <blockHash>
}
```
#### web3-olympus.js
```javascript
olympusRequest.getBlockState(blockHash)
```
### Returns
- **code** - Integer: Error code. *0*: success, *36*: invalid hash format.
- **msg** - String: Error message.
- **block state** - Object: The state of the block. *null* if the block does not exist.
    - **content** - Object
      - **level** - Number: The level of the block.
      - **witnessed_level** - Number: The witness level of the block.
      - **best_parent** - String: The best parent block hash of the block.
    - **is_stable** - Number: Is the block stable or not. *0*: not stable, *1*: stable.
    - **stable_content** - Object: Stable block state contents. *empty* if the block is not stable.
      - **status** - Number: Block status. *0*: success, *1*: double spending, *2*: invalid, *3*: contract execution failed. 
      - **stable_index** - Number: Stable block index which indicates the order of blocks on DAG. The value starts from genesis block as 0, and keeps increasing.
      - **stable_timestamp** - Number: Stable timestamp.
      - **mci** - Number: Main chain index.
      - **mc_timestamp** - Number: Main chain timestamp.
      - **is_on_mc** - Number: Is the block on main chain. *0*: not on main chain, *1*: on main chain.
      - **is_free** - Number: Does the block have children. *0*: no children, *1*: has children.
#### Example
```javascript
olympusRequest.getBlockState("0xdbd8c3d4264e92e59b3b9d5f500a258427466e23ad7904b0c01e2e5b0a81c174")

// Success
> {
    "code": 0,
    "msg": 'OK',
    "block_state": {
        "content": {
            "level": 76497,
            "witnessed_level": 76488,
            "best_parent":
                '0x45ff3d0989897cf4158516d7b42608f028e575c55b086bab0fb668ee2ac7d41a'
        },
        "is_stable": 1,
        "stable_content": {
            "status": 0,
            "stable_index": 105834,
            "stable_timestamp": 1677708703,
            "mci": 76497,
            "mc_timestamp": 1677708700,
            "is_on_mc": 1,
            "is_free": 0
        }
    }
}

// Failed
> {
    "code": 36,
    "msg": "Invalid hash format"
}
```

## stable_blocks
Return the stable blocks by giving the index of the first block to retrieve and the uplimit of the number of blocks to return.
### Parameters
- **action** - String: "stable_blocks".
- **limit** - String: Uplimit of the number of blocks to return. The maximum value is 100.
- **index** - String: The index of the first block to retrieve. It can be the value of next_index from the result of previous stable_blocks call. The default value is 0.
#### POST Request Body
```json
{
    "action": "stable_blocks",
    "limit": <limit>,
    "index": <index>
}
```
#### web3-olympus.js
```javascript
olympusRequest.stableBlocks(limit, index)
```
### Returns
- **code** - Integer: Error code. *0*: success, *12*: invalid limit, *14*: invalid index.
- **msg** - String: Error message.
- **blocks** - Array: List of returned stable blocks. Object on each index is a stable block object.
  - block object structure:
    - **hash** - String: Block hash.
    - **from** - String: Sender's account.
    - **previous** - String: Hash of the latest block of sender's account before the current block. *0* if it is the first block of the account.
    - **parents** - Array: List of parent blocks' hashes on the DAG.
    - **links** - Array: List of the ordinary blocks' hashes that the witness block referenced.
    - **last_summary** - String: The summary of last_summary_block.
    - **last_summary_block** - String: The last_stable_block of the best parent of this block on the DAG.
    - **last_stable_block** - String: The last stable block of this block on the DAG.
    - **timestamp** - Number: The timestamp when the block was generated.
    - **gasLimit** - String: Uplimit of the gas for the transaction.
    - **signature** - String: Sender's signature.
- **next_index** - Number: The index of next stable block. *null* if there is no subsequent block.
#### Example
```javascript
olympusRequest.stableBlocks("1", "10")

// Success
> {
    "code": 0,
    "msg": "OK",
    "blocks": [
        {
            "hash": "0xdab3afc89643b95d680c0156b6cfcf0f170c518ba4613ee68bff64911bcd4d5f",
            "from": "0x111a6899a9d63d4295e6de66f791acdaca6d07c6",
            "previous": "0x0000000000000000000000000000000000000000000000000000000000000000",
            "parents": [
                "0x454e45b9406a5f2a36e98c2f376d93924dd9d5ee3360eb2c8857cb0c1ec36437",
                "0x803318db753dc8ca697cb0f3764eb8fe696f741ab3bec47dec25ba3c1b7748bb"
            ],
            "links": [],
            "last_summary": "0x13927bc1a6208d43b5b70a7c0bdf488aabefaa86665619685018f3c0bc49c6c8",
            "last_summary_block": "0x5b4d77f0affac85e884f233d4ebd7720016fe04cc14855afd7fd1d13573f57ba",
            "last_stable_block": "0x5b4d77f0affac85e884f233d4ebd7720016fe04cc14855afd7fd1d13573f57ba",
            "timestamp": 1676447263,
            "gasLimit": "0x2faf080",
            "signature": "0x67582c4a43362d87d7333d92068d59b65ff8d02583267bfec1e8316ca4f5d993446c93709787094f2fc4087fc7de2a14f1fff5d59186a7d354fa5be62a5bfde801"
        }
    ],
    "next_index": 11
}

// Failed
> {
    "code": 12,
    "msg": "Invalid limit"
}
// or
{
    "code": 14,
    "msg": "Invalid index"
}
```

## block_summary
Return the summary of a stable block.
### Parameters
- **action** - String: "block_summary".
- **hash** - String: Block hash.
#### POST Request Body
```json
{
    "action": "block_summary",
    "hash": <blockHash>
}
```
#### web3-olympus.js
```javascript
olympusRequest.blockSummary(block_hash)
```
### Returns
- **code** - Integer: Error code. *0*: success, *36*: invalid hash format.
- **msg** - String: Error message.
- **summary** - String: Summary hash.
- **previous_summary** - String: Previous summary hash.
- **parent_summaries** - Array: List of parents' summary hashes.
- **skiplist_summaries** - Array: List of skipped summary hashes.
- **status** - Number: The status of the block state.
#### Example
```javascript
olympusRequest.blockSummary("0xdbd8c3d4264e92e59b3b9d5f500a258427466e23ad7904b0c01e2e5b0a81c174")

// Success
> {
    "code": 0,
    "msg": "OK",
    "summary": "0x5be821f80c34185279acfa219489cb41008415bea7a8ffd28f6f556c06cb7bd8",
    "previous_summary": "0x73eee53e998ae0e8f0b6e645ca5520c82cc0a551b31897746de8b46c7dff7d1c",
    "parent_summaries": [
        "0x9b8499d8ac9065ba91b778c0036d1ac7345d25d82920ec66b90a32823da762af",
        "0x73eee53e998ae0e8f0b6e645ca5520c82cc0a551b31897746de8b46c7dff7d1c"
    ],
    "skiplist_summaries": [],
    "status": 0
}

// Failed
> {
    "code": 36,
    "msg": "Invalid hash format"
}
```

## version
Acquire the current node version, rpc interface version, and database version.
### Parameters
None
#### POST Request Body
```json
{
    "action": "version"
}
```
#### web3-olympus.js
```javascript
olympusRequest.version()
```
### Returns
- **code** - Integer: Error code. *0*: success.
- **msg** - String: Error message.
- **version** - String: Current node version.
- **rpc_version** - String: RPC interface version.
- **store_version** - String: Database version.
#### Example
```javascript
olympusRequest.version()

// Success
> {
    "code": 0,
    "msg": "OK",
    "version": "1.0.10",
    "rpc_version": "1",
    "store_version": "1"
}
```

## status
Retrieve the current status of DAG on the node.
### Parameters
None
#### POST Request Body
```json
{
    "action": "status"
}
```
#### web3-olympus.js
```javascript
olympusRequest.status()
```
### Returns
- **code** - Integer: Error code. *0*: success.
- **msg** - String: Error message.
- **syncing** - Number: If the node is syncing to the other nodes. *0* if not syncing, *1* if syncing.
- **last_stable_mci** - Number: The mci of the last stable block.
- **last_mci** - Number: The mci of the last block on the main chain.
- **last_stable_block_index** - Number: The stable index of the last stable block. Stable index starts from value 0 and keep increasing. It indicates the order of stable blocks on DAG.
- **epoch** - Number: The current epoch number of mcp.
- **epoch_period** - Number: The number of blocks in main chain included in each epoch.
#### Example
```javascript
olympusRequest.status()

// Success
> {
    "code": 0,
    "msg": "OK",
    "syncing": 0,
    "last_stable_mci": 366451,
    "last_mci": 366469,
    "last_stable_block_index": 535333,
    "epoch": 3664,
    "epoch_period": 100
}
```

## peers
Retrieve the peers connected to the node.
### Parameters
None
#### POST Request Body
```json
{
    "action": "peers"
}
```
#### web3-olympus.js
```javascript
olympusRequest.peers()
```
### Returns
- **code** - Integer: Error code. *0*: success.
- **msg** - String: Error message.
- **peers** - Array: List of peers returned.
#### Example
```javascript
olympusRequest.peers()

// Success
> {
    "code": 0,
    "msg": "OK",
    "peers": [
        {
            "id": "0x25931a5f55212a19ac22b5fc3cb1dfda6025828854fa90c14fffeef8027127a8c22a5aae61909d73b6de2167253596853a123e6e2b0050193e45a08be6cc8129",
            "endpoint": "172.104.91.244:30607"
        },
        {
            "id": "0x8185ce9ca658354142847666ab45a1991fbe86a3fbbdfdab1acfa173a0041600c56d3419f34e88fc1bc888ce913589953f186cc5412c688f45a2de1074db4b4b",
            "endpoint": "43.154.130.109:30606"
        },
        ...
    ]
}
```

## nodes
Retrieve the nodes that are candidates to connect to the node.
### Parameters
None
#### POST Request Body
```json
{
    "action": "nodes"
}
```
#### web3-olympus.js
```javascript
olympusRequest.nodes()
```
### Returns
- **code** - Integer: Error code. *0*: success.
- **msg** - String: Error message.
- **nodes** - Array: List of nodes returned.
#### Example
```javascript
olympusRequest.nodes()

// Success
> {
    "code": 0,
    "msg": "OK",
    "nodes": [
        {
            "id": "0x25931a5f55212a19ac22b5fc3cb1dfda6025828854fa90c14fffeef8027127a8c22a5aae61909d73b6de2167253596853a123e6e2b0050193e45a08be6cc8129",
            "endpoint": "172.104.91.244:30607"
        },
        {
            "id": "0x8a7becc16d52a1d61079cb9c9cdeb13952f5e56d488fcd836b7fa87eaecda542b4abce8b5fe20c7befe78cbe13a83e056242f92b65259d522e9b326c0314face",
            "endpoint": "109.74.206.50:30607"
        },
        ...
    ]
}
```

## witness_list
Retrieve the list of witnesses.
### Parameters
- **action** - String: "witness_list".
- **epoch** (Optional) - String: Epoch number.
#### POST Request Body
```json
{
    "action": "witness_list",
    "epoch": "100" // Optional
}
```
#### web3-olympus.js
```javascript
olympusRequest.witnessList(epoch: Optional)
```
### Returns
- **code** - Integer: Error code. *0*: success, *50*: epoch is too big.
- **msg** - String: Error message.
- **witness_list** - Array: List of witnesses.
#### Example
```javascript
olympusRequest.witnessList("100")

// Success
> {
    "code": 0,
    "msg": "OK",
    "witness_list": [
        "0x111a6899a9d63d4295e6de66f791acdaca6d07c6",
        "0x234a808020b60abd2e85b68a57b19bc6aa7ac217",
        "0x27821d50355795d2ce792553201a36afc232c4c1",
        "0x2e2cb4884db9f2976a6b23e0544ea4d2d6f13c45",
        "0x422ceefcce450aa293f81777c3fa4972349778ab",
        "0x442f16643aeb9d466add91a464d9aa6acd63625d",
        "0x49eb9d07b82dbdc6efd3ca14b71336a6a56d2962",
        "0x712f0e7ef7e055923611721d38d3ed05a5fc878c",
        "0x9337d003c960c673f42116893f69c248ec4c655d",
        "0xa6b11d16bd51d996d921dce6c8c350cbb1723c86",
        "0xb1d9b0199bac38d32b2d539d9911941a14e56f60",
        "0xb82d856e065ae9b63115eb4024a71bd6df81ba52",
        "0xc1c64d93759b35effb645cf700983c7d1b9edcca",
        "0xdf127194cf3c7e314ed25952169d1c56fcbb2d46"
    ]
}

// Failed
> { 
    "code": 50, 
    "msg": "epoch is too big."
}
```