const { ethers } = require("hardhat")

async function main() {
  const [owner, operator] = await ethers.getSigners()

  const Impl = await ethers.getContractFactory("TestImpl")
  const impl0 = await Impl.deploy()
  impl0.deployed()

  const Test = await ethers.getContractFactory("TestAdmin")
  const test = await Test.deploy(impl0.address)
  await test.deployed()

  console.log('getSymbol', await test.getSymbol())
  console.log('getContent', await test.getContent())
  await test.setContent(impl0.address, '222')
  console.log('getContent', await test.content(), await impl0.content())
}

main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error)
    process.exit(1)
  })