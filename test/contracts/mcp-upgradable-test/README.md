# Upgradable Smart Contract featuring Bridge Proxy on CCNBETA

### Requirements
- Node =< 18 (18 works good)
- yarn

Go to the source directory and open terminal, please run this command.<br>
> yarn

### Compile
> npx hardhat compile (unless you want to manually compiled as yarn run does this under the hood)

### Config
Copy .env.example to .env and open it, then fill the ccnbetas url and accounts private key.<br> (All fields must be filled with different private keys)

### Test

### Run all test at once

> yarn run test-all

### Run test Individually

Test [main](https://github.com/oort-tech/Olympus/blob/master/test/contracts/mcp-upgradable-test/scripts/1_main.js)

> yarn run test-main

Test [bridge](https://github.com/oort-tech/Olympus/blob/master/test/contracts/mcp-upgradable-test/scripts/2_bridge.js)

> yarn run test-bridge

Test [DWDN](https://github.com/oort-tech/Olympus/blob/master/test/contracts/mcp-upgradable-test/scripts/3_dwdn.js)

> yarn run test-dwdn

Test [Test Script](https://github.com/oort-tech/Olympus/blob/master/test/contracts/mcp-upgradable-test/scripts/4_test.js)

yarn run test-test