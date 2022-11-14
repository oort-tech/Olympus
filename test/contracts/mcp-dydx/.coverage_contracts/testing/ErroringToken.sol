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

import { TestToken } from "./TestToken.sol";


contract ErroringToken is TestToken {
function coverage_0x8b06430b(bytes32 c__0x8b06430b) public pure {}


    function transfer(address, uint256) public returns (bool) {coverage_0x8b06430b(0x67162ca9855a080396dd80ce27b160ee81a3dc26fb587984b13a74dfc990b34e); /* function */ 

coverage_0x8b06430b(0xa31d163b2df804b30dbacfb7225b20cff218458ff81ed4fa1eb1fe5206672a36); /* line */ 
        coverage_0x8b06430b(0x00e4f50da56ef1377f1b0dc4e3dcc4dd304666eda2b9082710d23c6e918e0571); /* statement */ 
return false;
    }

    function transferFrom(address, address, uint256) public returns (bool) {coverage_0x8b06430b(0xbc191e70c4182263944a906a7547bdad77057c5f87164469a293cc899fab66a2); /* function */ 

coverage_0x8b06430b(0x8f120bbe93ba2d6648ebc1cbb8467e0d3ed4fb4f4a8156d1551ca47b0631a895); /* line */ 
        coverage_0x8b06430b(0x5c2ccbb31c3af40c6603edbfc0966a7a4e17bfd0c4323e2aff2202ca50db7968); /* statement */ 
return false;
    }

    function approve(address, uint256) public returns (bool) {coverage_0x8b06430b(0x43905c0cee8553f42b3e0322f8f85b4eccd319349f8405ab7bcfce362937a950); /* function */ 

coverage_0x8b06430b(0xfb3444e5588c7c80f6a79dab90648894420906db5a6394411c7511bee466e62d); /* line */ 
        coverage_0x8b06430b(0xfd9bbce322f15440622460096e0cdfa6105a1f1332c321a49057deacf6c76f09); /* statement */ 
return false;
    }
}
