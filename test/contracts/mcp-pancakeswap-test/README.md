# Test Pancakeswap
### Installation
yarn install
### Config
Rename .env.example to .env and open it, then fill the ccnbeta's url and account's private key.<br>
### Init
After running following command, you can get "INIT_CODE_PAIR_HASH" value.<br>
<code>yarn run init</code><br>
You should replace the hex '...' at line 24 in contracts/periphery/libraries/PancakeLibrary.sol with this value.
### Compile
npx hardhat compile
### Test
yarn run test
