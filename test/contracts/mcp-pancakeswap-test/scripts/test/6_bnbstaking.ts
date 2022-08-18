import { ethers, waffle } from "hardhat"
import { assert, expect } from 'chai'
import { BnbStaking, CakeToken, MockBEP20, WBNB } from "../../typechain"
const { expectRevert, time } = require('@openzeppelin/test-helpers')

let rewardToken: CakeToken
let lpToken: MockBEP20
let wBNB: WBNB
let bnbChef: BnbStaking

async function beforeEach() {
  const [minter, alice, bob, admin, dev] = await ethers.getSigners()

  const CakeTokenFactory = await ethers.getContractFactory('CakeToken')
  rewardToken = await CakeTokenFactory.deploy()

  const MockBEP20Factory = await ethers.getContractFactory('MockBEP20')
  lpToken = await MockBEP20Factory.deploy('LPToken', 'LP1', '1000000')

  const WBNBFactory = await ethers.getContractFactory('WBNB')
  wBNB = await WBNBFactory.deploy()

  const BnbStakingFactory = await ethers.getContractFactory('BnbStaking')
  bnbChef = await BnbStakingFactory.deploy(
    wBNB.address,
    rewardToken.address,
    1000,
    10,
    1010,
    admin.address,
    wBNB.address
  )
  await rewardToken.mintTo(bnbChef.address, 100000)

  await minter.sendTransaction({
    to: alice.address,
    value: ethers.utils.parseEther('1.0'),
  })
  await minter.sendTransaction({
    to: bob.address,
    value: ethers.utils.parseEther('1.0'),
  })
  await minter.sendTransaction({
    to: admin.address,
    value: ethers.utils.parseEther('1.0'),
  })
  await minter.sendTransaction({
    to: dev.address,
    value: ethers.utils.parseEther('1.0'),
  })
}

async function testDepositWithdraw() {
  await beforeEach()
  const [minter, alice, bob, admin, dev] = await ethers.getSigners()

  await time.advanceBlockTo((await waffle.provider.getBlockNumber()) + 10)
  await (await bnbChef.connect(alice).deposit({ value: 100 })).wait()
  await (await bnbChef.connect(bob).deposit({ value: 200 })).wait()
  assert.equal((await wBNB.balanceOf(bnbChef.address)).toString(), '300')
  assert.equal((await bnbChef.pendingReward(alice.address)).toString(), '1000')
  await (await bnbChef.connect(alice).deposit({ value: 300 })).wait()
  assert.equal((await bnbChef.pendingReward(alice.address)).toString(), '0')
  assert.equal((await rewardToken.balanceOf(alice.address)).toString(), '1333')
  await (await bnbChef.connect(alice).withdraw('100')).wait()
  assert.equal((await wBNB.balanceOf(bnbChef.address)).toString(), '500')
  await (await bnbChef.connect(minter).emergencyRewardWithdraw(1000)).wait()
  assert.equal((await bnbChef.pendingReward(bob.address)).toString(), '1399')
}

export async function main() {
  await testDepositWithdraw()
}

// main().catch((error) => {
//   console.error(error)
//   process.exitCode = 1
// })
