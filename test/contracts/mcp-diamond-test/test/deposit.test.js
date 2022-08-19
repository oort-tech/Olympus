const { BigNumber } = require("ethers");
const { ethers, waffle } = require("hardhat");
const utils = require("ethers").utils;

const { deployDiamond, provideLiquidity } = require("../scripts/deploy_all.js");
const { addMarkets } = require("../scripts/deploy_all.js");

const assert = require("assert");
const TOKENS_DECIMAL = 8;

let diamondAddress;
let rets;
let tokenList;
let library;
let deposit;
let faucet;
let accessRegistry;
let accounts;

let bepBtc;
let bepUsdc;
let bepUsdt;
let bepWbnb;

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
}

const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.t

const comit_ONEMONTH = utils.formatBytes32String("comit_ONEMONTH");
const comit_TWOWEEKS = utils.formatBytes32String("comit_TWOWEEKS");
const comit_NONE = utils.formatBytes32String("comit_NONE");

async function before_commit() {
  // deploying relevant contracts
  tokenList = await ethers.getContractAt("TokenList", diamondAddress);
  deposit = await ethers.getContractAt("Deposit", diamondAddress);
  accessRegistry = await ethers.getContractAt("AccessRegistry", rets["accessRegistryAddress"]);
  // deploying tokens
  bepUsdt = await ethers.getContractAt("BEP20Token", rets["tUsdtAddress"]);
  bepBtc = await ethers.getContractAt("BEP20Token", rets["tBtcAddress"]);
  bepUsdc = await ethers.getContractAt("BEP20Token", rets["tUsdcAddress"]);
  bepWbnb = await ethers.getContractAt("BEP20Token", rets["tWBNBAddress"]);
}

async function pauseDeposit() {
  console.log("pauseDeposit started");
  const adminDeposit = utils.formatBytes32String("adminDeposit");
  await deposit.pauseDeposit();
  assert.equal(await deposit.isPausedDeposit(), true);

  await deposit.unpauseDeposit();
  assert.equal(await deposit.isPausedDeposit(), false);

  try {
    reverted = true;
    tx = await deposit.connect(accounts[1]).pauseDeposit();
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await accessRegistry.addAdminRole(adminDeposit, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataGranted");

  try {
    reverted = true;
    tx = await accessRegistry.addAdminRole(adminDeposit, accounts[1].address);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  tx = await deposit.connect(accounts[1]).pauseDeposit();
  await tx.wait();
  assert.equal(await deposit.isPausedDeposit(), true);

  tx = await accessRegistry.removeAdminRole(adminDeposit, accounts[1].address);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[0]["event"], "AdminRoleDataRevoked");

  try {
    reverted = true;
    tx = await deposit.connect(accounts[1]).unpauseDeposit();
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);

  assert.equal(await deposit.isPausedDeposit(), true);

  tx = await deposit.unpauseDeposit();
  await tx.wait();
  assert.equal(await deposit.isPausedDeposit(), false);
  console.log("pauseDeposit passed");
}

async function USDT_New_Deposit() {
  console.log("USDT_New_Deposit started");
  const depositAmount = ethers.utils.parseUnits("5000", TOKENS_DECIMAL); // 500 (8-0's) 500 USDT
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();
  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    BigNumber.from(await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDT_New_Deposit passed");
}

async function USDT_Add_to_Deposit() {
  console.log("USDT_Add_to_Deposit started");
  const depositAmount = ethers.utils.parseUnits("5000", TOKENS_DECIMAL);
  const deposits = await deposit.getDeposits(accounts[1].address);

  assert.equal(await deposit.hasAccount(accounts[1].address), true);
  assert.notEqual(deposits, null);
  assert.equal(await deposit.connect(accounts[1]).hasYield(symbolUsdt, comit_NONE), true);
  assert.equal(await deposit.connect(accounts[1]).hasDeposit(symbolUsdt, comit_NONE), true);
  // assert.equal((BigNumber.from(await deposit.getDepositInterest(accounts[1].address, 1)));
  assert.equal((await deposit.getDepositInterest(accounts[1].address, 1)).toString(), "0");
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDT_Add_to_Deposit passed");
}

async function USDT_Minimum_Deposit() {
  console.log("USDT_Minimum_Deposit started");
  const depositAmount = 500000000; // 50 (8-0's) 50 UDST

  const reserveBalance = await bepUsdt.balanceOf(diamondAddress);

  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  try {
    reverted = true;
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_NONE, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString()),
    console.log("USDT_Minimum_Deposit passed");
}

async function Withdraw_USDT() {
  console.log("Withdraw_USDT started");
  const withdrawAmount = 50000000000; // 500 8-0's 500 USDT

  const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

  tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_NONE, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "DepositWithdrawal");
  assert.equal(
    (await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(fees)).sub(BigNumber.from(withdrawAmount)).toString(),
  );
  console.log("Withdraw_USDT passed");
}

async function Withdraw_USDT_more_than_deposited() {
  console.log("Withdraw_USDT_more_than_deposited started");
  const withdrawAmount = 600000000000000; // 600 8-0's 600 USDT
  const reserveBalance = await bepUsdt.balanceOf(diamondAddress);

  try {
    reverted = true;
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_NONE, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_USDT_more_than_deposited passed");
}

async function USDC_New_Deposit() {
  console.log("USDC_New_Deposit started");
  const depositAmount = 500000000000; // 5000 (8-0's) 5000 USDC

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    (await bepUsdc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDC_New_Deposit passed");
}

async function USDC_Add_to_Deposit() {
  console.log("USDC_Add_to_Deposit started");
  const depositAmount = 500000000000; // 5000 (8-0's) 5000 USDC

  const deposits = await deposit.getDeposits(accounts[1].address);
  console.log(deposits);
  assert.equal((await deposit.hasAccount(accounts[1].address)), true);
  assert.notEqual(deposits, null);
  assert.equal((await deposit.connect(accounts[1]).hasYield(symbolUsdc, comit_NONE)), true);
  assert.equal((await deposit.connect(accounts[1]).hasDeposit(symbolUsdc, comit_NONE)), true);
  // assert.equal((BigNumber.from(await deposit.getDepositInterest(accounts[1].address, 1)));
  assert.equal((await deposit.getDepositInterest(accounts[1].address, 1)).toString(), "0");

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDC_Add_to_Deposit passed");
}

async function USDC_Minimum_Deposit() {
  console.log("USDC_Minimum_Deposit started");
  const depositAmount = 500000000; // 50 (8-0's) 50 USDC

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_NONE, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal(
    BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
    BigNumber.from(reserveBalance).toString(),
  );
  console.log("USDC_Minimum_Deposit passed");
}

async function Withdraw_USDC() {
  console.log("Withdraw_USDC started");
  const withdrawAmount = 50000000000; // 500 8-0's 500 USDC

  const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_NONE, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "DepositWithdrawal");

  assert.equal(
    BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(withdrawAmount)).add(BigNumber.from(fees)).toString(),
  );
  console.log("Withdraw_USDC passed");
}

async function Withdraw_USDC_more_than_deposited() {
  console.log("Withdraw_USDC_more_than_deposited started");
  const withdrawAmount = 1000000000000; // 10000 8-0's 10000 USDC

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));
  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_NONE, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_USDC_more_than_deposited passed");
}

async function BTC_New_Deposit() {
  console.log("BTC_New_Deposit started");
  const depositAmount = 200000000; // 2 (8-0's)  2 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    (await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BTC_New_Deposit passed");
}

async function BTC_Add_to_Deposit() {
  console.log("BTC_Add_to_Deposit started");
  const depositAmount = 150000000; // 15 (7-0's) 1.5 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BTC_Add_to_Deposit passed");
}

async function BTC_Minimum_Deposit() {
  console.log("BTC_Minimum_Deposit started");
  const depositAmount = 5000000; // 5 (6-0's) 0.05 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_NONE, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal(
    BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
    BigNumber.from(reserveBalance).toString(),
  );
  console.log("BTC_Minimum_Deposit passed");
}

async function Withdraw_BTC() {
  console.log("Withdraw_BTC started");
  const withdrawAmount = 20000000; // 2 (7-0's)  0.2 BTC

  const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_NONE, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "DepositWithdrawal");

  assert.equal(
    BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(withdrawAmount)).add(BigNumber.from(fees)).toString(),
  );
  console.log("Withdraw_BTC passed");
}

async function Withdraw_BTC_more_than_deposited() {
  console.log("Withdraw_BTC_more_than_deposited started");
  const withdrawAmount = 400000000; // 4 (8-0's)  4 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_NONE, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal(BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_BTC_more_than_deposited passed");
}

async function BNB_New_Deposit() {
  console.log("BNB_New_Deposit started");
  const depositAmount = 300000000; // 3 (8-0's)  3 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

  tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    (await bepWbnb.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BNB_New_Deposit passed");
}

async function BNB_Add_to_Deposit() {
  console.log("BNB_Add_to_Deposit started");
  const depositAmount = 280000000; // 28 (7-0's) 2.8 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

  tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_NONE, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BNB_Add_to_Deposit passed");
}

async function BNB_Minimum_Deposit() {
  console.log("BNB_Minimum_Deposit started");
  const depositAmount = 5000000; // 5 (6-0's) 0.05 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

  tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_NONE, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal(
    BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(),
    BigNumber.from(reserveBalance).toString(),
  );
  console.log("BNB_Minimum_Deposit passed");
}

async function Withdraw_BNB() {
  console.log("Withdraw_BNB started");
  const withdrawAmount = 300000000; // 3 (8-0's)  3 BNB

  const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

  tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolWBNB, comit_NONE, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "DepositWithdrawal");

  assert.equal(
    BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(withdrawAmount)).add(BigNumber.from(fees)).toString(),
  );
  console.log("Withdraw_BNB passed");
}

async function Withdraw_BNB_more_than_deposited() {
  console.log("Withdraw_BNB_more_than_deposited started");
  const withdrawAmount = 500000000; // 5 (8-0's)  5 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));
  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolWBNB, comit_NONE, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal(BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_BNB_more_than_deposited passed");
}

async function USDT_New_Deposit_TWOWEEKS() {
  console.log("USDT_New_Deposit_TWOWEEKS started");
  const depositAmount = ethers.utils.parseUnits("5000", TOKENS_DECIMAL); // 500 (8-0's) 500 USDT
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();
  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    BigNumber.from(await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDT_New_Deposit_TWOWEEKS passed");
}

async function USDT_Add_to_Deposit_TWOWEEKS() {
  console.log("USDT_Add_to_Deposit_TWOWEEKS started");
  const depositAmount = ethers.utils.parseUnits("5000", TOKENS_DECIMAL);
  const deposits = await deposit.getDeposits(accounts[1].address);

  assert.equal(await deposit.hasAccount(accounts[1].address), true);
  assert.notEqual(deposits, null);
  assert.equal(await deposit.connect(accounts[1]).hasYield(symbolUsdt, comit_TWOWEEKS), true);
  assert.equal(await deposit.connect(accounts[1]).hasDeposit(symbolUsdt, comit_TWOWEEKS), true);
  // assert.equal((BigNumber.from(await deposit.getDepositInterest(accounts[1].address, 1)));
  assert.equal((await deposit.getDepositInterest(accounts[1].address, 1)).toString(), "0");
  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDT_Add_to_Deposit_TWOWEEKS passed");
}

async function USDT_Minimum_Deposit_TWOWEEKS() {
  console.log("USDT_Minimum_Deposit_TWOWEEKS started");
  const depositAmount = 500000000; // 50 (8-0's) 50 UDST

  const reserveBalance = await bepUsdt.balanceOf(diamondAddress);

  tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  try {
    reverted = true;
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_TWOWEEKS, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString()),
    console.log("USDT_Minimum_Deposit_TWOWEEKS passed");
}

async function Withdraw_Deposit_TWOWEEKS_USDT() {
  console.log("Withdraw_Deposit_TWOWEEKS_USDT started");
  const withdrawAmount = 50000000000; // 500 8-0's 500 USDT

  const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

  tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_TWOWEEKS, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  console.log(events);
  assert.equal(events[1]["event"], "DepositWithdrawal");
  assert.equal(
    (await bepUsdt.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(fees)).sub(BigNumber.from(withdrawAmount)).toString(),
  );
  console.log("Withdraw_Deposit_TWOWEEKS_USDT passed");
}

async function Withdraw_Deposit_TWOWEEKS_USDT_more_than_deposited() {
  console.log("Withdraw_Deposit_TWOWEEKS_USDT_more_than_deposited started");
  const withdrawAmount = 600000000000000; // 600 8-0's 600 USDT
  const reserveBalance = await bepUsdt.balanceOf(diamondAddress);

  try {
    reverted = true;
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_TWOWEEKS, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_Deposit_TWOWEEKS_USDT_more_than_deposited passed");
}

async function USDC_New_Deposit_TWOWEEKS() {
  console.log("USDC_New_Deposit_TWOWEEKS started");
  const depositAmount = 500000000000; // 5000 (8-0's) 5000 USDC

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    (await bepUsdc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDC_New_Deposit_TWOWEEKS passed");
}

async function USDC_Add_to_Deposit_TWOWEEKS() {
  console.log("USDC_Add_to_Deposit_TWOWEEKS started");
  const depositAmount = 500000000000; // 5000 (8-0's) 5000 USDC

  const deposits = await deposit.getDeposits(accounts[1].address);
  console.log(deposits);
  assert.equal((await deposit.hasAccount(accounts[1].address)), true);
  assert.notEqual(deposits, null);
  assert.equal((await deposit.connect(accounts[1]).hasYield(symbolUsdc, comit_TWOWEEKS)), true);
  assert.equal((await deposit.connect(accounts[1]).hasDeposit(symbolUsdc, comit_TWOWEEKS)), true);
  // assert.equal((BigNumber.from(await deposit.getDepositInterest(accounts[1].address, 1)));
  assert.equal((await deposit.getDepositInterest(accounts[1].address, 1)).toString(), "0");

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("USDC_Add_to_Deposit_TWOWEEKS passed");
}

async function USDC_Minimum_Deposit_TWOWEEKS() {
  console.log("USDC_Minimum_Deposit_TWOWEEKS started");
  const depositAmount = 500000000; // 50 (8-0's) 50 USDC

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_TWOWEEKS, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal(
    BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
    BigNumber.from(reserveBalance).toString(),
  );
  console.log("USDC_Minimum_Deposit_TWOWEEKS passed");
}

async function Withdraw_Deposit_TWOWEEKS_USDC() {
  console.log("Withdraw_Deposit_TWOWEEKS_USDC started");
  const withdrawAmount = 50000000000; // 500 8-0's 500 USDC

  const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

  tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_TWOWEEKS, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "DepositWithdrawal");

  assert.equal(
    BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(withdrawAmount)).add(BigNumber.from(fees)).toString(),
  );
  console.log("Withdraw_Deposit_TWOWEEKS_USDC passed");
}

async function Withdraw_Deposit_TWOWEEKS_USDC_more_than_deposited() {
  console.log("Withdraw_Deposit_TWOWEEKS_USDC_more_than_deposited started");
  const withdrawAmount = 10000000000000; // 100000 8-0's 100000 USDC

  const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));
  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_TWOWEEKS, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_Deposit_TWOWEEKS_USDC_more_than_deposited passed");
}

async function BTC_New_Deposit_TWOWEEKS() {
  console.log("BTC_New_Deposit_TWOWEEKS started");
  const depositAmount = 200000000; // 2 (8-0's)  2 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    (await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BTC_New_Deposit_TWOWEEKS passed");
}

async function BTC_Add_to_Deposit_TWOWEEKS() {
  console.log("BTC_Add_to_Deposit_TWOWEEKS started");
  const depositAmount = 150000000; // 15 (7-0's) 1.5 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BTC_Add_to_Deposit_TWOWEEKS passed");
}

async function BTC_Minimum_Deposit_TWOWEEKS() {
  console.log("BTC_Minimum_Deposit_TWOWEEKS started");
  const depositAmount = 5000000; // 5 (6-0's) 0.05 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_TWOWEEKS, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal(
    BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
    BigNumber.from(reserveBalance).toString(),
  );
  console.log("BTC_Minimum_Deposit_TWOWEEKS passed");
}

async function Withdraw_Deposit_TWOWEEKS_BTC() {
  console.log("Withdraw_Deposit_TWOWEEKS_BTC started");
  const withdrawAmount = 20000000; // 2 (7-0's)  0.2 BTC

  const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

  tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_TWOWEEKS, withdrawAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[1]["event"], "DepositWithdrawal");

  assert.equal(
    BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
    reserveBalance.sub(BigNumber.from(withdrawAmount)).add(BigNumber.from(fees)).toString(),
  );
  console.log("Withdraw_Deposit_TWOWEEKS_BTC passed");
}

async function Withdraw_Deposit_TWOWEEKS_BTC_more_than_deposited() {
  console.log("Withdraw_Deposit_TWOWEEKS_BTC_more_than_deposited started");
  const withdrawAmount = 400000000; // 4 (8-0's)  4 BTC

  const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_TWOWEEKS, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal(BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_Deposit_TWOWEEKS_BTC_more_than_deposited passed");
}

async function BNB_New_Deposit_TWOWEEKS() {
  console.log("BNB_New_Deposit_TWOWEEKS started");
  const depositAmount = 300000000; // 3 (8-0's)  3 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

  tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "NewDeposit");

  assert.equal(
    (await bepWbnb.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BNB_New_Deposit_TWOWEEKS passed");
}

async function BNB_Add_to_Deposit_TWOWEEKS() {
  console.log("BNB_Add_to_Deposit_TWOWEEKS started");
  const depositAmount = 280000000; // 28 (7-0's) 2.8 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

  tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_TWOWEEKS, depositAmount);
  response = await tx.wait();
  events = response["events"];
  assert.equal(events[2]["event"], "DepositAdded");

  assert.equal(
    BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(),
    reserveBalance.add(BigNumber.from(depositAmount)).toString(),
  );
  console.log("BNB_Add_to_Deposit_TWOWEEKS passed");
}

async function BNB_Minimum_Deposit_TWOWEEKS() {
  console.log("BNB_Minimum_Deposit_TWOWEEKS started");
  const depositAmount = 5000000; // 5 (6-0's) 0.05 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

  tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
  await tx.wait();

  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_TWOWEEKS, depositAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {}
  assert(reverted);
  assert.equal(
    BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(),
    BigNumber.from(reserveBalance).toString(),
  );
  console.log("BNB_Minimum_Deposit_TWOWEEKS passed");
}


async function Withdraw_Deposit_TWOWEEKS_BNB_more_than_deposited() {
  console.log("Withdraw_Deposit_TWOWEEKS_BNB_more_than_deposited started");
  const withdrawAmount = 900000000; // 9 (8-0's)  9 BNB

  const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));
  reverted = true;
  try {
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolWBNB, comit_TWOWEEKS, withdrawAmount);
    await tx.wait();
    reverted = false;
  } catch (error) {
    
  }
  assert(reverted);
  assert.equal(BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
  console.log("Withdraw_Deposit_TWOWEEKS_BNB_more_than_deposited passed");
}
// describe("Testing Deposit", async () => {

//   describe("Test: Deposit (Commit One Month)", async () => {
//     // USDT Deposits
//     it("USDT New Deposit", async () => {
//       const depositAmount = 50000000000; // 500 (8-0's) 500 USDT

//       const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

//       await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "NewDeposit",
//       );

//       expect(BigNumber.from(await bepUsdt.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("USDT Add to Deposit", async () => {
//       const depositAmount = 50000000000; // 500 (8-0's) 500 USDT

//       const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

//       await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "DepositAdded",
//       );

//       expect(BigNumber.from(await bepUsdt.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("USDT Minimum Deposit", async () => {
//       const depositAmount = 500000000; // 50 (8-0's) 50 UDST

//       const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

//       await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_ONEMONTH, depositAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepUsdt.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });

//     // skipped because timelock for commitment yet to be implemented
//     it.skip("Withdraw USDT", async () => {
//       const withdrawAmount = 50000000000; // 500 8-0's 500 USDT
//       const currentProvider = waffle.provider;
//       const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       const timeInSeconds = 30 * 86400 + 20;
//       await currentProvider.send("evm_increaseTime", [timeInSeconds]);
//       await currentProvider.send("evm_mine");

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_ONEMONTH, withdrawAmount)).emit(
//         deposit,
//         "DepositWithdrawal",
//       );

//       expect(BigNumber.from(await bepUsdt.balanceOf(diamondAddress)), "Reserve Balance unequal").to.lte(
//         reserveBalance.sub(BigNumber.from(withdrawAmount)),
//       );
//     });

//     it("Withdraw USDT(more than deposited)", async () => {
//       const withdrawAmount = 600000000000; // 600 8-0's 600 USDT

//       const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepUsdt.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });

//     // USDC Deposits
//     it("USDC New Deposit", async () => {
//       const depositAmount = 50000000000; // 500 (8-0's) 500 USDC

//       const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

//       await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "NewDeposit",
//       );

//       expect(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("USDC Add to Deposit", async () => {
//       const depositAmount = 50000000000; // 500 (8-0's) 500 USDC

//       const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

//       await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "DepositAdded",
//       );

//       expect(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("USDC Minimum Deposit", async () => {
//       const depositAmount = 500000000; // 50 (8-0's) 50 USDC

//       const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

//       await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_ONEMONTH, depositAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });

//     // skipped because timelock for commitment yet to be implemented
//     it.skip("Withdraw USDC", async () => {
//       const withdrawAmount = 50000000000; // 500 8-0's 500 USDC

//       const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance,
//       );
//     });

//     it("Withdraw USDC(more than deposited)", async () => {
//       const withdrawAmount = 600000000000; // 6000 8-0's 600 USDC

//       const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });

//     // BTC Deposits
//     it("BTC New Deposit", async () => {
//       const depositAmount = 20000000; // 2 (7-0's)  0.2 BTC

//       const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

//       await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "NewDeposit",
//       );

//       expect(BigNumber.from(await bepBtc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("BTC Add to Deposit", async () => {
//       const depositAmount = 15000000; // 15 (6-0's) 0.15 BTC

//       const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

//       await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "DepositAdded",
//       );

//       expect(BigNumber.from(await bepBtc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("BTC Minimum Deposit", async () => {
//       const depositAmount = 5000000; // 5 (6-0's) 0.05 BTC

//       const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

//       await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_ONEMONTH, depositAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepBtc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });

//     // skipped because timelock for commitment yet to be implemented
//     it.skip("Withdraw BTC", async () => {
//       const withdrawAmount = 20000000; // 2 (7-0's)  0.2 BTC

//       const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepBtc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance,
//       );
//     });

//     it("Withdraw BTC(more than deposited)", async () => {
//       const withdrawAmount = 200000000; // 2 (8-0's)  2 BTC

//       const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepBtc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });

//     // BNB Deposits
//     it("BNB New Deposit", async () => {
//       const depositAmount = 30000000; // 3 (7-0's)  0.3 BNB

//       const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

//       await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "NewDeposit",
//       );

//       expect(BigNumber.from(await bepWbnb.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("BNB Add to Deposit", async () => {
//       const depositAmount = 28000000; // 28 (6-0's) 0.28 BTC

//       const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

//       await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_ONEMONTH, depositAmount)).emit(
//         deposit,
//         "DepositAdded",
//       );

//       expect(BigNumber.from(await bepWbnb.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.add(BigNumber.from(depositAmount)),
//       );
//     });

//     it("BTC Minimum Deposit", async () => {
//       const depositAmount = 5000000; // 5 (6-0's) 0.05 BNB

//       const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

//       await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);

//       await expect(deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_ONEMONTH, depositAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepWbnb.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });

//     // skipped because timelock for commitment yet to be implemented
//     it.skip("Withdraw BNB", async () => {
//       const withdrawAmount = 30000000; // 3 (7-0's)  0.3 BNB

//       const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolWBNB, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepWbnb.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         reserveBalance.sub(BigNumber.from(withdrawAmount)),
//       );
//     });

//     it("Withdraw BNB(more than deposited)", async () => {
//       const withdrawAmount = 300000000; // 3 (8-0's)  3 BNB

//       const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));

//       await expect(deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_ONEMONTH, withdrawAmount)).to.be
//         .reverted;

//       expect(BigNumber.from(await bepBtc.balanceOf(diamondAddress)), "Reserve Balance unequal").to.equal(
//         BigNumber.from(reserveBalance),
//       );
//     });
//   });
// });

async function main() {
  await before();
  await before_commit();
  await pauseDeposit();
  await USDT_New_Deposit();
  await USDT_Add_to_Deposit();
  await USDT_Minimum_Deposit();
  await Withdraw_USDT();
  await Withdraw_USDT_more_than_deposited();
  await USDC_New_Deposit();
  await USDC_Add_to_Deposit();
  await USDC_Minimum_Deposit();
  await Withdraw_USDC();
  await Withdraw_USDC_more_than_deposited();
  await BTC_New_Deposit();
  await BTC_Add_to_Deposit();
  await BTC_Minimum_Deposit();
  await Withdraw_BTC();
  await Withdraw_BTC_more_than_deposited();
  await BNB_New_Deposit();
  await BNB_Add_to_Deposit();
  await BNB_Minimum_Deposit();
  await Withdraw_BNB();
  await Withdraw_BNB_more_than_deposited();

  await before();
  await before_commit();
  await USDT_New_Deposit_TWOWEEKS();
  await USDT_Add_to_Deposit_TWOWEEKS();
  await USDT_Minimum_Deposit_TWOWEEKS();
  await Withdraw_Deposit_TWOWEEKS_USDT_more_than_deposited();
  await USDC_New_Deposit_TWOWEEKS();
  await USDC_Add_to_Deposit_TWOWEEKS();
  await USDC_Minimum_Deposit_TWOWEEKS();
  await Withdraw_Deposit_TWOWEEKS_USDC_more_than_deposited();
  await BTC_New_Deposit_TWOWEEKS();
  await BTC_Add_to_Deposit_TWOWEEKS();
  await BTC_Minimum_Deposit_TWOWEEKS();
  await Withdraw_Deposit_TWOWEEKS_BTC_more_than_deposited();
  await BNB_New_Deposit_TWOWEEKS();
  await BNB_Add_to_Deposit_TWOWEEKS();
  await BNB_Minimum_Deposit_TWOWEEKS();
  await Withdraw_Deposit_TWOWEEKS_BNB_more_than_deposited();

  async function USDT_New_Deposit_ONEMONTH() {
    console.log("USDT_New_Deposit_ONEMONTH started");
    const depositAmount = ethers.utils.parseUnits("5000", TOKENS_DECIMAL); // 500 (8-0's) 500 USDT
    const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
    tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "NewDeposit");
  
    assert.equal(
      BigNumber.from(await bepUsdt.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("USDT_New_Deposit_ONEMONTH passed");
  }
  
  async function USDT_Add_to_Deposit_ONEMONTH() {
    console.log("USDT_Add_to_Deposit_ONEMONTH started");
    const depositAmount = ethers.utils.parseUnits("5000", TOKENS_DECIMAL);
    const deposits = await deposit.getDeposits(accounts[1].address);
  
    assert.equal(await deposit.hasAccount(accounts[1].address), true);
    assert.notEqual(deposits, null);
    assert.equal(await deposit.connect(accounts[1]).hasYield(symbolUsdt, comit_ONEMONTH), true);
    assert.equal(await deposit.connect(accounts[1]).hasDeposit(symbolUsdt, comit_ONEMONTH), true);
    // assert.equal((BigNumber.from(await deposit.getDepositInterest(accounts[1].address, 1)));
    assert.equal((await deposit.getDepositInterest(accounts[1].address, 1)).toString(), "0");
    const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  
    tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "DepositAdded");
  
    assert.equal(
      BigNumber.from(await bepUsdt.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("USDT_Add_to_Deposit_ONEMONTH passed");
  }
  
  async function USDT_Minimum_Deposit_ONEMONTH() {
    console.log("USDT_Minimum_Deposit_ONEMONTH started");
    const depositAmount = 500000000; // 50 (8-0's) 50 UDST
  
    const reserveBalance = await bepUsdt.balanceOf(diamondAddress);
  
    tx = await bepUsdt.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    try {
      reverted = true;
      tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdt, comit_ONEMONTH, depositAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {}
    assert(reverted);
    assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString()),
      console.log("USDT_Minimum_Deposit_ONEMONTH passed");
  }
  
  async function Withdraw_Deposit_ONEMONTH_USDT() {
    console.log("Withdraw_Deposit_ONEMONTH_USDT started");
    const withdrawAmount = 50000000000; // 500 8-0's 500 USDT
  
    const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);
  
    const reserveBalance = BigNumber.from(await bepUsdt.balanceOf(diamondAddress));
  
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_ONEMONTH, withdrawAmount);
    response = await tx.wait();
    events = response["events"];
    console.log(events);
    assert.equal(events[1]["event"], "DepositWithdrawal");
    assert.equal(
      (await bepUsdt.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(fees)).sub(BigNumber.from(withdrawAmount)).toString(),
    );
    console.log("Withdraw_Deposit_ONEMONTH_USDT passed");
  }
  
  async function Withdraw_Deposit_ONEMONTH_USDT_more_than_deposited() {
    console.log("Withdraw_Deposit_ONEMONTH_USDT_more_than_deposited started");
    const withdrawAmount = 600000000000000; // 600 8-0's 600 USDT
    const reserveBalance = await bepUsdt.balanceOf(diamondAddress);
  
    try {
      reverted = true;
      tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdt, comit_ONEMONTH, withdrawAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {}
    assert(reverted);
    assert.equal((await bepUsdt.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
    console.log("Withdraw_Deposit_ONEMONTH_USDT_more_than_deposited passed");
  }
  
  async function USDC_New_Deposit_ONEMONTH() {
    console.log("USDC_New_Deposit_ONEMONTH started");
    const depositAmount = 500000000000; // 5000 (8-0's) 5000 USDC
  
    const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));
  
    tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "NewDeposit");
  
    assert.equal(
      (await bepUsdc.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("USDC_New_Deposit_ONEMONTH passed");
  }
  
  async function USDC_Add_to_Deposit_ONEMONTH() {
    console.log("USDC_Add_to_Deposit_ONEMONTH started");
    const depositAmount = 500000000000; // 5000 (8-0's) 5000 USDC
  
    const deposits = await deposit.getDeposits(accounts[1].address);
    console.log(deposits);
    assert.equal((await deposit.hasAccount(accounts[1].address)), true);
    assert.notEqual(deposits, null);
    assert.equal((await deposit.connect(accounts[1]).hasYield(symbolUsdc, comit_ONEMONTH)), true);
    assert.equal((await deposit.connect(accounts[1]).hasDeposit(symbolUsdc, comit_ONEMONTH)), true);
    // assert.equal((BigNumber.from(await deposit.getDepositInterest(accounts[1].address, 1)));
    assert.equal((await deposit.getDepositInterest(accounts[1].address, 1)).toString(), "0");
  
    const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));
  
    tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "DepositAdded");
  
    assert.equal(
      BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("USDC_Add_to_Deposit_ONEMONTH passed");
  }
  
  async function USDC_Minimum_Deposit_ONEMONTH() {
    console.log("USDC_Minimum_Deposit_ONEMONTH started");
    const depositAmount = 500000000; // 50 (8-0's) 50 USDC
  
    const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));
  
    tx = await bepUsdc.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    reverted = true;
    try {
      tx = await deposit.connect(accounts[1]).depositRequest(symbolUsdc, comit_ONEMONTH, depositAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {}
    assert(reverted);
    assert.equal(
      BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
      BigNumber.from(reserveBalance).toString(),
    );
    console.log("USDC_Minimum_Deposit_ONEMONTH passed");
  }
  
  async function Withdraw_Deposit_ONEMONTH_USDC() {
    console.log("Withdraw_Deposit_ONEMONTH_USDC started");
    const withdrawAmount = 50000000000; // 500 8-0's 500 USDC
  
    const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);
  
    const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));
  
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_ONEMONTH, withdrawAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[1]["event"], "DepositWithdrawal");
  
    assert.equal(
      BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(),
      reserveBalance.sub(BigNumber.from(withdrawAmount)).add(BigNumber.from(fees)).toString(),
    );
    console.log("Withdraw_Deposit_ONEMONTH_USDC passed");
  }
  
  async function Withdraw_Deposit_ONEMONTH_USDC_more_than_deposited() {
    console.log("Withdraw_Deposit_ONEMONTH_USDC_more_than_deposited started");
    const withdrawAmount = 10000000000000; // 100000 8-0's 100000 USDC
  
    const reserveBalance = BigNumber.from(await bepUsdc.balanceOf(diamondAddress));
    reverted = true;
    try {
      tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolUsdc, comit_ONEMONTH, withdrawAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {
      
    }
    assert(reverted);
    assert.equal(BigNumber.from(await bepUsdc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
    console.log("Withdraw_Deposit_ONEMONTH_USDC_more_than_deposited passed");
  }
  
  async function BTC_New_Deposit_ONEMONTH() {
    console.log("BTC_New_Deposit_ONEMONTH started");
    const depositAmount = 200000000; // 2 (8-0's)  2 BTC
  
    const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  
    tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "NewDeposit");
  
    assert.equal(
      (await bepBtc.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("BTC_New_Deposit_ONEMONTH passed");
  }
  
  async function BTC_Add_to_Deposit_ONEMONTH() {
    console.log("BTC_Add_to_Deposit_ONEMONTH started");
    const depositAmount = 150000000; // 15 (7-0's) 1.5 BTC
  
    const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  
    tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "DepositAdded");
  
    assert.equal(
      BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("BTC_Add_to_Deposit_ONEMONTH passed");
  }
  
  async function BTC_Minimum_Deposit_ONEMONTH() {
    console.log("BTC_Minimum_Deposit_ONEMONTH started");
    const depositAmount = 5000000; // 5 (6-0's) 0.05 BTC
  
    const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  
    tx = await bepBtc.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    reverted = true;
    try {
      tx = await deposit.connect(accounts[1]).depositRequest(symbolBtc, comit_ONEMONTH, depositAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {}
    assert(reverted);
    assert.equal(
      BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
      BigNumber.from(reserveBalance).toString(),
    );
    console.log("BTC_Minimum_Deposit_ONEMONTH passed");
  }
  
  async function Withdraw_Deposit_ONEMONTH_BTC() {
    console.log("Withdraw_Deposit_ONEMONTH_BTC started");
    const withdrawAmount = 20000000; // 2 (7-0's)  0.2 BTC
  
    const fees = BigNumber.from(withdrawAmount).mul(10).div(10000);
  
    const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
  
    tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_ONEMONTH, withdrawAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[1]["event"], "DepositWithdrawal");
  
    assert.equal(
      BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(),
      reserveBalance.sub(BigNumber.from(withdrawAmount)).add(BigNumber.from(fees)).toString(),
    );
    console.log("Withdraw_Deposit_ONEMONTH_BTC passed");
  }
  
  async function Withdraw_Deposit_ONEMONTH_BTC_more_than_deposited() {
    console.log("Withdraw_Deposit_ONEMONTH_BTC_more_than_deposited started");
    const withdrawAmount = 400000000; // 4 (8-0's)  4 BTC
  
    const reserveBalance = BigNumber.from(await bepBtc.balanceOf(diamondAddress));
    reverted = true;
    try {
      tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolBtc, comit_ONEMONTH, withdrawAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {
      
    }
    assert(reverted);
    assert.equal(BigNumber.from(await bepBtc.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
    console.log("Withdraw_Deposit_ONEMONTH_BTC_more_than_deposited passed");
  }
  
  async function BNB_New_Deposit_ONEMONTH() {
    console.log("BNB_New_Deposit_ONEMONTH started");
    const depositAmount = 300000000; // 3 (8-0's)  3 BNB
  
    const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));
  
    tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "NewDeposit");
  
    assert.equal(
      (await bepWbnb.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("BNB_New_Deposit_ONEMONTH passed");
  }
  
  async function BNB_Add_to_Deposit_ONEMONTH() {
    console.log("BNB_Add_to_Deposit_ONEMONTH started");
    const depositAmount = 280000000; // 28 (7-0's) 2.8 BNB
  
    const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));
  
    tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_ONEMONTH, depositAmount);
    response = await tx.wait();
    events = response["events"];
    assert.equal(events[2]["event"], "DepositAdded");
  
    assert.equal(
      BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(),
      reserveBalance.add(BigNumber.from(depositAmount)).toString(),
    );
    console.log("BNB_Add_to_Deposit_ONEMONTH passed");
  }
  
  async function BNB_Minimum_Deposit_ONEMONTH() {
    console.log("BNB_Minimum_Deposit_ONEMONTH started");
    const depositAmount = 5000000; // 5 (6-0's) 0.05 BNB
  
    const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));
  
    tx = await bepWbnb.connect(accounts[1]).approve(diamondAddress, depositAmount);
    await tx.wait();
  
    reverted = true;
    try {
      tx = await deposit.connect(accounts[1]).depositRequest(symbolWBNB, comit_ONEMONTH, depositAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {}
    assert(reverted);
    assert.equal(
      BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(),
      BigNumber.from(reserveBalance).toString(),
    );
    console.log("BNB_Minimum_Deposit_ONEMONTH passed");
  }
  
  
  async function Withdraw_Deposit_ONEMONTH_BNB_more_than_deposited() {
    console.log("Withdraw_Deposit_ONEMONTH_BNB_more_than_deposited started");
    const withdrawAmount = 900000000; // 9 (8-0's)  9 BNB
  
    const reserveBalance = BigNumber.from(await bepWbnb.balanceOf(diamondAddress));
    reverted = true;
    try {
      tx = await deposit.connect(accounts[1]).withdrawDeposit(symbolWBNB, comit_ONEMONTH, withdrawAmount);
      await tx.wait();
      reverted = false;
    } catch (error) {
      
    }
    assert(reverted);
    assert.equal(BigNumber.from(await bepWbnb.balanceOf(diamondAddress)).toString(), reserveBalance.toString());
    console.log("Withdraw_Deposit_ONEMONTH_BNB_more_than_deposited passed");
  }

  await before();
  await before_commit();
  await USDT_New_Deposit_ONEMONTH();
  await USDT_Add_to_Deposit_ONEMONTH();
  await USDT_Minimum_Deposit_ONEMONTH();
  await Withdraw_Deposit_ONEMONTH_USDT_more_than_deposited();
  await USDC_New_Deposit_ONEMONTH();
  await USDC_Add_to_Deposit_ONEMONTH();
  await USDC_Minimum_Deposit_ONEMONTH();
  await Withdraw_Deposit_ONEMONTH_USDC_more_than_deposited();
  await BTC_New_Deposit_ONEMONTH();
  await BTC_Add_to_Deposit_ONEMONTH();
  await BTC_Minimum_Deposit_ONEMONTH();
  await Withdraw_Deposit_ONEMONTH_BTC_more_than_deposited();
  await BNB_New_Deposit_ONEMONTH();
  await BNB_Add_to_Deposit_ONEMONTH();
  await BNB_Minimum_Deposit_ONEMONTH();
  await Withdraw_Deposit_ONEMONTH_BNB_more_than_deposited();
}

// main()
//   .then(() => process.exit(0))
//   .catch(error => {
//     console.error(error);
//     process.exit(1);
//   });
exports.main = main;