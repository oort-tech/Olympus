import { ethers } from 'hardhat'
import { expect } from 'chai'
import { ERC20, PancakeFactory, PancakePair } from '../../typechain'
import { bigNumberify, expandTo18Decimals } from './shared/utilities'
import { BigNumber } from 'ethers/lib/ethers'

let factory: PancakeFactory
let token0: ERC20
let token1: ERC20
let pair: PancakePair

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

  await (await factory.createPair(token0.address, token1.address)).wait()
  const pairAddress = await factory.getPair(token0.address, token1.address)
  pair = await ethers.getContractAt('PancakePair', pairAddress)

  if (token0.address > token1.address) {
    const t = token0
    token0 = token1
    token1 = t
  }

  await wallet.sendTransaction({
    to: other.address,
    value: ethers.utils.parseEther('1.0'),
  })
}

async function testMint() {
  await beforeEach()
  const [wallet] = await ethers.getSigners()

  const token0Amount = expandTo18Decimals(1)
  const token1Amount = expandTo18Decimals(4)

  await (await token0.transfer(pair.address, token0Amount)).wait()
  await (await token1.transfer(pair.address, token1Amount)).wait()

  const expectedLiquidity = expandTo18Decimals(2)
  await expect(await pair.mint(wallet.address))
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
    .withArgs(wallet.address, token0Amount, token1Amount)

  expect(await pair.totalSupply()).to.eq(expectedLiquidity)
  expect(await pair.balanceOf(wallet.address)).to.eq(
    expectedLiquidity.sub(MINIMUM_LIQUIDITY)
  )
  expect(await token0.balanceOf(pair.address)).to.eq(token0Amount)
  expect(await token1.balanceOf(pair.address)).to.eq(token1Amount)
  const reserves = await pair.getReserves()
  expect(reserves[0]).to.eq(token0Amount)
  expect(reserves[1]).to.eq(token1Amount)
}

async function addLiquidity(token0Amount: BigNumber, token1Amount: BigNumber) {
  const [wallet] = await ethers.getSigners()

  await (await token0.transfer(pair.address, token0Amount)).wait()
  await (await token1.transfer(pair.address, token1Amount)).wait()
  await (await pair.mint(wallet.address)).wait()
}

async function testSwap() {
  const [wallet] = await ethers.getSigners()

  const swapTestCases: BigNumber[][] = [
    [1, 5, 10, '1663887962654218072'],
    [1, 10, 5, '453718857974177123'],

    [2, 5, 10, '2853058890794739851'],
    [2, 10, 5, '831943981327109036'],

    [1, 10, 10, '907437715948354246'],
    [1, 100, 100, '988138378977801540'],
    [1, 1000, 1000, '997004989020957084'],
  ].map((a) =>
    a.map((n) =>
      typeof n === 'string' ? bigNumberify(n) : expandTo18Decimals(n)
    )
  )

  for (let i = 0; i < swapTestCases.length; i++) {
    const [swapAmount, token0Amount, token1Amount, expectedOutputAmount] =
      swapTestCases[0]
    await beforeEach()
    await addLiquidity(token0Amount, token1Amount)
    await (await token0.transfer(pair.address, swapAmount)).wait()
    await expect(
      pair.swap(0, expectedOutputAmount.add(1), wallet.address, '0x')
    ).to.be.reverted
    await (
      await pair.swap(0, expectedOutputAmount, wallet.address, '0x')
    ).wait()
  }
}

async function testSwapToken0() {
  await beforeEach()
  const [wallet] = await ethers.getSigners()

  const token0Amount = expandTo18Decimals(5)
  const token1Amount = expandTo18Decimals(10)
  await addLiquidity(token0Amount, token1Amount)

  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('1662497915624478906')
  await (await token0.transfer(pair.address, swapAmount)).wait()
  await expect(pair.swap(0, expectedOutputAmount, wallet.address, '0x'))
    .to.emit(token1, 'Transfer')
    .withArgs(pair.address, wallet.address, expectedOutputAmount)
    .to.emit(pair, 'Sync')
    .withArgs(
      token0Amount.add(swapAmount),
      token1Amount.sub(expectedOutputAmount)
    )
    .to.emit(pair, 'Swap')
    .withArgs(
      wallet.address,
      swapAmount,
      0,
      0,
      expectedOutputAmount,
      wallet.address
    )

  const reserves = await pair.getReserves()
  expect(reserves[0]).to.eq(token0Amount.add(swapAmount))
  expect(reserves[1]).to.eq(token1Amount.sub(expectedOutputAmount))
  expect(await token0.balanceOf(pair.address)).to.eq(
    token0Amount.add(swapAmount)
  )
  expect(await token1.balanceOf(pair.address)).to.eq(
    token1Amount.sub(expectedOutputAmount)
  )
  const totalSupplyToken0 = await token0.totalSupply()
  const totalSupplyToken1 = await token1.totalSupply()
  expect(await token0.balanceOf(wallet.address)).to.eq(
    totalSupplyToken0.sub(token0Amount).sub(swapAmount)
  )
  expect(await token1.balanceOf(wallet.address)).to.eq(
    totalSupplyToken1.sub(token1Amount).add(expectedOutputAmount)
  )
}

async function testBurn() {
  await beforeEach()
  const [wallet] = await ethers.getSigners()

  const token0Amount = expandTo18Decimals(3)
  const token1Amount = expandTo18Decimals(3)
  await addLiquidity(token0Amount, token1Amount)

  const expectedLiquidity = expandTo18Decimals(3)
  await (
    await pair.transfer(pair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  ).wait()
  await expect(pair.burn(wallet.address))
    .to.emit(pair, 'Transfer')
    .withArgs(
      pair.address,
      ethers.constants.AddressZero,
      expectedLiquidity.sub(MINIMUM_LIQUIDITY)
    )
    .to.emit(token0, 'Transfer')
    .withArgs(pair.address, wallet.address, token0Amount.sub(1000))
    .to.emit(token1, 'Transfer')
    .withArgs(pair.address, wallet.address, token1Amount.sub(1000))
    .to.emit(pair, 'Sync')
    .withArgs(1000, 1000)
    .to.emit(pair, 'Burn')
    .withArgs(
      wallet.address,
      token0Amount.sub(1000),
      token1Amount.sub(1000),
      wallet.address
    )

  expect(await pair.balanceOf(wallet.address)).to.eq(0)
  expect(await pair.totalSupply()).to.eq(MINIMUM_LIQUIDITY)
  expect(await token0.balanceOf(pair.address)).to.eq(1000)
  expect(await token1.balanceOf(pair.address)).to.eq(1000)
  const totalSupplyToken0 = await token0.totalSupply()
  const totalSupplyToken1 = await token1.totalSupply()
  expect(await token0.balanceOf(wallet.address)).to.eq(
    totalSupplyToken0.sub(1000)
  )
  expect(await token1.balanceOf(wallet.address)).to.eq(
    totalSupplyToken1.sub(1000)
  )
}

async function testFeeToOff() {
  await beforeEach()
  const [wallet] = await ethers.getSigners()

  const token0Amount = expandTo18Decimals(1000)
  const token1Amount = expandTo18Decimals(1000)
  await addLiquidity(token0Amount, token1Amount)

  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('996006981039903216')
  await (await token1.transfer(pair.address, swapAmount)).wait()
  await (await pair.swap(expectedOutputAmount, 0, wallet.address, '0x')).wait()

  const expectedLiquidity = expandTo18Decimals(1000)
  await (
    await pair.transfer(pair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  ).wait()
  await (await pair.burn(wallet.address)).wait()
  expect(await pair.totalSupply()).to.eq(MINIMUM_LIQUIDITY)
}

async function testFeeToOn() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  await factory.setFeeTo(other.address)

  const token0Amount = expandTo18Decimals(1000)
  const token1Amount = expandTo18Decimals(1000)
  await addLiquidity(token0Amount, token1Amount)

  const swapAmount = expandTo18Decimals(1)
  const expectedOutputAmount = bigNumberify('996006981039903216')
  await (await token1.transfer(pair.address, swapAmount)).wait()
  await (await pair.swap(expectedOutputAmount, 0, wallet.address, '0x')).wait()

  const expectedLiquidity = expandTo18Decimals(1000)
  await (
    await pair.transfer(pair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
  ).wait()
  await (await pair.burn(wallet.address)).wait()
  expect(await pair.totalSupply()).to.eq(
    MINIMUM_LIQUIDITY.add('374625795658571')
  )
  expect(await pair.balanceOf(other.address)).to.eq('374625795658571')

  // using 1000 here instead of the symbolic MINIMUM_LIQUIDITY because the amounts only happen to be equal...
  // ...because the initial liquidity amounts were equal
  expect(await token0.balanceOf(pair.address)).to.eq(
    bigNumberify(1000).add('374252525546167')
  )
  expect(await token1.balanceOf(pair.address)).to.eq(
    bigNumberify(1000).add('375000280969452')
  )
}

export async function main() {
  console.log('1. testMint...')
  await testMint()
  // console.log('2. testSwap...')
  // await testSwap()
  console.log('3. testSwapToken0...')
  await testSwapToken0()
  console.log('4. testBurn...')
  await testBurn()
  console.log('5. testFeeToOff...')
  await testFeeToOff()
  console.log('6. testFeeToOn...')
  await testFeeToOn()
}

// main().catch((error) => {
//   console.error(error)
//   process.exitCode = 1
// })
