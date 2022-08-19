const { BigNumber } = require("ethers");
const { ethers } = require("hardhat");
const { solidity } = require("ethereum-waffle");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");

const assert = require("assert");

let diamondAddress;
let rets;
let accounts;

let accessRegistry;
let comptroller;
let library;
let tokenList;

async function before_all() {
  array = await deployDiamond();
  diamondAddress = array["diamondAddress"];
  rets = await addMarkets(array);
  accounts = await ethers.getSigners();
}

const comit_NONE = utils.formatBytes32String("comit_NONE");
const comit_TWOWEEKS = utils.formatBytes32String("comit_TWOWEEKS");
const comit_ONEMONTH = utils.formatBytes32String("comit_ONEMONTH");
const comit_THREEMONTHS = utils.formatBytes32String("comit_THREEMONTHS");
const adminComptroller = utils.formatBytes32String("adminComptroller");

const comit_THREEMONTH = utils.formatBytes32String("comit_THREEMONTH");

const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t

async function before_getter() {
  // deploying relevant contracts
  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  comptroller = await ethers.getContractAt("Comptroller", diamondAddress);
  accessRegistry = await ethers.getContractAt("AccessRegistry", rets["accessRegistryAddress"]);
}

async function pause_comptroller() {
  console.log("pause_comptroller started");
  tx = await comptroller.pauseComptroller();
  await tx.wait();
  assert.equal(await comptroller.isPausedComptroller(), true);
  tx = await comptroller.unpauseComptroller();
  await tx.wait();
  assert.equal(await comptroller.isPausedComptroller(), false);
  // await expect(comptroller.connect(accounts[1]).pauseComptroller()).to.be.reverted;
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).pauseComptroller();
    await tx.wait();
    reverted = false;
  } catch (error) {
  }
  assert(reverted);
  tx = await accessRegistry.addAdminRole(adminComptroller, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataGranted");
  reverted = true;
  try {
    tx = await accessRegistry.addAdminRole(adminComptroller, accounts[1].address);
    await tx.wait();
    reverted = false;
  } catch (error) {
  }
  assert(reverted);
  tx = await comptroller.connect(accounts[1]).pauseComptroller();
  await tx.wait();
  assert.equal(await comptroller.isPausedComptroller(), true);
  tx = await accessRegistry.removeAdminRole(adminComptroller, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataRevoked");
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).unpauseComptroller();
    await tx.wait();
    reverted = false;
  } catch (error) {
  }
  assert(reverted);
  // await expect(comptroller.connect(accounts[1]).unpauseComptroller()).to.be.reverted;
  assert.equal(await comptroller.isPausedComptroller(), true);
  console.log(9);
  tx = await comptroller.unpauseComptroller();
  await tx.wait();
  assert.equal(await comptroller.isPausedComptroller(), false);
  console.log("pause_comptroller passed");
}

async function Get_APR() {
  console.log("Get_APR Started");
  let apr;
  apr = BigNumber.from(await comptroller.getAPR(symbolBtc, comit_NONE));
  assert.equal(apr.toString(), "1800");

  apr = BigNumber.from(await comptroller.getAPR(symbolUsdt, comit_ONEMONTH));
  assert.equal(apr.toString(), "1500");
  console.log("Get_APR passed");
}

async function Get_APY() {
  console.log("Get_APY started");
  let apy;
  apy = await comptroller.getAPY(symbolBtc, comit_NONE);
  assert.equal(apy.toString(), "780");

  apy = await comptroller.getAPY(symbolUsdc, comit_TWOWEEKS);
  assert.equal(apy.toString(), "1000");

  apy = await comptroller.getAPY(symbolUsdt, comit_ONEMONTH);
  assert.equal(apy.toString(), "1500");

  apy = await comptroller.getAPY(symbolWBNB, comit_THREEMONTHS);
  assert.equal(apy.toString(), "1800");
  console.log("Get_APY passed");
}

async function Get_APR_Lasttime() {
  console.log("Get_APR_Lasttime started");
  let apr;
  apr = await comptroller.getAprLastTime(symbolBtc, comit_NONE);
  assert.notEqual(apr.toString(), "0");

  apr = await comptroller.getAprLastTime(symbolUsdc, comit_ONEMONTH);
  assert.notEqual(apr.toString(), "0");
  console.log("Get_APR_Lasttime passed");
}

async function Get_APY_Lasttime() {
  console.log("Get_APY_Lasttime started");
  let apy;
  apy = await comptroller.getApyLastTime(symbolBtc, comit_NONE);
  assert.notEqual(apy.toString(), "0");

  apy = await comptroller.getApyLastTime(symbolBtc, comit_TWOWEEKS);
  assert.notEqual(apy.toString(), "0");

  apy = await comptroller.getApyLastTime(symbolBtc, comit_ONEMONTH);
  assert.notEqual(apy.toString(), "0");

  apy = await comptroller.getApyLastTime(symbolBtc, comit_THREEMONTHS);
  assert.notEqual(apy.toString(), "0");
  console.log("Get_APY_Lasttime passed");
}

async function Get_APR_Ind() {
  console.log("Get_APR_Ind started");
  let apy;
  apy = await comptroller.getAPRInd(symbolUsdc, comit_NONE, 0);
  assert.equal(apy.toString(), "1800");

  apy = await comptroller.getAPRInd(symbolUsdc, comit_ONEMONTH, 0);
  assert.equal(apy.toString(), "1500");
  console.log("Get_APR_Ind passed");
}

async function Get_APY_Ind() {
  console.log("Get_APY_Ind started");
  let apy;
  apy = await comptroller.getAPYInd(symbolUsdc, comit_NONE, 0);
  assert.equal(apy.toString(), "780");

  apy = await comptroller.getAPYInd(symbolUsdc, comit_TWOWEEKS, 0);
  assert.equal(apy.toString(), "1000");

  apy = await comptroller.getAPYInd(symbolUsdc, comit_ONEMONTH, 0);
  assert.equal(apy.toString(), "1500");

  apy = await comptroller.getAPYInd(symbolUsdc, comit_THREEMONTHS, 0);
  assert.equal(apy.toString(), "1800");
  console.log("Get_APY_Ind passed");
}

async function Get_APR_Time() {
  console.log("Get_APR_Time started");
  let apr;
  apr = await comptroller.getAprtime(symbolUsdc, comit_NONE, 0);
  assert.notEqual(apr.toString(), "0");

  apr = await comptroller.getAprtime(symbolUsdc, comit_ONEMONTH, 0);
  assert.notEqual(apr.toString(), "0");
  console.log("Get_APR_Time passed");
}

async function Get_APY_Time() {
  console.log("Get_APY_Time started");
  let apr;
  apr = await comptroller.getApytime(symbolBtc, comit_NONE, 0);
  assert.notEqual(apr.toString(), "0");

  apr = await comptroller.getApytime(symbolBtc, comit_TWOWEEKS, 0);
  assert.notEqual(apr.toString(), "0");

  apr = await comptroller.getApytime(symbolBtc, comit_ONEMONTH, 0);
  assert.notEqual(apr.toString(), "0");

  apr = await comptroller.getApytime(symbolBtc, comit_THREEMONTHS, 0);
  assert.notEqual(apr.toString(), "0");
  console.log("Get_APY_Time passed");
}

async function Get_APR_Time_Length() {
  console.log("Get_APR_Time_Length started");
  let apr;
  apr = await comptroller.getAprTimeLength(symbolBtc, comit_NONE);
  assert.equal(apr.toString(), "1");

  apr = await comptroller.getAprTimeLength(symbolBtc, comit_ONEMONTH);
  assert.equal(apr.toString(), "1");
  console.log("Get_APR_Time_Length passed");
}

async function Get_APY_Time_Length() {
  console.log("Get_APY_Time_Length started");
  let apy;
  apy = await comptroller.getApyTimeLength(symbolBtc, comit_NONE);
  assert.equal(apy.toString(), "1");

  apy = await comptroller.getApyTimeLength(symbolBtc, comit_TWOWEEKS);
  assert.equal(apy.toString(), "1");

  apy = await comptroller.getApyTimeLength(symbolBtc, comit_ONEMONTH);
  assert.equal(apy.toString(), "1");

  apy = await comptroller.getApyTimeLength(symbolBtc, comit_THREEMONTHS);
  assert.equal(apy.toString(), "1");
  console.log("Get_APY_Time_Length passed");
}

async function Get_Commitment() {
  console.log("Get_Commitment started");
  let apy;
  apy = await comptroller.getCommitment(0, 1);
  assert.equal(apy, comit_NONE);

  apy = await comptroller.getCommitment(1, 0);
  assert.equal(apy, comit_TWOWEEKS);
  console.log("Get_Commitment passed");
}

async function Set_APR() {
  console.log("Set_APR started");
  reverted = true;
  try {
    tx = await expect(comptroller.connect(accounts[1]).updateAPR(symbolUsdc, comit_NONE, 2800)).to.be.reverted;
    response = await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.updateAPR(symbolUsdc, comit_NONE, 2800);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "APRupdated");

  let apy;
  apy = await comptroller.getAPR(symbolUsdc, comit_NONE);
  assert.equal(apy.toString(), "2800");

  apy = await comptroller.getAprTimeLength(symbolUsdc, comit_NONE);
  assert.equal(apy.toString(), "2");
  console.log("Set_APR passed");
}

async function Set_APY() {
  console.log("Set_APY started");
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).updateAPY(symbolUsdc, comit_NONE, 2800);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.updateAPY(symbolUsdc, comit_NONE, 2800);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "APYupdated");

  let apy;
  apy = await comptroller.getAPY(symbolUsdc, comit_NONE);
  assert.equal(apy.toString(), "2800");

  apy = await comptroller.getApyTimeLength(symbolUsdc, comit_NONE);
  assert.equal(apy.toString(), "2");
  console.log("Set_APY passed");
}

async function setTimelockValidityDeposit() {
  console.log("setTimelockValidityDeposit started");
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).setTimelockValidityDeposit(86400);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.setTimelockValidityDeposit(86400);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "TimelockValidityDeposit");
  console.log("setTimelockValidityDeposit passed");
}

async function SetLoanIssuanceFee() {
  console.log("SetLoanIssuanceFee started");
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).updateLoanIssuanceFees(10);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.updateLoanIssuanceFees(10);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "LoanIssuanceFeesUpdated");
  console.log("LoanIssuanceFeesUpdated passed");
}

async function SetLoanClosureFee() {
  console.log("SetLoanClosureFee started");
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).updateLoanClosureFees(5);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.updateLoanClosureFees(5);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "LoanClosureFeesUpdated");
  console.log("SetLoanClosureFee passed");
}

async function SetLoanpreClosureFee() {
  console.log("SetLoanpreClosureFee started");
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).updateLoanPreClosureFees(36);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.updateLoanPreClosureFees(36);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "LoanPreClosureFeesUpdated");
  console.log("SetLoanpreClosureFee passed");
}

async function SetDepositpreClosureFee() {
  console.log("SetDepositpreClosureFee started");
  reverted = true;
  try {
    tx = await comptroller.connect(accounts[1]).updateDepositPreclosureFees(36);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.updateDepositPreclosureFees(36);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "DepositPreClosureFeesUpdated");

  let apy = await comptroller.depositPreClosureFees();
  assert.equal(apy.toString(), "36");

  console.log("SetDepositpreClosureFee passed");
}

async function SetWithdrawalFee() {
  console.log("SetWithdrawalFee started");
  try {
    reverted = true;
    tx = await comptroller.connect(accounts[1]).updateWithdrawalFees(17);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await comptroller.updateWithdrawalFees(17);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "DepositWithdrawalFeesUpdated");

  let apy = await comptroller.depositWithdrawalFees();
  assert.equal(apy.toString(), "17");

  console.log("SetWithdrawalFee passed");
}

async function SetCollateralreleaseFee() {
  console.log("SetCollateralreleaseFee started");
  try {
    reverted = true;
    tx = await comptroller.connect(accounts[1]).updateCollateralReleaseFees(10);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await comptroller.updateCollateralReleaseFees(10);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "CollateralReleaseFeesUpdated");

  let apy = await comptroller.collateralReleaseFees();
  assert.equal(apy.toString(), "10");

  console.log("SetCollateralreleaseFee passed");
}

async function SetYieldConversionFee() {
  console.log("SetYieldConversionFee started");
  try {
    reverted = true;
    tx = await comptroller.connect(accounts[1]).updateYieldConversion(10);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await comptroller.updateYieldConversion(10);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "YieldConversionFeesUpdated");

  console.log("SetYieldConversionFee passed");
}

async function SetMarketSwapFee() {
  console.log("SetMarketSwapFee started");
  try {
    reverted = true;
    tx = await comptroller.connect(accounts[1]).updateMarketSwapFees(5);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await comptroller.updateMarketSwapFees(5);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "MarketSwapFeesUpdated");

  console.log("SetMarketSwapFee passed");
}

async function SetReserveFactor() {
  console.log("SetReserveFactor started");
  try {
    reverted = true;
    tx = await comptroller.connect(accounts[1]).updateReserveFactor(1);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await comptroller.updateReserveFactor(1);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "ReserveFactorUpdated");

  apy = await comptroller.getReserveFactor();
  assert.equal(apy.toString(), "1");
  console.log("SetReserveFactor passed");
}

async function SetMaxWithdrawalLimit() {
  console.log("SetMaxWithdrawalLimit started");
  const x = (await ethers.provider.getBlock()).timestamp;
  try {
    reverted = true;
    tx = await comptroller.connect(accounts[1]).updateMaxWithdrawal(2800, x);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await comptroller.updateMaxWithdrawal(2800, x);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "MaxWithdrawalUpdated");
  console.log("SetMaxWithdrawalLimit passed");
}

async function SetDepositCommitment() {
  console.log('SetDepositCommitment started')
  tx = await comptroller.setDepositCommitment("0x636f6d69745f54574f4d4f4e5448530000000000000000000000000000000000", 60);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "CommitmentAdded");

  let x = await comptroller.getCommitment(3, 0);
  assert.equal(x.toString(), "0x636f6d69745f54574f4d4f4e5448530000000000000000000000000000000000");

  tx = await comptroller.setDepositCommitment("0x636f6d69745f5349584d4f4e5448530000000000000000000000000000000000", 180);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "CommitmentAdded");

  x = await comptroller.getCommitment(5, 0);
  assert.equal(x.toString(), "0x636f6d69745f5349584d4f4e5448530000000000000000000000000000000000");
  console.log('SetDepositCommitment passed')
}

async function SetBorrowCommitment() {
  console.log('SetBorrowCommitment started')
  tx = await comptroller.setBorrowCommitment("0x636f6d69745f5349584d4f4e5448530000000000000000000000000000000000", 180)
  response = await tx.wait()
  events = response['events']
  assert.equal(events[0]['event'], "CommitmentAdded");

  let x = await comptroller.getCommitment(2, 1);
  assert.equal(x.toString(), "0x636f6d69745f5349584d4f4e5448530000000000000000000000000000000000");

  tx = await comptroller.setBorrowCommitment("0x636f6d69745f54574f4d4f4e5448530000000000000000000000000000000000", 60)
  response = await tx.wait()
  events = response['events']
  assert.equal(events[0]['event'], "CommitmentAdded");

  x = await comptroller.getCommitment(2, 1);
  assert.equal(x.toString(), "0x636f6d69745f54574f4d4f4e5448530000000000000000000000000000000000");

  x = await comptroller.getCommitment(3, 1);
  assert.equal(x.toString(), "0x636f6d69745f5349584d4f4e5448530000000000000000000000000000000000");
  console.log('SetBorrowCommitment passed')
}

async function main() {
  await before_all();
  await before_getter();
  await pause_comptroller();
  await Get_APR();
  await Get_APY();
  await Get_APR_Lasttime();
  await Get_APY_Lasttime();
  await Get_APR_Ind();
  await Get_APY_Ind();
  await Get_APR_Time();
  await Get_APY_Time();
  await Get_APR_Time_Length();
  await Get_APY_Time_Length();
  await Get_Commitment();

  await Set_APR();
  await Set_APY();
  await setTimelockValidityDeposit();
  await SetLoanIssuanceFee();
  await SetLoanClosureFee();
  await SetLoanpreClosureFee();
  await SetDepositpreClosureFee();

  await SetWithdrawalFee();
  await SetCollateralreleaseFee();
  await SetYieldConversionFee();
  await SetMarketSwapFee();
  await SetReserveFactor();
  await SetMaxWithdrawalLimit();
  await SetDepositCommitment();
  await SetBorrowCommitment();
}


// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;