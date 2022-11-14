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
import { IInterestSetter } from "../../protocol/interfaces/IInterestSetter.sol";
import { Interest } from "../../protocol/lib/Interest.sol";
import { Math } from "../../protocol/lib/Math.sol";


/**
 * @title PolynomialInterestSetter
 * @author dYdX
 *
 * Interest setter that sets interest based on a polynomial of the usage percentage of the market.
 */
contract PolynomialInterestSetter is
    IInterestSetter
{
function coverage_0xb2489445(bytes32 c__0xb2489445) public pure {}

    using Math for uint256;
    using SafeMath for uint256;

    // ============ Constants ============

    uint256 constant PERCENT = 100;

    uint256 constant BASE = 10 ** 18;

    uint256 constant SECONDS_IN_A_YEAR = 60 * 60 * 24 * 365;

    uint256 constant BYTE = 8;

    // ============ Structs ============

    struct PolyStorage {
        uint128 maxAPR;
        uint128 coefficients;
    }

    // ============ Storage ============

    PolyStorage g_storage;

    // ============ Constructor ============

    constructor(
        PolyStorage memory params
    )
        public
    {coverage_0xb2489445(0x632a488a71d058b1568a22136742727fa5f2140cbcf0499eaf871b57f795c2cd); /* function */ 

        // verify that all coefficients add up to 100%
coverage_0xb2489445(0x1b2619a1986846995d68dae30798ad7a3621d26edafa6a8b50774e269500d1b6); /* line */ 
        coverage_0xb2489445(0x3cd1afc46513ac1d63bbcbb6f8d7150e74f7f091c1af9d717678b39c4d4dd89e); /* statement */ 
uint256 sumOfCoefficients = 0;
coverage_0xb2489445(0x1a95fba5d1c83e140a96d4c921c21a81c183e8921e6f18af68ff2396cdabf6d7); /* line */ 
        coverage_0xb2489445(0x188f1237911b25c61a6868bca238bd7f38ff89aa71051dfc02abd9489adc81c3); /* statement */ 
for (
            uint256 coefficients = params.coefficients;
            coefficients != 0;
            coefficients >>= BYTE
        ) {
coverage_0xb2489445(0x27204f85be042ff6d7ef76a6cd26b8236f1aabef8df6b5c8e9bd5c31a109437f); /* line */ 
            coverage_0xb2489445(0x5b1e5953c7ecbe7479e6c0feeaca9a4a847e198b1c43da3acf33d1c236c0b676); /* statement */ 
sumOfCoefficients += coefficients % 256;
        }
coverage_0xb2489445(0x2d4f75cb5b1df682cf38993ad0eaeb02883ee1d39e4d18ca094a8124a26ff4fa); /* line */ 
        coverage_0xb2489445(0x9fb4ea90cd5c32c2741c85ca579f472cfcd3b717340bbebb7e12d4906bd91f32); /* assertPre */ 
coverage_0xb2489445(0xc9218291aab79d36042ed0628da862b6213d0ecd04a1c26b40dec7420b0ada7d); /* statement */ 
require(
            sumOfCoefficients == PERCENT,
            "Coefficients must sum to 100"
        );coverage_0xb2489445(0xa8ae0a3609e7d8595baa380fe5ae70c83c878b9b6ac96446e437674d0a8924bb); /* assertPost */ 


        // store the params
coverage_0xb2489445(0x41640501cb250cfe0fb711f928c7e532afc1a997086b74f444b834487b2cb0f9); /* line */ 
        coverage_0xb2489445(0x971692fb67262fc47452925563ee08e0da72b80374a8fc06bcdc661bc214cd14); /* statement */ 
g_storage = params;
    }

    // ============ Public Functions ============

    /**
     * Get the interest rate given some borrowed and supplied amounts. The interest function is a
     * polynomial function of the utilization (borrowWei / supplyWei) of the market.
     *
     *   - If borrowWei > supplyWei then the utilization is considered to be equal to 1.
     *   - If both are zero, then the utilization is considered to be equal to 0.
     *
     * @return The interest rate per second (times 10 ** 18)
     */
    function getInterestRate(
        address /* token */,
        uint256 borrowWei,
        uint256 supplyWei
    )
        external
        view
        returns (Interest.Rate memory)
    {coverage_0xb2489445(0x70571bc75c28dc514e96c9c8b9b2b16baa61053a7051a8abff026796c904ddfc); /* function */ 

coverage_0xb2489445(0x52598fdc56aacfe2f3876188e011fad2d45b9ee4344e39356159516b5d2e9842); /* line */ 
        coverage_0xb2489445(0xe5752698ac1801c87b6371948c753f487884f3dd80cdfcf7e1109cc7154e787d); /* statement */ 
if (borrowWei == 0) {coverage_0xb2489445(0xda5b02063836432372d9add4a0c862b72414cc8dcf514dfe92c006a4d08b49c8); /* branch */ 

coverage_0xb2489445(0x8bafba91a3f8e0de73902ff8bc37edcbfa39ce74cc7ec3382e9a2bd6ef4ef6a8); /* line */ 
            coverage_0xb2489445(0x304741abbb702b473458eee988a52884b8047372e357de9f43d8bf2bce9eaf51); /* statement */ 
return Interest.Rate({
                value: 0
            });
        }else { coverage_0xb2489445(0x4b0b1c9d0bd311f4409b8e54692b923d70f835aaa2fc4638c36c287f7167d211); /* branch */ 
}

coverage_0xb2489445(0xa02b02090a66aad4b20b8f312dc7daaa56f85a862cdaefa2fcb59ae97f227105); /* line */ 
        coverage_0xb2489445(0x7133d052b2a6c23e5196dde7ec55413fa280cf9b2a6b1e5ff55c96fc3135767e); /* statement */ 
PolyStorage memory s = g_storage;
coverage_0xb2489445(0xfc016993292246b08eb0f3f2970bc22a8c496cf18b89f38d64aad0cbb89d0bcd); /* line */ 
        coverage_0xb2489445(0xa45da554f197fec2d024daa222ada9b7b5e5a6132a1ba59af9b411b5298a834b); /* statement */ 
uint256 maxAPR = s.maxAPR;

coverage_0xb2489445(0xf9b4aa4fb6c51886b18d1b1853d601add2667c34b9b96244cd716cf937a77a29); /* line */ 
        coverage_0xb2489445(0xfa1c5c201b925f24a331254b345ccca0e6656af47b3c6c968b94ce9a5a12d119); /* statement */ 
if (borrowWei >= supplyWei) {coverage_0xb2489445(0xa65b553062eed1ba722a00c0f475282ab8fc823ac0999743185716329458bab4); /* branch */ 

coverage_0xb2489445(0x0980db0d8a7d967cc417a46310c60301b95dff929a6683e7bf782710fea1adb5); /* line */ 
            coverage_0xb2489445(0x3d7595526159fd4d23a48f8a95a93d0b06f73e7be1c4ea7c3b083bcda87bd90a); /* statement */ 
return Interest.Rate({
                value: maxAPR / SECONDS_IN_A_YEAR
            });
        }else { coverage_0xb2489445(0x289d4642b4af1c0325ae97ed5b2d759a8e125ddf98c1dfdd4b0c340249faf5c1); /* branch */ 
}

coverage_0xb2489445(0x3e7a26d560565624e3e4083f36f697d5c6e4a2caed6b9dc7e96b124c3199df7b); /* line */ 
        coverage_0xb2489445(0x5a5d708d10b2d3ccd970391ca9b3a4ae26acabda1d2898e71e4fb56a8ddfa63d); /* statement */ 
uint256 result = 0;
coverage_0xb2489445(0x73581018f48d3200f4b5b47581f39fc74ffa80f2670276e0acebda78394d3627); /* line */ 
        coverage_0xb2489445(0xc5f29393a0e901a3462cf3432a2f385b5a9cd2ca930ec858a4d7b716ef48b84a); /* statement */ 
uint256 polynomial = BASE;

        // for each non-zero coefficient...
coverage_0xb2489445(0x9384a2b4a35a32957b2c0c750b81eeed02641213fa5b8c8fde029682290af8db); /* line */ 
        coverage_0xb2489445(0xb1273632911324897ea5bf9c90623796899fa2d0ef7e5ba58e9935688e9e7859); /* statement */ 
uint256 coefficients = s.coefficients;
coverage_0xb2489445(0x8a9060b42926dc956955a7ad66239b79ea236292807283976b14fd89ee950b3a); /* line */ 
        coverage_0xb2489445(0xf069585a4fa1014bd3e7e479382d720dd49f78ae28a2bdc1d6a97a13a393712c); /* statement */ 
while (true) {
            // gets the lowest-order byte
coverage_0xb2489445(0x96dfd02ebae74eefefac200f593bf6984d4be7fde5e453130729442742896dfb); /* line */ 
            coverage_0xb2489445(0xf173cc54a945ea3455290e1a55de7cd1b25473a48ce610cc4c32dc525a9336e7); /* statement */ 
uint256 coefficient = coefficients % 256;

            // if non-zero, add to result
coverage_0xb2489445(0x9486c67b7869e2bed2de427b42af50c189ad3cf075d2d83397b9e50c575a7afb); /* line */ 
            coverage_0xb2489445(0xc38235fab50b798322e7e5d90a65ac67ae12342fdc081369abed67ad4c6d2e2d); /* statement */ 
if (coefficient != 0) {coverage_0xb2489445(0xf30c0ad052478b7776cb9ab2b390bfebd2bf0c2cd38855b2adaab008edfdc970); /* branch */ 

                // no safeAdd since there are at most 16 coefficients
                // no safeMul since (coefficient < 256 && polynomial <= 10**18)
coverage_0xb2489445(0x3715b1a28c61f88346966b170fdca71e29032f770d443e303702288ba178a2f8); /* line */ 
                coverage_0xb2489445(0x4b6b8bcf37ba1d24cbc99071b69cf70a4a8af6cf9ce5fac8c4f74a8fc563e8c4); /* statement */ 
result += coefficient * polynomial;

                // break if this is the last non-zero coefficient
coverage_0xb2489445(0x13d3364c43e5012e18fb0c5d2ab2b190de498893c23babee40e839bed7b7859f); /* line */ 
                coverage_0xb2489445(0xaeec99c83cb89b6351438b737dff94b8578e437855992ef11db4846fd02356e2); /* statement */ 
if (coefficient == coefficients) {coverage_0xb2489445(0x8eb7a8a51240cd4322154b928109d2533648d67447196f686836627d3b179e32); /* branch */ 

coverage_0xb2489445(0x5707bd6dd3119a6391250db0c465904da9e35f66981025e33ae618dd4472f665); /* line */ 
                    break;
                }else { coverage_0xb2489445(0xa4aea63e7c8ee0d3259eb2fd07f79dc70de5db3fa6b17090e408d7b1959b0801); /* branch */ 
}
            }else { coverage_0xb2489445(0x42ed0a0b10bf317964bc94b83e5f0844202e0ec4727c226bae2bc1c67d95033b); /* branch */ 
}

            // increase the order of the polynomial term
            // no safeDiv since supplyWei must be stricly larger than borrowWei
coverage_0xb2489445(0x7fe77a8c7c41baac4270ee42d89fb9c9959adfa5d30af91aa9c8fe122bb9c3ce); /* line */ 
            coverage_0xb2489445(0xd57612d4cca154583e2bd1f9bf4079b128528a9849949d0fa4e188c4921fbe39); /* statement */ 
polynomial = polynomial.mul(borrowWei) / supplyWei;

            // move to next coefficient
coverage_0xb2489445(0x79ce5b91c529e9eae99d177ff7d27024607183af86fa5117bb540f17d4d637c3); /* line */ 
            coverage_0xb2489445(0xcd35d553862be65dd73817718af7c64b57116c9ffa93a42d1186928090e527c6); /* statement */ 
coefficients >>= BYTE;
        }

        // normalize the result
        // no safeMul since result fits within 72 bits and maxAPR fits within 128 bits
        // no safeDiv since the divisor is a non-zero constant
coverage_0xb2489445(0x61a05be9c4b40a11f6cdd458c10a75ba37fbb57000718b39ba03899a2299524e); /* line */ 
        coverage_0xb2489445(0x320f8a2ae69cbca9dc06e11359afddfe45fc6818b367f0db763194cfa1555fb7); /* statement */ 
return Interest.Rate({
            value: result * maxAPR / (SECONDS_IN_A_YEAR * BASE * PERCENT)
        });
    }

    /**
     * Get the maximum APR that this interestSetter will return. The actual APY may be higher
     * depending on how often the interest is compounded.
     *
     * @return The maximum APR
     */
    function getMaxAPR()
        external
        view
        returns (uint256)
    {coverage_0xb2489445(0x65ec2236a2f2f60b3c05576b7f56b362dfd64b6b5882d70927a4e43a1b026501); /* function */ 

coverage_0xb2489445(0x10ba6c3dcee1dc8bb1902be0f34225a714eb4225382cffbe879ebe497f838df6); /* line */ 
        coverage_0xb2489445(0x581021add169aa92077556f3af386444ecacd6ca2fa6de858d72b27e33fceada); /* statement */ 
return g_storage.maxAPR;
    }

    /**
     * Get all of the coefficients of the interest calculation, starting from the coefficient for
     * the first-order utilization variable.
     *
     * @return The coefficients
     */
    function getCoefficients()
        external
        view
        returns (uint256[] memory)
    {coverage_0xb2489445(0x8e49ec2d195c1477db0833c684c40878b14e0744f46c5e426371ee2c2f13fefe); /* function */ 

        // allocate new array with maximum of 16 coefficients
coverage_0xb2489445(0x0494617cc99f6f62018edd65d880dde8c334eff3297fd7207d134886db7afb56); /* line */ 
        coverage_0xb2489445(0x97ea0b1d6992ac7c9e0e1b0e00a3623fdace653b7351e6588dc0063f0997d129); /* statement */ 
uint256[] memory result = new uint256[](16);

        // add the coefficients to the array
coverage_0xb2489445(0x167ce67bb461b02c1f31ade927cdb3931d1c2041c4a47221b3b03a8b0811860c); /* line */ 
        coverage_0xb2489445(0x5ce0db4962040ec9d17be023d558da377db8aadd471ac2e648cbff618fe75414); /* statement */ 
uint256 numCoefficients = 0;
coverage_0xb2489445(0xcee828033f654a0e3cec2fc277e1978df19d9ea640d299ef797b05e92462a702); /* line */ 
        coverage_0xb2489445(0xc9f47c5fe04284a3485c711f07b3cafd766688d511c8f58ce47dd33981044f7d); /* statement */ 
for (
            uint256 coefficients = g_storage.coefficients;
            coefficients != 0;
            coefficients >>= BYTE
        ) {
coverage_0xb2489445(0x1b3689292e12a60ad069ab9ff7ee679edc46d548de4d51000b0e2e206d26d94f); /* line */ 
            coverage_0xb2489445(0x4a205593ee67db3ca6cc828bc30abb7c8d3428cf8590636865b4972480bc1090); /* statement */ 
result[numCoefficients] = coefficients % 256;
coverage_0xb2489445(0xc549f3a59faefac870b373bfc1a8dc791062838ff7b76b6c2dba1054d82c0f81); /* line */ 
            numCoefficients++;
        }

        // modify result.length to match numCoefficients
        /* solium-disable-next-line security/no-inline-assembly */
coverage_0xb2489445(0xa49887daa2c2d7103a29360313425679281ca91e15996509ea4f212d86b1555e); /* line */ 
        assembly {
            mstore(result, numCoefficients)
        }

coverage_0xb2489445(0x51c7e83f905c35a6797dcac3294d7ac0ddca23615a8dfec9ee6e4e7614da70f3); /* line */ 
        coverage_0xb2489445(0x7ff96402b3d39f52e939a04bf22512e2ebb64010f7c544dc6e425a6f904d8006); /* statement */ 
return result;
    }
}
