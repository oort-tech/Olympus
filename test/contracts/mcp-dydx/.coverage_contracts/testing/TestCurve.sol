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

import { ICurve } from "../external/interfaces/ICurve.sol";


/**
 * @title TestCurve
 * @author dYdX
 *
 * Mock of the Curve contract.
 */
contract TestCurve is
    ICurve
{
function coverage_0x499c95f5(bytes32 c__0x499c95f5) public pure {}

    uint256 public fee = 4000000;
    uint256 public dy = 0;

    // ============ Getter Functions ============

    function get_dy(
        int128 /* i */,
        int128 /* j */,
        uint256 /* dx */
    )
        external
        view
        returns (uint256)
    {coverage_0x499c95f5(0x932df148cc14d624e4e125eba3435b0fe64bc5b3a140c6a01ee6bdab491ba24a); /* function */ 

coverage_0x499c95f5(0x67011c1dd6742517525d6ae92a495da9e64029c7df29d9366b6e0e6d0dfcefa8); /* line */ 
        coverage_0x499c95f5(0xbaafb5f92ba6e4afd03b8c597c485347508167e1e8d1811b02558a119d064f28); /* statement */ 
return dy;
    }

    // ============ Test Data Setter Functions ============

    function setFee(
        uint112 newFee
    )
        external
    {coverage_0x499c95f5(0xcaa4d190788e1ff210f1c65dcb51916a0527c5f88c3cd97bb487d5e5aeac3ed3); /* function */ 

coverage_0x499c95f5(0x9527e8e6ba55f3c6af5eb7c00b4e29655c01ff0fd2602ca2df31dbe1c491e1f8); /* line */ 
        coverage_0x499c95f5(0xc9d17354eed951108c3f4a2ba07109d363f969a5a4bc1943b03deb22d7e62671); /* statement */ 
fee = newFee;
    }

    function setDy(
        uint112 newDy
    )
        external
    {coverage_0x499c95f5(0xb9f65e4f15a9e21b3bf9dfa09b5e8ded9914191e00fa4e86fb4fd9f36b775907); /* function */ 

coverage_0x499c95f5(0x63ee5175a744a481a04a24d108c432a4b10ec5a44093c2e9df3a292421992f7c); /* line */ 
        coverage_0x499c95f5(0x2949e7fb410e998f952cf54d68ce36958b0d6fb4173c2d2d3f8d88d2c1003b31); /* statement */ 
dy = newDy;
    }
}
