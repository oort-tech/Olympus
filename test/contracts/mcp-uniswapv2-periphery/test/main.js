const { main: ExampleComputeLiquidityValue } = require('./1_ExampleComputeLiquidityValue.spec')
const { main: ExampleFlashSwap } = require('./2_ExampleFlashSwap.spec')
const { main: ExampleSwapToPrice } = require('./5_ExampleSwapToPrice.spec')
const { main: UniswapV2Migrator } = require('./6_UniswapV2Migrator.spec')
const { main: UniswapV2Router01_V1 } = require('./7_UniswapV2Router01_V1.spec')
const { main: UniswapV2Router02 } = require('./8_UniswapV2Router02.spec')

async function main() {
  await ExampleComputeLiquidityValue()
  await ExampleFlashSwap()
  await ExampleSwapToPrice()
  await UniswapV2Router01_V1()
  await UniswapV2Migrator()
  await UniswapV2Router02()
}

main()
  .then(() => process.exit(0))
  .catch(error => {
    console.error(error)
    process.exit(1)
  })
