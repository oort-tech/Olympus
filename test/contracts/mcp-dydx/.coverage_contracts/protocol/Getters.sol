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

import { State } from "./State.sol";
import { IInterestSetter } from "./interfaces/IInterestSetter.sol";
import { IPriceOracle } from "./interfaces/IPriceOracle.sol";
import { Account } from "./lib/Account.sol";
import { Cache } from "./lib/Cache.sol";
import { Decimal } from "./lib/Decimal.sol";
import { Interest } from "./lib/Interest.sol";
import { Monetary } from "./lib/Monetary.sol";
import { Require } from "./lib/Require.sol";
import { Storage } from "./lib/Storage.sol";
import { Token } from "./lib/Token.sol";
import { Types } from "./lib/Types.sol";


/**
 * @title Getters
 * @author dYdX
 *
 * Public read-only functions that allow transparency into the state of Solo
 */
contract Getters is
    State
{
function coverage_0x21bb3140(bytes32 c__0x21bb3140) public pure {}

    using Cache for Cache.MarketCache;
    using Storage for Storage.State;
    using Types for Types.Par;

    // ============ Constants ============

    bytes32 FILE = "Getters";

    // ============ Getters for Risk ============

    /**
     * Get the global minimum margin-ratio that every position must maintain to prevent being
     * liquidated.
     *
     * @return  The global margin-ratio
     */
    function getMarginRatio()
        public
        view
        returns (Decimal.D256 memory)
    {coverage_0x21bb3140(0x137344cca03e6a0d423bfbaf0f768c75e96a2d8c2e43cddd60f8e6998e3122b6); /* function */ 

coverage_0x21bb3140(0x5fbb6f901bbd502d23546d55ccca4b4511021b9332e4c7bcbcd903db121983fc); /* line */ 
        coverage_0x21bb3140(0x93120f08100388c8de654b8e77b7f575473f6314c0589420e2efabd0f9ab952b); /* statement */ 
return g_state.riskParams.marginRatio;
    }

    /**
     * Get the global liquidation spread. This is the spread between oracle prices that incentivizes
     * the liquidation of risky positions.
     *
     * @return  The global liquidation spread
     */
    function getLiquidationSpread()
        public
        view
        returns (Decimal.D256 memory)
    {coverage_0x21bb3140(0x2d617b85b1605e0dc92f6c366b9cc44f46dba8cbd81638ed41ae0b81c73ddec6); /* function */ 

coverage_0x21bb3140(0x35a9d725832b60e38ea6364848d70c1d6aa1fa6f52a278adeb58a1e16fd7c366); /* line */ 
        coverage_0x21bb3140(0xc4f94f14b8af3f982ff23d4d7f2913c4b46ba1688ed217e00b78d0c1503e7b29); /* statement */ 
return g_state.riskParams.liquidationSpread;
    }

    /**
     * Get the global earnings-rate variable that determines what percentage of the interest paid
     * by borrowers gets passed-on to suppliers.
     *
     * @return  The global earnings rate
     */
    function getEarningsRate()
        public
        view
        returns (Decimal.D256 memory)
    {coverage_0x21bb3140(0xd24d147288022bc3902cf1cb21683e6e8b1585963f7a9a6373f3ef032f649221); /* function */ 

coverage_0x21bb3140(0xd67609de9ca61f5a8d9e8a6034a91252a84bb97c1a393107805c38ae0a12b6f2); /* line */ 
        coverage_0x21bb3140(0x8b22bb443292aa35ab231664d4a78e8df8db3f3c28162a15065a1f9c52fbd92d); /* statement */ 
return g_state.riskParams.earningsRate;
    }

    /**
     * Get the global minimum-borrow value which is the minimum value of any new borrow on Solo.
     *
     * @return  The global minimum borrow value
     */
    function getMinBorrowedValue()
        public
        view
        returns (Monetary.Value memory)
    {coverage_0x21bb3140(0xfdf65592a08d3b5e23f5ebd3b34016ac41e554b365d0ba2f3996a3201aee0151); /* function */ 

coverage_0x21bb3140(0x63b7c9acac383c9be5856e3dad4599ff2d9460d4f0eda7096ef15dc48339048f); /* line */ 
        coverage_0x21bb3140(0x412eada4393876cf1598d785b1478649273abe537d1839ce1f4462521e50e5e4); /* statement */ 
return g_state.riskParams.minBorrowedValue;
    }

    /**
     * Get all risk parameters in a single struct.
     *
     * @return  All global risk parameters
     */
    function getRiskParams()
        public
        view
        returns (Storage.RiskParams memory)
    {coverage_0x21bb3140(0x3a9c4e6567ff352b65ab6ca9e0bf5e6c08b42094410d4dc69ecbc77d4d96876d); /* function */ 

coverage_0x21bb3140(0xf7833c8850f201f750ba5eb1bfb4f3d905cd73ebf5c8c511e9081405cb15e0e4); /* line */ 
        coverage_0x21bb3140(0x7e42c98f47addd74cb9a84637941121e9bbaa2d80ecff5e8dd7f1f9985f381e2); /* statement */ 
return g_state.riskParams;
    }

    /**
     * Get all risk parameter limits in a single struct. These are the maximum limits at which the
     * risk parameters can be set by the admin of Solo.
     *
     * @return  All global risk parameter limnits
     */
    function getRiskLimits()
        public
        view
        returns (Storage.RiskLimits memory)
    {coverage_0x21bb3140(0xa8980298fa355ab36330bc25e207a36d9a63ad6f3cfdb120c609de7c110e19fe); /* function */ 

coverage_0x21bb3140(0xb4a4e7b45c7138f026db8770ba0163a207c1fe11f295266c39a0ae647cb9f484); /* line */ 
        coverage_0x21bb3140(0xa77a086b8179927198a023cb120c099986cb2632897aae1f14163e3b9b53ce39); /* statement */ 
return g_state.riskLimits;
    }

    // ============ Getters for Markets ============

    /**
     * Get the total number of markets.
     *
     * @return  The number of markets
     */
    function getNumMarkets()
        public
        view
        returns (uint256)
    {coverage_0x21bb3140(0xc2517b5fec6702aa6824c62f24a9e5fa148f5563722b73f879daddca2be9116c); /* function */ 

coverage_0x21bb3140(0x7baffe338c28e33d98a225fc1d6fbf82d2f10885ea38efd256ac3afae116d26e); /* line */ 
        coverage_0x21bb3140(0x87fce2c1cdcbb1333b7dbcc45787ebadac88b4f66e9a81a1dc99ed56f9a071f0); /* statement */ 
return g_state.numMarkets;
    }

    /**
     * Get the ERC20 token address for a market.
     *
     * @param  marketId  The market to query
     * @return           The token address
     */
    function getMarketTokenAddress(
        uint256 marketId
    )
        public
        view
        returns (address)
    {coverage_0x21bb3140(0xb875fe6eeff04d8da865d2cddd66d70d71c359729262bdaa9a33a49a35a49670); /* function */ 

coverage_0x21bb3140(0xf3de9c1a3d391201183d32d818863d8608eaf1288fa091086f2c0fb7085e1bb2); /* line */ 
        coverage_0x21bb3140(0xf6cc70e964216edeb3bc3e3c3a392615bce2d6fc25b9bb8cec55cd1d48664f9e); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xd2715b7d71b50447e5dc77240d1fe7530fb68804ef3f4fc8196bc6fa4ba46e64); /* line */ 
        coverage_0x21bb3140(0xee7fb2e78b5e8eb2ea206440b5610f5921ee38a4e363323aa7cd127dbf2ece35); /* statement */ 
return g_state.getToken(marketId);
    }

    /**
     * Get the total principal amounts (borrowed and supplied) for a market.
     *
     * @param  marketId  The market to query
     * @return           The total principal amounts
     */
    function getMarketTotalPar(
        uint256 marketId
    )
        public
        view
        returns (Types.TotalPar memory)
    {coverage_0x21bb3140(0xa656eea051fe54c3da85c23707eb7115495857442b4df163bf2fb873036a32ba); /* function */ 

coverage_0x21bb3140(0x5dc50ca988792b48f7ca359bbff6250f4864d978318a9b6377e3a4ea71bd2cb7); /* line */ 
        coverage_0x21bb3140(0x45bbd65770befa07e5dc0c668a300bb5e7a43211b0f408b3eb99b9e5608e2546); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xf9f1af646eba3eaf9aaf471b44b75b36a8ca938bf2c036fecc6d95460aba5096); /* line */ 
        coverage_0x21bb3140(0x310c8492fe7f182552b8345e229695ce615d6ca67746901a7a22a615a1cae8a5); /* statement */ 
return g_state.getTotalPar(marketId);
    }

    /**
     * Get the most recently cached interest index for a market.
     *
     * @param  marketId  The market to query
     * @return           The most recent index
     */
    function getMarketCachedIndex(
        uint256 marketId
    )
        public
        view
        returns (Interest.Index memory)
    {coverage_0x21bb3140(0xf3e54d31f24997b00574ef5a9daa943af6a62c705d06e85af71beed505c199ec); /* function */ 

coverage_0x21bb3140(0xd38d004594d32fdbdefe24fa82ab0b5b624ba0305c9ef15852c44082775aa079); /* line */ 
        coverage_0x21bb3140(0x61edf176997043322b066f3126682004cb146d0d8526683b9995b78a73115f14); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xbc9fba8c8512476db5ba25d09cb8254d1ff14e6352994714220c624de91e2427); /* line */ 
        coverage_0x21bb3140(0x65de8bba2fb63a2d9fc2188f8aa78240be728ecb7a7c0d8cb7b390f0ff1dc368); /* statement */ 
return g_state.getIndex(marketId);
    }

    /**
     * Get the interest index for a market if it were to be updated right now.
     *
     * @param  marketId  The market to query
     * @return           The estimated current index
     */
    function getMarketCurrentIndex(
        uint256 marketId
    )
        public
        view
        returns (Interest.Index memory)
    {coverage_0x21bb3140(0x1fcfd4f99bc2ebc34f25182094dac878f19cc74dd4bafd3cd5c9cb9fc97a9ec9); /* function */ 

coverage_0x21bb3140(0x849afdb2efdc84ce8d33e30da4fbaa5e82bc8d6dda1c51ba7b8043085fe4f97d); /* line */ 
        coverage_0x21bb3140(0x8ace812665849ad79c8d8c8e4d438a04712fa769ce63169709ba6e24078d136d); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x7878e31accc50282fd44261bb45948b2533609b9f362d65ae9cdb7072404501d); /* line */ 
        coverage_0x21bb3140(0xd46a6316d754e7ffc3227149a75311e53d929bc368e8f63ac2c6875a09779adb); /* statement */ 
return g_state.fetchNewIndex(marketId, g_state.getIndex(marketId));
    }

    /**
     * Get the price oracle address for a market.
     *
     * @param  marketId  The market to query
     * @return           The price oracle address
     */
    function getMarketPriceOracle(
        uint256 marketId
    )
        public
        view
        returns (IPriceOracle)
    {coverage_0x21bb3140(0xdfbada8ea45b3b1ad73a47d7c1615157b8de3eca536f64b15beb61ccfffd6a89); /* function */ 

coverage_0x21bb3140(0x21bc605a78cb4208247b874e6a111cd32ae6313aff46aa25ac0fa9c3f7c2c63f); /* line */ 
        coverage_0x21bb3140(0x9e628a1cb13b588d87fb12cee98464196effc4dca89e6a18fedff76cf053c8f3); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x9bd1e9e6b3eb6f8d0da6db7cb926eac873478c39c977111c2426351f52b7c692); /* line */ 
        coverage_0x21bb3140(0x00db27db75d29a0deda1892fc089e7876159b3a63aea6fa8c59180908bd32d36); /* statement */ 
return g_state.markets[marketId].priceOracle;
    }

    /**
     * Get the interest-setter address for a market.
     *
     * @param  marketId  The market to query
     * @return           The interest-setter address
     */
    function getMarketInterestSetter(
        uint256 marketId
    )
        public
        view
        returns (IInterestSetter)
    {coverage_0x21bb3140(0x735aabc56c2003432d55d0f678e09cd7209ea84be97e8e7481bd0f4eecf0f1e5); /* function */ 

coverage_0x21bb3140(0x9e0d94ac1d0592794bed1c3c9c130be06721b221b9e627fab51ee70d8e4ffec5); /* line */ 
        coverage_0x21bb3140(0x17f99ac0b6f51935e32a2d075b60c22681d0f16378b10ba8cad4f1dd5e10f09c); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xeb563b41d26a3aa84ea2548580119f1bd2248ce67b795b67d52a34c07c2ecee3); /* line */ 
        coverage_0x21bb3140(0x2bdcbfabb37245d6bbf3375fc39bb42f6ffbcea3a6709998b5d3ef27ecc2ce36); /* statement */ 
return g_state.markets[marketId].interestSetter;
    }

    /**
     * Get the margin premium for a market. A margin premium makes it so that any positions that
     * include the market require a higher collateralization to avoid being liquidated.
     *
     * @param  marketId  The market to query
     * @return           The market's margin premium
     */
    function getMarketMarginPremium(
        uint256 marketId
    )
        public
        view
        returns (Decimal.D256 memory)
    {coverage_0x21bb3140(0x5ad2b0fb4553d05a43110d7905b4113aae664432e0a630cbc5d8ea20c4c18c55); /* function */ 

coverage_0x21bb3140(0xa3d80832a10492e765f722ba37c657597463b1a8b48ef8d9e01c2fb8fd8df73c); /* line */ 
        coverage_0x21bb3140(0x4bd1b0485519c399433ccf8533b3b3dfa2412efb97d2310f544c7327159975e9); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x020f684eda1c5e28c37dd7adfd2854027d2c128f6272c633a04f8115860e52bf); /* line */ 
        coverage_0x21bb3140(0x82ba5ab3ce627d6342f0fb4552f03d70be7dd18957b7c84296a0e4632081a0af); /* statement */ 
return g_state.markets[marketId].marginPremium;
    }

    /**
     * Get the spread premium for a market. A spread premium makes it so that any liquidations
     * that include the market have a higher spread than the global default.
     *
     * @param  marketId  The market to query
     * @return           The market's spread premium
     */
    function getMarketSpreadPremium(
        uint256 marketId
    )
        public
        view
        returns (Decimal.D256 memory)
    {coverage_0x21bb3140(0xe4bb1817d822400259ec0454a86baa3a7ed230410dea33bcb4815e55eccea559); /* function */ 

coverage_0x21bb3140(0x6b1c4c8c00b67a0f629e3b317e97c72cb32baf75f46de9c1ab76fc8fafbb234e); /* line */ 
        coverage_0x21bb3140(0x88ad63bcecc9834e499c266ac2ecade3d44c4f761dbd0b1136db4433b50f7035); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xc9ef254e146aa0762ecf7e08d3bf5f3bea1a63a68d1cf9b9713ddb2e802b3250); /* line */ 
        coverage_0x21bb3140(0x89d75440024902888350a16fb0cbb5eb4e6c1655957d29c6a5e59170e1630241); /* statement */ 
return g_state.markets[marketId].spreadPremium;
    }

    /**
     * Return true if a particular market is in closing mode. Additional borrows cannot be taken
     * from a market that is closing.
     *
     * @param  marketId  The market to query
     * @return           True if the market is closing
     */
    function getMarketIsClosing(
        uint256 marketId
    )
        public
        view
        returns (bool)
    {coverage_0x21bb3140(0xc80d5f7b90eccb628f9b1dcaab62f31eae91548379838b67b8781ac9180decb6); /* function */ 

coverage_0x21bb3140(0x91fd4a025b6621bc1c451ed9dc4123e498e088d3fd32bd3fcd7d29e193a22632); /* line */ 
        coverage_0x21bb3140(0xfd9c19099abff94312e6b479a7bd8da45e9bcf11eb64625b78d3436a5af246ae); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xa28af5762e12ddeab8ee0e8f944e37f0953b2a184f2a4bb44ec7d455ed3c5901); /* line */ 
        coverage_0x21bb3140(0x384f61f3236b67a66c8b6c0d88635464a2f8a05d1489220bc95fe204fc536bd2); /* statement */ 
return g_state.markets[marketId].isClosing;
    }

    /**
     * Get the price of the token for a market.
     *
     * @param  marketId  The market to query
     * @return           The price of each atomic unit of the token
     */
    function getMarketPrice(
        uint256 marketId
    )
        public
        view
        returns (Monetary.Price memory)
    {coverage_0x21bb3140(0xd22871492f4b8eed2ba5410f763264b9d9cd0b01fefcf06633564740b432983a); /* function */ 

coverage_0x21bb3140(0x85a4daa48e29ddedfeb3421951d767586e623b035136a44e81611aec45481a12); /* line */ 
        coverage_0x21bb3140(0xd1f30feea9263a2b541201b61fdb881b7d99104c566b3913159eb4cc5c4530d6); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x4f36cd1d0d5f79e994b38a34cff65184724ecf17e5d6f697535ff9fd9906573d); /* line */ 
        coverage_0x21bb3140(0xf6ef6586cb4f565b9e2c69e70db690cf7e92b1bee37417e74afa01fd9c1f17f6); /* statement */ 
return g_state.fetchPrice(marketId);
    }

    /**
     * Get the current borrower interest rate for a market.
     *
     * @param  marketId  The market to query
     * @return           The current interest rate
     */
    function getMarketInterestRate(
        uint256 marketId
    )
        public
        view
        returns (Interest.Rate memory)
    {coverage_0x21bb3140(0xedec5e1ca210ea7ea91600aa0d8d91b2feb56b93676b0da4a44b3985a2e73cbf); /* function */ 

coverage_0x21bb3140(0x4b9fdd4fedc9e7d0bc1cc8579e19a35310b12b4bbaaacf9e1efdb8075f8a4d39); /* line */ 
        coverage_0x21bb3140(0x53d8537dd9833bf1a0c832448f923871bff6d10b7beac6a9355b138bf090eea3); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x6bfda3aba083694dae5a2c8135f2263e056bfcfbaebaf91966ec03b55afb8f6d); /* line */ 
        coverage_0x21bb3140(0x276baa3540c8623036d3b41bfb977fbc907d3f3442fba3e9319cc444c603b0fe); /* statement */ 
return g_state.fetchInterestRate(
            marketId,
            g_state.getIndex(marketId)
        );
    }

    /**
     * Get the adjusted liquidation spread for some market pair. This is equal to the global
     * liquidation spread multiplied by (1 + spreadPremium) for each of the two markets.
     *
     * @param  heldMarketId  The market for which the account has collateral
     * @param  owedMarketId  The market for which the account has borrowed tokens
     * @return               The adjusted liquidation spread
     */
    function getLiquidationSpreadForPair(
        uint256 heldMarketId,
        uint256 owedMarketId
    )
        public
        view
        returns (Decimal.D256 memory)
    {coverage_0x21bb3140(0x7022c3e541721b1c631e8e8060645951183165f7ce3e1e8989c8e52498e111f6); /* function */ 

coverage_0x21bb3140(0x29265dd1c6e000f195c97327b8fc7255de3e8b2b1cbe902e5ede8317fd0aecc5); /* line */ 
        coverage_0x21bb3140(0x3ba77d27c74434eb007acaa6d129a0a302fabccf3b1f68c3d24a29f904b1bf43); /* statement */ 
_requireValidMarket(heldMarketId);
coverage_0x21bb3140(0x714991f7827583ec9a566ad0779bf9e7f57ecdbf8d2008feb6f7224b6ad4e4f7); /* line */ 
        coverage_0x21bb3140(0x20bf1a80ddc1186f291e02cd9819ad0d10e9cc0dae71bc2029e3f1f51e3a856b); /* statement */ 
_requireValidMarket(owedMarketId);
coverage_0x21bb3140(0x00c39e8e90d3ea67f6ff1f1ce4bd53bb4dd60278192d466e1d1f42888fe0c5fb); /* line */ 
        coverage_0x21bb3140(0x1acdaa084e743199e19296fb800791161a256a33688d31ae57d01f4544ffe681); /* statement */ 
return g_state.getLiquidationSpreadForPair(heldMarketId, owedMarketId);
    }

    /**
     * Get basic information about a particular market.
     *
     * @param  marketId  The market to query
     * @return           A Storage.Market struct with the current state of the market
     */
    function getMarket(
        uint256 marketId
    )
        public
        view
        returns (Storage.Market memory)
    {coverage_0x21bb3140(0xccb19a4507fd8976f0906c2c98a3be58b98bbdfe4c9c82c457c445d33894eafb); /* function */ 

coverage_0x21bb3140(0xdb50b53b94529b2f272f84204b15780e6eab3a0fc6421426bdbef062e5103af0); /* line */ 
        coverage_0x21bb3140(0x2a0fa401f0cb10d2d38f3969b925ec007176a4858c5c241f18f6ffbba5cc7e58); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x2f20330f21ca243058657389b3cdc575a9b218b9a78328457451d85956aef30a); /* line */ 
        coverage_0x21bb3140(0xfeef4ef07b2e7a35728e7a10da4de47ef67706cc57fba01a9e1972dec0298db4); /* statement */ 
return g_state.markets[marketId];
    }

    /**
     * Get comprehensive information about a particular market.
     *
     * @param  marketId  The market to query
     * @return           A tuple containing the values:
     *                    - A Storage.Market struct with the current state of the market
     *                    - The current estimated interest index
     *                    - The current token price
     *                    - The current market interest rate
     */
    function getMarketWithInfo(
        uint256 marketId
    )
        public
        view
        returns (
            Storage.Market memory,
            Interest.Index memory,
            Monetary.Price memory,
            Interest.Rate memory
        )
    {coverage_0x21bb3140(0x969bb6a7b7b8590e684273879a55ed97731960c16339f4158a392e29a23c30a4); /* function */ 

coverage_0x21bb3140(0x4ecd3fc0c7e7b53d7a8f07c1e7ce5ae85f7e27850d70a5bfd184bd940094b52d); /* line */ 
        coverage_0x21bb3140(0xe4e79cf5d6eb03e143873278d23047d20937de8c1998cf4375d3eea22de4b232); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xf796442d8a2499e02aff67a81ade1b3376095302ca7307d6eed459980d31b3a0); /* line */ 
        coverage_0x21bb3140(0x2d7104a23b5eb89dfa1756a101a5a522e0231ce3d4ace9192e4698c4b04c36ee); /* statement */ 
return (
            getMarket(marketId),
            getMarketCurrentIndex(marketId),
            getMarketPrice(marketId),
            getMarketInterestRate(marketId)
        );
    }

    /**
     * Get the number of excess tokens for a market. The number of excess tokens is calculated
     * by taking the current number of tokens held in Solo, adding the number of tokens owed to Solo
     * by borrowers, and subtracting the number of tokens owed to suppliers by Solo.
     *
     * @param  marketId  The market to query
     * @return           The number of excess tokens
     */
    function getNumExcessTokens(
        uint256 marketId
    )
        public
        view
        returns (Types.Wei memory)
    {coverage_0x21bb3140(0x67116e0bd0a93152a57febc5e05a7f09f17099b75d0cc8b44c60496178041213); /* function */ 

coverage_0x21bb3140(0x8a36f279632902538ec0d7b60c66ea80961d242b409365057c8d1450008545ff); /* line */ 
        coverage_0x21bb3140(0x6a3aa7fb54396d9f5fc0196ab56c95b60387bc3a737c0df7f31860b24a316990); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x7d46bbbd25ccbcd29e031165ba9f32e6c3e19e252d18f11ec9bcf1e37413f592); /* line */ 
        coverage_0x21bb3140(0xc8be87e25a46a05db040f6bf3f5fa17d9f5bc29d45ab9923506ecc03440e3f2f); /* statement */ 
return g_state.getNumExcessTokens(marketId);
    }

    // ============ Getters for Accounts ============

    /**
     * Get the principal value for a particular account and market.
     *
     * @param  account   The account to query
     * @param  marketId  The market to query
     * @return           The principal value
     */
    function getAccountPar(
        Account.Info memory account,
        uint256 marketId
    )
        public
        view
        returns (Types.Par memory)
    {coverage_0x21bb3140(0x9ce7b2d11585a5ae59495d8d30a0ee6378c5157b05342aa603ecad362e274bc2); /* function */ 

coverage_0x21bb3140(0xb556beb1db02c44b9c2bfaf7d87dd8b9992650856b0cb99800c9341473f35f5b); /* line */ 
        coverage_0x21bb3140(0x2b57d8f3047b7b32eb2afd767b52a017a1bc5ac50a2a1e672584c390da17a814); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0x8ae4f7cf230a00212553d10fb68fa8387633812c1d3c89797c5bf484367fc3d8); /* line */ 
        coverage_0x21bb3140(0x65c8389d287d44fd8a338b9f198fb83d2933b3eff5dc3c1c1413a07d5eeb4ecb); /* statement */ 
return g_state.getPar(account, marketId);
    }

    /**
     * Get the token balance for a particular account and market.
     *
     * @param  account   The account to query
     * @param  marketId  The market to query
     * @return           The token amount
     */
    function getAccountWei(
        Account.Info memory account,
        uint256 marketId
    )
        public
        view
        returns (Types.Wei memory)
    {coverage_0x21bb3140(0xb0f75e74c6edef27ffbdb2e6bae4c8cc650c27da6f6c0d6e15f6de3d2953d947); /* function */ 

coverage_0x21bb3140(0xe2eb8ef26b9a228ac399a948656eab1fa02e10fdc50d90f0400a68d7057afd16); /* line */ 
        coverage_0x21bb3140(0x00b9b51705a83dd2ca21059fc6e87be79c6b35f8a7f18c7a214fac5509394c95); /* statement */ 
_requireValidMarket(marketId);
coverage_0x21bb3140(0xb140e18dd9a82a5aa3c999555fb11aab5ac005269e2a4d3fe110da8ea08c3454); /* line */ 
        coverage_0x21bb3140(0x5554dae25c550bae760049a0ccaab9cc9380ebbc21f4706808a8142bf231135a); /* statement */ 
return Interest.parToWei(
            g_state.getPar(account, marketId),
            g_state.fetchNewIndex(marketId, g_state.getIndex(marketId))
        );
    }

    /**
     * Get the status of an account (Normal, Liquidating, or Vaporizing).
     *
     * @param  account  The account to query
     * @return          The account's status
     */
    function getAccountStatus(
        Account.Info memory account
    )
        public
        view
        returns (Account.Status)
    {coverage_0x21bb3140(0x1f4b652cd89d76f989fc9d5be1afdda10b5eec859a10948617de6f51db597cfd); /* function */ 

coverage_0x21bb3140(0xc0d626c7f65588b3cad40d99d922f3b6473c61e072019978cd8e8016e7b23a50); /* line */ 
        coverage_0x21bb3140(0xc91713b478de8edd0a2fdb0032f6ce14513aa2ef20a663fb8b9192f46fc31985); /* statement */ 
return g_state.getStatus(account);
    }

    /**
     * Get the total supplied and total borrowed value of an account.
     *
     * @param  account  The account to query
     * @return          The following values:
     *                   - The supplied value of the account
     *                   - The borrowed value of the account
     */
    function getAccountValues(
        Account.Info memory account
    )
        public
        view
        returns (Monetary.Value memory, Monetary.Value memory)
    {coverage_0x21bb3140(0x5730906a22a54888401cce1e804be6feb7b8ecdeb745eb85a70515d543432fa6); /* function */ 

coverage_0x21bb3140(0x9e67603c7f1678bb3e68ab6bb2b75bbfd8eeda3ad45314d28cb0ce289975a5ca); /* line */ 
        coverage_0x21bb3140(0x47f3bd13002b915b92050a9209267cd67a1ad46df692384f95bf45ac1a94eb28); /* statement */ 
return getAccountValuesInternal(account, /* adjustForLiquidity = */ false);
    }

    /**
     * Get the total supplied and total borrowed values of an account adjusted by the marginPremium
     * of each market. Supplied values are divided by (1 + marginPremium) for each market and
     * borrowed values are multiplied by (1 + marginPremium) for each market. Comparing these
     * adjusted values gives the margin-ratio of the account which will be compared to the global
     * margin-ratio when determining if the account can be liquidated.
     *
     * @param  account  The account to query
     * @return          The following values:
     *                   - The supplied value of the account (adjusted for marginPremium)
     *                   - The borrowed value of the account (adjusted for marginPremium)
     */
    function getAdjustedAccountValues(
        Account.Info memory account
    )
        public
        view
        returns (Monetary.Value memory, Monetary.Value memory)
    {coverage_0x21bb3140(0x60e38a6f76c3d7b03bf950bd0def41e5b02ba9832bc28ec7b1d2ed160794594b); /* function */ 

coverage_0x21bb3140(0xcf4f07320b8dc5c98842475fd8dd67a005eb388168cf090f6d78c3aafdfb0b19); /* line */ 
        coverage_0x21bb3140(0xa77757288cc8621f7b6c78608d9bdbb62c890b0b370c304da6335f098b7e8714); /* statement */ 
return getAccountValuesInternal(account, /* adjustForLiquidity = */ true);
    }

    /**
     * Get an account's summary for each market.
     *
     * @param  account  The account to query
     * @return          The following values:
     *                   - The ERC20 token address for each market
     *                   - The account's principal value for each market
     *                   - The account's (supplied or borrowed) number of tokens for each market
     */
    function getAccountBalances(
        Account.Info memory account
    )
        public
        view
        returns (
            address[] memory,
            Types.Par[] memory,
            Types.Wei[] memory
        )
    {coverage_0x21bb3140(0x59d03a6177509be3341e1bfd3ceea4f25c0d8471ecfb9edbf57d545519efefda); /* function */ 

coverage_0x21bb3140(0x7c8015a5b142a0179da31f29260cb4b5625eb04606ddfe71d1bd597280f561b1); /* line */ 
        coverage_0x21bb3140(0x09a0560859d7c345822e8d2e603cdcc85267c674bfee4ffe3e1ef9078b7491f8); /* statement */ 
uint256 numMarkets = g_state.numMarkets;
coverage_0x21bb3140(0x9b942ffa39ba973a9fd071ae1bfa9e260640669e1941ce2c1e3fd106a4151754); /* line */ 
        coverage_0x21bb3140(0x7a97ad5bda7aabf2f06b86eacc6bf312ce93aa7e698403db875498d55fdce24a); /* statement */ 
address[] memory tokens = new address[](numMarkets);
coverage_0x21bb3140(0x5e4da90f3d92217baeeca80fbf1f409024673cf848945d138fc33ebe2ae12bd0); /* line */ 
        coverage_0x21bb3140(0x600b50cde18d11759b9454dc154f799fbf9b743218aa27c383f330cf60ee77b0); /* statement */ 
Types.Par[] memory pars = new Types.Par[](numMarkets);
coverage_0x21bb3140(0xf5da964176039366e81328eee770de29b14b3439eedb074d6ae73b192867a596); /* line */ 
        coverage_0x21bb3140(0xe29c74f0fa19e485e1f3ba122a7f45afd5a0c033615b74ff9bf8a273f53b5fea); /* statement */ 
Types.Wei[] memory weis = new Types.Wei[](numMarkets);

coverage_0x21bb3140(0x110c456460fef1f4d305545396e44facd2c5d1572b3d61632b9ee0d9d57bbace); /* line */ 
        coverage_0x21bb3140(0xd567f33d64b907fd96aa0015e588992a808c3c7e3ac8844d111b291f04e8f6cf); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0x21bb3140(0x06ff08916a1d11e5b30d0fe62cdbd06fa5adf65b8c0462914b629d8582af2040); /* line */ 
            coverage_0x21bb3140(0xea7724dbb341fa77bf0a7ba57260ee62813d5e679648c411bb099446730dcb12); /* statement */ 
tokens[m] = getMarketTokenAddress(m);
coverage_0x21bb3140(0x28f41065038f6daba318ab30562bd0feb3ea9139ef1cdc56bfcca93a0194760a); /* line */ 
            coverage_0x21bb3140(0xafc0a75d426e68c4d2eee388e2b38406f169cdbfabccd954f9bcc8c3394d69be); /* statement */ 
pars[m] = getAccountPar(account, m);
coverage_0x21bb3140(0xb1b0ce50debb60d5efdef5a92abe7ef4e37abcc8fa0dc1bfbc4983a3935b2de5); /* line */ 
            coverage_0x21bb3140(0x8fb3ea16dbbcaba51e08dc36a0979967c19e59492cffd9824077a452e59de1e7); /* statement */ 
weis[m] = getAccountWei(account, m);
        }

coverage_0x21bb3140(0x931c23299526ddce282daac57a0a3498cc0c52f947cc3c64a941ff58d205174d); /* line */ 
        coverage_0x21bb3140(0x8674f061347612fd7b325555fafba1d79783ce2e6d0ebf8fe868f35a6ea5fe5d); /* statement */ 
return (
            tokens,
            pars,
            weis
        );
    }

    // ============ Getters for Permissions ============

    /**
     * Return true if a particular address is approved as an operator for an owner's accounts.
     * Approved operators can act on the accounts of the owner as if it were the operator's own.
     *
     * @param  owner     The owner of the accounts
     * @param  operator  The possible operator
     * @return           True if operator is approved for owner's accounts
     */
    function getIsLocalOperator(
        address owner,
        address operator
    )
        public
        view
        returns (bool)
    {coverage_0x21bb3140(0x1a6684a3426a2182592e945f701c630951ef6a8149eafcc1cbd8b5434f68b0f8); /* function */ 

coverage_0x21bb3140(0x345cdaf4f71a4b6025956dda7ff946272d3455e8eee869eb417d5f487528e9ef); /* line */ 
        coverage_0x21bb3140(0xf812a11493fd49715d5be6902a5ef705499da9f1025a7d6ac567b6d2a2e30525); /* statement */ 
return g_state.isLocalOperator(owner, operator);
    }

    /**
     * Return true if a particular address is approved as a global operator. Such an address can
     * act on any account as if it were the operator's own.
     *
     * @param  operator  The address to query
     * @return           True if operator is a global operator
     */
    function getIsGlobalOperator(
        address operator
    )
        public
        view
        returns (bool)
    {coverage_0x21bb3140(0x7e0cf62b94238277a0b036f92fb87db6162f33e3bce7b15f72d8fb1518fc0343); /* function */ 

coverage_0x21bb3140(0x7a5f6d9824ba4f8cf0c135b3ba9c394954899d51cbcf3f3dd86d7a7e0dd3540c); /* line */ 
        coverage_0x21bb3140(0xe9f3d833a02d1a2c3cab63d9fa4debf552866d6ffd86dd34a4027bcad0bbc7d2); /* statement */ 
return g_state.isGlobalOperator(operator);
    }

    // ============ Private Helper Functions ============

    /**
     * Revert if marketId is invalid.
     */
    function _requireValidMarket(
        uint256 marketId
    )
        private
        view
    {coverage_0x21bb3140(0x7989298afaa2d41e9a00da0eadac3a881a6f142bbc5d945dd6462901a30bf556); /* function */ 

coverage_0x21bb3140(0xcf6b5513ebcc6892ca73dc0abca03089cba1ef0d7a8a7b25c0ed12aebab55efc); /* line */ 
        coverage_0x21bb3140(0x6e28f9cac63c9d218c234acd880ee2eccab871e9bf10454d5da5f0a9fb21b9a3); /* statement */ 
Require.that(
            marketId < g_state.numMarkets,
            FILE,
            "Market OOB"
        );
    }

    /**
     * Private helper for getting the monetary values of an account.
     */
    function getAccountValuesInternal(
        Account.Info memory account,
        bool adjustForLiquidity
    )
        private
        view
        returns (Monetary.Value memory, Monetary.Value memory)
    {coverage_0x21bb3140(0x9492b22c54e8fee7a57e882358f67df7c7f2608edfe446e84d1b033a38ef1699); /* function */ 

coverage_0x21bb3140(0xe8f7a9d03ae964a0179833388e491cafc5ee0ae526d889f9695b3ef6b7960033); /* line */ 
        coverage_0x21bb3140(0x097abc3fcc9dce34a0da63e838571745296ed6cc8c37635424e3f571ea5bb49f); /* statement */ 
uint256 numMarkets = g_state.numMarkets;

        // populate cache
coverage_0x21bb3140(0xf39582de1a77aafe355cccabd6e0d6ef82ab118e97efef71079e5da8a2de73c8); /* line */ 
        coverage_0x21bb3140(0x0cf4cc66307394ccdedf93a0049cfaa5c4ec46e5f58b5f8c2e63df413505bf4e); /* statement */ 
Cache.MarketCache memory cache = Cache.create(numMarkets);
coverage_0x21bb3140(0xb24777e8c6ed946153a8d5a727b0947c121ec0e9a8427f5533b71c5e4048d169); /* line */ 
        coverage_0x21bb3140(0x5f240c9c7f7f70471ff077796b7106bb63efe617169fec52f99ddaf6c642040d); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0x21bb3140(0xf31b58482ee23f2db53bdea50bec1a1bc871c221a813ff00ca04f106dc2d9a11); /* line */ 
            coverage_0x21bb3140(0x69930d90cbe5aaeca22dee0075f81545d785ade8984a21ada33024a862a2fbdf); /* statement */ 
if (!g_state.getPar(account, m).isZero()) {coverage_0x21bb3140(0xe4830920000cc20d506c633252c39a1ba91d0686a83dfb16cf5053bf0ad9c295); /* branch */ 

coverage_0x21bb3140(0x97a07b92df2b6e781b0fb977d994029349d25ae2a77050868f6311812823a842); /* line */ 
                coverage_0x21bb3140(0x69d8dbef5ad81770df24a4c80ffa350b7aad3482444fae9f4d0c7d3eb6875cf5); /* statement */ 
cache.addMarket(g_state, m);
            }else { coverage_0x21bb3140(0x68316b44518c0bb75af01f4a99c72fdbf6b3e2502b0cad539170cfa3c639682e); /* branch */ 
}
        }

coverage_0x21bb3140(0x57863203f29d0ab8bec2a2158e44bd49efc4dbd0a50d3670bc5c20abfc2f917a); /* line */ 
        coverage_0x21bb3140(0x9ceb33780e4cdd7f4f915010b55c599a7941d646a3bbca2fa75aad7a98684fc9); /* statement */ 
return g_state.getAccountValues(account, cache, adjustForLiquidity);
    }
}
