const { ethers } = require("hardhat");
const utils = require("ethers").utils;
const {
  getSelectors,
  get,
  FacetCutAction,
  removeSelectors,
  findAddressPositionInFacets,
} = require("../scripts/libraries/diamond.js");

const assert = require("assert");

const { deployDiamond } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");

let diamondAddress;
let diamondCutFacet;
let diamondLoupeFacet;
let tokenList;
let comptroller;
let deposit;
let loan;
let loan1;
let oracle;
let reserve;
let liquidator;
let accounts;
let upgradeAdmin;
let bepUsdt;
let bepBtc;
let bepUsdc;
let bepCake;

let rets;
const addresses = [];

const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t
const symbolEth = "0x4554480000000000000000000000000000000000000000000000000000000000";
const symbolSxp = "0x5358500000000000000000000000000000000000000000000000000000000000"; // SXP
const symbolCAKE = "0x43414b4500000000000000000000000000000000000000000000000000000000"; // CAKE

const comit_NONE = utils.formatBytes32String("comit_NONE");
const comit_TWOWEEKS = utils.formatBytes32String("comit_TWOWEEKS");
const comit_ONEMONTH = utils.formatBytes32String("comit_ONEMONTH");
const comit_THREEMONTHS = utils.formatBytes32String("comit_THREEMONTHS");

async function before() {
  accounts = await ethers.getSigners();
  upgradeAdmin = accounts[0];
  console.log("account1 is ", accounts[1].address);

  array = await deployDiamond();
  diamondAddress = array["diamondAddress"];
  rets = await addMarkets(array);

  diamondCutFacet = await ethers.getContractAt("DiamondCutFacet", diamondAddress);
  diamondLoupeFacet = await ethers.getContractAt("DiamondLoupeFacet", diamondAddress);

  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  comptroller = await ethers.getContractAt("Comptroller", diamondAddress);
  deposit = await ethers.getContractAt("Deposit", diamondAddress);
  loan = await ethers.getContractAt("Loan", diamondAddress);
  loan1 = await ethers.getContractAt("Loan1", diamondAddress);
  oracle = await ethers.getContractAt("OracleOpen", diamondAddress);
  liquidator = await ethers.getContractAt("Liquidator", diamondAddress);
  reserve = await ethers.getContractAt("Reserve", diamondAddress);

  bepUsdt = await ethers.getContractAt("BEP20Token", rets["tUsdtAddress"]);
  bepBtc = await ethers.getContractAt("BEP20Token", rets["tBtcAddress"]);
  bepUsdc = await ethers.getContractAt("BEP20Token", rets["tUsdcAddress"]);
  bepWbnb = await ethers.getContractAt("BEP20Token", rets["tUsdcAddress"]);
  bepCake = await ethers.getContractAt("BEP20Token", rets["tCakeAddress"]);
}

async function facet_count() {
  console.log("facet_count started");
  for (const address of await diamondLoupeFacet.facetAddresses()) {
    addresses.push(address);
  }
  assert.equal(addresses.length, 12);
  console.log("facet_count passed");
}

async function function_selector() {
  console.log("function_selector started");
  let selectors = getSelectors(diamondCutFacet);
  result = await diamondLoupeFacet.facetFunctionSelectors(addresses[0]);
  assert.equal(result[0], selectors[0]);
  result = await diamondLoupeFacet.facetFunctionSelectors(addresses[1]);
  console.log("function_selector passed");
}

async function check_facets() {
  console.log("check_facets started");
  result = await diamondLoupeFacet.facets();
  assert.equal(result[0].facetAddress, addresses[0]);
  assert.equal(result[1].facetAddress, addresses[1]);
  assert.equal(result[2].facetAddress, addresses[2]);
  assert.equal(result[3].facetAddress, addresses[3]);
  assert.equal(result[4].facetAddress, addresses[4]);
  assert.equal(result[5].facetAddress, addresses[5]);
  assert.equal(result[6].facetAddress, addresses[6]);
  assert.equal(result[7].facetAddress, addresses[7]);
  console.log("check_facets passed");
}
async function main() {
  await before();
  await facet_count();
  await function_selector();
  await check_facets();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;