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

import { OnlySolo } from "../external/helpers/OnlySolo.sol";
import { ICallee } from "../protocol/interfaces/ICallee.sol";
import { Account } from "../protocol/lib/Account.sol";


/**
 * @title TestSimpleCallee
 * @author dYdX
 *
 * ICallee for testing any data being sent
 */
contract TestSimpleCallee is
    ICallee,
    OnlySolo
{
function coverage_0x42c680ea(bytes32 c__0x42c680ea) public pure {}

    // ============ Constants ============

    bytes32 constant FILE = "TestSimpleCallee";

    // ============ Events ============

    event Called(
        address indexed sender,
        address indexed accountOwner,
        uint256 accountNumber,
        bytes data
    );

    // ============ Constructor ============

    constructor(
        address soloMargin
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x42c680ea(0xcd9b4b621d5916eefb102d4633d0909a41735c89d2fcb2c79203597e70492b1e); /* function */ 
}

    // ============ ICallee Functions ============

    function callFunction(
        address sender,
        Account.Info memory account,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
    {coverage_0x42c680ea(0xbb24b9dbb337ebc4398ba5629b40f2b82c81ff37f67cf57087c608e04ff01f1d); /* function */ 

coverage_0x42c680ea(0x1c2918c0d9de8ec791534b359eecaeba8eafb98ac1623bd38fb8f74b30efb35a); /* line */ 
        coverage_0x42c680ea(0xc25316f9c970a7147d71f1e0a4aa1546e1ef1bfec9d371aca217e6c6d8500dd2); /* statement */ 
emit Called(
            sender,
            account.owner,
            account.number,
            data
        );
    }
}
