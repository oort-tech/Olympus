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

import { IPriceOracle } from "../protocol/interfaces/IPriceOracle.sol";
import { Monetary } from "../protocol/lib/Monetary.sol";


/**
 * @title TestPriceOracle
 * @author dYdX
 *
 * Oracle used for testing
 */
contract TestPriceOracle is IPriceOracle {
function coverage_0x42e05101(bytes32 c__0x42e05101) public pure {}


    mapping (address => uint256) public g_prices;

    function setPrice(
        address token,
        uint256 price
    )
        external
    {coverage_0x42e05101(0x29e7792978cab4f4a614657a5ce3df6f7a780eadc439e355e7822fecf4a54b87); /* function */ 

coverage_0x42e05101(0xa971add5b4e8bbde43cf8c01832ddfc29d5796943cf3932952ff7f50a3d3f0f9); /* line */ 
        coverage_0x42e05101(0x39aaff1fca3984ce4191e194493674e06ddf89955480ead76d03316a14af7761); /* statement */ 
g_prices[token] = price;
    }

    function getPrice(
        address token
    )
        public
        view
        returns (Monetary.Price memory)
    {coverage_0x42e05101(0x38bb7a559a881f6739c4eefb8701c216a3148082d648ad1152c6dc71f24fe41e); /* function */ 

coverage_0x42e05101(0xea9b5a7c7cea931273297e4fce66787c59e4269a4dc66f230154c8f52a29455f); /* line */ 
        coverage_0x42e05101(0x66d39ccb9428ffdb687bab3ebf7e457ac25e526b0fb0e08aecb214d43090069d); /* statement */ 
return Monetary.Price({
            value: g_prices[token]
        });
    }
}
