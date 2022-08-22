const fs = require("fs");
const path = require("path");
const exec = require('child_process').execSync;
const chalk = require('chalk');
const { table }  = require('table');

async function readFiles() {
    if (fs.existsSync("./.build/contracts.json")) {
        return fs.readFileSync("./.build/contracts.json");
    } else {
        console.log('Compile the contracts before running this script');
        process.exit()
    }
}

async function printSize() {
    let file = await readFiles();
    console.log(`NOTE- Maximum size of contracts allowed to deloyed on the Ethereum mainnet is 24 KB(EIP170)`);
    console.log(`---- Size of the contracts ----`);
    let dataTable = [['Contracts', 'Size in KB']];
    let contracts = JSON.parse(file.toString());
    for(let contract in contracts.contracts) {
        if(contract.indexOf('test') == -1) {
            let content = contracts.contracts[contract].bin;
            let sizeInKB = content.toString().length / 2 / 1024;
            if (sizeInKB > 24)
                dataTable.push([chalk.red(contract),chalk.red(sizeInKB)]);
            else if (sizeInKB > 20)
                dataTable.push([chalk.yellow(contract),chalk.yellow(sizeInKB)]);
            else
                dataTable.push([chalk.green(contract),chalk.green(sizeInKB)]);
        }
    }
    console.log(table(dataTable));
}

printSize();