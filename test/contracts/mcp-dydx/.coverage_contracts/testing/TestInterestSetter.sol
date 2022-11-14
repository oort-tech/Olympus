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
import { IInterestSetter } from "../protocol/interfaces/IInterestSetter.sol";
import { Interest } from "../protocol/lib/Interest.sol";


/**
 * @title TestInterestSetter
 * @author dYdX
 *
 * Interest setter used for testing that always returns a constant interest rate
 */
contract TestInterestSetter is
    IInterestSetter
{
function coverage_0xfccb7e59(bytes32 c__0xfccb7e59) public pure {}

    mapping (address => Interest.Rate) public g_interestRates;

    function setInterestRate(
        address token,
        Interest.Rate memory rate
    )
        public
    {coverage_0xfccb7e59(0x9d640bd5e968b4542fff91a020ee48f53e07edd4b747c2dfd4b67c225fab1208); /* function */ 

coverage_0xfccb7e59(0xff4735f1585adfc4b7f820c1e6c3d44a8109b8615ce26d863b0693529e90e49c); /* line */ 
        coverage_0xfccb7e59(0x8d2657b48f6bd10fad54db05521cc5048fdeea8d109013942fb17985dcd5abb5); /* statement */ 
g_interestRates[token] = rate;
    }

    function getInterestRate(
        address token,
        uint256 /* borrowWei */,
        uint256 /* supplyWei */
    )
        public
        view
        returns (Interest.Rate memory)
    {coverage_0xfccb7e59(0x87550755a634ec38ed22f58bdbc97fb7594a54c6aaf0129b56aa8ef1212c6bbe); /* function */ 

coverage_0xfccb7e59(0x3f46921e89826d5e8824a88a071b48ae4c868325a9dd43b1aac699c9636105b8); /* line */ 
        coverage_0xfccb7e59(0x20501c0b1f71091965428997452f0d9beaa983acfb325a95bf1232da140ae0c0); /* statement */ 
return g_interestRates[token];
    }
}
