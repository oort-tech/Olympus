const { BigNumber } = require("ethers");
const { ethers, waffle } = require("hardhat");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");
const assert = require("assert");

let diamondAddress;
let rets;
let tokenList;
let library;
let deposit;
let faucet;
let accounts;

let bepBtc;
let bepUsdc;
let bepUsdt;
let bepWbnb;

const comit_NONE = utils.formatBytes32String("comit_NONE");

async function before() {
  array = await deployDiamond();
  diamondAddress = array["diamondAddress"];
  rets = await addMarkets(array);
  accounts = await ethers.getSigners();
  faucet = await ethers.getContractAt("Faucet", rets["faucetAddress"]);
}

async function Faucet_Testing() {
  console.log('Faucet_Testing started');
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
  console.log('Faucet_Testing started');
}
async function Faucet_Testing_time_lock() {
  console.log('Faucet_Testing_time_lock started');
  reverted = true;
  try {
    tx = await faucet.connect(accounts[1]).getTokens(0);
    await tx.wait();  
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  
  reverted = true;
  try {
    tx = await faucet.connect(accounts[1]).getTokens(1);
    await tx.wait();  
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);

  reverted = true;
  try {
    tx = await faucet.connect(accounts[1]).getTokens(2);
    await tx.wait();  
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);

  reverted = true;
  try {
    tx = await faucet.connect(accounts[1]).getTokens(3);
    await tx.wait();  
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);

  
  console.log('Faucet_Testing_time_lock passed');
}

async function main() {
  await before();
  await Faucet_Testing();
  await Faucet_Testing_time_lock();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });
exports.main = main;