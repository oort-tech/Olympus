const assert = require("assert");
const { BigNumber } = require("ethers");
const { ethers } = require("hardhat");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");

let diamondAddress;
let rets;
let accounts;

let tokenList;
let accessRegistry;

const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t
const symbolSxp = "0x5358500000000000000000000000000000000000000000000000000000000000"; // SXP
const symbolCAKE = "0x43414b4500000000000000000000000000000000000000000000000000000000"; // CAKE
async function before() {
  array = await deployDiamond();
  diamondAddress = array["diamondAddress"];
  rets = await addMarkets(array);
  accounts = await ethers.getSigners();
  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  accessRegistry = await ethers.getContractAt("AccessRegistry", rets["accessRegistryAddress"]);
}

async function Primary_Market_Support() {
  console.log("Primary_Market_Support started");
  assert.equal(await tokenList.isMarketSupported(symbolBtc), true);

  assert.equal(await tokenList.isMarketSupported(symbolUsdc), true);

  assert.equal(await tokenList.isMarketSupported(symbolUsdt), true);

  assert.equal(await tokenList.isMarketSupported(symbolWBNB), true);

  reverted = true;
  try {
    tx = await tokenList.isMarketSupported(symbolCAKE);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  console.log("Primary_Market_Support passed");
}

async function Secondary_Market_Support() {
  console.log("Secondary_Market_Support started");
  assert.equal(await tokenList.isMarket2Supported(symbolCAKE), true);

  assert.equal(await tokenList.isMarket2Supported(symbolSxp), true);
  reverted = true;
  try {
    tx = await tokenList.isMarketSupported(symbolUsdc);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  console.log("Secondary_Market_Support passed");
}

async function Primary_Market_Removal() {
  console.log("Primary_Market_Removal started");
  tx = await tokenList.removeMarketSupport(symbolBtc);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "MarketSupportRemoved");
  reverted = true;
  try {
    tx = await tokenList.isMarketSupported(symbolBtc);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  console.log("Primary_Market_Removal passed");
}

async function Add_Primary_Market() {
  console.log("Add_Primary_Market started");
  reverted = true;
  try {
    tx = await tokenList.connect(accounts[1]).addMarketSupport(symbolBtc, 6, rets["tBtcAddress"], 100000000);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await tokenList.addMarketSupport(symbolBtc, 6, rets["tBtcAddress"], 100000000);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "MarketSupportAdded");

  assert.equal(await tokenList.isMarketSupported(symbolBtc), true);
  console.log("Add_Primary_Market passed");
}

async function Update_Primary_Market() {
  console.log("Update_Primary_Market started");
  reverted = true;
  try {
    tx = await tokenList.connect(accounts[1]).updateMarketSupport(symbolBtc, 8, rets["tBtcAddress"], 100000000);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  tx = await tokenList.updateMarketSupport(symbolBtc, 8, rets["tBtcAddress"], 100000000);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "MarketSupportUpdated");

  assert.equal(await tokenList.isMarketSupported(symbolBtc), true);
  console.log("Update_Primary_Market passed");
}

async function Secondary_Market_Removal() {
  console.log("Secondary_Market_Removal started");
  reverted = true;
  try {
    tx = await tokenList.connect(accounts[1]).removeMarket2Support(symbolCAKE);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await tokenList.removeMarket2Support(symbolCAKE);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "Market2Removed");

  reverted = true;
  try {
    tx = await tokenList.isMarketSupported(symbolCAKE);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  console.log("Secondary_Market_Removal passed");
}

async function Add_Secondary_Market() {
  console.log("Add_Secondary_Market started");
  reverted = true;
  try {
    tx = await tokenList.connect(accounts[1]).addMarket2Support(symbolCAKE, 6, rets["tCakeAddress"]);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await tokenList.addMarket2Support(symbolCAKE, 6, rets["tCakeAddress"]);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "Market2Added");

  assert.equal(await tokenList.isMarket2Supported(symbolCAKE), true);
  console.log("Add_Secondary_Market passed");
}

async function Update_Secondary_Market() {
  console.log("Update_Secondary_Market started");
  reverted = true;
  try {
    tx = await tokenList.connect(accounts[1]).updateMarket2Support(symbolCAKE, 8, rets["tCakeAddress"]);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await tokenList.updateMarket2Support(symbolCAKE, 8, rets["tCakeAddress"]);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "Market2Updated");

  assert.equal(await tokenList.isMarket2Supported(symbolCAKE), true);
  console.log("Update_Secondary_Market passed");
}

async function check_status() {
  console.log("check_status started");
  assert.equal(await tokenList.getMarketAddress(symbolBtc), rets["tBtcAddress"]);
  assert.equal((await tokenList.getMarketDecimal(symbolBtc)).toString(), "8");

  reverted = true;
  try {
    tx = await tokenList.minAmountCheck(symbolUsdc, 10000000);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal(await tokenList.getMarket2Address(symbolCAKE), rets["tCakeAddress"]);
  assert.equal((await tokenList.getMarket2Decimal(symbolCAKE)).toString(), "8");

  console.log("check_status passed");
}

async function Pause() {
  console.log("Pause started");
  const adminTokenList = utils.formatBytes32String("adminTokenList");

  tx = await tokenList.pauseTokenList();
  await tx.wait();
  assert.equal((await tokenList.isPausedTokenList()), true);

  tx = await tokenList.unpauseTokenList();
  await tx.wait();
  assert.equal((await tokenList.isPausedTokenList()), false);

  reverted = true;
  try {
    tx = await tokenList.connect(accounts[1]).pauseTokenList();
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await accessRegistry.addAdminRole(adminTokenList, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataGranted");

  reverted = true;
  try {
    tx = await accessRegistry.addAdminRole(adminTokenList, accounts[1].address);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await tokenList.connect(accounts[1]).pauseTokenList();
  await tx.wait();
  assert.equal((await tokenList.isPausedTokenList()), true);

  tx = await accessRegistry.removeAdminRole(adminTokenList, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataRevoked");

  reverted = true;
  try {
    tx = await tokenList.connect(accounts[1]).unpauseTokenList();
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal((await tokenList.isPausedTokenList()), true);

  tx = await tokenList.unpauseTokenList();
  await tx.wait();
  assert.equal((await tokenList.isPausedTokenList()), false);
  console.log("Pause passed");
}

async function main() {
  await before();
  await Primary_Market_Support();
  await Secondary_Market_Support();
  await Primary_Market_Removal();
  await Add_Primary_Market();
  await Update_Primary_Market();
  await Secondary_Market_Removal();
  await Add_Secondary_Market();
  await Update_Secondary_Market();
  await check_status();
  await Pause();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;