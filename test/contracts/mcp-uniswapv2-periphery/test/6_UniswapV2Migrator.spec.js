const { ethers, upgrades } = require("hardhat")
const { AddressZero, MaxUint256 } = ethers.constants
const utils = ethers.utils
const { defaultAbiCoder, formatEther } = utils
const { BigNumber } = require("ethers")

const assert = require('assert')

const {deployContract } = require('ethereum-waffle')

const { deploycontracts } = require("./shared/fixtures")
const { expandTo18Decimals, MINIMUM_LIQUIDITY } = require("./shared/utilities.js")


const overrides = {
  gasLimit: 600000
}

let WETHPartner
let WETHPair
let router
let migrator
let WETHExchangeV1
let wallet
let tx

async function beforeEach() {
  fixture =  await deploycontracts(wallet)
  WETHPartner = fixture.WETHPartner
  WETHPair = fixture.WETHPair
  router = fixture.router01 // we used router01 for this contract
  migrator = fixture.migrator
  WETHExchangeV1 = fixture.WETHExchangeV1
}

async function migrate() {
  await beforeEach()
  console.log('migrate started')
  const WETHPartnerAmount = expandTo18Decimals(1)
  const ETHAmount = expandTo18Decimals(4)
  tx = await WETHPartner.approve(WETHExchangeV1.address, MaxUint256)
  await tx.wait()
  tx = await WETHExchangeV1.addLiquidity(BigNumber.from(1), WETHPartnerAmount, MaxUint256, {
    ...overrides,
    value: ETHAmount
  })
  await tx.wait()
  tx = await WETHExchangeV1.approve(migrator.address, MaxUint256)
  await tx.wait()
  const expectedLiquidity = expandTo18Decimals(2)
  const WETHPairToken0 = await WETHPair.token0()
  tx = await migrator.migrate(WETHPartner.address, WETHPartnerAmount, ETHAmount, wallet.address, MaxUint256, overrides)
  const receipt =  await tx.wait()
  assert.equal((await WETHPair.balanceOf(wallet.address)).toString(), expectedLiquidity.sub(MINIMUM_LIQUIDITY).toString())

  console.log('migrate passed')
}

async function main() {
  [wallet] = await ethers.getSigners()
  await migrate()
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main