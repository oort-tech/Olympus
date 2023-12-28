const { ethers, upgrades } = require("hardhat");
//const BigNumber = require('bignumber.js');

async function MultisigDeploy() {
    const MultiSigContract = await ethers.getContractFactory("MultiSigWalletWithDailyLimit");
    const walletOwner = ["0xD2a8fd999bF4CC30A3fD86f017DeD1c6BF67E78B"];
    const requiredConfirmations = 2;

    const multiSig = await MultiSigContract.deploy(walletOwner, 1, 3000);

    console.log('MultiSig Wallet deployed at:', multiSig.address);
    console.log('Transaction hash:', multiSig.deployTransaction.hash);
}

MultisigDeploy()
    .then
    (function () {
        process.exit(0);
    })