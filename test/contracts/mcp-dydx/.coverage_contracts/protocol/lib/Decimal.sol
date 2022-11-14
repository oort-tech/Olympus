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
import { Math } from "./Math.sol";


/**
 * @title Decimal
 * @author dYdX
 *
 * Library that defines a fixed-point number with 18 decimal places.
 */
library Decimal {
function coverage_0x1c3bd8a6(bytes32 c__0x1c3bd8a6) public pure {}

    using SafeMath for uint256;

    // ============ Constants ============

    uint256 constant BASE = 10**18;

    // ============ Structs ============

    struct D256 {
        uint256 value;
    }

    // ============ Functions ============

    function one()
        internal
        pure
        returns (D256 memory)
    {coverage_0x1c3bd8a6(0x7b2b958c922321d62740928d16e37047284d4a6f90b984986c8d4d6a26dbb28f); /* function */ 

coverage_0x1c3bd8a6(0x53cf02c291baa4fbd76cbb44707a81d8c0ff74f2b2f6e481e934a63c73b97c15); /* line */ 
        coverage_0x1c3bd8a6(0x07d512780e87d89d169bec63c27dc1d33a9407747da6f66c08cc992679c7a4a7); /* statement */ 
return D256({ value: BASE });
    }

    function onePlus(
        D256 memory d
    )
        internal
        pure
        returns (D256 memory)
    {coverage_0x1c3bd8a6(0x0c686d2666aefc6f0625e5c7cfe9f951d19da9798fbf133f2e8c33b73b6f89a0); /* function */ 

coverage_0x1c3bd8a6(0x12f53853a274eed12d1ab26dfc0588f7ae09b282b000df68b9900265b4dacae4); /* line */ 
        coverage_0x1c3bd8a6(0xa63c5f4b14a47bddeafc6c6c7985220a0e080d5876baed5d4add704f5f3c5392); /* statement */ 
return D256({ value: d.value.add(BASE) });
    }

    function mul(
        uint256 target,
        D256 memory d
    )
        internal
        pure
        returns (uint256)
    {coverage_0x1c3bd8a6(0x5674a0a5b4a80fa089e4aa3b0e5dbdc7c9a31b29ede4c34ce41736facabdc338); /* function */ 

coverage_0x1c3bd8a6(0x35ffddfac0b4895f83ade6cc77b04b8ae68d3baaef04672db7419fc9fd70d922); /* line */ 
        coverage_0x1c3bd8a6(0x78569c8783f5762ef04ca8cac246fe99035f4cc4519e830bd7ba084e83230851); /* statement */ 
return Math.getPartial(target, d.value, BASE);
    }

    function div(
        uint256 target,
        D256 memory d
    )
        internal
        pure
        returns (uint256)
    {coverage_0x1c3bd8a6(0xdd3b5ce65b1c78e3ce3ebb7a5d89987169336cdad77f010684e4bc9332f47e80); /* function */ 

coverage_0x1c3bd8a6(0x041e79c8b52e134d5d46228443e689969cd652902a6e6b6dc50f4d335fa506a3); /* line */ 
        coverage_0x1c3bd8a6(0x03710a40ac8acd73b970111d00375f54241051ebb8d7490d3986b453b53d38f4); /* statement */ 
return Math.getPartial(target, BASE, d.value);
    }
}
