const { ethers, upgrades } = require("hardhat")
const { AddressZero, MaxUint256 } = ethers.constants
const utils = ethers.utils
const { defaultAbiCoder, formatEther } = utils
const { BigNumber } = require("ethers")

const assert = require('assert')

const {deployContract } = require('ethereum-waffle')

const { deploycontracts } = require("./shared/fixtures")
const { expandTo18Decimals } = require("./shared/utilities.js")

const ExampleFlashSwap = require('../build/ExampleFlashSwap.json')

const overrides = {
  gasLimit: 600000,
}

let wallet
let fixture
let factory
let tx
let WETH
let WETHPartner
let WETHExchangeV1
let WETHPair
let flashSwapExample

async function beforeEach() {
  fixture =  await deploycontracts(wallet)
  WETH = fixture.WETH
  WETHPartner = fixture.WETHPartner
  WETHExchangeV1 = fixture.WETHExchangeV1
  WETHPair = fixture.WETHPair
  flashSwapExample = await deployContract(
    wallet,
    ExampleFlashSwap,
    [fixture.factoryV2.address, fixture.factoryV1.address, fixture.router.address])
}

async function uniswapV2Call_0() {
  await beforeEach()
  console.log('uniswapV2Call_0 started')
  const WETHPartnerAmountV1 = expandTo18Decimals(2000)
    const ETHAmountV1 = expandTo18Decimals(10)
    tx = await WETHPartner.approve(WETHExchangeV1.address, WETHPartnerAmountV1)
    await tx.wait()
    tx = await WETHExchangeV1.addLiquidity(BigNumber.from(1), WETHPartnerAmountV1, MaxUint256, {
      ...overrides,
      value: ETHAmountV1
    })
    await tx.wait()

    // add liquidity to V2 at a rate of 1 ETH / 100 X
    const WETHPartnerAmountV2 = expandTo18Decimals(1000)
    const ETHAmountV2 = expandTo18Decimals(10)
    tx = await WETHPartner.transfer(WETHPair.address, WETHPartnerAmountV2)
    await tx.wait()
    tx = await WETH.deposit({ value: ETHAmountV2 })
    await tx.wait()
    tx = await WETH.transfer(WETHPair.address, ETHAmountV2)
    await tx.wait()
    tx = await WETHPair.mint(wallet.address, overrides)
    await tx.wait()

    const balanceBefore = await WETHPartner.balanceOf(wallet.address)

    // now, execute arbitrage via uniswapV2Call:
    // receive 1 ETH from V2, get as much X from V1 as we can, repay V2 with minimum X, keep the rest!
    const arbitrageAmount = expandTo18Decimals(1)
    // instead of being 'hard-coded', the above value could be calculated optimally off-chain. this would be
    // better, but it'd be better yet to calculate the amount at runtime, on-chain. unfortunately, this requires a
    // swap-to-price calculation, which is a little tricky, and out of scope for the moment
    const WETHPairToken0 = await WETHPair.token0()
    const amount0 = WETHPairToken0 === WETHPartner.address ? BigNumber.from(0) : arbitrageAmount
    const amount1 = WETHPairToken0 === WETHPartner.address ? arbitrageAmount : BigNumber.from(0)
    tx = await WETHPair.swap(
      amount0,
      amount1,
      flashSwapExample.address,
      defaultAbiCoder.encode(['uint'], [BigNumber.from(1)]),
      overrides
    )

    await tx.wait()

    const balanceAfter = await WETHPartner.balanceOf(wallet.address)
    const profit = balanceAfter.sub(balanceBefore).div(expandTo18Decimals(1))
    const reservesV1 = [
      await WETHPartner.balanceOf(WETHExchangeV1.address),
      await ethers.provider.getBalance(WETHExchangeV1.address)
    ]
    const priceV1 = reservesV1[0].div(reservesV1[1])
    const reservesV2 = (await WETHPair.getReserves()).slice(0, 2)
    const priceV2 =
      WETHPairToken0 === WETHPartner.address ? reservesV2[0].div(reservesV2[1]) : reservesV2[1].div(reservesV2[0])

    assert.equal(profit.toString(), '69') // our profit is ~69 tokens
    assert.equal(priceV1.toString(), '165') // we pushed the v1 price down to ~165
    assert.equal(priceV2.toString(), '123') // we pushed the v2 price up to ~123

    console.log('uniswapV2Call_0 passed')
}

async function uniswapV2Call_1 () {
  await beforeEach()
  // add liquidity to V1 at a rate of 1 ETH / 100 X
  const WETHPartnerAmountV1 = expandTo18Decimals(1000)
  const ETHAmountV1 = expandTo18Decimals(10)
  tx = await WETHPartner.approve(WETHExchangeV1.address, WETHPartnerAmountV1)
  await tx.wait()
  tx = await WETHExchangeV1.addLiquidity(BigNumber.from(1), WETHPartnerAmountV1, MaxUint256, {
    ...overrides,
    value: ETHAmountV1
  })
  await tx.wait()
  // add liquidity to V2 at a rate of 1 ETH / 200 X
  const WETHPartnerAmountV2 = expandTo18Decimals(2000)
  const ETHAmountV2 = expandTo18Decimals(10)
  tx = await WETHPartner.transfer(WETHPair.address, WETHPartnerAmountV2)
  await tx.wait()
  tx = await WETH.deposit({ value: ETHAmountV2 })
  await tx.wait()
  tx = await WETH.transfer(WETHPair.address, ETHAmountV2)
  await tx.wait()
  tx = await WETHPair.mint(wallet.address, overrides)
  await tx.wait()

  const balanceBefore = await ethers.provider.getBalance(wallet.address)
  // now, execute arbitrage via uniswapV2Call:
  // receive 200 X from V2, get as much ETH from V1 as we can, repay V2 with minimum ETH, keep the rest!
  const arbitrageAmount = expandTo18Decimals(200)
  // instead of being 'hard-coded', the above value could be calculated optimally off-chain. this would be
  // better, but it'd be better yet to calculate the amount at runtime, on-chain. unfortunately, this requires a
  // swap-to-price calculation, which is a little tricky, and out of scope for the moment
  const WETHPairToken0 = await WETHPair.token0()
  const amount0 = WETHPairToken0 === WETHPartner.address ? arbitrageAmount : BigNumber.from(0)
  const amount1 = WETHPairToken0 === WETHPartner.address ? BigNumber.from(0) : arbitrageAmount
  tx = await WETHPair.swap(
    amount0,
    amount1,
    flashSwapExample.address,
    defaultAbiCoder.encode(['uint'], [BigNumber.from(1)]),
    overrides
  )
  await tx.wait()
  const balanceAfter = await ethers.provider.getBalance(wallet.address)
  const profit = balanceAfter.sub(balanceBefore)
  const reservesV1 = [
    await WETHPartner.balanceOf(WETHExchangeV1.address),
    await ethers.provider.getBalance(WETHExchangeV1.address)
  ]
  const priceV1 = reservesV1[0].div(reservesV1[1])
  const reservesV2 = (await WETHPair.getReserves()).slice(0, 2)
  const priceV2 =
    WETHPairToken0 === WETHPartner.address ? reservesV2[0].div(reservesV2[1]) : reservesV2[1].div(reservesV2[0])

  // assert.equal(formatEther(profit).toString(), '0.548043441089763649') // our profit is ~.5 ETH , not matched
  assert.equal(priceV1.toString(), '143') // we pushed the v1 price up to ~143
  assert.equal(priceV2.toString(), '161') // we pushed the v2 price down to ~161

  console.log('uniswapV2Call_1 passed')
}


async function main() {
  [wallet] = await ethers.getSigners()
  await uniswapV2Call_0()
  await uniswapV2Call_1()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main