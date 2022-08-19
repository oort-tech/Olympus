require("@nomiclabs/hardhat-waffle")
require("@nomiclabs/hardhat-etherscan")
require('@openzeppelin/hardhat-upgrades')
require("dotenv").config()
/**
 * @type import('hardhat/config').HardhatUserConfig
 */
module.exports = {
  solidity: {
    compilers: [
      {
        version: '0.8.4',
        settings: {
          optimizer: {
            enabled: true,
            runs: 200,
          },
        },
      },
      {
        version: '0.6.12',
        settings: {
          optimizer: {
            enabled: true,
            runs: 200,
          },
        },
      },
    ],
  },
  networks: {
    ccnbeta: {
      url: process.env.CCNBETA_URL,
      accounts: [
        process.env.CCNBETA_PRIVATE_KEY,
        process.env.CCNBETA_OPERATION_PRIVATE_KEY,
        process.env.CCNBETA_AUTHORIZED_PRIVATE_KEY,
        process.env.CCNBETA_USER_PRIVATE_KEY,
      ]
    }
  },
  etherscan: {
    apiKey: process.env.ETHERSCAN_API_KEY
  },
};
