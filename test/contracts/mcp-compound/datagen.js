const fs = require('fs');

var contracts;
if (fs.existsSync("./.build/contracts.json")) {
    contracts = JSON.parse(fs.readFileSync("./.build/contracts.json").toString());
} else {
    console.log('Compile the contracts before running this script');
    process.exit()
}

fs.writeFileSync('./abis/RlendingLens.json', contracts.contracts['contracts/Lens/RlendingLens.sol:RlendingLens'].abi);
console.log('RlendingLens.json created');
fs.writeFileSync('./abis/Unitroller.json', contracts.contracts['contracts/Unitroller.sol:Unitroller'].abi);
console.log('Unitroller.json created');
fs.writeFileSync('./abis/Comptroller.json', contracts.contracts['contracts/Comptroller.sol:Comptroller'].abi);
console.log('Comptroller.json created');
fs.writeFileSync('./abis/JumpRateModelV2.json', contracts.contracts['contracts/JumpRateModelV2.sol:JumpRateModelV2'].abi);
console.log('JumpRateModelV2.json created');
fs.writeFileSync('./abis/CToken.json', contracts.contracts['contracts/CErc20Immutable.sol:CErc20Immutable'].abi);
console.log('CToken.json created');
fs.writeFileSync('./abis/CRBTC.json', contracts.contracts['contracts/CRBTC.sol:CRBTC'].abi);
console.log('CRBTC.json created');
fs.writeFileSync('./abis/WhitePaperInterestRateModel.json', contracts.contracts['contracts/WhitePaperInterestRateModel.sol:WhitePaperInterestRateModel'].abi);
console.log('WhitePaperInterestRateModel.json created');
fs.writeFileSync('./abis/Maximillion.json', contracts.contracts['contracts/Maximillion.sol:Maximillion'].abi);
console.log('Maximillion.json created');
fs.writeFileSync('./abis/PriceOracleProxy.json', contracts.contracts['contracts/PriceOracleProxy.sol:PriceOracleProxy'].abi);
console.log('PriceOracleProxy.json created');
fs.writeFileSync('./abis/PriceOracleAdapterMoc.json', contracts.contracts['contracts/PriceOracleAdapterMoc.sol:PriceOracleAdapterMoc'].abi);
console.log('PriceOracleAdapterMoc.json created');
fs.writeFileSync('./abis/PriceProviderMoC.json', contracts.contracts['contracts/PriceOracleAdapterMoc.sol:PriceProviderMoC'].abi);
console.log('PriceProviderMoC.json created');
