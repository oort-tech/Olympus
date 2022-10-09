const { ethers } = require('hardhat')
const utils = ethers.utils
const { defaultAbiCoder, toUtf8Bytes, solidityPack, keccak256, hexlify, getAddress } = utils
const assert = require('assert')

const { main: create_factory } = require('./3_Factory')
const { expandTo18Decimals, mineBlock } = require('../utils.js')
const { BigNumber } = require('ethers')

const MINIMUM_LIQUIDITY = BigNumber.from(10).pow(3)
let tx, receipt, events, pair, factory, UniswapV2ERC20, UniswapV2Pair, UniswapV2Factory, token0, token1

const provider = ethers.provider

async function before() {
  const {uniswapV2Factory_address, uniswapV2ERC20, uniswapV2ERC20_1, pair_address} = await create_factory()
  pair = await UniswapV2Pair.attach(pair_address)
  token0 = await UniswapV2ERC20.attach(uniswapV2ERC20)
  token1 = await UniswapV2ERC20.attach(uniswapV2ERC20_1)
  factory = await UniswapV2Factory.attach(uniswapV2Factory_address)
}
async function main() {
  const [wallet, other] = await ethers.getSigners()
  UniswapV2ERC20 = await ethers.getContractFactory('UniswapV2ERC20')
  UniswapV2Pair = await ethers.getContractFactory('UniswapV2Pair')
  UniswapV2Factory = await ethers.getContractFactory('UniswapV2Factory')

  async function first_mint() {
    await before()
    const token0Amount = expandTo18Decimals(1)
    const token1Amount = expandTo18Decimals(4)

    tx = await token0.transfer(pair.address, token0Amount)
    await tx.wait()
    tx = await token1.transfer(pair.address, token1Amount)
    await tx.wait()

    const expectedLiquidity = expandTo18Decimals(2)

    totalSupply = await pair.totalSupply()

    tx = await pair.mint(wallet.address)
    receipt = await tx.wait()
    events = receipt.events

    let event_transfer, event_sync, event_mint
    if (totalSupply.toString() == '0') {
      assert.equal(events[0].event, 'Transfer')
      assert.equal(events[0].args.from, '0x0000000000000000000000000000000000000000')
      assert.equal(events[0].args.to, '0x0000000000000000000000000000000000000000')
      assert.equal(events[0].args.value.toString(), MINIMUM_LIQUIDITY.toString())

      event_transfer = events[1]
      event_sync = events[2]
      event_mint = events[3]
    } else {
      event_transfer = events[0]
      event_sync = events[1]
      event_mint = events[2]
    }

    assert.equal(event_transfer.event, 'Transfer')
    assert.equal(event_transfer.args.from, '0x0000000000000000000000000000000000000000')
    assert.equal(event_transfer.args.to, wallet.address)
    assert.equal(event_transfer.args.value.toString(), expectedLiquidity.sub(MINIMUM_LIQUIDITY).toString())

    assert.equal(event_sync.event, 'Sync')
    assert.equal(event_sync.args.reserve0.toString(), token0Amount.toString())
    assert.equal(event_sync.args.reserve1.toString(), token1Amount.toString())

    assert.equal(event_mint.event, 'Mint')
    assert.equal(event_mint.args.sender, wallet.address)
    assert.equal(event_mint.args.amount0.toString(), token0Amount.toString())
    assert.equal(event_mint.args.amount1.toString(), token1Amount.toString())

    assert.equal((await pair.totalSupply()).toString(), expectedLiquidity.toString())
    assert.equal((await pair.balanceOf(wallet.address)).toString(), expectedLiquidity.sub(MINIMUM_LIQUIDITY).toString())
    assert.equal((await token0.balanceOf(pair.address)).toString(), token0Amount.toString())
    assert.equal((await token1.balanceOf(pair.address)).toString(), token1Amount.toString())

    const reserves = await pair.getReserves()
    assert.equal(reserves[0].toString(), token0Amount.toString())
    assert.equal(reserves[1].toString(), token1Amount.toString())
    console.log('First Mint Passed')
  }

  async function addLiquidity(token0Amount, token1Amount) {
    tx = await token0.transfer(pair.address, token0Amount)
    await tx.wait()
    tx = await token1.transfer(pair.address, token1Amount)
    await tx.wait()
    tx = await pair.mint(wallet.address)
    await tx.wait()
  }

  async function swaptest_1() {
    await before()
    const swapTestCases = [
      [1, 5, 10, '1662497915624478906'],
    ].map(a => a.map(n => (typeof n === 'string' ? BigNumber.from(n) : expandTo18Decimals(n))))

    await swapTestCases_sub(swapTestCases[0])
    
    console.log('swapTestCases Passed')
  }

  async function swapTestCases_sub(swapTestCase) {
    const [swapAmount, token0Amount, token1Amount, expectedOutputAmount] = swapTestCase
    await addLiquidity(token0Amount, token1Amount)
    tx = await token0.transfer(pair.address, swapAmount)
    await tx.wait()
    reverted = true;
    try {
      tx = await pair.swap(0, expectedOutputAmount.add(1), wallet.address, '0x')
      await tx.wait()
      reverted = false;
    } catch (error) {
    }
    assert(reverted, 'Swap should be rejected')
    tx = await pair.swap(0, expectedOutputAmount, wallet.address, '0x')
    await tx.wait()
  }

  async function swaptest_2() {
    await before()
    const optimisticTestCases = [
      ['997000000000000000', 5, 10, 1], // given amountIn, amountOut = floor(amountIn * .997)
      ['997000000000000000', 10, 5, 1],
      ['997000000000000000', 5, 5, 1],
      [1, 5, 5, '1003009027081243732'] // given amountOut, amountIn = ceiling(amountOut / .997)
    ].map(a => a.map(n => (typeof n === 'string' ? BigNumber.from(n) : expandTo18Decimals(n))))
    
    await swaptest_2_sub(optimisticTestCases[0])
    await swaptest_2_sub(optimisticTestCases[1])
    await swaptest_2_sub(optimisticTestCases[2])

    console.log('optimisticTestCases Passed')
  }

  async function swaptest_2_sub(optimisticTestCase) {
    const [outputAmount, token0Amount, token1Amount, inputAmount] = optimisticTestCase
    await addLiquidity(token0Amount, token1Amount)
    tx = await token0.transfer(pair.address, inputAmount)
    await tx.wait()
    reverted = true
    try {
      tx = await pair.swap(outputAmount.add(1), 0, wallet.address, '0x')
      await tx.wait()
      reverted = false
    } catch (error) {}
    assert(reverted, 'Swap should be rejected')
    tx = await pair.swap(outputAmount, 0, wallet.address, '0x')
    await tx.wait()

    
  }

  async function swap_token0() {
    console.log('swap_token0 started')
    await before()
    const token0Amount = expandTo18Decimals(5)
    const token1Amount = expandTo18Decimals(10)
    await addLiquidity(token0Amount, token1Amount)

    const swapAmount = expandTo18Decimals(1)
    const expectedOutputAmount = BigNumber.from('1662497915624478906')
    tx = await token0.transfer(pair.address, swapAmount)
    await tx.wait()
    tx = await pair.swap(0, expectedOutputAmount, wallet.address, '0x')
    receipt = await tx.wait()
    events = receipt.events
    event_transfer = events[0]
    event_sync = events[1]
    event_swap = events[2]

    assert.equal(event_transfer.event, 'Transfer')
    assert.equal(event_transfer.args.from, pair.address)
    assert.equal(event_transfer.args.to, wallet.address)
    assert.equal(event_transfer.args.value.toString(), expectedOutputAmount.toString())

    assert.equal(event_sync.event, 'Sync')
    assert.equal(event_sync.args.reserve0.toString(), token0Amount.add(swapAmount).toString())
    assert.equal(event_sync.args.reserve1.toString(), token1Amount.sub(expectedOutputAmount).toString())

    assert.equal(event_swap.event, 'Swap')
    assert.equal(event_swap.args.sender, wallet.address)
    assert.equal(event_swap.args.amount0In.toString(), swapAmount.toString())
    assert.equal(event_swap.args.amount1In.toString(), '0')
    assert.equal(event_swap.args.amount0Out.toString(), '0')
    assert.equal(event_swap.args.amount1Out.toString(), expectedOutputAmount.toString())
    assert.equal(event_swap.args.to, wallet.address)

    const reserves = await pair.getReserves()
    assert.equal(reserves[0].toString(), token0Amount.add(swapAmount).toString())
    assert.equal(reserves[1].toString(), token1Amount.sub(expectedOutputAmount).toString())

    assert.equal((await token0.balanceOf(pair.address)).toString(), token0Amount.add(swapAmount).toString())
    assert.equal((await token1.balanceOf(pair.address)).toString(), token1Amount.sub(expectedOutputAmount).toString())
    const totalSupplyToken0 = await token0.totalSupply()
    const totalSupplyToken1 = await token1.totalSupply()

    assert.equal(
      (await token0.balanceOf(wallet.address)).toString(),
      totalSupplyToken0
        .sub(token0Amount)
        .sub(swapAmount)
        .toString()
    )
    assert.equal(
      (await token1.balanceOf(wallet.address)).toString(),
      totalSupplyToken1
        .sub(token1Amount)
        .add(expectedOutputAmount)
        .toString()
    )
    console.log('swap_token0 Passed')
  }

  async function swap_token1() {
    await before()
    console.log('swap_token1 started')
    const token0Amount = expandTo18Decimals(5)
    const token1Amount = expandTo18Decimals(10)
    try {
      await addLiquidity(token0Amount, token1Amount) // could be rejected after pair is alredy created.
    } catch (error) {}

    const swapAmount = expandTo18Decimals(1)
    const expectedOutputAmount = BigNumber.from('453305446940074565')

    tx = await token1.transfer(pair.address, swapAmount)
    await tx.wait()
    tx = await pair.swap(expectedOutputAmount, 0, wallet.address, '0x')
    receipt = await tx.wait()
    events = receipt.events
    event_transfer = events[0]
    event_sync = events[1]
    event_swap = events[2]
    // check to.emit(token0, 'Transfer')
    assert.equal(event_transfer.event, 'Transfer')
    assert.equal(event_transfer.args.from, pair.address)
    assert.equal(event_transfer.args.to, wallet.address)
    assert.equal(event_transfer.args.value.toString(), expectedOutputAmount.toString())

    assert.equal(event_sync.event, 'Sync')
    assert.equal(event_sync.args.reserve0.toString(), token0Amount.sub(expectedOutputAmount).toString())
    assert.equal(event_sync.args.reserve1.toString(), token1Amount.add(swapAmount).toString())

    assert.equal(event_swap.event, 'Swap')
    assert.equal(event_swap.args.sender, wallet.address)
    assert.equal(event_swap.args.amount0In.toString(), '0')
    assert.equal(event_swap.args.amount1In.toString(), swapAmount.toString())
    assert.equal(event_swap.args.amount0Out.toString(), expectedOutputAmount.toString())
    assert.equal(event_swap.args.amount1Out.toString(), '0')
    assert.equal(event_swap.args.to, wallet.address)

    const reserves = await pair.getReserves()
    assert.equal(reserves[0].toString(), token0Amount.sub(expectedOutputAmount).toString())
    assert.equal(reserves[1].toString(), token1Amount.add(swapAmount).toString())

    assert.equal((await token0.balanceOf(pair.address)).toString(), token0Amount.sub(expectedOutputAmount).toString())
    assert.equal((await token1.balanceOf(pair.address)).toString(), token1Amount.add(swapAmount).toString())
    const totalSupplyToken0 = await token0.totalSupply()
    const totalSupplyToken1 = await token1.totalSupply()
    assert.equal(
      (await token0.balanceOf(wallet.address)).toString(),
      totalSupplyToken0
        .sub(token0Amount)
        .add(expectedOutputAmount)
        .toString()
    )
    assert.equal(
      (await token1.balanceOf(wallet.address)).toString(),
      totalSupplyToken1
        .sub(token1Amount)
        .sub(swapAmount)
        .toString()
    )
    console.log('swap_token1 Passed')
  }

  async function burn() {
    await before()
    const token0Amount = expandTo18Decimals(3)
    const token1Amount = expandTo18Decimals(3)
    try {
      await addLiquidity(token0Amount, token1Amount)
    } catch (error) {}
    const expectedLiquidity = expandTo18Decimals(3)
    await pair.transfer(pair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
    tx = await pair.burn(wallet.address)
    receipt = await tx.wait()
    events = receipt.events

    assert.equal(events[0].event, 'Transfer')
    assert.equal(events[0].args.from, pair.address)
    assert.equal(events[0].args.to, '0x0000000000000000000000000000000000000000')
    assert.equal(events[0].args.value.toString(), expectedLiquidity.sub(MINIMUM_LIQUIDITY).toString())

    assert.equal(events[1].event, 'Transfer')
    assert.equal(events[1].args.from, pair.address)
    assert.equal(events[1].args.to, wallet.address)
    assert.equal(events[1].args.value.toString(), token0Amount.sub(1000).toString())

    assert.equal(events[2].event, 'Transfer')
    assert.equal(events[2].args.from, pair.address)
    assert.equal(events[2].args.to, wallet.address)
    assert.equal(events[2].args.value.toString(), token1Amount.sub(1000))

    assert.equal(events[3].event, 'Sync')
    assert.equal(events[3].args.reserve0.toString(), '1000')
    assert.equal(events[3].args.reserve1.toString(), '1000')

    assert.equal(events[4].event, 'Burn')
    assert.equal(events[4].args.sender, wallet.address)
    assert.equal(events[4].args.amount0.toString(), token0Amount.sub(1000).toString())
    assert.equal(events[4].args.amount1.toString(), token1Amount.sub(1000).toString())
    assert.equal(events[4].args.to.toString(), wallet.address)

    assert.equal((await pair.balanceOf(wallet.address)).toString(), '0')
    assert.equal((await pair.totalSupply()).toString(), MINIMUM_LIQUIDITY.toString())
    assert.equal((await token0.balanceOf(pair.address)).toString(), '1000')
    assert.equal((await token1.balanceOf(pair.address)).toString(), '1000')
    const totalSupplyToken0 = await token0.totalSupply()
    const totalSupplyToken1 = await token1.totalSupply()
    assert.equal((await token0.balanceOf(wallet.address)).toString(), totalSupplyToken0.sub(1000).toString())
    assert.equal((await token1.balanceOf(wallet.address)).toString(), totalSupplyToken1.sub(1000).toString())

    console.log('Burn Passed')
  }

  async function feeTo_off() {
    await before()
    const token0Amount = expandTo18Decimals(1000)
    const token1Amount = expandTo18Decimals(1000)
    await addLiquidity(token0Amount, token1Amount)

    const swapAmount = expandTo18Decimals(1)
    const expectedOutputAmount = BigNumber.from('996006981039903216')
    tx = await token1.transfer(pair.address, swapAmount)
    await tx.wait()
    tx = await pair.swap(expectedOutputAmount, 0, wallet.address, '0x')
    await tx.wait()

    const expectedLiquidity = expandTo18Decimals(1000)
    tx = await pair.transfer(pair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
    await tx.wait()
    tx = await pair.burn(wallet.address)
    await tx.wait()
    assert.equal((await pair.totalSupply()).toString(), MINIMUM_LIQUIDITY.toString())

    console.log('feeTo_off Passed')
  }

  async function feeTo_on() {
    await before()
    await factory.setFeeTo(other.address)

    const token0Amount = expandTo18Decimals(1000)
    const token1Amount = expandTo18Decimals(1000)
    await addLiquidity(token0Amount, token1Amount)

    const swapAmount = expandTo18Decimals(1)
    const expectedOutputAmount = BigNumber.from('996006981039903216')
    tx = await token1.transfer(pair.address, swapAmount)
    await tx.wait()
    tx = await pair.swap(expectedOutputAmount, 0, wallet.address, '0x')
    await tx.wait()

    const expectedLiquidity = expandTo18Decimals(1000)
    tx = await pair.transfer(pair.address, expectedLiquidity.sub(MINIMUM_LIQUIDITY))
    await tx.wait()
    tx = await pair.burn(wallet.address)
    await tx.wait()
    assert.equal((await pair.totalSupply()).toString(), MINIMUM_LIQUIDITY.add('249750499251388').toString())
    assert.equal((await pair.balanceOf(other.address)).toString(), '249750499251388')

    // using 1000 here instead of the symbolic MINIMUM_LIQUIDITY because the amounts only happen to be equal...
    // ...because the initial liquidity amounts were equal
    assert.equal(
      (await token0.balanceOf(pair.address)).toString(),
      BigNumber.from(1000)
        .add('249501683697445')
        .toString()
    )
    assert.equal(
      (await token1.balanceOf(pair.address)).toString(),
      BigNumber.from(1000)
        .add('250000187312969')
        .toString()
    )

    console.log('FeeTo_on Passed')
  }

  await first_mint()
  await swaptest_1()
  await swaptest_2()
  await swap_token0()
  await swap_token1()
  await burn()
  await feeTo_off()
  await feeTo_on()
}

main()
  .then(() => process.exit(0))
  .catch(error => {
    console.error(error)
    process.exit(1)
  })
