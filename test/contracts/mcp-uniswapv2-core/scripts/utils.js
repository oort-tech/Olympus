const { ethers } = require("hardhat")

function expandTo18Decimals(n) {
    return ethers.BigNumber.from(n).mul(ethers.BigNumber.from(10).pow(18))
}

const overrides = {
  gasLimit: 9999999
}

async function mineBlock(provider, timestamp) {
    await new Promise(async (resolve, reject) => {
      ;(provider._web3Provider.sendAsync)(
        { jsonrpc: '2.0', method: 'evm_mine', params: [timestamp] },
        (error, result) => {
          if (error) {
            reject(error)
          } else {
            resolve(result)
          }
        }
      )
    })
  }

module.exports = {
    expandTo18Decimals: expandTo18Decimals,
    mineBlock: mineBlock
}