const {ethers} = require("hardhat");

async function createWalletFromFactory() {
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimitFactory", "0x65850996E19C1d31e02De08E617E9A69C40Ac666");
    const result = await MultiSig.create(["0xD2a8fd999bF4CC30A3fD86f017DeD1c6BF67E78B", "0x1F267D718732177126EA9899C39C2158675e5A37", "0x6080184Da41682D75562fcae090dD5eB4Fc2E771", "0x5c09D78bd073e6e7739918377D54da7E0422C2cb"], 1, 3000);
    console.log(result.hash, "tx hash");
    console.log(result.data, "tx data");
}

createWalletFromFactory()
.then(() => process.exit(0)).catch(error => {
    console.error(error);
    process.exit(1);
});