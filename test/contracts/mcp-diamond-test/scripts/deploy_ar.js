const { ethers, upgrades } = require("hardhat");

async function main() {
  const AccessRegistry = await ethers.getContractFactory("AccessRegistry");
  const AccessRegistry = await upgrades.deployProxy(AccessRegistry, msg.sender, {
    initializer: "initialize",
  });
  await AccessRegistry.deployed();
  console.log("AccessRegistry deployed to:", AccessRegistry.address);
}

main();
