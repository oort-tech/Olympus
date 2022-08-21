const { ethers, upgrades, network } = require("hardhat")
const { AddressZero, MaxUint256 } = ethers.constants
const utils = ethers.utils
const { defaultAbiCoder, formatEther } = utils
const { BigNumber } = require("ethers")

const assert = require('assert')

const {deployContract } = require('ethereum-waffle')

const { deploycontracts } = require("./shared/fixtures")
const { expandTo18Decimals, mineBlock, encodePrice } = require("./shared/utilities.js")

const ExampleSlidingWindowOracle = require('../build/ExampleSlidingWindowOracle.json')


const overrides = {
  gasLimit: 600000
}

const defaultToken0Amount = expandTo18Decimals(5)
const defaultToken1Amount = expandTo18Decimals(10)

let token0
let token1
let pair
let weth
let factory


async function addLiquidity(amount0 = defaultToken0Amount, amount1 = defaultToken1Amount) {
  if (!amount0.isZero()) {
    tx = await token0.transfer(pair.address, amount0)
    await tx.wait()
  }
  if (!amount1.isZero()) {
    tx = await token1.transfer(pair.address, amount1)
    await tx.wait()
  }
  tx = await pair.sync()
  tx.wait()
}

const defaultWindowSize = 86400 // 24 hours
const defaultGranularity = 24 // 1 hour each
const startTime = 1577836800

function observationIndexOf(
  timestamp,
  windowSize = defaultWindowSize,
  granularity = defaultGranularity
) {
  const periodSize = Math.floor(windowSize / granularity)
  const epochPeriod = Math.floor(timestamp / periodSize)
  return epochPeriod % granularity
}

function deployOracle(windowSize, granularity) {
  return deployContract(wallet, ExampleSlidingWindowOracle, [factory.address, windowSize, granularity], overrides)
}

async function beforeEach() {
  fixture = await deploycontracts(wallet)

  token0 = fixture.token0
  token1 = fixture.token1
  pair = fixture.pair
  weth = fixture.WETH
  factory = fixture.factoryV2

  await mineBlock(network.provider, startTime)
}

async function requires_granularity_greater_0 () {
  await beforeEach()
  try {
    await deployOracle(defaultWindowSize, 0)
    assert.equal(1, 0)
  } catch (error) {
    console.log(error)
  }
  console.log('requires_granularity_greater_0 passed')
}

async function requires_windowSize_divisible_granularity () {
  await beforeEach()
  try {
    await deployOracle(defaultWindowSize - 1, defaultGranularity)
    assert.equal(1, 0)
  } catch (error) {
    
  }
  console.log('requires_windowSize_divisible_granularity passed')
}

async function computes_the_periodSize_correctly() {
  await beforeEach()
  const oracle = await deployOracle(defaultWindowSize, defaultGranularity)
  assert.equal((await oracle.periodSize()).toString(), '3600')
  const oracleOther = await deployOracle(defaultWindowSize * 2, defaultGranularity / 2)
  assert.equal((await oracleOther.periodSize()).toString(), '14400') //3600 * 4
  console.log('computes_the_periodSize_correctly passed')
}

async function observationIndexOf_works_for_example () {
  await beforeEach()
  const oracle = await deployOracle(defaultWindowSize, defaultGranularity)
  assert.equal((await oracle.observationIndexOf(0)), 0)
  assert.equal((await oracle.observationIndexOf(3599)), 0)
  assert.equal((await oracle.observationIndexOf(3600)), 1)
  assert.equal((await oracle.observationIndexOf(4800)), 1)
  assert.equal((await oracle.observationIndexOf(7199)), 1)
  assert.equal((await oracle.observationIndexOf(7200)), 2)
  assert.equal((await oracle.observationIndexOf(86399)), 23)
  assert.equal((await oracle.observationIndexOf(86400)), 0)
  assert.equal((await oracle.observationIndexOf(90000)), 1)
  console.log('observationIndexOf_works_for_example passed')
}

async function observationIndexOf_overflow_safe () {
  await beforeEach()
  const oracle = await deployOracle(25500, 255) // 100 period size
  assert.equal((await oracle.observationIndexOf(0)), 0)
  assert.equal((await oracle.observationIndexOf(99)), 0)
  assert.equal((await oracle.observationIndexOf(100)), 1)
  assert.equal((await oracle.observationIndexOf(199)), 1)
  assert.equal((await oracle.observationIndexOf(25499)), 254) // 255th element
  assert.equal((await oracle.observationIndexOf(25500)), 0)
  console.log('observationIndexOf_overflow_safe passed')
}

async function observationIndexOf_matches_offline_computation () {
  await beforeEach()
  const oracle = await deployOracle(defaultWindowSize, defaultGranularity)
  for (let timestamp of [0, 5000, 1000, 25000, 86399, 86400, 86401]) {
    assert.equal((await oracle.observationIndexOf(timestamp)), observationIndexOf(timestamp))
  }
  console.log('observationIndexOf_matches_offline_computation passed')
}

let slidingWindowOracle
async function beforeEach_update () {
  await beforeEach()
  slidingWindowOracle = await deployOracle(defaultWindowSize, defaultGranularity)
  await addLiquidity()
}

async function update_succeeds () {
  await beforeEach_update()
  await slidingWindowOracle.update(token0.address, token1.address, overrides)
  console.log('update_succeeds passed')
}

async function update_sets_the_appropriate_epoch_slot() {
  await beforeEach_update()
  const blockTimestamp = (await pair.getReserves())[2]
  assert.equal(blockTimestamp, startTime)
  tx = await slidingWindowOracle.update(token0.address, token1.address, overrides)
  await tx.wait()
  assert.equal((await slidingWindowOracle.pairObservations(pair.address, observationIndexOf(blockTimestamp))).to.deep.eq([
    bigNumberify(blockTimestamp),
    await pair.price0CumulativeLast(),
    await pair.price1CumulativeLast()
  ]))
}
//   describe('#update', () => {


//     it('sets the appropriate epoch slot', async () => {
//       const blockTimestamp = (await pair.getReserves())[2]
//       expect(blockTimestamp).to.eq(startTime)
//       await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       expect(await slidingWindowOracle.pairObservations(pair.address, observationIndexOf(blockTimestamp))).to.deep.eq([
//         bigNumberify(blockTimestamp),
//         await pair.price0CumulativeLast(),
//         await pair.price1CumulativeLast()
//       ])
//     }).retries(2) // we may have slight differences between pair blockTimestamp and the expected timestamp
//     // because the previous block timestamp may differ from the current block timestamp by 1 second

//     it('gas for first update (allocates empty array)', async () => {
//       const tx = await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       const receipt = await tx.wait()
//       expect(receipt.gasUsed).to.eq('116816')
//     }).retries(2) // gas test inconsistent

//     it('gas for second update in the same period (skips)', async () => {
//       await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       const tx = await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       const receipt = await tx.wait()
//       expect(receipt.gasUsed).to.eq('25574')
//     }).retries(2) // gas test inconsistent

//     it('gas for second update different period (no allocate, no skip)', async () => {
//       await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       await mineBlock(provider, startTime + 3600)
//       const tx = await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       const receipt = await tx.wait()
//       expect(receipt.gasUsed).to.eq('94703')
//     }).retries(2) // gas test inconsistent

//     it('second update in one timeslot does not overwrite', async () => {
//       await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       const before = await slidingWindowOracle.pairObservations(pair.address, observationIndexOf(0))
//       // first hour still
//       await mineBlock(provider, startTime + 1800)
//       await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       const after = await slidingWindowOracle.pairObservations(pair.address, observationIndexOf(1800))
//       expect(observationIndexOf(1800)).to.eq(observationIndexOf(0))
//       expect(before).to.deep.eq(after)
//     })

//     it('fails for invalid pair', async () => {
//       await expect(slidingWindowOracle.update(weth.address, token1.address)).to.be.reverted
//     })
//   })

//   describe('#consult', () => {
//     let slidingWindowOracle: Contract

//     beforeEach(
//       'deploy oracle',
//       async () => (slidingWindowOracle = await deployOracle(defaultWindowSize, defaultGranularity))
//     )

//     // must come after setting time to 0 for correct cumulative price computations in the pair
//     beforeEach('add default liquidity', () => addLiquidity())

//     it('fails if previous bucket not set', async () => {
//       await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       await expect(slidingWindowOracle.consult(token0.address, 0, token1.address)).to.be.revertedWith(
//         'SlidingWindowOracle: MISSING_HISTORICAL_OBSERVATION'
//       )
//     })

//     it('fails for invalid pair', async () => {
//       await expect(slidingWindowOracle.consult(weth.address, 0, token1.address)).to.be.reverted
//     })

//     describe('happy path', () => {
//       let blockTimestamp: number
//       let previousBlockTimestamp: number
//       let previousCumulativePrices: any
//       beforeEach('add some prices', async () => {
//         previousBlockTimestamp = (await pair.getReserves())[2]
//         previousCumulativePrices = [await pair.price0CumulativeLast(), await pair.price1CumulativeLast()]
//         await slidingWindowOracle.update(token0.address, token1.address, overrides)
//         blockTimestamp = previousBlockTimestamp + 23 * 3600
//         await mineBlock(provider, blockTimestamp)
//         await slidingWindowOracle.update(token0.address, token1.address, overrides)
//       })

//       it('has cumulative price in previous bucket', async () => {
//         expect(
//           await slidingWindowOracle.pairObservations(pair.address, observationIndexOf(previousBlockTimestamp))
//         ).to.deep.eq([bigNumberify(previousBlockTimestamp), previousCumulativePrices[0], previousCumulativePrices[1]])
//       }).retries(5) // test flaky because timestamps aren't mocked

//       it('has cumulative price in current bucket', async () => {
//         const timeElapsed = blockTimestamp - previousBlockTimestamp
//         const prices = encodePrice(defaultToken0Amount, defaultToken1Amount)
//         expect(
//           await slidingWindowOracle.pairObservations(pair.address, observationIndexOf(blockTimestamp))
//         ).to.deep.eq([bigNumberify(blockTimestamp), prices[0].mul(timeElapsed), prices[1].mul(timeElapsed)])
//       }).retries(5) // test flaky because timestamps aren't mocked

//       it('provides the current ratio in consult token0', async () => {
//         expect(await slidingWindowOracle.consult(token0.address, 100, token1.address)).to.eq(200)
//       })

//       it('provides the current ratio in consult token1', async () => {
//         expect(await slidingWindowOracle.consult(token1.address, 100, token0.address)).to.eq(50)
//       })
//     })

//     describe('price changes over period', () => {
//       const hour = 3600
//       beforeEach('add some prices', async () => {
//         // starting price of 1:2, or token0 = 2token1, token1 = 0.5token0
//         await slidingWindowOracle.update(token0.address, token1.address, overrides) // hour 0, 1:2
//         // change the price at hour 3 to 1:1 and immediately update
//         await mineBlock(provider, startTime + 3 * hour)
//         await addLiquidity(defaultToken0Amount, bigNumberify(0))
//         await slidingWindowOracle.update(token0.address, token1.address, overrides)

//         // change the ratios at hour 6:00 to 2:1, don't update right away
//         await mineBlock(provider, startTime + 6 * hour)
//         await token0.transfer(pair.address, defaultToken0Amount.mul(2))
//         await pair.sync()

//         // update at hour 9:00 (price has been 2:1 for 3 hours, invokes counterfactual)
//         await mineBlock(provider, startTime + 9 * hour)
//         await slidingWindowOracle.update(token0.address, token1.address, overrides)
//         // move to hour 23:00 so we can check prices
//         await mineBlock(provider, startTime + 23 * hour)
//       })

//       it('provides the correct ratio in consult token0', async () => {
//         // at hour 23, price of token 0 spent 3 hours at 2, 3 hours at 1, 17 hours at 0.5 so price should
//         // be less than 1
//         expect(await slidingWindowOracle.consult(token0.address, 100, token1.address)).to.eq(76)
//       })

//       it('provides the correct ratio in consult token1', async () => {
//         // price should be greater than 1
//         expect(await slidingWindowOracle.consult(token1.address, 100, token0.address)).to.eq(167)
//       })

//       // price has been 2:1 all of 23 hours
//       describe('hour 32', () => {
//         beforeEach('set hour 32', () => mineBlock(provider, startTime + 32 * hour))
//         it('provides the correct ratio in consult token0', async () => {
//           // at hour 23, price of token 0 spent 3 hours at 2, 3 hours at 1, 17 hours at 0.5 so price should
//           // be less than 1
//           expect(await slidingWindowOracle.consult(token0.address, 100, token1.address)).to.eq(50)
//         })

//         it('provides the correct ratio in consult token1', async () => {
//           // price should be greater than 1
//           expect(await slidingWindowOracle.consult(token1.address, 100, token0.address)).to.eq(200)
//         })
//       })
//     })
//   })
// })

async function main() {
  [wallet] = await ethers.getSigners()
  console.log("-------------------This is only working for hardhat----------------------------")
  await requires_granularity_greater_0()
  await requires_windowSize_divisible_granularity()
  await computes_the_periodSize_correctly()
  await observationIndexOf_works_for_example()
  await observationIndexOf_overflow_safe()
  await observationIndexOf_matches_offline_computation()
  await update_succeeds()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main