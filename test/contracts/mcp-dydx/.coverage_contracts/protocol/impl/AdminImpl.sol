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

import { IInterestSetter } from "../interfaces/IInterestSetter.sol";
import { IPriceOracle } from "../interfaces/IPriceOracle.sol";
import { Decimal } from "../lib/Decimal.sol";
import { Interest } from "../lib/Interest.sol";
import { Monetary } from "../lib/Monetary.sol";
import { Require } from "../lib/Require.sol";
import { Storage } from "../lib/Storage.sol";
import { Token } from "../lib/Token.sol";
import { Types } from "../lib/Types.sol";


/**
 * @title AdminImpl
 * @author dYdX
 *
 * Administrative functions to keep the protocol updated
 */
library AdminImpl {
function coverage_0xd6b9804f(bytes32 c__0xd6b9804f) public pure {}

    using Storage for Storage.State;
    using Token for address;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "AdminImpl";

    // ============ Events ============

    event LogWithdrawExcessTokens(
        address token,
        uint256 amount
    );

    event LogAddMarket(
        uint256 marketId,
        address token
    );

    event LogSetIsClosing(
        uint256 marketId,
        bool isClosing
    );

    event LogSetPriceOracle(
        uint256 marketId,
        address priceOracle
    );

    event LogSetInterestSetter(
        uint256 marketId,
        address interestSetter
    );

    event LogSetMarginPremium(
        uint256 marketId,
        Decimal.D256 marginPremium
    );

    event LogSetSpreadPremium(
        uint256 marketId,
        Decimal.D256 spreadPremium
    );

    event LogSetMarginRatio(
        Decimal.D256 marginRatio
    );

    event LogSetLiquidationSpread(
        Decimal.D256 liquidationSpread
    );

    event LogSetEarningsRate(
        Decimal.D256 earningsRate
    );

    event LogSetMinBorrowedValue(
        Monetary.Value minBorrowedValue
    );

    event LogSetGlobalOperator(
        address operator,
        bool approved
    );

    // ============ Token Functions ============

    function ownerWithdrawExcessTokens(
        Storage.State storage state,
        uint256 marketId,
        address recipient
    )
        public
        returns (uint256)
    {coverage_0xd6b9804f(0x39953427ead6b8634d7da6943f678c5c95a80a6f939532813e9e6aacd1847e51); /* function */ 

coverage_0xd6b9804f(0x957f46b797ae34d0c5febeb79c25da31bfb37217242daa3f02e6c9612d97038f); /* line */ 
        coverage_0xd6b9804f(0x4703900b417d8323feadb467c8eb6474ebce879a5d9491c7892e8b5c0dd847a9); /* statement */ 
_validateMarketId(state, marketId);
coverage_0xd6b9804f(0xf067b84c3ea8f9656123c292e24091ee69e87cb41b06f32112b9f075945699d7); /* line */ 
        coverage_0xd6b9804f(0x6fccedf9ceeeb58aa6e5ca6d95c5aea476fc5152ff4a48c2adc40540f5f8aea1); /* statement */ 
Types.Wei memory excessWei = state.getNumExcessTokens(marketId);

coverage_0xd6b9804f(0x5bb991c4b4ba3d2c625767bf3251ca50293baf116c396211510be37fdfefc72c); /* line */ 
        coverage_0xd6b9804f(0xf34ed433d932dd10cdf1cfd73871fcf2832db8a36cc61329d0d12846e925dd9f); /* statement */ 
Require.that(
            !excessWei.isNegative(),
            FILE,
            "Negative excess"
        );

coverage_0xd6b9804f(0x0e8c4e84555e6bb2289423494d55c32dc7964410609099addea4e44e3191a1de); /* line */ 
        coverage_0xd6b9804f(0x750d38b9191c6288237bb38ec17b413227f3ba95b81ea408eddcb0123efb7643); /* statement */ 
address token = state.getToken(marketId);

coverage_0xd6b9804f(0xf2cc1b2cd7d906992d1a9190f9ce9702f79fbbddb5db97e520a6a8ae844072ac); /* line */ 
        coverage_0xd6b9804f(0x91377e48ab7cbce65f9d76301f98335fa154e11ded987087a3fb6089a055d206); /* statement */ 
uint256 actualBalance = token.balanceOf(address(this));
coverage_0xd6b9804f(0x6bf8699fd734143c92bd8c6fa38ed9238dc62f1a6a2244652c37e17d467baf41); /* line */ 
        coverage_0xd6b9804f(0xf009e5e67c28294397b211cd6dc1c4291ec133ea6ab1023bb945ad10cb78c78c); /* statement */ 
if (excessWei.value > actualBalance) {coverage_0xd6b9804f(0xa6c36e4d1e0e95e6c907116cee43a76ed6ea2742f839ff1f48341985ffbaa6a9); /* branch */ 

coverage_0xd6b9804f(0x1d8e574b763b76cb4cd6e8dc8451fa697c5c635c1afb3d1855eb5b6ef38fcf65); /* line */ 
            coverage_0xd6b9804f(0x49699db1d9b5014a5e5f02d2c04eb7bd85e772adf48bfb92d90ce034ce8497f0); /* statement */ 
excessWei.value = actualBalance;
        }else { coverage_0xd6b9804f(0x2cc2ad1c0c620c223fe076a0337e08c88edc3ce7e70fd0f0e7193bf0e0d70e1a); /* branch */ 
}

coverage_0xd6b9804f(0x04390fe8b9dd3aebb6cd084f82ce68263a1304b05de2d54c78ccdecbb1bec509); /* line */ 
        coverage_0xd6b9804f(0xc8034f3148ea48824905bea5295982d3e040a9dc647523d5658bb186a337fc8a); /* statement */ 
token.transfer(recipient, excessWei.value);

coverage_0xd6b9804f(0x3e83983f62767116acc1292d390e5f0b697129063e002fd3600196c53bf3330b); /* line */ 
        coverage_0xd6b9804f(0x3efb98d16650fc734c6c256fafb3fa05c7d95a9754d8b8527c28afe83c6b57a6); /* statement */ 
emit LogWithdrawExcessTokens(token, excessWei.value);

coverage_0xd6b9804f(0x3b4e24f757fbdb371f39079cb32ef91e7f0a3b1556801621b16f35fd067eb04c); /* line */ 
        coverage_0xd6b9804f(0xb524bd36b6428569a1fb6e076ff94c5d93522559234af369369c179181ab385f); /* statement */ 
return excessWei.value;
    }

    function ownerWithdrawUnsupportedTokens(
        Storage.State storage state,
        address token,
        address recipient
    )
        public
        returns (uint256)
    {coverage_0xd6b9804f(0x682731db76ea3c0ef03820d298effca7fbd25ba1aa32c62ddb955384376d0310); /* function */ 

coverage_0xd6b9804f(0x3d8a0fb30ba01ffc413322d395599d16397c9403cc40434ea7d9c4877dd89acf); /* line */ 
        coverage_0xd6b9804f(0x70072a4ce675951127cbcb9e3b05968074caacc62d85ea7ee199b68ecfee618e); /* statement */ 
_requireNoMarket(state, token);

coverage_0xd6b9804f(0x9498e0899fbb976171e37ca0f1383b69ad8a66006821c7308883e29b9363e4ed); /* line */ 
        coverage_0xd6b9804f(0x8b0b7ea5388f64dad0456ae2af6d5e937e20f9c0d0caa19201ed80ac1b5b8748); /* statement */ 
uint256 balance = token.balanceOf(address(this));
coverage_0xd6b9804f(0x54776d2d14909f16320119df76cec9b74c24d550bec6efa231c8ca3a27c4ed76); /* line */ 
        coverage_0xd6b9804f(0x17fe888f1f45e78acf68f6dece298344dae7945f80bee6dfcbcbd7773c54e33f); /* statement */ 
token.transfer(recipient, balance);

coverage_0xd6b9804f(0xca1b5ef084eef25d65d428a77981f6027eb2e9a213217a8091b18cb2c05e4df0); /* line */ 
        coverage_0xd6b9804f(0x3773b5045588f30a49bc62becbc033d66492d2183f9eb220bfc97d586ff9593c); /* statement */ 
emit LogWithdrawExcessTokens(token, balance);

coverage_0xd6b9804f(0x29d1cc9243006665b4cabaa3ba67e3208fc92f1521449f8036eb43e64bce7734); /* line */ 
        coverage_0xd6b9804f(0xbed7238bae9427148b6a42fa79123841b5a243a79d75ffb766e4f9752a746257); /* statement */ 
return balance;
    }

    // ============ Market Functions ============

    function ownerAddMarket(
        Storage.State storage state,
        address token,
        IPriceOracle priceOracle,
        IInterestSetter interestSetter,
        Decimal.D256 memory marginPremium,
        Decimal.D256 memory spreadPremium
    )
        public
    {coverage_0xd6b9804f(0xfd80da0134887ad7fda5a586be2de115f0cee74d51edc29ae48a10e2291614cc); /* function */ 

coverage_0xd6b9804f(0x8f6cd62396113096b6417cfd6d2547227ce7b889cae5fb0bb6167ee0aa761950); /* line */ 
        coverage_0xd6b9804f(0xfd4f1b33e569e9a226fcc5153d13cc7d2584edc6aeed2835b1b2ded09a4dae24); /* statement */ 
_requireNoMarket(state, token);

coverage_0xd6b9804f(0x955ba4086ed4ce444275063fd82f3015015a2849820026d90137b917c5e50d95); /* line */ 
        coverage_0xd6b9804f(0xadd4079422c7099b7b2b783ce8ffdf58e5fd413bd5df5c5c2afe2542d614e61b); /* statement */ 
uint256 marketId = state.numMarkets;

coverage_0xd6b9804f(0xaf5ef698d3c09754247c6123e121868e6f58d723be3050949653ae7cdcd88251); /* line */ 
        state.numMarkets++;
coverage_0xd6b9804f(0x873ec771b4ce3272d8c2765e2656807f92e4d770688189fed786ac7985b16a57); /* line */ 
        coverage_0xd6b9804f(0x5c94848086fd11e5cfc6bcfdf8d09ad457be3c7753510519bb8df856799886ca); /* statement */ 
state.markets[marketId].token = token;
coverage_0xd6b9804f(0xc2ab67a2a13fe780d57359a30836d197aa70d4d2e35a83c2a4bfb6b6efd2200c); /* line */ 
        coverage_0xd6b9804f(0x131831ee84cd2a76116c63c8364e62a5c7ff6b7fb4fb42b2d3832527c9dbd788); /* statement */ 
state.markets[marketId].index = Interest.newIndex();

coverage_0xd6b9804f(0x27c5a31c952a30608c98ea10570d4dab2cafc4a5c26892d17b845a9e3e3884e6); /* line */ 
        coverage_0xd6b9804f(0xb0c220364dc384009e7dcb6d76b4a47f67ea336f3cefdd03c5064e6a74a6a94f); /* statement */ 
emit LogAddMarket(marketId, token);

coverage_0xd6b9804f(0xb1e30538b6e11383470dafd695e6234a9e0266cd5210591b32c0fba4f8a0dc40); /* line */ 
        coverage_0xd6b9804f(0xdceb888437de85115d3d928e11744675b690cbc872662c35e1a984b4934e9abf); /* statement */ 
_setPriceOracle(state, marketId, priceOracle);
coverage_0xd6b9804f(0xc19fc17fc5e18d818a44fed854ba0b81172200e62426b40dcb6f492ad6f0d3a0); /* line */ 
        coverage_0xd6b9804f(0x6a50bdeacb80ab93c6e70eb43045cd0c4ac48df6f422b8e04b909ab2b6ad2412); /* statement */ 
_setInterestSetter(state, marketId, interestSetter);
coverage_0xd6b9804f(0xb7442821940a3bcdba33840893517380529aad015b89250259fb990715f253e4); /* line */ 
        coverage_0xd6b9804f(0x5420738b03e4eff7b15d2c41f42789417e409481965fa4fc064666513d862799); /* statement */ 
_setMarginPremium(state, marketId, marginPremium);
coverage_0xd6b9804f(0x170c0c1bcea1dff2b5a5618dc72195489624ef99d31cf373bd9264ad6ff0eee5); /* line */ 
        coverage_0xd6b9804f(0x8468352c4a5a28d6a82346d329109372bf64415c0d462feba7b7e66903e6ba38); /* statement */ 
_setSpreadPremium(state, marketId, spreadPremium);
    }

    function ownerSetIsClosing(
        Storage.State storage state,
        uint256 marketId,
        bool isClosing
    )
        public
    {coverage_0xd6b9804f(0x6468c2d53e05af0fb95e171f12d2edb66aac88bbc9154c710be3705ffade21b7); /* function */ 

coverage_0xd6b9804f(0xd0276b920b6fa5be59edbf9b0925d027347c8749ab30c8947b57d0fa431bcc12); /* line */ 
        coverage_0xd6b9804f(0xaaaae5661655ac9a261360182a12db1aa03706917e0e3976b5c3ace18799a94a); /* statement */ 
_validateMarketId(state, marketId);
coverage_0xd6b9804f(0x22a473e241390de0bb3a73dec807e375d7c100c704e19375b491de2849f1ee3d); /* line */ 
        coverage_0xd6b9804f(0xb719be24579e44288acdb49f0e3dd7f18d4feab87a1fd3d4b65d3fc042a08e83); /* statement */ 
state.markets[marketId].isClosing = isClosing;
coverage_0xd6b9804f(0x7d30537222a258f2ea8efb455d99624c9d691f143a4b44c036e25bcbbeb9df38); /* line */ 
        coverage_0xd6b9804f(0x7e0760312e0d10cc4a91fffd7447527af48ab0aa1c1d010e81df96e88da94109); /* statement */ 
emit LogSetIsClosing(marketId, isClosing);
    }

    function ownerSetPriceOracle(
        Storage.State storage state,
        uint256 marketId,
        IPriceOracle priceOracle
    )
        public
    {coverage_0xd6b9804f(0x33eb238920f2a8e3802611bf89351e151e3ec577f4f29172250ed938ffb08c13); /* function */ 

coverage_0xd6b9804f(0x59b3594bbbb0ff7004061071a5470d8ec668caf42ddfa5aec8fd3ab8e8d624bc); /* line */ 
        coverage_0xd6b9804f(0x43fa6030c97a5dbbf8a418d3b0c45c6a6a3bfadb9120e2c5a4f7c6e2c2b52f11); /* statement */ 
_validateMarketId(state, marketId);
coverage_0xd6b9804f(0xed47c66545df03365e3ad6f29334a27d508c93ab08b8789f8d0e0633d35223df); /* line */ 
        coverage_0xd6b9804f(0x1ecb4a4888f177690f729c88e57111db20d979e6f24537d4af03c0d0583c4626); /* statement */ 
_setPriceOracle(state, marketId, priceOracle);
    }

    function ownerSetInterestSetter(
        Storage.State storage state,
        uint256 marketId,
        IInterestSetter interestSetter
    )
        public
    {coverage_0xd6b9804f(0xe3e7ea6107b59a8c051f3ba0a626a80a82cc1a1ccaf1cf3a9dff8b948843c380); /* function */ 

coverage_0xd6b9804f(0xf209cae81978be2ceab63d6de6452b97012d88038115877fbdcc98a994c05073); /* line */ 
        coverage_0xd6b9804f(0xdc343ccdff49bde7335d5b53cc466033fb389dc0c7d8d35728c9c9a49ddeda18); /* statement */ 
_validateMarketId(state, marketId);
coverage_0xd6b9804f(0xbb42ae0e344ca6c2c5cd06b2c1fc6fb0b9a6f25e85a706a8131de3b8549ec768); /* line */ 
        coverage_0xd6b9804f(0x7ed24ebff10e5d4358af55ed9715d482daad69b7cfd464ee48ad0abbd7c8a0a2); /* statement */ 
_setInterestSetter(state, marketId, interestSetter);
    }

    function ownerSetMarginPremium(
        Storage.State storage state,
        uint256 marketId,
        Decimal.D256 memory marginPremium
    )
        public
    {coverage_0xd6b9804f(0xc6873cf77a1b05f27931fa4c3b6fcfe51f60c54348bdc15c133b8bf85e96c5fc); /* function */ 

coverage_0xd6b9804f(0x3fabd99775badc8c55c839c164a5557db56e1440ae156b5e741e644547921b57); /* line */ 
        coverage_0xd6b9804f(0x647e7802e4fd00bf700dd85bfc6f77133219d7ddb835759408f97e9fec22cc16); /* statement */ 
_validateMarketId(state, marketId);
coverage_0xd6b9804f(0x43a89371bfdcb2e8a70fac5f2d3629155b97249688d5a324c4be779a9cc2ee62); /* line */ 
        coverage_0xd6b9804f(0x3eaa4d99d79f327475c3bdd75b7341d31640ad395b2054f34d7d02f5d20c9d50); /* statement */ 
_setMarginPremium(state, marketId, marginPremium);
    }

    function ownerSetSpreadPremium(
        Storage.State storage state,
        uint256 marketId,
        Decimal.D256 memory spreadPremium
    )
        public
    {coverage_0xd6b9804f(0x1f33905e4197ed6783665c0a5601798b8590a8187299c99b272a2f1e8362f495); /* function */ 

coverage_0xd6b9804f(0x6ed56ba7d6aae2806921c1bb72ff86465168c4603a91bf6c0f736a2f1e255561); /* line */ 
        coverage_0xd6b9804f(0xc38add5357d9b38dabf131721228398990675409f1bebbfcda455af3626fb85c); /* statement */ 
_validateMarketId(state, marketId);
coverage_0xd6b9804f(0xea8a46495a42f25541e1a489e5c7393946ad473a6b38e146d349d134cd1c7a38); /* line */ 
        coverage_0xd6b9804f(0xa98c14a6fd4d3e47faa4bfe2c5427e176fb12d10e3a0b904e504cef0d2ebefd3); /* statement */ 
_setSpreadPremium(state, marketId, spreadPremium);
    }

    // ============ Risk Functions ============

    function ownerSetMarginRatio(
        Storage.State storage state,
        Decimal.D256 memory ratio
    )
        public
    {coverage_0xd6b9804f(0x30553a937b54efbb4d7f716d7e9fd17643385d42fcc981c8af7bcf6cf2ac9bfd); /* function */ 

coverage_0xd6b9804f(0xddc81654a09f498bae0363e6e7c111fab4276f9fd4bd52c0f8eed8add58795ea); /* line */ 
        coverage_0xd6b9804f(0x155ca493756cfb3ede6c7b363c1512c78354b0d57946bcd454afc5d6c0d96efc); /* statement */ 
Require.that(
            ratio.value <= state.riskLimits.marginRatioMax,
            FILE,
            "Ratio too high"
        );
coverage_0xd6b9804f(0xf26d0a5dcd6ab1a3a194bf03a3ad98cdb56eb913fd1d336980f64001b62a3b87); /* line */ 
        coverage_0xd6b9804f(0x33cc283ac2e35a71ac4fadbe9ccf8f0f098a84d7a96b7f2430854f08edec9f67); /* statement */ 
Require.that(
            ratio.value > state.riskParams.liquidationSpread.value,
            FILE,
            "Ratio cannot be <= spread"
        );
coverage_0xd6b9804f(0x28f1435123aedda3c406e1be47a041debd4774c8518d6db8fe44b40c014d83a7); /* line */ 
        coverage_0xd6b9804f(0x73df1398fda812f810bcf3e7c6f012c34ef7a1549a917dd4576f4eddc2e20827); /* statement */ 
state.riskParams.marginRatio = ratio;
coverage_0xd6b9804f(0xb89a7ad59dfa293b7671eced2e238c694789ef9d64b8f165b457287ac4a19f00); /* line */ 
        coverage_0xd6b9804f(0x68fdb88bc787e4028c84780ac5f6c274a2a99b49c3c1ba7d078eeeff6432f74a); /* statement */ 
emit LogSetMarginRatio(ratio);
    }

    function ownerSetLiquidationSpread(
        Storage.State storage state,
        Decimal.D256 memory spread
    )
        public
    {coverage_0xd6b9804f(0xde0b1430477e4cecdf42fd72fb2a832db59645db8b61d7aa039089dc36516796); /* function */ 

coverage_0xd6b9804f(0x05261a5b70fc02d59b7cdbd3095b07b76e61ef4082533b694f4a09cfcec89bf4); /* line */ 
        coverage_0xd6b9804f(0xeb7d3740fbda1ab067579e746b3d2a948eac72f947c9c60f03d810a1a58eff95); /* statement */ 
Require.that(
            spread.value <= state.riskLimits.liquidationSpreadMax,
            FILE,
            "Spread too high"
        );
coverage_0xd6b9804f(0x7e3c5e03be5d834cb6aa3a0832506f248903d90ef16cb0fa6a39cb8ed9972680); /* line */ 
        coverage_0xd6b9804f(0x37f5c46fbaa66c094e180ae2b7dfb0ccd32d32f7bec1d1a9e7adc52e54c12719); /* statement */ 
Require.that(
            spread.value < state.riskParams.marginRatio.value,
            FILE,
            "Spread cannot be >= ratio"
        );
coverage_0xd6b9804f(0x301b3010e3fc10dd25d5efe64aa3e4747025a095af2cb1156e9f1184db1796c4); /* line */ 
        coverage_0xd6b9804f(0x20d9b84ef3bad40c4f3b6910a3bd3851e39acd4faf757ba6409cd9859ddc65ca); /* statement */ 
state.riskParams.liquidationSpread = spread;
coverage_0xd6b9804f(0x468235311a8b83a2e66784b0201d7ed1577002019f06753af4653fb4f150163a); /* line */ 
        coverage_0xd6b9804f(0xfb6c2bcbbeca1076255020c27bc07e2962a1b079eca763ddeb5155fd92b324da); /* statement */ 
emit LogSetLiquidationSpread(spread);
    }

    function ownerSetEarningsRate(
        Storage.State storage state,
        Decimal.D256 memory earningsRate
    )
        public
    {coverage_0xd6b9804f(0x086c61e1498cc699d3bcfa9df91ef8b969a1eb34d5653f2e0adc46b0ba9726b1); /* function */ 

coverage_0xd6b9804f(0x4ae1983150bf887332887ee8908e122e8f03427fc42953a5595f74d7ccc0e1f8); /* line */ 
        coverage_0xd6b9804f(0x4aae0bcef3ac0aefe0a68901ffbfaa26af1d6e0fc21b4c98bd0a00ad9518756b); /* statement */ 
Require.that(
            earningsRate.value <= state.riskLimits.earningsRateMax,
            FILE,
            "Rate too high"
        );
coverage_0xd6b9804f(0x2a66510295dce82f42bf1777d69f618d3fa6a3e166bc0738b890f9700335039b); /* line */ 
        coverage_0xd6b9804f(0x6dc9448b496a16c8ca1798a09a93559e6837a1345a0448e7c7edb029bf370782); /* statement */ 
state.riskParams.earningsRate = earningsRate;
coverage_0xd6b9804f(0x7ee1f28468152f136e8798e1c2e958df305128168adee1c454cc8ef61ca0ba01); /* line */ 
        coverage_0xd6b9804f(0x403c2730b919a6cc392a2c56be1bc89ceaa3398377d748a04148471748ff357d); /* statement */ 
emit LogSetEarningsRate(earningsRate);
    }

    function ownerSetMinBorrowedValue(
        Storage.State storage state,
        Monetary.Value memory minBorrowedValue
    )
        public
    {coverage_0xd6b9804f(0xc61aacf1a1932942ae93b9efe515699ad809346e737b5aee8cca5f76370d84a8); /* function */ 

coverage_0xd6b9804f(0x218f79baaaabd16c449d17f2826c135dbc838ef5ae1f840e624dbec68e740cbb); /* line */ 
        coverage_0xd6b9804f(0x784d5495a6b1fefb5edd4b9dc558c75a0a6f0a4124dcb5ae28d34f88aecf37a7); /* statement */ 
Require.that(
            minBorrowedValue.value <= state.riskLimits.minBorrowedValueMax,
            FILE,
            "Value too high"
        );
coverage_0xd6b9804f(0xbf26110371b2112f100c0c7badca32adfb75c34c2107cc40de7cc45f1aee745f); /* line */ 
        coverage_0xd6b9804f(0x978e73ac3d1a01e08e9209586b7374e36ab863acfca25e015c292cf5ead7c008); /* statement */ 
state.riskParams.minBorrowedValue = minBorrowedValue;
coverage_0xd6b9804f(0xf2409e66a8baf6c5cf8a3477b64d1440392751fd2d323079bc73f876a8f688b5); /* line */ 
        coverage_0xd6b9804f(0x27389e8d8914eb902ffdaa05793b5f1bf4d7f153e82d92137cda6973a0aa9dbc); /* statement */ 
emit LogSetMinBorrowedValue(minBorrowedValue);
    }

    // ============ Global Operator Functions ============

    function ownerSetGlobalOperator(
        Storage.State storage state,
        address operator,
        bool approved
    )
        public
    {coverage_0xd6b9804f(0xc20ace0de3ac6adb3c78365a1725f25ba09155691435477fca7c7573dc59fdb1); /* function */ 

coverage_0xd6b9804f(0xb2c9db1fc31f215c42e51aa9edf9655317dabfef7b212eb83db9d04322ec4076); /* line */ 
        coverage_0xd6b9804f(0x9f365b6de9bf6d4e798dae54078838d386761baa89a705581b72110a3d5ca195); /* statement */ 
state.globalOperators[operator] = approved;

coverage_0xd6b9804f(0xa44d86fe2c69a7da198926897597f1c506037c900628ab8bdfab27cbce26956b); /* line */ 
        coverage_0xd6b9804f(0xf848d93ade378b582d4f8017f543d5c4d9141519ba1b89c1acda31bdaf666cbb); /* statement */ 
emit LogSetGlobalOperator(operator, approved);
    }

    // ============ Private Functions ============

    function _setPriceOracle(
        Storage.State storage state,
        uint256 marketId,
        IPriceOracle priceOracle
    )
        private
    {coverage_0xd6b9804f(0xe947587b7e02eb23037cf992af93d24ecaba6cc04f3cbec46ce8b00a711c8959); /* function */ 

        // require oracle can return non-zero price
coverage_0xd6b9804f(0xbb524f78618c34b569bb921d7d59115f1655185847fec42b5e07c65eb5e2d14d); /* line */ 
        coverage_0xd6b9804f(0x58f52cb7bdeb69bc530e65683e1843339a90ddeb0c86fcc517f6b83751c737ab); /* statement */ 
address token = state.markets[marketId].token;

coverage_0xd6b9804f(0x267a1db755048603f5ca960179559980f0a45141869094a48b0acf3e559c934c); /* line */ 
        coverage_0xd6b9804f(0x3b08e5a9e3783122b41009aa86230fabfbb57b60490c4e50ac910a2f8be98f32); /* statement */ 
Require.that(
            priceOracle.getPrice(token).value != 0,
            FILE,
            "Invalid oracle price"
        );

coverage_0xd6b9804f(0xa4c7807961e74907fdbe70508b777a9d3d8095d3240fc15a515a1e473974a117); /* line */ 
        coverage_0xd6b9804f(0x1052fb5b116b0cca97f7af66f0f8aa59a7f288c02dfc1ea9ff06c120c79cd596); /* statement */ 
state.markets[marketId].priceOracle = priceOracle;

coverage_0xd6b9804f(0x338eb986a9fb05cb0f4fcf0c422a727fce4f8af68473881470f52c866bea762d); /* line */ 
        coverage_0xd6b9804f(0xb8ce6a3537047d8e97d37cf70c52d5dce41898872cc7e2e403f0d9c129934cff); /* statement */ 
emit LogSetPriceOracle(marketId, address(priceOracle));
    }

    function _setInterestSetter(
        Storage.State storage state,
        uint256 marketId,
        IInterestSetter interestSetter
    )
        private
    {coverage_0xd6b9804f(0x80ae2ed78fd0e5d517089b223c04105ac12bbf501abe358c6520ee8a94d89f35); /* function */ 

        // ensure interestSetter can return a value without reverting
coverage_0xd6b9804f(0x98bc5a4282f7fc290f6e854af489997a7fe0319545ffdbc118516346ca972704); /* line */ 
        coverage_0xd6b9804f(0x3c9bea3d31c999d383c9a2a5be7525e8ab967f69b3b0a277680727c4bbb8208b); /* statement */ 
address token = state.markets[marketId].token;
coverage_0xd6b9804f(0xf51a97c5f7b2ac9dffeaaffe237488d65965a761660cabefc1bed59c5b27b5bd); /* line */ 
        coverage_0xd6b9804f(0x337d3804112a102f9aef2a1ed2e18e690981a79aca85ceab193e4646495801c9); /* statement */ 
interestSetter.getInterestRate(token, 0, 0);

coverage_0xd6b9804f(0xae3302def421bd61fbab0180365b557b393be101add3f74b3a0c50337c78316c); /* line */ 
        coverage_0xd6b9804f(0x2b2c4ed221dbcd36e326d73c18a48ce651268d21e16bc6153f8e12eed5b32905); /* statement */ 
state.markets[marketId].interestSetter = interestSetter;

coverage_0xd6b9804f(0x77e59bf14e5eb4f47db6c98edea703b8ca45dbdc0cc31f4781e88d6be136f1d3); /* line */ 
        coverage_0xd6b9804f(0xfcc39176e4dbcad0c1a7fc45aacb0b728e3042f99fe0a2ea44d961bdb402ebb1); /* statement */ 
emit LogSetInterestSetter(marketId, address(interestSetter));
    }

    function _setMarginPremium(
        Storage.State storage state,
        uint256 marketId,
        Decimal.D256 memory marginPremium
    )
        private
    {coverage_0xd6b9804f(0xaff872ae4bcc774918af7d31cf4288955a619917656699c7339b82be25774773); /* function */ 

coverage_0xd6b9804f(0xa0eba6d405c247a36409e61bc4673acb23d0724000c2f67736f97c9f2e58f0e7); /* line */ 
        coverage_0xd6b9804f(0x9d09cb6282351a1cd4b0e39823871c197a0171d8fd0987af347fd8f29e405565); /* statement */ 
Require.that(
            marginPremium.value <= state.riskLimits.marginPremiumMax,
            FILE,
            "Margin premium too high"
        );
coverage_0xd6b9804f(0xf41b6f807cc37f6a13f60b506867444060258254e933d7370bdb539b3a7e560e); /* line */ 
        coverage_0xd6b9804f(0x8224530a5c901ce4f3d28d2866fbdee94ffa9d20c6c1028c66ddb101f8b51004); /* statement */ 
state.markets[marketId].marginPremium = marginPremium;

coverage_0xd6b9804f(0x2ab5b38a09e79424d459d6ef337e1d8417204d21564c466650993cc1bb0e91dc); /* line */ 
        coverage_0xd6b9804f(0xa315bfc8bb3f7af08cb5e09556bb99590415b98b24839799c55dea661d258695); /* statement */ 
emit LogSetMarginPremium(marketId, marginPremium);
    }

    function _setSpreadPremium(
        Storage.State storage state,
        uint256 marketId,
        Decimal.D256 memory spreadPremium
    )
        private
    {coverage_0xd6b9804f(0x1f1c364896a36a02faec688a0ceb7b61341d3132b0132539f18651761929768c); /* function */ 

coverage_0xd6b9804f(0xedc3dfe085c9e1722c2190d6c2213354684c073fd7cea6e58f4bae424a1c8992); /* line */ 
        coverage_0xd6b9804f(0x186d19a60b8bfc0dda59a7cd9bead23c4060758a2dcbafd40f0cd505cefbdfc5); /* statement */ 
Require.that(
            spreadPremium.value <= state.riskLimits.spreadPremiumMax,
            FILE,
            "Spread premium too high"
        );
coverage_0xd6b9804f(0x8ef65e8a6a3d5ed51dc81d7402876fadab2b44f57a32a9f8e29c6e909615ddc9); /* line */ 
        coverage_0xd6b9804f(0x924c37769d8f658d80ec0c9c3ae39f759d4478f5188a7bd959389848a3a285d5); /* statement */ 
state.markets[marketId].spreadPremium = spreadPremium;

coverage_0xd6b9804f(0x3b1710877010a820cffd0e6222cd26d9faad2f9918f12bf7017acc402054d614); /* line */ 
        coverage_0xd6b9804f(0x10f496476d4856982c38890a8d0cd8045ecd312fbb68623cdc3f9f657c8cad99); /* statement */ 
emit LogSetSpreadPremium(marketId, spreadPremium);
    }

    function _requireNoMarket(
        Storage.State storage state,
        address token
    )
        private
        view
    {coverage_0xd6b9804f(0x512ab6b228c3f135b40cff74aa3d276753bd588026ec4ff14aa7738cc8a938fb); /* function */ 

coverage_0xd6b9804f(0xb93983546890ba598e81f542773380decccb40c01f231cbf332ef399ac4dcbec); /* line */ 
        coverage_0xd6b9804f(0x77d5cf73c05c370e5cc529e34ad8c23bdf8c83eb0d40b855580b89b4e88f264b); /* statement */ 
uint256 numMarkets = state.numMarkets;

coverage_0xd6b9804f(0x3ebd246f6384f4060e60be385b111472d6d5209c19d0634b58f784266d0714f1); /* line */ 
        coverage_0xd6b9804f(0x76fe90746a05b50daa14af876f9169096fb6fb7990b768c45876900f89bd32c7); /* statement */ 
bool marketExists = false;

coverage_0xd6b9804f(0x31385c6f87d1b2d785caa64eddfcd8b8c16a834412d558046c19516a5885a201); /* line */ 
        coverage_0xd6b9804f(0x7635a008eec8be69a26e29f99134accd1146d3a4356314bf38a99dfc0030f62d); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0xd6b9804f(0x43376eb257ca7e268799ba358da40c140968a203cd1af2c5e84fe3d3a2154d52); /* line */ 
            coverage_0xd6b9804f(0xc15e47386ad07e677e9a01193c94ef625c573dbab675dc5edd95fa1385f28e1a); /* statement */ 
if (state.markets[m].token == token) {coverage_0xd6b9804f(0xbc47872ff65c2ded810892902a8f4a164a865b598ecc1241955f0af5b0651bbd); /* branch */ 

coverage_0xd6b9804f(0x63377a70ebc8eb54b0944209b927c505c13a871c0460816f5e6c1ebe28068ef0); /* line */ 
                coverage_0xd6b9804f(0xe3241c01ac65f1c40ead68eaf6290c216fe154c502ed4ab19c9d34ec9900eaf4); /* statement */ 
marketExists = true;
coverage_0xd6b9804f(0x0d0499f49959d2b970a0921eec3170f6960a072cdeba39c586bd03d4d0400bd5); /* line */ 
                break;
            }else { coverage_0xd6b9804f(0x12e265d91d20540985a66f3592e3a9d4503b46768ace9431dec9ae6689745ef1); /* branch */ 
}
        }

coverage_0xd6b9804f(0x1825f11fed5b5b4be742ce84f814bf7a01e82164c920f8e4e43cd878848da728); /* line */ 
        coverage_0xd6b9804f(0xcca8dc6c2285ee45dac1c0c0355d542ecb04c5259b5b2d1e5da3437d46da075f); /* statement */ 
Require.that(
            !marketExists,
            FILE,
            "Market exists"
        );
    }

    function _validateMarketId(
        Storage.State storage state,
        uint256 marketId
    )
        private
        view
    {coverage_0xd6b9804f(0xc1a31378099ea64090a9547a4e3c9c4d0d1d3ab5ff34d2f53894002b80e580dc); /* function */ 

coverage_0xd6b9804f(0xcdded7ae5aa3574447a45c54d611f9db67763416d17f2880ad78c2c9d37fbf44); /* line */ 
        coverage_0xd6b9804f(0x5e99fcd5f4883ed02d120f7b134bf8ed41706f9b357efc360339d67dce6bb072); /* statement */ 
Require.that(
            marketId < state.numMarkets,
            FILE,
            "Market OOB",
            marketId
        );
    }
}
