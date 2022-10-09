const { ethers } = require("hardhat")

async function main() {
  const [owner, operator] = await ethers.getSigners()

  //const DWDNContract = await ethers.getContractFactory("DWDN")
  //const dwdnContract = await DWDNContract.deploy()
  //await dwdnContract.deployed()
  //console.log("> OK: DWDNContract deployed to:", dwdnContract.address)

  const dwdnContract = await ethers.getContractAt('DWDN', '0x90AFD1C4Fb63d497596EAF467424Bf8724Bc36D3')

  console.log("getAddresses:", await dwdnContract.getAddresses())

  // let tx = await dwdnContract.requestUserToJoinTheNetwork(operator.address)
  // let rc = await tx.wait()
  // console.log(rc)

  let tx = await dwdnContract.addUserToBlacklist(operator.address)
  let rc = await tx.wait()
  console.log(rc)

  console.log("getBlackListAddresses:", await dwdnContract.getBlackListAddresses())
}

main()
.then(() => process.exit(0))
.catch((error) => {
    console.error(error)
    process.exit(1)
})