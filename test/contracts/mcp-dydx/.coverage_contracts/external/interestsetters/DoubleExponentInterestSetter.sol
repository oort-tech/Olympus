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
 * @title DoubleExponentInterestSetter
 * @author dYdX
 *
 * Interest setter that sets interest based on a polynomial of the usage percentage of the market.
 * Interest = C_0 + C_1 * U^(2^0) + C_2 * U^(2^1) + C_3 * U^(2^2) ...
 */
contract DoubleExponentInterestSetter is
    IInterestSetter
{
function coverage_0x833b364c(bytes32 c__0x833b364c) public pure {}

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
    {coverage_0x833b364c(0x10a0ee2e41c4cc2c019816b3ea9ea8aaa98e2c0ef4e0f2e2aeba46d07959abd9); /* function */ 

        // verify that all coefficients add up to 100%
coverage_0x833b364c(0x2158ba88d66b48a3223f2db4f81ccd04a13bcff7c683fe4e4fa42bdcd11f3083); /* line */ 
        coverage_0x833b364c(0x5daa2ea83fa3864657e84e7bb89fff6990cfc61eb1e6ec00fe6664086cf2f3d6); /* statement */ 
uint256 sumOfCoefficients = 0;
coverage_0x833b364c(0xeb15aae0c83ece46991e00f453dce7cb2158dcafe40e997bf48b6b166d46bcf0); /* line */ 
        coverage_0x833b364c(0x80365f54e9aaf284a3e969e641a838b965f7b9b60f251f14e36d83242fca7211); /* statement */ 
for (
            uint256 coefficients = params.coefficients;
            coefficients != 0;
            coefficients >>= BYTE
        ) {
coverage_0x833b364c(0xbf94539ad277c1dcf0be7ceb1e397b10ef09202d6f614cdb0b158722923ae1ca); /* line */ 
            coverage_0x833b364c(0x223b97c84c5ae6d62920fe236b3644d33a559b24214508d679ff1c3377d8a8fc); /* statement */ 
sumOfCoefficients += coefficients % 256;
        }
coverage_0x833b364c(0x1f8021c15f41fa334f72cfde6977e217032c2ea4709d75d1f0088ff472daaccf); /* line */ 
        coverage_0x833b364c(0xa447dcc5443f1f12e683c1673056812856e5d18f95d722af38b095190714d015); /* assertPre */ 
coverage_0x833b364c(0xb97e6afa1818832af16453bcf0290854b570102c04107db25f85d38a1ee95be3); /* statement */ 
require(
            sumOfCoefficients == PERCENT,
            "Coefficients must sum to 100"
        );coverage_0x833b364c(0x9aab36fe308d51f36a61d9ff71a0dfc8d0b87368884a2d7ed553115a49c47f05); /* assertPost */ 


        // store the params
coverage_0x833b364c(0x584a7ac7aaacc62052fd87da0f1a538b6892a4c2555076e6520dcab6baeb5515); /* line */ 
        coverage_0x833b364c(0xa7e051300345f6fbfbc5dabc477de25579b8897bb54de160d28ad5a0e0689fa3); /* statement */ 
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
    {coverage_0x833b364c(0x35c0036375e3bb0f4a02386aea036f279331330e08de1295bcbcd1536ce2be77); /* function */ 

coverage_0x833b364c(0xf0fa24a820b9ce864c34e2554051ce7619d71697f26f2c6fd2969ed351dc9d3b); /* line */ 
        coverage_0x833b364c(0x69266dbc43cafab292cc89182ae04da52db0a3063d0d24eb6ea08a48aa72ca3a); /* statement */ 
if (borrowWei == 0) {coverage_0x833b364c(0x60d208af2b7a50bf6aad4a813ad40abaeca4a1474f3fd573085fafea01bfa272); /* branch */ 

coverage_0x833b364c(0x011fbd10f96fd774385f6093163d9addd46822c700eceeae335fe86ecdb8be1e); /* line */ 
            coverage_0x833b364c(0x4f89c1637f6591a21c7a4751581af468bd20b03639e82ebe9da6a0a20cdda755); /* statement */ 
return Interest.Rate({
                value: 0
            });
        }else { coverage_0x833b364c(0x7bbc4b62b4c3d6e4a057b5ae3a42a012e86ac6f842a6b1e64433e67cc465c502); /* branch */ 
}

coverage_0x833b364c(0x180f960c490a0046c2f6f4915626b3b9042eb9768de5b08cf0fbba026b6910e5); /* line */ 
        coverage_0x833b364c(0x3fa591a237882166ba8fd2f969a7e4c89b523e2c476bf44ab390d42f6c56edfb); /* statement */ 
PolyStorage memory s = g_storage;
coverage_0x833b364c(0xa32f2fcbd2dfd96e8d92fb781919b260f901a995085a111692aacf28d6eb51a6); /* line */ 
        coverage_0x833b364c(0xc04a00dcee6899b8e2a4f0d1d08ae759b2e8295ba3238a656abd0a2aa81d13b0); /* statement */ 
uint256 maxAPR = s.maxAPR;

coverage_0x833b364c(0xaa4e0ae7ea2b0290fb8dd7b092df3098b2967e563eed655f3c63f9f3f63c36e3); /* line */ 
        coverage_0x833b364c(0xc6639ea495007d3347618ae9c0e913cfe433c9b7a7170f27f6525956a3006da3); /* statement */ 
if (borrowWei >= supplyWei) {coverage_0x833b364c(0x13a5fe3958ac50cb8567678371373ddcc5efefeffc7d6f4e46466e7fce7d7fe2); /* branch */ 

coverage_0x833b364c(0xb301ce48c1aa13769761135ee1db15b54fa9ee921ec1a560b27acfe151e6fc4c); /* line */ 
            coverage_0x833b364c(0x8da3f4151da21d21971abae368059648d0436858b104186da4085d0b8028d4a6); /* statement */ 
return Interest.Rate({
                value: maxAPR / SECONDS_IN_A_YEAR
            });
        }else { coverage_0x833b364c(0xe63553efb441eeb79f201593c6a2054c5b117ab8e862050a42512369b121773c); /* branch */ 
}

        // process the first coefficient
coverage_0x833b364c(0x7369218c224846e7b44bea6789bcf072c410cd37b6da6013314ee9e4d7c8ba4e); /* line */ 
        coverage_0x833b364c(0x2d54dcdf985969ea88e3bb1589194bebbfc3393c0f57199316e61917016bad31); /* statement */ 
uint256 coefficients = s.coefficients;
coverage_0x833b364c(0xbce4cb34eb1fcb1a176aa891ddf59dda41db54642e4a90d62e939189d7cf193e); /* line */ 
        coverage_0x833b364c(0x5df84a06c52489ea96e399b2172d6d1c72de612ae0a7f19c01ca7dac1245fa06); /* statement */ 
uint256 result = uint8(coefficients) * BASE;
coverage_0x833b364c(0xb19ec022f7e46c26f4ab0d1533e1e70444e9321839ce010e60f90e1bbf6134ce); /* line */ 
        coverage_0x833b364c(0xf463438fc92fdaeaf462d4004f363c5fba203d30b58e598733df2286bfcf7280); /* statement */ 
coefficients >>= BYTE;

        // initialize polynomial as the utilization
        // no safeDiv since supplyWei must be non-zero at this point
coverage_0x833b364c(0xefcc7a1c6ee9738c45beabd461325321df7c8e32754ef758fde834644a3d7afd); /* line */ 
        coverage_0x833b364c(0x88573a212f11893bc9739c704836e989b2692d07b59a10d48103591fd91b5953); /* statement */ 
uint256 polynomial = BASE.mul(borrowWei) / supplyWei;

        // for each non-zero coefficient...
coverage_0x833b364c(0x9ee2a8e568d8ad83a5a2364f60ac1972499b7fe8f842c955b12c15e680735e91); /* line */ 
        coverage_0x833b364c(0x6d85a0969f880caefec07f179a77bf207d5788de0e59265d0b4725e2e57a7d83); /* statement */ 
while (true) {
            // gets the lowest-order byte
coverage_0x833b364c(0xc327ed45850cc20088fc3371d6006d1d81566b2b85cfb7eff5353db5cdced09d); /* line */ 
            coverage_0x833b364c(0x4d34f46a3cbe8b7533a0be408ee0f7e7c8bda01705e716c9b2993d312ed5e411); /* statement */ 
uint256 coefficient = uint256(uint8(coefficients));

            // if non-zero, add to result
coverage_0x833b364c(0x2a92ebd69d6ac233282c3c465451bece86391c4d44e52789813577b4937a40ba); /* line */ 
            coverage_0x833b364c(0xbb0ad3bb9d19320e4103e527f9d77335b0424e8053a7c321210cda4c29988e02); /* statement */ 
if (coefficient != 0) {coverage_0x833b364c(0x965688381b0ff60379edfd805872224e26ca48443b1e5b5f0826120ef1150e39); /* branch */ 

                // no safeAdd since there are at most 16 coefficients
                // no safeMul since (coefficient < 256 && polynomial <= 10**18)
coverage_0x833b364c(0x09c6d2ba43df189e1de82bfd40a7f7c7247a3074ae1376fbbcde34e1e203e4b9); /* line */ 
                coverage_0x833b364c(0x20d101125268ce15b6106e662ddd37e87384cff66128bc55ee252ef0ee7817cc); /* statement */ 
result += coefficient * polynomial;

                // break if this is the last non-zero coefficient
coverage_0x833b364c(0x815e39c36ce660d77c4b104e2054eec84dcc31006288341f7e228024172fc111); /* line */ 
                coverage_0x833b364c(0x4cf9754e0324a7f21fea4ef53b0c6d5f0d142a4d571e76af4ff48d63163fffde); /* statement */ 
if (coefficient == coefficients) {coverage_0x833b364c(0x732f8affee947562055fff9294e5930b3e8a983d6f82397865b004e1ec956efb); /* branch */ 

coverage_0x833b364c(0xc0c40e9fa38173dd88fe12524cd17a54888447009917d03c086ac95375b66fee); /* line */ 
                    break;
                }else { coverage_0x833b364c(0xeab059487d6018ea5266a842c6315efce0cc0ab0064ff16f0892fcef7ea249e7); /* branch */ 
}
            }else { coverage_0x833b364c(0x6f908f2f24bc96fb9c9bb2c21c48c8b2c9508a93ffa7f60e103a47f7f769a0cc); /* branch */ 
}

            // double the order of the polynomial term
            // no safeMul since polynomial <= 10^18
            // no safeDiv since the divisor is a non-zero constant
coverage_0x833b364c(0x49610b1b43d703504fb845a9952935d7981a3716642ad989965786cb3f58730b); /* line */ 
            coverage_0x833b364c(0xbc4633988169ef7139aa91bdc0e4ab0f4cfeaf7b0fefe2a6b6acc5284d48d287); /* statement */ 
polynomial = polynomial * polynomial / BASE;

            // move to next coefficient
coverage_0x833b364c(0x83be4453af26a36f5a6a38d435bd2d7117a179f46da30780ab42a05d04e4cf16); /* line */ 
            coverage_0x833b364c(0x544602ec5e476a0705fe7c446bc9b9e613a8a3db723064c28adf86ee0bae4e13); /* statement */ 
coefficients >>= BYTE;
        }

        // normalize the result
        // no safeMul since result fits within 72 bits and maxAPR fits within 128 bits
        // no safeDiv since the divisor is a non-zero constant
coverage_0x833b364c(0x11cf1f3b8b900bf148fbddc03ca4ad269c7276c1bceda8c530ad85f99dd5fa83); /* line */ 
        coverage_0x833b364c(0x9f384e81ee5fd30b7e7d79eb6c831cd129e07f60b89ea3dcb4462b03ab4c100c); /* statement */ 
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
    {coverage_0x833b364c(0x4acda9ddb696e299b59ed0a156c74a499896bf9acf4be2aab818b8d4145890ec); /* function */ 

coverage_0x833b364c(0x9c69187130ad3bcdc2d209a6128e1a36b6bd006083136f1a2f806974aff89ece); /* line */ 
        coverage_0x833b364c(0x9611910c86b135ef9972a92c2a192e5f128fa446e4e88bd0a5ef3c29495e33d5); /* statement */ 
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
    {coverage_0x833b364c(0x0040b534dfe662cc49fe19c71c793abbe2845bd4de71d314138c2aef76db92d5); /* function */ 

        // allocate new array with maximum of 16 coefficients
coverage_0x833b364c(0xc810c5cdd484b7c6d79ecab6881e877b92baf9054fa9c83e5ccfd4fea1a74a6a); /* line */ 
        coverage_0x833b364c(0x2ad9d4b57e7770f8429b3d9da922cdab59cee5ccf524f15f70b887e9d018edb0); /* statement */ 
uint256[] memory result = new uint256[](16);

        // add the coefficients to the array
coverage_0x833b364c(0xa0477c340c25b0be9d5a85a2af1b529d99b6f4e6feefe948f5120c1ff982b054); /* line */ 
        coverage_0x833b364c(0x07e33f558d38a4cc2905f6631dce6056b1d1e815ed18c77eca03b52938d0ba8c); /* statement */ 
uint256 numCoefficients = 0;
coverage_0x833b364c(0x465ce1ea471630e8898ff8af02b72294fbc51b4c1810246ad17220cbf3a5129d); /* line */ 
        coverage_0x833b364c(0x5fdce42b1cad0867999b682006a6db4f75fa822dd1c97604e3f20f44749845f6); /* statement */ 
for (
            uint256 coefficients = g_storage.coefficients;
            coefficients != 0;
            coefficients >>= BYTE
        ) {
coverage_0x833b364c(0x7c4b6b42ff87b8c4f0a3ffc95e6c904df66022d88e94baa7b2e8ba6f91aded34); /* line */ 
            coverage_0x833b364c(0xb5ea7cb13e096fa5a3d3e054f78f432cdf5c3c6bfbef956fea61d3203a480db2); /* statement */ 
result[numCoefficients] = coefficients % 256;
coverage_0x833b364c(0x9c19eef4c9d14bc9b7d62a5c7d675b882e8d0acae3e19de9ef36836c07957e14); /* line */ 
            numCoefficients++;
        }

        // modify result.length to match numCoefficients
        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x833b364c(0xf77416e78d84e1bd83d238b78bb220f5b00e3cd32ef0833ced43879b07a07a6c); /* line */ 
        assembly {
            mstore(result, numCoefficients)
        }

coverage_0x833b364c(0x6f9ff1c0c2fbc179f25df864487c409c4673b43a5b1605511ea9464ddc657f9c); /* line */ 
        coverage_0x833b364c(0xcc2f5c4e7d4d7ed978ea1722835099050542ddda7e53b7a1931b53a6c7ec4c00); /* statement */ 
return result;
    }
}
