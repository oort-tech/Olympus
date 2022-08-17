import { ethers, waffle } from 'hardhat'
import { assert, expect } from 'chai'
import { CakeToken, MasterChef, MockBEP20, SyrupBar } from '../../typechain'
const { expectRevert, time } = require('@openzeppelin/test-helpers')

let cake: CakeToken
let syrup: SyrupBar
let lp1: MockBEP20
let lp2: MockBEP20
let lp3: MockBEP20
let lp4: MockBEP20
let lp5: MockBEP20
let lp6: MockBEP20
let lp7: MockBEP20
let lp8: MockBEP20
let lp9: MockBEP20
let chef: MasterChef

async function beforeEach() {
  const [minter, dev, alice, bob, carol] = await ethers.getSigners()

  const CakeTokenFactory = await ethers.getContractFactory('CakeToken')
  cake = await CakeTokenFactory.connect(minter).deploy()
  await cake.deployed()

  const SyrupBarFactory = await ethers.getContractFactory('SyrupBar')
  syrup = await SyrupBarFactory.connect(minter).deploy(cake.address)
  await syrup.deployed()

  const MockBEP20Factory = await ethers.getContractFactory('MockBEP20')
  lp1 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP1',
    '1000000'
  )
  lp1.deployed()

  lp2 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP2',
    '1000000'
  )
  lp2.deployed()

  lp3 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP3',
    '1000000'
  )
  lp3.deployed()

  const MasterChefFactory = await ethers.getContractFactory('MasterChef')
  chef = await MasterChefFactory.connect(minter).deploy(
    cake.address,
    syrup.address,
    dev.address,
    '1000',
    '100'
  )
  chef.deployed()

  await (await cake.connect(minter).transferOwnership(chef.address)).wait()
  await (await syrup.connect(minter).transferOwnership(chef.address)).wait()

  await (await lp1.connect(minter).transfer(bob.address, '2000')).wait()
  await (await lp2.connect(minter).transfer(bob.address, '2000')).wait()
  await (await lp3.connect(minter).transfer(bob.address, '2000')).wait()

  await (await lp1.connect(minter).transfer(alice.address, '2000')).wait()
  await (await lp2.connect(minter).transfer(alice.address, '2000')).wait()
  await (await lp3.connect(minter).transfer(alice.address, '2000')).wait()

  await minter.sendTransaction({
    to: dev.address,
    value: ethers.utils.parseEther('1.0'),
  })
  await minter.sendTransaction({
    to: alice.address,
    value: ethers.utils.parseEther('1.0'),
  })
  await minter.sendTransaction({
    to: bob.address,
    value: ethers.utils.parseEther('1.0'),
  })
  await minter.sendTransaction({
    to: carol.address,
    value: ethers.utils.parseEther('1.0'),
  })
}

async function testRealCase() {
  await beforeEach()
  const [minter, dev, alice, bob, carol] = await ethers.getSigners()

  const MockBEP20Factory = await ethers.getContractFactory('MockBEP20')
  lp4 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP4',
    '1000000'
  )
  await lp4.deployed()

  lp5 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP5',
    '1000000'
  )
  await lp5.deployed()

  lp6 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP6',
    '1000000'
  )
  await lp6.deployed()

  lp7 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP7',
    '1000000'
  )
  await lp7.deployed()

  lp8 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP8',
    '1000000'
  )
  await lp8.deployed()

  lp9 = await MockBEP20Factory.connect(minter).deploy(
    'LPToken',
    'LP9',
    '1000000'
  )
  await lp9.deployed()

  await (await chef.connect(minter).add('2000', lp1.address, true)).wait()
  await (await chef.connect(minter).add('1000', lp2.address, true)).wait()
  await (await chef.connect(minter).add('500', lp3.address, true)).wait()
  await (await chef.connect(minter).add('500', lp3.address, true)).wait()
  await (await chef.connect(minter).add('500', lp3.address, true)).wait()
  await (await chef.connect(minter).add('500', lp3.address, true)).wait()
  await (await chef.connect(minter).add('500', lp3.address, true)).wait()
  await (await chef.connect(minter).add('100', lp3.address, true)).wait()
  await (await chef.connect(minter).add('100', lp3.address, true)).wait()
  assert.equal((await chef.poolLength()).toString(), '10')

  await time.advanceBlockTo('170')
  await (await lp1.connect(alice).approve(chef.address, '1000')).wait()
  assert.equal((await cake.balanceOf(alice.address)).toString(), '0')
  await (await chef.connect(alice).deposit(1, '20')).wait()
  await (await chef.connect(alice).withdraw(1, '20')).wait()
  assert.equal((await cake.balanceOf(alice.address)).toString(), '263')

  await (await cake.connect(alice).approve(chef.address, '1000')).wait()
  await (await chef.connect(alice).enterStaking('20')).wait()
  await (await chef.connect(alice).enterStaking('0')).wait()
  await (await chef.connect(alice).enterStaking('0')).wait()
  await (await chef.connect(alice).enterStaking('0')).wait()
  assert.equal((await cake.balanceOf(alice.address)).toString(), '993')
  // assert.equal((await chef.getPoolPoint(0, { from: minter })).toString(), '1900');
}

async function testDepositWithdraw() {
  await beforeEach()
  const [minter, dev, alice, bob, carol] = await ethers.getSigners()

  await chef.connect(minter).add('1000', lp1.address, true)
  await chef.connect(minter).add('1000', lp2.address, true)
  await chef.connect(minter).add('1000', lp3.address, true)

  await lp1.connect(alice).approve(chef.address, '100')
  await chef.connect(alice).deposit(1, '20')
  await chef.connect(alice).deposit(1, '0')
  await chef.connect(alice).deposit(1, '40')
  await chef.connect(alice).deposit(1, '0')
  assert.equal((await lp1.balanceOf(alice.address)).toString(), '1940')
  await chef.connect(alice).withdraw(1, '10')
  assert.equal((await lp1.balanceOf(alice.address)).toString(), '1950')
  assert.equal((await cake.balanceOf(alice.address)).toString(), '999')
  assert.equal((await cake.balanceOf(dev.address)).toString(), '100')

  await lp1.connect(bob).approve(chef.address, '100')
  assert.equal((await lp1.balanceOf(bob.address)).toString(), '2000')
  await chef.connect(bob).deposit(1, '50')
  assert.equal((await lp1.balanceOf(bob.address)).toString(), '1950')
  await chef.connect(bob).deposit(1, '0')
  assert.equal((await cake.balanceOf(bob.address)).toString(), '125')
  await chef.connect(bob).emergencyWithdraw(1)
  assert.equal((await lp1.balanceOf(bob.address)).toString(), '2000')
}

async function testStakingUnstaking() {
  await beforeEach()
  const [minter, dev, alice, bob, carol] = await ethers.getSigners()

  await chef.connect(minter).add('1000', lp1.address, true)
  await chef.connect(minter).add('1000', lp2.address, true)
  await chef.connect(minter).add('1000', lp3.address, true)

  await lp1.connect(alice).approve(chef.address, '10')
  await chef.connect(alice).deposit(1, '2') //0
  await chef.connect(alice).withdraw(1, '2') //1

  await cake.connect(alice).approve(chef.address, '250')
  await chef.connect(alice).enterStaking('240') //3
  assert.equal((await syrup.balanceOf(alice.address)).toString(), '240')
  assert.equal((await cake.balanceOf(alice.address)).toString(), '10')
  await chef.connect(alice).enterStaking('10') //4
  assert.equal((await syrup.balanceOf(alice.address)).toString(), '250')
  assert.equal((await cake.balanceOf(alice.address)).toString(), '249')
  await chef.connect(alice).leaveStaking(250)
  assert.equal((await syrup.balanceOf(alice.address)).toString(), '0')
  assert.equal((await cake.balanceOf(alice.address)).toString(), '749')
}

async function testUpdateMultiplier() {
  await beforeEach()
  const [minter, dev, alice, bob, carol] = await ethers.getSigners()

  await chef.connect(minter).add('1000', lp1.address, true)
  await chef.connect(minter).add('1000', lp2.address, true)
  await chef.connect(minter).add('1000', lp3.address, true)

  await lp1.connect(alice).approve(chef.address, '100')
  await lp1.connect(bob).approve(chef.address, '100')
  await chef.connect(alice).deposit(1, '100')
  await chef.connect(bob).deposit(1, '100')
  await chef.connect(alice).deposit(1, '0')
  await chef.connect(bob).deposit(1, '0')

  await cake.connect(alice).approve(chef.address, '100')
  await cake.connect(bob).approve(chef.address, '100')
  await chef.connect(alice).enterStaking('50')
  await chef.connect(bob).enterStaking('100')

  await chef.connect(minter).updateMultiplier('0')

  await chef.connect(alice).enterStaking('0')
  await chef.connect(bob).enterStaking('0')
  await chef.connect(alice).deposit(1, '0')
  await chef.connect(bob).deposit(1, '0')

  assert.equal((await cake.balanceOf(alice.address)).toString(), '700')
  assert.equal((await cake.balanceOf(bob.address)).toString(), '150')

  await time.advanceBlockTo('265')

  await chef.connect(alice).enterStaking('0')
  await chef.connect(bob).enterStaking('0')
  await chef.connect(alice).deposit(1, '0')
  await chef.connect(bob).deposit(1, '0')

  assert.equal((await cake.balanceOf(alice.address)).toString(), '700')
  assert.equal((await cake.balanceOf(bob.address)).toString(), '150')

  await chef.connect(alice).leaveStaking('50')
  await chef.connect(bob).leaveStaking('100')
  await chef.connect(alice).withdraw(1, '100')
  await chef.connect(bob).withdraw(1, '100')
}

async function testAllowDev() {
  await beforeEach()
  const [minter, dev, alice, bob, carol] = await ethers.getSigners()

  assert.equal((await chef.devaddr()).valueOf(), dev.address)
  await expectRevert(chef.connect(bob).dev(bob.address), 'dev: wut?')
  await chef.connect(dev).dev(bob.address)
  assert.equal((await chef.devaddr()).valueOf(), bob.address)
  await chef.connect(bob).dev(alice.address)
  assert.equal((await chef.devaddr()).valueOf(), alice.address)
}

export async function main() {
  await testRealCase()
  await testDepositWithdraw()
  await testStakingUnstaking()
  // await testUpdateMultiplier()
  await testAllowDev()
}

// main().catch((error) => {
//   console.error(error)
//   process.exitCode = 1
// })
