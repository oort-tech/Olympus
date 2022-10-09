const { main: token_deploy } = require('./1_deploy')
const { ethers } = require("hardhat")
const utils = ethers.utils
const { defaultAbiCoder, toUtf8Bytes, solidityPack, keccak256, hexlify, getAddress } = utils
const assert = require('assert');


function getCreate2Address(
  factoryAddress,
  [tokenA, tokenB],
  bytecode
) {
  const [token0, token1] = tokenA < tokenB ? [tokenA, tokenB] : [tokenB, tokenA]
  const create2Inputs = [
    '0xff',
    factoryAddress,
    keccak256(solidityPack(['address', 'address'], [token0, token1])),
    keccak256(bytecode),
    // '0x96e8ac4277198ff8b6f785478aa9a39f403cb768dd02cbee326c3e7da348845f'
  ]
  const sanitizedInputs = `0x${create2Inputs.map(i => i.slice(2)).join('')}`
  return getAddress(`0x${keccak256(sanitizedInputs).slice(-40)}`)
}

async function main() {
  
  const {uniswapV2Factory_address, uniswapV2ERC20, uniswapV2ERC20_1 } = await token_deploy();
  const [token0, token1] = uniswapV2ERC20.toLowerCase() < uniswapV2ERC20_1.toLowerCase() ? [uniswapV2ERC20, uniswapV2ERC20_1] : [uniswapV2ERC20_1, uniswapV2ERC20]
  let tx, receipt, events, pair
  // const UniswapV2Pair = await hre.artifacts.readArtifact("contracts/UniswapV2Pair.sol:UniswapV2Pair")
  const [owner, other] = await ethers.getSigners()
  const UniswapV2Factory = await ethers.getContractFactory('UniswapV2Factory')
  const UniswapV2Pair = await ethers.getContractFactory('UniswapV2Pair')
  uniswapV2Factory = await UniswapV2Factory.attach(uniswapV2Factory_address);
  
  const feeTo = await uniswapV2Factory.feeToSetter()
  assert.equal(feeTo.toUpperCase(), owner.address.toUpperCase())

  const preaddress = await uniswapV2Factory.pairFor(uniswapV2Factory_address, token0, token1)

  // const bytecode = `0x${UniswapV2Pair.evm.bytecode.object}`

  const bytecode = UniswapV2Pair.bytecode
  const create2Address = getCreate2Address(uniswapV2Factory_address, [token0, token1], bytecode)

  let pre_pair_count = await uniswapV2Factory.allPairsLength()

  tx = await uniswapV2Factory.createPair(token0, token1)
  receipt = await tx.wait()
  events = receipt.events[0]
  assert.equal(events.event, 'PairCreated')
  assert.equal(events.args.token0, token0)
  assert.equal(events.args.token1, token1)
  let after_pair_count = await uniswapV2Factory.allPairsLength()
  assert.equal(pre_pair_count.toString(), after_pair_count.sub(1).toString())
  pair = events.args.pair
  const uniswapV2Pair = await UniswapV2Pair.attach(pair)
  assert.equal(await uniswapV2Pair.token0(), token0)
  assert.equal(await uniswapV2Pair.token1(), token1)

  reverted = true
  try {
    await uniswapV2Factory.connect(other).setFeeToSetter(other.address)
    reverted = false
  } catch (error) {
    
  }
  assert(reverted);

  try {
    await uniswapV2Factory.connect(other).setFeeTo(other.address)
    reverted = false
  } catch (error) {
    
  }
  assert(reverted)


  return {
    uniswapV2Factory_address: uniswapV2Factory.address,
    uniswapV2ERC20: token0,
    uniswapV2ERC20_1: token1,
    pair_address: pair
  }
}

// main()
// .then(() => process.exit(0))
// .catch((error) => {
//     console.error(error)
//     process.exit(1)
// })

exports.main = main;