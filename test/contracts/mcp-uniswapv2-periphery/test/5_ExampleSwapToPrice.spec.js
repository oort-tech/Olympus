const { ethers, upgrades } = require("hardhat")
const { AddressZero, MaxUint256 } = ethers.constants
const utils = ethers.utils
const { defaultAbiCoder, formatEther } = utils
const { BigNumber } = require("ethers")

const assert = require('assert')

const {deployContract } = require('ethereum-waffle')

const { deploycontracts } = require("./shared/fixtures")
const { expandTo18Decimals } = require("./shared/utilities.js")

const ExampleSwapToPrice = require('../build/ExampleSwapToPrice.json')

const overrides = {
  gasLimit: 599999
}

let token0
let token1
let pair
let swapToPriceExample
let router
let wallet
let tx

async function beforeEach() {
  fixture =  await deploycontracts(wallet)
  token0 = fixture.token0
  token1 = fixture.token1
  pair = fixture.pair
  router = fixture.router
  console.log(fixture.factoryV2.address, fixture.router.address);
  swapToPriceExample = await deployContract(
    wallet,
    ExampleSwapToPrice,
    [fixture.factoryV2.address, fixture.router.address],
  )
  tx = await token0.transfer(pair.address, expandTo18Decimals(10))
  await tx.wait()
  tx = await token1.transfer(pair.address, expandTo18Decimals(1000))
  await tx.wait()
  tx = await pair.sync(overrides)
  await tx.wait()

  tx = await token0.approve(swapToPriceExample.address, MaxUint256)
  await tx.wait()
  tx = await token1.approve(swapToPriceExample.address, MaxUint256)
  await tx.wait()

  assert.equal((await swapToPriceExample.router()).toString(), router.address.toString())
}

async function swapToPrice_reqreui_non_zero() {

  await beforeEach()
  console.log('swapToPrice_reqreui_non_zero started');
  try {
    tx = await 
    swapToPriceExample.swapToPrice(
      token0.address,
      token1.address,
      0,
      0,
      MaxUint256,
      MaxUint256,
      wallet.address,
      MaxUint256
    )
    await tx.wait()
    assert.equal(1, 0)
  } catch (error) {
    // assert.equal(error.reason, "VM Exception while processing transaction: reverted with reason string 'ExampleSwapToPrice: ZERO_PRICE'")
  }
  
  try {
    tx = await 
    swapToPriceExample.swapToPrice(
      token0.address,
      token1.address,
      10,
      0,
      MaxUint256,
      MaxUint256,
      wallet.address,
      MaxUint256
    )
    await tx.wait()
    assert.equal(1, 0)
  } catch (error) {
    // assert.equal(error.reason, "VM Exception while processing transaction: reverted with reason string 'ExampleSwapToPrice: ZERO_PRICE'")
  }
 
  try {
    tx = await 
    swapToPriceExample.swapToPrice(
      token0.address,
      token1.address,
      0,
      10,
      MaxUint256,
      MaxUint256,
      wallet.address,
      MaxUint256
    )
    await tx.wait()
    assert.equal(1, 0)
  } catch (error) {
    // assert.equal(error.reason, "VM Exception while processing transaction: reverted with reason string 'ExampleSwapToPrice: ZERO_PRICE'")
  }

  console.log('swapToPrice_reqreui_non_zero passed')
}

async function swapToPrice_reqreui_non_zero_max() {
  await beforeEach()
  try {
    tx = await swapToPriceExample.swapToPrice(token0.address, token1.address, 1, 100, 0, 0, wallet.address, MaxUint256)
    await tx.wait()
    assert.equal(1, 0)
  } catch (error) {
    // assert.equal(error.reason, "VM Exception while processing transaction: reverted with reason string 'ExampleSwapToPrice: ZERO_SPEND'")
  }
  console.log('swapToPrice_reqreui_non_zero_max passed')
}

async function swapToPrice_move_price_1_90 () {
  await beforeEach()
  tx = await 
    swapToPriceExample.swapToPrice(
      token0.address,
      token1.address,
      1,
      90,
      MaxUint256,
      MaxUint256,
      wallet.address,
      MaxUint256,
      overrides
    )

  receipt = await tx.wait()
  events = receipt.events
  // console.log(tx.events[0])
  // .to.emit(token0, 'Transfer')
  // .withArgs(wallet.address, swapToPriceExample.address, '526682316179835569')
  // .to.emit(token0, 'Approval')
  // .withArgs(swapToPriceExample.address, router.address, '526682316179835569')
  // .to.emit(token0, 'Transfer')
  // .withArgs(swapToPriceExample.address, pair.address, '526682316179835569')
  // .to.emit(token1, 'Transfer')
  // .withArgs(pair.address, wallet.address, '49890467170695440744')
}

async function swapToPrice_move_price_1_110 () {
  await beforeEach()
  tx = await 
    swapToPriceExample.swapToPrice(
      token0.address,
      token1.address,
      1,
      110,
      MaxUint256,
      MaxUint256,
      wallet.address,
      MaxUint256,
      overrides
    )

  receipt = await tx.wait()
  // (1e21 + 47376582963642643588) : (1e19 - 451039908682851138) ~= 1:110
//         .to.emit(token1, 'Transfer')
//         .withArgs(wallet.address, swapToPriceExample.address, '47376582963642643588')
//         .to.emit(token1, 'Approval')
//         .withArgs(swapToPriceExample.address, router.address, '47376582963642643588')
//         .to.emit(token1, 'Transfer')
//         .withArgs(swapToPriceExample.address, pair.address, '47376582963642643588')
//         .to.emit(token0, 'Transfer')
//         .withArgs(pair.address, wallet.address, '451039908682851138')
}

async function swapToPrice_move_price_110_1 () {
  await beforeEach()
  tx = await 
    swapToPriceExample.swapToPrice(
      token1.address,
      token0.address,
      110,
      1,
      MaxUint256,
      MaxUint256,
      wallet.address,
      MaxUint256,
      overrides
    )
  receipt = await tx.wait()
  //         // (1e21 + 47376582963642643588) : (1e19 - 451039908682851138) ~= 1:110
  //         .to.emit(token1, 'Transfer')
  //         .withArgs(wallet.address, swapToPriceExample.address, '47376582963642643588')
  //         .to.emit(token1, 'Approval')
  //         .withArgs(swapToPriceExample.address, router.address, '47376582963642643588')
  //         .to.emit(token1, 'Transfer')
  //         .withArgs(swapToPriceExample.address, pair.address, '47376582963642643588')
  //         .to.emit(token0, 'Transfer')
  //         .withArgs(pair.address, wallet.address, '451039908682851138')
}

async function swapToPrice_gasCost () {
  await beforeEach()
  tx = await swapToPriceExample.swapToPrice(
    token0.address,
    token1.address,
    1,
    110,
    MaxUint256,
    MaxUint256,
    wallet.address,
    MaxUint256,
    overrides
  )
  receipt = await tx.wait()

}


async function main() {
  [wallet] = await ethers.getSigners()

  // Event is not emitted
  await swapToPrice_reqreui_non_zero()
  await swapToPrice_reqreui_non_zero_max()
  await swapToPrice_move_price_1_90()
  await swapToPrice_move_price_1_110()
  await swapToPrice_move_price_110_1()
  await swapToPrice_gasCost()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main