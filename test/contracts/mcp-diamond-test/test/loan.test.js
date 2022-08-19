const assert = require("assert");
const { BigNumber } = require("ethers");
const { ethers, waffle } = require("hardhat");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");

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

const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t
const symbolSxp = "0x5358500000000000000000000000000000000000000000000000000000000000"; // SXP
const symbolCAKE = "0x43414b4500000000000000000000000000000000000000000000000000000000"; // CAKE
const comit_NONE = utils.formatBytes32String("comit_NONE");
const comit_ONEMONTH = utils.formatBytes32String("comit_ONEMONTH");

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

  // deploying relevant contracts
  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  loan = await ethers.getContractAt("Loan", diamondAddress);
  loan1 = await ethers.getContractAt("Loan1", diamondAddress);
  loan2 = await ethers.getContractAt("Loan2", diamondAddress);
  accessRegistry = await ethers.getContractAt("AccessRegistry", rets["accessRegistryAddress"]);

  // deploying tokens
  bepUsdt = await ethers.getContractAt("BEP20Token", rets["tUsdtAddress"]);
  bepBtc = await ethers.getContractAt("BEP20Token", rets["tBtcAddress"]);
  bepUsdc = await ethers.getContractAt("BEP20Token", rets["tUsdcAddress"]);
  bepWbnb = await ethers.getContractAt("BEP20Token", rets["tWBNBAddress"]);
  bepCake = await ethers.getContractAt("BEP20Token", rets["tCakeAddress"]);
  bepSxp = await ethers.getContractAt("BEP20Token", rets["tSxpAddress"]);
}

async function USDT_New_Loan_1_4_CDR() {
  console.log("USDT_New_Loan_1_4_CDR started");
  const loanAmount = 40000000000;
  const collateralAmount = 10000000000;

  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();
  reverted = true;
  try {
    tx = await loan1.connect(accounts[1]).loanRequest(symbolUsdt, comit_NONE, loanAmount, symbolUsdt, collateralAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());

  assert.equal((await loan1.avblReservesLoan(symbolUsdt)).toString(), "0");
  assert.equal((await loan1.utilisedReservesLoan(symbolUsdt)).toString(), "0");
  console.log("USDT_New_Loan_1_4_CDR passed");
}

async function USDT_New_Loan() {
  console.log("USDT_New_Loan started");
  const loanAmount = 500000000000;
  const collateralAmount = 400000000000;

  const loanFees = BigNumber.from(loanAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();

  tx = await loan1.connect(accounts[1]).loanRequest(symbolUsdt, comit_NONE, loanAmount, symbolUsdt, collateralAmount);
  response = await tx.wait();
  console.log(response);
  assert.equal(response["events"][2]["event"], "NewLoan");

  let loanData = await loan1.getLoans(accounts[1].address);
  const loanAmountPostFees = loanAmount - loanFees; // 0.17 Btc
  assert.equal(loanData.loanAmount[0].toString(), loanAmountPostFees.toString());

  assert.equal(
    (await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(collateralAmount)).toString(),
  );
  console.log("USDT_New_Loan passed");
}

async function USDT_New_Loan_Retry() {
  console.log("USDT_New_Loan_Retry started");
  const loanAmount = 30000000000;
  const collateralAmount = 20000000000;

  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await loan1.connect(accounts[1]).loanRequest(symbolUsdt, comit_NONE, loanAmount, symbolUsdt, collateralAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("USDT_New_Loan_Retry passed");
}

async function USDT_Add_Collateral() {
  console.log("USDT_Add_Collateral started");
  const collateralAmount = 20000000000;

  assert.equal(await loan1.hasLoanAccount(accounts[1].address), true);

  const loans = await loan1.getLoans(accounts[1].address);

  assert.notEqual(loans, null);

  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();

  tx = await loan.connect(accounts[1]).addCollateral(symbolUsdt, comit_NONE, collateralAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "AddCollateral");

  assert.equal(
    (await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(collateralAmount).toString()),
  );
  console.log("USDT_Add_Collateral passed");
}

async function USDT_Withdraw_Loan() {
  console.log("USDT_Withdraw_Loan started");
  const withdrawAmount = 550000000000;
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  reverted = true;
  try {
    tx = await loan.connect(accounts[1]).withdrawPartialLoan(symbolUsdt, comit_NONE, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("USDT_Withdraw_Loan passed");
}

async function Swap_Loan() {
  console.log("Swap_Loan started");
  // const loanAmount = 30000000000;
  const preLoan = BigNumber.from(30000000000);
  const loanFees = BigNumber.from(preLoan).mul(10).div(10000);
  const loanAmount = preLoan - loanFees; // 0.17 Btc
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  const fees = BigNumber.from(loanAmount).mul(5).div(10000);

  tx = await loan.connect(accounts[1]).swapLoan(symbolUsdt, comit_NONE, symbolCAKE);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[9]["event"], "MarketSwapped");

  const reserveLoanPost = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

  // /// CHECKS FEE
  // assert.equal(
  //   BigNumber.from(reserveBalance).sub(BigNumber.from(loanAmount)).add(BigNumber.from(fees)).toString(),
  //   reserveLoanPost.toString(),
  // );
  // assert.equal(BigNumber.from(await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Swap_Loan passed");
}

async function Swap_to_Loan() {
  console.log("Swap_to_Loan started");
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  const reserveBalanceCake = BigNumber.from(await bepCake.balanceOf(diamondAddress));
  let loanData = await loan1.getLoans(accounts[1].address);
  LoanAmount = BigNumber.from(loanData.loanAmount[0]);
  CurrentLoan = BigNumber.from(loanData.loanCurrentAmount[0]);
  fees = CurrentLoan.mul(5).div(10000);
  tx = await loan.connect(accounts[1]).swapToLoan(symbolUsdt, comit_NONE);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[9]["event"], "MarketSwapped");

  const reserveLoanPost = BigNumber.from(await bepCake.balanceOf(diamondAddress));
  /// CHECKS FEE
  // assert.equal(reserveBalanceCake.sub(CurrentLoan).add(fees).toString(), reserveLoanPost.toString());

  // assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Swap_to_Loan passed");
}

async function USDT_Withdraw_Loan_1() {
  console.log("USDT_Withdraw_Loan_1 started");
  const withdrawAmount = 25000000000;
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

  tx = await loan.connect(accounts[1]).withdrawPartialLoan(symbolUsdt, comit_NONE, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "WithdrawPartialLoan");

  assert.equal(
    (await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(withdrawAmount)).toString(),
  );
  console.log("USDT_Withdraw_Loan_1 passed");
}

async function BTC_New_Loan_1_4_CDR() {
  console.log("BTC_New_Loan_1_4_CDR started");
  const loanAmount = 40000000000;
  const collateralAmount = 10000000000;

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await loan1
      .connect(accounts[1])
      .loanRequest(symbolBtc, comit_ONEMONTH, loanAmount, symbolBtc, collateralAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}

  assert(reverted);

  assert.equal((await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("BTC_New_Loan_1_4_CDR passed");
}

async function Btc_New_Loan_Same_Market() {
  console.log("Btc_New_Loan_Same_Market started");
  const loanAmount = 170000000; // 0.15 Btc
  const collateralAmount = 200000000; // 0.2 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();
  tx = await loan1.connect(accounts[1]).loanRequest(symbolBtc, comit_ONEMONTH, loanAmount, symbolBtc, collateralAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewLoan");

  assert.equal(
    (await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(collateralAmount)),
  );
  console.log("Btc_New_Loan_Same_Market started");
}

async function Btc_New_Loan_Retry() {
  console.log("Btc_New_Loan_Retry started");
  const loanAmount = 300000000;
  const collateralAmount = 200000000;

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await loan1
      .connect(accounts[1])
      .loanRequest(symbolBtc, comit_ONEMONTH, loanAmount, symbolBtc, collateralAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal((await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Btc_New_Loan_Retry passed");
}

async function Btc_Add_Collateral() {
  console.log("Btc_Add_Collateral started");
  const collateralAmount = 15000000; // 0.15 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, collateralAmount);
  await tx.wait();

  tx = await loan.connect(accounts[1]).addCollateral(symbolBtc, comit_ONEMONTH, collateralAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "AddCollateral");

  assert.equal(
    (await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(collateralAmount)).toString(),
  );
  console.log("Btc_Add_Collateral passed");
}

async function BTC_Swap_Loan() {
  console.log("BTC_Swap_Loan started");
  const preLoan = BigNumber.from(17000000);
  const loanFees = BigNumber.from(preLoan).mul(10).div(10000);
  const loanAmount = preLoan - loanFees; // 0.17 Btc
  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  const reserveBal = BigNumber.from(await bepCake.balanceOf(diamondAddress));
  const fees = BigNumber.from(loanAmount).mul(5).div(10000);
  tx = await loan.connect(accounts[1]).swapLoan(symbolBtc, comit_ONEMONTH, symbolCAKE);
  response = await tx.wait();
  assert.equal(response["events"][9]["event"], "MarketSwapped");
  const reserveLoanPost = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  /// CHECKS FEE
  // assert.equal(
  //   BigNumber.from(reserveBalance).sub(BigNumber.from(loanAmount)).add(BigNumber.from(fees)).toString(),
  //   reserveLoanPost.toString(),
  // );

  console.log((await bepBtc.balanceOf(diamondAddress)).toString());
  console.log(reserveBalance.toString());
  assert(BigNumber.from(await bepBtc.balanceOf(diamondAddress)).lt(BigNumber.from(reserveBalance)));
  assert(BigNumber.from(await bepCake.balanceOf(diamondAddress)).gt(BigNumber.from(reserveBal)));
  console.log("BTC_Swap_Loan passed");
}

async function Btc_Swap_Loan_2nd_Attempt() {
  console.log("Btc_Swap_Loan_2nd_Attempt started");
  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  const reserveBal = await bepSxp.balanceOf(diamondAddress);
  reverted = true;
  try {
    tx = await loan.connect(accounts[1]).swapLoan(symbolBtc, comit_ONEMONTH, symbolSxp);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal((await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  assert.equal((await bepSxp.balanceOf(diamondAddress)).toString(), reserveBal.toString());
  console.log("Btc_Swap_Loan_2nd_Attempt passed");
}

async function BTC_Swap_to_Loan() {
  console.log("Swap_to_Loan started");
  const reserveBalance = BigNumber.from(await bepCake.balanceOf(diamondAddress));
  const reserveBal = BigNumber.from(await bepCake.balanceOf(diamondAddress));
  let loanData = await loan1.getLoans(accounts[1].address);
  LoanAmount = BigNumber.from(loanData.loanAmount[1]);
  CurrentLoan = BigNumber.from(loanData.loanCurrentAmount[1]);
  fees = CurrentLoan.mul(5).div(10000);

  tx = await loan.connect(accounts[1]).swapToLoan(symbolBtc, comit_ONEMONTH);
  response = await tx.wait();
  assert.equal(response["events"][9]["event"], "MarketSwapped");
  const reserveLoanPost = BigNumber.from(await bepCake.balanceOf(diamondAddress));
  /// CHECKS FEE
  assert.equal(reserveBalance.sub(CurrentLoan).add(fees).toString(), reserveLoanPost.toString());
  assert(BigNumber.from(await bepBtc.balanceOf(diamondAddress)).gt(reserveBalance));
  assert(BigNumber.from(await bepCake.balanceOf(diamondAddress)).lt(reserveBal));
  console.log("Swap_to_Loan passed");
}

async function Btc_Swap_to_Loan_2nd_Attempt() {
  console.log("Btc_Swap_to_Loan_2nd_Attempt started");
  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  const reserveBal = await bepSxp.balanceOf(diamondAddress);
  reverted = true;
  try {
    tx = await loan.connect(accounts[1]).swapToLoan(symbolBtc, comit_ONEMONTH);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);

  assert.equal((await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  assert.equal((await bepSxp.balanceOf(diamondAddress)), reserveBal.toString());
  console.log("Btc_Swap_to_Loan_2nd_Attempt passed");
}

async function Btc_Withdraw_Loan() {
  console.log("Btc_Withdraw_Loan started");
  const withdrawAmount = 15000000;
  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await loan.connect(accounts[1]).withdrawPartialLoan(symbolBtc, comit_ONEMONTH, withdrawAmount);
  response = await tx.wait();
  assert.equal(response['events'][1]['event'], "WithdrawPartialLoan");

  assert.equal((await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(withdrawAmount)).toString()
  );
  console.log("Btc_Withdraw_Loan passed");
}

async function Btc_Repay_Loan() {
  console.log("Btc_Repay_Loan started");
  const repayAmount = 50000000; // 0.5 BTC
  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, repayAmount);
  await tx.wait();
  tx = await loan2.connect(accounts[1]).repayLoan(symbolBtc, comit_ONEMONTH, repayAmount);
  response = await tx.wait();
  assert.equal(response['events'][2]['event'], "LoanRepaid");

  assert(BigNumber.from(await bepBtc.balanceOf(diamondAddress)).gte(reserveBalance));
  console.log("Btc_Repay_Loan passed");
}


async function main() {
  await before();
  await USDT_New_Loan_1_4_CDR();
  await USDT_New_Loan();
  await USDT_New_Loan_Retry();
  await USDT_Add_Collateral();
  await USDT_Withdraw_Loan();
  await Swap_Loan();
  await Swap_to_Loan();
  await USDT_Withdraw_Loan_1();
  await BTC_New_Loan_1_4_CDR();
  await Btc_New_Loan_Same_Market();
  await Btc_New_Loan_Retry();
  await Btc_Add_Collateral();
  await BTC_Swap_Loan();
  await Btc_Swap_Loan_2nd_Attempt();
  await BTC_Swap_to_Loan();
  await Btc_Swap_to_Loan_2nd_Attempt();
  await Btc_Withdraw_Loan();
  await Btc_Repay_Loan();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });

exports.main = main;
