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

import { Math } from "./Math.sol";


/**
 * @title Time
 * @author dYdX
 *
 * Library for dealing with time, assuming timestamps fit within 32 bits (valid until year 2106)
 */
library Time {
function coverage_0xb8be588c(bytes32 c__0xb8be588c) public pure {}


    // ============ Library Functions ============

    function currentTime()
        internal
        view
        returns (uint32)
    {coverage_0xb8be588c(0x93449638b184ef99749245920fbd132feb01c78245a1c5cbab79d1a38f753ba7); /* function */ 

coverage_0xb8be588c(0x85dc4ba4330d2cb3e9859e2a63ac6f0e16fa9695a67ec407230fa2bfc09bb0a0); /* line */ 
        coverage_0xb8be588c(0xd525bf55bf8d7e4c5f45383009509dc4e31fe74dcd091371762f78ef9f117454); /* statement */ 
return Math.to32(block.timestamp);
    }
}
