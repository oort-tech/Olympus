const { ethers } = require("hardhat")

async function main() {
  const [owner, operator] = await ethers.getSigners()

  const Impl = await ethers.getContractFactory("TestImpl")
  const impl0 = await Impl.deploy()
  impl0.deployed()

  const impl1 = await Impl.deploy()
  impl1.deployed()

  const impl2 = await Impl.deploy()
  impl2.deployed()

  const Test = await ethers.getContractFactory("TestAdmin")
  const test = await Test.deploy(impl0.address)
  await test.deployed()

  console.log('1---------------------', await test.poolLength(), await test.poolInfo(0))
  await test.update(0)
  await test.add(impl1.address)

  // case 1
  console.log('2---------------------', await test.poolLength(), await test.poolInfo(1))
  await (await test.update(1)).wait()
  console.log('3---------------------', await test.poolInfo(1))
  await (await test.add(impl2.address)).wait()

  // case 2
  // await (await test.add(impl2.address)).wait()
  // console.log('2---------------------', await test.poolLength(), await test.poolInfo(1))
  // await (await test.update(1)).wait()
  // console.log('3---------------------', await test.poolInfo(1))
}

main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error)
    process.exit(1)
  })