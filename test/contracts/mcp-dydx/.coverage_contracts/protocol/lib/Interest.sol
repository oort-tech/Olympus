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
import { Decimal } from "./Decimal.sol";
import { Math } from "./Math.sol";
import { Time } from "./Time.sol";
import { Types } from "./Types.sol";


/**
 * @title Interest
 * @author dYdX
 *
 * Library for managing the interest rate and interest indexes of Solo
 */
library Interest {
function coverage_0x3adcdafa(bytes32 c__0x3adcdafa) public pure {}

    using Math for uint256;
    using SafeMath for uint256;

    // ============ Constants ============

    bytes32 constant FILE = "Interest";
    uint64 constant BASE = 10**18;

    // ============ Structs ============

    struct Rate {
        uint256 value;
    }

    struct Index {
        uint96 borrow;
        uint96 supply;
        uint32 lastUpdate;
    }

    // ============ Library Functions ============

    /**
     * Get a new market Index based on the old index and market interest rate.
     * Calculate interest for borrowers by using the formula rate * time. Approximates
     * continuously-compounded interest when called frequently, but is much more
     * gas-efficient to calculate. For suppliers, the interest rate is adjusted by the earningsRate,
     * then prorated the across all suppliers.
     *
     * @param  index         The old index for a market
     * @param  rate          The current interest rate of the market
     * @param  totalPar      The total supply and borrow par values of the market
     * @param  earningsRate  The portion of the interest that is forwarded to the suppliers
     * @return               The updated index for a market
     */
    function calculateNewIndex(
        Index memory index,
        Rate memory rate,
        Types.TotalPar memory totalPar,
        Decimal.D256 memory earningsRate
    )
        internal
        view
        returns (Index memory)
    {coverage_0x3adcdafa(0x6c8abd5b9104fecf718945c6f6ccf3303d4119b690371ad1d6ce9e2ebb6268b4); /* function */ 

coverage_0x3adcdafa(0x39cc5cbb88bf700582fd7019a4c3866cdbd335cc814a35fe8c8630043434fe37); /* line */ 
        coverage_0x3adcdafa(0x472c1c03c21b157b7f726f302775257eb60b9979081d264ce3d28f7fbced3e7d); /* statement */ 
(
            Types.Wei memory supplyWei,
            Types.Wei memory borrowWei
        ) = totalParToWei(totalPar, index);

        // get interest increase for borrowers
coverage_0x3adcdafa(0x5ab6029ee4a1b744712497029ba7df5d82f97b7981ca741b26c844f10aa17505); /* line */ 
        coverage_0x3adcdafa(0x1615c59bd247028e485428522e4e18c3e03e4c31c36b0060dea2ef174098b71d); /* statement */ 
uint32 currentTime = Time.currentTime();
coverage_0x3adcdafa(0x89092133c7828c76b4819a4fcf1b8e208b4fd7416801570109e3f9bd7f09f87c); /* line */ 
        coverage_0x3adcdafa(0xdba963aae84694c13be892a4bec9ba464e6f73b3c29a711576b4e08226e3e474); /* statement */ 
uint256 borrowInterest = rate.value.mul(uint256(currentTime).sub(index.lastUpdate));

        // get interest increase for suppliers
coverage_0x3adcdafa(0x920b4a2115d749d035b39096336be4778d5e1a85f4b59c590a50cb44aba04c3a); /* line */ 
        coverage_0x3adcdafa(0xcfd4a8ada9d3172df9b9ae9d7ac9c12c3815a264891a057ca9778f78e394d3e6); /* statement */ 
uint256 supplyInterest;
coverage_0x3adcdafa(0x836e54be7cb8a422824207c00e58465b3764e5dabcb7920d2f9b1f2cfe1509a1); /* line */ 
        coverage_0x3adcdafa(0xe52391472a5acc9f606e1bca1a85832e8c4ed8c255d7d527151a031cc1e8e940); /* statement */ 
if (Types.isZero(supplyWei)) {coverage_0x3adcdafa(0x7a5a5ae9fbb83b16f1c2d6ab2a42745967edbc9b3ddbeace28e0f30876c4482c); /* branch */ 

coverage_0x3adcdafa(0x428b56fd883442a77f41739712515f4144fe03f9c1a5ad8402e7db02f8fa3af3); /* line */ 
            coverage_0x3adcdafa(0xc183ccd271c93650b1dfb41abcf32a52bad26b466f4814ba4fdd7f64da8f90db); /* statement */ 
supplyInterest = 0;
        } else {coverage_0x3adcdafa(0xc3d9392efffe71bf32dd0c664b0563700753ee940733bed7e642f3abc875f414); /* branch */ 

coverage_0x3adcdafa(0x0b2f09ecd9921d9907a7e286f30922eb36070477cc7c71d702a38b16268ddcbd); /* line */ 
            coverage_0x3adcdafa(0x9d894b1957e9874ee2d3b4ec29c23b012c10baf4c5e0a12fea485b12723de971); /* statement */ 
supplyInterest = Decimal.mul(borrowInterest, earningsRate);
coverage_0x3adcdafa(0x782371c8bdaab88e0da2884da19e7100a71bde87cb45b904313d6fef49c56343); /* line */ 
            coverage_0x3adcdafa(0xb1b9b3156f7ffcfff8d3cd4b27a23d99967069c9fdcf29538d83c2e8e75de924); /* statement */ 
if (borrowWei.value < supplyWei.value) {coverage_0x3adcdafa(0xdcd0989c6bf61a5017d4069c9c87466a3d357cc0521daf3e515055de1bc9c611); /* branch */ 

coverage_0x3adcdafa(0xecd753d62d6a0882ccd8de333cbc54ba7d4157c900bc1ffed8f4031c707fe6e9); /* line */ 
                coverage_0x3adcdafa(0x4f112331ba09a9a954e379feacab2029d8f21ed6ccea8c4b9fbd5811a86ed23c); /* statement */ 
supplyInterest = Math.getPartial(supplyInterest, borrowWei.value, supplyWei.value);
            }else { coverage_0x3adcdafa(0x9e256859cc63834e27233035f8cfaed421509b00314e66bad5310a42d6825998); /* branch */ 
}
        }
coverage_0x3adcdafa(0xe451724f27cfebbd1bd82e12b2b691699595189b13b2a38a795a8db673cfadc9); /* line */ 
        coverage_0x3adcdafa(0x5c8b8ae80bc4156e8b2983af01b5d2b5b5dd2c45375fa56284b4ff0bb83332fd); /* assertPre */ 
coverage_0x3adcdafa(0xb40aa906a09de0926649ba681572412dfea211180c921ef71720b313e69c3a08); /* statement */ 
assert(supplyInterest <= borrowInterest);coverage_0x3adcdafa(0xb14912f72c168a33a89805c1b0b507329e3caccd767e9ad9f785978edd532d88); /* assertPost */ 


coverage_0x3adcdafa(0xc3d251d48eaf1fdd59e58dc3bc0f9f16faf437630ae5acaf116f382bc5c672c9); /* line */ 
        coverage_0x3adcdafa(0x6b1fce926226bd2934fcec262300d95b2be93ca8ed54f75d01338d5a679a2aa6); /* statement */ 
return Index({
            borrow: Math.getPartial(index.borrow, borrowInterest, BASE).add(index.borrow).to96(),
            supply: Math.getPartial(index.supply, supplyInterest, BASE).add(index.supply).to96(),
            lastUpdate: currentTime
        });
    }

    function newIndex()
        internal
        view
        returns (Index memory)
    {coverage_0x3adcdafa(0x516f2090481fdf133f8436deb5f7ae32eaab4b1b850c437bb9fd68818a90ab42); /* function */ 

coverage_0x3adcdafa(0x1981729ee312752b43900cac0691edfa5f65d650dc12074ca9f2a6c6f75a4c01); /* line */ 
        coverage_0x3adcdafa(0x362390209d85622cea50c218e99b84ce0511ccbb40c0cad27e86734c011e6022); /* statement */ 
return Index({
            borrow: BASE,
            supply: BASE,
            lastUpdate: Time.currentTime()
        });
    }

    /*
     * Convert a principal amount to a token amount given an index.
     */
    function parToWei(
        Types.Par memory input,
        Index memory index
    )
        internal
        pure
        returns (Types.Wei memory)
    {coverage_0x3adcdafa(0xd44f322d049188797ce4198e5123ecd47c497d897bcb9ab94d0e846069febe89); /* function */ 

coverage_0x3adcdafa(0xd182179a1ae90859e284933039b8fa823bfea2a51e81fe68ce548fc0104ab983); /* line */ 
        coverage_0x3adcdafa(0x999b5d6953be3472dcd124284cf04d439cf2d5b4abfbeb76c2ed7b19730b2ced); /* statement */ 
uint256 inputValue = uint256(input.value);
coverage_0x3adcdafa(0x5600a3fa81736f110f55200377ef01c2844db688782a19ddfd006bc6c78166af); /* line */ 
        coverage_0x3adcdafa(0xefb87b7b3759d9cdbb5b9e50655df37553c3d0928016b3f716c5cb51801ef896); /* statement */ 
if (input.sign) {coverage_0x3adcdafa(0xc9834018701130bc4e611521937a94d64190aa3b06df2086706d86204ebe2894); /* branch */ 

coverage_0x3adcdafa(0x8d62e337b360d8f07f14810077315076a5ed0106312e1914d86424e1a66c34aa); /* line */ 
            coverage_0x3adcdafa(0x5e3f1026f7c5547a119f452cae1e6a3d87c0ad89e9e2241d1836af13d894f15e); /* statement */ 
return Types.Wei({
                sign: true,
                value: inputValue.getPartial(index.supply, BASE)
            });
        } else {coverage_0x3adcdafa(0x68003a87c0d953cc0a5ec2e370e2bb24c3a4d3093792afdd945319d396af4c04); /* branch */ 

coverage_0x3adcdafa(0x71ae629913ba44fa025da78aac2e328381371e90c9f0ce5e14ddbf73f516b234); /* line */ 
            coverage_0x3adcdafa(0x41f4316cfea65f9c64df2f20308edecf8df4fd8bf116960dc2669d701962fc1f); /* statement */ 
return Types.Wei({
                sign: false,
                value: inputValue.getPartialRoundUp(index.borrow, BASE)
            });
        }
    }

    /*
     * Convert a token amount to a principal amount given an index.
     */
    function weiToPar(
        Types.Wei memory input,
        Index memory index
    )
        internal
        pure
        returns (Types.Par memory)
    {coverage_0x3adcdafa(0x5bd4cf193831e68fbb5d043826aec6a2ab7841abdbc5de497cdf120b5b53c833); /* function */ 

coverage_0x3adcdafa(0x8b6d2827c7c444166f70a11d01008ed7d291c0ad7e0a8fe087331812bbc39774); /* line */ 
        coverage_0x3adcdafa(0xb1ad0b1d84a1fe13a2df017133a5c62eaba528b2b3e6a32f4749a8274d2aea80); /* statement */ 
if (input.sign) {coverage_0x3adcdafa(0x4dcc8b1b0e89f8dbfc031066a99c5db1ffeb035446a66f14022134abec035157); /* branch */ 

coverage_0x3adcdafa(0xea17372bcde884e610b7fe4474116ffdbab0d09211cc2aca9f804077a3c1ef33); /* line */ 
            coverage_0x3adcdafa(0x8a04d6b8ae2ac9c79a9c719f6316127e8b068d6ce60efa5be4cb78b247f87c1c); /* statement */ 
return Types.Par({
                sign: true,
                value: input.value.getPartial(BASE, index.supply).to128()
            });
        } else {coverage_0x3adcdafa(0xe8a955243e9c398a691a429947c8cb1c4e42d7f16c949eabd39fbe53a55e7b16); /* branch */ 

coverage_0x3adcdafa(0x5671f50b844bb08b3bc97252ccb0765b6e26737d2e0a50c2c295632033ffa99f); /* line */ 
            coverage_0x3adcdafa(0x71b01da6ae58c32e57e16d7f13b31c5d93187f7d7bae14235f9b4e59571f4b23); /* statement */ 
return Types.Par({
                sign: false,
                value: input.value.getPartialRoundUp(BASE, index.borrow).to128()
            });
        }
    }

    /*
     * Convert the total supply and borrow principal amounts of a market to total supply and borrow
     * token amounts.
     */
    function totalParToWei(
        Types.TotalPar memory totalPar,
        Index memory index
    )
        internal
        pure
        returns (Types.Wei memory, Types.Wei memory)
    {coverage_0x3adcdafa(0x57040ce8a811dc52d032cfdd871d48c9f7b21a8d1feeba06a5f6b96a24a16297); /* function */ 

coverage_0x3adcdafa(0xe58bd726041fed6a6e7b96b1718c7e0d17423ff1c04d5298ca255480affcb3a9); /* line */ 
        coverage_0x3adcdafa(0x5e93716556892f811489384edd5e5514ae16496df2ce62e3553b680b9708e049); /* statement */ 
Types.Par memory supplyPar = Types.Par({
            sign: true,
            value: totalPar.supply
        });
coverage_0x3adcdafa(0x0a09dd40654cf9a85d6b760641e8cf692c7063362d63c53d880b1473667a7907); /* line */ 
        coverage_0x3adcdafa(0xdfdbec2bf1ab716c89354ca00ca84119bd7d39a913c0dbdb016628672421f3fe); /* statement */ 
Types.Par memory borrowPar = Types.Par({
            sign: false,
            value: totalPar.borrow
        });
coverage_0x3adcdafa(0x520c641b3e1b7cd4f7c1f6a3b4a514cf6c8650f4e1da6f5f4fdd9a109e729ce5); /* line */ 
        coverage_0x3adcdafa(0x70e3a3218d38e9ff125fd9de632ce0825d23ce206f92a983ea3c52879ef07b87); /* statement */ 
Types.Wei memory supplyWei = parToWei(supplyPar, index);
coverage_0x3adcdafa(0x5cb6baf6a053c2e8298c492de21f361235561a368db010096dcac50718db3db4); /* line */ 
        coverage_0x3adcdafa(0x38dc3c217e375671cd00e21334e7b3d76ed9c45b1f662422c5a4c4bbe6138f4f); /* statement */ 
Types.Wei memory borrowWei = parToWei(borrowPar, index);
coverage_0x3adcdafa(0xaaf5e4efc0e425dcf04cb82aed80d2d8a85001bccdc75fd9e85989b1309f117b); /* line */ 
        coverage_0x3adcdafa(0x7d44cf6cfab502ce8db356c45ce303dadc2e055204ae581ed96432396891bc4e); /* statement */ 
return (supplyWei, borrowWei);
    }
}
