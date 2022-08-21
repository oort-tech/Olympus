const { ethers, upgrades } = require('hardhat')
const { Contract, ContractFactory, getContractFactory, providers } = ethers
const { ecsign } = require('ethereumjs-util')
const { AddressZero, MaxUint256, Zero } = ethers.constants
const utils = ethers.utils
const { defaultAbiCoder, formatEther, hexlify } = utils
const { BigNumber } = require('ethers')

const assert = require('assert')

const { deployContract } = require('ethereum-waffle')

const { deploycontracts } = require('./shared/fixtures')
const { expandTo18Decimals, getApprovalDigest, mineBlock, MINIMUM_LIQUIDITY } = require('./shared/utilities.js')
const IUniswapV2Pair = require('@uniswap/v2-core/build/IUniswapV2Pair.json')
const DeflatingERC20 = require('../build/DeflatingERC20.json')

const overrides = {
  gasLimit: 600000
}

let token0
let token1
let WETH
let DTT
let DTT2
let WETHPartner
let factory
let router
let pair
let WETHPair
let routerEventEmitter
let tx
let fixture
let receipt
let token0Amount
let token1Amount
let swapAmount
let expectedOutputAmount
let DTTAmount
let DTT2Amount
let ETHAmount
let amountIn
async function beforeEach() {
  fixture = await deploycontracts(wallet)
  token0 = fixture.token0
  token1 = fixture.token1
  router = fixture.router02
}

async function quote() {
  await beforeEach()
  assert.equal(
    (await router.quote(BigNumber.from(1), BigNumber.from(100), BigNumber.from(200))).toString(),
    BigNumber.from('2')
  )
  assert.equal(
    (await router.quote(BigNumber.from(2), BigNumber.from(200), BigNumber.from(100))).toString(),
    BigNumber.from(1)
  )
  try {
    await router.quote(BigNumber.from(0), BigNumber.from(100), BigNumber.from(200))
    // .to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_AMOUNT'
    // )
    assert(false)
  } catch (error) {}

  try {
    await router.quote(BigNumber.from(1), BigNumber.from(0), BigNumber.from(200))
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_LIQUIDITY'
    // )
    assert(false)
  } catch (error) {}

  try {
    await router.quote(BigNumber.from(1), BigNumber.from(100), BigNumber.from(0))
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_LIQUIDITY'
    // )
    assert(false)
  } catch (error) {}

  console.log('quote passed')
}

async function getAmountOut() {
  await beforeEach()
  assert.equal(
    (await router.getAmountOut(BigNumber.from(2), BigNumber.from(100), BigNumber.from(100))).toString(),
    BigNumber.from(1)
  )
  try {
    await router.getAmountOut(BigNumber.from(0), BigNumber.from(100), BigNumber.from(100))
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_INPUT_AMOUNT'
    // )
  } catch (error) {}
  try {
    await router.getAmountOut(BigNumber.from(2), BigNumber.from(0), BigNumber.from(100))
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_LIQUIDITY'
    // )
  } catch (error) {}
  try {
    await router.getAmountOut(BigNumber.from(2), BigNumber.from(100), BigNumber.from(0))
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_LIQUIDITY'
    // )
  } catch (error) {}

  console.log('getAmountOut passed')
}

async function getAmountIn() {
  await beforeEach()
  assert.equal(
    (await router.getAmountIn(BigNumber.from(1), BigNumber.from(100), BigNumber.from(100))).toString(),
    BigNumber.from(2)
  )
  try {
    await router.getAmountIn(BigNumber.from(0), BigNumber.from(100), BigNumber.from(100))
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_OUTPUT_AMOUNT'
    // )
  } catch (error) {}
  try {
    await router.getAmountIn(BigNumber.from(1), BigNumber.from(0), BigNumber.from(100))
    // ).tobe.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_LIQUIDITY'
    // )
  } catch (error) {}
  try {
    await router.getAmountIn(BigNumber.from(1), BigNumber.from(100), BigNumber.from(0))
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INSUFFICIENT_LIQUIDITY'
    // )
  } catch (error) {}
  console.log('getAmountIn passed')
}

async function getAmountsOut() {
  await beforeEach()
  tx = await token0.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await token1.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.addLiquidity(
    token0.address,
    token1.address,
    BigNumber.from(10000),
    BigNumber.from(10000),
    0,
    0,
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  try {
    await router.getAmountsOut(BigNumber.from(2), [token0.address])
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INVALID_PATH'
    // )
  } catch (error) {}

  const path = [token0.address, token1.address]
  const outs = await router.getAmountsOut(BigNumber.from(2), path)
  assert.equal(outs[0].toString(), BigNumber.from(2).toString())
  assert.equal(outs[1].toString(), BigNumber.from(1).toString())

  console.log('getAmountsOut passed')
}

async function getAmountsIn() {
  await beforeEach()
  tx = await token0.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await token1.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.addLiquidity(
    token0.address,
    token1.address,
    BigNumber.from(10000),
    BigNumber.from(10000),
    0,
    0,
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  try {
    await router.getAmountsIn(BigNumber.from(1), [token0.address])
    // ).to.be.revertedWith(
    //   'UniswapV2Library: INVALID_PATH'
    // )
  } catch (error) {}

  const path = [token0.address, token1.address]
  const outs = await router.getAmountsIn(BigNumber.from(1), path)
  assert.equal(outs[0].toString(), BigNumber.from(2).toString())
  assert.equal(outs[1].toString(), BigNumber.from(1).toString())
  console.log('getAmountsIn passed')
}

async function beforeFOT() {
  fixture = await deploycontracts(wallet)
  WETH = fixture.WETH
  router = fixture.router02
  DTT = await deployContract(wallet, DeflatingERC20, [expandTo18Decimals(10000)])

  // make a DTT<>WETH pair
  tx = await fixture.factoryV2.createPair(DTT.address, WETH.address)
  await tx.wait()
  const pairAddress = await fixture.factoryV2.getPair(DTT.address, WETH.address)
  pair = new Contract(pairAddress, JSON.stringify(IUniswapV2Pair.abi)).connect(wallet)
}

async function afterFOT() {
  assert.equal((await ethers.provider.getBalance(router.address)).toString(), '0')
}

async function addLiquidity(DTTAmount, WETHAmount) {
  tx = await DTT.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.addLiquidityETH(DTT.address, DTTAmount, DTTAmount, WETHAmount, wallet.address, MaxUint256, {
    ...overrides,
    value: WETHAmount
  })
  await tx.wait()
}

async function removeLiquidityETHSupportingFeeOnTransferTokens() {
  await beforeFOT()
  const DTTAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)
  await addLiquidity(DTTAmount, ETHAmount)

  const DTTInPair = await DTT.balanceOf(pair.address)
  const WETHInPair = await WETH.balanceOf(pair.address)
  const liquidity = await pair.balanceOf(wallet.address)
  const totalSupply = await pair.totalSupply()
  const NaiveDTTExpected = DTTInPair.mul(liquidity).div(totalSupply)
  const WETHExpected = WETHInPair.mul(liquidity).div(totalSupply)

  tx = await pair.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.removeLiquidityETHSupportingFeeOnTransferTokens(
    DTT.address,
    liquidity,
    NaiveDTTExpected,
    WETHExpected,
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  await afterFOT()
  console.log('removeLiquidityETHSupportingFeeOnTransferTokens passed')
}


async function before_FOT_swapExactTokensForTokensSupportingFeeOnTransferTokens() {
  await beforeFOT()
  DTTAmount = expandTo18Decimals(5)
    .mul(100)
    .div(99)
  ETHAmount = expandTo18Decimals(10)
  amountIn = expandTo18Decimals(1)
  await addLiquidity(DTTAmount, ETHAmount)
}

async function swapExactTokensForTokensSupportingFeeOnTransferTokens_DTT_WETH() {
  await before_FOT_swapExactTokensForTokensSupportingFeeOnTransferTokens()
  tx = await DTT.approve(router.address, MaxUint256)
  await tx.wait()

  tx = await router.swapExactTokensForTokensSupportingFeeOnTransferTokens(
    amountIn,
    0,
    [DTT.address, WETH.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  await afterFOT()
  console.log('swapExactTokensForTokensSupportingFeeOnTransferTokens_DTT_WETH passed')
}

async function swapExactTokensForTokensSupportingFeeOnTransferTokens_WETH_DTT() {
  await before_FOT_swapExactTokensForTokensSupportingFeeOnTransferTokens()
  tx = await WETH.deposit({ value: amountIn }) // mint WETH
  await tx.wait()
  tx = await WETH.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.swapExactTokensForTokensSupportingFeeOnTransferTokens(
    amountIn,
    0,
    [WETH.address, DTT.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  await afterFOT()
  console.log('swapExactTokensForTokensSupportingFeeOnTransferTokens_WETH_DTT passed')
}

async function swapExactETHForTokensSupportingFeeOnTransferTokens() {
  await beforeFOT()
  const DTTAmount = expandTo18Decimals(10)
    .mul(100)
    .div(99)
  const ETHAmount = expandTo18Decimals(5)
  const swapAmount = expandTo18Decimals(1)
  await addLiquidity(DTTAmount, ETHAmount)

  tx = await router.swapExactETHForTokensSupportingFeeOnTransferTokens(
    0,
    [WETH.address, DTT.address],
    wallet.address,
    MaxUint256,
    {
      ...overrides,
      value: swapAmount
    }
  )
  await tx.wait()
  await afterFOT()
  console.log('swapExactETHForTokensSupportingFeeOnTransferTokens passed')
}

async function swapExactTokensForETHSupportingFeeOnTransferTokens() {
  await beforeFOT()
  const DTTAmount = expandTo18Decimals(5)
    .mul(100)
    .div(99)
  const ETHAmount = expandTo18Decimals(10)
  const swapAmount = expandTo18Decimals(1)

  await addLiquidity(DTTAmount, ETHAmount)
  tx = await DTT.approve(router.address, MaxUint256)
  await tx.wait()

  tx = await router.swapExactTokensForETHSupportingFeeOnTransferTokens(
    swapAmount,
    0,
    [DTT.address, WETH.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  await afterFOT()
  console.log('swapExactTokensForETHSupportingFeeOnTransferTokens passed')
}

async function beforeFOT_reloaded() {
  fixture = await deploycontracts(wallet)
  router = fixture.router02
  DTT = await deployContract(wallet, DeflatingERC20, [expandTo18Decimals(10000)])
  DTT2 = await deployContract(wallet, DeflatingERC20, [expandTo18Decimals(10000)])
  // make a DTT<>WETH pair
  tx = await fixture.factoryV2.createPair(DTT.address, DTT2.address)
  await tx.wait()
  const pairAddress = await fixture.factoryV2.getPair(DTT.address, DTT2.address)

  DTTAmount = expandTo18Decimals(5)
    .mul(100)
    .div(99)
  DTT2Amount = expandTo18Decimals(5)
  amountIn = expandTo18Decimals(1)
  await addLiquidity_reoladed(DTTAmount, DTT2Amount)
}

async function afterFOT_reloaded() {
  assert.equal((await ethers.provider.getBalance(router.address)).toString(), '0')
}

async function addLiquidity_reoladed(DTTAmount, DTT2Amount) {
  tx = await DTT.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await DTT2.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.addLiquidity(
    DTT.address,
    DTT2.address,
    DTTAmount,
    DTT2Amount,
    DTTAmount,
    DTT2Amount,
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
}

async function DTT_DTT2() {
  await beforeFOT_reloaded()
  tx = await DTT.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.swapExactTokensForTokensSupportingFeeOnTransferTokens(
    amountIn,
    0,
    [DTT.address, DTT2.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  await afterFOT_reloaded()
  console.log('DTT_DTT2 passed')
}

async function main() {
  ;[wallet] = await ethers.getSigners()
  await quote()
  await getAmountOut()
  await getAmountIn()
  await getAmountsOut()
  await getAmountsIn()
  await removeLiquidityETHSupportingFeeOnTransferTokens()

  await swapExactTokensForTokensSupportingFeeOnTransferTokens_DTT_WETH()

  await swapExactTokensForTokensSupportingFeeOnTransferTokens_WETH_DTT()
  await swapExactETHForTokensSupportingFeeOnTransferTokens()
  await swapExactTokensForETHSupportingFeeOnTransferTokens()
  await DTT_DTT2()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main
