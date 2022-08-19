const { ethers } = require("hardhat");
const utils = require("ethers").utils;
const assert = require("assert");
const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");

let diamondAddress;
let rets;
let accounts;

let oracle;
let library;
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
  await provideLiquidity(rets);
  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  oracle = await ethers.getContractAt("OracleOpen", diamondAddress);
  accessRegistry = await ethers.getContractAt("AccessRegistry", rets["accessRegistryAddress"]);
}

async function Get_Quote_Price() {
  console.log("Get_Quote_Price started");
  let x;
  x = await oracle.getQuote(symbolBtc);
  assert.equal(x.toString(), "3944259440650");

  x = await oracle.getQuote(symbolUsdt);
  assert.equal(x.toString(), "100000000");

  x = await oracle.getQuote(symbolUsdc);
  assert.equal(x.toString(), "99600000");

  x = await oracle.getQuote(symbolWBNB);
  assert.equal(x.toString(), "39919960159");

  x = await oracle.getQuote(symbolSxp);
  assert.equal(x.toString(), "199001997");

  x = await oracle.getQuote(symbolCAKE);
  assert.equal(x.toString(), "793634733");
  console.log("Get_Quote_Price passed");
}

async function Pause() {
  console.log('Pause started');
  const adminOracle = utils.formatBytes32String("adminOpenOracle");
  tx = await oracle.pauseOracle();
  await tx.wait();
  assert.equal((await oracle.isPausedOracle()), true);

  await oracle.unpauseOracle();
  assert.equal((await oracle.isPausedOracle()), false);

  reverted = true;
  try {
    tx = await oracle.connect(accounts[1]).pauseOracle();
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  tx = await accessRegistry.addAdminRole(adminOracle, accounts[1].address);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[0]['event'], "AdminRoleDataGranted");

  reverted = true;
  try {
    tx = await accessRegistry.addAdminRole(adminOracle, accounts[1].address)
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);

  tx = await oracle.connect(accounts[1]).pauseOracle();
  await tx.wait();
  assert.equal((await oracle.isPausedOracle()), true);

  tx = await accessRegistry.removeAdminRole(adminOracle, accounts[1].address);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[0]['event'], "AdminRoleDataRevoked");

  reverted = true;
  try {
    tx = await oracle.connect(accounts[1]).unpauseOracle()
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal((await oracle.isPausedOracle()), true);

  tx = await oracle.unpauseOracle();
  await tx.wait();
  assert.equal((await oracle.isPausedOracle()), false);
  console.log('Pause passed');
}

async function main() {
  await before();
  await Get_Quote_Price();
  await Pause();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;
