const utils = ethers.utils
const Web3Provider = ethers.provider
const { defaultAbiCoder, toUtf8Bytes, solidityPack, keccak256, hexlify, getAddress } = utils
const { BigNumber } = require("ethers")

const MINIMUM_LIQUIDITY = BigNumber.from(10).pow(3)

const PERMIT_TYPEHASH = keccak256(
  toUtf8Bytes('Permit(address owner,address spender,uint256 value,uint256 nonce,uint256 deadline)')
)

function expandTo18Decimals(n) {
  return BigNumber.from(n).mul(BigNumber.from(10).pow(18))
}

function getDomainSeparator(name, tokenAddress) {
  return keccak256(
    defaultAbiCoder.encode(
      ['bytes32', 'bytes32', 'bytes32', 'uint256', 'address'],
      [
        keccak256(toUtf8Bytes('EIP712Domain(string name,string version,uint256 chainId,address verifyingContract)')),
        keccak256(toUtf8Bytes(name)),
        keccak256(toUtf8Bytes('1')),
        1,
        tokenAddress
      ]
    )
  )
}

async function getApprovalDigest(
  token,
  approve,
  nonce,
  deadline
) {
  const name = await token.name()
  const DOMAIN_SEPARATOR = getDomainSeparator(name, token.address)
  return keccak256(
    solidityPack(
      ['bytes1', 'bytes1', 'bytes32', 'bytes32'],
      [
        '0x19',
        '0x01',
        DOMAIN_SEPARATOR,
        keccak256(
          defaultAbiCoder.encode(
            ['bytes32', 'address', 'address', 'uint256', 'uint256', 'uint256'],
            [PERMIT_TYPEHASH, approve.owner, approve.spender, approve.value, nonce, deadline]
          )
        )
      ]
    )
  )
}

async function mineBlock(provider, timestamp) {
  // await new Promise(async (resolve, reject) => {
  //   ;(provider._web3Provider.sendAsync)(
  //     { jsonrpc: '2.0', method: 'evm_mine', params: [timestamp] },
  //     (error, result) => {
  //       if (error) {
  //         reject(error)
  //       } else {
  //         resolve(result)
  //       }
  //     }
  //   )
  // })
  await provider.send("evm_increaseTime", [timestamp]);
  await provider.send("evm_mine");
}

function encodePrice(reserve0, reserve1) {
  return [reserve1.mul(BigNumber.from(2).pow(112)).div(reserve0), reserve0.mul(BigNumber.from(2).pow(112)).div(reserve1)]
}


module.exports = {
  expandTo18Decimals: expandTo18Decimals,
  getDomainSeparator: getDomainSeparator,
  getApprovalDigest: getApprovalDigest,
  mineBlock: mineBlock,
  encodePrice: encodePrice,
  MINIMUM_LIQUIDITY: MINIMUM_LIQUIDITY
}