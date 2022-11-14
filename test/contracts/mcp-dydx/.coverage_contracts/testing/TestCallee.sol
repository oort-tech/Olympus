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
import { IAutoTrader } from "../protocol/interfaces/IAutoTrader.sol";
import { Account } from "../protocol/lib/Account.sol";
import { Require } from "../protocol/lib/Require.sol";


/**
 * @title TestCallee
 * @author dYdX
 *
 * ICallee for testing
 */
contract TestCallee is
    ICallee,
    OnlySolo
{
function coverage_0x2fff666d(bytes32 c__0x2fff666d) public pure {}

    // ============ Constants ============

    bytes32 constant FILE = "TestCallee";

    // ============ Events ============

    event Called(
        address indexed sender,
        address indexed accountOwner,
        uint256 accountNumber,
        uint256 accountData,
        uint256 senderData
    );

    // ============ Storage ============

    // owner => number => data
    mapping (address => mapping (uint256 => uint256)) public accountData;

    // sender => data
    mapping (address => uint256) public senderData;

    // ============ Constructor ============

    constructor(
        address soloMargin
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x2fff666d(0xc90c70dd0e4ccbbde9151d8383d08fdf383b65161dc450506f8d81fdaf9cbc54); /* function */ 
}

    // ============ ICallee Functions ============

    function callFunction(
        address sender,
        Account.Info memory account,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
    {coverage_0x2fff666d(0xff06fa5e0f5614b303248367e2d4a484829727f753b853e4f20b63a2fe5fe258); /* function */ 

coverage_0x2fff666d(0x3d9754a031d8501de57a31309b19ffa885fb2d7b3354ab7cdbdb04b965bbbbf1); /* line */ 
        coverage_0x2fff666d(0xdf4e194bbd085c581a340862a87c60ffcc497926af6612f1b6a9bd4823f3a240); /* statement */ 
(
            uint256 aData,
            uint256 sData
        ) = parseData(data);

coverage_0x2fff666d(0xb1b482e4b8d662b3725ed58cbc8514367745e757cae57726452b276ef222ddd0); /* line */ 
        coverage_0x2fff666d(0x53d58a2c17c49c83039561b63a4bfbe5cf9663d03eb043b9275a559b01a82459); /* statement */ 
emit Called(
            sender,
            account.owner,
            account.number,
            aData,
            sData
        );

coverage_0x2fff666d(0x3863f42ac2de5bd5a73fa03495a1acf64918709818dd2f26c509f3c82f4b334b); /* line */ 
        coverage_0x2fff666d(0xc9913e6552103c038225e1148d627673dc7698e69d0ca19bdd6e67a2c92513a3); /* statement */ 
accountData[account.owner][account.number] = aData;
coverage_0x2fff666d(0xaf8415c071b646fe3c59c80d01875d78e6f26070eb4aa27cb9859abe34520bb7); /* line */ 
        coverage_0x2fff666d(0xa5a18337243d1a71f7da5297d482e3100f5cb782679f59491e2cbd85f393d9ef); /* statement */ 
senderData[sender] = sData;
    }

    // ============ Private Functions ============

    function parseData(
        bytes memory data
    )
        private
        pure
        returns (
            uint256,
            uint256
        )
    {coverage_0x2fff666d(0xb6279025d9d3386bd062741289b181c84c2dc8395e3645e3098b1a01c6e224a5); /* function */ 

coverage_0x2fff666d(0x1bdcfe02d4190742ef66223ab705ccb2f98893368959e109221159a4f4845aee); /* line */ 
        coverage_0x2fff666d(0xf675c2aaac5d9d87be0130761db3a48e9718ebc38869a5cc0ae87d4f6daad5b2); /* statement */ 
Require.that(
            data.length == 64,
            FILE,
            "Call data invalid length"
        );

coverage_0x2fff666d(0x86accb605755bba0804b8763f0bd131ba8434ce999ea763df92ed6fed9906204); /* line */ 
        coverage_0x2fff666d(0x170f5eafe3fbc7d24992777a534368bc8e79980937fdf31b88ac562685c92b09); /* statement */ 
uint256 aData;
coverage_0x2fff666d(0xd8ca9286994232eb6c679703a88e0674e2d7151b7b89848b16815a16e19af354); /* line */ 
        coverage_0x2fff666d(0xd5d205a408294c39976dd0dc3cf1ea4625333635b78aff4a7bf974d3672d5309); /* statement */ 
uint256 sData;

        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x2fff666d(0x8b16342539480f82a1d2d94f5a662fc39cf6764baba07fd680e0573c5392a5d7); /* line */ 
        assembly {
            aData := mload(add(data, 32))
            sData := mload(add(data, 64))
        }

coverage_0x2fff666d(0xc04d97af1a951c4fd4f5351a72c88a53adda40aaff566a12de4979fd86112e27); /* line */ 
        coverage_0x2fff666d(0x831cfb22da7ec8966cea6c6df603a848ba17417d7dd862e0a45e56bf53d85805); /* statement */ 
return (
            aData,
            sData
        );
    }
}
