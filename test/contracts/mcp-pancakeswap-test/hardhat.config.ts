import * as dotenv from 'dotenv'

import { HardhatUserConfig, task } from 'hardhat/config'
import '@nomiclabs/hardhat-etherscan'
import '@nomiclabs/hardhat-waffle'
import '@typechain/hardhat'
import 'hardhat-gas-reporter'
import 'solidity-coverage'

dotenv.config()

// This is a sample Hardhat task. To learn how to create your own go to
// https://hardhat.org/guides/create-task.html
task('accounts', 'Prints the list of accounts', async (taskArgs, hre) => {
  const accounts = await hre.ethers.getSigners()

  for (const account of accounts) {
    console.log(account.address)
  }
})

// You need to export an object to set up your config
// Go to https://hardhat.org/config/ to learn more

const config: HardhatUserConfig = {
  solidity: {
    compilers: [
      {
        version: '0.6.6',
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
      {
        version: '0.5.16',
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
      url: process.env.CCNBETA_URL || '',
      accounts:
        process.env.CCNBETA_PRIVATE_KEY !== undefined &&
        process.env.CCNBETA_PRIVATE_KEY2 !== undefined &&
        process.env.CCNBETA_PRIVATE_KEY3 !== undefined &&
        process.env.CCNBETA_PRIVATE_KEY4 !== undefined &&
        process.env.CCNBETA_PRIVATE_KEY5 !== undefined
          ? [
              process.env.CCNBETA_PRIVATE_KEY,
              process.env.CCNBETA_PRIVATE_KEY2,
              process.env.CCNBETA_PRIVATE_KEY3,
              process.env.CCNBETA_PRIVATE_KEY4,
              process.env.CCNBETA_PRIVATE_KEY5,
            ]
          : [],
    },
  },
  gasReporter: {
    enabled: process.env.REPORT_GAS !== undefined,
    currency: 'USD',
  },
}

export default config
