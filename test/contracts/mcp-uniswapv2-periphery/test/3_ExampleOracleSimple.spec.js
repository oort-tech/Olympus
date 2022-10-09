const { ethers, upgrades, network } = require("hardhat")
const { AddressZero, MaxUint256 } = ethers.constants
const utils = ethers.utils
const { defaultAbiCoder, formatEther } = utils
const { BigNumber } = require("ethers")

const assert = require('assert')

const {deployContract } = require('ethereum-waffle')

const { deploycontracts } = require("./shared/fixtures")
const { expandTo18Decimals, mineBlock, encodePrice } = require("./shared/utilities.js")

const ExampleOracleSimple = require('../build/ExampleOracleSimple.json')


const overrides = {
  gasLimit: 599999
}

const token0Amount = expandTo18Decimals(5)
const token1Amount = expandTo18Decimals(10)
let token0
let token1
let pair
let exampleOracleSimple
let fixture
let tx

async function addLiquidity() {
  await token0.transfer(pair.address, token0Amount)
  await token1.transfer(pair.address, token1Amount)
  await pair.mint(wallet.address)
}

async function beforeEach () {
  fixture =  await deploycontracts(wallet)
  token0 = fixture.token0
  token1 = fixture.token1
  pair = fixture.pair
  await addLiquidity()
  exampleOracleSimple = await deployContract(
    wallet,
    ExampleOracleSimple,
    [fixture.factoryV2.address, token0.address, token1.address]  )
}

async function update() {
  await beforeEach()
  console.log('Update started')
  const blockTimestamp = (await pair.getReserves())[2]
  await mineBlock(network.provider, 60 * 60 * 23)
  
  try {
    tx = await exampleOracleSimple.update(overrides)
    await tx.wait()
    assert.equal(0,1)
  } catch (error) {
    console.log('error', error)
    // assert.equal(error.toString()., "VM Exception while processing transaction: reverted with reason string 'ExampleOracleSimple: PERIOD_NOT_ELAPSED'")
  }
  // 
  await mineBlock(network.provider, 60 * 60 * 24)
  tx = await exampleOracleSimple.update()
  await tx.wait()

  const expectedPrice = encodePrice(token0Amount, token1Amount)

  assert.equal((await exampleOracleSimple.price0Average()).toString(), expectedPrice[0].toString())
  assert.equal((await exampleOracleSimple.price1Average()).toString(), expectedPrice[1].toString())

  assert.equal((await exampleOracleSimple.consult(token0.address, token0Amount)).toString(), token1Amount.toString())
  assert.equal((await exampleOracleSimple.consult(token1.address, token1Amount)).toString(), token0Amount.toString())

  console.log('update passed')
}

async function main() {
  [wallet] = await ethers.getSigners()
  console.log("-------------------This is only working for hardhat----------------------------")
  await update()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main