
const { ethers, upgrades } = require("hardhat")
const assert = require('assert')

function expandTo18Decimals(n) {
  return ethers.BigNumber.from(n).mul(ethers.BigNumber.from(10).pow(18))
}

const TOTAL_SUPPLY = expandTo18Decimals(10000)
// const TOTAL_SUPPLY = 0

async function main() {

  const [owner, other] = await ethers.getSigners();
  const UniswapV2ERC20 = await ethers.getContractFactory('ERC20')
  uniswapV2ERC20 = await UniswapV2ERC20.deploy(TOTAL_SUPPLY)
  await uniswapV2ERC20.deployed()
  uniswapV2ERC20_1 = await UniswapV2ERC20.deploy(TOTAL_SUPPLY)
  await uniswapV2ERC20_1.deployed()
  console.log("UniswapV2ERC20 deployed to:", uniswapV2ERC20_1.address)
  
  const name = await uniswapV2ERC20.name()
  assert.equal(name, 'Uniswap V2')
  assert.equal(await uniswapV2ERC20.symbol(), 'UNI-V2')
  assert.equal(await uniswapV2ERC20.decimals(), 18)
  const balance = await uniswapV2ERC20.balanceOf(owner.address)
  assert.equal(balance.toString(), TOTAL_SUPPLY.toString())
  assert.equal(await uniswapV2ERC20.PERMIT_TYPEHASH(), ethers.utils.keccak256(ethers.utils.toUtf8Bytes('Permit(address owner,address spender,uint256 value,uint256 nonce,uint256 deadline)')))

  const UniswapV2Factory = await ethers.getContractFactory('UniswapV2Factory')
  uniswapV2Factory = await UniswapV2Factory.deploy(owner.address)
  await uniswapV2Factory.deployed()

 
  
  console.log("end deploy.")
  return {
    uniswapV2Factory_address: uniswapV2Factory.address,
    uniswapV2ERC20: uniswapV2ERC20.address,
    uniswapV2ERC20_1: uniswapV2ERC20_1.address
  }
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })
exports.main = main;