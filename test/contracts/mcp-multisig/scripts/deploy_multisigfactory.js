const { ethers, upgrades } = require("hardhat");
//const BigNumber = require('bignumber.js');

async function MultisigDeploy() {
    const multiSigDailyLimitFactory = await ethers.getContractFactory("MultiSigWalletWithDailyLimitFactory");

    const multiSig = await multiSigDailyLimitFactory.deploy();

    console.log('Factory Daily Limit deployed at:', multiSig.address);
    console.log('Transaction hash:', multiSig.deployTransaction.hash);
}

MultisigDeploy()
    .then
    (function () {
        process.exit(0);
    })