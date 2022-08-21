const { ethers, upgrades } = require("hardhat")
const { AddressZero, MaxUint256 } = ethers.constants
const assert = require('assert')
const { deploycontracts } = require("./shared/fixtures")
const { expandTo18Decimals } = require("./shared/utilities.js")


const overrides = {
  gasLimit: 600000
}
let wallet, other
let ExampleComputeLiquidityValue
let fixture
let computeLiquidityValue
let factory
let tx
async function beforeEach() {
  fixture =  await deploycontracts(wallet)
  factory = fixture.factoryV2
  computeLiquidityValue = await ExampleComputeLiquidityValue.deploy(fixture.factoryV2.address)
  tx = await fixture.token0.transfer(fixture.pair.address, expandTo18Decimals(10))
  await tx.wait()
  tx = await fixture.token1.transfer(fixture.pair.address, expandTo18Decimals(1000))
  await tx.wait()
  tx = await fixture.pair.mint(wallet.address, overrides)
  await tx.wait()
  assert.equal((await fixture.pair.totalSupply()).toString(), expandTo18Decimals(100).toString())
  assert.equal((await computeLiquidityValue.factory()), factory.address)
}
  

async function correct_for_5_shares() {
  await beforeEach()
  console.log('correct_for_5_shares started')
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValue(
    fixture.token0.address,
    fixture.token1.address,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '500000000000000000')
  assert.equal(token1Amount.toString(), '50000000000000000000')

  console.log('correct_for_5_shares passed')
}

async function correct_for_7_shares() {
  await beforeEach()
  console.log('correct_for_7_shares started')
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValue(
    fixture.token0.address,
    fixture.token1.address,
    expandTo18Decimals(7)
  )
  assert.equal(token0Amount.toString(), '700000000000000000')
  assert.equal(token1Amount.toString(), '70000000000000000000')
  console.log('correct_for_7_shares passed')
}

async function correct_after_swap () {
  await beforeEach()
  tx = await fixture.token0.approve(fixture.router.address, MaxUint256, overrides)
  await tx.wait()
  tx = await fixture.router.swapExactTokensForTokens(
    expandTo18Decimals(10),
    0,
    [fixture.token0.address, fixture.token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValue(
    fixture.token0.address,
    fixture.token1.address,
    expandTo18Decimals(7)
  )
  assert.equal(token0Amount.toString(), '1400000000000000000')
  assert.equal(token1Amount.toString(), '35052578868302453680')
}


// describe('fee on')
async function beforeEach_fee_on () {
  tx = await factory.setFeeTo(wallet.address)
  await tx.wait()
}

async function beforeEach1_fee_on () {
  tx = await fixture.token0.transfer(fixture.pair.address, expandTo18Decimals(10))
  await tx.wait()
  tx = await fixture.token1.transfer(fixture.pair.address, expandTo18Decimals(1000))
  await tx.wait()
  tx = await fixture.pair.mint(AddressZero, overrides)
  await tx.wait()
  assert.equal((await fixture.pair.totalSupply()).toString(), expandTo18Decimals(200).toString())
}

async function coorect_after_swap_fee_on() {
  await beforeEach()
  await beforeEach_fee_on()
  await beforeEach1_fee_on()

  console.log('coorect_after_swap_fee_on started')
  tx = await fixture.token0.approve(fixture.router.address, MaxUint256, overrides)
  await tx.wait()
  tx = await fixture.router.swapExactTokensForTokens(
    expandTo18Decimals(20),
    0,
    [fixture.token0.address, fixture.token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValue(
    fixture.token0.address,
    fixture.token1.address,
    expandTo18Decimals(7)
  )
  assert.equal(token0Amount.toString(), '1399824934325735058')
  assert.equal(token1Amount.toString(), '35048195651620807684')
  console.log('coorect_after_swap_fee_on ended')
}
 
// describe('#getReservesAfterArbitrage', () => {

async function getReservesAfterArbitrage_1_400() {
  await beforeEach()
  const [reserveA, reserveB] = await computeLiquidityValue.getReservesAfterArbitrage(
    fixture.token0.address,
    fixture.token1.address,
    1,
    400
  )
  assert.equal(reserveA.toString(), '5007516917298542016')
  assert.equal(reserveB.toString(), '1999997739838173075192')

  console.log('getReservesAfterArbitrage_1_400 passed')
}

async function getReservesAfterArbitrage_1_200() {
  await beforeEach()
  const [reserveA, reserveB] = await computeLiquidityValue.getReservesAfterArbitrage(
    fixture.token0.address,
    fixture.token1.address,
    1,
    200
  )
  assert.equal(reserveA.toString(), '7081698338256310291')
  assert.equal(reserveB.toString(), '1413330640570018326894')

  console.log('getReservesAfterArbitrage_1_200 passed')
}

async function getReservesAfterArbitrage_1_100() {
  await beforeEach()
  const [reserveA, reserveB] = await computeLiquidityValue.getReservesAfterArbitrage(
    fixture.token0.address,
    fixture.token1.address,
    1,
    100
  )
  assert.equal(reserveA.toString(), '10000000000000000000')
  assert.equal(reserveB.toString(), '1000000000000000000000')

  console.log('getReservesAfterArbitrage_1_100 passed')
}

async function getReservesAfterArbitrage_1_50() {
  await beforeEach()
  const [reserveA, reserveB] = await computeLiquidityValue.getReservesAfterArbitrage(
    fixture.token0.address,
    fixture.token1.address,
    1,
    50
  )
  assert.equal(reserveA.toString(), '14133306405700183269')
  assert.equal(reserveB.toString(), '708169833825631029041')

  console.log('getReservesAfterArbitrage_1_50 passed')
}

async function getReservesAfterArbitrage_1_25() {
  await beforeEach()
  const [reserveA, reserveB] = await computeLiquidityValue.getReservesAfterArbitrage(
    fixture.token0.address,
    fixture.token1.address,
    1,
    25
  )
  assert.equal(reserveA.toString(), '19999977398381730752')
  assert.equal(reserveB.toString(), '500751691729854201595')

  console.log('getReservesAfterArbitrage_1_25 passed')
}

async function getReservesAfterArbitrage_25_1() {
  await beforeEach()
  const [reserveA, reserveB] = await computeLiquidityValue.getReservesAfterArbitrage(
    fixture.token0.address,
    fixture.token1.address,
    25,
    1
  )
  assert.equal(reserveA.toString(), '500721601459041764285')
  assert.equal(reserveB.toString(), '20030067669194168064')

  console.log('getReservesAfterArbitrage_25_1 passed')
}

async function getReservesAfterArbitrage_large() {
  await beforeEach()
  const [reserveA, reserveB] = await computeLiquidityValue.getReservesAfterArbitrage(
    fixture.token0.address,
    fixture.token1.address,
    MaxUint256.div(1000),
    MaxUint256.div(1000)
  )
  assert.equal(reserveA.toString(), '100120248075158403008')
  assert.equal(reserveB.toString(), '100150338345970840319')

  console.log('getReservesAfterArbitrage_large passed')
}

// describe('#getLiquidityValue', () => {

async function getLiquidityValue_fee_off_1_105 () {
  await beforeEach()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    105,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '488683612488266114') // slightly less than 5% of 10, or 0.5
  assert.equal(token1Amount.toString(), '51161327957205755422') // slightly more than 5% of 100, or 5

  console.log('getLiquidityValue_fee_off_1_105 passed')
}

async function getLiquidityValue_fee_off_1_95 () {
  await beforeEach()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    95,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '512255881944227034') // slightly more than 5% of 10, or 0.5
  assert.equal(token1Amount.toString(), '48807237571060645526') // slightly less than 5% of 100, or 5

  console.log('getLiquidityValue_fee_off_1_95 passed')
}



async function beforeEach_getLiquidityValue_fee_off_swap () {
  tx = await fixture.token0.approve(fixture.router.address, MaxUint256, overrides)
  await tx.wait()
  tx = await fixture.router.swapExactTokensForTokens(
    expandTo18Decimals(10),
    0,
    [fixture.token0.address, fixture.token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  const [reserve0, reserve1] = await fixture.pair.getReserves()
  assert.equal(reserve0.toString(),'20000000000000000000')
  assert.equal(reserve1.toString(),'500751126690035052579') // half plus the fee
}

async function getLiquidityValue_fee_off_1_25_liquidity() {
  await beforeEach()
  await beforeEach_getLiquidityValue_fee_off_swap()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    25,
    expandTo18Decimals(5)
  )

  assert.equal(token0Amount.toString(), '1000000000000000000')
  assert.equal(token1Amount.toString(), '25037556334501752628')

  console.log('getLiquidityValue_fee_off_1_25_liquidity passed')
}

async function getLiquidityValue_fee_off_arbing_1_100() {
  await beforeEach()
  await beforeEach_getLiquidityValue_fee_off_swap()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    100,
    expandTo18Decimals(5)
  )

  assert.equal(token0Amount.toString(), '501127678536722155')
  assert.equal(token1Amount.toString(), '50037429168613534246')

  console.log('getLiquidityValue_fee_off_arbing_1_100 passed')
}

//   describe('fee is on', () => {
async function beforeEach_getLiquidityValue_fee_on () {
  tx = await factory.setFeeTo(wallet.address)
  await tx.wait()
}

async function beforeEach_getLiquidityValue_fee_on_mint () {
  tx = await fixture.token0.transfer(fixture.pair.address, expandTo18Decimals(10))
  await tx.wait()
  tx = await fixture.token1.transfer(fixture.pair.address, expandTo18Decimals(1000))
  await tx.wait()
  tx = await fixture.pair.mint(AddressZero, overrides)
  await tx.wait()
  assert.equal((await fixture.pair.totalSupply()).toString(), expandTo18Decimals(200).toString())
}

async function getLiquidityValue_fee_on_arbing_1_105 () {
  await beforeEach()
  await beforeEach_getLiquidityValue_fee_on()
  await beforeEach_getLiquidityValue_fee_on_mint()

  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    105,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '488680839243189328') // slightly less than 5% of 10, or 0.5
  assert.equal(token1Amount.toString(), '51161037620273529068') // slightly more than 5% of 100, or 5

  console.log('getLiquidityValue_fee_on_arbing_1_105 passed')
}

async function getLiquidityValue_fee_on_arbing_1_95 () {
  await beforeEach()
  await beforeEach_getLiquidityValue_fee_on()
  await beforeEach_getLiquidityValue_fee_on_mint()

  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    95,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '512252817918759166') // slightly more than 5% of 10, or 0.5
  assert.equal(token1Amount.toString(), '48806945633721895174') // slightly less than 5% of 100, or 5

  console.log('getLiquidityValue_fee_on_arbing_1_95 passed')
}

async function getLiquidityValue_fee_on_arbing_1_100 () {
  await beforeEach()
  await beforeEach_getLiquidityValue_fee_on()
  await beforeEach_getLiquidityValue_fee_on_mint()

  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    100,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '500000000000000000') 
  assert.equal(token1Amount.toString(), '50000000000000000000') 

  console.log('getLiquidityValue_fee_on_arbing_1_100 passed')
}

async function beforeEach_getLiquidityValue_fee_on_swap () {
  await fixture.token0.approve(fixture.router.address, MaxUint256, overrides)
  await fixture.router.swapExactTokensForTokens(
    expandTo18Decimals(20),
    0,
    [fixture.token0.address, fixture.token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  const [reserve0, reserve1] = await fixture.pair.getReserves()
  assert.equal(reserve0.toString(), '40000000000000000000')
  assert.equal(reserve1.toString(), '1001502253380070105158') // half plus the fee
}

async function getLiquidityValue_fee_on_1_25_liquidity () {
  await beforeEach()
  await beforeEach_fee_on()
  await beforeEach1_fee_on()
  await beforeEach_getLiquidityValue_fee_on_swap()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    25,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '999874953089810756')
  assert.equal(token1Amount.toString(), '25034425465443434060')

  console.log('getLiquidityValue_fee_on_1_25_liquidity passed')
}

async function getLiquidityValue_fee_on_1_100_liquidity () {
  await beforeEach()
  await beforeEach_fee_on()
  await beforeEach1_fee_on()
  await beforeEach_getLiquidityValue_fee_on_swap()
  const [token0Amount, token1Amount] = await computeLiquidityValue.getLiquidityValueAfterArbitrageToPrice(
    fixture.token0.address,
    fixture.token1.address,
    1,
    100,
    expandTo18Decimals(5)
  )
  assert.equal(token0Amount.toString(), '501002443792372662')
  assert.equal(token1Amount.toString(), '50024924521757597314')

  console.log('getLiquidityValue_fee_on_1_100_liquidity passed')
}

async function main() {
  [wallet, other] = await ethers.getSigners()
  ExampleComputeLiquidityValue = await ethers.getContractFactory('ExampleComputeLiquidityValue')
  await correct_for_5_shares()
  await correct_for_7_shares()
  await correct_after_swap()
  await coorect_after_swap_fee_on()
  await getReservesAfterArbitrage_1_400()
  await getReservesAfterArbitrage_1_200()
  await getReservesAfterArbitrage_1_100()
  await getReservesAfterArbitrage_1_50()
  await getReservesAfterArbitrage_1_25()
  await getReservesAfterArbitrage_25_1()
  await getReservesAfterArbitrage_large()
  await getLiquidityValue_fee_off_1_105()
  await getLiquidityValue_fee_off_1_95()
  await getLiquidityValue_fee_off_1_25_liquidity()
  await getLiquidityValue_fee_off_arbing_1_100()
  await getLiquidityValue_fee_on_arbing_1_105()
  await getLiquidityValue_fee_on_arbing_1_95()
  await getLiquidityValue_fee_on_arbing_1_100()
  await getLiquidityValue_fee_on_1_25_liquidity()
  await getLiquidityValue_fee_on_1_100_liquidity()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main
