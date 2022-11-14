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

import { DoubleExponentInterestSetter } from
    "../external/interestsetters/DoubleExponentInterestSetter.sol";


/**
 * @title TestDoubleExponentInterestSetter
 * @author dYdX
 *
 * DoubleExponentInterestSetter for testing
 */
contract TestDoubleExponentInterestSetter is
    DoubleExponentInterestSetter
{
function coverage_0xed6228ad(bytes32 c__0xed6228ad) public pure {}

    constructor(
        PolyStorage memory parameters
    )
        public
        DoubleExponentInterestSetter(parameters)
    {coverage_0xed6228ad(0xae1ea7a8773180d9305db2cdc15282f4442d2e565a87a67d811bbd52611aa6b0); /* function */ 

    }

    function setParameters(
        PolyStorage memory parameters
    )
        public
    {coverage_0xed6228ad(0xfb627d00a1758168a740d94faf54a2a2006323a2ca14c6e5f0d88e9a6b9e0ae7); /* function */ 

coverage_0xed6228ad(0xfab4d03a7d0f63f650e88a357dbaae87e58081ff327395f2f5b4e4e27ad9b285); /* line */ 
        coverage_0xed6228ad(0xbf0b89bf44e657fa7b3d0fd11903b3f82d5f00998473521f52ad63d4487038a4); /* statement */ 
g_storage = parameters;
    }

    function createNew(
        PolyStorage memory parameters
    )
        public
        returns (DoubleExponentInterestSetter)
    {coverage_0xed6228ad(0x25e3c10c5bf7afe7f7da2b3b603fd55f76bdcf472c1b21a874f1568cbea90fcb); /* function */ 

coverage_0xed6228ad(0xa533b550c286e91f5d63ba035011884e79b1fa5d2c44397b3660b9f3c91e8d7f); /* line */ 
        coverage_0xed6228ad(0x74bb896520bacf4d2e7162b93c270371dc882fed41da18a18a774e98c9c2da24); /* statement */ 
return new DoubleExponentInterestSetter(parameters);
    }
}
