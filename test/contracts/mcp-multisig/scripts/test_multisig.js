const { assert } = require("chai");
const { ethers, upgrades } = require("hardhat");

const ONE = ethers.utils.parseEther("1");
const FINNEY = ethers.utils.parseEther("0.001");

async function mainTest() {
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimit", "0xFd3Aefa15a57eD3f91852a574a671acBDE46a920");
    const result = await MultiSig.submitTransaction("0x3c44cdddb6a900fa2b585dd299e03d12fa4293bc", FINNEY, "0x");
    console.log(result.hash);
    console.log(result.data);

}

async function calcMaxWithdrawLimit() {
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimit", "0xFd3Aefa15a57eD3f91852a574a671acBDE46a920");
    const result = await MultiSig.calcMaxWithdraw();
    console.log(result.toString());

}

async function changeDayLimit() {
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimit", "0xFd3Aefa15a57eD3f91852a574a671acBDE46a920");
    const result = await MultiSig.changeRequirement(BigInt(5000000000000000000));
    console.log(result.toString());
}

async function testaddOwner() {
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimit", "0xFd3Aefa15a57eD3f91852a574a671acBDE46a920");
    const result = await MultiSig.addOwner("0x5c09D78bd073e6e7739918377D54da7E0422C2cb");
    console.log(result.data);
}

async function submitTx() {
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimit", "0xFd3Aefa15a57eD3f91852a574a671acBDE46a920");
    const result = await MultiSig.submitTransaction("0xFd3Aefa15a57eD3f91852a574a671acBDE46a920", ONE, "0x7065cb4800000000000000000000000009a858450f4AF499cd79DeB8b2FF24C2211B695b");
    // revert or data
    console.log(result.data);

}

async function confirmTx() {
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimit", "0xFd3Aefa15a57eD3f91852a574a671acBDE46a920");
    const result = await MultiSig.confirmTransaction(5);
    console.log(result.hash, "tx hash");
    console.log(result.data, "tx data");
}



async function testExecution() {
    const owner = await ethers.getSigner();
    const deposit = FINNEY;
    const MultiSig = await ethers.getContractAt("MultiSigWalletWithDailyLimit", "0xFd3Aefa15a57eD3f91852a574a671acBDE46a920");
    await owner.sendTransaction({to: MultiSig.address, value: deposit})
    console.assert(await ethers.provider.getBalance(MultiSig.address), deposit, "deposit not received");
}


//mainTest()
//calcMaxWithdrawLimit()
//changeDayLimit()
//testaddOwner()
//submitTx()
//confirmTx()
///createWalletFromFactory()
testExecution()
    .then(() => process.exit(0)).catch(error => {
    console.error(error);
    process.exit(1);
});