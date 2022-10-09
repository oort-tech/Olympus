const assert = require("assert");
const { BigNumber } = require("ethers");
const { ethers, waffle } = require("hardhat");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");
const TOKENS_DECIMAL = 8;

let diamondAddress;
let rets;
let accounts;

let loan;
let loan1;
let loan2;
let accessRegistry;
let faucet;
let library;
let tokenList;
let bepBtc;
let bepUsdc;
let bepUsdt;
let bepWbnb;
let bepCake;
let bepSxp;
let pancakeRouter;
let swapAmount;

const symbolWbnb = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // Usdt.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t
const symbolSxp = "0x5358500000000000000000000000000000000000000000000000000000000000"; // SXP
const symbolCAKE = "0x43414b4500000000000000000000000000000000000000000000000000000000"; // CAKE
const comit_ONEMONTH = utils.formatBytes32String("comit_ONEMONTH");
const comit_NONE = utils.formatBytes32String("comit_NONE");
const comit_TWOWEEKS = utils.formatBytes32String("comit_TWOWEEKS");
const pancakeRouterAddr = process.env.PancakeRouterAddress;

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

  loan1 = await ethers.getContractAt("Loan1", diamondAddress);
  oracle = await ethers.getContractAt("OracleOpen", diamondAddress);
  liquidator = await ethers.getContractAt("Liquidator", diamondAddress);
  deposit = await ethers.getContractAt("Deposit", diamondAddress);

  // deploying tokens
  bepUsdt = await ethers.getContractAt("BEP20Token", rets["tUsdtAddress"]);
  bepBtc = await ethers.getContractAt("BEP20Token", rets["tBtcAddress"]);
  bepUsdc = await ethers.getContractAt("BEP20Token", rets["tUsdcAddress"]);
  bepWbnb = await ethers.getContractAt("BEP20Token", rets["tWBNBAddress"]);
  bepCake = await ethers.getContractAt("BEP20Token", rets["tCakeAddress"]);
  bepSxp = await ethers.getContractAt("BEP20Token", rets["tSxpAddress"]);
  pancakeRouter = await ethers.getContractAt("PancakeRouter", pancakeRouterAddr);
}

async function Should_fail_if_liquidator_not_eligible(){
  console.log('Should_fail_if_liquidator_not_eligible started');
  const loanAmount = ethers.utils.parseUnits("80000", TOKENS_DECIMAL);
  const collateralAmount = ethers.utils.parseUnits("1", TOKENS_DECIMAL);
  const accounts = await ethers.getSigners();
  const upgradeAdmin = accounts[0];

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();
  tx = await loan1.connect(accounts[1]).loanRequest(symbolUsdc, comit_NONE, loanAmount, symbolBtc, collateralAmount);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[2]['event'], "NewLoan");

  swapAmount = await pancakeRouter.getAmountsOut(ethers.utils.parseUnits("10", TOKENS_DECIMAL), [
    rets["tBtcAddress"],
    rets["tWBNBAddress"],
  ]);

  // Decrease price so that loan gets liquidable
  tx = await bepBtc
    .connect(accounts[0])
    .approve(pancakeRouter.address, ethers.utils.parseUnits("1000", TOKENS_DECIMAL));
  await tx.wait();
  tx = await pancakeRouter
    .connect(accounts[0])
    .swapExactTokensForTokens(
      ethers.utils.parseUnits("1000", TOKENS_DECIMAL),
      1,
      [rets["tBtcAddress"], rets["tWBNBAddress"]],
      upgradeAdmin.address,
      Date.now() + 1000 * 60 * 10,
    );
  await tx.wait();

  tx = await bepUsdc.connect(accounts[0]).approve(diamondAddress, loanAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await liquidator.connect(accounts[0]).liquidation(accounts[1].address, symbolUsdc, comit_NONE);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  console.log('Should_fail_if_liquidator_not_eligible passed');
}

async function Non_Liquidable_loan(){
  console.log('Non_Liquidable_loan started');
  const loanAmount = ethers.utils.parseUnits("10", TOKENS_DECIMAL);
  const collateralAmount = ethers.utils.parseUnits("7", TOKENS_DECIMAL);
  const accounts = await ethers.getSigners();
  const upgradeAdmin = accounts[0];

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));
  tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();
  tx = await loan1.connect(accounts[1]).loanRequest(symbolWbnb, comit_ONEMONTH, loanAmount, symbolWbnb, collateralAmount);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[2]['event'], "NewLoan");
  // make liquidator eligible
  const depositAmount = ethers.utils.parseUnits("2500", TOKENS_DECIMAL);

  tx = await bepUsdt.connect(accounts[0]).approve(diamondAddress, depositAmount);
  await tx.wait();
  tx = await deposit.connect(accounts[0]).depositRequest(symbolUsdt, comit_TWOWEEKS, depositAmount);
  await tx.wait();

  assert.equal((await bepWbnb.balanceOf(diamondAddress)).toString(), reserveBalance.add(BigNumber.from(collateralAmount)).toString());

  let prevLoanAssetBalance = await bepWbnb.balanceOf(accounts[0].address);
  let prevDiamondLoanAssetBalance = await bepWbnb.balanceOf(diamondAddress);

  tx = await bepWbnb.connect(accounts[0]).approve(diamondAddress, loanAmount);
  await tx.wait();
  reverted = true;
  try {
    tx = await liquidator.connect(accounts[0]).liquidation(accounts[1].address, symbolWbnb, comit_ONEMONTH);
    await tx.wait();
    reverted = false;  
  } catch (error) {
    
  }
  assert(reverted);

  let postLoanAssetBalance = await bepWbnb.balanceOf(accounts[0].address);
  let postDiamondLoanAssetBalance = await bepWbnb.balanceOf(diamondAddress);

  assert.equal(prevLoanAssetBalance.toString(), postLoanAssetBalance.toString());
  assert.equal(prevDiamondLoanAssetBalance.toString(), postDiamondLoanAssetBalance.toString());
  console.log('Non_Liquidable_loan passed');
}

async function DC3_Liquidation() {
  console.log('DC3_Liquidation started');
  const loanAmount = ethers.utils.parseUnits("8000", TOKENS_DECIMAL);
  const collateralAmount = ethers.utils.parseUnits("10", TOKENS_DECIMAL);
  const accounts = await ethers.getSigners();
  const upgradeAdmin = accounts[0];

  tx = await bepWbnb.connect(accounts[0]).approve(pancakeRouter.address, swapAmount[swapAmount.length - 1]);
  await tx.wait();
  tx = await pancakeRouter
    .connect(accounts[0])
    .swapExactTokensForTokens(
      swapAmount[swapAmount.length - 1],
      1,
      [rets["tWBNBAddress"], rets["tBtcAddress"]],
      upgradeAdmin.address,
      Date.now() + 1000 * 60 * 10,
    );
  await tx.wait();
  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();
  tx = await loan1.connect(accounts[1]).loanRequest(symbolUsdc, comit_ONEMONTH, loanAmount, symbolBtc, collateralAmount);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[2]['event'], "NewLoan");

  tx = await bepBtc
    .connect(accounts[0])
    .approve(pancakeRouter.address, ethers.utils.parseUnits("1000", TOKENS_DECIMAL));
  await tx.wait();
  tx = await pancakeRouter
    .connect(accounts[0])
    .swapExactTokensForTokens(
      ethers.utils.parseUnits("1000", TOKENS_DECIMAL),
      1,
      [rets["tBtcAddress"], rets["tWBNBAddress"]],
      upgradeAdmin.address,
      Date.now() + 1000 * 60 * 10,
    );
  await tx.wait();

  // make liquidator eligible
  const depositAmount = ethers.utils.parseUnits("2500", TOKENS_DECIMAL);

  tx = await bepUsdt.connect(accounts[0]).approve(diamondAddress, depositAmount);
  await tx.wait();
  tx = await deposit.connect(accounts[0]).depositRequest(symbolUsdt, comit_TWOWEEKS, depositAmount);
  await tx.wait();

  assert.equal((await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.add(BigNumber.from(collateralAmount)).toString());

  // Decrease price so that loan gets liquidated
  tx = await bepBtc
    .connect(accounts[0])
    .approve(pancakeRouter.address, ethers.utils.parseUnits("1000", TOKENS_DECIMAL));
  await tx.wait();
  tx = await pancakeRouter
    .connect(accounts[0])
    .swapExactTokensForTokens(
      ethers.utils.parseUnits("1000", TOKENS_DECIMAL),
      1,
      [rets["tBtcAddress"], rets["tWBNBAddress"]],
      upgradeAdmin.address,
      Date.now() + 1000 * 60 * 10,
    );
  await tx.wait();

  let prevLoanAssetBalance = await bepUsdc.balanceOf(accounts[0].address);
  let prevCollateralAssetBalance = await bepBtc.balanceOf(accounts[0].address);

  let prevDiamondLoanAssetBalance = await bepUsdc.balanceOf(diamondAddress);
  let prevDiamondCollateralAssetBalance = await bepBtc.balanceOf(diamondAddress);

  tx = await bepUsdc.connect(accounts[0]).approve(diamondAddress, loanAmount);
  await tx.wait();
  tx = await liquidator.connect(accounts[0]).liquidation(accounts[1].address, symbolUsdc, comit_ONEMONTH);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[0]['event'], "Liquidation");

  let laterLoanAssetBalance = await bepUsdc.balanceOf(accounts[0].address);
  let laterCollateralAssetBalance = await bepBtc.balanceOf(accounts[0].address);

  let laterDiamondLoanAssetBalance = await bepUsdc.balanceOf(diamondAddress);
  let laterDiamondCollateralAssetBalance = await bepBtc.balanceOf(diamondAddress);

  assert(laterCollateralAssetBalance > prevCollateralAssetBalance);
  assert(prevLoanAssetBalance > laterLoanAssetBalance); // because of swap gas fee
  assert(laterDiamondLoanAssetBalance > prevDiamondLoanAssetBalance);
  assert(prevDiamondCollateralAssetBalance > laterDiamondCollateralAssetBalance);
  console.log('DC3_Liquidation passed');
}

async function after_liquidation_new_loan() {
  console.log('after liquidation new loan for same asset and commitment should work');
  const loanAmount = ethers.utils.parseUnits("3000", TOKENS_DECIMAL);
  const collateralAmount = ethers.utils.parseUnits("4", TOKENS_DECIMAL);
  const accounts = await ethers.getSigners();
  const upgradeAdmin = accounts[0];

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();
  tx = await loan1.connect(accounts[1]).loanRequest(symbolUsdc, comit_ONEMONTH, loanAmount, symbolBtc, collateralAmount);
  response = await tx.wait();
  events = response['events'];
  assert.equal(events[2]['event'], "NewLoan");
  console.log("after_liquidation_new_loan passed");
}


async function main() {
  await before();
  await Should_fail_if_liquidator_not_eligible();
  await Non_Liquidable_loan();
  // await DC3_Liquidation();
  await after_liquidation_new_loan();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;
