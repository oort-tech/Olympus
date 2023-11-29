![Horizontal Logo (TM)](https://github.com/oort-tech/Huygens_smartcontract_101/assets/90715387/675970c2-5ecd-4132-a9f4-66460d5f2e03)

# Uniswap V2 on Oort

Uniswap V2 on CCN

# Local Development

The following assumes the use of `node@>=10`.

## Install Dependencies

`yarn`

## Config
Copy .env.example to .env and open it, then fill the ccnbeta's url and accounts private key.<br>

## Run Tests

You can test the contracts on the beta network as following. 

`npx hardhat run --network ccnbeta scripts/ccn/main.js`
