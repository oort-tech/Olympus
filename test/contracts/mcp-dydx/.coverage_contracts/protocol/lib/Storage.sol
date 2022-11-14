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

import { SafeMath } from "openzeppelin-solidity/contracts/math/SafeMath.sol";
import { Account } from "./Account.sol";
import { Cache } from "./Cache.sol";
import { Decimal } from "./Decimal.sol";
import { Interest } from "./Interest.sol";
import { Math } from "./Math.sol";
import { Monetary } from "./Monetary.sol";
import { Require } from "./Require.sol";
import { Time } from "./Time.sol";
import { Token } from "./Token.sol";
import { Types } from "./Types.sol";
import { IInterestSetter } from "../interfaces/IInterestSetter.sol";
import { IPriceOracle } from "../interfaces/IPriceOracle.sol";


/**
 * @title Storage
 * @author dYdX
 *
 * Functions for reading, writing, and verifying state in Solo
 */
library Storage {
function coverage_0xe2a4dd29(bytes32 c__0xe2a4dd29) public pure {}

    using Cache for Cache.MarketCache;
    using Storage for Storage.State;
    using Math for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;
    using SafeMath for uint256;

    // ============ Constants ============

    bytes32 constant FILE = "Storage";

    // ============ Structs ============

    // All information necessary for tracking a market
    struct Market {
        // Contract address of the associated ERC20 token
        address token;

        // Total aggregated supply and borrow amount of the entire market
        Types.TotalPar totalPar;

        // Interest index of the market
        Interest.Index index;

        // Contract address of the price oracle for this market
        IPriceOracle priceOracle;

        // Contract address of the interest setter for this market
        IInterestSetter interestSetter;

        // Multiplier on the marginRatio for this market
        Decimal.D256 marginPremium;

        // Multiplier on the liquidationSpread for this market
        Decimal.D256 spreadPremium;

        // Whether additional borrows are allowed for this market
        bool isClosing;
    }

    // The global risk parameters that govern the health and security of the system
    struct RiskParams {
        // Required ratio of over-collateralization
        Decimal.D256 marginRatio;

        // Percentage penalty incurred by liquidated accounts
        Decimal.D256 liquidationSpread;

        // Percentage of the borrower's interest fee that gets passed to the suppliers
        Decimal.D256 earningsRate;

        // The minimum absolute borrow value of an account
        // There must be sufficient incentivize to liquidate undercollateralized accounts
        Monetary.Value minBorrowedValue;
    }

    // The maximum RiskParam values that can be set
    struct RiskLimits {
        uint64 marginRatioMax;
        uint64 liquidationSpreadMax;
        uint64 earningsRateMax;
        uint64 marginPremiumMax;
        uint64 spreadPremiumMax;
        uint128 minBorrowedValueMax;
    }

    // The entire storage state of Solo
    struct State {
        // number of markets
        uint256 numMarkets;

        // marketId => Market
        mapping (uint256 => Market) markets;

        // owner => account number => Account
        mapping (address => mapping (uint256 => Account.Storage)) accounts;

        // Addresses that can control other users accounts
        mapping (address => mapping (address => bool)) operators;

        // Addresses that can control all users accounts
        mapping (address => bool) globalOperators;

        // mutable risk parameters of the system
        RiskParams riskParams;

        // immutable risk limits of the system
        RiskLimits riskLimits;
    }

    // ============ Functions ============

    function getToken(
        Storage.State storage state,
        uint256 marketId
    )
        internal
        view
        returns (address)
    {coverage_0xe2a4dd29(0x8dfab0328211379c918e2801a61717e62b05eeea6107d58bcf96b734fdaeec0c); /* function */ 

coverage_0xe2a4dd29(0x6727b3efbe16d3c66f7af2382392cd31ff0738551e535255768977c9400dc65c); /* line */ 
        coverage_0xe2a4dd29(0x938a8bd55c3b834002ad3cc7c55517b74d548d0e4cfebb5b753fcd269acad903); /* statement */ 
return state.markets[marketId].token;
    }

    function getTotalPar(
        Storage.State storage state,
        uint256 marketId
    )
        internal
        view
        returns (Types.TotalPar memory)
    {coverage_0xe2a4dd29(0x8ed8a6c9af1124a952437ad379bb5af3129a98e369499ca0692e13668d46c9af); /* function */ 

coverage_0xe2a4dd29(0x0e429c6c35ca2c1a74f10a225e5b105bfeabab66a38cda52179ac996ed229c50); /* line */ 
        coverage_0xe2a4dd29(0x641a02fd758951d8272806c46833ef29ea629238fe43a0d8e55bd37e4c6729aa); /* statement */ 
return state.markets[marketId].totalPar;
    }

    function getIndex(
        Storage.State storage state,
        uint256 marketId
    )
        internal
        view
        returns (Interest.Index memory)
    {coverage_0xe2a4dd29(0x7cd444b039bd38933b1336927ce97a36bd2e695b921fc90e65cb8b26443e36d3); /* function */ 

coverage_0xe2a4dd29(0x62e5f931048bc65d843638eed12cf24b55d88d6015e10d437de1a7e28d533ada); /* line */ 
        coverage_0xe2a4dd29(0x9034aadc7d941abd58e08a4b4bf0574b18a8b9dc651e40b0e2a1a49a33f23a3c); /* statement */ 
return state.markets[marketId].index;
    }

    function getNumExcessTokens(
        Storage.State storage state,
        uint256 marketId
    )
        internal
        view
        returns (Types.Wei memory)
    {coverage_0xe2a4dd29(0x09aaeff5490c1476bd4945a43c8dc7e988546697b93a530988326e1dae73c6e7); /* function */ 

coverage_0xe2a4dd29(0x93985d7e24832ddbbbd5361c52e3f2828f4d74707245a7cdba9f5e3a93288703); /* line */ 
        coverage_0xe2a4dd29(0xd682417b05b7becd9d3bdbbf996dac842804eb6814f1762dd78852dc9333c228); /* statement */ 
Interest.Index memory index = state.getIndex(marketId);
coverage_0xe2a4dd29(0xa5fdd84ce7df585d117518257873dedd15f725aab75e783c0977c402d2b12e19); /* line */ 
        coverage_0xe2a4dd29(0x6cd3ed044e97418a4b6cda247dd58006c10229ba2587c36dc7590b023e9ab2bc); /* statement */ 
Types.TotalPar memory totalPar = state.getTotalPar(marketId);

coverage_0xe2a4dd29(0x5c520bf48d3c9fb68c94bbc4cf7418361842f0bc87ae428f7765f0ee7f8a9401); /* line */ 
        coverage_0xe2a4dd29(0x31b4ff237e83f7123537d52f443906aeafa63e611b2d5212a6f7df0d578aafc9); /* statement */ 
address token = state.getToken(marketId);

coverage_0xe2a4dd29(0xaa1221cc9c03a1367c5a7350aadc63f8d98ff4d45c70fa3e438462887fa0c17d); /* line */ 
        coverage_0xe2a4dd29(0x7aabab21164767c1c51f448065a0e1be1611155c0feaac7e4292e63bd245faf1); /* statement */ 
Types.Wei memory balanceWei = Types.Wei({
            sign: true,
            value: Token.balanceOf(token, address(this))
        });

coverage_0xe2a4dd29(0x47cb20045b3295eb8d0483ff622926f0cda3b472553dbce8c7d6705115190665); /* line */ 
        coverage_0xe2a4dd29(0x81f61e593dbb3e31aa5f51751f437fa6b93d98d621c4c8b0ae34690da6c900f8); /* statement */ 
(
            Types.Wei memory supplyWei,
            Types.Wei memory borrowWei
        ) = Interest.totalParToWei(totalPar, index);

        // borrowWei is negative, so subtracting it makes the value more positive
coverage_0xe2a4dd29(0x5305298aa77296eb76253e7b7c1562e05e9eff8e1371415c7fa8cfde467b560d); /* line */ 
        coverage_0xe2a4dd29(0xec62d0a30d85357efa2b5e409f7d839499a27869798a4ee11dd5790b7c6ce17d); /* statement */ 
return balanceWei.sub(borrowWei).sub(supplyWei);
    }

    function getStatus(
        Storage.State storage state,
        Account.Info memory account
    )
        internal
        view
        returns (Account.Status)
    {coverage_0xe2a4dd29(0x8cac8636d9f1c104024f7cc54c81391001bceded51e3038dab7ab2ac15b8131c); /* function */ 

coverage_0xe2a4dd29(0xefd91a52ecd8cf6e268a046fcf243e2ee2111750fc61f23071b368d2c692d66e); /* line */ 
        coverage_0xe2a4dd29(0x945f6d10a8d273bf19c3a3140e1a299fab9f75dd7862c76e34663987e5eff32f); /* statement */ 
return state.accounts[account.owner][account.number].status;
    }

    function getPar(
        Storage.State storage state,
        Account.Info memory account,
        uint256 marketId
    )
        internal
        view
        returns (Types.Par memory)
    {coverage_0xe2a4dd29(0x0019ba80551b80d8a753fe754ba0cdf94bebec2986935b5a7b10748de37b63e8); /* function */ 

coverage_0xe2a4dd29(0xc49157e5afab04408365fcb039e94ea4ec1b821aa24b5020a1395ee4b8d1d17e); /* line */ 
        coverage_0xe2a4dd29(0x0c4f132ece2f5fae7252ca0afb8c1545bd57c861e359c6db8ec352665aee452a); /* statement */ 
return state.accounts[account.owner][account.number].balances[marketId];
    }

    function getWei(
        Storage.State storage state,
        Account.Info memory account,
        uint256 marketId
    )
        internal
        view
        returns (Types.Wei memory)
    {coverage_0xe2a4dd29(0x03c92e5ff8b9a91599326bf1c2a764b9d2c8a02830fc5ba9cb03e58912fa6d32); /* function */ 

coverage_0xe2a4dd29(0x40e7516cb347c2eb74bd67a614642d2b8e9e20eee28f81344e6053537834dc71); /* line */ 
        coverage_0xe2a4dd29(0xa76417178e05c29ed77478076b6432a53c917cd42358b2fa0996b99c69886ecc); /* statement */ 
Types.Par memory par = state.getPar(account, marketId);

coverage_0xe2a4dd29(0x85313b87e08f52c9f34489754d826fc0a0529b7e9077fba4d88255bbc4dc4291); /* line */ 
        coverage_0xe2a4dd29(0x33cac6db1d5c34353d043b8c147bd9a6f2cc3066cb0abdb03e1567bb705e8a4c); /* statement */ 
if (par.isZero()) {coverage_0xe2a4dd29(0xe263d07df019850656e3c0c54ae7f9846973807ea3651fc26d79aa8f19ab8d6b); /* branch */ 

coverage_0xe2a4dd29(0x240205139570be1e52eb8a034051a45d039201faefa1323f6ad684d9d68bd769); /* line */ 
            coverage_0xe2a4dd29(0x7d1929639875308195ee6991caf5eaffc7058beb46ed2bacb0257d7140f9d0ae); /* statement */ 
return Types.zeroWei();
        }else { coverage_0xe2a4dd29(0xf689171a9cb4b49e1c5ccdfb6dfaaebe22958cc11c3fd2d8743807c18febcf97); /* branch */ 
}

coverage_0xe2a4dd29(0x24f62a9cfd5b998023469abae79142f3e0935b4359a754dd1846c0b21387249c); /* line */ 
        coverage_0xe2a4dd29(0xe9c5c127fa8e39fcff31b6258c90b703d21c4aa570676e36e20c198f82cbbd1e); /* statement */ 
Interest.Index memory index = state.getIndex(marketId);
coverage_0xe2a4dd29(0xf470dc1edb3cbb117dadcdfe784b5b95a2993c636182d7fa2a969112e7565845); /* line */ 
        coverage_0xe2a4dd29(0xb6ceb44dd5ce18b3bacd050541373b212efa59632e31dffe230a699954b465ab); /* statement */ 
return Interest.parToWei(par, index);
    }

    function getLiquidationSpreadForPair(
        Storage.State storage state,
        uint256 heldMarketId,
        uint256 owedMarketId
    )
        internal
        view
        returns (Decimal.D256 memory)
    {coverage_0xe2a4dd29(0x434999d9d0cfb8ba83f70bbbf8fba3f074e179ebfa03c5274f6f0be62530f4e2); /* function */ 

coverage_0xe2a4dd29(0xc33d03f2c44258a48ef3568cbaa08b1130a2a93a672da8befc87972db0f5dc2c); /* line */ 
        coverage_0xe2a4dd29(0x6532733b6d2f683cf1ddfb8fc94ee2c2c423af2f86f23ad4a9387aeb7d29ffc8); /* statement */ 
uint256 result = state.riskParams.liquidationSpread.value;
coverage_0xe2a4dd29(0xce84daacfa9ecf687fe759fc2ec19f8962bdcf35d75a75a916116b2e8a51a8b7); /* line */ 
        coverage_0xe2a4dd29(0x0e4906e8ba62fe4c45302a2f1375ef4f0add698d778dffa367e951264cefd471); /* statement */ 
result = Decimal.mul(result, Decimal.onePlus(state.markets[heldMarketId].spreadPremium));
coverage_0xe2a4dd29(0x4feb29d0f83e065583325f3e9feeba3b8bfa6e840957ae3ccbf87c2c867de7fe); /* line */ 
        coverage_0xe2a4dd29(0xd5b83aab499dc983e8bacf167ab5add35e2ef4a3681be184bdc16b36830a44b2); /* statement */ 
result = Decimal.mul(result, Decimal.onePlus(state.markets[owedMarketId].spreadPremium));
coverage_0xe2a4dd29(0xb7e4ce96bb882a21da88aac0198cb1275a693d86be025c27e680d41a3e0b96aa); /* line */ 
        coverage_0xe2a4dd29(0x0c116d33f3b423f15e6cc3691b91d5a0640ac9a6511dc10436af948b73ae2647); /* statement */ 
return Decimal.D256({
            value: result
        });
    }

    function fetchNewIndex(
        Storage.State storage state,
        uint256 marketId,
        Interest.Index memory index
    )
        internal
        view
        returns (Interest.Index memory)
    {coverage_0xe2a4dd29(0xfed557636f65368f32c9e57199271251eab59fcf43f4e64ba46d32a912f5ad45); /* function */ 

coverage_0xe2a4dd29(0x257dd1634b13ad62da66570dbb5d2fc4895ad2da3484cd05c1557bacdd989571); /* line */ 
        coverage_0xe2a4dd29(0x4eb38b89bf924b65b44cb8cadfcec93bfc9ae771f5c38a86e11b42ad35604c9a); /* statement */ 
Interest.Rate memory rate = state.fetchInterestRate(marketId, index);

coverage_0xe2a4dd29(0x0aa4a4e329556c818b0578887753138ba6d36703b7894123b0bd4927d7b9c260); /* line */ 
        coverage_0xe2a4dd29(0xe8279248c454aad70a51914b4d552922b76624a46290ae89d08554ca091c37f5); /* statement */ 
return Interest.calculateNewIndex(
            index,
            rate,
            state.getTotalPar(marketId),
            state.riskParams.earningsRate
        );
    }

    function fetchInterestRate(
        Storage.State storage state,
        uint256 marketId,
        Interest.Index memory index
    )
        internal
        view
        returns (Interest.Rate memory)
    {coverage_0xe2a4dd29(0x92822ca6282abeb453e329ae0947b692bccddb9ff49b86e8e685caebb92dc241); /* function */ 

coverage_0xe2a4dd29(0x7abd9a5b0e34a8e1cbe3198e61d9fb85a9fedb4b3b7e81cc474ab2db3eedeb5b); /* line */ 
        coverage_0xe2a4dd29(0x3ada25d5c8e3d9ecb3f58e4806b2d297c5372db4884823e3c256538e788f49c8); /* statement */ 
Types.TotalPar memory totalPar = state.getTotalPar(marketId);
coverage_0xe2a4dd29(0xc805da8c62068aea7daf191f07aedaf6756871e992c69fa438a8977d774b7abc); /* line */ 
        coverage_0xe2a4dd29(0x0d46e62055d273c86761e321ca16259757321d85c7aab06a9567deb66963942a); /* statement */ 
(
            Types.Wei memory supplyWei,
            Types.Wei memory borrowWei
        ) = Interest.totalParToWei(totalPar, index);

coverage_0xe2a4dd29(0xe633cbb9a809cc7a97edb05fc4a6f0790b8afafbc4a06f51d8ca06bc8b9356f1); /* line */ 
        coverage_0xe2a4dd29(0xdd6bd6147e81c431175821d27c2bd28c0680640504af5e373f7e2c3108eb8200); /* statement */ 
Interest.Rate memory rate = state.markets[marketId].interestSetter.getInterestRate(
            state.getToken(marketId),
            borrowWei.value,
            supplyWei.value
        );

coverage_0xe2a4dd29(0x1d67805f2587c5369344e4c2d4cd4fabcc61463562c3305e6da55f70bb142241); /* line */ 
        coverage_0xe2a4dd29(0x98773b5233f3b3fca0b5dcaaea0b670a256466b5d8eb9a5b190c709b7915eb9f); /* statement */ 
return rate;
    }

    function fetchPrice(
        Storage.State storage state,
        uint256 marketId
    )
        internal
        view
        returns (Monetary.Price memory)
    {coverage_0xe2a4dd29(0xed8314b2237df624ffe7669d01637c9e63abafaafe3b230c2195d8a3ba32eebc); /* function */ 

coverage_0xe2a4dd29(0x7feafcee46c692e722bc0c6f9b6fe9af6601145143eeaa5432d675123a5829e8); /* line */ 
        coverage_0xe2a4dd29(0xb1e03e4d11672251f8bc109e8d53b215e541bb34323a64e8516784fb576be9ec); /* statement */ 
IPriceOracle oracle = IPriceOracle(state.markets[marketId].priceOracle);
coverage_0xe2a4dd29(0x93a3d6abe314c827d9ec29fa2603fbfe7e36fbf067252b8d0c792e2beca27295); /* line */ 
        coverage_0xe2a4dd29(0x47ad3bad05280b26c762ce3cd6937df4150dc65ebbc50d410b435e3c1ab7546a); /* statement */ 
Monetary.Price memory price = oracle.getPrice(state.getToken(marketId));
coverage_0xe2a4dd29(0xc6dd24ad690b33e0aedd63071731458a926b52156e9035a73b1a7daf07ed820a); /* line */ 
        coverage_0xe2a4dd29(0xb9452be50cc58e2ddd594ec58063c0c0bf240a24e0d4043a9a4653d266950c1a); /* statement */ 
Require.that(
            price.value != 0,
            FILE,
            "Price cannot be zero",
            marketId
        );
coverage_0xe2a4dd29(0x27b823e4e5b33e45861d4ffba47eae062cd7fed6310caed3b0c8702799e5ba85); /* line */ 
        coverage_0xe2a4dd29(0x3d59ed71759bc13ef0cf40173ffb52e7e33a5bfcff3a6535756e31ce1d5aa02d); /* statement */ 
return price;
    }

    function getAccountValues(
        Storage.State storage state,
        Account.Info memory account,
        Cache.MarketCache memory cache,
        bool adjustForLiquidity
    )
        internal
        view
        returns (Monetary.Value memory, Monetary.Value memory)
    {coverage_0xe2a4dd29(0x0fa58d8ace1a9795fa5a91234216b1b15b74612457ba6585af059de10e454130); /* function */ 

coverage_0xe2a4dd29(0xffbbd99bde8f70a9a7f4d586e835415aeea44e088997a15ddc7cd9de5b420d76); /* line */ 
        coverage_0xe2a4dd29(0x773ef6cdf5010db7472dc94c45f91a317b8880d7f1e66a716f1c8a0f89c11456); /* statement */ 
Monetary.Value memory supplyValue;
coverage_0xe2a4dd29(0xfc264b9d2f9f6da09ed05d3dd2f9ee1cf9aa0928a4245a53fc8bad502ea4ab07); /* line */ 
        coverage_0xe2a4dd29(0x2f8178d9d2c9b72c4eef455416d62db64352ebc7658fa9cccab6fde46e418998); /* statement */ 
Monetary.Value memory borrowValue;

coverage_0xe2a4dd29(0xa9e00c70460e6d0cdad1605494f55a06a2b6a47a05b75fe20576b1518b0baf19); /* line */ 
        coverage_0xe2a4dd29(0x9b40307e08ee136e2235303d1b8d2152dfb326262ae51da14e67c5d0cf56fb53); /* statement */ 
uint256 numMarkets = cache.getNumMarkets();
coverage_0xe2a4dd29(0xb877421b37d8bc2c4f3189c71d2141e77b80fe64fc098ce4ac86e2e6654538ee); /* line */ 
        coverage_0xe2a4dd29(0x79cdcbbc942a3514a0a1e58667cdda962d4255448f8072ecbb9cd3938aecc430); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0xe2a4dd29(0xfbec597cdca757c40445fa1e0c2ad4ed7a3158a5b40a6c992dbed81131656458); /* line */ 
            coverage_0xe2a4dd29(0x55f1bca6ee578bc69ad10a0118ed7fcaff34516527522aec085865769722b454); /* statement */ 
if (!cache.hasMarket(m)) {coverage_0xe2a4dd29(0x41c777d8a5e31c28917620ae95a59269c773a3e58dcc7b8eb48ffd1f809ae483); /* branch */ 

coverage_0xe2a4dd29(0x6f3840f70b3b0569bcae3f44363789d22ba19d7741708a1dd236b348f6c9afa3); /* line */ 
                continue;
            }else { coverage_0xe2a4dd29(0xa6c13d2e15e05b5efb587ea708ec1de1db8a54ca687be9ae71102d1e1a42cb74); /* branch */ 
}

coverage_0xe2a4dd29(0x0282dd77961ab012bb3138dd9e312a4ec871579daa68980e1f585055043dbc45); /* line */ 
            coverage_0xe2a4dd29(0x7d50bf3d50873f1d5fcd796f1514c9439ab66f82c81ccff5ab958813951825b5); /* statement */ 
Types.Wei memory userWei = state.getWei(account, m);

coverage_0xe2a4dd29(0xbe95527bfaec20b6a0a2c4947afd31f4eda1c677ab4bbaed5a2c4acc44563f0e); /* line */ 
            coverage_0xe2a4dd29(0x86f4fa3155ecd51da5f0efe20251d089f404c377f5fc617c8e016562642140ab); /* statement */ 
if (userWei.isZero()) {coverage_0xe2a4dd29(0xc2862180024609b9737dfbb833300b08b1fb06ac338dae28adb9d0293048ad5c); /* branch */ 

coverage_0xe2a4dd29(0xcaa62430aec1e8a62aa271ace115a1908b686e1879ab039012345b8f43abe0ec); /* line */ 
                continue;
            }else { coverage_0xe2a4dd29(0xa15adfe18f249a13f7dc4a37655a154a6d82fe60d7e742f5d0437d39847c1e45); /* branch */ 
}

coverage_0xe2a4dd29(0x1ae9fcc1a37ccb154a23677a20152b3430bd718dd271dc668369528387a71c41); /* line */ 
            coverage_0xe2a4dd29(0xe0e6e334787ce6417d513262ab3666562182a32a231a2fa06375a1b991d71b22); /* statement */ 
uint256 assetValue = userWei.value.mul(cache.getPrice(m).value);
coverage_0xe2a4dd29(0x607dfa636a77eea923b4bf002604c3d24d898a48e5c662b6cda1e47d834391b8); /* line */ 
            coverage_0xe2a4dd29(0xbcf901983d84d3315b8931911d95a5cba000619fb7a7b8c9a515b0d1565760b7); /* statement */ 
Decimal.D256 memory adjust = Decimal.one();
coverage_0xe2a4dd29(0x8c4edbdaae43cb724122384117167734ee23546a3c90f9d9b41d7fdb2371a647); /* line */ 
            coverage_0xe2a4dd29(0xdfe310afaa507f79118a966398172ee46413a8cd24d29b6097960d79555b82b2); /* statement */ 
if (adjustForLiquidity) {coverage_0xe2a4dd29(0x08bd4f1426100df6a59b24ccd80ba2a7df4f3e14e01ca9539ad9185f8ef457a3); /* branch */ 

coverage_0xe2a4dd29(0xbc722dbe5b7a966538c486a4e0c42310177eefbc6fa425ec3988c5157a497a66); /* line */ 
                coverage_0xe2a4dd29(0x97d5cac1682f33d48be6fceaf91ecb1228b0820fb8a5ffba298c9034af4917d2); /* statement */ 
adjust = Decimal.onePlus(state.markets[m].marginPremium);
            }else { coverage_0xe2a4dd29(0x9bb05796e2618abbd6b228fbab431d0aa77d8d0d8de93fdbe235474626746e8d); /* branch */ 
}

coverage_0xe2a4dd29(0x9540c91935724533d748e7582e0f53bd79f2756bf6dfad04626f9ba98ea4b308); /* line */ 
            coverage_0xe2a4dd29(0x384fbd6533e12ae9b80df0afabd597234d8a8bb36d0eeb43507362a53eb206b6); /* statement */ 
if (userWei.sign) {coverage_0xe2a4dd29(0x1cb8f5df2acafa23ff2099f295d152f95b1f9d7db2e6794d1382d1eccad9cc1f); /* branch */ 

coverage_0xe2a4dd29(0xf05bff96a2f48132af7bad64348613f0ef2cf3ea9e6d02070c5ff4503499e9ae); /* line */ 
                coverage_0xe2a4dd29(0xcebfbd0238265e2f13d8eea1b04a1621d4ec040d6f4ea846a83dc2483f82fbc8); /* statement */ 
supplyValue.value = supplyValue.value.add(Decimal.div(assetValue, adjust));
            } else {coverage_0xe2a4dd29(0x3171d3624964a5bea6902613720b000168ddf17709411322591b16d9dca90648); /* branch */ 

coverage_0xe2a4dd29(0x62fa5b85eb20ea3c8cc3992606599670c60ddb463398757b3abc0574b638e23e); /* line */ 
                coverage_0xe2a4dd29(0xd6bb64661ad67bb5fbc2b1437e3e5fd1f3965e7c1fe8b5971ad7f52c55fc0f1a); /* statement */ 
borrowValue.value = borrowValue.value.add(Decimal.mul(assetValue, adjust));
            }
        }

coverage_0xe2a4dd29(0x0f2bd8f031898ffe23e424a4b558d845c4d9744fe1a845e8cc9e72112dd846ba); /* line */ 
        coverage_0xe2a4dd29(0x3e388a2a13665bd6e12fe46bf93cb1828a33f5290e1e94cfb6a8ea7d19150fe0); /* statement */ 
return (supplyValue, borrowValue);
    }

    function isCollateralized(
        Storage.State storage state,
        Account.Info memory account,
        Cache.MarketCache memory cache,
        bool requireMinBorrow
    )
        internal
        view
        returns (bool)
    {coverage_0xe2a4dd29(0x6092442841ff0ed039914a2d5892a87bf436e0bbf695a2972a127597ff514c0b); /* function */ 

        // get account values (adjusted for liquidity)
coverage_0xe2a4dd29(0xecca931d431597ef10e2406cea35994c1906122a077b9a71c14359c6355f9d94); /* line */ 
        coverage_0xe2a4dd29(0xe7ae68305d59a9f0710cad6ca2375291f2b33597102c2698b56153210ebad1d5); /* statement */ 
(
            Monetary.Value memory supplyValue,
            Monetary.Value memory borrowValue
        ) = state.getAccountValues(account, cache, /* adjustForLiquidity = */ true);

coverage_0xe2a4dd29(0x6a11f4aeb4098540b1d414f78db3c3fa4b280aa362815def6d734cba15da27c7); /* line */ 
        coverage_0xe2a4dd29(0x7df8b9eec17b226e752a0d08b2a76b1e3911887a84baef07ed76b65c462e4a9f); /* statement */ 
if (borrowValue.value == 0) {coverage_0xe2a4dd29(0xc55dfea4d3c3d7f029bb02342686603b535e1f434bdaf1ce0ab3fa5b853f44ae); /* branch */ 

coverage_0xe2a4dd29(0x6745fdd336b50c7d7813602ddf17f86e6174131a54b9978cfe157173e982ddc8); /* line */ 
            coverage_0xe2a4dd29(0xd72013b302af61651a097e74550b1c685c329151adcaa960e796bb9cdf5980a3); /* statement */ 
return true;
        }else { coverage_0xe2a4dd29(0xa18cb848b86e6f87ce2a7f3d1e97bc7b97648638a6423729d852bde22f474ca4); /* branch */ 
}

coverage_0xe2a4dd29(0x257aae41bc1ad8cc8177404490674b9f072b1c8706030383ef3c3caf65c17722); /* line */ 
        coverage_0xe2a4dd29(0x9c22c1878040d4c9be1626ed6d60dce719d96b44ce9079b5a921f9df565a5ba4); /* statement */ 
if (requireMinBorrow) {coverage_0xe2a4dd29(0x0200f619c935287020a2697eef69c4ac385459055576a1da4e250add48bff366); /* branch */ 

coverage_0xe2a4dd29(0xb4c63842e9228a9026ce47b959fbc39d3d8c37aaf6a33de8da9bb2939dcd51c4); /* line */ 
            coverage_0xe2a4dd29(0x078e65e28072592eb545954e13bb401a5a4bc7caccfe094cc1319bff3820da9c); /* statement */ 
Require.that(
                borrowValue.value >= state.riskParams.minBorrowedValue.value,
                FILE,
                "Borrow value too low",
                account.owner,
                account.number,
                borrowValue.value
            );
        }else { coverage_0xe2a4dd29(0xda1301344fa3daa73ff6a3f39a765d8f405d2585aaa3909df588a8e01015acaf); /* branch */ 
}

coverage_0xe2a4dd29(0x360779c2fa0f437b2286cdc72d6466c07f77de7a55b0a33351698bad59112523); /* line */ 
        coverage_0xe2a4dd29(0x68fa670311dfa4f5c9a87dd3c7de571ac3a2c0743c645ac42ba7618585496e3c); /* statement */ 
uint256 requiredMargin = Decimal.mul(borrowValue.value, state.riskParams.marginRatio);

coverage_0xe2a4dd29(0x573054415d9148201bf065a12e792add0340c268856ce41ca475b849057e2298); /* line */ 
        coverage_0xe2a4dd29(0xb1575dbf11bd4f6684be837472ce7115e3b7716b92c6b825764370d3855ca82d); /* statement */ 
return supplyValue.value >= borrowValue.value.add(requiredMargin);
    }

    function isGlobalOperator(
        Storage.State storage state,
        address operator
    )
        internal
        view
        returns (bool)
    {coverage_0xe2a4dd29(0xa814f4dbffb4ee3a410931be3f7bc654a46de49d45c06b5213e74db72b6c4423); /* function */ 

coverage_0xe2a4dd29(0x3c494566b1f30622c2f1fbe8772fd74c0d09c4c5184055c13fe6d8e629498a0c); /* line */ 
        coverage_0xe2a4dd29(0xfb69dc1f0e027d76a17aff30b213cefb2f70061b567341c32cc07df5e5e22257); /* statement */ 
return state.globalOperators[operator];
    }

    function isLocalOperator(
        Storage.State storage state,
        address owner,
        address operator
    )
        internal
        view
        returns (bool)
    {coverage_0xe2a4dd29(0xa453c3df638c71124fdf3703feab2947d4ebe4e8b8731bc7e4019b1db804b1b0); /* function */ 

coverage_0xe2a4dd29(0x489dab5a228dd3781005c2bf427b5fcc66b2964c705698bac29779442be9d78f); /* line */ 
        coverage_0xe2a4dd29(0xd100b9500d38340475fdf0cead42c20df87b753feb08388d9a6d58460e0d3ab1); /* statement */ 
return state.operators[owner][operator];
    }

    function requireIsOperator(
        Storage.State storage state,
        Account.Info memory account,
        address operator
    )
        internal
        view
    {coverage_0xe2a4dd29(0x44e2cb88ac53159cad4332c91720621ab9a24a9eae0abca132e2bf6bff2b04e5); /* function */ 

coverage_0xe2a4dd29(0x28d704e8bd8ead3e0646973425542c6ac98ad903104509dd430ad7faac8c4e4d); /* line */ 
        coverage_0xe2a4dd29(0x03cb856b2e6f82bef9e1b533de4a4e8265b39b63b7672bacdc26e32ff31bba62); /* statement */ 
bool isValidOperator =
            operator == account.owner
            || state.isGlobalOperator(operator)
            || state.isLocalOperator(account.owner, operator);

coverage_0xe2a4dd29(0x0831c5990f1a038513dff64cbb389939d0c2325a23ad13255c8633bef4fada9d); /* line */ 
        coverage_0xe2a4dd29(0x0885f78b7d29961483f0cc186c948559768206aa967c8db676577df108e05df4); /* statement */ 
Require.that(
            isValidOperator,
            FILE,
            "Unpermissioned operator",
            operator
        );
    }

    /**
     * Determine and set an account's balance based on the intended balance change. Return the
     * equivalent amount in wei
     */
    function getNewParAndDeltaWei(
        Storage.State storage state,
        Account.Info memory account,
        uint256 marketId,
        Types.AssetAmount memory amount
    )
        internal
        view
        returns (Types.Par memory, Types.Wei memory)
    {coverage_0xe2a4dd29(0x88065974f095d2817ab5e4e3dca0149bb2a28bb0a2bfe37c8857da2280330ada); /* function */ 

coverage_0xe2a4dd29(0x166bc314a3a01712d81a2871d0030186242b65051aeaefcb2071acc8fdf23f46); /* line */ 
        coverage_0xe2a4dd29(0x90389ce0a8a6419229b7dbf8839a4c9056dfac11f548756384acc6e3829a9118); /* statement */ 
Types.Par memory oldPar = state.getPar(account, marketId);

coverage_0xe2a4dd29(0x2bee5192cef322021620d808fc1d7b4d086917d2ca788d0732f523ded2125985); /* line */ 
        coverage_0xe2a4dd29(0xd2cdd96cfa6815961347ed68c15e71e1bd1e4e903f61d23e2bb97652748cd5c6); /* statement */ 
if (amount.value == 0 && amount.ref == Types.AssetReference.Delta) {coverage_0xe2a4dd29(0x67765405b4e0c04d25932b875d238e352efa65230ff9e8bfdc59d16aece4af7c); /* branch */ 

coverage_0xe2a4dd29(0x0b08b547a16e43ecce820c496c59c64c93e49fa012336e5ad46cbebf3a0fe28b); /* line */ 
            coverage_0xe2a4dd29(0xe08bafc1a8a096a13d6775a275acd9cd56150217170ec6d2f9201d0d977865ff); /* statement */ 
return (oldPar, Types.zeroWei());
        }else { coverage_0xe2a4dd29(0x09025ee2876d22025146a2d9e663720f402b02dd125a772c943c7e40022dddde); /* branch */ 
}

coverage_0xe2a4dd29(0x81f71692aaeed57c79f01b979b89c56dffb813a53910d7ecc88034619732c188); /* line */ 
        coverage_0xe2a4dd29(0xd9a611d3de2ab43f3f5489731ae10db88891a45f300dc8d0cdca279af279aa7b); /* statement */ 
Interest.Index memory index = state.getIndex(marketId);
coverage_0xe2a4dd29(0x646bca6e0ef07bff54563d8a364a4155cb97e2c1a8beeb4c0b002592d3f54982); /* line */ 
        coverage_0xe2a4dd29(0x663bb3e0725642831c0b9bc7806b8e11ab497da8dbe73d9c773f5c89208380f2); /* statement */ 
Types.Wei memory oldWei = Interest.parToWei(oldPar, index);
coverage_0xe2a4dd29(0x4b4391d995326eb384fc0dccea1cd07bffd0e3064c5c59ca259d505d3f3dc8ea); /* line */ 
        coverage_0xe2a4dd29(0xcd7a7163b49ce3943ddfabaf5c3f10a9d27b64930c91b7a5f05c114b574bd00e); /* statement */ 
Types.Par memory newPar;
coverage_0xe2a4dd29(0xc39c816d36bc8e39e3b2804295de1b04b45a7fe5d5d643791f21c3002103df6b); /* line */ 
        coverage_0xe2a4dd29(0x5b49dc6472d32b2128528b70803e775cf2b353e1e741346540b4c0baedba0224); /* statement */ 
Types.Wei memory deltaWei;

coverage_0xe2a4dd29(0x7f48b3243a8de53fa67f972d84efac7804719ad83bd2687683064489ef0bd080); /* line */ 
        coverage_0xe2a4dd29(0x05d0c6a4f8fdb871620212484088c96560f834fe56900eb84d684652bb882844); /* statement */ 
if (amount.denomination == Types.AssetDenomination.Wei) {coverage_0xe2a4dd29(0x7f8b83be3ea0691f61889fc095b0c233e4f93051bcdfceecb018bee5d8c7414b); /* branch */ 

coverage_0xe2a4dd29(0xbfde0e764373fd9221f57dce72faa6b61052b3e24f1f38eae6229bd6fc5a53fc); /* line */ 
            coverage_0xe2a4dd29(0x13044a83a7acaaed38b464833f7e17ae1cb945b3a18b027466b46154803ae133); /* statement */ 
deltaWei = Types.Wei({
                sign: amount.sign,
                value: amount.value
            });
coverage_0xe2a4dd29(0x4dbcdb2ec0a5452bf68446f69f1e62731bd2a3247e6609a8244d6a58b00dc7ff); /* line */ 
            coverage_0xe2a4dd29(0xd5b38014d6eea0540516e481a7f781863ae2c0a580dfcd79208dc5fb4d529f21); /* statement */ 
if (amount.ref == Types.AssetReference.Target) {coverage_0xe2a4dd29(0x383f2b78ab56961251520c9bb94950478e9038246f675fd8008548033f3d9ddd); /* branch */ 

coverage_0xe2a4dd29(0x58c1aedc1e71a69db86e165d62b86f28cb85586feb174248038d188bbd5d6426); /* line */ 
                coverage_0xe2a4dd29(0x6f466323f930c8d755c2014a79499c11a6c3927c33af2cf58a1c3e47f6de5ef0); /* statement */ 
deltaWei = deltaWei.sub(oldWei);
            }else { coverage_0xe2a4dd29(0x73507088c27c78ba762f8305f7e62a6a67fac5bb287c284c40ec5d311c01856a); /* branch */ 
}
coverage_0xe2a4dd29(0x28c84a3c816314a0fc8bdc06b21ec42b732982e85bb7759a94d8e65f02564591); /* line */ 
            coverage_0xe2a4dd29(0xe48de1a4067d9d180ae7b666dcde1e8b837ded23a93e80eb5a1cd5bc12aaa40e); /* statement */ 
newPar = Interest.weiToPar(oldWei.add(deltaWei), index);
        } else {coverage_0xe2a4dd29(0x8561e6021923cdd2cd1c8d71ba16d6a7526db4f84094e72c5dcea156e5284fc0); /* branch */ 
 // AssetDenomination.Par
coverage_0xe2a4dd29(0x32c38172f3d4de113959e8e3f477f3f5b99d6bd37278abfbc3100c2d94cd8fe2); /* line */ 
            coverage_0xe2a4dd29(0xf254460e04ec91839c732142309c53c45b465893f7585a6b6a0dd0f1b12bb72f); /* statement */ 
newPar = Types.Par({
                sign: amount.sign,
                value: amount.value.to128()
            });
coverage_0xe2a4dd29(0x96e260089c6327a0d6e8cfcd1d4c2238324c28bf74f2f235cf05227168ab55fc); /* line */ 
            coverage_0xe2a4dd29(0x250820af2e07b0f4c537878fc7ecf0662545c9434ecd096d38caae408d9b34a0); /* statement */ 
if (amount.ref == Types.AssetReference.Delta) {coverage_0xe2a4dd29(0xcf17d46392ede3c6daf9dd46c9f9b282f3356a15a49e4d3be874053c7be4f71b); /* branch */ 

coverage_0xe2a4dd29(0x060964f3c14509a2d081c67dbd1424cf7aed84310345eed013d0c1d1b48646f7); /* line */ 
                coverage_0xe2a4dd29(0xcd7fd5148731bc02f7d9170d512964bb9764a4aa20402f10ec193fbe6a19045b); /* statement */ 
newPar = oldPar.add(newPar);
            }else { coverage_0xe2a4dd29(0x0ce04407429aba5f76b79981d3feebbee776623ecd0088ff237a3686b519f0f2); /* branch */ 
}
coverage_0xe2a4dd29(0xcd4e5519b7942166932d5f8240e5f17c1e6df8bf16a1231627c826b4081da5d2); /* line */ 
            coverage_0xe2a4dd29(0x5a71f424f9080cd41b1e33c53c4c71c5a7ac388445a9a8f1b51753dbc644aee2); /* statement */ 
deltaWei = Interest.parToWei(newPar, index).sub(oldWei);
        }

coverage_0xe2a4dd29(0xaa430fa24cb27127f11a60a05fb2d03522089be3f63463c48a6ef9280368a8f8); /* line */ 
        coverage_0xe2a4dd29(0x83416c205a8a1745377de88655c5b8b92149239e74732277b84b56448b5f62be); /* statement */ 
return (newPar, deltaWei);
    }

    function getNewParAndDeltaWeiForLiquidation(
        Storage.State storage state,
        Account.Info memory account,
        uint256 marketId,
        Types.AssetAmount memory amount
    )
        internal
        view
        returns (Types.Par memory, Types.Wei memory)
    {coverage_0xe2a4dd29(0x053f436f366bff5df2a5165bf3e7b714b0685f83f9e8e085129282f77a5ed181); /* function */ 

coverage_0xe2a4dd29(0xd7d2ac68362c60761a8daa434867cc1b537de36f99697820103ecf6b13e8f3ee); /* line */ 
        coverage_0xe2a4dd29(0xde0112ee88816cf5bed7663e5031810657a4d6b6f6ec96a529e00b244afe360f); /* statement */ 
Types.Par memory oldPar = state.getPar(account, marketId);

coverage_0xe2a4dd29(0xc83fea2d69046d6bb928778432113cd1d5fbea027bc9c7f0d254ea8d11d4451f); /* line */ 
        coverage_0xe2a4dd29(0xd512e88c2343518dc2f4d18807bcafaf789aa85553aa16bf307c01a39c7a6e58); /* statement */ 
Require.that(
            !oldPar.isPositive(),
            FILE,
            "Owed balance cannot be positive",
            account.owner,
            account.number,
            marketId
        );

coverage_0xe2a4dd29(0x98c3966f31256b4a1ceb927667691986c160a1d531146cc2bb9bb43c8f23de33); /* line */ 
        coverage_0xe2a4dd29(0x6f0bfd581714263b6e32dd9df0d36a2edab1aa1e902292e9240cd5166ac0313e); /* statement */ 
(
            Types.Par memory newPar,
            Types.Wei memory deltaWei
        ) = state.getNewParAndDeltaWei(
            account,
            marketId,
            amount
        );

        // if attempting to over-repay the owed asset, bound it by the maximum
coverage_0xe2a4dd29(0x947ab570b091cedd3b3f6afd2bdf32726c0b9ddd08264fcf10afea2e288523b6); /* line */ 
        coverage_0xe2a4dd29(0x3bc8da702b86bd7768d1f1a3986be121fbdb87f1c186b8575be1a6fc18f4821a); /* statement */ 
if (newPar.isPositive()) {coverage_0xe2a4dd29(0xc7cc0fbd7fe4c8be3f71be86e2bc63d33e8e1677248466b8d9515f0af02dc478); /* branch */ 

coverage_0xe2a4dd29(0xdcd8129fecfb846beba11f413609f1666b82055b4191f49374712d519d45df1e); /* line */ 
            coverage_0xe2a4dd29(0xfde2c96c5e59f4efb7f5d3c3e523c144f7d691d7ba0b0882ec38ae4992c85c1a); /* statement */ 
newPar = Types.zeroPar();
coverage_0xe2a4dd29(0x8e89d1e9abb61e8da0181019760072f714c5aa6ea5816d0d1fe4366ab50d3210); /* line */ 
            coverage_0xe2a4dd29(0x2f8f7fa50d5be31ca3fb09aba0f2660c955b15265bf1c2436139440ec8802dc1); /* statement */ 
deltaWei = state.getWei(account, marketId).negative();
        }else { coverage_0xe2a4dd29(0x750ebacbe14725f108bd9989b31e94068b4f78ccf59c366474654b3c12a5a6e1); /* branch */ 
}

coverage_0xe2a4dd29(0x9072a2019a027c2c12e4c343417a67fa2a18eb9e2dd55fb4efff86990d1d73cf); /* line */ 
        coverage_0xe2a4dd29(0xceb6864e328e909c72fefcab6dbcd12e97bacf2103d58a24e51a8b3f5e55be95); /* statement */ 
Require.that(
            !deltaWei.isNegative() && oldPar.value >= newPar.value,
            FILE,
            "Owed balance cannot increase",
            account.owner,
            account.number,
            marketId
        );

        // if not paying back enough wei to repay any par, then bound wei to zero
coverage_0xe2a4dd29(0x46297aff02b41788cd2ef44cbc9f7dfa4df8bbfc89801fad679215d5154c8280); /* line */ 
        coverage_0xe2a4dd29(0xaa9a4c419bcd6bc20600a2275c442d3fe6257ea9a1756ce4eb0107dd0bbc51ce); /* statement */ 
if (oldPar.equals(newPar)) {coverage_0xe2a4dd29(0xbd9df59fd8912c85044d80c24ea9aebaf96da0808227d74d32f5a1d2dce386af); /* branch */ 

coverage_0xe2a4dd29(0xabfa61ab83f7849b524cefaff402e8e8b7cdec35f2b6077ff1d31ff0a5066885); /* line */ 
            coverage_0xe2a4dd29(0x0ce8b077d4d22c840b6c99019028ca109e5ff0c72f39bc0a82b77d3de77e16bf); /* statement */ 
deltaWei = Types.zeroWei();
        }else { coverage_0xe2a4dd29(0x3bfd538102e216bf72d20cca3a4972e098990fb2da6c6ff0ad9f38ff2aea2469); /* branch */ 
}

coverage_0xe2a4dd29(0x495a32a4d5a0f597d9e58fbe15fad376065587189f88157bdcea3e12aa510d45); /* line */ 
        coverage_0xe2a4dd29(0x34031b5247f21d939518b16e60253881ab438f5dfcbc4b7fe0c1bef8d9ecef15); /* statement */ 
return (newPar, deltaWei);
    }

    function isVaporizable(
        Storage.State storage state,
        Account.Info memory account,
        Cache.MarketCache memory cache
    )
        internal
        view
        returns (bool)
    {coverage_0xe2a4dd29(0xbfbc56539985fa36471a06eaea5261ece0206cf0966401d06208c565deb3c10b); /* function */ 

coverage_0xe2a4dd29(0xc1c1c95aa6b4adf8828c766d4291c8fe3382a37f9f7168e5b57ab8797680c547); /* line */ 
        coverage_0xe2a4dd29(0x98007e70487420b9b2feab16ac8306113b216072b3f283deaf6e869333bfb64e); /* statement */ 
bool hasNegative = false;
coverage_0xe2a4dd29(0x29e5fe5d50d5cd4758caf6ebde2f5f06df80a7ace1683f1a835c519301f8bced); /* line */ 
        coverage_0xe2a4dd29(0x8f6d8ca77477d075e0ed086a12aa571d02e3444f956bb1c76d8276714f35a08c); /* statement */ 
uint256 numMarkets = cache.getNumMarkets();
coverage_0xe2a4dd29(0x658512476d97af0896ff59fe07fffec090908254afb778c76ab279628cbb1599); /* line */ 
        coverage_0xe2a4dd29(0x6d957593a3eea385afe9d056be593691fc9455e239b1a2a59f8cc2905947fb7f); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0xe2a4dd29(0x6495c24092434e5624369c78d9154b624c229e8afde61643e287bcf1d389aa56); /* line */ 
            coverage_0xe2a4dd29(0xee1fa2b6a80239f8cdd8e66124a4ee4c6226b3aed5ef2fc3ff2b995d613f8d20); /* statement */ 
if (!cache.hasMarket(m)) {coverage_0xe2a4dd29(0x45ff981b3e07392439bca033dce3987526cb7086bfa403a93150a494defb3807); /* branch */ 

coverage_0xe2a4dd29(0xb82669a481ddc4a363c4a94227e2852be8fd6183b9ace60cf788560e480dfd07); /* line */ 
                continue;
            }else { coverage_0xe2a4dd29(0xb2f8a3952aace5cd01ec043258b7f17a3d16469728a41df28f2e2b49f51c8792); /* branch */ 
}
coverage_0xe2a4dd29(0x3cfce0501a8af0c187ee95592721a472142d74206db771929ccd5dbb12c9b956); /* line */ 
            coverage_0xe2a4dd29(0x6531ec562ba621215ab9db5995d80007b61601e7d624c9686e806d9f448626cb); /* statement */ 
Types.Par memory par = state.getPar(account, m);
coverage_0xe2a4dd29(0xd00859ca78d8756e877a7e1066fcdb4a3f8d4d74d0e363de784beed5b91f1e0f); /* line */ 
            coverage_0xe2a4dd29(0xe857cd5b6736df45b962fef729f278f80a727edef7c6d801826b8102669f3961); /* statement */ 
if (par.isZero()) {coverage_0xe2a4dd29(0x787f9d6fea05fff9d46258f0d337531d1f9c4b47a66b076c052568e94b6ef372); /* branch */ 

coverage_0xe2a4dd29(0xb38243c356576927d3850a66c4259e56ae35f2996540bdb3a3bccbdf11586cdb); /* line */ 
                continue;
            } else {coverage_0xe2a4dd29(0xf4a89c943e9e7c79fae5aa9e11e1ee1b4e85d90be8a79a079bfdef6c72527479); /* statement */ 
coverage_0xe2a4dd29(0x020119443a7d5362bcd04c3f93f8dcfb82f4d9dfa01bced86b2e1ff9f2a1548e); /* branch */ 
if (par.sign) {coverage_0xe2a4dd29(0xc0f53b15e42fa7d1f9c829fa746db337121c0db9765f94676e30e6e18f5741e0); /* branch */ 

coverage_0xe2a4dd29(0xc20aedaa7a0783748528dcfbd7e0877120645399362589290e8b5cba08fef83d); /* line */ 
                coverage_0xe2a4dd29(0x51f686ff97ba62c58879347ad51676ebe61f300ad693d1416d1b638bfa19553d); /* statement */ 
return false;
            } else {coverage_0xe2a4dd29(0x6b7daa13ba32b2a2e2992b414ef7e60de35477f5b6db2300891f1e4a5478175e); /* branch */ 

coverage_0xe2a4dd29(0x85a2d8f312b8c2d2d96eba7440018082699f4ad2dc2887904575235b4aa003aa); /* line */ 
                coverage_0xe2a4dd29(0x8cf0fe26b3e2a6e858d2f3a16ca04ef2228adef6c88a3fca035c108fe0fec906); /* statement */ 
hasNegative = true;
            }}
        }
coverage_0xe2a4dd29(0x88f01f8ca89482df62ba0def7003241cb3fa1b0e859243f9385cfd2e6961ca23); /* line */ 
        coverage_0xe2a4dd29(0x1dde318ce2e5f87adfc1c1cd4df8eb2170583b76a63fdcb568e92802187158c7); /* statement */ 
return hasNegative;
    }

    // =============== Setter Functions ===============

    function updateIndex(
        Storage.State storage state,
        uint256 marketId
    )
        internal
        returns (Interest.Index memory)
    {coverage_0xe2a4dd29(0x20ee857172bfde458d6491601e514d353fcac5ca206a7b02ee5d5f96393ca9e8); /* function */ 

coverage_0xe2a4dd29(0x7389437e1813fcb114daee283433da2341e6530299497af4a4bb3978cdde8ed7); /* line */ 
        coverage_0xe2a4dd29(0x67e7ab44e8506bc77a7457daa603a1b775fd9fd3a2b74975b20fe59a9222bc4f); /* statement */ 
Interest.Index memory index = state.getIndex(marketId);
coverage_0xe2a4dd29(0x641c537db3f995e2e3647f8f9e501c9d7e40b93f28739445d12fbdcfae81937a); /* line */ 
        coverage_0xe2a4dd29(0xec00ce7f6e14be4a6782a887d58e70af7544e17342c16c5091d8d753ed9d912a); /* statement */ 
if (index.lastUpdate == Time.currentTime()) {coverage_0xe2a4dd29(0x5f55a8dbc2d1cd8010a7aa1c11643de4e72cb704f910be60988e3255843c9e4c); /* branch */ 

coverage_0xe2a4dd29(0xc8f4cf545184c5d5b24017eb93a19b9412e2ecf44ccfef97358cf8cdd4a0bf89); /* line */ 
            coverage_0xe2a4dd29(0xf306712136d0406a637f95e7cc3319a3f2bba1a942c64ca2cbc2a1d4fdc5192a); /* statement */ 
return index;
        }else { coverage_0xe2a4dd29(0xcfa543723d218432df181c4b25ffff79ae354528d0b1352fe9e0d2e22b3ea99c); /* branch */ 
}
coverage_0xe2a4dd29(0xbe047f35490de8fa17db11018066c6b08e10c6bb1fb61dd3011d1d115d21d5cc); /* line */ 
        coverage_0xe2a4dd29(0xf638e0fad4b13dde77166f3e0d84df50b19535fead611490bfdd628548e1efb4); /* statement */ 
return state.markets[marketId].index = state.fetchNewIndex(marketId, index);
    }

    function setStatus(
        Storage.State storage state,
        Account.Info memory account,
        Account.Status status
    )
        internal
    {coverage_0xe2a4dd29(0xc42384b87179d28b6682988f519be856004cf419583380d63ce5ddec41c46042); /* function */ 

coverage_0xe2a4dd29(0x0e2c55d3449aff524fa18c086eabeed3484dbc4aeb05fb11d9ba9d4a36c452b0); /* line */ 
        coverage_0xe2a4dd29(0xd16ba6abe1ed170a96319e4466e69a76614ecdd0c124bf514cd5becfaf27adbd); /* statement */ 
state.accounts[account.owner][account.number].status = status;
    }

    function setPar(
        Storage.State storage state,
        Account.Info memory account,
        uint256 marketId,
        Types.Par memory newPar
    )
        internal
    {coverage_0xe2a4dd29(0xadefc7e9d63af2704b0f14b0bd0873c5485631b85ea1ba50b609a9336e9a2383); /* function */ 

coverage_0xe2a4dd29(0x03fda8a26ce4c8de8ef49fe39b9f8701cdbdf311b74e10a23bc9f014d44ea5dd); /* line */ 
        coverage_0xe2a4dd29(0x5fdca41383df4c4c6900b5150c147a1990786ac6bafaf9d034e04e54bac0f43e); /* statement */ 
Types.Par memory oldPar = state.getPar(account, marketId);

coverage_0xe2a4dd29(0x48a5d2838971222ebfb7f5c2bf604ee550a8e5b97d4f117959b6a57d04794ef7); /* line */ 
        coverage_0xe2a4dd29(0x4d31377aa5c64db813071771c28904883c46a7265ecccd4d66cb388a1804e0f7); /* statement */ 
if (Types.equals(oldPar, newPar)) {coverage_0xe2a4dd29(0xa1d116edcbc65ff3dbf93a2330760024337efb1ec2162b16fa3b724bdbe099a6); /* branch */ 

coverage_0xe2a4dd29(0x642e566a1fd8c6fc392d9e8588e53450af0983c644acee8353fd0d7f0968a585); /* line */ 
            coverage_0xe2a4dd29(0x54cfc62160d3b63894952ccd7484be9c2dc21a827619e9a234b4a1049e0f50da); /* statement */ 
return;
        }else { coverage_0xe2a4dd29(0xff119f27c4909cc9298f6d5f59fad88d4a0c137cf90ad7163d291eba51ce158f); /* branch */ 
}

        // updateTotalPar
coverage_0xe2a4dd29(0x9aa44680c10f5d63b7e1a2f2d0f506986c0eeb53cb9d3551acf1ba69ec0bb282); /* line */ 
        coverage_0xe2a4dd29(0x611b67b05309ed049b36c564eeb2bb01233f44fd7c9e67f661dd21d87778bf72); /* statement */ 
Types.TotalPar memory totalPar = state.getTotalPar(marketId);

        // roll-back oldPar
coverage_0xe2a4dd29(0xa4a2189b5f5e5099dab0709e907a6d9464498812aa9989bc0b7b389743895bdb); /* line */ 
        coverage_0xe2a4dd29(0xbe25398a6d2787779f5f6da79123e13dc08729169c3bf75d76dea49e021117e0); /* statement */ 
if (oldPar.sign) {coverage_0xe2a4dd29(0x46a0a1a5b19ad669a12998ddcfe45cba071847eab17a007ce18adb65ab063ab5); /* branch */ 

coverage_0xe2a4dd29(0xf0d9a0393611e6b1780fef2d495fa92f031a0940455facdf6a8247da4548ade6); /* line */ 
            coverage_0xe2a4dd29(0x43d16d9eed6b5a0a6800fe78c2e25f616adc613d7e9f264102ad5a571ea19c33); /* statement */ 
totalPar.supply = uint256(totalPar.supply).sub(oldPar.value).to128();
        } else {coverage_0xe2a4dd29(0xfbfcf90398512243cda0686a02ef519f9c27316f82f408d220cfaa1c71b4d7a3); /* branch */ 

coverage_0xe2a4dd29(0x4b6d0288239af243408ed3989f93861dbb99754c70bfcd05a0df69ec2f59a793); /* line */ 
            coverage_0xe2a4dd29(0x92f8210ff34d42040b14d6c964bf8f9a704368608457386133290795db8bdb13); /* statement */ 
totalPar.borrow = uint256(totalPar.borrow).sub(oldPar.value).to128();
        }

        // roll-forward newPar
coverage_0xe2a4dd29(0x1dc87a5cf4d2493f95304dc3644112e5082427f501917a222682a00ff553e324); /* line */ 
        coverage_0xe2a4dd29(0x3a5455b609f7c3e9336b7982f29d29bf7fbbf3ba9622ae773369520960d88ed5); /* statement */ 
if (newPar.sign) {coverage_0xe2a4dd29(0x319573183cc1cbaa72ea63b24bda56835ac898b1ba41d7fd1b5845310c079987); /* branch */ 

coverage_0xe2a4dd29(0x13196348871ace51b065b5704f5277bfbd99c05de114a4e0ae943036df829446); /* line */ 
            coverage_0xe2a4dd29(0x5566e2ca1aa0aeaa14c80fe001f9f925c452fdb2fae4bc8f05cdf4845557f8dc); /* statement */ 
totalPar.supply = uint256(totalPar.supply).add(newPar.value).to128();
        } else {coverage_0xe2a4dd29(0x16d62f21c6fbd9c092250a682295b34fe06fe0b00bd36e1c40280191b4d4e15c); /* branch */ 

coverage_0xe2a4dd29(0x309a438bde137847707ece89dfe7fda8999e259637dbd1df9f7d0dca91eb8325); /* line */ 
            coverage_0xe2a4dd29(0xee4e9b9b55ac020d5387599b40d94a61f86126cb033010cd720bea888da50e9e); /* statement */ 
totalPar.borrow = uint256(totalPar.borrow).add(newPar.value).to128();
        }

coverage_0xe2a4dd29(0x7f488bd2a58750ab7161729959de79a8964a2cb0480baa876a006906a09e31f2); /* line */ 
        coverage_0xe2a4dd29(0xbf5d83285a070be3add5fd198de36268866fd6064429844f81fa0f0dcb8b3801); /* statement */ 
state.markets[marketId].totalPar = totalPar;
coverage_0xe2a4dd29(0x8d16dfd12a8cb45f2facdef34353ff258517846de2617a4eebeabdf7e2abad13); /* line */ 
        coverage_0xe2a4dd29(0xbea1f7372a50ffca5fc75f8f91408216de6b6b895ef5965e6894c672c1affa90); /* statement */ 
state.accounts[account.owner][account.number].balances[marketId] = newPar;
    }

    /**
     * Determine and set an account's balance based on a change in wei
     */
    function setParFromDeltaWei(
        Storage.State storage state,
        Account.Info memory account,
        uint256 marketId,
        Types.Wei memory deltaWei
    )
        internal
    {coverage_0xe2a4dd29(0x405e4ab182a835e14e9d2c5e03054a5190d44551d5e93fb1ba3102231001041d); /* function */ 

coverage_0xe2a4dd29(0x37d16d38f96b2f1f748b93e6a43728f28bf6245ff16b8d69b65ae1c6595d561a); /* line */ 
        coverage_0xe2a4dd29(0x33e854f60e1d9a13f2152eef151cb524b7d41875c31ae38e16c563418c1e1869); /* statement */ 
if (deltaWei.isZero()) {coverage_0xe2a4dd29(0x11c35cfbb61dc49f9b9f59e473f3171da0baa349710e1f3a2e9f622e33c983bf); /* branch */ 

coverage_0xe2a4dd29(0xae8bed931fbfccb9ffe42952b5074d1bca70e2d577557f4f6ea14484f79b359e); /* line */ 
            coverage_0xe2a4dd29(0xb5f1acf6a67bab1319b9d3f4fd88b1aaedb28f408fdd02274b3a6744791f01b6); /* statement */ 
return;
        }else { coverage_0xe2a4dd29(0xcf3a62d1347cc4b2cb25b94304fb20afd870bbffb508e224397b89833c2c0f67); /* branch */ 
}
coverage_0xe2a4dd29(0x16e3dff17856859ea730bcac94c236ddf9eec0ce0ab7b04a2f52409b50e8b898); /* line */ 
        coverage_0xe2a4dd29(0x13f9e91873027aafd44523f9572d144ba8f2c5160d5e5c726baf6d24aa73f036); /* statement */ 
Interest.Index memory index = state.getIndex(marketId);
coverage_0xe2a4dd29(0xfd55a4c938e705fe1ed0363b37ce37f93205f04dc78022968fe46fe18044f4d1); /* line */ 
        coverage_0xe2a4dd29(0xb83988b09bd02397ef73286ddab88017f117e590454c4f0efbe84821ec8df976); /* statement */ 
Types.Wei memory oldWei = state.getWei(account, marketId);
coverage_0xe2a4dd29(0x5ceba3127000b26b457b000b6d9825166a858c86983ce6283d396f7706023b1e); /* line */ 
        coverage_0xe2a4dd29(0x48fbdde966b08bd7342b914319b8cc9ba046641c4d57fe02873b4a6a29130e86); /* statement */ 
Types.Wei memory newWei = oldWei.add(deltaWei);
coverage_0xe2a4dd29(0x3fcfdd97d4f56453d3462bae5fcd28a1e59a1cef0566e280537a87b7d7330b6b); /* line */ 
        coverage_0xe2a4dd29(0x4ba84886c7d4a800042912f0d083b445fdd5d88fd4fdbcdc4e104016e0583c23); /* statement */ 
Types.Par memory newPar = Interest.weiToPar(newWei, index);
coverage_0xe2a4dd29(0xcc94f92df88a2bbde21627df93079855be2a1c8f0266cc8e2f12906939ddd7fb); /* line */ 
        coverage_0xe2a4dd29(0x98b9ea3eb511581f4d1dd5229feb78c55d58e63461e1599ed3872032a033a655); /* statement */ 
state.setPar(
            account,
            marketId,
            newPar
        );
    }
}
