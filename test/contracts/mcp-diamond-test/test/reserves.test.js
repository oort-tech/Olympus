const assert = require("assert");
const { BigNumber } = require("ethers");
const { ethers } = require("hardhat");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");

let diamondAddress;
let rets;
let accounts;

let reserve;
let library;
let tokenList;
let accessRegistry;

const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t
async function before() {
  array = await deployDiamond();
  diamondAddress = array["diamondAddress"];
  rets = await addMarkets(array);
  accounts = await ethers.getSigners();
  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  reserve = await ethers.getContractAt("Reserve", diamondAddress);
  accessRegistry = await ethers.getContractAt("AccessRegistry", rets["accessRegistryAddress"]);
}

async function Available_Market_reserves() {
  console.log("Available_Market_reserves started");
  let x;
  x = await reserve.avblMarketReserves(symbolBtc);
  assert.notEqual(x.toString(), "0");

  x = await reserve.avblMarketReserves(symbolUsdt);
  assert.notEqual(x.toString(), "0");

  x = await reserve.avblMarketReserves(symbolUsdc);
  assert.notEqual(x.toString(), "0");

  x = await reserve.avblMarketReserves(symbolWBNB);
  assert.notEqual(x.toString(), "0");
  console.log("Available_Market_reserves passed");
}

async function Market_reserves() {
  console.log("Market_reserves started");
  let x;
  x = BigNumber.from(await reserve.marketReserves(symbolBtc));
  assert(x > BigNumber.from(0));

  x = BigNumber.from(await reserve.marketReserves(symbolUsdt));
  assert(x > BigNumber.from(0));

  x = BigNumber.from(await reserve.marketReserves(symbolUsdc));
  assert(x > BigNumber.from(0));

  x = BigNumber.from(await reserve.marketReserves(symbolWBNB));
  assert(x > BigNumber.from(0));
  console.log("Market_reserves passed");
}

async function Market_Utilization() {
  console.log("Market_Utilization started");
  let x;
  x = await reserve.marketUtilisation(symbolBtc);
  assert.equal(x.toString(), "0");

  x = await reserve.marketUtilisation(symbolUsdt);
  assert.equal(x.toString(), "0");

  x = await reserve.marketUtilisation(symbolUsdc);
  assert.equal(x.toString(), "0");

  x = await reserve.marketUtilisation(symbolWBNB);
  assert.equal(x.toString(), "0");
  console.log("Market_Utilization passed");
}

async function transferAnyBep20() {
  console.log("transferAnyBep20 started");
  bepUsdt = await ethers.getContractAt("BEP20Token", rets["tUsdtAddress"]);
  const reserveBalance = await bepUsdt.balanceOf(diamondAddress);
  reverted = true;
  try {
    tx = await reserve.connect(accounts[1]).transferAnyBEP20(rets["tUsdtAddress"], accounts[1].address, 10000000000);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await reserve.connect(accounts[0]).transferAnyBEP20(rets["tUsdtAddress"], accounts[0].address, 10000000000);
  await tx.wait();
  assert.equal(
    (await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(10000000000)).toString(),
  );
  console.log("transferAnyBep20 passed");
}

async function Pause() {
  console.log('Pause started');
  const adminReserve = utils.formatBytes32String("adminReserve");
  tx = await reserve.pauseReserve();
  await tx.wait();
  assert.equal((await reserve.isPausedReserve()), true);

  await reserve.unpauseReserve();
  await tx.wait();
  assert.equal((await reserve.isPausedReserve()), false);

  reverted = true;
  try {
    tx = await reserve.connect(accounts[1]).pauseReserve();
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);

  tx = await accessRegistry.addAdminRole(adminReserve, accounts[1].address);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events['0']['event'],"AdminRoleDataGranted");

  reverted = true;
  try {
    tx = await accessRegistry.addAdminRole(adminReserve, accounts[1].address);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);

  tx = await reserve.connect(accounts[1]).pauseReserve();
  await tx.wait();
  assert.equal((await reserve.isPausedReserve()), true);

  tx = await accessRegistry.removeAdminRole(adminReserve, accounts[1].address);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events['0']['event'], "AdminRoleDataRevoked");

  reverted = true;
  try {
    tx = await reserve.connect(accounts[1]).unpauseReserve();
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal((await reserve.isPausedReserve()), true);

  tx = await reserve.unpauseReserve();
  await tx.wait();
  assert.equal((await reserve.isPausedReserve()), false);
  console.log('Pause passed');
}

async function main() {
  await before();
  await Available_Market_reserves();
  await Market_reserves();
  await Market_Utilization();
  await transferAnyBep20();
  await Pause();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;
