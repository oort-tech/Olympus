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

import { IMakerOracle } from "../external/interfaces/IMakerOracle.sol";


contract TestMakerOracle is
    IMakerOracle
{
function coverage_0xde08436a(bytes32 c__0xde08436a) public pure {}

    uint256 public price;
    bool public valid;

    function setValues(
        uint256 _price,
        bool _valid
    )
        external
    {coverage_0xde08436a(0xdda2d2759f73ee27da6671dd311cbfd5188d762375f7ab449e31257bbb6054f4); /* function */ 

coverage_0xde08436a(0x624fd6568bbe91138e561885b701d8a60040c0b3adec09289a2d6f74cd076345); /* line */ 
        coverage_0xde08436a(0x70c015ff257c72494b3898a0cf427b8bf4ea9700f4150939fea00f8def145117); /* statement */ 
price = _price;
coverage_0xde08436a(0xde3d09010df09b86acf6c718f42275fdec2f3c83f538daeb010f1b8b39031a51); /* line */ 
        coverage_0xde08436a(0xffca94daf9a6cac0f1a87e956b8e2aa5d8f0a718f2fc1d0a20ee59c379e6359c); /* statement */ 
valid = _valid;
    }

    function peek()
        external
        view
        returns (bytes32, bool)
    {coverage_0xde08436a(0x3de0ffde0abb6b649791f83fd432d0832176705d2a6756d0bb07f1a49436fc8c); /* function */ 

coverage_0xde08436a(0x843a0e44790a3eb7671b9e0a74c6509d4069d36c737cf18a23fc14bd2989088d); /* line */ 
        coverage_0xde08436a(0xc97829c161038a42dc68d9a64567d951cc7a6156364e17e9cece3d09921d3c9c); /* statement */ 
return (bytes32(price), valid);
    }

    function read()
        external
        view
        returns (bytes32)
    {coverage_0xde08436a(0x1e637e0e81440140c39ac85f04a5fa10b03acb663f1a40092440ed1a77ee1d3e); /* function */ 

coverage_0xde08436a(0xc26accc5b24aa179089d930f2977cd3530442425aa54553823fb180efef5583b); /* line */ 
        coverage_0xde08436a(0xbbec62bacc49d45262ea7d2bb45470bc84dcab22b4fb9c0c266a8939b2a94d07); /* assertPre */ 
coverage_0xde08436a(0x544a944b2b48fe95092a2ac39c8ffa2900dd05023937e0dc3d44a2f97353797b); /* statement */ 
require(valid);coverage_0xde08436a(0x34626552c3e7929770c11e3f296476bc8361e93c1053bfa86fc98b8caf27e3fe); /* assertPost */ 

coverage_0xde08436a(0x5e3d542b7136235f00719692f440b875bac7f61a369696fe117c3e5dd1870e34); /* line */ 
        coverage_0xde08436a(0x6c4da1de2a1a58e1320245784cd4420ec528d034713016cde272b2f920b35888); /* statement */ 
return bytes32(price);
    }
}
