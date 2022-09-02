import { ethers, waffle } from 'hardhat'
import { expect } from 'chai'

import {
  keccak256,
  defaultAbiCoder,
  toUtf8Bytes,
  hexlify,
  splitSignature,
} from 'ethers/lib/utils'

import { ecsign, ecrecover } from 'ethereumjs-util'

import { ERC20 } from '../../typechain'
import {
  expandTo18Decimals,
  getApprovalDigest,
  MaxUint256,
} from './shared/utilities'
import { bigNumberify } from './shared/utilities'

let token: ERC20
const TOTAL_SUPPLY = expandTo18Decimals(10000)
const TEST_AMOUNT = expandTo18Decimals(10)

async function beforeEach() {
  const ERC20 = await ethers.getContractFactory('ERC20')
  token = await ERC20.deploy(TOTAL_SUPPLY)
  await token.deployed()

  const [wallet, other] = await ethers.getSigners()
  await wallet.sendTransaction({
    to: other.address,
    value: ethers.utils.parseEther('1.0'),
  })
}

async function testAttributes() {
  await beforeEach()

  const [wallet] = await ethers.getSigners()

  const name = await token.name()
  expect(name).to.eq('Pancake LPs')
  expect(await token.symbol()).to.eq('Cake-LP')
  expect(await token.decimals()).to.eq(18)
  expect(await token.totalSupply()).to.eq(TOTAL_SUPPLY)
  expect(await token.balanceOf(wallet.address)).to.eq(TOTAL_SUPPLY)
  expect(await token.DOMAIN_SEPARATOR()).to.eq(
    keccak256(
      defaultAbiCoder.encode(
        ['bytes32', 'bytes32', 'bytes32', 'uint256', 'address'],
        [
          keccak256(
            toUtf8Bytes(
              'EIP712Domain(string name,string version,uint256 chainId,address verifyingContract)'
            )
          ),
          keccak256(toUtf8Bytes(name)),
          keccak256(toUtf8Bytes('1')),
          await wallet.getChainId(), // chain id
          token.address,
        ]
      )
    )
  )
  expect(await token.PERMIT_TYPEHASH()).to.eq(
    keccak256(
      toUtf8Bytes(
        'Permit(address owner,address spender,uint256 value,uint256 nonce,uint256 deadline)'
      )
    )
  )
}

async function testApprove() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  await expect(token.approve(other.address, TEST_AMOUNT))
    .to.emit(token, 'Approval')
    .withArgs(wallet.address, other.address, TEST_AMOUNT)
  expect(await token.allowance(wallet.address, other.address)).to.eq(
    TEST_AMOUNT
  )
}

async function testTransfer() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  await expect(token.transfer(other.address, TEST_AMOUNT))
    .to.emit(token, 'Transfer')
    .withArgs(wallet.address, other.address, TEST_AMOUNT)
  expect(await token.balanceOf(wallet.address)).to.eq(
    TOTAL_SUPPLY.sub(TEST_AMOUNT)
  )
  expect(await token.balanceOf(other.address)).to.eq(TEST_AMOUNT)
}

async function testTransferFail() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  await expect(token.transfer(other.address, TOTAL_SUPPLY.add(1))).to.be
    .reverted // ds-math-sub-underflow
  await expect(token.connect(other).transfer(wallet.address, 1)).to.be.reverted // ds-math-sub-underflow
}

async function testTransferFrom() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  await (await token.approve(other.address, TEST_AMOUNT)).wait()
  await expect(
    token
      .connect(other)
      .transferFrom(wallet.address, other.address, TEST_AMOUNT)
  )
    .to.emit(token, 'Transfer')
    .withArgs(wallet.address, other.address, TEST_AMOUNT)
  expect(await token.allowance(wallet.address, other.address)).to.eq(0)
  expect(await token.balanceOf(wallet.address)).to.eq(
    TOTAL_SUPPLY.sub(TEST_AMOUNT)
  )
  expect(await token.balanceOf(other.address)).to.eq(TEST_AMOUNT)
}

async function testTransferFromMax() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  await (await token.approve(other.address, MaxUint256)).wait()
  await expect(
    token
      .connect(other)
      .transferFrom(wallet.address, other.address, TEST_AMOUNT)
  )
    .to.emit(token, 'Transfer')
    .withArgs(wallet.address, other.address, TEST_AMOUNT)
  expect(await token.allowance(wallet.address, other.address)).to.eq(MaxUint256)
  expect(await token.balanceOf(wallet.address)).to.eq(
    TOTAL_SUPPLY.sub(TEST_AMOUNT)
  )
  expect(await token.balanceOf(other.address)).to.eq(TEST_AMOUNT)
}

async function testPermit() {
  await beforeEach()
  const [wallet, other] = await ethers.getSigners()

  const nonce = await token.nonces(wallet.address)
  const deadline = MaxUint256
  const digest = await getApprovalDigest(
    token,
    { owner: wallet.address, spender: other.address, value: TEST_AMOUNT },
    nonce,
    deadline
  )

  const { v, r, s } = ecsign(
    Buffer.from(digest.slice(2), 'hex'),
    Buffer.from(
      process.env.CCNBETA_PRIVATE_KEY ? process.env.CCNBETA_PRIVATE_KEY : '',
      'hex'
    )
  )

  await expect(
    token.permit(
      wallet.address,
      other.address,
      TEST_AMOUNT,
      deadline,
      v,
      hexlify(r),
      hexlify(s)
    )
  )
    .to.emit(token, 'Approval')
    .withArgs(wallet.address, other.address, TEST_AMOUNT)
  expect(await token.allowance(wallet.address, other.address)).to.eq(
    TEST_AMOUNT
  )
  expect(await token.nonces(wallet.address)).to.eq(bigNumberify(1))
}

export async function main() {
  console.log('1. testAttributes...')
  await testAttributes()
  console.log('2. testApprove...')
  await testApprove()
  console.log('3. testTransfer...')
  await testTransfer()
  console.log('4. testTransferFail...')
  // await testTransferFail()
  console.log('5. testTransferFrom...')
  await testTransferFrom()
  console.log('6. testTransferFromMax...')
  await testTransferFromMax()
  console.log('7. testPermit...')
  await testPermit()
}

// main().catch((error) => {
//   console.error(error)
//   process.exitCode = 1
// })
