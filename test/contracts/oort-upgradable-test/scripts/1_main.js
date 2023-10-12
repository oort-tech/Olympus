const { ethers, upgrades } = require("hardhat")

async function main() {
  const [owner, operation, authorized, user] = await ethers.getSigners()

  console.log(operation.address, authorized.address, user.address)

  const CCNWRAP = await ethers.getContractFactory('UCCN')
  ccnwrap = await CCNWRAP.deploy()
  await ccnwrap.deployed()
  console.log("> OK: CCNWRAP deployed to:", ccnwrap.address, "Minted:", await ccnwrap.balanceOf(owner.address))

  const BridgeContract = await ethers.getContractFactory("BridgeContract")
  const bridgeContract = await upgrades.deployProxy(BridgeContract, [
    operation.address,
    ccnwrap.address,
    authorized.address
  ])
  await bridgeContract.deployed()
  console.log("> OK: BridgeContract deployed to:", bridgeContract.address)

  const bridgeContract2 = await upgrades.upgradeProxy(bridgeContract.address, BridgeContract)
  console.log("> OK: wrapAddress:", await bridgeContract2.wrapAddress())

  // const TEST = await ethers.getContractFactory('TestToken')
  // const test = await TEST.deploy()
  // await test.deployed()
  // console.log("> OK: TEST deployed to:", test.address, "Minted:", await test.balanceOf(owner.address))
}

main()
.then(() => process.exit(0))
.catch((error) => {
    console.error(error)
    process.exit(1)
})