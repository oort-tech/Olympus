import { ethers, waffle } from 'hardhat'
import { expect } from 'chai'
import {
  ERC20,
  PancakeFactory,
  PancakePair,
  PancakeRouter,
  RouterEventEmitter,
  WETH9,
} from '../../typechain'
import { bigNumberify, expandTo18Decimals, getApprovalDigest } from './shared/utilities'
import { BigNumber } from 'ethers/lib/ethers'
import { ecsign } from 'ethereumjs-util'

let factory: PancakeFactory
let token0: ERC20
let token1: ERC20
let weth: WETH9
let wethPartner: ERC20
let pair: PancakePair
let wethPair: PancakePair
let router: PancakeRouter
let routerEventEmitter: RouterEventEmitter

const MINIMUM_LIQUIDITY = bigNumberify(10).pow(3)

async function beforeEach() {
  const [wallet, other] = await ethers.getSigners()
  const PancakeFactory = await ethers.getContractFactory('PancakeFactory')
  factory = await PancakeFactory.deploy(wallet.address)
  await factory.deployed()

  const ERC20 = await ethers.getContractFactory('ERC20')
  token0 = await ERC20.deploy(expandTo18Decimals(10000))
  await token0.deployed()
  token1 = await ERC20.deploy(expandTo18Decimals(10000))
  await token1.deployed()
  wethPartner = await ERC20.deploy(expandTo18Decimals(10000))
  await wethPartner.deployed()

  const WETH = await ethers.getContractFactory('WETH9')
  weth = await WETH.deploy()

  await (await factory.createPair(token0.address, token1.address)).wait()
  let pairAddress = await factory.getPair(token0.address, token1.address)
  pair = await ethers.getContractAt('PancakePair', pairAddress)

  if (token0.address !== (await pair.token0())) {
    const t = token0
    token0 = token1
    token1 = t
  }

  await (await factory.createPair(weth.address, wethPartner.address)).wait()
  pairAddress = await factory.getPair(weth.address, wethPartner.address)
  wethPair = await ethers.getContractAt('PancakePair', pairAddress)

  const PancakeRouter = await ethers.getContractFactory('PancakeRouter')
  router = await PancakeRouter.deploy(factory.address, weth.address)

  const RouterEventEmitter = await ethers.getContractFactory(
    'RouterEventEmitter'
  )
  routerEventEmitter = await RouterEventEmitter.deploy()

  await wallet.sendTransaction({
    to: other.address,
    value: ethers.utils.parseEther('1.0'),
  })
}

async function afterEach() {
  expect(await waffle.provider.getBalance(router.address)).to.eq(
    ethers.constants.Zero
  )
}

async function testFactoryAndWETH() {
  await beforeEach()

  expect(await router.factory()).to.eq(factory.address)
  expect(await router.WETH()).to.eq(weth.address)

  await afterEach()
}

async function testAddLiquidity() {
  await beforeEach()

  const [wallet, other] = await ethers.getSigners()
  const token0Amount = expandTo18Decimals(1)
  const token1Amount = expandTo18Decimals(4)

  const expectedLiquidity = expandTo18Decimals(2)
  await (
    await token0.approve(router.address, ethers.constants.MaxUint256)
  ).wait()
  await (
    await token1.approve(router.address, ethers.constants.MaxUint256)
  ).wait()

  await expect(
    router.addLiquidity(
      token0.address,
      token1.address,
      token0Amount,
      token1Amount,
      0,
      0,
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(token0, 'Transfer')
    .withArgs(wallet.address, pair.address, token0Amount)
    .to.emit(token1, 'Transfer')
    .withArgs(wallet.address, pair.address, token1Amount)
    .to.emit(pair, 'Transfer')
    .withArgs(
      ethers.constants.AddressZero,
      ethers.constants.AddressZero,
      MINIMUM_LIQUIDITY
    )
    .to.emit(pair, 'Transfer')
    .withArgs(
      ethers.constants.AddressZero,
      wallet.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY)
    )
    .to.emit(pair, 'Sync')
    .withArgs(token0Amount, token1Amount)
    .to.emit(pair, 'Mint')
    .withArgs(router.address, token0Amount, token1Amount)

  expect(await pair.balanceOf(wallet.address)).to.eq(
    expectedLiquidity.sub(MINIMUM_LIQUIDITY)
  )

  await afterEach()
}

async function testAddLiquidityETH() {
  await beforeEach()

  const [wallet, other] = await ethers.getSigners()

  const WETHPartnerAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)

  const expectedLiquidity = expandTo18Decimals(2)
  const WETHPairToken0 = await wethPair.token0()
  await (
    await wethPartner.approve(router.address, ethers.constants.MaxUint256)
  ).wait()
  await expect(
    router.addLiquidityETH(
      wethPartner.address,
      WETHPartnerAmount,
      WETHPartnerAmount,
      ETHAmount,
      wallet.address,
      ethers.constants.MaxUint256,
      { value: ETHAmount }
    )
  )
    .to.emit(wethPair, 'Transfer')
    .withArgs(
      ethers.constants.AddressZero,
      ethers.constants.AddressZero,
      MINIMUM_LIQUIDITY
    )
    .to.emit(wethPair, 'Transfer')
    .withArgs(
      ethers.constants.AddressZero,
      wallet.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY)
    )
    .to.emit(wethPair, 'Sync')
    .withArgs(
      WETHPairToken0 === wethPartner.address ? WETHPartnerAmount : ETHAmount,
      WETHPairToken0 === wethPartner.address ? ETHAmount : WETHPartnerAmount
    )
    .to.emit(wethPair, 'Mint')
    .withArgs(
      router.address,
      WETHPairToken0 === wethPartner.address ? WETHPartnerAmount : ETHAmount,
      WETHPairToken0 === wethPartner.address ? ETHAmount : WETHPartnerAmount
    )

  expect(await wethPair.balanceOf(wallet.address)).to.eq(
    expectedLiquidity.sub(MINIMUM_LIQUIDITY)
  )

  await afterEach()
}

async function addLiquidity(token0Amount: BigNumber, token1Amount: BigNumber) {
  const [wallet, other] = await ethers.getSigners()
  await (await token0.transfer(pair.address, token0Amount)).wait()
  await (await token1.transfer(pair.address, token1Amount)).wait()
  await (await pair.mint(wallet.address)).wait()
}

async function testRemoveLiquidity() {
  await beforeEach()

  const [wallet, other] = await ethers.getSigners()

  const token0Amount = expandTo18Decimals(1)
  const token1Amount = expandTo18Decimals(4)
  await addLiquidity(token0Amount, token1Amount)

  const expectedLiquidity = expandTo18Decimals(2)
  await (await pair.approve(router.address, ethers.constants.MaxUint256)).wait()
  await expect(
    router.removeLiquidity(
      token0.address,
      token1.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY),
      0,
      0,
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(pair, 'Transfer')
    .withArgs(
      wallet.address,
      pair.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY)
    )
    .to.emit(pair, 'Transfer')
    .withArgs(
      pair.address,
      ethers.constants.AddressZero,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY)
    )
    .to.emit(token0, 'Transfer')
    .withArgs(pair.address, wallet.address, token0Amount.sub(500))
    .to.emit(token1, 'Transfer')
    .withArgs(pair.address, wallet.address, token1Amount.sub(2000))
    .to.emit(pair, 'Sync')
    .withArgs(500, 2000)
    .to.emit(pair, 'Burn')
    .withArgs(
      router.address,
      token0Amount.sub(500),
      token1Amount.sub(2000),
      wallet.address
    )

  expect(await pair.balanceOf(wallet.address)).to.eq(0)
  const totalSupplyToken0 = await token0.totalSupply()
  const totalSupplyToken1 = await token1.totalSupply()
  expect(await token0.balanceOf(wallet.address)).to.eq(
    totalSupplyToken0.sub(500)
  )
  expect(await token1.balanceOf(wallet.address)).to.eq(
    totalSupplyToken1.sub(2000)
  )

  await afterEach()
}

async function testRemoveLiquidityETH() {
  await beforeEach()

  const [wallet, other] = await ethers.getSigners()

  const WETHPartnerAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)
  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  const expectedLiquidity = expandTo18Decimals(2)
  const WETHPairToken0 = await wethPair.token0()
  await (
    await wethPair.approve(router.address, ethers.constants.MaxUint256)
  ).wait()
  await expect(
    router.removeLiquidityETH(
      wethPartner.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY),
      0,
      0,
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(wethPair, 'Transfer')
    .withArgs(
      wallet.address,
      wethPair.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY)
    )
    .to.emit(wethPair, 'Transfer')
    .withArgs(
      wethPair.address,
      ethers.constants.AddressZero,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY)
    )
    .to.emit(weth, 'Transfer')
    .withArgs(wethPair.address, router.address, ETHAmount.sub(2000))
    .to.emit(wethPartner, 'Transfer')
    .withArgs(wethPair.address, router.address, WETHPartnerAmount.sub(500))
    .to.emit(wethPartner, 'Transfer')
    .withArgs(router.address, wallet.address, WETHPartnerAmount.sub(500))
    .to.emit(wethPair, 'Sync')
    .withArgs(
      WETHPairToken0 === wethPartner.address ? 500 : 2000,
      WETHPairToken0 === wethPartner.address ? 2000 : 500
    )
    .to.emit(wethPair, 'Burn')
    .withArgs(
      router.address,
      WETHPairToken0 === wethPartner.address
        ? WETHPartnerAmount.sub(500)
        : ETHAmount.sub(2000),
      WETHPairToken0 === wethPartner.address
        ? ETHAmount.sub(2000)
        : WETHPartnerAmount.sub(500),
      router.address
    )

  expect(await wethPair.balanceOf(wallet.address)).to.eq(0)
  const totalSupplyWETHPartner = await wethPartner.totalSupply()
  const totalSupplyWETH = await weth.totalSupply()
  expect(await wethPartner.balanceOf(wallet.address)).to.eq(
    totalSupplyWETHPartner.sub(500)
  )
  expect(await weth.balanceOf(wallet.address)).to.eq(totalSupplyWETH.sub(2000))

  await afterEach()
}

async function testRemoveLiquidityWithPermit() {
  await beforeEach()

  const [wallet, other] = await ethers.getSigners()

  const token0Amount = expandTo18Decimals(1)
  const token1Amount = expandTo18Decimals(4)
  await addLiquidity(token0Amount, token1Amount)

  const expectedLiquidity = expandTo18Decimals(2)

  const nonce = await pair.nonces(wallet.address)
  const digest = await getApprovalDigest(
    pair,
    {
      owner: wallet.address,
      spender: router.address,
      value: expectedLiquidity.sub(MINIMUM_LIQUIDITY),
    },
    nonce,
    ethers.constants.MaxUint256
  )

  const { v, r, s } = ecsign(
    Buffer.from(digest.slice(2), 'hex'),
    Buffer.from(
      process.env.CCNBETA_PRIVATE_KEY ? process.env.CCNBETA_PRIVATE_KEY : '',
      'hex'
    )
  )

  await (
    await router.removeLiquidityWithPermit(
      token0.address,
      token1.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY),
      0,
      0,
      wallet.address,
      ethers.constants.MaxUint256,
      false,
      v,
      r,
      s
    )
  ).wait()

  await afterEach()
}

async function testAddRemoveLiquidityETHWithPermit() {
  await beforeEach()

  const [wallet, other] = await ethers.getSigners()

  const WETHPartnerAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)
  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  const expectedLiquidity = expandTo18Decimals(2)

  const nonce = await wethPair.nonces(wallet.address)
  const digest = await getApprovalDigest(
    wethPair,
    {
      owner: wallet.address,
      spender: router.address,
      value: expectedLiquidity.sub(MINIMUM_LIQUIDITY),
    },
    nonce,
    ethers.constants.MaxUint256
  )

  const { v, r, s } = ecsign(
    Buffer.from(digest.slice(2), 'hex'),
    Buffer.from(
      process.env.CCNBETA_PRIVATE_KEY ? process.env.CCNBETA_PRIVATE_KEY : '',
      'hex'
    )
  )

  await (
    await router.removeLiquidityETHWithPermit(
      wethPartner.address,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY),
      0,
      0,
      wallet.address,
      ethers.constants.MaxUint256,
      false,
      v,
      r,
      s
    )
  ).wait()

  await afterEach()
}

async function testSwapExactTokensForTokensHappyPath() {
  await beforeEach()

  const token0Amount = expandTo18Decimals(5)
  const token1Amount = expandTo18Decimals(10)
  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('1663887962654218072')

  await addLiquidity(token0Amount, token1Amount)
  await (
    await token0.approve(router.address, ethers.constants.MaxUint256)
  ).wait()

  const [wallet, other] = await ethers.getSigners()

  await expect(
    router.swapExactTokensForTokens(
      swapAmount,
      0,
      [token0.address, token1.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(token0, 'Transfer')
    .withArgs(wallet.address, pair.address, swapAmount)
    .to.emit(token1, 'Transfer')
    .withArgs(pair.address, wallet.address, expectedOutputAmount)
    .to.emit(pair, 'Sync')
    .withArgs(
      token0Amount.add(swapAmount),
      token1Amount.sub(expectedOutputAmount)
    )
    .to.emit(pair, 'Swap')
    .withArgs(
      router.address,
      swapAmount,
      0,
      0,
      expectedOutputAmount,
      wallet.address
    )
}

async function testSwapExactTokensForTokensAmounts() {
  await beforeEach()

  const token0Amount = expandTo18Decimals(5)
  const token1Amount = expandTo18Decimals(10)
  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('1663887962654218072')

  await addLiquidity(token0Amount, token1Amount)
  await (
    await token0.approve(router.address, ethers.constants.MaxUint256)
  ).wait()

  const [wallet, other] = await ethers.getSigners()

  await (
    await token0.approve(
      routerEventEmitter.address,
      ethers.constants.MaxUint256
    )
  ).wait()
  await expect(
    routerEventEmitter.swapExactTokensForTokens(
      router.address,
      swapAmount,
      0,
      [token0.address, token1.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(routerEventEmitter, 'Amounts')
    .withArgs([swapAmount, expectedOutputAmount])
}

async function testSwapTokensForExactTokensHappyPath() {
  await beforeEach()

  const token0Amount = expandTo18Decimals(5)
  const token1Amount = expandTo18Decimals(10)
  const expectedSwapAmount = bigNumberify('556668893342240036')
  const outputAmount = expandTo18Decimals(1)

  await addLiquidity(token0Amount, token1Amount)

  const [wallet, other] = await ethers.getSigners()

  await (
    await token0.approve(router.address, ethers.constants.MaxUint256)
  ).wait()
  await expect(
    router.swapTokensForExactTokens(
      outputAmount,
      ethers.constants.MaxUint256,
      [token0.address, token1.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(token0, 'Transfer')
    .withArgs(wallet.address, pair.address, expectedSwapAmount)
    .to.emit(token1, 'Transfer')
    .withArgs(pair.address, wallet.address, outputAmount)
    .to.emit(pair, 'Sync')
    .withArgs(
      token0Amount.add(expectedSwapAmount),
      token1Amount.sub(outputAmount)
    )
    .to.emit(pair, 'Swap')
    .withArgs(
      router.address,
      expectedSwapAmount,
      0,
      0,
      outputAmount,
      wallet.address
    )
}

async function testSwapTokensForExactTokensAmounts() {
  await beforeEach()

  const token0Amount = expandTo18Decimals(5)
  const token1Amount = expandTo18Decimals(10)
  const expectedSwapAmount = bigNumberify('556668893342240036')
  const outputAmount = expandTo18Decimals(1)

  await addLiquidity(token0Amount, token1Amount)

  const [wallet, other] = await ethers.getSigners()

  await (
    await token0.approve(
      routerEventEmitter.address,
      ethers.constants.MaxUint256
    )
  ).wait()
  await expect(
    routerEventEmitter.swapTokensForExactTokens(
      router.address,
      outputAmount,
      ethers.constants.MaxUint256,
      [token0.address, token1.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(routerEventEmitter, 'Amounts')
    .withArgs([expectedSwapAmount, outputAmount])
}

async function testSwapExactETHForTokensHappyPath() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(10)
  const ETHAmount = expandTo18Decimals(5)
  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('1663887962654218072')

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  await (
    await token0.approve(router.address, ethers.constants.MaxUint256)
  ).wait()

  const WETHPairToken0 = await wethPair.token0()
  await expect(
    router.swapExactETHForTokens(
      0,
      [weth.address, wethPartner.address],
      wallet.address,
      ethers.constants.MaxUint256,
      { value: swapAmount }
    )
  )
    .to.emit(weth, 'Transfer')
    .withArgs(router.address, wethPair.address, swapAmount)
    .to.emit(wethPartner, 'Transfer')
    .withArgs(wethPair.address, wallet.address, expectedOutputAmount)
    .to.emit(wethPair, 'Sync')
    .withArgs(
      WETHPairToken0 === wethPartner.address
        ? WETHPartnerAmount.sub(expectedOutputAmount)
        : ETHAmount.add(swapAmount),
      WETHPairToken0 === wethPartner.address
        ? ETHAmount.add(swapAmount)
        : WETHPartnerAmount.sub(expectedOutputAmount)
    )
    .to.emit(wethPair, 'Swap')
    .withArgs(
      router.address,
      WETHPairToken0 === wethPartner.address ? 0 : swapAmount,
      WETHPairToken0 === wethPartner.address ? swapAmount : 0,
      WETHPairToken0 === wethPartner.address ? expectedOutputAmount : 0,
      WETHPairToken0 === wethPartner.address ? 0 : expectedOutputAmount,
      wallet.address
    )
}

async function testSwapExactETHForTokensAmounts() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(10)
  const ETHAmount = expandTo18Decimals(5)
  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('1663887962654218072')

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  await (
    await token0.approve(router.address, ethers.constants.MaxUint256)
  ).wait()

  await expect(
    routerEventEmitter.swapExactETHForTokens(
      router.address,
      0,
      [weth.address, wethPartner.address],
      wallet.address,
      ethers.constants.MaxUint256,
      {
        value: swapAmount,
      }
    )
  )
    .to.emit(routerEventEmitter, 'Amounts')
    .withArgs([swapAmount, expectedOutputAmount])
}

async function testSwapTokensForExactETHHappyPath() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(5)
  const ETHAmount = expandTo18Decimals(10)
  const expectedSwapAmount = bigNumberify('556668893342240036')
  const outputAmount = expandTo18Decimals(1)

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  await (
    await wethPartner.approve(router.address, ethers.constants.MaxUint256)
  ).wait()
  const WETHPairToken0 = await wethPair.token0()
  await expect(
    router.swapTokensForExactETH(
      outputAmount,
      ethers.constants.MaxUint256,
      [wethPartner.address, weth.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(wethPartner, 'Transfer')
    .withArgs(wallet.address, wethPair.address, expectedSwapAmount)
    .to.emit(weth, 'Transfer')
    .withArgs(wethPair.address, router.address, outputAmount)
    .to.emit(wethPair, 'Sync')
    .withArgs(
      WETHPairToken0 === wethPartner.address
        ? WETHPartnerAmount.add(expectedSwapAmount)
        : ETHAmount.sub(outputAmount),
      WETHPairToken0 === wethPartner.address
        ? ETHAmount.sub(outputAmount)
        : WETHPartnerAmount.add(expectedSwapAmount)
    )
    .to.emit(wethPair, 'Swap')
    .withArgs(
      router.address,
      WETHPairToken0 === wethPartner.address ? expectedSwapAmount : 0,
      WETHPairToken0 === wethPartner.address ? 0 : expectedSwapAmount,
      WETHPairToken0 === wethPartner.address ? 0 : outputAmount,
      WETHPairToken0 === wethPartner.address ? outputAmount : 0,
      router.address
    )
}

async function testSwapTokensForExactETHAmounts() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(5)
  const ETHAmount = expandTo18Decimals(10)
  const expectedSwapAmount = bigNumberify('556668893342240036')
  const outputAmount = expandTo18Decimals(1)

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  await (
    await wethPartner.approve(
      routerEventEmitter.address,
      ethers.constants.MaxUint256
    )
  ).wait()
  await expect(
    routerEventEmitter.swapTokensForExactETH(
      router.address,
      outputAmount,
      ethers.constants.MaxUint256,
      [wethPartner.address, weth.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(routerEventEmitter, 'Amounts')
    .withArgs([expectedSwapAmount, outputAmount])
}

async function testSwapExactTokensForETHHapyPath() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(5)
  const ETHAmount = expandTo18Decimals(10)
  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('1663887962654218072')

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  await (
    await wethPartner.approve(router.address, ethers.constants.MaxUint256)
  ).wait()
  const WETHPairToken0 = await wethPair.token0()
  await expect(
    router.swapExactTokensForETH(
      swapAmount,
      0,
      [wethPartner.address, weth.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(wethPartner, 'Transfer')
    .withArgs(wallet.address, wethPair.address, swapAmount)
    .to.emit(weth, 'Transfer')
    .withArgs(wethPair.address, router.address, expectedOutputAmount)
    .to.emit(wethPair, 'Sync')
    .withArgs(
      WETHPairToken0 === wethPartner.address
        ? WETHPartnerAmount.add(swapAmount)
        : ETHAmount.sub(expectedOutputAmount),
      WETHPairToken0 === wethPartner.address
        ? ETHAmount.sub(expectedOutputAmount)
        : WETHPartnerAmount.add(swapAmount)
    )
    .to.emit(wethPair, 'Swap')
    .withArgs(
      router.address,
      WETHPairToken0 === wethPartner.address ? swapAmount : 0,
      WETHPairToken0 === wethPartner.address ? 0 : swapAmount,
      WETHPairToken0 === wethPartner.address ? 0 : expectedOutputAmount,
      WETHPairToken0 === wethPartner.address ? expectedOutputAmount : 0,
      router.address
    )
}

async function testSwapExactTokensForETHAmounts() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(5)
  const ETHAmount = expandTo18Decimals(10)
  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('1663887962654218072')

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  await (
    await wethPartner.approve(
      routerEventEmitter.address,
      ethers.constants.MaxUint256
    )
  ).wait()
  await expect(
    routerEventEmitter.swapExactTokensForETH(
      router.address,
      swapAmount,
      0,
      [wethPartner.address, weth.address],
      wallet.address,
      ethers.constants.MaxUint256
    )
  )
    .to.emit(routerEventEmitter, 'Amounts')
    .withArgs([swapAmount, expectedOutputAmount])
}

async function testSwapETHForExactTokensHappyPath() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(10)
  const ETHAmount = expandTo18Decimals(5)
  const expectedSwapAmount = bigNumberify('556668893342240036')
  const outputAmount = expandTo18Decimals(1)

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  const WETHPairToken0 = await wethPair.token0()

  await expect(
    router.swapETHForExactTokens(
      outputAmount,
      [weth.address, wethPartner.address],
      wallet.address,
      ethers.constants.MaxUint256,
      {
        value: expectedSwapAmount,
      }
    )
  )
    .to.emit(weth, 'Transfer')
    .withArgs(router.address, wethPair.address, expectedSwapAmount)
    .to.emit(wethPartner, 'Transfer')
    .withArgs(wethPair.address, wallet.address, outputAmount)
    .to.emit(wethPair, 'Sync')
    .withArgs(
      WETHPairToken0 === wethPartner.address
        ? WETHPartnerAmount.sub(outputAmount)
        : ETHAmount.add(expectedSwapAmount),
      WETHPairToken0 === wethPartner.address
        ? ETHAmount.add(expectedSwapAmount)
        : WETHPartnerAmount.sub(outputAmount)
    )
    .to.emit(wethPair, 'Swap')
    .withArgs(
      router.address,
      WETHPairToken0 === wethPartner.address ? 0 : expectedSwapAmount,
      WETHPairToken0 === wethPartner.address ? expectedSwapAmount : 0,
      WETHPairToken0 === wethPartner.address ? outputAmount : 0,
      WETHPairToken0 === wethPartner.address ? 0 : outputAmount,
      wallet.address
    )
}

async function testSwapETHForExactTokensAmounts() {
  await beforeEach()

  const WETHPartnerAmount = expandTo18Decimals(10)
  const ETHAmount = expandTo18Decimals(5)
  const expectedSwapAmount = bigNumberify('556668893342240036')
  const outputAmount = expandTo18Decimals(1)

  const [wallet, other] = await ethers.getSigners()

  await (await wethPartner.transfer(wethPair.address, WETHPartnerAmount)).wait()
  await (await weth.deposit({ value: ETHAmount })).wait()
  await (await weth.transfer(wethPair.address, ETHAmount)).wait()
  await (await wethPair.mint(wallet.address)).wait()

  await expect(
    routerEventEmitter.swapETHForExactTokens(
      router.address,
      outputAmount,
      [weth.address, wethPartner.address],
      wallet.address,
      ethers.constants.MaxUint256,
      {
        value: expectedSwapAmount,
      }
    )
  )
    .to.emit(routerEventEmitter, 'Amounts')
    .withArgs([expectedSwapAmount, outputAmount])
}

async function testGetAmountsOut() {
  await beforeEach()

  const [wallet, other] = await ethers.getSigners()

  await (
    await token0.approve(router.address, ethers.constants.MaxUint256)
  ).wait()
  await (
    await token1.approve(router.address, ethers.constants.MaxUint256)
  ).wait()

  await (
    await router.addLiquidity(
      token0.address,
      token1.address,
      bigNumberify(10000),
      bigNumberify(10000),
      0,
      0,
      wallet.address,
      ethers.constants.MaxUint256
    )
  ).wait()

  // await expect(router.getAmountsOut(bigNumberify(2), [token0.address])).to.be.revertedWith(
  //   'UniswapV2Library: INVALID_PATH'
  // )
  const path = [token0.address, token1.address]
  expect(await router.getAmountsOut(bigNumberify(2), path)).to.deep.eq([
    bigNumberify(2),
    bigNumberify(1),
  ])
}

export async function main() {
  console.log('1. testFactoryAndWETH...')
  await testFactoryAndWETH()
  console.log('2. testAddLiquidity...')
  await testAddLiquidity()
  console.log('3. testAddLiquidityETH...')
  await testAddLiquidityETH()
  console.log('4. testRemoveLiquidity...')
  await testRemoveLiquidity()
  console.log('5. testRemoveLiquidityETH...')
  await testRemoveLiquidityETH()
  // console.log('6. testRemoveLiquidityWithPermit...')
  // await testRemoveLiquidityWithPermit()
  // console.log('7. testAddRemoveLiquidityETHWithPermit...')
  // await testAddRemoveLiquidityETHWithPermit()
  console.log('8. testSwapExactTokensForTokensHappyPath...')
  await testSwapExactTokensForTokensHappyPath()
  console.log('9. testSwapExactTokensForTokensAmounts...')
  await testSwapExactTokensForTokensAmounts()
  console.log('10. testSwapTokensForExactTokensHappyPath...')
  await testSwapTokensForExactTokensHappyPath()
  console.log('11. testSwapTokensForExactTokensAmounts...')
  await testSwapTokensForExactTokensAmounts()
  console.log('12. testSwapExactETHForTokensHappyPath...')
  await testSwapExactETHForTokensHappyPath()
  console.log('13. testSwapExactETHForTokensAmounts...')
  await testSwapExactETHForTokensAmounts()
  console.log('14. testSwapTokensForExactETHHappyPath...')
  await testSwapTokensForExactETHHappyPath()
  console.log('15. testSwapTokensForExactETHAmounts...')
  await testSwapTokensForExactETHAmounts()
  console.log('16. testSwapExactTokensForETHHapyPath...')
  await testSwapExactTokensForETHHapyPath()
  console.log('17. testSwapExactTokensForETHAmounts...')
  await testSwapExactTokensForETHAmounts()
  // console.log('18. testSwapETHForExactTokensHappyPath...')
  // await testSwapETHForExactTokensHappyPath()
  // console.log('19. testSwapETHForExactTokensAmounts...')
  // await testSwapETHForExactTokensAmounts()
  console.log('20. testGetAmountsOut...')
  await testGetAmountsOut()
}

// main().catch((error) => {
//   console.error(error)
//   process.exitCode = 1
// })
