import { main as testErc20 } from './1_erc20'
import { main as testFactory } from './2_factory'
import { main as testPair } from './3_pair'
import { main as testRouter } from './4_router'
import { main as testMasterchef } from './5_masterchef'
import { main as testBnbstaking } from './6_bnbstaking'

async function main() {
  console.log('-----------------testErc20--------------------')
  await testErc20()
  console.log('-----------------testFactory--------------------')
  await testFactory()
  console.log('-----------------testPair--------------------')
  await testPair()
  console.log('-----------------testRouter--------------------')
  await testRouter()
  // await testMasterchef()
  // await testBnbstaking()
}

main().catch((error) => {
  console.error(error)
  process.exitCode = 0
})
