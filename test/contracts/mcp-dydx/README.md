# MCP-DyDx

### Clone the repo
#### Make sure you select the correct branch
![branch](https://user-images.githubusercontent.com/41552663/201589355-ba4c1dec-2d79-40fe-bff5-45758dbb8cf9.png)

```npm install```

![npm run build](https://user-images.githubusercontent.com/41552663/201587723-2891d83f-17a9-4a3b-a488-cdc21ca6651d.gif)

### Deploy to local testnet

```npx truffle migrate --network dev```

### Run some tests

[EVM.test.ts](https://github.com/molecula451/Olympus/blob/mcp-dydx/test/contracts/mcp-dydx/__tests__/EVM.test.ts)

![evm_test](https://user-images.githubusercontent.com/41552663/201588233-70938bef-9fc1-42f5-83eb-6f78f06929d1.gif)

[Solo.test.ts](https://github.com/molecula451/Olympus/blob/mcp-dydx/test/contracts/mcp-dydx/__tests__/Solo.test.ts)

![docker_ounCzRQxsY](https://user-images.githubusercontent.com/41552663/201589157-06808888-ea21-4631-b84c-c5160a8ea3b6.gif)

[PartiallyDelayedMultiSig.test.ts](https://github.com/molecula451/Olympus/blob/mcp-dydx/test/contracts/mcp-dydx/__tests__/multisig/PartiallyDelayedMultisig.test.ts)

![docker_LInoNgx5Fm](https://user-images.githubusercontent.com/41552663/201590404-b9fb10d8-3733-400a-af0b-63b132a3a889.gif)

## Known Bugs when testing due to original repo

#### You will have to make sure you actually deploy the contract artifact when testing else you will get a JavaScript superficial error saying it can't find any contract address for this instance, this is a bug that resides under the original DyDx repo

![known bugs](https://user-images.githubusercontent.com/41552663/201593211-9a061a3e-7bbe-4e3a-9122-a74872c50935.png)


