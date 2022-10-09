In this repository, you will find the latest smart contracts enabling Open protocol.

------


### Deployment Guide

After the repository is cloned

```
$ npm install
```
Set environment variables by updating `MNEMONIC` in `.env` file which will be used by hardhat for deployment

Now that all the dependencies are installed, you're ready to go!

```
$ npx hardhat run --network ccnbeta scripts/deploy_all.js
```
Congrats! you have successfully deployed all the contracts on CCN Beta net.

You can test the contracts on the beta network as following. 

Before test, you have to deploy the PancakeRouter and update the router's address in .env file and contract file(.\contracts\libraries\LibiCommon.sol line 7).
```
$ npx hardhat run --network ccnbeta test/main.js
```
