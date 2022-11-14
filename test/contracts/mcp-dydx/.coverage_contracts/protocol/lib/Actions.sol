/*

    Copyright 2019 dYdX Trading Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

pragma solidity 0.5.7;
pragma experimental ABIEncoderV2;

import { Account } from "./Account.sol";
import { Types } from "./Types.sol";


/**
 * @title Actions
 * @author dYdX
 *
 * Library that defines and parses valid Actions
 */
library Actions {
function coverage_0x9e136e13(bytes32 c__0x9e136e13) public pure {}


    // ============ Constants ============

    bytes32 constant FILE = "Actions";

    // ============ Enums ============

    enum ActionType {
        Deposit,   // supply tokens
        Withdraw,  // borrow tokens
        Transfer,  // transfer balance between accounts
        Buy,       // buy an amount of some token (externally)
        Sell,      // sell an amount of some token (externally)
        Trade,     // trade tokens against another account
        Liquidate, // liquidate an undercollateralized or expiring account
        Vaporize,  // use excess tokens to zero-out a completely negative account
        Call       // send arbitrary data to an address
    }

    enum AccountLayout {
        OnePrimary,
        TwoPrimary,
        PrimaryAndSecondary
    }

    enum MarketLayout {
        ZeroMarkets,
        OneMarket,
        TwoMarkets
    }

    // ============ Structs ============

    /*
     * Arguments that are passed to Solo in an ordered list as part of a single operation.
     * Each ActionArgs has an actionType which specifies which action struct that this data will be
     * parsed into before being processed.
     */
    struct ActionArgs {
        ActionType actionType;
        uint256 accountId;
        Types.AssetAmount amount;
        uint256 primaryMarketId;
        uint256 secondaryMarketId;
        address otherAddress;
        uint256 otherAccountId;
        bytes data;
    }

    // ============ Action Types ============

    /*
     * Moves tokens from an address to Solo. Can either repay a borrow or provide additional supply.
     */
    struct DepositArgs {
        Types.AssetAmount amount;
        Account.Info account;
        uint256 market;
        address from;
    }

    /*
     * Moves tokens from Solo to another address. Can either borrow tokens or reduce the amount
     * previously supplied.
     */
    struct WithdrawArgs {
        Types.AssetAmount amount;
        Account.Info account;
        uint256 market;
        address to;
    }

    /*
     * Transfers balance between two accounts. The msg.sender must be an operator for both accounts.
     * The amount field applies to accountOne.
     * This action does not require any token movement since the trade is done internally to Solo.
     */
    struct TransferArgs {
        Types.AssetAmount amount;
        Account.Info accountOne;
        Account.Info accountTwo;
        uint256 market;
    }

    /*
     * Acquires a certain amount of tokens by spending other tokens. Sends takerMarket tokens to the
     * specified exchangeWrapper contract and expects makerMarket tokens in return. The amount field
     * applies to the makerMarket.
     */
    struct BuyArgs {
        Types.AssetAmount amount;
        Account.Info account;
        uint256 makerMarket;
        uint256 takerMarket;
        address exchangeWrapper;
        bytes orderData;
    }

    /*
     * Spends a certain amount of tokens to acquire other tokens. Sends takerMarket tokens to the
     * specified exchangeWrapper and expects makerMarket tokens in return. The amount field applies
     * to the takerMarket.
     */
    struct SellArgs {
        Types.AssetAmount amount;
        Account.Info account;
        uint256 takerMarket;
        uint256 makerMarket;
        address exchangeWrapper;
        bytes orderData;
    }

    /*
     * Trades balances between two accounts using any external contract that implements the
     * AutoTrader interface. The AutoTrader contract must be an operator for the makerAccount (for
     * which it is trading on-behalf-of). The amount field applies to the makerAccount and the
     * inputMarket. This proposed change to the makerAccount is passed to the AutoTrader which will
     * quote a change for the makerAccount in the outputMarket (or will disallow the trade).
     * This action does not require any token movement since the trade is done internally to Solo.
     */
    struct TradeArgs {
        Types.AssetAmount amount;
        Account.Info takerAccount;
        Account.Info makerAccount;
        uint256 inputMarket;
        uint256 outputMarket;
        address autoTrader;
        bytes tradeData;
    }

    /*
     * Each account must maintain a certain margin-ratio (specified globally). If the account falls
     * below this margin-ratio, it can be liquidated by any other account. This allows anyone else
     * (arbitrageurs) to repay any borrowed asset (owedMarket) of the liquidating account in
     * exchange for any collateral asset (heldMarket) of the liquidAccount. The ratio is determined
     * by the price ratio (given by the oracles) plus a spread (specified globally). Liquidating an
     * account also sets a flag on the account that the account is being liquidated. This allows
     * anyone to continue liquidating the account until there are no more borrows being taken by the
     * liquidating account. Liquidators do not have to liquidate the entire account all at once but
     * can liquidate as much as they choose. The liquidating flag allows liquidators to continue
     * liquidating the account even if it becomes collateralized through partial liquidation or
     * price movement.
     */
    struct LiquidateArgs {
        Types.AssetAmount amount;
        Account.Info solidAccount;
        Account.Info liquidAccount;
        uint256 owedMarket;
        uint256 heldMarket;
    }

    /*
     * Similar to liquidate, but vaporAccounts are accounts that have only negative balances
     * remaining. The arbitrageur pays back the negative asset (owedMarket) of the vaporAccount in
     * exchange for a collateral asset (heldMarket) at a favorable spread. However, since the
     * liquidAccount has no collateral assets, the collateral must come from Solo's excess tokens.
     */
    struct VaporizeArgs {
        Types.AssetAmount amount;
        Account.Info solidAccount;
        Account.Info vaporAccount;
        uint256 owedMarket;
        uint256 heldMarket;
    }

    /*
     * Passes arbitrary bytes of data to an external contract that implements the Callee interface.
     * Does not change any asset amounts. This function may be useful for setting certain variables
     * on layer-two contracts for certain accounts without having to make a separate Ethereum
     * transaction for doing so. Also, the second-layer contracts can ensure that the call is coming
     * from an operator of the particular account.
     */
    struct CallArgs {
        Account.Info account;
        address callee;
        bytes data;
    }

    // ============ Helper Functions ============

    function getMarketLayout(
        ActionType actionType
    )
        internal
        pure
        returns (MarketLayout)
    {coverage_0x9e136e13(0x5a2ef9f14568c859690529720da8a1aedf7c8c7cb5c17fef10663b0429519edc); /* function */ 

coverage_0x9e136e13(0x7ac85dcaf8acc2307bafc6251ee135c5058224922a3a4bb996d9459314764fbd); /* line */ 
        coverage_0x9e136e13(0x4cc04dae9c40db29ddc206dbdf58a223ba37f48c73dcb39757dd411befcfc24b); /* statement */ 
if (
            actionType == Actions.ActionType.Deposit
            || actionType == Actions.ActionType.Withdraw
            || actionType == Actions.ActionType.Transfer
        ) {coverage_0x9e136e13(0x214228dce80be8f33f9c0d22cbcc78956b7158283020120b804e03a0bf97841a); /* branch */ 

coverage_0x9e136e13(0xb3f4af47f772eb478af432f6d07ae35598287240f92294b2403ae8fba9b29f27); /* line */ 
            coverage_0x9e136e13(0xc5cc7cce07ef916df1302c45346e221ce9de15247899e7282f131e62d273abf1); /* statement */ 
return MarketLayout.OneMarket;
        }
        else {coverage_0x9e136e13(0x33900c6cc7d5f504526901a9f55f7545e00bf18da7119299f98a12f61c8ac8d8); /* statement */ 
coverage_0x9e136e13(0x7d61a50373f8a1054ed9ef8f808be5d93fc4523be3702694cca15bb3f532632f); /* branch */ 
if (actionType == Actions.ActionType.Call) {coverage_0x9e136e13(0xa8bd75491086298d0510a07aaa5538141f422ce015c606c2f7fb7507c2655ddc); /* branch */ 

coverage_0x9e136e13(0xf8b11afae18cbe3bb9f5e620513ad787f1d5944e384e47703ab4dc2229105938); /* line */ 
            coverage_0x9e136e13(0x11e8fba4302dbe3e066daebfc97fd8fdbc9acf61fd66d7117119f302d89d841a); /* statement */ 
return MarketLayout.ZeroMarkets;
        }else { coverage_0x9e136e13(0x210fb8fbc0ca6e671cdf97e97e2d14ac3978e076c0a35d1a070540046b3a61b0); /* branch */ 
}}
coverage_0x9e136e13(0x9741226ec4ad5096c90d1f9365a5655e95c0e7b3f95913fd4b1ae894044e03bb); /* line */ 
        coverage_0x9e136e13(0x8e12c864d9ba1deeed210f7330191aba94e08cfbfd5ddb63becac5b3c4e2e808); /* statement */ 
return MarketLayout.TwoMarkets;
    }

    function getAccountLayout(
        ActionType actionType
    )
        internal
        pure
        returns (AccountLayout)
    {coverage_0x9e136e13(0xd4eef7cc922b7bd4895b5a702ffea7ed40049b716ace1cb3423e7d7bbfbfe5ec); /* function */ 

coverage_0x9e136e13(0xa4ac3af7325ddd897f17b82c20a4355aff88856c06d594460b31d39bf1fbcc85); /* line */ 
        coverage_0x9e136e13(0xfe929e683c9cd2468672c545f27149f88c1f3bc1d0f500541415f180acecf6d7); /* statement */ 
if (
            actionType == Actions.ActionType.Transfer
            || actionType == Actions.ActionType.Trade
        ) {coverage_0x9e136e13(0x36e728e558eb9c4a8f98f7d33b7e818f3190435c8b77ffb34eca0e09a3fb4961); /* branch */ 

coverage_0x9e136e13(0x1e18abfcf290ab836b19bfeaee4e94231f6e33aec53ff53d4dc218943e60403c); /* line */ 
            coverage_0x9e136e13(0x826cce5012473df1b62514e5da283de3bce356e0f575faefbc888ae1a9ba64f5); /* statement */ 
return AccountLayout.TwoPrimary;
        } else {coverage_0x9e136e13(0xa5a7032838826cd3115817c23b0037c37e85bca19d65f75295e53d0ee9f21766); /* statement */ 
coverage_0x9e136e13(0xb0f956b1c33282885d22b6abf50ed4a25f1827e0efe1e09799eca9d19f92188a); /* branch */ 
if (
            actionType == Actions.ActionType.Liquidate
            || actionType == Actions.ActionType.Vaporize
        ) {coverage_0x9e136e13(0x25fe9d8e9923e9e56e322fb7d694abc10129bba5dc91dfe62d3e8889460f3038); /* branch */ 

coverage_0x9e136e13(0xd306dd0e18a2d3b24dab4c603bc7b729fe1c9b3be195ec8335d2ac2c88785955); /* line */ 
            coverage_0x9e136e13(0x2f677d50e6206cdaab09cc8b964e7ae270fcc0c411f6f58ffbccb3887900e544); /* statement */ 
return AccountLayout.PrimaryAndSecondary;
        }else { coverage_0x9e136e13(0xe279802b1d1e218d5c8b0d3dcbf0d257e6c037c623379d7d31057ddb6c445928); /* branch */ 
}}
coverage_0x9e136e13(0x39a863098b79c1f354254a23ec0da6ffdf2ca221653b23b2eeb74c66b9df584e); /* line */ 
        coverage_0x9e136e13(0x5f955263ac7df6726dad6187e4748f4c0905f8e56a00b346dd6f3c7624ee90db); /* statement */ 
return AccountLayout.OnePrimary;
    }

    // ============ Parsing Functions ============

    function parseDepositArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (DepositArgs memory)
    {coverage_0x9e136e13(0xb6f8ee8a92620e399ddb9de7b5ecc0d1b25e6534e7b51f301d8e94d6efeafa31); /* function */ 

coverage_0x9e136e13(0xc0e65b46c2ced15ae32dec679dd86319d5931e3b38746c76c778ed018d54282d); /* line */ 
        coverage_0x9e136e13(0xefd5dba3be6680dc48880e9c795e349404b7b320656f60c6f276a5f91298b0fa); /* assertPre */ 
coverage_0x9e136e13(0xce74042e619854b8a5279edacfd022e32ea181d60f7c64a95a511769eafb4a64); /* statement */ 
assert(args.actionType == ActionType.Deposit);coverage_0x9e136e13(0x77f8c053ce52d879ee964192edb89f6fcfbf6bcf159ae77f4b8ec19ffbf1b482); /* assertPost */ 

coverage_0x9e136e13(0xf3da63012086f5db720d6ed094e89e719acd0d009cda380c5cde13b8f8069f4f); /* line */ 
        coverage_0x9e136e13(0xd28dfc6e65f76c64805d64da01fc5dc5539ab13e07954101fc8fd96fd001c153); /* statement */ 
return DepositArgs({
            amount: args.amount,
            account: accounts[args.accountId],
            market: args.primaryMarketId,
            from: args.otherAddress
        });
    }

    function parseWithdrawArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (WithdrawArgs memory)
    {coverage_0x9e136e13(0xf6ce619c88729781268270592df3571bb30bed44eb6070c55d21860ffe18f33c); /* function */ 

coverage_0x9e136e13(0x76cb6ea4b9c151ac60f3372f1ff02dc85a639599bf08059a1b39a2a956431d8c); /* line */ 
        coverage_0x9e136e13(0xa128ea98cb940d853bcdbc23df3d6c14cc23fa6433a8371fdabd0a6d188a6ee3); /* assertPre */ 
coverage_0x9e136e13(0xe8ec371e4dc1b90f17f13b755087539118b958e4479a50b1bcf0c7bf3aa98d04); /* statement */ 
assert(args.actionType == ActionType.Withdraw);coverage_0x9e136e13(0xcdb2837459d9ca5ae565bc0bd5bf2e004bf5bc9a406e11c2f65a65e5e9982d40); /* assertPost */ 

coverage_0x9e136e13(0xe27932948578609ac1917688973bba394c36a14b97b11ee32bcf318f3a5c8675); /* line */ 
        coverage_0x9e136e13(0xfd6eb2ac5c57e0207b12c2661747232de37774728228fa73ab2c4ce17f401a22); /* statement */ 
return WithdrawArgs({
            amount: args.amount,
            account: accounts[args.accountId],
            market: args.primaryMarketId,
            to: args.otherAddress
        });
    }

    function parseTransferArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (TransferArgs memory)
    {coverage_0x9e136e13(0x07e3fc4a579fda6ae7d6fc94c432164eea931273652f5ec68c97ebe9e7d16d25); /* function */ 

coverage_0x9e136e13(0xda40acd1214b3e3e2600a4abcb3032b7163ef217a02bebed21eeaa86f4c600c0); /* line */ 
        coverage_0x9e136e13(0x2326855e35765c292bf18727b5fe6bfd3b71903d9634b699802001c6ce534cfe); /* assertPre */ 
coverage_0x9e136e13(0x432315b4a12c63b13c5f900dd0fc045dc08c3a60b430cd776f7985ca8c808073); /* statement */ 
assert(args.actionType == ActionType.Transfer);coverage_0x9e136e13(0x391cb2a8f4fabd3535ff8cff8fe4e3a426b13ab8ab2427910e99d7a3a2bf9bcb); /* assertPost */ 

coverage_0x9e136e13(0x4b9847e559e5c1e69151d95b716fdfb86f9c29ae12404a59361243f7f6ac08f4); /* line */ 
        coverage_0x9e136e13(0xbf581b9ffed861f8e1ae7ae4145431de09602f7b8cf93467f9c160288fb80312); /* statement */ 
return TransferArgs({
            amount: args.amount,
            accountOne: accounts[args.accountId],
            accountTwo: accounts[args.otherAccountId],
            market: args.primaryMarketId
        });
    }

    function parseBuyArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (BuyArgs memory)
    {coverage_0x9e136e13(0x7a4420daf43a2d2c0e496d401e119a7ebbed97697994b1184ca2b818be48b565); /* function */ 

coverage_0x9e136e13(0xf2327054fc506890778fbd20d4c957ab2960b94cce4cb110dc589be02a93486f); /* line */ 
        coverage_0x9e136e13(0x6d63af9a31bff2bdb11fb059d915498398ad74ce0e42b0ac31ac94f971789fd2); /* assertPre */ 
coverage_0x9e136e13(0x5df5a77c47b5d96397933d69be70ff0a5243337dd1507847bbc662633c48e4b2); /* statement */ 
assert(args.actionType == ActionType.Buy);coverage_0x9e136e13(0xf6ae9cac8df4475ecc850ab3c0bcf82629790e354035a1b76c2979d47c1a3ef6); /* assertPost */ 

coverage_0x9e136e13(0xb07352372b130971cb895ed6f7ec926f4d37dd972b7868d4509080b78a34e8b9); /* line */ 
        coverage_0x9e136e13(0x16b0d8cea5b7f58140a1c5df57e078bef79b8f630d63ad4b339027d8d89d7ac8); /* statement */ 
return BuyArgs({
            amount: args.amount,
            account: accounts[args.accountId],
            makerMarket: args.primaryMarketId,
            takerMarket: args.secondaryMarketId,
            exchangeWrapper: args.otherAddress,
            orderData: args.data
        });
    }

    function parseSellArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (SellArgs memory)
    {coverage_0x9e136e13(0x2c96bd715618bcf6ddac8f9da30671d8f6b9cf43b9cf6afa70207c4d3f5105e3); /* function */ 

coverage_0x9e136e13(0x840a8389ab7d12f355720df511b106af49be34cb3028303c52b56103ed529c77); /* line */ 
        coverage_0x9e136e13(0x8e59d94693406545f81a1e0aa398c8a1bc827c1daf05b74c45eb6e17000494f6); /* assertPre */ 
coverage_0x9e136e13(0x0404571b999a0f283f97ad9584a4f8ba2f15a405e545d4ab2ece57871c3abdb6); /* statement */ 
assert(args.actionType == ActionType.Sell);coverage_0x9e136e13(0xf1ae7e9b63da25a7118c0f644958ca70d1916443987932e50fec85d013b77cf1); /* assertPost */ 

coverage_0x9e136e13(0x27571417a9534c0cae4c692fd8e2a1d2190adeff09e63c25e7d84569d4dcc0af); /* line */ 
        coverage_0x9e136e13(0xf76ee164bf39592c5ec2834209087cad200d2a22c9ffc5aaab3e2f2bec81b757); /* statement */ 
return SellArgs({
            amount: args.amount,
            account: accounts[args.accountId],
            takerMarket: args.primaryMarketId,
            makerMarket: args.secondaryMarketId,
            exchangeWrapper: args.otherAddress,
            orderData: args.data
        });
    }

    function parseTradeArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (TradeArgs memory)
    {coverage_0x9e136e13(0xa83aac5ac417f5bbb29757aa0fd33727afce68cccc2bb12e49706167b58663d7); /* function */ 

coverage_0x9e136e13(0x722343e732c124cd07db000ae7906b571b77faf1f17f764e06b187722f8d41c2); /* line */ 
        coverage_0x9e136e13(0xe7c009c21d4a24f87d06ebe6ed3b17d800b29a38a01db92317c3688e758a9c8a); /* assertPre */ 
coverage_0x9e136e13(0xcfc4604837a404e1ade64e2e69be5a4b9d6433d28cd5a4cc242a59e7aa21900e); /* statement */ 
assert(args.actionType == ActionType.Trade);coverage_0x9e136e13(0xc4aad77b13046abfe43c1fe529d22c346525fdba34ae8bb2ea5e154131ef8423); /* assertPost */ 

coverage_0x9e136e13(0x391371fe2960c463665134e5479d422843b56a8f7d6fbe0dc2c8aa3827d7426a); /* line */ 
        coverage_0x9e136e13(0xb93c0220fc40e20ab5efb85f0843aacf3995de6071a07a0816d6f201a13000e5); /* statement */ 
return TradeArgs({
            amount: args.amount,
            takerAccount: accounts[args.accountId],
            makerAccount: accounts[args.otherAccountId],
            inputMarket: args.primaryMarketId,
            outputMarket: args.secondaryMarketId,
            autoTrader: args.otherAddress,
            tradeData: args.data
        });
    }

    function parseLiquidateArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (LiquidateArgs memory)
    {coverage_0x9e136e13(0x95d4fe3845ce5eb60dc5716e3f5232d4f78806edd011a5e3e5e21f9f21338f5a); /* function */ 

coverage_0x9e136e13(0x7204c6ad051391ae4a9000ae1c5e3cd5441b9d36f4a242a06b2848a7e4155e73); /* line */ 
        coverage_0x9e136e13(0x054467f15ce0fcfaf304385fe5d90df09a47c09a67729f0841218255a8153e66); /* assertPre */ 
coverage_0x9e136e13(0x710f182b00be8e352756828e9f0199ad495fe23cf5e753343523e7e93f18a7ce); /* statement */ 
assert(args.actionType == ActionType.Liquidate);coverage_0x9e136e13(0xf2f938f3e506ffd54f0dc24bb4f8d66313fea7ad7e8dc31a6d919b81a4f5723f); /* assertPost */ 

coverage_0x9e136e13(0x04e933755c1780951bbf7de13ca53a5b5ad4345e6183eda470a596cd0abe2fe5); /* line */ 
        coverage_0x9e136e13(0xc4ea9862258b66e22f3aba9df8a8d6926279aa9309483ee4e02bd90f99d5b016); /* statement */ 
return LiquidateArgs({
            amount: args.amount,
            solidAccount: accounts[args.accountId],
            liquidAccount: accounts[args.otherAccountId],
            owedMarket: args.primaryMarketId,
            heldMarket: args.secondaryMarketId
        });
    }

    function parseVaporizeArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (VaporizeArgs memory)
    {coverage_0x9e136e13(0xcd1af839b193575d6058f85c23941d1df90782ac22e76b5933dbd0da33b36fbe); /* function */ 

coverage_0x9e136e13(0xcc6a5d57779f571680a18c727a801737d324c587ba60b129f486fc24d838ca99); /* line */ 
        coverage_0x9e136e13(0xc26e32d42c6cf54ec2be69cc537d13c6979c222284be0fbf0076abcd8ddf933a); /* assertPre */ 
coverage_0x9e136e13(0x6f67a89cca211216205eb4b6c30e00c38175216a30151d0400905ed21d2dba31); /* statement */ 
assert(args.actionType == ActionType.Vaporize);coverage_0x9e136e13(0x2ffd7e305a8a4dac52d862bc58cd1c18ab0788c35a0a246bb187078a5ddc9596); /* assertPost */ 

coverage_0x9e136e13(0xa4dd176e4c47f949d4083a75bd4c2c02824319584f39f72bd2de7778fcfd1e3f); /* line */ 
        coverage_0x9e136e13(0xb96d7711456c23b622ecfbcb963cb58763a239dc58e123384f7c5fd511f1713d); /* statement */ 
return VaporizeArgs({
            amount: args.amount,
            solidAccount: accounts[args.accountId],
            vaporAccount: accounts[args.otherAccountId],
            owedMarket: args.primaryMarketId,
            heldMarket: args.secondaryMarketId
        });
    }

    function parseCallArgs(
        Account.Info[] memory accounts,
        ActionArgs memory args
    )
        internal
        pure
        returns (CallArgs memory)
    {coverage_0x9e136e13(0x4d8907752d4b6706c759316a59112fb6e680e65c4a6bf778a4f418f1def89bb1); /* function */ 

coverage_0x9e136e13(0xf6593c804a73b95512d75c5c889adfe8403ab149ae807046336b95842fc7eaa9); /* line */ 
        coverage_0x9e136e13(0x8359e7e97ffae0ffe24f91091b869f4f13dd4dbfca4045a800b80ab3a877cb33); /* assertPre */ 
coverage_0x9e136e13(0x5f185aa9bbeba968cda973582e3c9d848d107422666b3ee029607d9666d6721b); /* statement */ 
assert(args.actionType == ActionType.Call);coverage_0x9e136e13(0x69ebeab50a72bf323971e3b6c1704b07ab39613c62d8448c7d5d62572b8327ad); /* assertPost */ 

coverage_0x9e136e13(0x167ed92bd43c37a1188c9537a7240a5826b953dfa8d18f0fecbb5f4e10a19e3d); /* line */ 
        coverage_0x9e136e13(0x3caf0b51a93f3676c3990d741f1639747ef7898c4fc22b441e62a3cecf8ce975); /* statement */ 
return CallArgs({
            account: accounts[args.accountId],
            callee: args.otherAddress,
            data: args.data
        });
    }
}
