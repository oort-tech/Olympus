/*

    Copyright 2020 dYdX Trading Inc.

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

import { IUniswapV2Pair } from "../external/interfaces/IUniswapV2Pair.sol";


/**
 * @title TestUniswapV2Pair
 * @author dYdX
 *
 * Mock Uniswap V2 pair.
 */
contract TestUniswapV2Pair is
    IUniswapV2Pair
{
function coverage_0xa92c41fa(bytes32 c__0xa92c41fa) public pure {}

    uint112 public RESERVE0 = 0;
    uint112 public RESERVE1 = 0;

    // ============ Getter Functions ============

    function getReserves()
        external
        view
        returns (uint112 reserve0, uint112 reserve1, uint32 blockTimestampLast)
    {coverage_0xa92c41fa(0x302cd5a809a4ae1d44a5836b7d73da89d1241eba5955aaafd755fa1d19a9f600); /* function */ 

coverage_0xa92c41fa(0x982ecc374aa19dc5a8b847845a2ffe3803fef7e0e8e223c5b513894e6498a0e4); /* line */ 
        coverage_0xa92c41fa(0xb55937a6faa329bc85868c1369b47a53c8f1ae582cf5e2129e3999a511b785fd); /* statement */ 
return (RESERVE0, RESERVE1, 0);
    }

    // ============ Test Data Setter Functions ============

    function setReserves(
        uint112 reserve0,
        uint112 reserve1
    )
        external
    {coverage_0xa92c41fa(0x0b276747c033c06c128d0df5f8b7ff13bf7b8a14213e3618e802b031a4f50474); /* function */ 

coverage_0xa92c41fa(0x6d6367c52a52e151d5aa749b6f287236ca2a308896201efb4565893f7a7aec1d); /* line */ 
        coverage_0xa92c41fa(0x736daa6b43fe5b2441a1b39b53f9a33facfffad0a935dc361632b06bf89b9b38); /* statement */ 
RESERVE0 = reserve0;
coverage_0xa92c41fa(0xb114c3df3a5e0a91ceb261cfb8afd5b61c8ce437f1bd4f6724ed5e8d66171a9f); /* line */ 
        coverage_0xa92c41fa(0xfa13b2a9adf53308b1069796bc03c2800826698df1fa6c4410f3ea3c800c7735); /* statement */ 
RESERVE1 = reserve1;
    }
}
