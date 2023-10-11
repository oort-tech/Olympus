require("@nomiclabs/hardhat-waffle")
require("dotenv").config()
/**
 * @type import('hardhat/config').HardhatUserConfig
 */
module.exports = {
  solidity: {
    version: "0.5.16",
    settings: {
      optimizer: {
        enabled: true,
        runs: 1000,
      },
      evmVersion: "istanbul"
    },
  },
  networks: {
    mainnet: {
      url: process.env.MAINNET_END_POINT,
      accounts: [process.env.MAINNET_PRIVATE_KEY]
    },
    rinkeby: {
      url: process.env.RINKEBY_END_POINT,
      accounts: [process.env.RINKEBY_PRIVATE_KEY]
    },
    bsc_test: {
      url: process.env.BSC_TEST_END_POINT,
      accounts: [process.env.BSC_TEST_PRIVATE_KEY, process.env.BSC_OTHER_KEY]
    },
    ccnbeta: {
      chainid: 9700,
      url: process.env.CCNBETA_END_POINT,
      accounts: [process.env.CCNBETA_PRIVATE_KEY, process.env.CCNBETA_OTHER_KEY]
    },
  },
  etherscan: {
    apiKey: process.env.ETHERSCAN_API_KEY
  },
};
