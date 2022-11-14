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

import { PolynomialInterestSetter } from "../external/interestsetters/PolynomialInterestSetter.sol";


/**
 * @title TestPolynomialInterestSetter
 * @author dYdX
 *
 * PolynomialInterestSetter for testing
 */
contract TestPolynomialInterestSetter is
    PolynomialInterestSetter
{
function coverage_0x525bcc4d(bytes32 c__0x525bcc4d) public pure {}

    constructor(
        PolyStorage memory parameters
    )
        public
        PolynomialInterestSetter(parameters)
    {coverage_0x525bcc4d(0x45b9ef36113e23410efd427218758b06b2e1343de0d72537a65c99bd0ac17130); /* function */ 

    }

    function setParameters(
        PolyStorage memory parameters
    )
        public
    {coverage_0x525bcc4d(0x8bb46c24564fb19c680bcc5f940010cf87dbd97e0985a831725f19c7c44687b2); /* function */ 

coverage_0x525bcc4d(0xfb4737292b5a7d90e6c4756fa879ff07487f1602044f5791dad51f98062ce852); /* line */ 
        coverage_0x525bcc4d(0x7adb5756a60879d12fb603a7f5cbfca460cffc954ebbff4df3fcc5bd00b5f1c7); /* statement */ 
g_storage = parameters;
    }

    function createNew(
        PolyStorage memory parameters
    )
        public
        returns (PolynomialInterestSetter)
    {coverage_0x525bcc4d(0xa0e5d4a37dbaed0d528b4bd230e593aadbcca17cde343b0123e59680d9a65ca5); /* function */ 

coverage_0x525bcc4d(0xa1d5af83cd0aa07e0140df7f3a5aad06c174d982c54788ba641a8e59ac91eb72); /* line */ 
        coverage_0x525bcc4d(0x2e84fcd28be758afa250e4f50235b8e7c0f069667fb6fc513508b9ca762261d9); /* statement */ 
return new PolynomialInterestSetter(parameters);
    }
}
