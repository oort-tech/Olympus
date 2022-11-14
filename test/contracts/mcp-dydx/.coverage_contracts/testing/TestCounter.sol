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

/**
 * @title TestCounter
 * @author dYdX
 *
 * Counts function calls for testing
 */
contract TestCounter
{
function coverage_0x1a810027(bytes32 c__0x1a810027) public pure {}

    // ============ Storage ============

    uint256 public counterFallback;

    uint256 public counterOne;

    mapping (uint256 => uint256) public counterTwo;

    mapping (uint256 => mapping (uint256 => uint256)) public counterThree;

    // ============ Functions ============

    function()
        external
        payable
    {coverage_0x1a810027(0x499787a2590c364a6e1ff15a60aa3419361e84d83ef306696273c90af697a7f2); /* function */ 

coverage_0x1a810027(0x98bc3811906bb4602b4a4a1647f41520605ea4dd1b54da0bb0ceecba43efd73f); /* line */ 
        counterFallback++;
    }

    function functionOne()
        public
    {coverage_0x1a810027(0x624198ed225eb98fd93e26f4b26f332968fff828d62bc73689e08b4d1eccb8e2); /* function */ 

coverage_0x1a810027(0xcb39e60b13f28db10b9d5f732fa1edb570fccc4c7402251adc66c72d00a064c6); /* line */ 
        counterOne++;
    }

    function functionTwo(
        uint256 input
    )
        public
    {coverage_0x1a810027(0x70203718de31224c62a3b2da00324a58175af0743d9c556281eb59ba2a45255b); /* function */ 

coverage_0x1a810027(0xe9eefb2aa13e1ab370a82aaa1d2af72d48371404bf3c894e00fd4e23a10eb8b9); /* line */ 
        counterTwo[input]++;
    }

    function functionThree(
        uint256 input1,
        uint256 input2
    )
        public
    {coverage_0x1a810027(0x4fdc31e30079eee3a124a437afaa8794116bde933af068c93a11e4a8201b5c03); /* function */ 

coverage_0x1a810027(0xe026379d133caf7d36b2f94588ef4a8842bb0518ea50f448206b1ba72b23cd8f); /* line */ 
        counterThree[input1][input2]++;
    }
}
