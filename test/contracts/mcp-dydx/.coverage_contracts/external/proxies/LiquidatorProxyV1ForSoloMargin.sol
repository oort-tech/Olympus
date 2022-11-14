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
import { ReentrancyGuard } from "openzeppelin-solidity/contracts/utils/ReentrancyGuard.sol";
import { SoloMargin } from "../../protocol/SoloMargin.sol";
import { Account } from "../../protocol/lib/Account.sol";
import { Actions } from "../../protocol/lib/Actions.sol";
import { Decimal } from "../../protocol/lib/Decimal.sol";
import { Interest } from "../../protocol/lib/Interest.sol";
import { Math } from "../../protocol/lib/Math.sol";
import { Monetary } from "../../protocol/lib/Monetary.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Types } from "../../protocol/lib/Types.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";


/**
 * @title LiquidatorProxyV1ForSoloMargin
 * @author dYdX
 *
 * Contract for liquidating other accounts in Solo. Does not take marginPremium into account.
 */
contract LiquidatorProxyV1ForSoloMargin is
    OnlySolo,
    ReentrancyGuard
{
function coverage_0x4962bbd4(bytes32 c__0x4962bbd4) public pure {}

    using Math for uint256;
    using SafeMath for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "LiquidatorProxyV1ForSoloMargin";

    // ============ Structs ============

    struct Constants {
        Account.Info fromAccount;
        Account.Info liquidAccount;
        Decimal.D256 minLiquidatorRatio;
        MarketInfo[] markets;
    }

    struct MarketInfo {
        Monetary.Price price;
        Interest.Index index;
    }

    struct Cache {
        // mutable
        uint256 toLiquidate;
        Types.Wei heldWei;
        Types.Wei owedWei;
        uint256 supplyValue;
        uint256 borrowValue;

        // immutable
        Decimal.D256 spread;
        uint256 heldMarket;
        uint256 owedMarket;
        uint256 heldPrice;
        uint256 owedPrice;
        uint256 owedPriceAdj;
    }

    // ============ Constructor ============

    constructor (
        address soloMargin
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x4962bbd4(0x503ee88a1ea0c5937890a31586ffedfdc2794b47ff2abb142e0d1eed37457349); /* function */ 
} /* solium-disable-line no-empty-blocks */

    // ============ Public Functions ============

    /**
     * Liquidate liquidAccount using fromAccount. This contract and the msg.sender to this contract
     * must both be operators for the fromAccount.
     *
     * @param  fromAccount         The account that will do the liquidating
     * @param  liquidAccount       The account that will be liquidated
     * @param  minLiquidatorRatio  The minimum collateralization ratio to leave the fromAccount at
     * @param  owedPreferences     Ordered list of markets to repay first
     * @param  heldPreferences     Ordered list of markets to recieve payout for first
     */
    function liquidate(
        Account.Info memory fromAccount,
        Account.Info memory liquidAccount,
        Decimal.D256 memory minLiquidatorRatio,
        uint256 minValueLiquidated,
        uint256[] memory owedPreferences,
        uint256[] memory heldPreferences
    )
        public
        nonReentrant
    {coverage_0x4962bbd4(0x7c0933295134873996fd80e1491f83107d4469e7ebd15151860e49423efa7cd3); /* function */ 

        // put all values that will not change into a single struct
coverage_0x4962bbd4(0xef71f2cffffc492a70f79b4d6b227cc44b9cc5cb186de33eadf3e0161452d590); /* line */ 
        coverage_0x4962bbd4(0x7d213f2dfccdc98836497784f3c47757849e705d0fe88eca32a4d7f7eab85059); /* statement */ 
Constants memory constants = Constants({
            fromAccount: fromAccount,
            liquidAccount: liquidAccount,
            minLiquidatorRatio: minLiquidatorRatio,
            markets: getMarketsInfo()
        });

        // validate the msg.sender and that the liquidAccount can be liquidated
coverage_0x4962bbd4(0x8cf716bef7d685cf1611453bde23bbc3277c68406f3444e9d439af0ebc41c743); /* line */ 
        coverage_0x4962bbd4(0xc6eaf7cd372c294412eb47372376035b0a5dd325107d3b61fd82500b8603575c); /* statement */ 
checkRequirements(constants);

        // keep a running tally of how much value will be attempted to be liquidated
coverage_0x4962bbd4(0x34aa8801da4c5170ddfeb95189eede4792f324b76dc3e5196ec053c1eb3684cf); /* line */ 
        coverage_0x4962bbd4(0xc14c97350cef7d8f365aaee2854699c88e5a8448360f9789a37ff918cab0e29c); /* statement */ 
uint256 totalValueLiquidated = 0;

        // for each owedMarket
coverage_0x4962bbd4(0x0f13c631135fc9b3e8625a46479d416ab8c09cd239731cb21463acbfdde3a2b8); /* line */ 
        coverage_0x4962bbd4(0xb09512fa2ecec2995df866a796a5818604752faea35822ee07a99f6aece81e24); /* statement */ 
for (uint256 owedIndex = 0; owedIndex < owedPreferences.length; owedIndex++) {
coverage_0x4962bbd4(0xac738427095279ae1c67bcd22f564d452560ac6a0e6b9d6578ed172cc0c1e807); /* line */ 
            coverage_0x4962bbd4(0x7aea92f2a17fdfb7358458b8288c714c254d0b38adcec6da828b59a4ec496233); /* statement */ 
uint256 owedMarket = owedPreferences[owedIndex];

            // for each heldMarket
coverage_0x4962bbd4(0xa2a07835a1b3042433a43c635bce41b3c350d5b5e12a3b33e97b515dee10fa02); /* line */ 
            coverage_0x4962bbd4(0x99597f829b6729d1f43fd51ed2aa024b99ef69e36ac144cdde18668d10a5a61c); /* statement */ 
for (uint256 heldIndex = 0; heldIndex < heldPreferences.length; heldIndex++) {
coverage_0x4962bbd4(0x18f75c409d190eab9697f5b917c9706652c6bb89ad08d58269273bbbea5bda90); /* line */ 
                coverage_0x4962bbd4(0xcf5cd8aa7995f92e0eb25f9fb119e961027071aa5495dcd75a34d7a63ef93c27); /* statement */ 
uint256 heldMarket = heldPreferences[heldIndex];

                // cannot use the same market
coverage_0x4962bbd4(0xe5edf47fee3d2a017515058f851289c3dd2e5be02ee2bd2b0803ac90513fadd1); /* line */ 
                coverage_0x4962bbd4(0x327777b51a6984b64f420f96326fe2aa80ccf52ee370a37e2e16dc77f737d417); /* statement */ 
if (heldMarket == owedMarket) {coverage_0x4962bbd4(0x70b605e17b81e2f8e3203fd7293cecd9db0756a0f3aa4370a330f6e7801ebe9f); /* branch */ 

coverage_0x4962bbd4(0xf5b17a73f842513030317d78c4d216173a7dfe7ffeeb6df84bb796b0abbc19ca); /* line */ 
                    continue;
                }else { coverage_0x4962bbd4(0x1cd1a67d4ac17da4b6dfdd693aecaf06c7119fe53d3777e895875c8e70c6ddd2); /* branch */ 
}

                // cannot liquidate non-negative markets
coverage_0x4962bbd4(0x33683b816ffea09790f56f03b9a7de2664fc0434832d98025192db7d4f25a8e4); /* line */ 
                coverage_0x4962bbd4(0xae155fbc228af0afe2dee3109610506db829409725b2b538e21dd0d6b0240a99); /* statement */ 
if (!SOLO_MARGIN.getAccountPar(liquidAccount, owedMarket).isNegative()) {coverage_0x4962bbd4(0x6f4303390a16981f453ea6f94182d81e4e17f2a843f95a00fd47089929558100); /* branch */ 

coverage_0x4962bbd4(0xe0fa936dd8f0daf4ed3cfbc9c617fa2d2d829696563571557b6c493d5d394f54); /* line */ 
                    break;
                }else { coverage_0x4962bbd4(0xf2d2dbcd03e8982946e647bf9d141141e7cdfce12ff7e2ec72fa11b73d86e0de); /* branch */ 
}

                // cannot use non-positive markets as collateral
coverage_0x4962bbd4(0x2cb4c6cba875abd10b60910001f2cbed21fe4c80d7e5bf95932e4b618ff84116); /* line */ 
                coverage_0x4962bbd4(0xcff3ff5edd5e7526db06b72da9270552fc03f78842f6eaf8e1e33768ea53a830); /* statement */ 
if (!SOLO_MARGIN.getAccountPar(liquidAccount, heldMarket).isPositive()) {coverage_0x4962bbd4(0x911344212c604b682c0b134de9f1aa3bc1d0341a5bf9aad44e8f50ef5fecb56e); /* branch */ 

coverage_0x4962bbd4(0x52a8f8c9961dd9d1550199e4cd9015b71b25dfb9286546f776456e9c7b6c0966); /* line */ 
                    continue;
                }else { coverage_0x4962bbd4(0x009764e10ea6a4ff86244fc5853ac2a95fefed596a2ea3d02328e29fed1cf358); /* branch */ 
}

                // get all relevant values
coverage_0x4962bbd4(0x09f30fd167806a13cda0439968b4bc2e98ead466874f829751eefdf837fc200a); /* line */ 
                coverage_0x4962bbd4(0x6620ced388a8889c87e42264b0b58efc1a9b5354dee474e0f71bc9cbc0da0275); /* statement */ 
Cache memory cache = initializeCache(
                    constants,
                    heldMarket,
                    owedMarket
                );

                // get the liquidation amount (before liquidator decreases in collateralization)
coverage_0x4962bbd4(0xa3f9c7da0ab04e0a5b445c54ffc4cb8d2ebe75ce89c0b9c65609925957ae79df); /* line */ 
                coverage_0x4962bbd4(0x869a405acef1c4a120f1a6d365297d6ca89f61358822c8ee41c538ca288d3088); /* statement */ 
calculateSafeLiquidationAmount(cache);

                // get the max liquidation amount (before liquidator reaches minLiquidatorRatio)
coverage_0x4962bbd4(0xbf3a16ac2eb03b00a37a7115b7d77aabc4c05f9bdb0e4a755a4e29e38056bb75); /* line */ 
                coverage_0x4962bbd4(0x03aae2bd3e4036ccec229b49e5b6e4860d1b2573bce4919db47655bb9daafb1e); /* statement */ 
calculateMaxLiquidationAmount(constants, cache);

                // if nothing to liquidate, do nothing
coverage_0x4962bbd4(0xbd99d12ec7e925ad3af83a3ec0880c138124b43c416751f36310f7f89afc9ffc); /* line */ 
                coverage_0x4962bbd4(0xb2394b40765c459774ca062901abe0d03f1e137b66cf99536e90a41b7acdeea5); /* statement */ 
if (cache.toLiquidate == 0) {coverage_0x4962bbd4(0x066d15fa3881fc0c5aa09269cb715cfd9fd95ba5e4b75119873b360543591bc5); /* branch */ 

coverage_0x4962bbd4(0x061edd51820e9cf71d308c61adfaac456e7073f1b9a01f389e8c53c674633777); /* line */ 
                    continue;
                }else { coverage_0x4962bbd4(0xa3b6fe960341655199ad591071d7434f4ec2e211452cc3fbbb93f4a1a937a4ca); /* branch */ 
}

                // execute the liquidations
coverage_0x4962bbd4(0x29c1456ff8dc3db54e970b293a6553f73b44015a7df76b1fc824b2184b73c840); /* line */ 
                coverage_0x4962bbd4(0xcd22fe983aa3e0d17a74d17138a4c5933dd8822c1147ce4070ec009fa79337c7); /* statement */ 
SOLO_MARGIN.operate(
                    constructAccountsArray(constants),
                    constructActionsArray(cache)
                );

                // increment the total value liquidated
coverage_0x4962bbd4(0x2d5b1bd10018726be57703d4e15b071baba3b6ba3c4bbbd5942379baed56633b); /* line */ 
                coverage_0x4962bbd4(0x6240ec5c407ebf1347dc606ec2f7b0f0d6ae43d9d866e41b62f18e29659719fd); /* statement */ 
totalValueLiquidated =
                    totalValueLiquidated.add(cache.toLiquidate.mul(cache.owedPrice));
            }
        }

        // revert if liquidator account does not have a lot of overhead to liquidate these pairs
coverage_0x4962bbd4(0xc8e16b4528d7b2b7405168468c7ee03497ce7916fda1073c4b47bedc8ca38e2f); /* line */ 
        coverage_0x4962bbd4(0x668104c4a79835b4f8bd3435cb930b91775092e13fb5ecaa1a68c36b34c2d63a); /* statement */ 
Require.that(
            totalValueLiquidated >= minValueLiquidated,
            FILE,
            "Not enough liquidatable value",
            totalValueLiquidated,
            minValueLiquidated
        );
    }

    // ============ Calculation Functions ============

    /**
     * Calculate the owedAmount that can be liquidated until the liquidator account will be left
     * with BOTH a non-negative balance of heldMarket AND a non-positive balance of owedMarket.
     * This is the amount that can be liquidated until the collateralization of the liquidator
     * account will begin to decrease.
     */
    function calculateSafeLiquidationAmount(
        Cache memory cache
    )
        private
        pure
    {coverage_0x4962bbd4(0x34599b343ee55b9e6373b485314b8fc9e1e2fd079ebc1faa7c974628d057c725); /* function */ 

coverage_0x4962bbd4(0xbcaf67c3967552652a5679d1baec8da76a0a1ac05d0dfd0ed03674e8e877d4e2); /* line */ 
        coverage_0x4962bbd4(0xc34fbbda5d3253bcfb403284f37940cf3947ae890ac1a73416b358c1fff16cf2); /* statement */ 
bool negOwed = !cache.owedWei.isPositive();
coverage_0x4962bbd4(0x406ab0b19116ab9ca7a3ca51b0f4b125c1a59cadee6cd26fd681e1d147cb9980); /* line */ 
        coverage_0x4962bbd4(0x138e1787f79c33cf1f6161c9de050be8f6b38a193642735774f76a1cdcbee67a); /* statement */ 
bool posHeld = !cache.heldWei.isNegative();

        // owedWei is already negative and heldWei is already positive
coverage_0x4962bbd4(0xd387cbf12e1ac2b856249076fac0521f159fead26a9c1c3f9b3cd4ec3210e0e7); /* line */ 
        coverage_0x4962bbd4(0x54086efc81089b8bb6a67c21ccb1e809b45aac62b164ac3d8b6e0cc56b513673); /* statement */ 
if (negOwed && posHeld) {coverage_0x4962bbd4(0xc7546ad5752663476e914bb8d23dd0f7ba831bc83975a2025a20f93b28ec91f7); /* branch */ 

coverage_0x4962bbd4(0xdff6c3b19097428da518ceb68e3683e9a2898c2e396e8763516e4e0e192ab2c5); /* line */ 
            coverage_0x4962bbd4(0x0a2fc3b441a9e8b2ae10e422e91506f5c78af6709c2e8d0eacf26f79a55d45c7); /* statement */ 
return;
        }else { coverage_0x4962bbd4(0x35e5aeac8107e70ceab1bc9fa1ae43588357319dad31d5c37ba51f5b2bdb19c2); /* branch */ 
}

        // true if it takes longer for the liquidator owed balance to become negative than it takes
        // the liquidator held balance to become positive.
coverage_0x4962bbd4(0xaf253f62f66f7726d8dc9aa0e2b0d4506a53130a86b35ec74f43a0933be0b74d); /* line */ 
        coverage_0x4962bbd4(0x689c3eeb4606de214dffa367b6b1b63df87449748c9071c57b53b70e9fb5b936); /* statement */ 
bool owedGoesToZeroLast;
coverage_0x4962bbd4(0xf49a423b9a6f24016b59017207e4fac5c92d89a4ec9e6fe5961bd044d0036079); /* line */ 
        coverage_0x4962bbd4(0x09355c37e52d1d312b16afd726fbc32279a239e6bb16eb7f28bc6148d084fd9b); /* statement */ 
if (negOwed) {coverage_0x4962bbd4(0xf55c15eebd379f0f5e5fb0c297f49bd0a39cdad2ac05ea05cde8c332a90d309a); /* branch */ 

coverage_0x4962bbd4(0xa54c68001a6f463864d92ccc72f61e7064ca0fdad9c1503b7d2dea2cca14c21f); /* line */ 
            coverage_0x4962bbd4(0xf1676414278692215c7f6b8aa0a32ca752b6709930923dda39beab3785fd974f); /* statement */ 
owedGoesToZeroLast = false;
        } else {coverage_0x4962bbd4(0xda5b7e8dd260c8e4ec9d9da40fb6a8342e753e43875bf4c410aacfcdcf5a2570); /* statement */ 
coverage_0x4962bbd4(0xea07c09d396ec5a1d83a5cffa321da28f742592bee8184814238f728644c57f1); /* branch */ 
if (posHeld) {coverage_0x4962bbd4(0x7857c0f84f647dda38e77dc4596577bcd17ca2774259bb0194066ad3928f433a); /* branch */ 

coverage_0x4962bbd4(0xffea2fad6685fe28e9d671635fc11fa17cb503a921a6e29c3db1663d59fb69a8); /* line */ 
            coverage_0x4962bbd4(0xf373b046fa2fda1c04de44d6c8ff1778102e6e1c2a1b03c1bbd5f421575a9e1b); /* statement */ 
owedGoesToZeroLast = true;
        } else {coverage_0x4962bbd4(0xd40884abc45449a3e6c865599d995527a781f7c6c539a7abf55247d68c6d03dc); /* branch */ 

            // owed is still positive and held is still negative
coverage_0x4962bbd4(0xc4074271fbf519810d6dfb553e331f5b578b4d4297c789c61f736b11298fba79); /* line */ 
            coverage_0x4962bbd4(0xe54139bfb3252bbf528aad1fe5f5d07ceeb4cc40f8761b2dd47aee0ae53010af); /* statement */ 
owedGoesToZeroLast =
                cache.owedWei.value.mul(cache.owedPriceAdj) >
                cache.heldWei.value.mul(cache.heldPrice);
        }}

coverage_0x4962bbd4(0x217998455936d51a909239398a440ed51b92f6dcd0b4aed03a4c25c8a640730a); /* line */ 
        coverage_0x4962bbd4(0x9c27b7642adc175701bdf36f9011def488480ed0e6d848648cdc862293d995af); /* statement */ 
if (owedGoesToZeroLast) {coverage_0x4962bbd4(0x8d9cd9abb08a019cc43496b2b9ae7b08c969466c6412558ced036b077b8296a5); /* branch */ 

            // calculate the change in heldWei to get owedWei to zero
coverage_0x4962bbd4(0xfbc6e2d1b36f74657da84a4ee696f5ee46070ff41eda7307c8d0ee176c3641a0); /* line */ 
            coverage_0x4962bbd4(0x69aa28f9274cb87a65848ef44e5c1d8527d1d1285e2f2c7c479ecb1fe7fe69a9); /* statement */ 
Types.Wei memory heldWeiDelta = Types.Wei({
                sign: cache.owedWei.sign,
                value: cache.owedWei.value.getPartial(cache.owedPriceAdj, cache.heldPrice)
            });
coverage_0x4962bbd4(0x939a5363177921116636f7d8bc1347afc9089cb2132c06a73c60115d4301bd3e); /* line */ 
            coverage_0x4962bbd4(0xe6e59789886de95598d483f2244720784d277df963e6e0539c8157d612e85488); /* statement */ 
setCacheWeiValues(
                cache,
                cache.heldWei.add(heldWeiDelta),
                Types.zeroWei()
            );
        } else {coverage_0x4962bbd4(0x1eb11270f041759803e45c60f70b9dc82a84c99d0782dda61dfb86cfd30288a0); /* branch */ 

            // calculate the change in owedWei to get heldWei to zero
coverage_0x4962bbd4(0x9c0a8b8f8a7121835fd1b9631586f14b10dadb5a9492c38a7aba31c74df69a2f); /* line */ 
            coverage_0x4962bbd4(0x9cfd99d6e40b10304e43c35f582d85fc80a2a253d40fe7a48d1c0d13ce6f0906); /* statement */ 
Types.Wei memory owedWeiDelta = Types.Wei({
                sign: cache.heldWei.sign,
                value: cache.heldWei.value.getPartial(cache.heldPrice, cache.owedPriceAdj)
            });
coverage_0x4962bbd4(0xece0cb5e81a6c4ef1cec4d17f13391324fa575b98bf5d6f6e3b57d5a4ab83156); /* line */ 
            coverage_0x4962bbd4(0xcd28501de407fe8eb43528f2bc829e09c755ecc1782f65112a8e1fbfdae7da42); /* statement */ 
setCacheWeiValues(
                cache,
                Types.zeroWei(),
                cache.owedWei.add(owedWeiDelta)
            );
        }
    }

    /**
     * Calculate the additional owedAmount that can be liquidated until the collateralization of the
     * liquidator account reaches the minLiquidatorRatio. By this point, the cache will be set such
     * that the amount of owedMarket is non-positive and the amount of heldMarket is non-negative.
     */
    function calculateMaxLiquidationAmount(
        Constants memory constants,
        Cache memory cache
    )
        private
        pure
    {coverage_0x4962bbd4(0x003d3dfe2d83fefd7a92f84099675f614632abf18f61a9353ae59cf1d34f55eb); /* function */ 

coverage_0x4962bbd4(0xf18acd415319310c214282cdc5d8f20eff0b768afaa7f5616cd21c1d89d9b210); /* line */ 
        coverage_0x4962bbd4(0x1bf788edacda0c58ee8c7e4dc67675d6cd3f10cecda7beaa00dfb923c1196ce5); /* assertPre */ 
coverage_0x4962bbd4(0x9b90301a3a797db1d26b1db05bfdedcf12afc034bf3c7c40238311ce55abb8de); /* statement */ 
assert(!cache.heldWei.isNegative());coverage_0x4962bbd4(0x8d5bd1ec55715f45062acbcef373b691cab6f6cff0cdfb5b992495a6d9e59f5d); /* assertPost */ 

coverage_0x4962bbd4(0x5c1333f211ddd5a16c4509d5838477e90efa61fbef7865a724902e0d02486db5); /* line */ 
        coverage_0x4962bbd4(0x076f581f2412370874b268a49be06541ed7a0a8a432d90743f0e2d0e67fa8742); /* assertPre */ 
coverage_0x4962bbd4(0x368294c8369d8c3b423ec12ecfcfa3dc2c8feb4f27eba4ed464184cdbaaf0a0f); /* statement */ 
assert(!cache.owedWei.isPositive());coverage_0x4962bbd4(0xead78d5bd0dfa3c024de895e186ac578a160573c9ebd03f4afb5f63a520f92a9); /* assertPost */ 


        // if the liquidator account is already not above the collateralization requirement, return
coverage_0x4962bbd4(0x1128a4126a051f838fe12938f505244dd2a38f9d6e7a06e89bc096aef7966b5e); /* line */ 
        coverage_0x4962bbd4(0x2a095c1cfae1484740145d0b4bfee9a313ae9bf319866bb8227dd15d8a39721e); /* statement */ 
bool liquidatorAboveCollateralization = isCollateralized(
            cache.supplyValue,
            cache.borrowValue,
            constants.minLiquidatorRatio
        );
coverage_0x4962bbd4(0xe476ba80c63d7a46e5a45269d687090316425babdc8424fc045a9e975de93b47); /* line */ 
        coverage_0x4962bbd4(0xef764820233b2e626ed6c5100641ed0a04162c11fe30788036685b567a9efb39); /* statement */ 
if (!liquidatorAboveCollateralization) {coverage_0x4962bbd4(0x99ac8e3facfe6818332c5d3553624dc8a48e3bf6c6727808ecc2bea7aa0d558b); /* branch */ 

coverage_0x4962bbd4(0xdd164c965e1642590f88e45e2c3284efb7c235312af9b7a864b6df2b2b7fd7a9); /* line */ 
            coverage_0x4962bbd4(0xcc9d2302d063cd6feba0cfd888cd5e4dc88c22b3106857697497e21f38c7cbf9); /* statement */ 
cache.toLiquidate = 0;
coverage_0x4962bbd4(0x0309ff963168985f783921a93084e858462766a5c5b2d22c6ba4220ef24655ae); /* line */ 
            coverage_0x4962bbd4(0xdcf2e92a3e7c006160ab3c25787983ca5acd48c587e36c991f37d8a42a9d260c); /* statement */ 
return;
        }else { coverage_0x4962bbd4(0xb6bcd1c9ed8ada09023298d79c5f95a2d02e8f6a848a020c8b2fe2673c0ec12b); /* branch */ 
}

        // find the value difference between the current margin and the margin at minLiquidatorRatio
coverage_0x4962bbd4(0x6a709963044e376c103668a6212bd0c2c9ee63eda488412b6102c20e7056d434); /* line */ 
        coverage_0x4962bbd4(0xec1e2dfa48542f1815bc02ac1436072141c54f2b5d1f98d2ca32882a2c6034b5); /* statement */ 
uint256 requiredOverhead = Decimal.mul(cache.borrowValue, constants.minLiquidatorRatio);
coverage_0x4962bbd4(0x65c01a0be7bbd8f76deb67eb6b1c51fa8ef602621aa1fe0ff425ef3808dd10d6); /* line */ 
        coverage_0x4962bbd4(0x11eb37d551ad177ced661ab6ffab498dcef7fc899168f0d1c5c4af8ad91fea5c); /* statement */ 
uint256 requiredSupplyValue = cache.borrowValue.add(requiredOverhead);
coverage_0x4962bbd4(0x727cb2d60ef595c5050516a8e0dbd93f7110b5c3e3961c6934165b9423ce9736); /* line */ 
        coverage_0x4962bbd4(0xc3facc1426d438f7e500917e3063a58ca0717ccc96cb120e8d9c63018168e3ed); /* statement */ 
uint256 remainingValueBuffer = cache.supplyValue.sub(requiredSupplyValue);

        // get the absolute difference between the minLiquidatorRatio and the liquidation spread
coverage_0x4962bbd4(0x62ea4325920e8deb4665a31a6ffcfa126d6c185b7b808a361b6c0cf9829bd0ef); /* line */ 
        coverage_0x4962bbd4(0x007945c58856ac0e8025f425585cc9c3f0810599308cab217828213f30f47897); /* statement */ 
Decimal.D256 memory spreadMarginDiff = Decimal.D256({
            value: constants.minLiquidatorRatio.value.sub(cache.spread.value)
        });

        // get the additional value of owedToken I can borrow to liquidate this position
coverage_0x4962bbd4(0x04459447d1c73c30c4a6c3680626b0695b5012f40f6ef7e3d9142a863f973cca); /* line */ 
        coverage_0x4962bbd4(0xf8d1582175d1817b34c9a0e6721bc3e970554d3a1dfe7532c3d17e74c9fa768b); /* statement */ 
uint256 owedValueToTakeOn = Decimal.div(remainingValueBuffer, spreadMarginDiff);

        // get the additional amount of owedWei to liquidate
coverage_0x4962bbd4(0xe61b4116ac56a4d89902887ff34a037cdef7980863e9e4893d9308e341c6977b); /* line */ 
        coverage_0x4962bbd4(0x8b9a0a0edcae767e2fc2debe7f2c1bec2479aad54892fcca4f00e02c50b402ed); /* statement */ 
uint256 owedWeiToLiquidate = owedValueToTakeOn.div(cache.owedPrice);

        // store the additional amount in the cache
coverage_0x4962bbd4(0xa505c1a0887c5227325d3780a53800877b74caa31a440d55fc2cf818018ed43b); /* line */ 
        coverage_0x4962bbd4(0x2ef38b7ac338880f12a390adce7fffd346c63dacbe538516f535773a23df5fb4); /* statement */ 
cache.toLiquidate = cache.toLiquidate.add(owedWeiToLiquidate);
    }

    // ============ Helper Functions ============

    /**
     * Make some basic checks before attempting to liquidate an account.
     *  - Require that the msg.sender is permissioned to use the liquidator account
     *  - Require that the liquid account is liquidatable
     */
    function checkRequirements(
        Constants memory constants
    )
        private
        view
    {coverage_0x4962bbd4(0x12e1e9cdfad8ce8b25b60a48c972290434b6aab8d268b14a40175a097a6a5cd2); /* function */ 

        // check credentials for msg.sender
coverage_0x4962bbd4(0xa9e0b6c31570f5df096c609618e555a55ae7f9aa0058a27a57c4596717e16460); /* line */ 
        coverage_0x4962bbd4(0x3d2c742c866d2e059293bf1a2ef522df5a83d03d5d505dae2b4397f3e0604b71); /* statement */ 
Require.that(
            constants.fromAccount.owner == msg.sender
            || SOLO_MARGIN.getIsLocalOperator(constants.fromAccount.owner, msg.sender),
            FILE,
            "Sender not operator",
            constants.fromAccount.owner
        );

        // require that the liquidAccount is liquidatable
coverage_0x4962bbd4(0x24512a94ebdcd2b09a1cbc8615ffd33ea347ca4609f582d3bbcc4b9a53414fbb); /* line */ 
        coverage_0x4962bbd4(0x9af7783238743d1bd12ee955be490d6d3bf5bdf17550d54fcb474bff3e00750a); /* statement */ 
(
            Monetary.Value memory liquidSupplyValue,
            Monetary.Value memory liquidBorrowValue
        ) = getCurrentAccountValues(constants, constants.liquidAccount);
coverage_0x4962bbd4(0xf4c996af3daa2da0416ff2c45ed64b5d7116e7d8c66ca1f39eafb7e170af2f33); /* line */ 
        coverage_0x4962bbd4(0x848abf3fab563ef48f4ed49bdbe0ca251717f0bd3f7e267a33554fffaf2355b1); /* statement */ 
Require.that(
            liquidSupplyValue.value != 0,
            FILE,
            "Liquid account no supply"
        );
coverage_0x4962bbd4(0x4c68e1509070f9b62c466a050298a19a019d87f2d1f559f8cda57758c6a867d6); /* line */ 
        coverage_0x4962bbd4(0xf88bc98228c7fb7e2dcab90fc48fc5cca2c77d6a01361e59417a174d826c0872); /* statement */ 
Require.that(
            SOLO_MARGIN.getAccountStatus(constants.liquidAccount) == Account.Status.Liquid
            || !isCollateralized(
                liquidSupplyValue.value,
                liquidBorrowValue.value,
                SOLO_MARGIN.getMarginRatio()
            ),
            FILE,
            "Liquid account not liquidatable",
            liquidSupplyValue.value,
            liquidBorrowValue.value
        );
    }

    /**
     * Changes the cache values to reflect changing the heldWei and owedWei of the liquidator
     * account. Changes toLiquidate, heldWei, owedWei, supplyValue, and borrowValue.
     */
    function setCacheWeiValues(
        Cache memory cache,
        Types.Wei memory newHeldWei,
        Types.Wei memory newOwedWei
    )
        private
        pure
    {coverage_0x4962bbd4(0x97efced9ff372e8df6b92faf1e226df17d933bb57846b36320500269ba8b8a01); /* function */ 

        // roll-back the old held value
coverage_0x4962bbd4(0xa7c2b3c7f9ba4a847ef380b7d4334a666e996d8f225007c507848f46aae3ddf4); /* line */ 
        coverage_0x4962bbd4(0x6a2fbc3913b44a78d80e58cf1f34a04bb900264026c20d3493f89c47b82ef9bd); /* statement */ 
uint256 oldHeldValue = cache.heldWei.value.mul(cache.heldPrice);
coverage_0x4962bbd4(0x4e5c0bad0f66b141d1ce862372a815dc321a8496f170989ba107de7f199c503d); /* line */ 
        coverage_0x4962bbd4(0xc940dda607b6801b37066601edcda4f6fd50531df005fca6237e828fcbe04c7e); /* statement */ 
if (cache.heldWei.sign) {coverage_0x4962bbd4(0x8e3a267284781c6e7787f6eb215c5e04dbeb2110d8fe8ccc9bf0c85f25e82638); /* branch */ 

coverage_0x4962bbd4(0xfcb14089c628ed6364a5dce93ab294c26f0b8cb696fbe89990c5c607606dac7d); /* line */ 
            coverage_0x4962bbd4(0x1f62c1befc45cf971199fcdff6035702804195987495ea773ce810d0397631e9); /* statement */ 
cache.supplyValue = cache.supplyValue.sub(oldHeldValue);
        } else {coverage_0x4962bbd4(0xee9bf1c01b9a8a4a0b2cc607797c1e61250208ee38361a0655704d3cd0409afc); /* branch */ 

coverage_0x4962bbd4(0xb4fc1c07dcf53717c9cec9245edec675e9bb88b40de920ad18684ffcd195713a); /* line */ 
            coverage_0x4962bbd4(0x6ed69cb846ba7a2b2db37f4e50b400457d1c91c7b2bb11e61274c813aea74790); /* statement */ 
cache.borrowValue = cache.borrowValue.sub(oldHeldValue);
        }

        // add the new held value
coverage_0x4962bbd4(0x23899788bdab2d898d935a49fe53a916bebc05461371c446df38c653ffbb11aa); /* line */ 
        coverage_0x4962bbd4(0xb7ef10e3cfdf9fb8003e41263b3e4ae16550f3dee376f47eb6bd286e11a248ff); /* statement */ 
uint256 newHeldValue = newHeldWei.value.mul(cache.heldPrice);
coverage_0x4962bbd4(0xc8eb363a2c865e3f8a8b586d949fd5f49429e8f0e6dd33219f1b6cf38855afdb); /* line */ 
        coverage_0x4962bbd4(0x2f3e0451a8ff93bd42da6feb818723d6a95b7d49c43737c20c8df5a7ed7fa095); /* statement */ 
if (newHeldWei.sign) {coverage_0x4962bbd4(0x3f8fa083bf581fab581ed86ffd7fc84e1b1f7a0e8164f7c3b3db2f0fa2f8c6c4); /* branch */ 

coverage_0x4962bbd4(0x6ab43b72d19e598998ae8cbaff7860b6103b7ab00256b362b1cd97f29705cceb); /* line */ 
            coverage_0x4962bbd4(0x91c6167f52dff395f6e3cc7d74868f253fbf8bb7c91822a5b394ccda856a44db); /* statement */ 
cache.supplyValue = cache.supplyValue.add(newHeldValue);
        } else {coverage_0x4962bbd4(0x9b0abc506a223af189a8e24d7c3a9eef48dd76883dd7413363e1a2f95ed89153); /* branch */ 

coverage_0x4962bbd4(0xdec0d7df829cefe71c93b4e5448836efb400a15c7119eaa6d1820944e3880278); /* line */ 
            coverage_0x4962bbd4(0x2d372b332f40f80fb6d8e1b344e81b1240a41c6c841d08489d381e07c2c3a29e); /* statement */ 
cache.borrowValue = cache.borrowValue.add(newHeldValue);
        }

        // roll-back the old owed value
coverage_0x4962bbd4(0x7687c702b0c11833a45ce4ece75023739ff23864022458a8a1e7cf13541947b8); /* line */ 
        coverage_0x4962bbd4(0x9368d62fb88a1830da3af3c7cc4135306da79ba3ca1e90df6e7127eb22d79d19); /* statement */ 
uint256 oldOwedValue = cache.owedWei.value.mul(cache.owedPrice);
coverage_0x4962bbd4(0x74e4bdf1987c01504df126d83d143175903aeacf8fc3b682fd4b426ae9ac4fa5); /* line */ 
        coverage_0x4962bbd4(0xf95d2ef949887dcf7e985df2868bcfab5b3a0aab728ec502f39a98aa0071a90b); /* statement */ 
if (cache.owedWei.sign) {coverage_0x4962bbd4(0x5843223792fc08a2c2632905bbdb32642daf8339a8dfcc39f38c33555afaff41); /* branch */ 

coverage_0x4962bbd4(0xf6cb249e00e281e6582c5d26905bf4c491f52f814831dfda42952364ff4dcf62); /* line */ 
            coverage_0x4962bbd4(0x16630887046542fc35288cf0212a6813f1d64c56c50aa52efa40c0a84b3f3025); /* statement */ 
cache.supplyValue = cache.supplyValue.sub(oldOwedValue);
        } else {coverage_0x4962bbd4(0xf9c4a8221728d640c597201ca6938df822cdbf38262da5fe456c7aa0c94adad4); /* branch */ 

coverage_0x4962bbd4(0x70d407a2af82b891a3e5af44a4bbe9bda80ef32d65c79433ecf9309ff29a0ba7); /* line */ 
            coverage_0x4962bbd4(0x6d26023f972a2aaef786b252e3d034f20f66798ef28a9456d44c49222ef15827); /* statement */ 
cache.borrowValue = cache.borrowValue.sub(oldOwedValue);
        }

        // add the new owed value
coverage_0x4962bbd4(0xf9df3e944f181060ea58ddbb5f4efff47b7000bece614513c3d2849e988ceafa); /* line */ 
        coverage_0x4962bbd4(0x5b33ea9232db1986aa2cc07ad80b64cd5853da7c9fc4995874221f5550e39c51); /* statement */ 
uint256 newOwedValue = newOwedWei.value.mul(cache.owedPrice);
coverage_0x4962bbd4(0xdd0358025a7687509b8b4e4c413e7e39df23b7d0090886e9ab2a60ee9ad633a3); /* line */ 
        coverage_0x4962bbd4(0x609c72a0efb5b180c73ee98eb4cb1f10442ca1c087798d623375d385103813eb); /* statement */ 
if (newOwedWei.sign) {coverage_0x4962bbd4(0x5293a81a31ffd7d6b8603757896c9a04f672a36e7c143c33719e1f933a927f1e); /* branch */ 

coverage_0x4962bbd4(0x353524d3d25bec74a904b4c7dff0a374bf935ab5b095e7deb2f7e4ca56726c27); /* line */ 
            coverage_0x4962bbd4(0x2b10b9596b7493269e679c5c9c0903d8b3a64cc5ecfbff6553ee55b3f868acbb); /* statement */ 
cache.supplyValue = cache.supplyValue.add(newOwedValue);
        } else {coverage_0x4962bbd4(0xf7bb89bf65729015d69fe9c0ef43b333b95071b44f3d195b71e9f7cac1ae21e7); /* branch */ 

coverage_0x4962bbd4(0x0b4c171b9129ab923cd0b78f06ff6f0bec856d8901babef9e8e2af5a43ff398c); /* line */ 
            coverage_0x4962bbd4(0xe58356847f4a54bb721ad3db6ee0fb5ac59536a6a9bab27c31054ad640cf8bf6); /* statement */ 
cache.borrowValue = cache.borrowValue.add(newOwedValue);
        }

        // update toLiquidate, heldWei, and owedWei
coverage_0x4962bbd4(0xb3dec994cfd9bce346750eec40d95bc52f3a199ce1a775d12d7e617e353b9bbf); /* line */ 
        coverage_0x4962bbd4(0x19cf9d3d8e62122ee722e63293eb95c412a20acad8f55e83ee7810cf5769ad55); /* statement */ 
Types.Wei memory delta = cache.owedWei.sub(newOwedWei);
coverage_0x4962bbd4(0xc14fbb0f46d3443095dd54e2faf3f5d35f1a9f3f05a813eba8f545f95b283378); /* line */ 
        coverage_0x4962bbd4(0xb6e791890d611358b0cebeb722ab91ed616e3e2313b445d61e2e14bacc48a9b1); /* assertPre */ 
coverage_0x4962bbd4(0xa2093fbbc0d23af82aa1285a161bfc8478ee43d7f1f70d5e1d2f33d6b2bc36a4); /* statement */ 
assert(!delta.isNegative());coverage_0x4962bbd4(0x23e40e9074f9f6178f70067694a3fccbd45ead32ab8b81bde29c6441e344043b); /* assertPost */ 

coverage_0x4962bbd4(0xc18c089c33d706acf81a2fd87f172769fb37d5c04862c72df0f7d4c2c73b8cfe); /* line */ 
        coverage_0x4962bbd4(0xedd64a70edc2ec6a869d91d0c8048c509b8be1b3fba7d91a4279ddc6df8fa3f9); /* statement */ 
cache.toLiquidate = cache.toLiquidate.add(delta.value);
coverage_0x4962bbd4(0x94eb8c4db8f1307728f6efc055693913b566e87586bb4e2affeed8c587efcd21); /* line */ 
        coverage_0x4962bbd4(0xffdd3e0d12075229b185e1047059af19d1b87ca3d2f9a026d68eeb4cee631fa0); /* statement */ 
cache.heldWei = newHeldWei;
coverage_0x4962bbd4(0x5863d969992244d9fcf5ab048db2101a3c5a087109812b65f2fb06716accb9ac); /* line */ 
        coverage_0x4962bbd4(0x0505cfed1b56d1eec3d3ace80b85590b66f9dc1b2f6d830fb78166db64c36a16); /* statement */ 
cache.owedWei = newOwedWei;
    }

    /**
     * Returns true if the supplyValue over-collateralizes the borrowValue by the ratio.
     */
    function isCollateralized(
        uint256 supplyValue,
        uint256 borrowValue,
        Decimal.D256 memory ratio
    )
        private
        pure
        returns(bool)
    {coverage_0x4962bbd4(0xcdeac38bc12a4a3b6fcedaefe9aed44be47a9a5bc10e0223ef52785d7073336b); /* function */ 

coverage_0x4962bbd4(0x015d0ddfc03f97714180e1637b79100fc27c847dce28931c0058584aa5fb042d); /* line */ 
        coverage_0x4962bbd4(0x5c425ffa30dcca67a14e6c7d2928093ecc2cdf901bc24c7d10678dea27eb6aef); /* statement */ 
uint256 requiredMargin = Decimal.mul(borrowValue, ratio);
coverage_0x4962bbd4(0x20d54c59c0c60e0f827cb8d77347341bd5aa963615fc26ec5a53313f8ac70dba); /* line */ 
        coverage_0x4962bbd4(0x2e6f222f01a52e8594d3b8fc0c8876ac5036632eca8b615aa0f447b88e139e9c); /* statement */ 
return supplyValue >= borrowValue.add(requiredMargin);
    }

    // ============ Getter Functions ============

    /**
     * Gets the current total supplyValue and borrowValue for some account. Takes into account what
     * the current index will be once updated.
     */
    function getCurrentAccountValues(
        Constants memory constants,
        Account.Info memory account
    )
        private
        view
        returns (
            Monetary.Value memory,
            Monetary.Value memory
        )
    {coverage_0x4962bbd4(0xeb7cc44349039fe2b640a70ecd2800026e5fd6f84a59dea3e2319a2fbd5923a0); /* function */ 

coverage_0x4962bbd4(0x3b7a12aa8c5f56272c502eca79500cf2daef952cd5147c4886e29c6deee13a25); /* line */ 
        coverage_0x4962bbd4(0xfa3995428578bfa7d9a0be5b92fd59ee24f47ef3aada5e40aa2d6cd750774acc); /* statement */ 
Monetary.Value memory supplyValue;
coverage_0x4962bbd4(0x2aa04b5e07ae74b1627d7c60b5e259053dcc9da89e724c5015760fb0ed4273aa); /* line */ 
        coverage_0x4962bbd4(0xcdd0d6de6664aa5bd617e8dc811f58649386b31d51376e9d8176b822f1879d49); /* statement */ 
Monetary.Value memory borrowValue;

coverage_0x4962bbd4(0x306743bc8042dd62b55b67e1778e606b7f2cf5f62647baebd3c636426354142a); /* line */ 
        coverage_0x4962bbd4(0x35fb5a813559329ec053f3d6d6b0e19984654edfe266ccca53c91d2853b1dce8); /* statement */ 
for (uint256 m = 0; m < constants.markets.length; m++) {
coverage_0x4962bbd4(0x60334a4101b4240acc15041b4fed40f7b8caf69c30c8cd84088c90416c2c7d96); /* line */ 
            coverage_0x4962bbd4(0x53f0436b3d96886dcba9a918629a8a617c34595340e4d3b4bdca0ce35cc6390c); /* statement */ 
Types.Par memory par = SOLO_MARGIN.getAccountPar(account, m);
coverage_0x4962bbd4(0x4d2976893d9e87055d99c78148eeba9fd9bf267d4db4c799bbacae24c62ccea7); /* line */ 
            coverage_0x4962bbd4(0x04ad5f560e178611333550564f7e7772bd8d4b83590fe8dae8f953e62cde020d); /* statement */ 
if (par.isZero()) {coverage_0x4962bbd4(0x3b1a933e8c6b75686d71d974541bfcf514eaeb603e6c2e874bc5c3f1c84f0263); /* branch */ 

coverage_0x4962bbd4(0xff1fca2c2dc529b5ec06fc99d8f517a3656c042ffd61157a7c9b9fa49d602564); /* line */ 
                continue;
            }else { coverage_0x4962bbd4(0x49a322e352691fc4a0f084b1c24007316fd87fd148497e9fef079179a9677bf8); /* branch */ 
}
coverage_0x4962bbd4(0x5bec2342056bd534bf7d87696ca80437f82e01f7398525baf9db4905bbb1f8ef); /* line */ 
            coverage_0x4962bbd4(0x3f5a6c515e016eb47a89813a496932679041d8bfacf7a47815a1a9fd05b80372); /* statement */ 
Types.Wei memory userWei = Interest.parToWei(par, constants.markets[m].index);
coverage_0x4962bbd4(0x647cf5a30e168a6b3cff4759c0890ad34a3c1d7747702e6675f85936fdfaefb0); /* line */ 
            coverage_0x4962bbd4(0xfcd0d83259f135fa90d9b73466d4ef34206293bd9fc6165f191b651d0482d32f); /* statement */ 
uint256 assetValue = userWei.value.mul(constants.markets[m].price.value);
coverage_0x4962bbd4(0xd90795f7bd7b30e644bce9c0b86721c5eee3a380a93bfe40fea90ce4b26feab3); /* line */ 
            coverage_0x4962bbd4(0xf75da055cd5ed4f304b557a7725d849fe849f2f02be8f6cedacaa923f469d107); /* statement */ 
if (userWei.sign) {coverage_0x4962bbd4(0xd28f86d548c464e12e8751ec01b1b14318a90ce7ad2005e53f89d54f3deb8e62); /* branch */ 

coverage_0x4962bbd4(0x348c22217e5898db188bc3954b4de4242ae1bc5514fea7a9beadf7174b34bf18); /* line */ 
                coverage_0x4962bbd4(0x4390665db8fe0b6088963fed0f3896c19aa9d6ef6bac07be765d6bb115c61bca); /* statement */ 
supplyValue.value = supplyValue.value.add(assetValue);
            } else {coverage_0x4962bbd4(0x18e1c4e6de9febb654adadf920cfe7e943015d8ae1e9416f797ee863664c4d03); /* branch */ 

coverage_0x4962bbd4(0x4c59bb565f38a82efc3109b7801799c9d0d66acfc1e1688599e3674082bd3bdf); /* line */ 
                coverage_0x4962bbd4(0x0851e72489f70daefbae20addad3e7303c70daf84c53d7c192972eeab474bcdc); /* statement */ 
borrowValue.value = borrowValue.value.add(assetValue);
            }
        }

coverage_0x4962bbd4(0xd574db0888283d348549ecdff7f974d112ead571c52d03710b26642febc0e1f7); /* line */ 
        coverage_0x4962bbd4(0xe4e58568d7b5cff24a929814486e7e9d11e3db120b12cfcdd3b226aa62b5e6d9); /* statement */ 
return (supplyValue, borrowValue);
    }

    /**
     * Get the updated index and price for every market.
     */
    function getMarketsInfo()
        private
        view
        returns (MarketInfo[] memory)
    {coverage_0x4962bbd4(0xa790783cbd60d09a7dfa030a14a7f6d818b031e69f4fdef468e29e9b3cc4e5c9); /* function */ 

coverage_0x4962bbd4(0xb2f9309ffb1fce2812cf552789bb7c9cb2c744c81308a16bfa2cb523e5f857da); /* line */ 
        coverage_0x4962bbd4(0x47b51d4a6da70dbcf9590b66a250bb37bc3ed5aee3783bf5968ef3fb6a72aa4f); /* statement */ 
uint256 numMarkets = SOLO_MARGIN.getNumMarkets();
coverage_0x4962bbd4(0x21370da87eaece2a2db76d055123f878bdd0f9a7b755ae34925669e0c2996b7d); /* line */ 
        coverage_0x4962bbd4(0x5c3311b37c2ac45a78db67650313598e68ac2846f1afcd7a7c60e6aed51bc360); /* statement */ 
MarketInfo[] memory markets = new MarketInfo[](numMarkets);
coverage_0x4962bbd4(0x14410a84dc4f1e3acd02a9bd6c75a163b6e859ab83fcac205ccf406452ddc7d8); /* line */ 
        coverage_0x4962bbd4(0xefd82539905926ed08403a70b345b396c59a5aa68edc6a388f3ca527da20ab25); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0x4962bbd4(0xfdd939f77b8d2d59ac777bcc8e3c9a68c61f02f40159f2c09c8b348b8ce03a7b); /* line */ 
            coverage_0x4962bbd4(0xed22071e9fe3ff3871e3be1bd5cf523de33ae2cdace02a4b67179f6d3cc4e47f); /* statement */ 
markets[m] = MarketInfo({
                price: SOLO_MARGIN.getMarketPrice(m),
                index: SOLO_MARGIN.getMarketCurrentIndex(m)
            });
        }
coverage_0x4962bbd4(0xd61026dc62bf1d19219ff0158150d3e421d1a98f4d677323820044268807a896); /* line */ 
        coverage_0x4962bbd4(0x905f9b674dbb205765279659de84745a5f639e8854ba252eecdceb8ee3bfd5af); /* statement */ 
return markets;
    }

    /**
     * Pre-populates cache values for some pair of markets.
     */
    function initializeCache(
        Constants memory constants,
        uint256 heldMarket,
        uint256 owedMarket
    )
        private
        view
        returns (Cache memory)
    {coverage_0x4962bbd4(0xee71c58fa83a293ba1d8ee3cf5068d5e3f9865a669a53866b1699a9aa36e6972); /* function */ 

coverage_0x4962bbd4(0x65ff5e76113c621880fcc4885502ecf1e20bb5b02ed0fd00ba9964cc57b18f7f); /* line */ 
        coverage_0x4962bbd4(0xd0c7cf8030c8e8b4c23d2318185c27525659d33904d97d0f5ff7fc490fa34250); /* statement */ 
(
            Monetary.Value memory supplyValue,
            Monetary.Value memory borrowValue
        ) = getCurrentAccountValues(constants, constants.fromAccount);

coverage_0x4962bbd4(0x23d6175389126a5b05038c46bd3f29b50d2e0ae92315f579d8515fddb29fc06a); /* line */ 
        coverage_0x4962bbd4(0xf57df01704ef45f5a015dcc261174a5cc7675c6ceb8e5c5385d01e40e33ca7e5); /* statement */ 
uint256 heldPrice = constants.markets[heldMarket].price.value;
coverage_0x4962bbd4(0x20f1fcea52838cc1caa03d6e8acabf75a274176b5c7ce262a1b533b3151312f1); /* line */ 
        coverage_0x4962bbd4(0xd6f7af3096d49e4de0453dac47c808650419022766e547a09565610aac39f442); /* statement */ 
uint256 owedPrice = constants.markets[owedMarket].price.value;
coverage_0x4962bbd4(0x08ddc2437d32e06df8a2ba7792e6c07b0c6401424d2102c9ee96fc59ab18384b); /* line */ 
        coverage_0x4962bbd4(0xbf6757b5ffc290ff1fff31f19db936bdebebb8e55848af18784599510cbb1875); /* statement */ 
Decimal.D256 memory spread =
            SOLO_MARGIN.getLiquidationSpreadForPair(heldMarket, owedMarket);

coverage_0x4962bbd4(0xb9a7a4dba54fc81c44c98eb34816c59694b135dd60324695b5b5ddcc98d03ee7); /* line */ 
        coverage_0x4962bbd4(0x0a68a8f5bedb8735eedeb3512757564753dac0b7a42044a7e1a7e2c259f4b0a0); /* statement */ 
return Cache({
            heldWei: Interest.parToWei(
                SOLO_MARGIN.getAccountPar(constants.fromAccount, heldMarket),
                constants.markets[heldMarket].index
            ),
            owedWei: Interest.parToWei(
                SOLO_MARGIN.getAccountPar(constants.fromAccount, owedMarket),
                constants.markets[owedMarket].index
            ),
            toLiquidate: 0,
            supplyValue: supplyValue.value,
            borrowValue: borrowValue.value,
            heldMarket: heldMarket,
            owedMarket: owedMarket,
            spread: spread,
            heldPrice: heldPrice,
            owedPrice: owedPrice,
            owedPriceAdj: Decimal.mul(owedPrice, Decimal.onePlus(spread))
        });
    }

    // ============ Operation-Construction Functions ============

    function constructAccountsArray(
        Constants memory constants
    )
        private
        pure
        returns (Account.Info[] memory)
    {coverage_0x4962bbd4(0xca5d9829e20e69c6189476b9db6060c1138a97a7e80c727897c09aceeee5c5cd); /* function */ 

coverage_0x4962bbd4(0xd49b99aa733de237a93fc9998c9a782e6008625caa00bfe04404efa3571719cf); /* line */ 
        coverage_0x4962bbd4(0x58fd6fcc9d3809f78b237f4c0b9a287a6d384abcd66fd01aa708211b6bf3162e); /* statement */ 
Account.Info[] memory accounts = new Account.Info[](2);
coverage_0x4962bbd4(0xadc1ad1bd0f58255acb4f179df1f0579b1762a7fe8fa51ae3e2e94d101df1ac8); /* line */ 
        coverage_0x4962bbd4(0x164c8e397a25a26ae4cd607fca8c2fda42040c102ef006525e0b10fd987f6471); /* statement */ 
accounts[0] = constants.fromAccount;
coverage_0x4962bbd4(0xf9f681d60b8cbc396d9f19e9a48622b9026d7effafa7acb96e707d3b7d405ec7); /* line */ 
        coverage_0x4962bbd4(0x395ff059d9346e40099b2d428a3d0b5d9ff1bc0f83e8b12dfe59352cb43dc951); /* statement */ 
accounts[1] = constants.liquidAccount;
coverage_0x4962bbd4(0x13cd23d45a58ac1cff05ebf83f1d10e4f36a9e30a21f5511fe1a37f055aba556); /* line */ 
        coverage_0x4962bbd4(0xe4d19d4ed82a5fc656c46033a3bad60fe1a7482914e5dac70896dc0564ee55ab); /* statement */ 
return accounts;
    }

    function constructActionsArray(
        Cache memory cache
    )
        private
        pure
        returns (Actions.ActionArgs[] memory)
    {coverage_0x4962bbd4(0x41f9ffffc8cdb0aaab451e90bc14fb80d640afd4020aa00e1a457ab7eea41e9d); /* function */ 

coverage_0x4962bbd4(0x224717741fbeaaa24ea4a9cdb29c90674db9cc09bd46f91d7234af45103d29f7); /* line */ 
        coverage_0x4962bbd4(0x70767745f069742d08aa2f521d6baff04ccc8c7de64c939b69a2a49c6931d623); /* statement */ 
Actions.ActionArgs[] memory actions = new Actions.ActionArgs[](1);
coverage_0x4962bbd4(0x061a5bcb472cabc44196eddb21140f64c3d99bba1708233a668b742c8e36837c); /* line */ 
        coverage_0x4962bbd4(0x7c3319ff164981a48adb28ea1a4046b2a523c680e6ee8db85352ba206c412703); /* statement */ 
actions[0] = Actions.ActionArgs({
            actionType: Actions.ActionType.Liquidate,
            accountId: 0,
            amount: Types.AssetAmount({
                sign: true,
                denomination: Types.AssetDenomination.Wei,
                ref: Types.AssetReference.Delta,
                value: cache.toLiquidate
            }),
            primaryMarketId: cache.owedMarket,
            secondaryMarketId: cache.heldMarket,
            otherAddress: address(0),
            otherAccountId: 1,
            data: new bytes(0)
        });
coverage_0x4962bbd4(0xb752aaf769a9a77afb107dade8183471fbc5e82ee213a52f6f2d9f06d4e762e4); /* line */ 
        coverage_0x4962bbd4(0xa779991e88d5fdd60b3b13f2722f27a82ee5d7d986d98abf4ca87cbdfa0fec3f); /* statement */ 
return actions;
    }
}
