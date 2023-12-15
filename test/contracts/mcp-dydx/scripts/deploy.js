const { ethers } = require('hardhat');
const { getRiskParams, getRiskLimits, getPolynomialParams } = require('./helpers');
const { BigNumber } = require('bignumber.js');

const ZERO = '0';
const ONE_HUNDRED = '100';
const ONE_DOLLAR = new BigNumber('1e18');

async function getTokens() {
  try {
    const tokenContracts = ['TokenA', 'TokenB', 'TokenC'];
    const deployedTokens = [];

    for (const contractName of tokenContracts) {
      const tokenFactory = await ethers.getContractFactory(contractName);
      const tokenInstance = await tokenFactory.deploy();
      deployedTokens.push(tokenInstance);
       console.log(tokenInstance.address, contractName);
    }

    return Promise.all(deployedTokens);

  } catch (error) {
    console.error('Error during token deployment:', error);
    throw new Error('Cannot find Tokens');
  }
}


async function setupProtocol() {
  try {
    const AdminImpl = await ethers.getContractFactory("AdminImpl");
    const OperationImpl = await ethers.getContractFactory("OperationImpl");

    const admin = await AdminImpl.deploy();
    const operation = await OperationImpl.deploy();

    console.log(await admin.deployTransaction.hash, "Deployed AdminImpl");
    console.log(await operation.deployTransaction.hash, "Deployed OperationImpl");

    const soloMarginContract = await ethers.getContractFactory(
      "SoloMargin", {
      libraries: {
        AdminImpl: admin.address,
        OperationImpl: operation.address
      }
    });

    const testSoloMargin = await soloMarginContract.deploy(await getRiskParams(), await getRiskLimits());
    console.log(await testSoloMargin.deployTransaction.hash, "Deployed SoloMargin");

    const tokens = await getTokens();
    const account = ethers.getSigners();

    const testOracle = await ethers.getContractFactory("TestPriceOracle");
    const oracle = await testOracle.deploy();
    console.log(await oracle.address, "Oracle");

    const testPriceOracle = await oracle.deployed();

    const testPoly = await ethers.getContractFactory("TestPolynomialInterestSetter");
    const Poly = await testPoly.deploy(await getPolynomialParams());
    console.log(await Poly.address, "Setter Address");

    (await oracle.setPrice(tokens[0].address, ONE_DOLLAR.times('100').toFixed(0)))
    console.log(await oracle.setPrice(tokens[1].address, ONE_DOLLAR.toFixed(0)))
    console.log(await oracle.setPrice(tokens[2].address, ONE_DOLLAR.times('0.3').toFixed(0)))

    for (let i = 0; i < tokens.length; i += 1) {
      const tokenAddress = await tokens[i].address;

      console.log(tokenAddress, "tokens");
      console.log(await testSoloMargin.ownerSetGlobalOperator(tokenAddress, true));

      console.log(
        await testSoloMargin.ownerAddMarket(
        tokenAddress, 
        testPriceOracle.address,
          Poly.address,
          { value: ZERO },
          { value: ZERO }
        ));
    }

  } catch (error) {
    console.error('Error during protocol setup:', error);
  }
}

// Call the setupProtocol function
setupProtocol()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });
