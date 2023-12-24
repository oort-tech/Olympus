const { expect } = require("chai")
const { ethers, upgrades } = require("hardhat")

const BN = ethers.BigNumber.from

async function main() {
  const [owner, operation, authorized, user] = await ethers.getSigners()

  await owner.sendTransaction({to: operation.address, value: ethers.utils.parseEther("1.0")})
  await owner.sendTransaction({to: authorized.address, value: ethers.utils.parseEther("1.0")})
  await owner.sendTransaction({to: user.address, value: ethers.utils.parseEther("1.0")})
  
  // const erc20 = await ethers.getContractAt('TetherToken', '0x5FbDB2315678afecb367f032d93F642f64180aa3')
  // const ccnwrap = await ethers.getContractAt('TestERC20', '0xCf7Ed3AccA5a467e9e704C703E8D87F634fB0Fc9')
  // const bridgeContract = await ethers.getContractAt('BridgeContract', '0x2279B7A0a67DB372996a5FaB50D91eAA73d2eBe6')
  
  const coinAddress = '0x0000000000000000000000000000000000000000'
  const targetAddress = '0x6a3fBD2903A52Dd469e9fd43434fd835B2556c82'

  console.log("1. USDM deploying...")
  const ERC20 = await ethers.getContractFactory('TetherToken')
  erc20 = await ERC20.deploy(1000000000000, 'tether', 'usdm', 4)
  await erc20.deployed()
  console.log("> OK: USDM deployed to:", erc20.address, owner.address)
  
  console.log("2 USDM send to authorized account...", authorized.address)
  await (await erc20.transfer(authorized.address, 100000000)).wait()
  expect(await erc20.balanceOf(authorized.address)).to.equal(100000000)
  console.log("> OK: USDM sent to authorized account")

  console.log("3 USDM send to user account...")
  await (await erc20.transfer(user.address, 100000000)).wait()
  expect(await erc20.balanceOf(user.address)).to.equal(100000000)
  console.log("> OK: USDM sent to user account")

  console.log("4. CCNWRAP deploying...")
  const CCNWRAP = await ethers.getContractFactory('UCCN')
  ccnwrap = await CCNWRAP.deploy()
  await ccnwrap.deployed()
  console.log("> OK: CCNWRAP deployed to:", ccnwrap.address)

  console.log("5. CCNWRAP send to authorized account...")
  await (await ccnwrap.transfer(authorized.address, 100000000)).wait()
  expect(await ccnwrap.balanceOf(authorized.address)).to.equal(100000000)
  console.log("> OK: CCNWRAP sent to authorized account")

  console.log("6. CCNWRAP send to user account...")
  await (await ccnwrap.transfer(user.address, 100000000)).wait()
  expect(await ccnwrap.balanceOf(user.address)).to.equal(100000000)
  console.log("> OK: CCNWRAP sent to user account")

  console.log("7. BridgeContract deploying...")
  const BridgeContract = await ethers.getContractFactory("BridgeContract")
  const bridgeContract = await upgrades.deployProxy(BridgeContract, [
    operation.address,
    ccnwrap.address,
    authorized.address
  ])
  await bridgeContract.deployed()
  console.log("> OK: BridgeContract deployed to:", bridgeContract.address)

  console.log("8. setCrossChainFee calling...")
  await (await bridgeContract.connect(operation).setCrossChainFee(coinAddress, 100000000)).wait()
  expect(await bridgeContract.crossChainFee(coinAddress)).to.equal(100000000)

  await (await bridgeContract.connect(operation).setCrossChainFee(ccnwrap.address, 10)).wait()
  expect(await bridgeContract.crossChainFee(ccnwrap.address)).to.equal(10)

  await (await bridgeContract.connect(operation).setCrossChainFee(erc20.address, 10)).wait()
  expect(await bridgeContract.crossChainFee(erc20.address)).to.equal(10)
  console.log("> OK: setCrossChainFee called")

  console.log("9. deposit with ERC20...")
  await (await erc20.connect(user).approve(bridgeContract.address, 100)).wait()
  await (await bridgeContract.connect(user).deposit(erc20.address, 100, targetAddress)).wait()
  expect(await erc20.balanceOf(bridgeContract.address)).to.equal(100)
  console.log("> OK: deposit with ERC20")

  console.log("10. deposit with CCNWrap...")
  await (await ccnwrap.connect(user).approve(bridgeContract.address, 100)).wait()
  await (await bridgeContract.connect(user).deposit(ccnwrap.address, 100, targetAddress)).wait()
  expect(await ccnwrap.balanceOf(bridgeContract.address)).to.equal(100)
  console.log("> OK: deposit with CCNWrap")

  console.log("11. deposit with Coin...")
  await (await bridgeContract.connect(user).depositWithCoin(targetAddress, {value: ethers.BigNumber.from(10000000000)})).wait()
  expect(await waffle.provider.getBalance(bridgeContract.address)).to.equal(10000000000)
  console.log("> OK: deposit with Coin")

  console.log("12. addAvailableBalanceWithAdjustmentQuota: erc20->erc20...")
  await (await bridgeContract.connect(authorized).resetBalanceAdjustmentQuota(erc20.address, 1000)).wait()
  expect(await bridgeContract.balanceAdjustmentQuota(erc20.address)).to.equal(1000)

  await (await bridgeContract.connect(operation).addAvailableBalanceWithAdjustmentQuota(erc20.address, 100, targetAddress)).wait()
  expect(await erc20.balanceOf(targetAddress)).to.equal(90)
  console.log("> OK: addAvailableBalanceWithAdjustmentQuota: erc20->erc20")

  console.log("13. addAvailableBalanceWithAdjustmentQuota: wrap->coin...")
  await (await bridgeContract.connect(authorized).resetBalanceAdjustmentQuota(ccnwrap.address, 1000)).wait()
  expect(await bridgeContract.balanceAdjustmentQuota(ccnwrap.address)).to.equal(1000)

  await (await bridgeContract.connect(operation).addAvailableBalanceWithAdjustmentQuota(ccnwrap.address, 100, targetAddress)).wait()
  expect(await ccnwrap.balanceOf(targetAddress)).to.equal(90)
  console.log("> OK: addAvailableBalanceWithAdjustmentQuota: wrap->coin")

  /*console.log("14. addAvailableBalanceWithAdjustmentQuota : coin->wrap...")
  await (await bridgeContract.connect(authorized).resetBalanceAdjustmentQuota(coinAddress, ethers.utils.parseEther("10"))).wait()
  expect(await bridgeContract.balanceAdjustmentQuota(coinAddress)).to.equal(ethers.utils.parseEther("10"))

  const targetBalance = await waffle.provider.getBalance(targetAddress)
  await (await bridgeContract.connect(operation).addAvailableBalanceWithAdjustmentQuota(coinAddress, 10000000000, targetAddress)).wait()
  expect(await waffle.provider.getBalance(targetAddress)).to.equal(ethers.BigNumber.from(9900000000).add(targetBalance))
  console.log("> OK: addAvailableBalanceWithAdjustmentQuota : coin->wrap...")
  */
  console.log("15. inject : erc20...")
  await (await erc20.connect(authorized).approve(bridgeContract.address, 150)).wait()
  await (await bridgeContract.connect(authorized).inject(erc20.address, 150)).wait()
  expect(await erc20.balanceOf(bridgeContract.address)).to.equal(160)
  console.log("> OK: inject : erc20")

  console.log("16. inject : CCNWrap...")
  await (await ccnwrap.connect(authorized).approve(bridgeContract.address, 150)).wait()
  await (await bridgeContract.connect(authorized).inject(ccnwrap.address, 150)).wait()
  expect(await ccnwrap.balanceOf(bridgeContract.address)).to.equal(160)
  console.log("> OK: inject : CCNWrap")

  console.log("17. inject : Coin...")
  const bridgeBalance = await waffle.provider.getBalance(bridgeContract.address)
  await (await bridgeContract.connect(authorized).inject(coinAddress, 1000000000000, {value: ethers.BigNumber.from(1000000000000)})).wait()
  expect(await waffle.provider.getBalance(bridgeContract.address)).to.equal(ethers.BigNumber.from(1000000000000).add(bridgeBalance))
  console.log("> OK: inject : Coin")

  console.log("18. Withraw...")
  console.log('Chain Fee (COIN): ', (await bridgeContract.balanceCrossChainFee(coinAddress)).toString())

  const BridgeContract2 = await ethers.getContractFactory('BridgeContract2')
  const bridgeContract2 = await upgrades.upgradeProxy(bridgeContract.address, BridgeContract2)
  console.log("> OK: BridgeContract wrapAddress:", await bridgeContract2.wrapAddress())

  expect(await bridgeContract2.wrapAddress()).to.equal(await bridgeContract.wrapAddress())

  expect(await bridgeContract.balanceCrossChainFee(coinAddress)).to.equal(100000000)

  await (await bridgeContract2.withraw(coinAddress, await bridgeContract2.balanceCrossChainFee(coinAddress))).wait()
  expect(await bridgeContract2.balanceCrossChainFee(coinAddress)).to.equal(0)

  console.log('Chain Fee (ERC20): ', (await bridgeContract2.balanceCrossChainFee(erc20.address)).toString())
  await (await bridgeContract2.withraw(erc20.address, await bridgeContract2.balanceCrossChainFee(erc20.address))).wait()
  expect(await bridgeContract2.balanceCrossChainFee(erc20.address)).to.equal(0)

  console.log('Chain Fee (CCNWrap): ', (await bridgeContract2.balanceCrossChainFee(ccnwrap.address)).toString())
  await (await bridgeContract2.withraw(ccnwrap.address, await bridgeContract2.balanceCrossChainFee(ccnwrap.address))).wait()
  expect(await bridgeContract2.balanceCrossChainFee(ccnwrap.address)).to.equal(0)
  console.log("> OK: Withraw")
}

main()
.then(() => process.exit(0))
.catch((error) => {
    console.error(error)
    process.exit(1)
})