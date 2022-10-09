import { ethers } from 'hardhat'
import { expect } from 'chai'

import { PancakeFactory } from '../../typechain'
import { bigNumberify, getCreate2Address } from './shared/utilities'

const TEST_ADDRESSES: [string, string] = [
  '0x1000000000000000000000000000000000000000',
  '0x2000000000000000000000000000000000000000',
]

let factory: PancakeFactory

async function beforeEach() {
  const [wallet] = await ethers.getSigners()
  const PancakeFactory = await ethers.getContractFactory('PancakeFactory')
  factory = await PancakeFactory.deploy(wallet.address)
  await factory.deployed()
}

async function testSetters() {
  await beforeEach()
  const [wallet] = await ethers.getSigners()

  expect(await factory.feeTo()).to.eq(ethers.constants.AddressZero)
  expect(await factory.feeToSetter()).to.eq(wallet.address)
  expect(await factory.allPairsLength()).to.eq(0)
}

async function createPair(tokens: [string, string]) {
  const PancakePair = await ethers.getContractFactory('PancakePair')
  const create2Address = getCreate2Address(
    factory.address,
    tokens,
    PancakePair.bytecode
  )
  await expect(factory.createPair(...tokens))
    .to.emit(factory, 'PairCreated')
    .withArgs(
      TEST_ADDRESSES[0],
      TEST_ADDRESSES[1],
      create2Address,
      bigNumberify(1)
    )

  await expect((await factory.createPair(...tokens)).wait()).to.be.reverted // Pancake: PAIR_EXISTS
  // await expect(factory.createPair(...tokens.slice().reverse())).to.be.reverted // Pancake: PAIR_EXISTS
  expect(await factory.getPair(...tokens)).to.eq(create2Address)
  // expect(await factory.getPair(...tokens.slice().reverse())).to.eq(
  //   create2Address
  // )
  expect(await factory.allPairs(0)).to.eq(create2Address)
  expect(await factory.allPairsLength()).to.eq(1)

  const pair = await ethers.getContractAt('PancakePair', create2Address)
  expect(await pair.factory()).to.eq(factory.address)
  expect(await pair.token0()).to.eq(TEST_ADDRESSES[0])
  expect(await pair.token1()).to.eq(TEST_ADDRESSES[1])
}

async function testCreatePair() {
  await beforeEach()

  await createPair(TEST_ADDRESSES)
}

async function testCreatePairReverse() {
  await beforeEach()

  await createPair(TEST_ADDRESSES.slice().reverse() as [string, string])
}

async function testCreatePairGas() {
  await beforeEach()

  const tx = await factory.createPair(...TEST_ADDRESSES)
  const receipt = await tx.wait()
  expect(receipt.gasUsed).to.eq(2012492)
}

async function testSetFeeTo() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  // await expect(
  //   (await factory.connect(other).setFeeTo(other.address)).wait()
  // ).to.be.revertedWith('Pancake: FORBIDDEN')
  await (await factory.setFeeTo(wallet.address)).wait()
  expect(await factory.feeTo()).to.eq(wallet.address)
}

async function testSetFeeToSetter() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  // await expect(
  //   (await factory.connect(other).setFeeToSetter(other.address)).wait()
  // ).to.be.revertedWith('Pancake: FORBIDDEN')
  await (await factory.setFeeToSetter(other.address)).wait()
  expect(await factory.feeToSetter()).to.eq(other.address)
  // await expect(
  //   (await factory.setFeeToSetter(wallet.address)).wait()
  // ).to.be.revertedWith('Pancake: FORBIDDEN')
}

export async function main() {
  console.log('1. testSetters...')
  await testSetters()
  console.log('2. testCreatePair...')
  await testCreatePair()
  console.log('3. testCreatePairReverse...')
  await testCreatePairReverse()
  console.log('4. testCreatePairGas...')
  await testCreatePairGas()
  console.log('5. testSetFeeTo...')
  await testSetFeeTo()
  console.log('6. testSetFeeToSetter...')
  await testSetFeeToSetter()
}

// main().catch((error) => {
//   console.error(error)
//   process.exitCode = 1
// })
