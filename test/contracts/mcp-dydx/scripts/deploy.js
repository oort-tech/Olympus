const { ethers } = require('hardhat');
const { getRiskParams, getRiskLimits, getPolynomialParams, getExpiryRampTime, getFinalSettlementRampTime } = require('./helpers');
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

    const tokenAPrice = (await oracle.setPrice(tokens[0].address, ONE_DOLLAR.times('100').toFixed(0)))
    const tokenBPrice = (await oracle.setPrice(tokens[1].address, ONE_DOLLAR.toFixed(0)))
    const tokenCPrice = (await oracle.setPrice(tokens[2].address, ONE_DOLLAR.times('0.3').toFixed(0)))
    console.log(tokenAPrice.hash, "Token A Price TX Hash");
    console.log(tokenBPrice.hash, "Token B Price TX Hash");
    console.log(tokenCPrice.hash, "Token C Price TX Hash");

    for (let i = 0; i < tokens.length; i += 1) {
      const tokenAddress = await tokens[i].address;

      //console.log(tokenAddress, "tokens");
      const ownerSetGlobalOperatorTest = (await testSoloMargin.ownerSetGlobalOperator(tokenAddress, true));
      console.log(ownerSetGlobalOperatorTest.hash, "Set Global Operator Tx Hash")

      // owner add market
    
        const ownerAddMarketCall = await testSoloMargin.ownerAddMarket(
          tokenAddress,
          testPriceOracle.address,
          Poly.address,
          { value: ZERO },
          { value: ZERO }
        );
        console.log(ownerAddMarketCall.hash, "Add Market Tx Hash")
    }

    const TestCallee = await ethers.getContractFactory('TestCallee');
    const TestSimpleCallee = await ethers.getContractFactory('TestSimpleCallee');
    
    // Second-Layer Contracts
    const PayableProxyForSoloMargin = await ethers.getContractFactory('PayableProxyForSoloMargin');
    const Expiry = await ethers.getContractFactory('Expiry');
    const ExpiryV2 = await ethers.getContractFactory('ExpiryV2');
    const FinalSettlement = await ethers.getContractFactory('FinalSettlement');
    const Refunder = await ethers.getContractFactory('Refunder');
    const DaiMigrator = await ethers.getContractFactory('DaiMigrator');
    const LiquidatorProxyV1ForSoloMargin = await ethers.getContractFactory('LiquidatorProxyV1ForSoloMargin');
    const LimitOrders = await ethers.getContractFactory('LimitOrders');
    const StopLimitOrders = await ethers.getContractFactory('StopLimitOrders');
    const CanonicalOrders = await ethers.getContractFactory('CanonicalOrders');
    const SignedOperationProxy = await ethers.getContractFactory('SignedOperationProxy');
    const WETH9 = await ethers.getContractFactory('WETH9');
    const weth = await WETH9.deploy();

    const payableProxyForSoloMargin = await PayableProxyForSoloMargin.deploy(
      testSoloMargin.address,
      weth.address
    );
    console.log(payableProxyForSoloMargin.address, "PayableProxyForSoloMargin Contract AddRess");
  
    const expiry = await Expiry.deploy(
      testSoloMargin.address,
      getExpiryRampTime()
    );
    console.log(expiry.address, "Expiry Contract Address");
  
    const expiryV2 = await ExpiryV2.deploy(
      testSoloMargin.address,
      getExpiryRampTime()
    );
    console.log(expiryV2.address, "ExpiryV2 Contract Address");
  
    const finalSettlement = await FinalSettlement.deploy(
      testSoloMargin.address,
      getFinalSettlementRampTime()
    );
    console.log(finalSettlement.address, "FinalSettlement Contract Address");
  
    const refunder = await Refunder.deploy(
      testSoloMargin.address,
      []
    );
    console.log(refunder.address, "Refunder Contract Address");
  
    const daiMigrator = await DaiMigrator.deploy([]);
    console.log(daiMigrator.address, "DaiMigrator Contract Address");
  
  } catch (error) {
    console.error('Error during deployment:', error);
  }
}

// Call the setupProtocol function
setupProtocol()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });
