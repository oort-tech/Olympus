const { coefficientsToString, decimalToString } = require('../src/lib/Helpers');

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
  let mbv = '0.00';
  return {
    marginRatio: { value: decimalToString('0.15') },
    liquidationSpread: { value: decimalToString('0.05') },
    earningsRate: { value: decimalToString('0.90') },
    minBorrowedValue: { value: decimalToString(mbv) },
  };
}

async function getPolynomialParams() {
  return {
    maxAPR: decimalToString('1.00'), // 100%
    coefficients: coefficientsToString([0, 10, 10, 0, 0, 80]),
  };
}

async function getDoubleExponentParams() {
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

module.exports = {
  getRiskLimits,
  getRiskParams,
  getPolynomialParams,
  getDoubleExponentParams,
  getDaiPriceOracleDeviationParams,
  getExpiryRampTime,
  getFinalSettlementRampTime,
};
