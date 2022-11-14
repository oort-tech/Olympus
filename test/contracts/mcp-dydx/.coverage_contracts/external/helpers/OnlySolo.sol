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

import { SoloMargin } from "../../protocol/SoloMargin.sol";
import { Require } from "../../protocol/lib/Require.sol";


/**
 * @title OnlySolo
 * @author dYdX
 *
 * Inheritable contract that restricts the calling of certain functions to Solo only
 */
contract OnlySolo {
function coverage_0x8f36cf2a(bytes32 c__0x8f36cf2a) public pure {}


    // ============ Constants ============

    bytes32 constant FILE = "OnlySolo";

    // ============ Storage ============

    SoloMargin public SOLO_MARGIN;

    // ============ Constructor ============

    constructor (
        address soloMargin
    )
        public
    {coverage_0x8f36cf2a(0x67903e4e98b856271c207e53838061688b19bb718b129e159e41da053cd580f9); /* function */ 

coverage_0x8f36cf2a(0x64fa56c7a727caef89efe575523a64c7f0e83a0b86fbf5f3e9dcac7d6b94b494); /* line */ 
        coverage_0x8f36cf2a(0xe04c270b5949b64ddc7abfbd5a5c20059e5e9bd22d685083feddd750e7fea9f1); /* statement */ 
SOLO_MARGIN = SoloMargin(soloMargin);
    }

    // ============ Modifiers ============

    modifier onlySolo(address from) {coverage_0x8f36cf2a(0x75c60ce25f197e2f24a9eb832cdb565ba24934bb7609934884abc16c74bc8329); /* function */ 

coverage_0x8f36cf2a(0xf277e7b9e29242f20c5ed6d0bde13aadc34143bb6b14e98a85b985d590d4e5c4); /* line */ 
        coverage_0x8f36cf2a(0xdbe0216f7412aa63e256d1b5cf7da0009077355456a636ba291729962d7c3bce); /* statement */ 
Require.that(
            from == address(SOLO_MARGIN),
            FILE,
            "Only Solo can call function",
            from
        );
coverage_0x8f36cf2a(0x6a3d05843a854038e7eddbd3063bd7b3acad103f128ff8bc3c0b39e7d52ffc35); /* line */ 
        _;
    }
}
