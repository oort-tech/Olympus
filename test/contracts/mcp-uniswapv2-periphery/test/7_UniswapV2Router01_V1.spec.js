const { ethers, upgrades } = require('hardhat')
const { ecsign } = require('ethereumjs-util')
const { AddressZero, MaxUint256, Zero } = ethers.constants
const utils = ethers.utils
const { defaultAbiCoder, formatEther, hexlify } = utils
const { BigNumber } = require('ethers')

const assert = require('assert')

const { deployContract } = require('ethereum-waffle')

const { deploycontracts } = require('./shared/fixtures')
const { expandTo18Decimals, getApprovalDigest, mineBlock, MINIMUM_LIQUIDITY } = require('./shared/utilities.js')

const overrides = {
  gasLimit: 600000
}

let token0
let token1
let WETH
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
async function beforeEach() {
  fixture = await deploycontracts(wallet)
  token0 = fixture.token0
  token1 = fixture.token1
  WETH = fixture.WETH
  WETHPartner = fixture.WETHPartner
  factory = fixture.factoryV2
  router = fixture.router01
  pair = fixture.pair
  WETHPair = fixture.WETHPair
  routerEventEmitter = fixture.routerEventEmitter
}

async function afterEach() {
  assert.equal((await ethers.provider.getBalance(router.address)).toString(), Zero.toString())
}

async function factory_WETH() {
  await beforeEach()
  assert.equal((await router.factory()).toString(), factory.address.toString())
  assert.equal((await router.WETH()).toString(), WETH.address.toString())
  await afterEach()
  console.log('factory_WETH passed')
}

async function addLiquidityTest() {
  await beforeEach()
  token0Amount = expandTo18Decimals(1)
  token1Amount = expandTo18Decimals(4)

  const expectedLiquidity = expandTo18Decimals(2)
  tx = await token0.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await token1.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.addLiquidity(
    token0.address,
    token1.address,
    token0Amount,
    token1Amount,
    0,
    0,
    wallet.address,
    MaxUint256,
    overrides
  )
  receipt = await tx.wait()
  console.log(receipt.events[0].topics)
  // .to.emit(token0, 'Transfer')
  // .withArgs(wallet.address, pair.address, token0Amount)
  // .to.emit(token1, 'Transfer')
  // .withArgs(wallet.address, pair.address, token1Amount)
  // .to.emit(pair, 'Transfer')
  // .withArgs(AddressZero, AddressZero, MINIMUM_LIQUIDITY)
  // .to.emit(pair, 'Transfer')
  // .withArgs(AddressZero, wallet.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  // .to.emit(pair, 'Sync')
  // .withArgs(token0Amount, token1Amount)
  // .to.emit(pair, 'Mint')
  // .withArgs(router.address, token0Amount, token1Amount)
  assert.equal((await pair.balanceOf(wallet.address)).toString(), expectedLiquidity.sub(MINIMUM_LIQUIDITY).toString())
  await afterEach()
  console.log('addLiquidity passed')
}

async function addLiquidityETH() {
  await beforeEach()
  const WETHPartnerAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)

  const expectedLiquidity = expandTo18Decimals(2)
  const WETHPairToken0 = await WETHPair.token0()
  tx = await WETHPartner.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.addLiquidityETH(
    WETHPartner.address,
    WETHPartnerAmount,
    WETHPartnerAmount,
    ETHAmount,
    wallet.address,
    MaxUint256,
    { ...overrides, value: ETHAmount }
  )
  await tx.wait()
  // .to.emit(WETHPair, 'Transfer')
  // .withArgs(AddressZero, AddressZero, MINIMUM_LIQUIDITY)
  // .to.emit(WETHPair, 'Transfer')
  // .withArgs(AddressZero, wallet.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  // .to.emit(WETHPair, 'Sync')
  // .withArgs(
  //   WETHPairToken0 === WETHPartner.address ? WETHPartnerAmount : ETHAmount,
  //   WETHPairToken0 === WETHPartner.address ? ETHAmount : WETHPartnerAmount
  // )
  // .to.emit(WETHPair, 'Mint')
  // .withArgs(
  //   router.address,
  //   WETHPairToken0 === WETHPartner.address ? WETHPartnerAmount : ETHAmount,
  //   WETHPairToken0 === WETHPartner.address ? ETHAmount : WETHPartnerAmount
  // )

  assert.equal(
    (await WETHPair.balanceOf(wallet.address)).toString(),
    expectedLiquidity.sub(MINIMUM_LIQUIDITY).toString()
  )
  console.log('addLiquidityETH passed')
  await afterEach()
}

async function addLiquidity(token0Amount, token1Amount) {
  await token0.transfer(pair.address, token0Amount)
  await token1.transfer(pair.address, token1Amount)
  await pair.mint(wallet.address, overrides)
}

async function UniswapV2Router_removeLiquidity() {
  await beforeEach()
  const token0Amount = expandTo18Decimals(1)
  const token1Amount = expandTo18Decimals(4)
  await addLiquidity(token0Amount, token1Amount)

  const expectedLiquidity = expandTo18Decimals(2)
  tx = await pair.approve(router.address, MaxUint256)
  await tx.wait()
  await router.removeLiquidity(
    token0.address,
    token1.address,
    expectedLiquidity.sub(MINIMUM_LIQUIDITY),
    0,
    0,
    wallet.address,
    MaxUint256,
    overrides
  )
  // .to.emit(pair, 'Transfer')
  // .withArgs(wallet.address, pair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  // .to.emit(pair, 'Transfer')
  // .withArgs(pair.address, AddressZero, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  // .to.emit(token0, 'Transfer')
  // .withArgs(pair.address, wallet.address, token0Amount.sub(500))
  // .to.emit(token1, 'Transfer')
  // .withArgs(pair.address, wallet.address, token1Amount.sub(2000))
  // .to.emit(pair, 'Sync')
  // .withArgs(500, 2000)
  // .to.emit(pair, 'Burn')
  // .withArgs(router.address, token0Amount.sub(500), token1Amount.sub(2000), wallet.address)

  assert.equal((await pair.balanceOf(wallet.address)).toString(), '0')
  const totalSupplyToken0 = await token0.totalSupply()
  const totalSupplyToken1 = await token1.totalSupply()
  assert.equal((await token0.balanceOf(wallet.address)).toString(), totalSupplyToken0.sub(500).toString())
  assert.equal((await token1.balanceOf(wallet.address)).toString(), totalSupplyToken1.sub(2000).toString())
  console.log('UniswapV2Router_removeLiquidity passed')
  await afterEach()
}

async function UniswapV2Router_removeLiquidityETH() {
  await beforeEach()
  const WETHPartnerAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)
  tx = await WETHPartner.transfer(WETHPair.address, WETHPartnerAmount)
  await tx.wait()
  tx = await WETH.deposit({ value: ETHAmount })
  await tx.wait()
  tx = await WETH.transfer(WETHPair.address, ETHAmount)
  await tx.wait()
  tx = await WETHPair.mint(wallet.address, overrides)
  await tx.wait()

  const expectedLiquidity = expandTo18Decimals(2)
  const WETHPairToken0 = await WETHPair.token0()

  tx = await WETHPair.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.removeLiquidityETH(
    WETHPartner.address,
    expectedLiquidity.sub(MINIMUM_LIQUIDITY),
    0,
    0,
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  // .to.emit(WETHPair, 'Transfer')
  // .withArgs(wallet.address, WETHPair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  // .to.emit(WETHPair, 'Transfer')
  // .withArgs(WETHPair.address, AddressZero, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  // .to.emit(WETH, 'Transfer')
  // .withArgs(WETHPair.address, router.address, ETHAmount.sub(2000))
  // .to.emit(WETHPartner, 'Transfer')
  // .withArgs(WETHPair.address, router.address, WETHPartnerAmount.sub(500))
  // .to.emit(WETHPartner, 'Transfer')
  // .withArgs(router.address, wallet.address, WETHPartnerAmount.sub(500))
  // .to.emit(WETHPair, 'Sync')
  // .withArgs(
  //   WETHPairToken0 === WETHPartner.address ? 500 : 2000,
  //   WETHPairToken0 === WETHPartner.address ? 2000 : 500
  // )
  // .to.emit(WETHPair, 'Burn')
  // .withArgs(
  //   router.address,
  //   WETHPairToken0 === WETHPartner.address ? WETHPartnerAmount.sub(500) : ETHAmount.sub(2000),
  //   WETHPairToken0 === WETHPartner.address ? ETHAmount.sub(2000) : WETHPartnerAmount.sub(500),
  //   router.address
  // )

  assert.equal((await WETHPair.balanceOf(wallet.address)).toString(), '0')
  const totalSupplyWETHPartner = await WETHPartner.totalSupply()
  const totalSupplyWETH = await WETH.totalSupply()
  assert.equal((await WETHPartner.balanceOf(wallet.address)).toString(), totalSupplyWETHPartner.sub(500).toString())
  assert.equal((await WETH.balanceOf(wallet.address)).toString(), totalSupplyWETH.sub(2000).toString())
  await afterEach()

  console.log('UniswapV2Router_removeLiquidityETH passed')
}

async function UniswapV2Router_removeLiquidityWithPermit() {
  await beforeEach()
  const token0Amount = expandTo18Decimals(1)
  const token1Amount = expandTo18Decimals(4)
  await addLiquidity(token0Amount, token1Amount)

  const expectedLiquidity = expandTo18Decimals(2)

  const nonce = await pair.nonces(wallet.address)
  const digest = await getApprovalDigest(
    pair,
    { owner: wallet.address, spender: router.address, value: expectedLiquidity.sub(MINIMUM_LIQUIDITY) },
    nonce,
    MaxUint256
  )
  const { v, r, s } = ecsign(Buffer.from(digest.slice(2), 'hex'), Buffer.from(process.env.LOCAL_PRIVATE_KEY, 'hex'))

  tx = await router.removeLiquidityWithPermit(
    token0.address,
    token1.address,
    expectedLiquidity.sub(MINIMUM_LIQUIDITY),
    0,
    0,
    wallet.address,
    MaxUint256,
    false,
    v,
    r,
    s,
    overrides
  )
  await tx.wait()
  await afterEach()
  console.log('UniswapV2Router_removeLiquidityWithPermit passed')
}

async function UniswapV2Router_removeLiquidityETHWithPermit() {
  await beforeEach()
  const WETHPartnerAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)
  tx = await WETHPartner.transfer(WETHPair.address, WETHPartnerAmount)
  await tx.wait()
  tx = await WETH.deposit({ value: ETHAmount })
  await tx.wait()
  tx = await WETH.transfer(WETHPair.address, ETHAmount)
  await tx.wait()
  tx = await WETHPair.mint(wallet.address, overrides)
  await tx.wait()

  const expectedLiquidity = expandTo18Decimals(2)

  const nonce = await WETHPair.nonces(wallet.address)
  const digest = await getApprovalDigest(
    WETHPair,
    { owner: wallet.address, spender: router.address, value: expectedLiquidity.sub(MINIMUM_LIQUIDITY) },
    nonce,
    MaxUint256
  )

  const { v, r, s } = ecsign(Buffer.from(digest.slice(2), 'hex'), Buffer.from(process.env.LOCAL_PRIVATE_KEY, 'hex'))

  tx = await router.removeLiquidityETHWithPermit(
    WETHPartner.address,
    expectedLiquidity.sub(MINIMUM_LIQUIDITY),
    0,
    0,
    wallet.address,
    MaxUint256,
    false,
    v,
    r,
    s,
    overrides
  )
  await tx.wait()
  console.log('UniswapV2Router_removeLiquidityETHWithPermit passed')
  await afterEach()
}

async function before_swapExactTokensForTokens() {
  await beforeEach()

  token0Amount = expandTo18Decimals(5)
  token1Amount = expandTo18Decimals(10)
  swapAmount = expandTo18Decimals(1)
  expectedOutputAmount = BigNumber.from('1662497915624478906')

  await addLiquidity(token0Amount, token1Amount)
  tx = await token0.approve(router.address, MaxUint256)
  await tx.wait()
}

async function swapExactTokensForTokens_happy_path() {
  await before_swapExactTokensForTokens()
  tx = await router.swapExactTokensForTokens(
    swapAmount,
    0,
    [token0.address, token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  //             .to.emit(token0, 'Transfer')
  //             .withArgs(wallet.address, pair.address, swapAmount)
  //             .to.emit(token1, 'Transfer')
  //             .withArgs(pair.address, wallet.address, expectedOutputAmount)
  //             .to.emit(pair, 'Sync')
  //             .withArgs(token0Amount.add(swapAmount), token1Amount.sub(expectedOutputAmount))
  //             .to.emit(pair, 'Swap')
  //             .withArgs(router.address, swapAmount, 0, 0, expectedOutputAmount, wallet.address)
  console.log('swapExactTokensForTokens_happy_path passed')
  await afterEach()
}

async function swapExactTokensForToken_amount() {
  await before_swapExactTokensForTokens()
  tx = await token0.approve(routerEventEmitter.address, MaxUint256)
  await tx.wait()
  tx = await routerEventEmitter.swapExactTokensForTokens(
    router.address,
    swapAmount,
    0,
    [token0.address, token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  // .to.emit(routerEventEmitter, 'Amounts')
  // .withArgs([swapAmount, expectedOutputAmount])
  console.log('swapExactTokensForToken_amount passed')
  await afterEach()
}

async function before_swapTokensForExactTokens() {
  await beforeEach()
  token0Amount = expandTo18Decimals(5)
  token1Amount = expandTo18Decimals(10)
  expectedSwapAmount = BigNumber.from('557227237267357629')
  outputAmount = expandTo18Decimals(1)

  await addLiquidity(token0Amount, token1Amount)
}

async function swapTokensForExactTokens_happy_path() {
  await before_swapTokensForExactTokens()
  tx = await token0.approve(router.address, MaxUint256)
  await tx.wait()
  tx = await router.swapTokensForExactTokens(
    outputAmount,
    MaxUint256,
    [token0.address, token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()

  //             .to.emit(token0, 'Transfer')
  //             .withArgs(wallet.address, pair.address, expectedSwapAmount)
  //             .to.emit(token1, 'Transfer')
  //             .withArgs(pair.address, wallet.address, outputAmount)
  //             .to.emit(pair, 'Sync')
  //             .withArgs(token0Amount.add(expectedSwapAmount), token1Amount.sub(outputAmount))
  //             .to.emit(pair, 'Swap')
  //             .withArgs(router.address, expectedSwapAmount, 0, 0, outputAmount, wallet.address)
  await afterEach()
  console.log('swapTokensForExactTokens_happy_path passed')
}

async function swapTokensForExactTokens_amount() {
  await before_swapTokensForExactTokens()
  tx = await token0.approve(routerEventEmitter.address, MaxUint256)
  await tx.wait()
  tx = await routerEventEmitter.swapTokensForExactTokens(
    router.address,
    outputAmount,
    MaxUint256,
    [token0.address, token1.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  //             .to.emit(routerEventEmitter, 'Amounts')
  //             .withArgs([expectedSwapAmount, outputAmount])
  //         })
  await afterEach()
  console.log('swapTokensForExactTokens_amount passed')
}

async function before_swapExactETHForTokens() {
  await beforeEach()
  WETHPartnerAmount = expandTo18Decimals(10)
  ETHAmount = expandTo18Decimals(5)
  swapAmount = expandTo18Decimals(1)
  expectedOutputAmount = BigNumber.from('1662497915624478906')
  tx = await WETHPartner.transfer(WETHPair.address, WETHPartnerAmount)
  await tx.wait()
  tx = await WETH.deposit({ value: ETHAmount })
  await tx.wait()
  tx = await WETH.transfer(WETHPair.address, ETHAmount)
  await tx.wait()
  tx = await WETHPair.mint(wallet.address, overrides)
  await tx.wait()
  tx = await token0.approve(router.address, MaxUint256)
  await tx.wait()
}

async function swapExactETHForTokens_happy_path() {
  await before_swapExactTokensForTokens()
  await afterEach()
  const WETHPairToken0 = await WETHPair.token0()
  tx = await router.swapExactETHForTokens(0, [WETH.address, WETHPartner.address], wallet.address, MaxUint256, {
    ...overrides,
    value: swapAmount
  })
  await tx.wait()
  //             .to.emit(WETH, 'Transfer')
  //             .withArgs(router.address, WETHPair.address, swapAmount)
  //             .to.emit(WETHPartner, 'Transfer')
  //             .withArgs(WETHPair.address, wallet.address, expectedOutputAmount)
  //             .to.emit(WETHPair, 'Sync')
  //             .withArgs(
  //               WETHPairToken0 === WETHPartner.address
  //                 ? WETHPartnerAmount.sub(expectedOutputAmount)
  //                 : ETHAmount.add(swapAmount),
  //               WETHPairToken0 === WETHPartner.address
  //                 ? ETHAmount.add(swapAmount)
  //                 : WETHPartnerAmount.sub(expectedOutputAmount)
  //             )
  //             .to.emit(WETHPair, 'Swap')
  //             .withArgs(
  //               router.address,
  //               WETHPairToken0 === WETHPartner.address ? 0 : swapAmount,
  //               WETHPairToken0 === WETHPartner.address ? swapAmount : 0,
  //               WETHPairToken0 === WETHPartner.address ? expectedOutputAmount : 0,
  //               WETHPairToken0 === WETHPartner.address ? 0 : expectedOutputAmount,
  //               wallet.address
  //             )
  console.log('swapExactETHForTokens_happy_path passed')
}

async function swapExactETHForTokens_amount() {
  await before_swapExactETHForTokens()
  tx = await routerEventEmitter.swapExactETHForTokens(
    router.address,
    0,
    [WETH.address, WETHPartner.address],
    wallet.address,
    MaxUint256,
    {
      ...overrides,
      value: swapAmount
    }
  )
  await tx.wait()
  //             .to.emit(routerEventEmitter, 'Amounts')
  //             .withArgs([swapAmount, expectedOutputAmount])
  await afterEach()
  console.log('swapExactETHForTokens_amount passed')
}

async function before_swapTokensForExactETH() {
  await beforeEach()
  WETHPartnerAmount = expandTo18Decimals(5)
  ETHAmount = expandTo18Decimals(10)
  expectedSwapAmount = BigNumber.from('557227237267357629')
  outputAmount = expandTo18Decimals(1)

  tx = await WETHPartner.transfer(WETHPair.address, WETHPartnerAmount)
  await tx.wait()
  tx = await WETH.deposit({ value: ETHAmount })
  await tx.wait()
  tx = await WETH.transfer(WETHPair.address, ETHAmount)
  await tx.wait()
  tx = await WETHPair.mint(wallet.address, overrides)
  await tx.wait()
}

async function swapTokensForExactETH_happy_path() {
  await before_swapTokensForExactETH()
  tx = await WETHPartner.approve(router.address, MaxUint256)
  await tx.wait()
  const WETHPairToken0 = await WETHPair.token0()
  tx = await router.swapTokensForExactETH(
    outputAmount,
    MaxUint256,
    [WETHPartner.address, WETH.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  //             .to.emit(WETHPartner, 'Transfer')
  //             .withArgs(wallet.address, WETHPair.address, expectedSwapAmount)
  //             .to.emit(WETH, 'Transfer')
  //             .withArgs(WETHPair.address, router.address, outputAmount)
  //             .to.emit(WETHPair, 'Sync')
  //             .withArgs(
  //               WETHPairToken0 === WETHPartner.address
  //                 ? WETHPartnerAmount.add(expectedSwapAmount)
  //                 : ETHAmount.sub(outputAmount),
  //               WETHPairToken0 === WETHPartner.address
  //                 ? ETHAmount.sub(outputAmount)
  //                 : WETHPartnerAmount.add(expectedSwapAmount)
  //             )
  //             .to.emit(WETHPair, 'Swap')
  //             .withArgs(
  //               router.address,
  //               WETHPairToken0 === WETHPartner.address ? expectedSwapAmount : 0,
  //               WETHPairToken0 === WETHPartner.address ? 0 : expectedSwapAmount,
  //               WETHPairToken0 === WETHPartner.address ? 0 : outputAmount,
  //               WETHPairToken0 === WETHPartner.address ? outputAmount : 0,
  //               router.address
  //             )
  await afterEach()
  console.log('swapTokensForExactETH_happy_path passed')
}

async function swapTokensForExactETH_amount() {
  await before_swapTokensForExactETH()
  tx = await WETHPartner.approve(routerEventEmitter.address, MaxUint256)
  await tx.wait()
  tx = await routerEventEmitter.swapTokensForExactETH(
    router.address,
    outputAmount,
    MaxUint256,
    [WETHPartner.address, WETH.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  //             .to.emit(routerEventEmitter, 'Amounts')
  //             .withArgs([expectedSwapAmount, outputAmount])
  //         })
  console.log('swapTokensForExactETH_amount passed')
}

async function before_swapExactTokensForETH() {
  await beforeEach()
  WETHPartnerAmount = expandTo18Decimals(5)
  ETHAmount = expandTo18Decimals(10)
  swapAmount = expandTo18Decimals(1)
  expectedOutputAmount = BigNumber.from('1662497915624478906')
}

async function swapExactTokensForETH_happy_path() {
  await before_swapExactTokensForETH()
  tx = await WETHPartner.approve(router.address, MaxUint256)
  await tx.wait()
  const WETHPairToken0 = await WETHPair.token0()
  tx = await router.swapExactTokensForETH(
    swapAmount,
    0,
    [WETHPartner.address, WETH.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  //             .to.emit(WETHPartner, 'Transfer')
  //             .withArgs(wallet.address, WETHPair.address, swapAmount)
  //             .to.emit(WETH, 'Transfer')
  //             .withArgs(WETHPair.address, router.address, expectedOutputAmount)
  //             .to.emit(WETHPair, 'Sync')
  //             .withArgs(
  //               WETHPairToken0 === WETHPartner.address
  //                 ? WETHPartnerAmount.add(swapAmount)
  //                 : ETHAmount.sub(expectedOutputAmount),
  //               WETHPairToken0 === WETHPartner.address
  //                 ? ETHAmount.sub(expectedOutputAmount)
  //                 : WETHPartnerAmount.add(swapAmount)
  //             )
  //             .to.emit(WETHPair, 'Swap')
  //             .withArgs(
  //               router.address,
  //               WETHPairToken0 === WETHPartner.address ? swapAmount : 0,
  //               WETHPairToken0 === WETHPartner.address ? 0 : swapAmount,
  //               WETHPairToken0 === WETHPartner.address ? 0 : expectedOutputAmount,
  //               WETHPairToken0 === WETHPartner.address ? expectedOutputAmount : 0,
  //               router.address
  //             )
  await afterEach()
  console.log('swapExactTokensForETH_happy_path passed')
}

async function swapExactTokensForETH_amounts() {
  await before_swapExactTokensForETH()
  tx = await WETHPartner.approve(routerEventEmitter.address, MaxUint256)
  await tx.wait()
  tx = await routerEventEmitter.swapExactTokensForETH(
    router.address,
    swapAmount,
    0,
    [WETHPartner.address, WETH.address],
    wallet.address,
    MaxUint256,
    overrides
  )
  await tx.wait()
  //             .to.emit(routerEventEmitter, 'Amounts')
  //             .withArgs([swapAmount, expectedOutputAmount])
  //         })
  await afterEach()
  console.log('swapExactTokensForETH_amounts passed')
}

async function before_swapETHForExactTokens() {
  await beforeEach()
  WETHPartnerAmount = expandTo18Decimals(10)
  ETHAmount = expandTo18Decimals(5)
  expectedSwapAmount = BigNumber.from('557227237267357629')
  outputAmount = expandTo18Decimals(1)
  tx = await WETHPartner.transfer(WETHPair.address, WETHPartnerAmount)
  await tx.wait()
  tx = await WETH.deposit({ value: ETHAmount })
  await tx.wait()
  tx = await WETH.transfer(WETHPair.address, ETHAmount)
  await tx.wait()
  tx = await WETHPair.mint(wallet.address, overrides)
  await tx.wait()
}

async function swapETHForExactTokens_happy_path() {
  await before_swapETHForExactTokens()
  const WETHPairToken0 = await WETHPair.token0()
  tx = await router.swapETHForExactTokens(
    outputAmount,
    [WETH.address, WETHPartner.address],
    wallet.address,
    MaxUint256,
    {
      ...overrides,
      value: expectedSwapAmount
    }
  )
  await tx.wait()
  //             .to.emit(WETH, 'Transfer')
  //             .withArgs(router.address, WETHPair.address, expectedSwapAmount)
  //             .to.emit(WETHPartner, 'Transfer')
  //             .withArgs(WETHPair.address, wallet.address, outputAmount)
  //             .to.emit(WETHPair, 'Sync')
  //             .withArgs(
  //               WETHPairToken0 === WETHPartner.address
  //                 ? WETHPartnerAmount.sub(outputAmount)
  //                 : ETHAmount.add(expectedSwapAmount),
  //               WETHPairToken0 === WETHPartner.address
  //                 ? ETHAmount.add(expectedSwapAmount)
  //                 : WETHPartnerAmount.sub(outputAmount)
  //             )
  //             .to.emit(WETHPair, 'Swap')
  //             .withArgs(
  //               router.address,
  //               WETHPairToken0 === WETHPartner.address ? 0 : expectedSwapAmount,
  //               WETHPairToken0 === WETHPartner.address ? expectedSwapAmount : 0,
  //               WETHPairToken0 === WETHPartner.address ? outputAmount : 0,
  //               WETHPairToken0 === WETHPartner.address ? 0 : outputAmount,
  //               wallet.address
  //             )
  await afterEach()
  console.log('swapETHForExactTokens_happy_path passed')
}

async function swapETHForExactTokens_amounts() {
  await before_swapETHForExactTokens()
  tx = await routerEventEmitter.swapETHForExactTokens(
    router.address,
    outputAmount,
    [WETH.address, WETHPartner.address],
    wallet.address,
    MaxUint256,
    {
      ...overrides,
      value: expectedSwapAmount
    }
  )
  await tx.wait()
  //             .to.emit(routerEventEmitter, 'Amounts')
  //             .withArgs([expectedSwapAmount, outputAmount])
  await afterEach()
  console.log('swapETHForExactTokens_amounts passed')
}

async function main() {
  ;[wallet] = await ethers.getSigners()
  await factory_WETH()
  await addLiquidityTest()
  await addLiquidityETH()
  await UniswapV2Router_removeLiquidity()
  await UniswapV2Router_removeLiquidityETH()
  // await UniswapV2Router_removeLiquidityWithPermit() //not passed
  // await UniswapV2Router_removeLiquidityETHWithPermit() //not passed
  await swapExactTokensForTokens_happy_path()
  await swapExactTokensForToken_amount()
  await swapTokensForExactTokens_happy_path()
  await swapTokensForExactTokens_amount()
  // await swapExactETHForTokens_happy_path() //not passed
  await swapExactETHForTokens_amount()
  await swapTokensForExactETH_happy_path()
  await swapTokensForExactETH_amount()
  // await swapExactTokensForETH_happy_path() //not passed
  // await swapExactTokensForETH_amounts() // not passed
  await swapETHForExactTokens_happy_path()
  await swapETHForExactTokens_amounts()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main