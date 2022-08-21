const { ethers } = require("hardhat")
const Web3 = require("web3")

const { Contract, ContractFactory, getContractFactory, providers } = ethers
const { Web3Provider } = providers
const { deployContract } = require('ethereum-waffle')

const { expandTo18Decimals } = require('./utilities.js')

const UniswapV2Factory = require ('@uniswap/v2-core/build/UniswapV2Factory.json')
const IUniswapV2Pair = require('@uniswap/v2-core/build/IUniswapV2Pair.json')

const ERC20 = require( '../../build/ERC20.json')
const WETH9 = require( '../../build/WETH9.json')
const UniswapV1Exchange = require( '../../build/UniswapV1Exchange.json')
const UniswapV1Factory = require( '../../build/UniswapV1Factory.json')
const UniswapV2Router01 = require( '../../build/UniswapV2Router01.json')
const UniswapV2Migrator = require( '../../build/UniswapV2Migrator.json')
const UniswapV2Router02 = require( '../../build/UniswapV2Router02.json')
const RouterEventEmitter = require( '../../build/RouterEventEmitter.json')

const overrides = {
  gasLimit: 600000
}

async function deploycontracts(wallet) {
  // deploy tokens
  const tokenA = await deployContract(wallet, ERC20, [expandTo18Decimals(10000)])
  const tokenB = await deployContract(wallet, ERC20, [expandTo18Decimals(10000)])
  const WETH = await deployContract(wallet, WETH9)
  const WETHPartner = await deployContract(wallet, ERC20, [expandTo18Decimals(10000)])
  // deploy V1
  const factoryV1 = await deployContract(wallet, UniswapV1Factory, [])
  await factoryV1.initializeFactory((await deployContract(wallet, UniswapV1Exchange, [])).address)
  
  // deploy V2
  const factoryV2 = await deployContract(wallet, UniswapV2Factory, [wallet.address])
  // deploy routers
  const router01 = await deployContract(wallet, UniswapV2Router01, [factoryV2.address, WETH.address])
  const router02 = await deployContract(wallet, UniswapV2Router02, [factoryV2.address, WETH.address])
  // event emitter for testing
  const routerEventEmitter = await deployContract(wallet, RouterEventEmitter, [])
  // deploy migrator
  const migrator = await deployContract(wallet, UniswapV2Migrator, [factoryV1.address, router01.address])
  // initialize V1

  /**
   * createExchange is not working
   */
  let tx = await factoryV1.createExchange(WETHPartner.address, overrides)  
  await tx.wait()
  const WETHExchangeV1Address = await factoryV1.getExchange(WETHPartner.address, overrides)
  const WETHExchangeV1 = new Contract(WETHExchangeV1Address, JSON.stringify(UniswapV1Exchange.abi)).connect(
    wallet
  )
  // initialize V2
  tx = await factoryV2.createPair(tokenA.address, tokenB.address)
  await tx.wait()
  const pairAddress = await factoryV2.getPair(tokenA.address, tokenB.address)
  const pair = new Contract(pairAddress, JSON.stringify(IUniswapV2Pair.abi)).connect(wallet)

  const token0Address = await pair.token0()
  const token0 = tokenA.address === token0Address ? tokenA : tokenB
  const token1 = tokenA.address === token0Address ? tokenB : tokenA
  tx = await factoryV2.createPair(WETH.address, WETHPartner.address)
  await tx.wait()
  const WETHPairAddress = await factoryV2.getPair(WETH.address, WETHPartner.address)
  const WETHPair = new Contract(WETHPairAddress, JSON.stringify(IUniswapV2Pair.abi)).connect(wallet)

  return {
    token0,
    token1,
    WETH,
    WETHPartner,
    factoryV1,
    factoryV2,
    router01,
    router02,
    router: router02, // the default router, 01 had a minor bug
    routerEventEmitter,
    migrator,
    WETHExchangeV1,
    pair,
    WETHPair
  }
}
module.exports = {
  deploycontracts: deploycontracts
}