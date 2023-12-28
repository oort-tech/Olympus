const { ethers, upgrades } = require("hardhat");
//const BigNumber = require('bignumber.js');

async function FactoryDeploy() {
    const Factory = await ethers.getContractFactory("Factory");
    const factory = await Factory.deploy();

    console.log('Factory deployed at:', factory.address);
    console.log('Transaction hash:', factory.deployTransaction.hash);
}

FactoryDeploy()
    .then
    (function () {
        process.exit(0);
    })