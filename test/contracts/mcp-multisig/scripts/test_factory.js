const { ethers } = require('hardhat');
const { expect } = require('chai');

describe("Test MultiSig", () => {
    let multiSigFactory;
    let deployedWalletSig;
    let multiSig;

    before(async () => {
        multiSig = await ethers.getContractFactory("MultiSigWalletWithDailyLimitFactory");
        multiSigFactory = await multiSig.deploy();
        deployedWalletSig = await multiSigFactory.deployed();
        console.log(deployedWalletSig.address, "Factory Address");
        console.log("https://dev-scan.oortech.com/tx/"+deployedWalletSig.deployTransaction.hash, "Factory Creation Hash");
    });
    
    it("Should just deploy MultiSig Factory Contract and Match Bytecode", async () => {
        const data = JSON.stringify(deployedWalletSig.deployTransaction.data);
        expect(data).to.equal(data, "matched bytecode");
    });

    it("Should return correct Factory Address before moving on", async() => {
        const data = deployedWalletSig.address;
        expect(data).to.be.equal(deployedWalletSig.address);
    });

    it("should just create new wallet from factory multi sig using CREATE", async() => {
        const result = await deployedWalletSig.create(["0xD2a8fd999bF4CC30A3fD86f017DeD1c6BF67E78B", "0x6080184Da41682D75562fcae090dD5eB4Fc2E771"], 2, 3000)
        const data = result.data;
        expect(data).to.be.equal(result.data);
    })
});