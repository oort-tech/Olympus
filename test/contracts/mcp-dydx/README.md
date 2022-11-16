# MCP-DyDx

### Clone the repo

#### Make sure you select the correct branch
![branch](https://user-images.githubusercontent.com/41552663/201589355-ba4c1dec-2d79-40fe-bff5-45758dbb8cf9.png)

```npm install && npm run build```

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

### Trivial test-case original done by DyDx it includes addresses not yet available on MCP

[Logs.test.ts](https://github.com/molecula451/Olympus/blob/mcp-dydx/test/contracts/mcp-dydx/__tests__/Logs.test.ts)

![docker_mGmc4t4ltw](https://user-images.githubusercontent.com/41552663/201595412-9a0ada53-3add-4dd8-a09c-d0f0eda403e4.gif)

## Known Bugs when testing due to original repo

#### You will have to make sure you actually deploy the contract artifact when testing else you will get a JavaScript superficial error saying it can't find any contract address for this instance, this is a bug that resides under the original DyDx repo

![known bugs](https://user-images.githubusercontent.com/41552663/201593211-9a061a3e-7bbe-4e3a-9122-a74872c50935.png)

```
//Testing Protocol


Starting migrations...
======================
> Network name:    'huygens'
> Network id:      971
> Block gas limit: 8000000 (0x7a1200)


1_initial.js
============

   Replacing 'Migrations'
   ----------------------
   > transaction hash:    0x20193480247299bde3fb34e4dfbee50b6bf2e0edcae7aa45d5a8543b06bc71fa
   > Blocks: 28           Seconds: 13
   > contract address:    0xb7B783A08375d1d89BA75875Fb37f85f28cC6758
   > block number:        2208375
   > block timestamp:     1668559094
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.9986922644
   > gas used:            324476 (0x4f37c)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000324476 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000324476 ETH


2_deploy copy.js
================

   Replacing 'TokenA'
   ------------------
   > transaction hash:    0x662739a9a1b5130a659fb34c10924da12ecda64c0965c1457e3e541dc14f619a
   > Blocks: 25           Seconds: 9
   > contract address:    0xe4F41ae0C18C738fce88ac051AfEC15139f83fd9
   > block number:        2208429
   > block timestamp:     1668559129
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99868099307
   > gas used:            1084384 (0x108be0)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00001084384 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00001084384 ETH


3_deploy.js
===========

   Replacing 'TokenB'
   ------------------
   > transaction hash:    0x236af87724850188bdb2fdb1406537de91d904733fa4cbd4537dca22b6c80d7b
   > Blocks: 34           Seconds: 9
   > contract address:    0xcB49B3Aa23026381DD932Cf908eBC1b89dAf8110
   > block number:        2208489
   > block timestamp:     1668559164
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99866987174
   > gas used:            1084384 (0x108be0)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00001084384 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00001084384 ETH


4_deploy.js
===========

   Replacing 'TokenC'
   ------------------
   > transaction hash:    0xfc78c19f16beb1d372879128a4baa38060f688867d97b18d9e52cd12ab25b489
   > Blocks: 29           Seconds: 10
   > contract address:    0xDE70d1D126eF3e05b9ABE84B7A468360b932914b
   > block number:        2208544
   > block timestamp:     1668559203
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99865875041
   > gas used:            1084384 (0x108be0)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00001084384 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00001084384 ETH


5_deploy.js
===========

   Replacing 'ErroringToken'
   -------------------------
   > transaction hash:    0xb751015e768243623165b1abf03b0e63f2f58846b99fe3734036c70b95310b67
   > Blocks: 27           Seconds: 9
   > contract address:    0xf5276d6E369392f64b85047388cF5D1935bf3BEc
   > block number:        2208599
   > block timestamp:     1668559242
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99865220936
   > gas used:            626356 (0x98eb4)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000626356 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000626356 ETH


6_deploy.js
===========

   Deploying 'OmiseToken'
   ----------------------
   > transaction hash:    0xdab05b51645874902abeb649cca9c145ae6d5095a0707b5916e57eb3eccc425c
   > Blocks: 27           Seconds: 13
   > contract address:    0x9aE552C62c73dDD264Da5F5c32252470b45BBE87
   > block number:        2208658
   > block timestamp:     1668559282
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99864167681
   > gas used:            1025506 (0xfa5e2)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00001025506 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00001025506 ETH


7_deploy.js
===========

   Deploying 'TestLib'
   -------------------
   > transaction hash:    0xec0eca251293fdfc7bd86351a707d114573daeca45738cd41df9f4c64ddd155c
   > Blocks: 28           Seconds: 14
   > contract address:    0x5dc70AbaBbbBd2C1b048DB392768fbbed6A97d68
   > block number:        2208714
   > block timestamp:     1668559320
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99860557609
   > gas used:            3582323 (0x36a973)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00003582323 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00003582323 ETH


8_deploy.js
===========

   Deploying 'TestAutoTrader'
   --------------------------
   > transaction hash:    0x39f25f2f31c113303f6610740d67bd0ebff7f7d0036a5798341b62d3ed8a8ba8
   > Blocks: 29           Seconds: 13
   > contract address:    0xCdCb6020216713cB6018dDa9a21d6c3A73Af3e84
   > block number:        2208768
   > block timestamp:     1668559359
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99858615509
   > gas used:            1914351 (0x1d35ef)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00001914351 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00001914351 ETH


9_deploy.js
===========

   Deploying 'TestExchangeWrapper'
   -------------------------------
   > transaction hash:    0x5ada5faf4af2438c083d58bcb6418e1483faa4c7c49f935228acde48a2b810c8
   > Blocks: 28           Seconds: 13
   > contract address:    0x60Ab5Aa82f19FF4B997f825491B6AfA0B64C63a1
   > block number:        2208825
   > block timestamp:     1668559401
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99857780529
   > gas used:            807231 (0xc513f)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000807231 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000807231 ETH


10_deploy.js
============

   Deploying 'TestMakerOracle'
   ---------------------------
   > transaction hash:    0x7c2892aed271e5dd4065b44a63bc0d188bee5399964d109224c0bf65709d665b
   > Blocks: 32           Seconds: 13
   > contract address:    0x056f861E39Cb4dCA025987105c0e8d85576888D8
   > block number:        2208882
   > block timestamp:     1668559445
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99857499636
   > gas used:            253144 (0x3dcd8)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000253144 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000253144 ETH


11_deploy.js
============

   Replacing 'TestMakerOracle'
   ---------------------------
   > transaction hash:    0x55951ad38ee8e7d6d2dc0784af3b0b24326a0b93c8017c6576fc5713546c08a3
   > Blocks: 31           Seconds: 13
   > contract address:    0x17580277d2c603A8AE9DC2B1A2a50eD5c4450b5f
   > block number:        2208944
   > block timestamp:     1668559483
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99857218743
   > gas used:            253144 (0x3dcd8)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000253144 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000253144 ETH


12_deploy.js
============

   Deploying 'TestCurve'
   ---------------------
   > transaction hash:    0x166c7674c4a3374beb6aab3bee317f4480352646611f0d072a3b2a7f0d297700
   > Blocks: 29           Seconds: 13
   > contract address:    0xf82a246A9Db7609C1a45b09371B4955d09CCC89D
   > block number:        2209001
   > block timestamp:     1668559525
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99856936971
   > gas used:            254023 (0x3e047)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000254023 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000254023 ETH


13_deploy.js
============

   Deploying 'TestUniswapV2Pair'
   -----------------------------
   > transaction hash:    0x24317799d038e15c1c1eea716bde92b51b684cd97b63780d007b06ad6a508503
   > Blocks: 30           Seconds: 13
   > contract address:    0xd3005A1d6eF1a5DEb46084b523eC8e0D050065e0
   > block number:        2209063
   > block timestamp:     1668559568
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99856640744
   > gas used:            268478 (0x418be)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000268478 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000268478 ETH


14_deploy.js
============

   Deploying 'TestUniswapV2Pair2'
   ------------------------------
   > transaction hash:    0xcaa9ed9c71edde9925eef0dc48ac8c3aa423c831155dd71ca0435e9b33d91eea
   > Blocks: 29           Seconds: 13
   > contract address:    0xC0092B1Dd2b7249D1b58C3B50615f54E26421D04
   > block number:        2209124
   > block timestamp:     1668559612
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99856344517
   > gas used:            268478 (0x418be)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00000268478 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00000268478 ETH

Summary
=======
> Total deployments:   14
> Final cost:          0.00012830662 ETH


15_deploy.js
===========

   Deploying 'WETH9'
   -----------------
   > transaction hash:    0x19ca6b0a37c618433a06b6263be349b53b3bfa7ac714603d2aec305109575180
   > Blocks: 30           Seconds: 13
   > contract address:    0x14B32058f368303Ee7307e9aaC85Fba6028Ba86b
   > block number:        2209182
   > block timestamp:     1668562534
   > account:             0xE884FA0EB45955889fa3A5700d6CB49b1A428F72
   > balance:             38.99855309662
   > gas used:            1007106 (0xf5e02)
   > gas price:           0.01 gwei
   > value sent:          0 ETH
   > total cost:          0.00001007106 ETH

   > Saving migration to chain.
   > Saving artifacts
   -------------------------------------
   > Total cost:       0.00001007106 ETH

Summary
=======
> Total deployments:   1
> Final cost:          0.00001007106 ETH
```

[TokenA]()

Address deployed at: 0xe4F41ae0C18C738fce88ac051AfEC15139f83fd9

####RawCall back

![tokenA](https://user-images.githubusercontent.com/41552663/202072705-a7636522-ac0d-41a2-a30e-4b7974786ff3.gif)


Original Apache License

    Copyright 2019 dYdX Trading Inc.

    Licensed under the Apache License, Version 2.0 (the "License";
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


