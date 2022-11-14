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
import { IAutoTrader } from "../../protocol/interfaces/IAutoTrader.sol";
import { Account } from "../../protocol/lib/Account.sol";
import { Decimal } from "../../protocol/lib/Decimal.sol";
import { Math } from "../../protocol/lib/Math.sol";
import { Monetary } from "../../protocol/lib/Monetary.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Time } from "../../protocol/lib/Time.sol";
import { Types } from "../../protocol/lib/Types.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";


/**
 * @title FinalSettlement
 * @author dYdX
 *
 * FinalSettlement contract that allows closing of all positions on Solo.
 */
contract FinalSettlement is
    OnlySolo,
    IAutoTrader
{
function coverage_0x9026ed65(bytes32 c__0x9026ed65) public pure {}

    using Math for uint256;
    using SafeMath for uint32;
    using SafeMath for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "FinalSettlement";

    // ============ Events ============

    event Initialized(
        uint32 time
    );

    event Settlement(
        address indexed makerAddress,
        address indexed takerAddress,
        uint256 heldMarketId,
        uint256 owedMarketId,
        uint256 heldWei,
        uint256 owedWei
    );

    // ============ Storage ============

    // Time over which the liquidation spread goes from zero to maximum.
    uint256 public g_spreadRampTime;

    // Time at which the contract was initialized. Zero if uninitialized.
    uint32 public g_startTime = 0;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        uint256 rampTime
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x9026ed65(0xe4f642b4d8acc2a6f69b52394bd68bc9884340477b4ffbca6e372e2693bca17f); /* function */ 

coverage_0x9026ed65(0xc0223c67308fef1922edd6a50774cb8292f191ce875364b1338f334627b6d533); /* line */ 
        coverage_0x9026ed65(0x81b3f78982e0db380c27be3d38e30b34fc77581c5d0fe83a084ed1623b75d7a5); /* statement */ 
g_spreadRampTime = rampTime;
    }

    // ============ External Functions ============

    function initialize()
        external
    {coverage_0x9026ed65(0x91cd5cb833c0b4a67f4edb823b30b0ce209bcec24a8f44c9eff68260bed29f33); /* function */ 

coverage_0x9026ed65(0xef74c2e0a6032a182afffd5e5c82b5afb82eb7a5d3b4d14014f9cf629de74385); /* line */ 
        coverage_0x9026ed65(0xe4477cebdda929dfde679d41c47b046886277aa668ecfe540c0745952bc6a4e5); /* statement */ 
Require.that(
            g_startTime == 0,
            FILE,
            "Already initialized"
        );
coverage_0x9026ed65(0x5469c60f59b2ccbb464ee943c9762ef5fd199549e7e8fe39fc199973987e3257); /* line */ 
        coverage_0x9026ed65(0x1dc1df6bde317817c034209268f4f7573ccbfffa15dfb1f69607566b76cdc8ca); /* statement */ 
Require.that(
            SOLO_MARGIN.getIsGlobalOperator(address(this)),
            FILE,
            "Not a global operator"
        );

coverage_0x9026ed65(0xa13d9d2684d889fe034d7cf030dc6483889a2f4bb7a0e6f94d87f728f28ced0b); /* line */ 
        coverage_0x9026ed65(0xcc4b8a67b985f4898094bc77fb8e5eb44845c610ebc83d233a15d1b124c2e2be); /* statement */ 
g_startTime = Time.currentTime();

coverage_0x9026ed65(0x44b0e0230935e303c905cf7d4f6a857803b8120481ab472f9bf467174e4f5fcc); /* line */ 
        coverage_0x9026ed65(0x00ab0d2b0a85b57e8e649b82ba165b43aece8aa0ca87201f292b8c23efaf5051); /* statement */ 
emit Initialized(
            g_startTime
        );
    }

    // ============ Getters ============

    function getSpreadAdjustedPrices(
        uint256 heldMarketId,
        uint256 owedMarketId,
        uint32 timestamp
    )
        public
        view
        returns (
            Monetary.Price memory,
            Monetary.Price memory
        )
    {coverage_0x9026ed65(0x3cd9c96b0ef2aca183fd530984e8e55216f896a96aa842d6b39a7f1555dcb5f2); /* function */ 

        // Require timestamp to be at-or-after initialization
coverage_0x9026ed65(0x94575ac36ed200abb463fe978526862cb306933e415f8ecbad4bb2292e2fc45b); /* line */ 
        coverage_0x9026ed65(0xc001c70eaf47b8f08737ec5f4bb4c85765fcbfb5d09c3d7ad5cb3ef12285ef82); /* statement */ 
Require.that(
            g_startTime != 0,
            FILE,
            "Not initialized"
        );

        // Get the amount of time passed since deployment, bounded by the maximum ramp time.
coverage_0x9026ed65(0x2ae1d143a175a2aef5e96a03463ea19b26f68ba18255be06b7e8e1fa4cd2933c); /* line */ 
        coverage_0x9026ed65(0x857538ef843722e8570222f90d3b091b66c5ffa170f5b415353025f6380ed3b5); /* statement */ 
uint256 rampedTime = Math.min(
            timestamp.sub(g_startTime),
            g_spreadRampTime
        );

        // Get the liquidation spread prorated by the ramp time.
coverage_0x9026ed65(0x37b44d316b14027a9a22e49cf648755332d1479a13bf079e56ecc71f0c054779); /* line */ 
        coverage_0x9026ed65(0xb1bcf2016fe6bea29713ab9b57a3b2a7c24a1ba8466bbf1e8b99d7aac51b7b6a); /* statement */ 
Decimal.D256 memory spread = SOLO_MARGIN.getLiquidationSpreadForPair(
            heldMarketId,
            owedMarketId
        );
coverage_0x9026ed65(0xf7e6a14bf3ba72b213877b69aae43da7d73f2b778c3931560332edb7545bda01); /* line */ 
        coverage_0x9026ed65(0x087b7997b109c9e8a146f02f55fbf6c477a4dec168f89e2aa1216f01e3adaec2); /* statement */ 
spread.value = Math.getPartial(spread.value, rampedTime, g_spreadRampTime);

coverage_0x9026ed65(0x98c5835921467754bccc87e3a2613d9394642a172bd016610f82840500206794); /* line */ 
        coverage_0x9026ed65(0x4f26621e02cac5870bd1d26433e320d5a1a37c9466639b842ebfb3b3e50ecaf4); /* statement */ 
Monetary.Price memory heldPrice = SOLO_MARGIN.getMarketPrice(heldMarketId);
coverage_0x9026ed65(0x3d2f9ab47e1cff267df2cc3002c1657eca1108ba56be98163c87d36416d0d0b8); /* line */ 
        coverage_0x9026ed65(0xbb294643184dcd855df98052ef639aa4e1c536780e5cedf80fe565594ef48de0); /* statement */ 
Monetary.Price memory owedPrice = SOLO_MARGIN.getMarketPrice(owedMarketId);
coverage_0x9026ed65(0xe46efe4535ff2de1d55e1e1404ab9aa78b401183475d2b2d3f5eb61752de32cd); /* line */ 
        coverage_0x9026ed65(0x76e305cd14febd45bdda382e5531c7ae5ad82e8a31a6138c654ea0200aba2aac); /* statement */ 
owedPrice.value = owedPrice.value.add(Decimal.mul(owedPrice.value, spread));

coverage_0x9026ed65(0x6e2624b5ebced89dfca224a7f6ed63de2f63f255bac839528f62c1b48a60ac49); /* line */ 
        coverage_0x9026ed65(0x97b08410d494db4742a3dc4810b340f12bd08bbbaac2f1899c1f9e25a384e042); /* statement */ 
return (heldPrice, owedPrice);
    }

    // ============ Only-Solo Functions ============

    function getTradeCost(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory makerAccount,
        Account.Info memory takerAccount,
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.Wei memory inputWei,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
        returns (Types.AssetAmount memory)
    {coverage_0x9026ed65(0x3be131e1ba87bfff40b8570134afe28cc4ca94d6f6955946437f22dd49b4d93a); /* function */ 

        // require contract to be initialized
coverage_0x9026ed65(0xf50e80a8c34da4d4e11acc55568830e06c874d2d8f44b4ffb125f5304b89ff42); /* line */ 
        coverage_0x9026ed65(0xb41877c6c57ff5a6ad7883d2ded71b9d8b31653a14f4839fbecd8d096c3e91f4); /* statement */ 
Require.that(
            g_startTime != 0,
            FILE,
            "Contract must be initialized"
        );

        // return zero if input amount is zero
coverage_0x9026ed65(0x391ea80c26060295d443d5013bd44bad715255965b241f4e61ecd60c1c0e1fc0); /* line */ 
        coverage_0x9026ed65(0x9225413d522aea946551dab34d123ef7841f57df8e14e88bfb3f0cbd48e14775); /* statement */ 
if (inputWei.isZero()) {coverage_0x9026ed65(0x53380bc1987e88bdc6b25f6de6802160e49620e81f2f61373f87a021ad2e3b43); /* branch */ 

coverage_0x9026ed65(0x5fad4e30e67484ed71fdbe206e92a446d55a67842c76319f837491855b07c884); /* line */ 
            coverage_0x9026ed65(0xd73e5a8cc6aef41f0afd1c06981bd4349430f1dd4376e7c67f9bd08de0b29ed9); /* statement */ 
return Types.AssetAmount({
                sign: true,
                denomination: Types.AssetDenomination.Par,
                ref: Types.AssetReference.Delta,
                value: 0
            });
        }else { coverage_0x9026ed65(0xfba9e2b3ba378f270d9c273573440974d0167c94a82dd78aa9d51f9ffdaff20e); /* branch */ 
}

coverage_0x9026ed65(0x4e9749a8b009c5db4b50fa5992ed13c2b02bfc7357d8d5d28694cbeb9573de08); /* line */ 
        coverage_0x9026ed65(0x7983e2290954856e56c672182679c1a8df688a36c506c7258a6e4fac62f5531a); /* statement */ 
(uint256 owedMarketId) = abi.decode(data, (uint256));

coverage_0x9026ed65(0x429ebf5a908396e728b1173bb3315121e0bdd00d6656bf86666ed99b7e583f80); /* line */ 
        coverage_0x9026ed65(0xd6baeba8ae6b98e8d5c16a3f332627f3e9537041988191fa31390226924bd5c7); /* statement */ 
Types.AssetAmount memory result = getTradeCostInternal(
            inputMarketId,
            outputMarketId,
            makerAccount,
            oldInputPar,
            newInputPar,
            inputWei,
            owedMarketId
        );

coverage_0x9026ed65(0x63fbab9ae4351cf5d276639cca0bd77398f9bb40b074111ac5ef903768a7e631); /* line */ 
        coverage_0x9026ed65(0x863ac0383cac2b0886786c9bc8dcedcca6d8e35da540279f1a4cd39abc6c43f3); /* statement */ 
uint256 heldMarketId = (owedMarketId == inputMarketId)
            ? outputMarketId
            : inputMarketId;
coverage_0x9026ed65(0xe774a6b7985fe2a65d0246fb4a5e0f474d9eaf1d7ba1c0b32fbf5fd42da0d01e); /* line */ 
        coverage_0x9026ed65(0xe33a4cd7400c322c70324a894276982835b69df646bdd36a61c0bcbe71293732); /* statement */ 
uint256 heldWei = (owedMarketId == inputMarketId)
            ? result.value
            : inputWei.value;
coverage_0x9026ed65(0x51996300687264aef070589e398021cc570fd8f242267573368ac770f410ceb4); /* line */ 
        coverage_0x9026ed65(0xa35796c196e319b418972faaf168f331c65e93974d052d3a322d88e3170a6324); /* statement */ 
uint256 owedWei = (owedMarketId == inputMarketId)
            ? inputWei.value
            : result.value;
coverage_0x9026ed65(0xe0db965c61917dcbe9d5e981728d5a0aa26288bb9124db619daa05a0b569d246); /* line */ 
        coverage_0x9026ed65(0xa3bf247bad43cafdd919db21f36e281040e6d3c03b5814883e97c7bc3503d95f); /* statement */ 
emit Settlement(
            makerAccount.owner,
            takerAccount.owner,
            heldMarketId,
            owedMarketId,
            heldWei,
            owedWei
        );

coverage_0x9026ed65(0x8f1b62167e229f382a667f1264314efbd19f828dce78335ec0011f260b85147f); /* line */ 
        coverage_0x9026ed65(0xf2e524ffc1535aea663434078a1f553437f4d07830d8b11ff4d5f02c0330ec12); /* statement */ 
return result;
    }

    // ============ Helper Functions ============

    function getTradeCostInternal(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory makerAccount,
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.Wei memory inputWei,
        uint256 owedMarketId
    )
        private
        view
        returns (Types.AssetAmount memory)
    {coverage_0x9026ed65(0xbfcdd239decacfdff2f9bfb8e3125bcee6c759a940d6c62e181ab342cf0200eb); /* function */ 

coverage_0x9026ed65(0x4355753fb077e80ab367e52707119005a747d7ce41217e4d8de949e2f85a7d60); /* line */ 
        coverage_0x9026ed65(0x088056b810c9283b88e90b1b43601f7d69f5a16a76927628ec8f400a6ccd70ff); /* statement */ 
Types.AssetAmount memory output;
coverage_0x9026ed65(0x7388876aee42974c2e080c6094bb1e04cfffd88cbc84e8eb1bb776e0e0a0fd7c); /* line */ 
        coverage_0x9026ed65(0xe83a0c578306de9b4983c643ec77c90387aaa9a02fd70d2405bd2899f219b562); /* statement */ 
Types.Wei memory maxOutputWei = SOLO_MARGIN.getAccountWei(makerAccount, outputMarketId);

coverage_0x9026ed65(0xf7179e08a4e21d6f146e275c80211fadadf4ed5ace7bc21c157673b5e0f33e51); /* line */ 
        coverage_0x9026ed65(0x28b89d71edaf6bf5077b3a391a6c3bcec39e97540c725033678d4c9bc63334d5); /* statement */ 
if (inputWei.isPositive()) {coverage_0x9026ed65(0x1c52643c34894db3859e0c950695ed9068b675a63a82b1b24012413ac2d3916d); /* branch */ 

coverage_0x9026ed65(0x302fe7e1ac61480b3a029c0ee82c04218d082884a6f614bc1f8713193cd11ecd); /* line */ 
            coverage_0x9026ed65(0xa01b2ec003f9d88050e30ff00259bd84982de79c18d0fcf83dba9298e1d9b0fc); /* statement */ 
Require.that(
                inputMarketId == owedMarketId,
                FILE,
                "inputMarket mismatch",
                inputMarketId
            );
coverage_0x9026ed65(0x7b8b0474193c87051adc9cd332d92399b3364051bf277280229bec129326ab9a); /* line */ 
            coverage_0x9026ed65(0x2e2b9252eb25ba7cdb4af11f7be0bb73c29b5d426c732bff5cb7513077553419); /* statement */ 
Require.that(
                !newInputPar.isPositive(),
                FILE,
                "Borrows cannot be overpaid",
                newInputPar.value
            );
coverage_0x9026ed65(0xc6a3e0415039b7d39599a5e63f38be9ccb50d163bd69540456fcffe213431da0); /* line */ 
            coverage_0x9026ed65(0x4f357e8482324b97910a672957451bcb90e20feb8ff16b48d575c9421a5d08aa); /* assertPre */ 
coverage_0x9026ed65(0xacce4f0f210f53cefc9e37439df7d32ef73b911c8eaf765414e09fa4511a72aa); /* statement */ 
assert(oldInputPar.isNegative());coverage_0x9026ed65(0x95a99df1e7c7b58fb21768f99191870ea28018f6c96255dd82c0a516618a6fe5); /* assertPost */ 

coverage_0x9026ed65(0xa78c4ddf300b3d53e7865606a2eaf08efc4283f6de06fed183d5a963a52dbb1c); /* line */ 
            coverage_0x9026ed65(0xf9d9c29911f05f81ad568330d079756f832805aae38e2e65a8f77a87dee35c85); /* statement */ 
Require.that(
                maxOutputWei.isPositive(),
                FILE,
                "Collateral must be positive",
                outputMarketId,
                maxOutputWei.value
            );
coverage_0x9026ed65(0x111132b0548345fe4db351e7353c2505fe57d4584d96a42e27435331b47eb1ce); /* line */ 
            coverage_0x9026ed65(0x126ea1275f45742b4067c6fa764fd0e35dab4dfd838357249b1c1c378624628e); /* statement */ 
output = owedWeiToHeldWei(
                inputWei,
                outputMarketId,
                inputMarketId
            );
        } else {coverage_0x9026ed65(0xd441dc9b6a08de9b8e848184e7aeff55e82e74559c64371452e4853857420ae5); /* branch */ 

coverage_0x9026ed65(0x3f358c0555817b2f4ca2b97ea89ff092e91f6c8028972d2669d4197e1a36502b); /* line */ 
            coverage_0x9026ed65(0x5463ae9d3cd2e850924a83cb88f433a271d3645d2a69f84aa28d3132d028e19a); /* statement */ 
Require.that(
                outputMarketId == owedMarketId,
                FILE,
                "outputMarket mismatch",
                outputMarketId
            );
coverage_0x9026ed65(0x3c67247159531a350ad93ef506459400fb04888b025f0f17defa9c3b13eeb7c2); /* line */ 
            coverage_0x9026ed65(0x13568ce6084c4a258af84e5e3124d55619d8991cc3a866021a15f023de47d558); /* statement */ 
Require.that(
                !newInputPar.isNegative(),
                FILE,
                "Collateral cannot be overused",
                newInputPar.value
            );
coverage_0x9026ed65(0x5ef1fb352c8410261e5de82c124e921e461b5cd36efddee42c9ea07d9fd7ff9c); /* line */ 
            coverage_0x9026ed65(0xbc07b1a0953f351741bb9364412a5e09ea5a1361bfe56afa6ac7b6754c68f47e); /* assertPre */ 
coverage_0x9026ed65(0x12b6d6839f9be53b3913bbd7b7c59043e87acf60c4fa23129177385c01f9ae4b); /* statement */ 
assert(oldInputPar.isPositive());coverage_0x9026ed65(0x3ac97ecd20e8e0a2444821d16e7a9761e16b670dde182058acc198649f4a6088); /* assertPost */ 

coverage_0x9026ed65(0x48aca6df1b6878f783dd3c6d9bff596d622ba09b3b82d72d8640caa6882dd0b9); /* line */ 
            coverage_0x9026ed65(0x96046a8deae49d408660876cec559b486797c63c5f8fe3cc23bcec63539df222); /* statement */ 
Require.that(
                maxOutputWei.isNegative(),
                FILE,
                "Borrows must be negative",
                outputMarketId,
                maxOutputWei.value
            );
coverage_0x9026ed65(0x073f588475a3054bce9b52e9daa181ccd0b10fa5de24ef7dce6c5ecc72fef87e); /* line */ 
            coverage_0x9026ed65(0x93ab488712922eb22ece46804ba04147ca0b9dad544f62bd053939f7d1140e16); /* statement */ 
output = heldWeiToOwedWei(
                inputWei,
                inputMarketId,
                outputMarketId
            );
        }

coverage_0x9026ed65(0xc59a2f0e44c47e9d462d42fab80a1dd8fe37767576038cac3543b5835013b489); /* line */ 
        coverage_0x9026ed65(0x61636b5b7f552eef7493d03eda6ffb3341ac1b4cd9fcb98099b277e5f723c688); /* statement */ 
Require.that(
            output.value <= maxOutputWei.value,
            FILE,
            "outputMarket too small",
            output.value,
            maxOutputWei.value
        );
coverage_0x9026ed65(0xe6b5945992109cb03cd24a9edc8447b4214c636177ccb0636cccc813bc03cc16); /* line */ 
        coverage_0x9026ed65(0xb035d4006065d17c0e1e7057b2715befbc5a588d3e2de53b876b6e722ef1caac); /* assertPre */ 
coverage_0x9026ed65(0x43f788ba7eb821cda411cf2b994887c10faacca0d574340b6dad67bbaae30a90); /* statement */ 
assert(output.sign != maxOutputWei.sign);coverage_0x9026ed65(0x5c98c920f332e993a7e65343937b2eea845b376474ff3eccbbb58e7f8a699040); /* assertPost */ 


coverage_0x9026ed65(0x06d4358a1c06a59d0f335232ffbfc20c03e59d1cc5b7faee1d715ef83a29c5e5); /* line */ 
        coverage_0x9026ed65(0x9ee92186a29a1efb6a1b11475d7be094ce46c8be315719dc12e4d47842fc5ff2); /* statement */ 
return output;
    }

    function heldWeiToOwedWei(
        Types.Wei memory heldWei,
        uint256 heldMarketId,
        uint256 owedMarketId
    )
        private
        view
        returns (Types.AssetAmount memory)
    {coverage_0x9026ed65(0xb82d4377c43e67c64d5804fbfbc870417387249ed41a240dd87629413646e738); /* function */ 

coverage_0x9026ed65(0xd9aae6212d04647a3ceaf25e022d0c91f0a3664c87a9c7358556fa8c8941e41b); /* line */ 
        coverage_0x9026ed65(0x927f76db7de90374e5db6ffe5959089d1d70e95a2f8d7f772ed78afd59fbcb3e); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = getSpreadAdjustedPrices(
            heldMarketId,
            owedMarketId,
            Time.currentTime()
        );

coverage_0x9026ed65(0xee3c354d869d0a38c247000d06453d70b8828422f7a0714ac213e58658185a44); /* line */ 
        coverage_0x9026ed65(0x512a819c85e98860521983d01f632ee8c0d1786f7696510064c4c155197db094); /* statement */ 
uint256 owedAmount = Math.getPartialRoundUp(
            heldWei.value,
            heldPrice.value,
            owedPrice.value
        );

coverage_0x9026ed65(0x879b950b68478c5fe3e262d1d149729b6293f1e4951f5788343c75fe8fbf6654); /* line */ 
        coverage_0x9026ed65(0xaea847c5d32925d2b8f32184a6fe8c7e31bde643ec399fee7252ef2d7ba0447c); /* statement */ 
return Types.AssetAmount({
            sign: true,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: owedAmount
        });
    }

    function owedWeiToHeldWei(
        Types.Wei memory owedWei,
        uint256 heldMarketId,
        uint256 owedMarketId
    )
        private
        view
        returns (Types.AssetAmount memory)
    {coverage_0x9026ed65(0xfce9e036480ec078e072f29ed0adce4bec4f4004cbb6e5441271e2ab5fc9bf36); /* function */ 

coverage_0x9026ed65(0x8060cf4c85e6b35850ca471ec674bf442856910e4a8fa5b0e467b5a6b0e7c21a); /* line */ 
        coverage_0x9026ed65(0x65ed595513e9de85c139d7b3f1ab926c9bc561e0ba46d4daa6b9a0eceae8a662); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = getSpreadAdjustedPrices(
            heldMarketId,
            owedMarketId,
            Time.currentTime()
        );

coverage_0x9026ed65(0xbc57e5dbba7977b47d20a0d48d2aca9a07a8789ed0dca09078aa489fbe938d79); /* line */ 
        coverage_0x9026ed65(0x9d6bb829c28594b6b6c5f7837f5c185086489ab9232ffbd1e325276651050b9d); /* statement */ 
uint256 heldAmount = Math.getPartial(
            owedWei.value,
            owedPrice.value,
            heldPrice.value
        );

coverage_0x9026ed65(0xf46b1c44b3b85327f3c585b3b09342501155697ffa908688c992885ab7b79c71); /* line */ 
        coverage_0x9026ed65(0x6744b9dc074dbe06c564b42ea3dbd75c0da1e19653bc33ee28ea5c33c66081a8); /* statement */ 
return Types.AssetAmount({
            sign: false,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: heldAmount
        });
    }
}
