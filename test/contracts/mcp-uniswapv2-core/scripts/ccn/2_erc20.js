require("dotenv").config()
const add = require("../../chainData/chainData-ccntest.json")
const { ethers } = require("hardhat")
const utils = ethers.utils
const { defaultAbiCoder, toUtf8Bytes, solidityPack, keccak256, hexlify } = utils
const { constants} = require('ethers')
const { MaxUint256 } = constants
const assert = require('assert')
const { ecsign } = require('ethereumjs-util')
const { expandTo18Decimals } = require("../utils.js")

const PERMIT_TYPEHASH = keccak256(
  toUtf8Bytes('Permit(address owner,address spender,uint256 value,uint256 nonce,uint256 deadline)')
)


function getDomainSeparator(name, tokenAddress) {
  return utils.keccak256(
    defaultAbiCoder.encode(
      ['bytes32', 'bytes32', 'bytes32', 'uint256', 'address'],
      [
        utils.keccak256(toUtf8Bytes('EIP712Domain(string name,string version,uint256 chainId,address verifyingContract)')),
        utils.keccak256(toUtf8Bytes(name)),
        utils.keccak256(toUtf8Bytes('1')),
        5,
        tokenAddress
      ]
    )
  )
}
async function getApprovalDigest(
  token,
  approve,
  nonce,
  deadline
) {
  const name = await token.name()
  const DOMAIN_SEPARATOR = getDomainSeparator(name, token.address)
  return utils.keccak256(
    solidityPack(
      ['bytes1', 'bytes1', 'bytes32', 'bytes32'],
      [
        '0x19',
        '0x01',
        DOMAIN_SEPARATOR,
        utils.keccak256(
          defaultAbiCoder.encode(
            ['bytes32', 'address', 'address', 'uint256', 'uint256', 'uint256'],
            [PERMIT_TYPEHASH, approve.owner, approve.spender, approve.value, nonce, deadline]
          )
        )
      ]
    )
  )
}
const TOTAL_SUPPLY = expandTo18Decimals(10000)
const TEST_AMOUNT = expandTo18Decimals(10)
async function main() {
  let tx, receipt, events, prev_balance, other_prev_balance, after_balance, other_after_balance
  const [owner, other] = await ethers.getSigners()
  const UniswapV2ERC20 = await ethers.getContractFactory('UniswapV2ERC20')
  uniswapV2ERC20 = await UniswapV2ERC20.attach(add.ERC20);

  const name = await uniswapV2ERC20.name()
  
  // try {
  //   tx = await uniswapV2ERC20.connect(other).transferFrom(owner.address, other.address, TEST_AMOUNT, {gasLimit: 600000})
  //   receipt = await tx.wait()
  //   assert(false)
  // } catch (error) {
  //   assert(true)
  // }
  
  tx = await uniswapV2ERC20.approve(other.address, TEST_AMOUNT)
  receipt = await tx.wait()
  events = receipt.events[0]
  assert.equal(events.event, 'Approval')
  assert.equal(events.args.owner, owner.address)
  assert.equal(events.args.spender, other.address)
  assert.equal(events.args.value.toString(), TEST_AMOUNT.toString())
  let allowance = await uniswapV2ERC20.allowance(owner.address, other.address)
  assert.equal(allowance.toString(), TEST_AMOUNT.toString())
  console.log('Approve passed')

  prev_balance = await uniswapV2ERC20.balanceOf(owner.address)
  other_prev_balance = await uniswapV2ERC20.balanceOf(other.address)
  tx = await uniswapV2ERC20.transfer(other.address, TEST_AMOUNT)
  receipt = await tx.wait()
  events = receipt.events[0]
  assert.equal(events.event, 'Transfer')
  after_balance = await uniswapV2ERC20.balanceOf(owner.address)
  other_after_balance = await uniswapV2ERC20.balanceOf(other.address)
  assert.equal(after_balance.toString(), prev_balance.sub(TEST_AMOUNT).toString())
  assert.equal(other_prev_balance.toString(), other_after_balance.sub(TEST_AMOUNT).toString())
  console.log('Transfer Passed')

  // await uniswapV2ERC20.approve(other, TEST_AMOUNT)
  prev_balance = await uniswapV2ERC20.balanceOf(owner.address)
  other_prev_balance = await uniswapV2ERC20.balanceOf(other.address)
  tx = await uniswapV2ERC20.connect(other).transferFrom(owner.address, other.address, TEST_AMOUNT, {gasLimit: 600000})
  receipt = await tx.wait()
  assert.equal(events.event, 'Transfer')
  after_balance = await uniswapV2ERC20.balanceOf(owner.address)
  other_after_balance = await uniswapV2ERC20.balanceOf(other.address)
  assert.equal(await uniswapV2ERC20.allowance(owner.address, other.address), 0)
  assert.equal(after_balance.toString(), prev_balance.sub(TEST_AMOUNT).toString())
  assert.equal(other_prev_balance.toString(), other_after_balance.sub(TEST_AMOUNT).toString())
  console.log('Transferfrom passed ')

  const nonce = await uniswapV2ERC20.nonces(owner.address)
  console.log('nonce', nonce)
  const deadline = MaxUint256
  const digest = await getApprovalDigest(
    uniswapV2ERC20,
    { owner: owner.address, spender: other.address, value: TEST_AMOUNT },
    nonce,
    deadline
  )
  const { v, r, s } = ecsign(Buffer.from(digest.slice(2), 'hex'), Buffer.from(process.env.LOCAL_PRIVATE_KEY, 'hex'))

  tx = await uniswapV2ERC20.permit(owner.address, other.address, TEST_AMOUNT, deadline, v, hexlify(r), hexlify(s))
  receipt = await tx.wait()
  events = receipt.events[0]
  assert.equal(events.event, 'Approval')
  assert.equal(events.args.owner, owner.address)
  assert.equal(events.args.spender, other.address)
  assert.equal(events.args.value.toString(), TEST_AMOUNT.toString())
  allowance = await uniswapV2ERC20.allowance(owner.address, other.address)
  assert.equal(allowance.toString(), TEST_AMOUNT.toString())
  console.log('Permit passed')
  // expect(await uniswapV2ERC20.nonces(owner.address)).to.eq(bigNumberify(1))


  console.log('Test Passed')
}

main()
.then(() => process.exit(0))
.catch((error) => {
    console.error(error)
    process.exit(1)
})