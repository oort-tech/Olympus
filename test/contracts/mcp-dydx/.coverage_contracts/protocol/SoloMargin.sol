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

import { Admin } from "./Admin.sol";
import { Getters } from "./Getters.sol";
import { Operation } from "./Operation.sol";
import { Permission } from "./Permission.sol";
import { State } from "./State.sol";
import { Storage } from "./lib/Storage.sol";


/**
 * @title SoloMargin
 * @author dYdX
 *
 * Main contract that inherits from other contracts
 */
contract SoloMargin is
    State,
    Admin,
    Getters,
    Operation,
    Permission
{
function coverage_0x8b9b65b2(bytes32 c__0x8b9b65b2) public pure {}

    // ============ Constructor ============

    constructor(
        Storage.RiskParams memory riskParams,
        Storage.RiskLimits memory riskLimits
    )
        public
    {coverage_0x8b9b65b2(0xf187b0c544ada76dacc74a1ff9f2f55b82f78ac68dce3b97de929e636ba70d25); /* function */ 

coverage_0x8b9b65b2(0xabf1e46e5255b588a29ffd1c3c0cd50c43d79786d7a975bfb6b7a6264eb0bfc3); /* line */ 
        coverage_0x8b9b65b2(0x25024ea266a1d68cb1031884a0d61ab9501b2611d1e12b153b9c24b5496ccdcd); /* statement */ 
g_state.riskParams = riskParams;
coverage_0x8b9b65b2(0xf3403e5b75c938a9344b3192a3303d22a7bb7b5c91c18320f836bcd9a311818b); /* line */ 
        coverage_0x8b9b65b2(0xf9113c737dc40588ed5f342fe26b735bbcd4a6a91ac0f2c1594d1c0fdbb8c336); /* statement */ 
g_state.riskLimits = riskLimits;
    }
}
