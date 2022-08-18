# Test Pancakeswap
### Installation
yarn install
### Config
Rename .env.example to .env and open it, then fill the ccnbeta's url and account's private key.<br>
### Init
After running following command, you can get "INIT_CODE_PAIR_HASH" value.<br>
<code>yarn run init</code><br>
You should replace the hex '...' at line 24 in contracts/periphery/libraries/PancakeLibrary.sol with this value.
https://github.com/computecoin-network/mcp/blob/a645bd205ed5696aae6a40e8da01356e0c42d197/test/contracts/mcp-pancakeswap-test/contracts/periphery/libraries/PancakeLibrary.sol#L24
### Compile
npx hardhat compile
### Test
yarn run test
