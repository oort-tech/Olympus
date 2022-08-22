require("dotenv").config()
const networks = {
  hardhat: {
    blockGasLimit: 200000000,
    allowUnlimitedContractSize: true,
    chainId: 31337
  },
  ganache: {
    url: 'http://127.0.0.1:8545',
    blockGasLimit: 200000000,
    allowUnlimitedContractSize: false,
    chainId: 1337
  },
  ccnbeta: {
    url: process.env.CCNBETA_END_POINT,
    blockGasLimit: 68000000,
    allowUnlimitedContractSize: false,
    chainId: 971,
    accounts: [process.env.CCNBETA_PRIVATE_KEY, process.env.CCNBETA_OTHER_KEY]
  },
}

module.exports = networks