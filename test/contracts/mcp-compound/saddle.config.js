
module.exports = {
  // solc: "solc",                                          // Solc command to run
  solc_args: [                                              // Extra solc args
    '--allow-paths','contracts,tests/Contracts',
    '--evm-version', 'istanbul'
  ],
  solc_shell_args: {                                        // Args passed to `exec`, see:
    maxBuffer: 1024 * 500000,                               // https://nodejs.org/api/child_process.html#child_process_child_process_spawn_command_args_options
    shell: process.env['SADDLE_SHELL'] || '/bin/bash'
  },
  // build_dir: ".build",                                   // Directory to place built contracts
  extra_build_files: ['remote/*.json'],                     // Additional build files to deep merge
  // coverage_dir: "coverage",                              // Directory to place coverage files
  // coverage_ignore: [],                                   // List of files to ignore for coverage
  contracts: process.env['SADDLE_CONTRACTS'] || "{contracts,contracts/**,tests/Contracts}/*.sol",
                                                            // Glob to match contract files
  trace: false,                                             // Compile with debug artifacts
  // TODO: Separate contracts for test?
  tests: ['**/tests/{,**/}*Test.js'],                       // Glob to match test files
  networks: {                                               // Define configuration for each network
    development: {
      providers: [
      {env: "PROVIDER"},
        {ganache: {
          gasLimit: 20000000,
          gasPrice: 20000,
          defaultBalanceEther: 1000000000,
          allowUnlimitedContractSize: true,
          hardfork: 'istanbul'
        }}
      ],
      web3: {                                               // Web3 options for immediate confirmation in development mode
        gas: [
          {env: "GAS"},
          {default: "6700000"}
        ],
        gas_price: [
          {env: "GAS_PRICE"},
          {default: "20000"}
        ],
        options: {
          transactionConfirmationBlocks: 1,
          transactionBlockTimeout: 5
        }
      },
      accounts: [                                           // How to load default account for transactions
        {env: "ACCOUNT"},                                   // Load from `ACCOUNT` env variable (e.g. env ACCOUNT=0x...)
        {unlocked: 0}                                       // Else, try to grab first "unlocked" account from provider
      ]
    },
    test: {
      providers: [
        // UNCOMMENT BELOW TO RUN TESTS ON GANACHE-CORE
              {
                ganache: {
                  gasLimit: 200000000,
                  gasPrice: 20000,
                  defaultBalanceEther: 1000000000,
                  allowUnlimitedContractSize: true,
                  hardfork: 'istanbul'
                }
              }
        // UNCOMMENT BELOW TO POINT TO LOCALHOST(i.e. `ganache-cli --gasLimit 200000000 --gasPrice 20000 --defaultBalanceEther 1000000000 --allowUnlimitedContractSize true -k "istanbul" -v`)
              //{env: "PROVIDER"},                                      // Checks env $PROVIDER (i.e. `export PROVIDER="http://localhost:8545"`)
              //{file: "~/.ethereum/ganache-local"},                    // Load from given file with contents as the URL (e.g. https://infura.io/api-key)
              //{http: "http://localhost:4444"}                         // FIX THIS, SHOULD BE 8545 by default
            ],
        /////////////////////////////////////////
      web3: {
        gas: [
          {env: "GAS"},
          {default: "20000000"}
        ],
        gas_price: [
          {env: "GAS_PRICE"},
          {default: "12000000002"}
        ],
        options: {
          transactionConfirmationBlocks: 1,
          transactionBlockTimeout: 5
        }
      },
      accounts: [
        {env: "ACCOUNT"},
        {unlocked: 0}
      ]
    },
    rsk: {
      providers: [
              //{env: "PROVIDER"},                                      // Checks env $PROVIDER (i.e. `export PROVIDER="http://localhost:8545"`)
              //{file: "~/.ethereum/goerli-url"},                    // Load from given file with contents as the URL (e.g. https://infura.io/api-key)
              // {http: "http://localhost:8545"}                         //
              {http: "http://localhost:4444"}                         //
            ],
      web3: {
        gas: [
          {env: "GAS"},
          {default: "20000000"}
        ],
        gas_price: [
          {env: "GAS_PRICE"},
          {default: "12000000002"}
        ],
        options: {
          transactionConfirmationBlocks: 1,
          transactionBlockTimeout: 5
        }
      },
      accounts: [
        {env: "ACCOUNT"},
        {unlocked: 0}
      ]
    },
    rsktestnet: {
      providers: [
              {http: "https://public-node.testnet.rsk.co"}  //RSK Public Node
            ],
      web3: {
        gas: [
          {env: "GAS"},
          {default: "6800000"}
        ],
        gas_price: [
          {env: "GAS_PRICE"},
          {default: "70000000"}
        ],
        options: {
          transactionConfirmationBlocks: 1,
          transactionBlockTimeout: 5
        }
      },
      accounts: [
        {env: "ACCOUNT"}
      ]
    },
    rskmainnet: {
      providers: [
              {http: "https://public-node.rsk.co"}  //RSK Public Node
            ],
      web3: {
        gas: [
          {env: "GAS"},
          {default: "6800000"}
        ],
        gas_price: [
          {env: "GAS_PRICE"},
          {default: "70000000"}
        ],
        options: {
          transactionConfirmationBlocks: 1,
          transactionBlockTimeout: 5
        }
      },
      accounts: [
        {env: "ACCOUNT"}
      ]
    },
  },
  get_network_file: (network) => {
    return null;
  },
  read_network_file: (network) => {
    const fs = require('fs');
    const path = require('path');
    const util = require('util');

    const networkFile = path.join(process.cwd(), 'networks', `${network}.json`);
    return util.promisify(fs.readFile)(networkFile).then((json) => {
      return JSON.parse(json)['Contracts'] || {};
    });
  },
  write_network_file: (network, value) => {
    const fs = require('fs');
    const path = require('path');
    const util = require('util');

    const networkFile = path.join(process.cwd(), 'networks', `${network}.json`);
    return util.promisify(fs.readFile)(networkFile).then((json) => {
      return util.promisify(fs.writeFile)(networkFile,
      JSON.stringify({
        ...JSON.parse(json),
        'Contracts': value
      }, null, 4));
    });
  },
  scripts: {
    'token:deploy': "script/saddle/deployToken.js",
    'token:verify': "script/saddle/verifyToken.js",
    'token:match': "script/saddle/matchToken.js",
    'flywheel:init': "script/saddle/flywheelInit.js"
  }
}
