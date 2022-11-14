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

import { IPriceOracle } from "../../protocol/interfaces/IPriceOracle.sol";
import { Monetary } from "../../protocol/lib/Monetary.sol";


/**
 * @title UsdcPriceOracle
 * @author dYdX
 *
 * PriceOracle that returns the price of USDC in USD
 */
contract UsdcPriceOracle is
    IPriceOracle
{
function coverage_0xe12fc512(bytes32 c__0xe12fc512) public pure {}

    // ============ Constants ============

    uint256 constant DECIMALS = 6;

    uint256 constant EXPECTED_PRICE = ONE_DOLLAR / (10 ** DECIMALS);

    // ============ IPriceOracle Functions =============

    function getPrice(
        address /* token */
    )
        public
        view
        returns (Monetary.Price memory)
    {coverage_0xe12fc512(0xd5fd6eeea611711a20ba60c378d257a091968fdb962dd9f02f6cb95d112d7a51); /* function */ 

coverage_0xe12fc512(0x46ff740f506c264c1ef7ce96a3f5210ddf48d03d174fb5e7ee79a72ebceb81b6); /* line */ 
        coverage_0xe12fc512(0xbe441401d32c24f51a3c7b0462c328927b55bc4626427308efd57e6926cf16b7); /* statement */ 
return Monetary.Price({ value: EXPECTED_PRICE });
    }
}
