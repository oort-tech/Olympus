import { ethers } from 'hardhat'

export async function main() {
  const [wallet] = await ethers.getSigners()
  const PancakeFactory = await ethers.getContractFactory('PancakeFactory')
  const factory = await PancakeFactory.deploy(wallet.address)
  await factory.deployed()

  console.log('INIT_CODE_PAIR_HASH:', await factory.INIT_CODE_PAIR_HASH())
}

main().catch((error) => {
  console.error(error)
  process.exitCode = 1
})
