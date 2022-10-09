require("@nomiclabs/hardhat-ethers");
require("@nomiclabs/hardhat-waffle");
require("hardhat-deploy");

const networks = require('./hardhat.networks')

const config = {
  defaultNetwork: "hardhat",
  solidity: {
    version: "0.5.17",
    settings: {
      optimizer: {
        enabled: true,
        runs: 200
      },
    }
  },
  paths: {
    sources: "./contracts",
    cache: "./cache",
    artifacts: "./build"
  },
  networks,
  namedAccounts: {
    deployer: {
      default: 0
    },
    usdtOracle: {
      4002: '0x9BB8A6dcD83E36726Cc230a97F1AF8a84ae5F128',
    },
    etherOracle: {
      4002: '0xB8C458C957a6e6ca7Cc53eD95bEA548c52AFaA24',
    },
    ftmOracle: {
      4002: ' 0xe04676B9A9A2973BCb0D1478b5E1E9098BBB7f3D',
    },
    usdt: {
      30: "0xef213441a85df4d7acbdae0cf78004e1e486bb96",
      31: "0x4cfe225ce54c6609a525768b13f7d87432358c57"
    },
    rif: {
      30: "0x2acc95758f8b5f583470ba265eb685a8f45fc9d5",
      31: "0x19f64674d8a5b4e652319f5e239efd3bc969a1fe"
    },
    admin1: {
      31: "0x8f63de841e7bccce39faa828128da25f8a93411f"
    },
    admin2: {
      31: "0x170346689cc312d8e19959bc68c3ad03e72c9850"
    },
    multiSig: {
      30: "0x2992181d390c5f35a70c8012a8a6a4a6b7603a37",
      31: "0x9760d4a155058f6bec8d9fd8d50222073e57083e"
    }
  },
  timeout: 300000,
};

module.exports = config
