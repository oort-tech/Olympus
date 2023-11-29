![Horizontal Logo (TM)](https://github.com/oort-tech/Olympus/assets/41552663/bc195389-b1ec-4d96-9f23-bb90415b1e36)

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
