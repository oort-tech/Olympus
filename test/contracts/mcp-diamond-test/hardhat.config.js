const { task } = require("hardhat/config");
const { getSelectors, FacetCutAction } = require("./scripts/libraries/diamond.js");
const utils = require("ethers").utils;
require("hardhat-abi-exporter");
require("@nomiclabs/hardhat-waffle");
require("solidity-coverage");

task("accounts", "Prints the list of accounts", async (taskArgs, hre) => {
  console.log("Task run");
  const accounts = await hre.ethers.getSigners();
  for (const account of accounts) {
    console.log(account.address);
  }
});

require("hardhat-contract-sizer");
require("dotenv").config();
require("@tenderly/hardhat-tenderly");

module.exports = {
  defaultNetwork: "hardhat",
  networks: {
    hardhat: {
      // forking: {
      //   url: "https://data-seed-prebsc-1-s1.binance.org:8545",
      // },
      // chainId: 1337,
    },
    ropsten: {
      chainId: 3,
      url: process.env.API_URL,
      accounts: {
        mnemonic: process.env.MNEMONIC,
      },
      gas: 2100000,
      gasPrice: 8000000000,
    },

    kovan: {
      accounts: {
        mnemonic: process.env.MNEMONIC,
      },
      url: "https://kovan.infura.io/v3/99b8947af7e14278ae235bb21eb81f53",
      chainId: 42,
      timeout: 200000,
      gas: 2100000,
      gasPrice: 8000000000,
      nonce: 150,
    },
    bsctestnet: {
      accounts: {
        mnemonic: process.env.MNEMONIC,
      },
      url: 'https://data-seed-prebsc-1-s1.binance.org:8545',
      chainId: 97,
      timeout: 2000000,
      gas: 2100000,
      gasLimit: 5000000,
    },
    ccnbeta: {
      chainid: 971,
      url: process.env.CCNBETA_END_POINT,
      accounts: [process.env.CCNBETA_PRIVATE_KEY, process.env.CCNBETA_OTHER_KEY]
    }
  },
  solidity: {
    compilers: [
      {
        version: "0.8.1",
        settings: {
          optimizer: {
            enabled: true,
            runs: 200,
          },
        },
      },
      {
        version: "0.7.3",
        settings: {
          optimizer: {
            enabled: true,
            runs: 200,
          },
        },
      },
      {
        version: "0.6.6",
        settings: {
          optimizer: {
            enabled: true,
            runs: 200,
          },
        },
      },
      {
        version: "0.5.16",
        settings: {
          optimizer: {
            enabled: true,
            runs: 200,
          },
        },
      },
    ],
  },
  mocha: {
    timeout: 200000,
  },
  tenderly: {
    project: "Open-Protocol Testnet",
    username: "v9hstk",
  },
};
