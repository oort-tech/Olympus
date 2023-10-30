# Pancakeswap on CNNBETA

### Installation
yarn install

### Config
Copy .env.example to .env and open it, then fill the ccnbeta's url and account's private key.<br>

### Init
After running following command, you can get "INIT_CODE_PAIR_HASH" value.<br>

<code>yarn run init</code><br>
You should replace the hex '...' at line 24 in contracts/periphery/libraries/PancakeLibrary.sol with this value.
https://github.com/oort-tech/Olympus/blob/c56a4165af73d082028a3b13a100b00102e95ee2/test/contracts/mcp-pancakeswap-test/contracts/periphery/libraries/PancakeLibrary.sol#L24

### Test
yarn run test
