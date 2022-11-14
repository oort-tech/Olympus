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

import { IAutoTrader } from "../protocol/interfaces/IAutoTrader.sol";
import { Account } from "../protocol/lib/Account.sol";
import { Actions } from "../protocol/lib/Actions.sol";
import { Math } from "../protocol/lib/Math.sol";
import { Require } from "../protocol/lib/Require.sol";
import { Time } from "../protocol/lib/Time.sol";
import { Types } from "../protocol/lib/Types.sol";


/**
 * @title TestAutoTrader
 * @author dYdX
 *
 * IAutoTrader for testing
 */
contract TestAutoTrader is
    IAutoTrader
{
function coverage_0xf0fc8bb4(bytes32 c__0xf0fc8bb4) public pure {}

    // ============ Constants ============

    bytes32 constant FILE = "TestAutoTrader";

    // ============ Events ============

    event DataSet(
        uint256 indexed input,
        Types.AssetAmount output
    );

    event DataDeleted(
        uint256 indexed input
    );

    // ============ Storage ============

    // input => output
    mapping (uint256 => Types.AssetAmount) public data;
    mapping (uint256 => bool) public valid;

    uint256 public requireInputMarketId;
    uint256 public requireOutputMarketId;
    Account.Info public requireMakerAccount;
    Account.Info public requireTakerAccount;
    Types.Par public requireOldInputPar;
    Types.Par public requireNewInputPar;
    Types.Wei public requireInputWei;

    // ============ Testing Functions ============

    function setData(
        uint256 input,
        Types.AssetAmount memory output
    )
        public
    {coverage_0xf0fc8bb4(0x091aaa982cf7563e1255488b40220ded6221ae867f9a821653cab9eabbe44b0d); /* function */ 

coverage_0xf0fc8bb4(0x36030743b85dc923f8e6e1af2f388e2472726f9f97bce3db1d23ea42776bd36f); /* line */ 
        coverage_0xf0fc8bb4(0x10eb93f1e84b5f4f232a4834d15c952afc3734cd72af7a5667a8b3b115c75cb3); /* statement */ 
setDataInternal(input, output);
    }

    function setRequireInputMarketId(
        uint256 inputMarketId
    )
        public
    {coverage_0xf0fc8bb4(0xe97c44b05f34d21d2822cbe68297315f0d44d30c13ff838f738f7c6a0f53a851); /* function */ 

coverage_0xf0fc8bb4(0xa4684231f9a8f5025d4ee51384fa818cdcaec78963ea99c892b18fb1838f10e3); /* line */ 
        coverage_0xf0fc8bb4(0xcd81f74a582ffda66056f4cc81e6d6f8b62c8e5bf09c739fd5c820a4ec4cceb8); /* statement */ 
requireInputMarketId = inputMarketId;
    }

    function setRequireOutputMarketId(
        uint256 outputMarketId
    )
        public
    {coverage_0xf0fc8bb4(0x4c872f58012ed07f75e7e251d1bf38c652489f99a00717a322b7b769f093ac8e); /* function */ 

coverage_0xf0fc8bb4(0x7f1b0ecdf617752ca33e4f0734bdc3d9218e36f431906f94378f576816bc8a7c); /* line */ 
        coverage_0xf0fc8bb4(0x89de0b02063ddb7ae7a57e569b42215478c5ca8cde2d5a653e4f4e28b23be298); /* statement */ 
requireOutputMarketId = outputMarketId;
    }

    function setRequireMakerAccount(
        Account.Info memory account
    )
        public
    {coverage_0xf0fc8bb4(0xfa5d8d29a9eb073389790a5b1bf2475ce108f6e5512dc861fa4bdcd0526c2d7f); /* function */ 

coverage_0xf0fc8bb4(0x498656ea95464f6ee40d4c98354b74d072cc8bd765cad9f7c9280f4f49235362); /* line */ 
        coverage_0xf0fc8bb4(0x83874aa80aac6dbf4794816ea8b2a44a401d8e87f7f3f2a69e42e7793035f257); /* statement */ 
requireMakerAccount = account;
    }

    function setRequireTakerAccount(
        Account.Info memory account
    )
        public
    {coverage_0xf0fc8bb4(0x84513d9ed06a6e5c949ddef9ab61701e10fe02d588aad0335db94fcdeeef5979); /* function */ 

coverage_0xf0fc8bb4(0xcbaac1293ac4128298c5349e210d8bc077bf1ce4a03ac808d244a633fee7cae4); /* line */ 
        coverage_0xf0fc8bb4(0x272b46a86b7bbf403c66c406b3fd0cc4c8e767b857faca06fb68711f6794ad67); /* statement */ 
requireTakerAccount = account;
    }

    function setRequireOldInputPar(
        Types.Par memory oldInputPar
    )
        public
    {coverage_0xf0fc8bb4(0xa0359e76dd8d54aa3ad8d83b561afad1a4646204baa66c92f46104e4756e2f87); /* function */ 

coverage_0xf0fc8bb4(0x6183fdcf3ee2b4b9e790e3df1e0eee0b047b3d8df7a8bcc7c2c760c47b9909d3); /* line */ 
        coverage_0xf0fc8bb4(0xbb21d5ce1aaeaafce3a2d82139d09b666fc5534cfe8c9aeb43c0578b632a7616); /* statement */ 
requireOldInputPar = oldInputPar;
    }

    function setRequireNewInputPar(
        Types.Par memory newInputPar
    )
        public
    {coverage_0xf0fc8bb4(0xf121be9f667eea3028e43be444513818a79b4d38d8189687f5099346842efe1a); /* function */ 

coverage_0xf0fc8bb4(0x94783727e0123f87b8bef628a7447803e0025d26d583a80fd0136fc54fcdbdd3); /* line */ 
        coverage_0xf0fc8bb4(0x6cdbfeaaa36d6798d38a2f0962fd294ffeb1eb2cef60a9b6b0e6ebb752395c63); /* statement */ 
requireNewInputPar = newInputPar;
    }

    function setRequireInputWei(
        Types.Wei memory inputWei
    )
        public
    {coverage_0xf0fc8bb4(0xf8cbd27f6920378efdeadd3493bb7d4ada7ae7e14313ae143dfd12b53758361f); /* function */ 

coverage_0xf0fc8bb4(0x16911f0bf774e7e55204e6cc6e919405136fe4f66de4a5e32606d9cd2101b225); /* line */ 
        coverage_0xf0fc8bb4(0xc8b5df103d248670d170905e4d91fdf7bf424a04866fc139e1700bcee4d99d5d); /* statement */ 
requireInputWei = inputWei;
    }

    // ============ AutoTrader Functions ============

    function getTradeCost(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory makerAccount,
        Account.Info memory takerAccount,
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.Wei memory inputWei,
        bytes memory tradeData
    )
        public
        returns (Types.AssetAmount memory)
    {coverage_0xf0fc8bb4(0x9a657fdf6cdbeeb255160ab038bc104345f57b19e681fdd6fabea4ec3fc9f193); /* function */ 

coverage_0xf0fc8bb4(0x1677100b4c6a4336b5f2c4a4081051593991bd7ea1bcc59d1040f0aee5d80b7b); /* line */ 
        coverage_0xf0fc8bb4(0x041ab65f1a858d26e042c04c75bb280b786a1a48d1738158125c718bf0bd4fc6); /* statement */ 
if (requireInputMarketId != 0) {coverage_0xf0fc8bb4(0x6ffc98b7efba69ba545133bd0edcfbc7973f18fc1545de4de614b640e7cfd109); /* branch */ 

coverage_0xf0fc8bb4(0x716d3301e523fb795cb6c9e776d6de26bd091d12e6e46ace681747ceb35c074c); /* line */ 
            coverage_0xf0fc8bb4(0x3e3fc8000edf71d429ed865def0cd2287bd4699c71ea9dcba372f40eb69402f7); /* statement */ 
Require.that(
                requireInputMarketId == inputMarketId,
                FILE,
                "input market mismatch"
            );
        }else { coverage_0xf0fc8bb4(0x963c17c836c58c248b03ee695e8abecd36c46655f0beb6c78e77338bfa3a91ba); /* branch */ 
}
coverage_0xf0fc8bb4(0x234f3b53feca35583d8ad9bcb58e27b9ea2fba362a0895bcde98aa55f6335e69); /* line */ 
        coverage_0xf0fc8bb4(0xfa8eea35f28c292ecd642691499955bcb4164b80953b3a77dee8b9dd7498111b); /* statement */ 
if (requireOutputMarketId != 0) {coverage_0xf0fc8bb4(0xc6d4c480e5dafc7db81116fac54eca8916a3b0c9a4c8132d1e86a6f2a5281b93); /* branch */ 

coverage_0xf0fc8bb4(0x1743f13d15b78c70a7df8dcc3d462cf425d662675b8bd08840376188070b3550); /* line */ 
            coverage_0xf0fc8bb4(0xe6c37cb7b5fdbb1221c540e0af821084c41806eacff1aadd73719f092c1a6e9c); /* statement */ 
Require.that(
                requireOutputMarketId == outputMarketId,
                FILE,
                "output market mismatch"
            );
        }else { coverage_0xf0fc8bb4(0xe34bd88a836be5af60d9154053438c40682c7118cd0ad54caf8158e41df20cd8); /* branch */ 
}
coverage_0xf0fc8bb4(0xc22bf7db5ce480c8fe1a0ae9af25ca98a23513f258e876f0552f4bb95415c0ed); /* line */ 
        coverage_0xf0fc8bb4(0x2a6337487907b1f3109765f37daaccbdaeae79ae175d7b4cfd85a00eb406cf91); /* statement */ 
if (requireMakerAccount.owner != address(0)) {coverage_0xf0fc8bb4(0x1552310df1b9d24f841d99457d8055b6f426fc2684a023771c8cd9a13c26a407); /* branch */ 

coverage_0xf0fc8bb4(0xf0a0bf4bcfe8f08d5750e5509d6ee7005f873969d26ff8840db6043c1f6546de); /* line */ 
            coverage_0xf0fc8bb4(0x0a50bb34d1722019f0c75e45fda961543fae848c4118eb16bd620a6843502d39); /* statement */ 
Require.that(
                requireMakerAccount.owner == makerAccount.owner,
                FILE,
                "maker account owner mismatch"
            );
coverage_0xf0fc8bb4(0xe02bb96a7d4d16ecf13690028cbb7b33c57e3df237a01b11c7736b3ba72450a5); /* line */ 
            coverage_0xf0fc8bb4(0xd557105452fb147b225e939b76b324c69d7c8a15bd119488a3f0a48f10f56cc1); /* statement */ 
Require.that(
                requireMakerAccount.number == makerAccount.number,
                FILE,
                "maker account number mismatch"
            );
        }else { coverage_0xf0fc8bb4(0x6c10170b22d4404667b28605cd933b001f591ce7204190ba7f143a413f17ea08); /* branch */ 
}
coverage_0xf0fc8bb4(0x50793b052818c270c3d15b0cec2e69c8c58e6c9c7aa0846135ae4881913c2b83); /* line */ 
        coverage_0xf0fc8bb4(0xfc95be1e2784713072d2f7ec8464370711fd47ca3570fbfdcd415b8d4efc4b33); /* statement */ 
if (requireTakerAccount.owner != address(0)) {coverage_0xf0fc8bb4(0x4fa830b8e81eadf68c2ff9079b6b9b1eaf93fbaa981b275d1391437c646a4127); /* branch */ 

coverage_0xf0fc8bb4(0x2631f3dc948dda33f876e9d3689d950ac3e2f001f741bf26d81f49a9a935f814); /* line */ 
            coverage_0xf0fc8bb4(0xcf9de7f767393a014983a2667f52c83baacecf2672c850675492dd05d195e5c2); /* statement */ 
Require.that(
                requireTakerAccount.owner == takerAccount.owner,
                FILE,
                "taker account owner mismatch"
            );
coverage_0xf0fc8bb4(0x9fab3248c2714f9ade121e264d54f223a3743ab6f8543415b2f6adcb337ca507); /* line */ 
            coverage_0xf0fc8bb4(0x7afcb84772b8d8b8232e4f42c5f68e0ae9e1ff5d4e873cc365e12575b036722a); /* statement */ 
Require.that(
                requireTakerAccount.number == takerAccount.number,
                FILE,
                "taker account number mismatch"
            );
        }else { coverage_0xf0fc8bb4(0x4e29e7e18636e0be74c14e862e6ebc271466668445ce0fb4647d4929dfa7e8de); /* branch */ 
}
coverage_0xf0fc8bb4(0xa4c8a9d7f5175614a1cbdd687ea425fd369fb0558aaffdb945f3170dc74ffbd5); /* line */ 
        coverage_0xf0fc8bb4(0x731e5fc813e125c67c88388ca89bbfcb3ce3e585cc7c1651fac2eaa4a7c736bc); /* statement */ 
if (requireOldInputPar.value != 0) {coverage_0xf0fc8bb4(0x349edac7e910f829f055006726842661a1eb086fbda56d8615b9119ca896befd); /* branch */ 

coverage_0xf0fc8bb4(0xa28b87975b5b1591697fc90a14ebe83d23f631e1205baf6b1a3c888b36b19eac); /* line */ 
            coverage_0xf0fc8bb4(0x9cffe35dbd32aa55dc7bd9f46a88f896661e85815f5768913c5c49f59160b899); /* statement */ 
Require.that(
                requireOldInputPar.sign == oldInputPar.sign,
                FILE,
                "oldInputPar sign mismatch"
                );
coverage_0xf0fc8bb4(0x2cd3f251195de9a79b6b308364e8892b10b0eb4163f972d3e175b96763ed78b5); /* line */ 
            coverage_0xf0fc8bb4(0x42eb7aaafbdb10d54548f2a532f28fd7d89b85bcb371616cf1ad3328a0b62fe0); /* statement */ 
Require.that(
                requireOldInputPar.value == oldInputPar.value,
                FILE,
                "oldInputPar value mismatch"
                );
        }else { coverage_0xf0fc8bb4(0xe4a43952a10652d79cae0ceea853cf17f2da4149495715fb0c913e2961876a4e); /* branch */ 
}
coverage_0xf0fc8bb4(0x0ac73c5714210978102549408f2299624e631c096da7c8b1b2280e46555b8262); /* line */ 
        coverage_0xf0fc8bb4(0x8dcc3255e4100fb1d7a1963bd503951f434dd8b4c1c5dfcc81c270a9c4a5532e); /* statement */ 
if (requireNewInputPar.value != 0) {coverage_0xf0fc8bb4(0xb35ce09fc9c66b2a47f38f9b0862dfbfdd4229dde25eb594246b4f7ed40d1220); /* branch */ 

coverage_0xf0fc8bb4(0x75f3978cc78acda317c3de235935e9b8abf41080a5b9cc58bee5118ec20e41ae); /* line */ 
            coverage_0xf0fc8bb4(0x3657ac991b208de86968b905a82f40767f8e9a18de64f4ca8dd55a5a2a071891); /* statement */ 
Require.that(
                requireNewInputPar.sign == newInputPar.sign,
                FILE,
                "newInputPar sign mismatch"
            );
coverage_0xf0fc8bb4(0x9ae33b05ce6670d98f9b2e54ba3baa4d89de5aae6b5d302d04369312af3a427e); /* line */ 
            coverage_0xf0fc8bb4(0x9f9c2c1176b8f5840353bd30148c80e425358f220e16b6ea9f76a4253d28a372); /* statement */ 
Require.that(
                requireNewInputPar.value == newInputPar.value,
                FILE,
                "newInputPar value mismatch"
            );
        }else { coverage_0xf0fc8bb4(0x910d533bf65a12bcec85852d5b1edc9f2722375a48af89b1557335ee997d1bd4); /* branch */ 
}
coverage_0xf0fc8bb4(0xe32f0c7cb036c40b01fd03d2d308522a96e084b6e3685923dffa344a81da81d3); /* line */ 
        coverage_0xf0fc8bb4(0xc74a4f3cd0577b1d01080799309438b418aa7aa0a6f879ddfad1aba49b20dca7); /* statement */ 
if (requireInputWei.value != 0) {coverage_0xf0fc8bb4(0x0da27ff32094877d3600ff5ddb38491b0fab6b636285ef1ba5678b89b2fe7840); /* branch */ 

coverage_0xf0fc8bb4(0xf507d5affb9ee8e07229cbbd514e3bf3e99a9a2541abeaaba8a93957c4f02d24); /* line */ 
            coverage_0xf0fc8bb4(0x4a569d27403cd5194f3f7a90c0b03285bd821e0f9a6850180347fb057a0e4efe); /* statement */ 
Require.that(
                requireInputWei.sign == inputWei.sign,
                FILE,
                "inputWei sign mismatch"
            );
coverage_0xf0fc8bb4(0x87010354f91d7839b5dfafebce139ba2ae59da4fc809bc0ae7aa9a76540bc043); /* line */ 
            coverage_0xf0fc8bb4(0xa43f9995a312ebbf37cb37750a5a7815e213e08520be04d1b2c6881eeab53878); /* statement */ 
Require.that(
                requireInputWei.value == inputWei.value,
                FILE,
                "inputWei value mismatch"
            );
        }else { coverage_0xf0fc8bb4(0x98e0f8a1dd6980e9fdb001aa7099d534e3c1884c9ff0e2bcd733827104611227); /* branch */ 
}

coverage_0xf0fc8bb4(0x40665774bb5281f1ccf57d3d49859c1c9ddfb7bba94cd32aed883389f124f344); /* line */ 
        coverage_0xf0fc8bb4(0x187cbe73bcbb5e87fe7d79bd751a1f7e7e08e3ee8bfbd5f514888f0721b353c2); /* statement */ 
uint256 input = parseTradeData(tradeData);
coverage_0xf0fc8bb4(0xa85d740d91f769c698ac551cfa8d6a332d14fc2c6ed2be67e16cefabcf102ff4); /* line */ 
        coverage_0xf0fc8bb4(0x90fe97b6dc243db9c1b2115e40a6f28d859dd0d2e1a7e3ed506779166c8cd8f3); /* statement */ 
return deleteDataInternal(input);
    }

    // ============ Private Functions ============

    function setDataInternal(
        uint256 input,
        Types.AssetAmount memory output
    )
        private
    {coverage_0xf0fc8bb4(0x5a00e75d966a7f84780583fb0820ce6356701013086dcb8779a99538572b653c); /* function */ 

coverage_0xf0fc8bb4(0xd3fb81381236e9e378cfdd17b8387c1ccfd88dfb9478aa6093ecfd3a54e869c4); /* line */ 
        coverage_0xf0fc8bb4(0x3f61c2687721c6b6b584488bf697e37de5fc1b361d81ed64d0e7bec262455621); /* statement */ 
data[input] = output;
coverage_0xf0fc8bb4(0x481094ad5aad968e8f3b3c055a3e8287fcacde874a96ee9eecfddd912cbad6ea); /* line */ 
        coverage_0xf0fc8bb4(0xeffad51a0e941766e06140b0043d95ce592d0de795e86e8eac4533942c907b90); /* statement */ 
valid[input] = true;
coverage_0xf0fc8bb4(0x73cf35e44b8007bf2fe62727212c6bd020cf4efb8b57e3331fc6b14ba860cff5); /* line */ 
        coverage_0xf0fc8bb4(0x00ceb9cdc84baeefa55024b104b96b5206524e5526be372187d50520d30bf665); /* statement */ 
emit DataSet(input, output);
    }

    function deleteDataInternal(
        uint256 input
    )
        private
        returns (Types.AssetAmount memory)
    {coverage_0xf0fc8bb4(0xdec9515b744daa84921a39f2be439831c7e78fe86662f31265604519fd833b46); /* function */ 

coverage_0xf0fc8bb4(0xb3ecece2ca1e20f2e7fb0d30caa10ddea1b3dd8abe3c806d7bab5ea604e39c70); /* line */ 
        coverage_0xf0fc8bb4(0xa97ee3f986d144cbe3ab2aa9764a2382d004503953e5eb69efe280fda892ca0c); /* statement */ 
Require.that(
            valid[input],
            FILE,
            "Trade does not exist"
        );
coverage_0xf0fc8bb4(0x6ab5d79f83cf1d2745975dc1fbfaf373e77be85f5db985b8317362bae6146643); /* line */ 
        coverage_0xf0fc8bb4(0xd8843ad912cf5cba495fb1fddbfe1930262fc2a15186c6b9b96f2c887f161ab2); /* statement */ 
Types.AssetAmount memory output = data[input];
coverage_0xf0fc8bb4(0x2b9544bf9a0aa13fc54fc549c5aefcd2de5055683ebf8f63b8e5baf567547b87); /* line */ 
        delete data[input];
coverage_0xf0fc8bb4(0xe290902df80ad9d8470d257f48df2ac392da89fa37aa6b8d2b51fbdd89c681ac); /* line */ 
        delete valid[input];
coverage_0xf0fc8bb4(0x5cee940e83340886367a386e8855c37d2e319b7739243d0095f57abd9195b7b4); /* line */ 
        coverage_0xf0fc8bb4(0x1f997cb344e0d01bb07c7f8ebf5f438760c6ff2c1656afda99fc8bdc96e374fa); /* statement */ 
emit DataDeleted(input);
coverage_0xf0fc8bb4(0x4c81f7182e7c9ab4583663aa2bdb4a543aa4a8523b798c55cddf10c584ef008b); /* line */ 
        coverage_0xf0fc8bb4(0x478f20d60176ce006103eaea3a48c54acc0c29e4f23e1cc5e121d21e4f251d54); /* statement */ 
return output;
    }

    function parseTradeData(
        bytes memory tradeData
    )
        private
        pure
        returns (uint256)
    {coverage_0xf0fc8bb4(0x6187505e1d6402bb34549878b3b6438017606674d567f27abeb3f8563da7c65a); /* function */ 

coverage_0xf0fc8bb4(0x6c2dc4cb06dfa44353a54b3ff89410b66c9ba9274cb14ea2136b936ee40d5489); /* line */ 
        coverage_0xf0fc8bb4(0x6ba216d51ffc98e64e6038988b00866ee87f60434aa669af953924bc99be888c); /* statement */ 
Require.that(
            tradeData.length == 32,
            FILE,
            "Call data invalid length"
        );

coverage_0xf0fc8bb4(0xd7332d1bff901e685adeacd597a106259fef471aeae0c0a10bea5785a2c9b168); /* line */ 
        coverage_0xf0fc8bb4(0x9966fdd2af8b74c1eccdd04aca1df8183c27d284aec9295aab8ef1e9c1502578); /* statement */ 
uint256 input;

        /* solium-disable-next-line security/no-inline-assembly */
coverage_0xf0fc8bb4(0xb0c34360811558882ac8174f8f8b62847c58e43c8ef898c857827470a3800fce); /* line */ 
        assembly {
            input := mload(add(tradeData, 32))
        }

coverage_0xf0fc8bb4(0x2aeb46da2e473d02397c17734da89e0465af398c4c1972d096a1dbd30febc08e); /* line */ 
        coverage_0xf0fc8bb4(0xd6d200dd21430c6fdddc5f7016b764ec8675dd63e04d931cfee444f889fac7cf); /* statement */ 
return input;
    }
}
