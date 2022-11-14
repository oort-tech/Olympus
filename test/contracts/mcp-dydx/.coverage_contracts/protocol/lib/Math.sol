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
import { Require } from "./Require.sol";


/**
 * @title Math
 * @author dYdX
 *
 * Library for non-standard Math functions
 */
library Math {
function coverage_0x28245d09(bytes32 c__0x28245d09) public pure {}

    using SafeMath for uint256;

    // ============ Constants ============

    bytes32 constant FILE = "Math";

    // ============ Library Functions ============

    /*
     * Return target * (numerator / denominator).
     */
    function getPartial(
        uint256 target,
        uint256 numerator,
        uint256 denominator
    )
        internal
        pure
        returns (uint256)
    {coverage_0x28245d09(0xf15cf3527bedcd148f9b1f94cb4800029ab08edc7af50dd42425a4ca0274143d); /* function */ 

coverage_0x28245d09(0x94286f023b6d80b1ae24cd375a37d9cea54bbd51587b5f8c9d645d9b3891103c); /* line */ 
        coverage_0x28245d09(0x9758f4b3029c7318282c8363538bb14b579ba1f16061a6fa7a6fb7c785167cd8); /* statement */ 
return target.mul(numerator).div(denominator);
    }

    /*
     * Return target * (numerator / denominator), but rounded up.
     */
    function getPartialRoundUp(
        uint256 target,
        uint256 numerator,
        uint256 denominator
    )
        internal
        pure
        returns (uint256)
    {coverage_0x28245d09(0x93509161e0e71503be28b5508ca866d5e7cfcdb11d2d44580561260c6f747f4f); /* function */ 

coverage_0x28245d09(0x06b30e514178c3c4bb4999b618c33495475be4f549d53e0896c32f2115ab7ad2); /* line */ 
        coverage_0x28245d09(0x9b026ca585d00615cdfcbb198f1b8252521ee2b192b65b0cce282e9bedd44cc8); /* statement */ 
if (target == 0 || numerator == 0) {coverage_0x28245d09(0xa91e3bbcb4b99156e7a5f3b752a535889dadcd8be467f6cf09a3c58d8b4add4c); /* branch */ 

            // SafeMath will check for zero denominator
coverage_0x28245d09(0x33a2427505b1872d576f23d687fc307d730aa8d6aa64a8183c5c5eb6140d7c3a); /* line */ 
            coverage_0x28245d09(0xa92dc1661e1a8458d3e87e624f2db4244e058db45900a702ac85d5f1d7a7b1e5); /* statement */ 
return SafeMath.div(0, denominator);
        }else { coverage_0x28245d09(0xa00f18f07fed8e58c0a00fefecf0a574e8d26cdd8e9e218d13f67da754c71c22); /* branch */ 
}
coverage_0x28245d09(0x256e1f7d9c653cea55eadb0ef99d83fd21f2775c586e5b0900e5599d5ee738aa); /* line */ 
        coverage_0x28245d09(0x2a756e6d1cfb4b29135b75d0bb81c3c33f3ebdc52a0d5e3229574482b78f473d); /* statement */ 
return target.mul(numerator).sub(1).div(denominator).add(1);
    }

    function to128(
        uint256 number
    )
        internal
        pure
        returns (uint128)
    {coverage_0x28245d09(0x9b231e01c25e4d1ce9d10d550adce65d0d09f4811391ddac032680fd4e84823d); /* function */ 

coverage_0x28245d09(0x4e7ef79616bd10e45d85f50c59a35eeefbb682c5f5a92dc6b8d255fd0fe1ceca); /* line */ 
        coverage_0x28245d09(0x485e3d5abf8323726ef8eb46312189b5c574e7acde7d1932163c780c110ec9b6); /* statement */ 
uint128 result = uint128(number);
coverage_0x28245d09(0xd48ec8cd9df9f4a78a6e43e650af79220f92b941bc9491252684d19c9b5f8604); /* line */ 
        coverage_0x28245d09(0x8a45805e54f6e6c3daefb40a4c27b28e1072330cd38be8db9819f20899d8b65e); /* statement */ 
Require.that(
            result == number,
            FILE,
            "Unsafe cast to uint128"
        );
coverage_0x28245d09(0xa59407b6df74f6910e83fefd47f2535bfce8a9a66a80f22c745b50617632d26a); /* line */ 
        coverage_0x28245d09(0x14551ba29913500f387a809fa5d526e2ff281a99611ff67e83498a3907cc98cd); /* statement */ 
return result;
    }

    function to96(
        uint256 number
    )
        internal
        pure
        returns (uint96)
    {coverage_0x28245d09(0x18b0e3f8d505a6d2494a7beb67985819c691d44c89a3bab7a63e322b088d24ae); /* function */ 

coverage_0x28245d09(0x482c8610dd692486233181e805bb24a54b600ad0268c98c736d75c5dd401ceb1); /* line */ 
        coverage_0x28245d09(0x1ff88edc2bda901a333672e8e03ee460a38b2f175a1e08e11ebd13ddc3b29529); /* statement */ 
uint96 result = uint96(number);
coverage_0x28245d09(0x36a80ddcea9ebdd0f109ef34c6ceae2a9caebaad2c2ebba7e5b57b461178c37c); /* line */ 
        coverage_0x28245d09(0x3f018741beb34cbeffc788aa21b47cbb4383a77a8d576b4e867cd7fe25ba0439); /* statement */ 
Require.that(
            result == number,
            FILE,
            "Unsafe cast to uint96"
        );
coverage_0x28245d09(0xccedec15a3908e9446af76f70a324d0414d58c2cb85b6770720a20233b294619); /* line */ 
        coverage_0x28245d09(0x14ac2567d2af428a8a96cc7fda8881ef65b877661d1f1faf65d639ac357b40ee); /* statement */ 
return result;
    }

    function to32(
        uint256 number
    )
        internal
        pure
        returns (uint32)
    {coverage_0x28245d09(0x0abfb3e10d5feabf50698cd7cb040706f12b19a17936e2d02fbe1f4f68325f59); /* function */ 

coverage_0x28245d09(0xc2f0947b2aabd7e7056013802e1ddf74e7b3a0ebbd389ff1b267182a2768219a); /* line */ 
        coverage_0x28245d09(0xa624e4a45e60989177289e15b78894d5d796141299613fe23fdca862b396574e); /* statement */ 
uint32 result = uint32(number);
coverage_0x28245d09(0x9ada3834d7c72466d523c7131f003fe98acce76f1eb51a77d17753891eb178df); /* line */ 
        coverage_0x28245d09(0xeaa97be24f5d507fa55485bcea3eebced16e5b4fd0a79a1c243e5a4241b05ab9); /* statement */ 
Require.that(
            result == number,
            FILE,
            "Unsafe cast to uint32"
        );
coverage_0x28245d09(0x027de05ed62f094bbd8a98d0164966ff2fae14046f54a63c430a8879ff18697b); /* line */ 
        coverage_0x28245d09(0xc26b459ed1abc1c42e614de56f1f0653c645af718fc26933bedde44c13afafaa); /* statement */ 
return result;
    }

    function min(
        uint256 a,
        uint256 b
    )
        internal
        pure
        returns (uint256)
    {coverage_0x28245d09(0x911329cbd4d50aace3efee68e95aada4894816dc9325f785ba4ae21dbab7fe47); /* function */ 

coverage_0x28245d09(0xc482e49106aab8a9b2eff5b4e8ba1240a2f954f97b47ec5b7bcebb0f3635cc69); /* line */ 
        coverage_0x28245d09(0x6f70282a35a17a6908482d39d5c8c2e7ddc35386e4ece20f0779e071d17f7f5c); /* statement */ 
return a < b ? a : b;
    }

    function max(
        uint256 a,
        uint256 b
    )
        internal
        pure
        returns (uint256)
    {coverage_0x28245d09(0xeaa6dcee0730ac46ce71e917c2e856ed1044bcd181c78fcafdf9b2faae9b1088); /* function */ 

coverage_0x28245d09(0x603cd9ffe0149dc147f63e57a680058a01e5d2fd237cc187eaf8d73c5dac6d36); /* line */ 
        coverage_0x28245d09(0x349ebe1326598a3c74ab202e2fe535b8704104730e889917391e8f8d9a6a431e); /* statement */ 
return a > b ? a : b;
    }
}
