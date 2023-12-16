const { coefficientsToString, decimalToString } = require('../src/lib/Helpers');

// ============ Network Helper Functions ============

function isDevNetwork(network) {
  verifyNetwork(network);
  return network.startsWith('development')
      || network.startsWith('test')
      || network.startsWith('test_ci')
      || network.startsWith('develop')
      || network.startsWith('dev')
      || network.startsWith('docker')
      || network.startsWith('coverage')
      || network.startsWith('ccnbeta');
}

function isHuygensDev(network) {
  verifyNetwork(network);
  return network.startsWith('ccnbeta');
}

function isMainNet(network) {
  verifyNetwork(network);
  return network.startsWith('mainnet');
}

function isDocker(network) {
  verifyNetwork(network);
  return network.startsWith('docker');
}

function getChainId(network) {
  if (isMainNet(network)) {
    return 1;
  }
  if (network.startsWith('ccnbeta')) {
    return 9800;
  }
  if (network.startsWith('coverage')) {
    return 1002;
  }
  if (network.startsWith('docker')) {
    return 804; //1313;
  }
  if (network.startsWith('dev')) {
    return 9700;
  }
  if (network.startsWith('ccnbeta')) {
    return 9800;
  }
  if (network.startsWith('development')) {
    return 971;
  }
  if (network.startsWith('test') || network.startsWith('test_ci')) {
    return 1001;
  }
  throw new Error('No chainId for network', network);
}

async function getRiskLimits() {
  return {
    marginRatioMax: decimalToString('2.00'),
    liquidationSpreadMax: decimalToString('0.50'),
    earningsRateMax: decimalToString('1.00'),
    marginPremiumMax: decimalToString('2.00'),
    spreadPremiumMax: decimalToString('2.00'),
    minBorrowedValueMax: decimalToString('100.00'),
  };
}

async function getRiskParams() {
  //verifyNetwork(network);
  let mbv = '0.00';
  //if (isDevNetwork(network)) {
  //  mbv = '0.05';
  //}
  return {
    marginRatio: { value: decimalToString('0.15') },
    liquidationSpread: { value: decimalToString('0.05') },
    earningsRate: { value: decimalToString('0.90') },
    minBorrowedValue: { value: decimalToString(mbv) },
  };
}

async function getPolynomialParams() {
  /*if (isMainNet(network)) {
    return {
      maxAPR: decimalToString('0.50'), // 50%
      coefficients: coefficientsToString([0, 20, 10, 0, 0, 0, 0, 0, 0, 0, 70]),
    };
  }*/
  return {
    maxAPR: decimalToString('1.00'), // 100%
    coefficients: coefficientsToString([0, 10, 10, 0, 0, 80]),
  };
}

async function getDoubleExponentParams() {
  /*if (isMainNet(network)) {
    return {
      maxAPR: decimalToString('0.50'), // 50%
      coefficients: coefficientsToString([0, 20, 0, 0, 0, 0, 20, 60]),
    };
  }*/
  return {
    maxAPR: decimalToString('1.00'), // 100%
    coefficients: coefficientsToString([20, 20, 20, 20, 20]),
  };
}

function getDaiPriceOracleDeviationParams(network) {
  verifyNetwork(network);
  if (isDevNetwork) {
    return {
      denominator: decimalToString('1.00'),
      maximumPerSecond: decimalToString('0.0001'),
      maximumAbsolute: decimalToString('0.01'),
    };
  }
  return {
    denominator: decimalToString('1.00'),
    maximumPerSecond: decimalToString('0.0001'),
    maximumAbsolute: decimalToString('0.01'),
  };
}

function getExpiryRampTime() {
  return '3600'; // 1 hour
}

function getFinalSettlementRampTime() {
  return '2419200'; // 28 days
}

function verifyNetwork(network) {
  if (!network) {
    throw new Error('No network provided');
  }
}

function getSenderAddress(network, accounts) {
  if (isMainNet(network)) {
    return '0xf809e07870dca762B9536d61A4fBEF1a17178092';
  }
  if (isDevNetwork(network)) {
    return accounts[0];
  }
  throw new Error('Cannot find Sender address');
}

function getOraclePokerAddress(network, accounts) {
  if (isMainNet(network)) {
    return '0x500dd93a74dbfa65a4eeda44da489adcef530cb9';
  }
  if (isDevNetwork(network)) {
    return accounts[0];
  }
  if (isHuygens_Dev(network)) {
    return accounts[0];
  }
  throw new Error('Cannot find Oracle Poker');
}

function getPartiallyDelayedMultisigAddress(network) {
  if (isMainNet(network)) {
    return '0xba2906b18B069b40C6D2CAFd392E76ad479B1B53';
  }
  if (isHuygensDev(network)) {
    return accounts[1]
  }
  throw new Error('Cannot find Admin Multisig');
}

function getNonDelayedMultisigAddress(network) {
  if (isMainNet(network)) {
    return '0x03b24cf9fe32dd719631d52bd6705d014c49f86f';
  }
  if (isHuygensDev(network)) {
    return accounts[3]
  }
  throw new Error('Cannot find Admin Multisig');
}

module.exports = {
  isDevNetwork,
  isMainNet,
  isHuygensDev,
  isDocker,
  getChainId,
  getRiskLimits,
  getRiskParams,
  getPolynomialParams,
  getDoubleExponentParams,
  getDaiPriceOracleDeviationParams,
  getExpiryRampTime,
  getFinalSettlementRampTime,
  getOraclePokerAddress,
  getSenderAddress,
  getPartiallyDelayedMultisigAddress,
  getNonDelayedMultisigAddress,
};
