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

import { Monetary } from "./Monetary.sol";
import { Storage } from "./Storage.sol";


/**
 * @title Cache
 * @author dYdX
 *
 * Library for caching information about markets
 */
library Cache {
function coverage_0xf5eff040(bytes32 c__0xf5eff040) public pure {}

    using Cache for MarketCache;
    using Storage for Storage.State;

    // ============ Structs ============

    struct MarketInfo {
        bool isClosing;
        uint128 borrowPar;
        Monetary.Price price;
    }

    struct MarketCache {
        MarketInfo[] markets;
    }

    // ============ Setter Functions ============

    /**
     * Initialize an empty cache for some given number of total markets.
     */
    function create(
        uint256 numMarkets
    )
        internal
        pure
        returns (MarketCache memory)
    {coverage_0xf5eff040(0x110814b90692a4061c6b18d82db3a06a46dc0217d199f2157402ea3aa463ce50); /* function */ 

coverage_0xf5eff040(0x633d344135a1d6566bdf7859798dad4741a1aa2f28017d7f20fd460db5d9c378); /* line */ 
        coverage_0xf5eff040(0xc8960f7234fe0d3ef5c2aa2fb4abd139d812c1b1da056c47f37a2a4728c09b38); /* statement */ 
return MarketCache({
            markets: new MarketInfo[](numMarkets)
        });
    }

    /**
     * Add market information (price and total borrowed par if the market is closing) to the cache.
     * Return true if the market information did not previously exist in the cache.
     */
    function addMarket(
        MarketCache memory cache,
        Storage.State storage state,
        uint256 marketId
    )
        internal
        view
        returns (bool)
    {coverage_0xf5eff040(0x408cfaac3d124cb18b36a3288d2e689c99c3c17439461f189c0756649cb0c9ef); /* function */ 

coverage_0xf5eff040(0xe897074dc4e48f2c63e0109e380ae2e608350c21600f2b3985793f5b6f7e08d6); /* line */ 
        coverage_0xf5eff040(0xeef0a7e8f95069918e7d91fc44ab8d4802f4e0d587c8810260b347fe798c70b9); /* statement */ 
if (cache.hasMarket(marketId)) {coverage_0xf5eff040(0xb648fe73238af75575803e36cdc8a8f6c0b854eacaaa0e66680626423683d71f); /* branch */ 

coverage_0xf5eff040(0xe14f63dfc431666944ca2b49c1267d5abe02f8916cf5b857e7738940788731f7); /* line */ 
            coverage_0xf5eff040(0x6535c9b928831c8b1799d7422859041f65a9966dadd06c7b4d72747b3dea11ef); /* statement */ 
return false;
        }else { coverage_0xf5eff040(0x686996ab374df7e41f62909b30f2e285fd7555ba1a8f947ef33cb8ab42a45122); /* branch */ 
}
coverage_0xf5eff040(0x8d6df4116c3679381d01632ea1b5a51dbef8adc3345145eaba815c036e0ac7f3); /* line */ 
        coverage_0xf5eff040(0x49ca1ea773ee8ca2b75198a4680527fc17dbb91a6ae0edb7f10e2b10721b1ae1); /* statement */ 
cache.markets[marketId].price = state.fetchPrice(marketId);
coverage_0xf5eff040(0x4a240e0b6d44a8e9a4959125dc3252777f55b681e28103ba366963e59c94505d); /* line */ 
        coverage_0xf5eff040(0xdc676a78e585878594e3f3fa1f459118345420a7c4d9f5cb4b912b215d8458cd); /* statement */ 
if (state.markets[marketId].isClosing) {coverage_0xf5eff040(0xd0945027d0dd00faeb110a65a16227c34143bd0059e4b15f6c98c6372b5c425f); /* branch */ 

coverage_0xf5eff040(0x4849f801d2c81435d7d8d383dbf882ff35dbcfb86ad727e706f12965ac0afdec); /* line */ 
            coverage_0xf5eff040(0xe0226b220076a1cc524a9f21ca3c430cf8864209ecf46d3a9d79bdef1d53f998); /* statement */ 
cache.markets[marketId].isClosing = true;
coverage_0xf5eff040(0xc64a6ad7eed83a9b33657b55ef9aa2541a71ca422d9668a86271d43b933122f5); /* line */ 
            coverage_0xf5eff040(0x59734235025d613c858a7324568837dc2a5e849f9245d923dff664b0bbe4ba46); /* statement */ 
cache.markets[marketId].borrowPar = state.getTotalPar(marketId).borrow;
        }else { coverage_0xf5eff040(0xf64086890bca5f9d083619c1eb1e2a21ab77af6834f2865b68948043dfd4e31d); /* branch */ 
}
coverage_0xf5eff040(0x8a30d8c43453055f00749d52d0eebcdaf0b68c2597315682df7163f9fe8a3f0a); /* line */ 
        coverage_0xf5eff040(0x57b916b9b3fea90a2a89e96286a1cf1590adee4fdbf61a97336899919dda2b0d); /* statement */ 
return true;
    }

    // ============ Getter Functions ============

    function getNumMarkets(
        MarketCache memory cache
    )
        internal
        pure
        returns (uint256)
    {coverage_0xf5eff040(0x804873289a515d92e929dd5cb810b550d10c302d476cd6b493ca681d626c39a7); /* function */ 

coverage_0xf5eff040(0xef4c9d41fb1e446a2fee6dc2db47b7b554c7e031d79ab22815279499f6eb08f9); /* line */ 
        coverage_0xf5eff040(0x5563f1fa7a9e566075953d9418b1ab7cf09cf0b65c1d8e0438011fa9edcee552); /* statement */ 
return cache.markets.length;
    }

    function hasMarket(
        MarketCache memory cache,
        uint256 marketId
    )
        internal
        pure
        returns (bool)
    {coverage_0xf5eff040(0xa9c64638f9fb1a00aa04fa2a285c5686434dc9a9062bdeee45c8dff8165f909d); /* function */ 

coverage_0xf5eff040(0xd63d8b446de9211b1876c51a6c0272dec75f55658c2b1af541ddfabba69ef3bf); /* line */ 
        coverage_0xf5eff040(0x8fe202860b2d0113404f4ee55a7d26fce2a0386c32b57421667296b169d52dc9); /* statement */ 
return cache.markets[marketId].price.value != 0;
    }

    function getIsClosing(
        MarketCache memory cache,
        uint256 marketId
    )
        internal
        pure
        returns (bool)
    {coverage_0xf5eff040(0x4a3e6271eff72dadff409122a2739a7b32ac6af9c427d0397db3dc144db0cb4e); /* function */ 

coverage_0xf5eff040(0xc1a8b7b5ee4ea53ec2142b1efc539189878992a36b96c832b1a509bd103bbcb5); /* line */ 
        coverage_0xf5eff040(0x0b2fba8bd4d0024da94534103e12b70fba4f126c5fd2d4c70a2aa0d673eb3cfc); /* statement */ 
return cache.markets[marketId].isClosing;
    }

    function getPrice(
        MarketCache memory cache,
        uint256 marketId
    )
        internal
        pure
        returns (Monetary.Price memory)
    {coverage_0xf5eff040(0xf8ca86b972f5f39f11771a99b02546cb20b16876ecce33d3e31f93682e255d18); /* function */ 

coverage_0xf5eff040(0xf3f6a6b6dcf8f1cedfc6e5f1bf1c90e2f8a7bca3f40293b378453196c7731f7f); /* line */ 
        coverage_0xf5eff040(0x98486412d3ff198e48605caa38815622bf86ced3d6e4f3c9913a6accdb303840); /* statement */ 
return cache.markets[marketId].price;
    }

    function getBorrowPar(
        MarketCache memory cache,
        uint256 marketId
    )
        internal
        pure
        returns (uint128)
    {coverage_0xf5eff040(0x3a9de9c61fbbfa8182dfc63bab413d028fec0c312ffaac413a100bed85c3a531); /* function */ 

coverage_0xf5eff040(0xf965aa5b535c29b72fbd9177c9e6ae49a2743ca61451494c74f9baf6dfce9117); /* line */ 
        coverage_0xf5eff040(0xb5f70d079038ab8ada83e367c1a47b5df247c102d145b0dac1ace541464d3e24); /* statement */ 
return cache.markets[marketId].borrowPar;
    }
}
