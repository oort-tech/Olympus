const chainName = (chainId) => {
    switch(chainId) {
      case 30: return 'Rsk Mainnet';
      case 31: return 'Rsk testnet';
      case 33: return 'Rsk regtest';
      case 1337: return 'Ganache';
      case 3: return 'Ropsten';
      case 31337: return 'hardhatEVM';
      case 804: return 'MCP';
      default: return 'Unknown';
    }
}
const parseEther = ethers.utils.parseEther;
const config = {
    initialExchangeRateMantissa:  '205082655126857156242949754',
    liquidationIncentiveMantisa: parseEther('1.08'),
    closeFactorMantisa: parseEther('0.5'),
    compSpeed: parseEther('0'), //0 to not drip
};


module.exports = async (hardhat) => {
    const { getNamedAccounts, deployments, getChainId, ethers } = hardhat
    const { deploy, execute } = deployments

    let {
      deployer,
      daiOracle,
      ftmOracle,
      usdtOracle,
      usdt,
      dai,
      multiSig,
      admin1,
      admin2
    } = await getNamedAccounts()

    const chainId = parseInt(await getChainId(), 10)
    console.log('ChainID', chainId);
    const isLocal = [30, 31].indexOf(chainId) == -1
    // 31337 is unit testing, 1337 is for coverage, 33 is rsk regtest
    const isTestEnvironment = chainId === 31337 || chainId === 1337 || chainId === 33 || chainId === 3;
    console.log('isTestEnvironment', isTestEnvironment);
    // Fix transaction format  error from etherjs getTransactionReceipt as transactionReceipt format
    // checks root to be a 32 bytes hash when on RSK its 0x01
    const format = ethers.provider.formatter.formats
    if (format) format.receipt['root'] = format.receipt['logsBloom']
    Object.assign(ethers.provider.formatter, { format: format })

    const signer = await ethers.provider.getSigner(deployer)
    Object.assign(signer.provider.formatter, { format: format })

    console.log('deploy with ', deployer);

    console.log("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
    console.log("MarketIndicator Contracts - Deploy Script")
    console.log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")

    const locus = isLocal ? 'local' : 'remote'
    console.log(`  Deploying to Network: ${chainName(chainId)} (${locus})`)

    if (isLocal) { // ----------- Start if local ------------- //


      console.log("\n  Deploying FTM Oracle...")
      const ftmOracleResult = await deploy("FTMOracle", {
        args: [deployer, '3912800000000000000000'],
        contract: 'MockPriceProviderMoC',
        from: deployer,
        skipIfAlreadyDeployed: false
      });
      ftmOracle = ftmOracleResult.address


      const usdtOracleResult = await deploy("USDTOracle", {
        args: [deployer, ethers.utils.parseEther('1')],
        contract: 'MockPriceProviderMoC',
        from: deployer,
        skipIfAlreadyDeployed: false
      });
      usdtOracle = usdtOracleResult.address

      console.log("\n  Deploying USDT...")
      const usdtResult = await deploy("USDT", {
        args: [ethers.utils.parseEther('2000000'), "USDT token", 18, "USDT"],
        contract: 'StandardToken',
        from: deployer,
        skipIfAlreadyDeployed: false
      })
      usdt = usdtResult.address

      

      // Display Contract Addresses
      console.log("\n 🔹 Local Contract Deployments;\n")
      // console.log("  - FTM Oracle:       ", ftmOracleResult.address)
      // console.log("  - Dai Oracle:       ", daiOracleResult.address)
      console.log("  - USDT:              ", usdtResult.address)
      // console.log("  - Dai:              ", daiResult.address)
    } // ----------- End if local ------------- //



    // USDT Oracle returns always 1
    console.log("\n 🔸 Deploying USDT Oracle...")
    console.log('multiSig', multiSig);
    // if not set by named config
    if (!multiSig) {
        console.log("\n  Deploying MultiSigWallet...")
        const owners =  isLocal ? [deployer] : [deployer, admin1, admin2]
        const multiSigResult = await deploy("MultiSigWallet", {
            args: [owners, 1],
            contract: "MultiSigWallet",
            from: deployer,
            skipIfAlreadyDeployed: false
        })
        multiSig = multiSigResult.address
    }
    
    const multiSigContract = await ethers.getContractAt(
        "MultiSigWallet",
        multiSig,
        signer
    )

    console.log("\n  Deploying Unitroller...")
    const unitrollerResult = await deploy("Unitroller", {
        contract: "Unitroller",
        from: deployer,
        skipIfAlreadyDeployed: false
    })
    const unitrollerContract = await ethers.getContractAt(
        "Unitroller",
        unitrollerResult.address,
        signer
    )



    //-------------- Start deploying Oracles Adapters ------------- //
    console.log("\n  Deploying PriceOracleProxy...")
    const priceOracleProxyResult = await deploy("PriceOracleProxy", {
        args: [deployer],
        contract: "PriceOracleProxy",
        from: deployer,
        skipIfAlreadyDeployed: false
    })
    const priceOracleProxyContract = await ethers.getContractAt(
        "PriceOracleProxy",
        priceOracleProxyResult.address,
        signer
    )


    // console.log("\n  Deploying UsdtPriceOracleAdapterMoc...")
    // const usdtPriceOracleAdapterResult = await deploy("UsdtPriceOracleAdapterMoc", {
    //     args: [multiSig, usdtOracle],
    //     contract: "PriceOracleAdapterMoc",
    //     from: deployer,
    //     skipIfAlreadyDeployed: false
    // })

    
    // ----------- End deploying Oracles Adapters ------------ //



    // ------------ Start Deploying and configuring Comptroller --------- //
    console.log("\n  Deploying Comptroller...")
    const comptrollerResult = await deploy("Comptroller", {
        contract: "Comptroller",
        from: deployer,
        skipIfAlreadyDeployed: false
    })

    if (comptrollerResult.newlyDeployed) {
        console.log("\n  _setPendingImplementation Unitroller...")
        await execute("Unitroller", {from: deployer}, "_setPendingImplementation", comptrollerResult.address)
        console.log("\n  _become Comptroller...")
        await execute("Comptroller", {from: deployer}, "_become", unitrollerResult.address)
    } else {
        console.log("\n  already become Unitroller...")
    }

    const newUnitrollerContract = await ethers.getContractAt(
        "Comptroller",
        unitrollerContract.address,
        signer
    )

    if (comptrollerResult.newlyDeployed) {
        console.log("\n  _setPriceOracle new Unitroller...")
        await newUnitrollerContract._setPriceOracle(priceOracleProxyResult.address).then((tx) => tx.wait());

        console.log("\n  _setCloseFactor new Unitroller...")
        await newUnitrollerContract._setCloseFactor(config.closeFactorMantisa).then((tx) => tx.wait());

        console.log("\n  _setLiquidationIncentive new Unitroller...")
        await newUnitrollerContract._setLiquidationIncentive(config.liquidationIncentiveMantisa).then((tx) => tx.wait());

    } else {
        console.log("\n  already setted up new Unitroller...")
    }
    // ------------ End Deploying and configuring Comptroller --------- //



    // --------------------- Deploy InterestRateModel ----------------- //
    // nice explination of the arguments https://compound.finance/governance/proposals/23
    console.log("\n  USDT Deploy JumpRateModelV2...")
    const usdtJumpRateModelV2Result = await deploy("UsdtJumpRateModelV2", {
        // 0% base rate, 4% borrow rate at kink, 25% borrow rate at 100% utilization, Kink at 80% utilization
        args: [parseEther('0'), parseEther('0.04'), parseEther('1.09'), parseEther('0.8'), multiSig],
        contract: "JumpRateModelV2",
        from: deployer,
        skipIfAlreadyDeployed: false
    })
    console.log("\n  Deploy FTM WhitePaperInterestRateModel...")
    const ftmWhitePaperInterestRateModelResult = await deploy("FtmWhitePaperInterestRateModel", {
        args: [parseEther('0.02'), parseEther('0.3')],
        contract: "WhitePaperInterestRateModel",
        from: deployer,
        skipIfAlreadyDeployed: false
    })
    console.log("\n  Deploy DAI JumpRateModelV2...")
    const daiInterestRateModelResult = await deploy("DaiJumpRateModelV2", {
        // 2% base rate, 30% borrow rate at kink, Kink at 80%, 150% borrow rate at 100% utilization
        args: [parseEther('0.02'), parseEther('0.30'), parseEther('6'), parseEther('0.8'), multiSig],
        contract: "JumpRateModelV2",
        from: deployer,
        skipIfAlreadyDeployed: false
    })
    // --------------------- End Deploy InterestRateModel ----------------- //

    // -------------------------- Deploy CTokerns ------------------------- //
    // ### Deploy miUSDT ### //
    /*
    console.log("\n  Deploy miUSDT...", usdt)
    const miUsdtResult = await deploy("miUSDT", {
        args: [usdt, newUnitrollerContract.address, usdtJumpRateModelV2Result.address, config.initialExchangeRateMantissa, "Market Indicator USDT", "miUSDT", 8, deployer],
        contract: "CErc20Immutable",
        from: deployer,
        skipIfAlreadyDeployed: false
    })

    const miUsdtContract = await ethers.getContractAt(
        "CErc20Immutable",
        miUsdtResult.address,
        signer
    )
    if (miUsdtResult.newlyDeployed) {
        console.log("\n  setAdapterToToken miUSDT...")
        await priceOracleProxyContract.setAdapterToToken(miUsdtResult.address, usdtPriceOracleAdapterResult.address).then((tx) => tx.wait())

        console.log("\n  _supportMarket miUSDT...")
        await newUnitrollerContract._supportMarket(miUsdtResult.address).then((tx) => tx.wait())

        console.log("\n  _setCollateralFactor miUSDT...")
        await newUnitrollerContract._setCollateralFactor(miUsdtResult.address, parseEther('0.75')).then((tx) => tx.wait())

        console.log("\n  _setCompSpeed new Unitroller...")
        result = await newUnitrollerContract._setCompSpeed(miUsdtResult.address, config.compSpeed).then((tx) => tx.wait());

        console.log("\n  _setReserveFactor miUSDT...")
        await miUsdtContract._setReserveFactor(parseEther('0.15')).then((tx) => tx.wait())
    } else {
        console.log("\n miUSDT already deployed...")
    }
*/
    let tokens = [
        {
            symbol: 'wBTC',
            name: 'Wrapped Bitcoin',
            decimals: 18,
            oracle: '0x65E8d79f3e8e36fE48eC31A2ae935e92F5bBF529',
            pair: '0xe0215f05D0b589f9f2A706A9125209b1680Afac1',
            reversed: true,
            anchorToleranceMantissa: '150000000000000000',
            interestRateModel: daiInterestRateModelResult.address,
        },
        {
            symbol: 'wETH',
            name: 'Wrapped Ether',
            decimals: 18,
            pair: '0xe0215f05D0b589f9f2A706A9125209b1680Afac1',
            reversed: true,
            anchorToleranceMantissa: '150000000000000000',
            oracle: '0xB8C458C957a6e6ca7Cc53eD95bEA548c52AFaA24',
            interestRateModel: ftmWhitePaperInterestRateModelResult.address,
        },
        {
            symbol: 'LINK',
            name: 'Chainlink',
            decimals: 18,
            pair: '0xe0215f05D0b589f9f2A706A9125209b1680Afac1',
            reversed: true,
            anchorToleranceMantissa: '150000000000000000',
            oracle: '0x6d5689Ad4C1806D1BA0c70Ab95ebe0Da6B204fC5',
            interestRateModel: daiInterestRateModelResult.address,
        },
        {
            symbol: 'USDT',
            name: 'Tether',
            decimals: 6,
            pair: '0xe0215f05D0b589f9f2A706A9125209b1680Afac1',
            reversed: true,
            anchorToleranceMantissa: '150000000000000000',
            oracle: '0x9BB8A6dcD83E36726Cc230a97F1AF8a84ae5F128',
            interestRateModel: daiInterestRateModelResult.address,
        },
    ]

    let underlyingAddress = {};
    let miTokenAddress = {};
    let priceOracleAdapter = {};
    
    for (let token of tokens) {
        console.log(`\n  Deploying ${token.symbol}...`)
        const tokenResult = await deploy(token.symbol, {
            args: [ethers.utils.parseEther('2000000'), token.name, token.decimals, token.symbol],
            contract: 'StandardToken',
            from: deployer,
            skipIfAlreadyDeployed: false
        })
        tokenAddress = tokenResult.address;
        underlyingAddress[token.symbol] = tokenAddress;

        // ### Deploy mi Token ### //
        console.log(`\n  Deploy mi${token.symbol}...`)
        const miTokenResult = await deploy(`mi${token.symbol}`, {
            args: [tokenAddress, newUnitrollerContract.address, token.interestRateModel, config.initialExchangeRateMantissa, `Market Indicator ${token.symbol}`, `mi${token.symbol}`, 8, deployer],
            contract: "CErc20Immutable",
            from: deployer,
            skipIfAlreadyDeployed: false
        });
        console.log(`mi${token.symbol} deployed`);
        miTokenAddress[token.symbol] = miTokenResult.address;
        const miTokenContract = await ethers.getContractAt(
            "CErc20Immutable",
            miTokenResult.address,
            signer
        )

        console.log("\n  Deploying PriceOracleAdapterMoc...")
        const priceOracleAdapterResult = await deploy(`${token.symbol}PriceOracleAdapterMoc`, {
            args: [multiSig, token.oracle, token.pair, token.reversed, token.anchorToleranceMantissa],
            contract: "PriceOracleAdapterMoc",
            from: deployer,
            skipIfAlreadyDeployed: false
        });

        priceOracleAdapter[token.symbol] = priceOracleAdapterResult.address;

        if (miTokenResult.newlyDeployed) {
            console.log(`\n  setAdapterToToken mi${token.symbol}...`)
            await priceOracleProxyContract.setAdapterToToken(miTokenResult.address, priceOracleAdapterResult.address).then((tx) => tx.wait())

            console.log(`\n  _supportMarket mi${token.symbol}...`)
            await newUnitrollerContract._supportMarket(miTokenResult.address).then((tx) => tx.wait())

            console.log(`\n  _setCollateralFactor mi${token.symbol}...`)
            await newUnitrollerContract._setCollateralFactor(miTokenResult.address, parseEther('0.75')).then((tx) => tx.wait())

            console.log(`\n  _setCompSpeed new Unitroller...`)
            result = await newUnitrollerContract._setCompSpeed(miTokenResult.address, config.compSpeed).then((tx) => tx.wait());

            console.log(`\n  _setReserveFactor mi${token.symbol}...`)
            await miTokenContract._setReserveFactor(parseEther('0.15')).then((tx) => tx.wait())
        } else {
            console.log(`\n mi${token.symbol} already deployed...`)
        }
    }

    // ### Deploy miFTM ### //
    console.log("\n  Deploy miFTM...")
    const miFTMResult = await deploy("MIFTM", {
        args: [newUnitrollerContract.address, ftmWhitePaperInterestRateModelResult.address, config.initialExchangeRateMantissa, "Market Indicator FTM", "miFTM", 8, deployer],
        contract: "MIEther", //mifantom
        from: deployer,
        skipIfAlreadyDeployed: false
    })
    const miFTMContract = await ethers.getContractAt(
        "MIEther",
        miFTMResult.address,
        signer
    )
    ftmOracle = '0xe04676B9A9A2973BCb0D1478b5E1E9098BBB7f3D';
    ftmpair = '0xe0215f05D0b589f9f2A706A9125209b1680Afac1';
    ftmreversed = true;
    ftmanchorToleranceMantissa = '150000000000000000';
    console.log("\n  Deploying FTMPriceOracleAdapterMoc...")
    const ftmPriceOracleAdapterResult = await deploy("FTMPriceOracleAdapterMoc", {
        args: [multiSig, ftmOracle, ftmpair, ftmreversed, ftmanchorToleranceMantissa],
        contract: "PriceOracleAdapterMoc",
        from: deployer,
        skipIfAlreadyDeployed: false
    });

    if (miFTMResult.newlyDeployed) {
        console.log("\n  setAdapterToToken miFTM...")
        await priceOracleProxyContract.setAdapterToToken(miFTMResult.address, ftmPriceOracleAdapterResult.address).then((tx) => tx.wait())

        console.log(`\n  _supportMarket miFTM...`)
        await newUnitrollerContract._supportMarket(miFTMResult.address).then((tx) => tx.wait())

        console.log("\n  _setCollateralFactor miFTM...")
        await newUnitrollerContract._setCollateralFactor(miFTMResult.address, parseEther('0.75')).then((tx) => tx.wait())

        console.log("\n  _setCompSpeed new Unitroller...")
        result = await newUnitrollerContract._setCompSpeed(miFTMResult.address, config.compSpeed).then((tx) => tx.wait());

        console.log("\n  _setReserveFactor miFTM...")
        await miFTMContract._setReserveFactor(parseEther('0.2')).then((tx) => tx.wait())
    } else {
        console.log("\n miFTM already deployed...")
    }
    // -------------------------- End Deploy CTokerns ------------------------- //

    // -------------------------- Deploy rLen ------------------------- //
    console.log("\n  Deploy RLEN...")
    const rLenResult = await deploy("RLEN", {
        args: [multiSig],
        contract: "RLEN",
        from: deployer,
        skipIfAlreadyDeployed: false
    })
    if (rLenResult.newlyDeployed) {
        console.log("\n  setCompAddress RLEN...")
        await newUnitrollerContract.setCompAddress(rLenResult.address).then((tx) => tx.wait());
    } else {
        console.log("\n RLEN already deployed...")
    }
    // -------------------------- End Deploy rLen ------------------------- //

    // -------------------------- Deploy Maximillion ------------------------- //
    console.log("\n  Deploy Maximillion...")
    const maximillionResult = await deploy("Maximillion", {
        args: [miFTMResult.address],
        contract: "Maximillion",
        from: deployer,
        skipIfAlreadyDeployed: false
    })

    // -------------------------- Deploy MarketIndicatorLens ------------------------- //
    console.log("\n  Deploy MarketIndicatorLens...")
    const MILensResult = await deploy("MarketIndicatorLens", {
        contract: "MarketIndicatorLens",
        from: deployer,
        skipIfAlreadyDeployed: false
    })

    // -------------------------- setMultiSignOwnerAlpha ------------------------- //
    console.log("\n  set Multisig  as Owner...")
    let arrayToMultisigOwner = [miFTMContract] //, miDaiContract, miUsdtContract, priceOracleProxyContract, unitrollerContract];
    for (let index = 0; index < arrayToMultisigOwner.length; index++) {
        //set pending admin
        console.log(`\n  _setPendingAdmin Multisig...`)
        await arrayToMultisigOwner[index]["_setPendingAdmin"](multiSig)
        //generate data method accept admin
        const data = arrayToMultisigOwner[index].interface.encodeFunctionData("_acceptAdmin",[])
        //submit transacion multisig, when accept the admin of contract
        console.log(`\n  _acceptAdmin Multisig...`)
        let receipt
        console.log('address', arrayToMultisigOwner[index].address)
        console.log('data', data)
        receipt = await multiSigContract.submitTransaction(arrayToMultisigOwner[index].address, 0, data).then((tx) => tx.wait())
        // console.log('receipt', receipt)
        console.log(`multiSig owner of ${arrayToMultisigOwner[index].address}`)
    }
    console.log("\n  changeRequirement Multisig ...")
    let data = multiSigContract.interface.encodeFunctionData("changeRequirement",isLocal?[1]:[2])
    let owners = await multiSigContract.getOwners()
    console.log('owners', owners)
    //submit transacion multisig
    console.log('multiSigContract.address', multiSigContract.address)
    console.log('data', data)
    await multiSigContract.submitTransaction(multiSigContract.address, 0, data).then((tx) => tx.wait())
    const multicallResult = await deploy("Multicall", {
        contract: "Multicall",
        from: deployer,
        skipIfAlreadyDeployed: false
    })

    // Display Contract Addresses
    console.log("\n  Contract Deployments Complete!\n")
    console.log(`   Unitroller:                       '${unitrollerResult.address}',`)
    console.log(`   PriceOracleProxy:                '${priceOracleProxyResult.address}',`)
    for (let token of tokens)
        console.log(`   PriceOracleAdapter${token.symbol}: '${priceOracleAdapter[token.symbol]}',`)
    // console.log(`   PriceOracleAdapteUSDT:         '${usdtPriceOracleAdapterResult.address}',`)
    console.log(`   PriceOracleAdapterFTM:        '${ftmPriceOracleAdapterResult.address}',`)
    console.log(`   MILens:                    '${MILensResult.address}',`)
    for (let token of tokens)
        console.log(`   mi${token.symbol}:      '${miTokenAddress[token.symbol]}',`)
    // console.log(`   miDAI:                            '${miDaiResult.address}',`)
    console.log(`   miFTM:                           '${miFTMResult.address}',`)
    console.log(`   RLEN:                            '${rLenResult.address}',`)
    console.log(`   MaximillionAddress:                     '${maximillionResult.address}',`)
    // console.log(`   FTMMocOracle:                 '${ftmOracle}',`)
    // console.log(`   DAIMocOracle:                  '${daiOracle}',`)
    for (let token of tokens)
        console.log(`   ${token.symbol}:      '${underlyingAddress[token.symbol]}',`)
    console.log(`   Multicall:                       '${multicallResult.address}',`)
    console.log(`   MultiSigWallet:                  '${multiSig}',`)
    console.log(`   Market Indicator DAI JumpRateInterestRateModel: '${daiInterestRateModelResult.address}',`)
    console.log(`   Comptroller (Logic):             '${comptrollerResult.address}',`)
    console.log(`   USDT JumpRateModelV2:            '${usdtJumpRateModelV2Result.address}',`)
    console.log(`   FTM WhitePaperInterestRateModel: '${ftmWhitePaperInterestRateModelResult.address}',`)
    console.log("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")

    console.log('\n \x1b[32m%s\x1b[0m', "All done..", "🌱\n");

}