const { BigNumber } = require("ethers");
const { ethers, waffle, artifacts } = require("hardhat");
const { solidity } = require("ethereum-waffle");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");
const assert = require("assert");

let diamondAddress;
let rets;
let accounts;
const TOKENS_DECIMAL = 8;

let accessRegistry;
let comptroller;
let dynamicInterest;
let dynamic;
let loan1;
let tokenList;

async function before() {
  array = await deployDiamond();
  diamondAddress = array["diamondAddress"];
  rets = await addMarkets(array);
  await provideLiquidity(rets);
  accounts = await ethers.getSigners();
  faucet = await ethers.getContractAt("Faucet", rets["faucetAddress"]);
  tx = await faucet.connect(accounts[1]).getTokens(0);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "TokensIssued");

  tx = await faucet.connect(accounts[1]).getTokens(1);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "TokensIssued");
  tx = await faucet.connect(accounts[1]).getTokens(2);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "TokensIssued");
  tx = await faucet.connect(accounts[1]).getTokens(3);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "TokensIssued");

  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  comptroller = await ethers.getContractAt("Comptroller", diamondAddress);
  dynamicInterest = await ethers.getContractAt("DynamicInterest", diamondAddress);
  loan1 = await ethers.getContractAt("Loan1", diamondAddress);
  accessRegistry = await ethers.getContractAt("AccessRegistry", rets["accessRegistryAddress"]);

  bepUsdt = await ethers.getContractAt("BEP20Token", rets["tUsdtAddress"]);
  bepBtc = await ethers.getContractAt("BEP20Token", rets["tBtcAddress"]);
  bepUsdc = await ethers.getContractAt("BEP20Token", rets["tUsdcAddress"]);
  bepWbnb = await ethers.getContractAt("BEP20Token", rets["tWBNBAddress"]);
  bepCake = await ethers.getContractAt("BEP20Token", rets["tCakeAddress"]);
  bepSxp = await ethers.getContractAt("BEP20Token", rets["tSxpAddress"]);
  pancakeRouter = await ethers.getContractAt("PancakeRouter", pancakeRouterAddr);

  tx = await dynamicInterest.setDepositInterests(200, 1000);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "DepositInterestUpdated");
  tx = await dynamicInterest.setBorrowInterests(500, 2000);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "BorrowInterestUpdated");
  tx = await dynamicInterest.setInterestFactors(2, 12);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "InterestFactorsUpdated");
}

const comit_NONE = utils.formatBytes32String("comit_NONE");
const comit_TWOWEEKS = utils.formatBytes32String("comit_TWOWEEKS");
const comit_ONEMONTH = utils.formatBytes32String("comit_ONEMONTH");
const comit_THREEMONTHS = utils.formatBytes32String("comit_THREEMONTHS");
const adminDynamicInterest = utils.formatBytes32String("adminDynamicInterest");

const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t

const pancakeRouterAddr = process.env.PancakeRouterAddress;

async function Pause_DynamicInterest() {
  console.log("Pause_DynamicInterest started");
  tx = await dynamicInterest.pauseDynamicInterest();
  await tx.wait();
  assert.equal(await dynamicInterest.isPausedDynamicInterest(), true);

  tx = await dynamicInterest.unpauseDynamicInterest();
  await tx.wait();
  assert.equal(await dynamicInterest.isPausedDynamicInterest(), false);

  reverted = true;
  try {
    tx = await dynamicInterest.connect(accounts[1]).pauseDynamicInterest();
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await accessRegistry.addAdminRole(adminDynamicInterest, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataGranted");

  reverted = true;
  try {
    tx = await accessRegistry.addAdminRole(adminDynamicInterest, accounts[1].address);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await dynamicInterest.connect(accounts[1]).pauseDynamicInterest();
  await tx.wait();
  assert.equal(await dynamicInterest.isPausedDynamicInterest(), true);

  tx = await accessRegistry.removeAdminRole(adminDynamicInterest, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataRevoked");

  reverted = true;
  try {
    tx = await dynamicInterest.connect(accounts[1]).unpauseDynamicInterest();
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal(await dynamicInterest.isPausedDynamicInterest(), true);

  tx = await dynamicInterest.unpauseDynamicInterest();
  await tx.wait();
  assert.equal(await dynamicInterest.isPausedDynamicInterest(), false);

  console.log("Pause_DynamicInterest passed");
}

async function Update_interests_Uf_25() {
  console.log("Update_interests_Uf_25 started");
  tx = await dynamicInterest.updateInterests(symbolBtc);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "InterestsUpdated");

  let apr;
  apr = await comptroller.getAPR(symbolBtc, comit_NONE);
  assert.equal(apr.toString(), "500"); // minimum value of borrowed interest

  let apy;
  apy = await comptroller.getAPY(symbolBtc, comit_THREEMONTHS);
  assert.equal(apy.toString(), "0"); // Uf < 25, deposit interest = 0
  console.log("Update_interests_Uf_25 passed");
}

async function Update_interests_Uf_70() {
  console.log("Update_interests_Uf_70 started");
  const loanAmount = ethers.utils.parseUnits("2600000", TOKENS_DECIMAL);
  const collateralAmount = ethers.utils.parseUnits("2000000", TOKENS_DECIMAL);
  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(pancakeRouterAddr));
  console.log(reserveBalance.toString());
  tx = await bepBtc.approve(diamondAddress, collateralAmount);
  await tx.wait();
  tx = await loan1.loanRequest(symbolBtc, comit_NONE, loanAmount, symbolBtc, collateralAmount);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[2]["event"], "NewLoan");

  tx = await dynamicInterest.updateInterests(symbolBtc);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "InterestsUpdated");

  // Uf = 61.84, therefore Uf= 62
  let apr;
  apr = await comptroller.getAPR(symbolBtc, comit_NONE);
  console.log('APR', apr.toString());

  let apy;
  apy = await comptroller.getAPY(symbolBtc, comit_THREEMONTHS);
  console.log('APY', apy.toString());
  console.log("Update_interests_Uf_70 passed");
}

async function Update_interests_Uf_70_more() {
  console.log("Update_interests_Uf_70_more started");
  const loanAmount = ethers.utils.parseUnits("1000000", TOKENS_DECIMAL);
  const collateralAmount = ethers.utils.parseUnits("400000", TOKENS_DECIMAL);

  tx = await bepBtc.approve(diamondAddress, collateralAmount);
  await tx.wait();
  tx = await loan1.loanRequest(symbolBtc, comit_ONEMONTH, loanAmount, symbolBtc, collateralAmount);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[2]['event'], "NewLoan");

  tx = await dynamicInterest.updateInterests(symbolBtc);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[0]['event'], "InterestsUpdated");
  // Uf = 86
  let apr;
  apr = await comptroller.getAPR(symbolBtc, comit_NONE);
  console.log('APR', apr.toString());

  apr = await comptroller.getAPR(symbolBtc, comit_ONEMONTH);
  console.log('APR', apr.toString());

  let apy;
  apy = await comptroller.getAPY(symbolBtc, comit_THREEMONTHS);
  console.log('APY', apr.toString());

  apy = await comptroller.getAPY(symbolBtc, comit_ONEMONTH);
  console.log('APY', apr.toString());

  apy = await comptroller.getAPY(symbolBtc, comit_TWOWEEKS);
  console.log('APY', apr.toString());
  console.log("Update_interests_Uf_70_more passed");
}


async function main() {
  await before();
  await Pause_DynamicInterest();
  await Update_interests_Uf_25();
  await Update_interests_Uf_70();
  await Update_interests_Uf_70_more();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;