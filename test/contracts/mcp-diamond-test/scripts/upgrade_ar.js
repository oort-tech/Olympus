const { ethers, upgrades } = require("hardhat");

async function main() {
  const AccessRegistryV2 = await ethers.getContractFactory("AccessRegistryV2");
  const AccessRegistry = await upgrades.upgradeProxy(AccessRegistry_ADDRESS, AccessRegistryV2);
  console.log("AccessRegistry upgraded");
}

main();
