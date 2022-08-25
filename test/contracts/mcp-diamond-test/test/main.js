const {main: comptroller} = require('./comptroller.test');
const {main: deposit} = require('./deposit.test');
const {main: diamond} = require('./diamond.test');
const {main: dynamicInterest} = require('./dynamicInterest.test');
const {main: faucet} = require('./faucet.test');
const {main: liquidation} = require('./liquidation.test');
const {main: loan} = require('./loan.test');
const {main: oracleOpen} = require('./oracleOpen.test');
const {main: reserves} = require('./reserves.test');
const {main: tokenList} = require('./tokenList.test');

async function main() {
  
  await loan();
  await liquidation();
  await oracleOpen();
  await reserves();
  await tokenList();
  await comptroller();
  await deposit();
  await diamond();
  await dynamicInterest();
  await faucet();
}

main().catch((error) => {
  console.error(error)
  process.exitCode = 1
})