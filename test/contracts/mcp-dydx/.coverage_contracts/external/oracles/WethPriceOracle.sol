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

import { IPriceOracle } from "../../protocol//interfaces/IPriceOracle.sol";
import { Monetary } from "../../protocol/lib/Monetary.sol";
import { IMakerOracle } from "../interfaces/IMakerOracle.sol";


/**
 * @title WethPriceOracle
 * @author dYdX
 *
 * PriceOracle that returns the price of Wei in USD
 */
contract WethPriceOracle is
    IPriceOracle
{
function coverage_0x66a44404(bytes32 c__0x66a44404) public pure {}

    // ============ Storage ============

    IMakerOracle public MEDIANIZER;

    // ============ Constructor =============

    constructor(
        address medianizer
    )
        public
    {coverage_0x66a44404(0x51abaa97de1320ab03bbcd90649c4bd2404ffed3de0991b678dd71449d6fd62a); /* function */ 

coverage_0x66a44404(0xc74a53f9f8ca63e3913f5d85c60e58b57551398e65a3ca4cbbc9f1f2807f022c); /* line */ 
        coverage_0x66a44404(0x576680303bb222f5c7a350d9f6cf9e867a09b28e3e60530974252b7d5fc86bab); /* statement */ 
MEDIANIZER = IMakerOracle(medianizer);
    }

    // ============ IPriceOracle Functions =============

    function getPrice(
        address /* token */
    )
        public
        view
        returns (Monetary.Price memory)
    {coverage_0x66a44404(0x73ca4901a3bb2c96b59055991b169e3bb77a0728719bd59b34f7b3c01802ca35); /* function */ 

coverage_0x66a44404(0x83ad7b009968f6731f796bf9ec567da3595759fd2b5a5b5743f870cfcfa5450e); /* line */ 
        coverage_0x66a44404(0x68994831607376f08b68a8c7f81f358c2bd09ffafa73c3ba0807a0a9f196abe4); /* statement */ 
(bytes32 value, /* bool fresh */) = MEDIANIZER.peek();
coverage_0x66a44404(0x52d5670cae1e7a9cb477b48d17e0a9788dd7a6936405f36ea2512eca92a8f97b); /* line */ 
        coverage_0x66a44404(0x1d9fc020f3f0ce2cebfaa771b2914b46eb32e45f44a0538ef132b9509b4b2c78); /* statement */ 
return Monetary.Price({ value: uint256(value) });
    }
}
