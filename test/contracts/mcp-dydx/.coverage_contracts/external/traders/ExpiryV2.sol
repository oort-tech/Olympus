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
import { Ownable } from "openzeppelin-solidity/contracts/ownership/Ownable.sol";
import { IAutoTrader } from "../../protocol/interfaces/IAutoTrader.sol";
import { ICallee } from "../../protocol/interfaces/ICallee.sol";
import { Account } from "../../protocol/lib/Account.sol";
import { Decimal } from "../../protocol/lib/Decimal.sol";
import { Math } from "../../protocol/lib/Math.sol";
import { Monetary } from "../../protocol/lib/Monetary.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Time } from "../../protocol/lib/Time.sol";
import { Types } from "../../protocol/lib/Types.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";


/**
 * @title ExpiryV2
 * @author dYdX
 *
 * Expiry contract that also allows approved senders to set expiry to be 28 days in the future.
 */
contract ExpiryV2 is
    Ownable,
    OnlySolo,
    ICallee,
    IAutoTrader
{
function coverage_0x3c7ab02b(bytes32 c__0x3c7ab02b) public pure {}

    using Math for uint256;
    using SafeMath for uint32;
    using SafeMath for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "ExpiryV2";

    // ============ Enums ============

    enum CallFunctionType {
        SetExpiry,
        SetApproval
    }

    // ============ Structs ============

    struct SetExpiryArg {
        Account.Info account;
        uint256 marketId;
        uint32 timeDelta;
        bool forceUpdate;
    }

    struct SetApprovalArg {
        address sender;
        uint32 minTimeDelta;
    }

    // ============ Events ============

    event ExpirySet(
        address owner,
        uint256 number,
        uint256 marketId,
        uint32 time
    );

    event LogExpiryRampTimeSet(
        uint256 expiryRampTime
    );

    event LogSenderApproved(
        address approver,
        address sender,
        uint32 minTimeDelta
    );

    // ============ Storage ============

    // owner => number => market => time
    mapping (address => mapping (uint256 => mapping (uint256 => uint32))) g_expiries;

    // owner => sender => minimum time delta
    mapping (address => mapping (address => uint32)) public g_approvedSender;

    // time over which the liquidation ratio goes from zero to maximum
    uint256 public g_expiryRampTime;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        uint256 expiryRampTime
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x3c7ab02b(0xa1a9a1ec822e66dfac09a83469f567ba1dfce60a325ed97653df781d3d294aed); /* function */ 

coverage_0x3c7ab02b(0x699b500d4a663603f7e2a68d8884644da6251ad5ee94af7f01c4ae9468a58324); /* line */ 
        coverage_0x3c7ab02b(0xaf23524adc0e09300d85d31aeefc8355cbeea0b32a445996b8bb43fa419882d8); /* statement */ 
g_expiryRampTime = expiryRampTime;
    }

    // ============ Admin Functions ============

    function ownerSetExpiryRampTime(
        uint256 newExpiryRampTime
    )
        external
        onlyOwner
    {coverage_0x3c7ab02b(0x36863521d5d9a45748ce6e2337553c2b9e9ce2e64f226268199d10b258d039a0); /* function */ 

coverage_0x3c7ab02b(0xe9bda645d1b47c4aa503b24ad9d7e6c37d65ca93bd7cebd1f66ef1d6010f837d); /* line */ 
        coverage_0x3c7ab02b(0x65d1423d246e6aca0d795e3ebcdb717329c85cac980ea7d685d7defcfa3487fd); /* statement */ 
emit LogExpiryRampTimeSet(newExpiryRampTime);
coverage_0x3c7ab02b(0x733f256a451469ec4833dc72d2dd0d70cda53e1294600fa7a8805a7c12ddc920); /* line */ 
        coverage_0x3c7ab02b(0x1fd119337fb2ab35190bb63e0d58ef8816e404f1a91b56480414be3ac6cfdf4c); /* statement */ 
g_expiryRampTime = newExpiryRampTime;
    }

    // ============ Approval Functions ============

    function approveSender(
        address sender,
        uint32 minTimeDelta
    )
        external
    {coverage_0x3c7ab02b(0x2b4c9428d067c48009c27e41bc2ced5e3a7e7991a1039110ab67a575e2a518b9); /* function */ 

coverage_0x3c7ab02b(0xffaac46238747f7bdeb92245ddecdeabc038dd507543ceac3f17762b1a3a07e0); /* line */ 
        coverage_0x3c7ab02b(0xb3bcb77ec9e2f4020049ced3924b2a430f631fd3131d36974940adbf11d08bd8); /* statement */ 
setApproval(msg.sender, sender, minTimeDelta);
    }

    // ============ Only-Solo Functions ============

    function callFunction(
        address /* sender */,
        Account.Info memory account,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
    {coverage_0x3c7ab02b(0x4da4b3c8764a335f3c37a45bc6908fe30c61ceb51eb809b875cbce94d8107aab); /* function */ 

coverage_0x3c7ab02b(0xac879b80d5833f074f352ee9eae0b9f151229440a0f8b9f74b98fb77c47b66e2); /* line */ 
        coverage_0x3c7ab02b(0x537f7930924726e112654ff503fffaf18c117512916918a76511c1f10d508c91); /* statement */ 
CallFunctionType callType = abi.decode(data, (CallFunctionType));
coverage_0x3c7ab02b(0x0311a30543aadc17a29e5e563e059e944792cc596740be22e0ac9aeb2893c0d2); /* line */ 
        coverage_0x3c7ab02b(0xb962c7092556a8f9cb15b650fde581e0dd2765bd60db172021a0589dc72d7e98); /* statement */ 
if (callType == CallFunctionType.SetExpiry) {coverage_0x3c7ab02b(0xf0d33bcba57f167aec6fef2ac544c22c4fdf3d024d454ffb95b20a40672b38ca); /* branch */ 

coverage_0x3c7ab02b(0x1ac714dddf1717dc18ec61b6bdfd6633c2cfb6788afeafef53cfde85e7316eef); /* line */ 
            coverage_0x3c7ab02b(0x3bafc5c1277d0ecce5394a5664662bca122ce89e5c32b24d431a981186e270fd); /* statement */ 
callFunctionSetExpiry(account.owner, data);
        } else {coverage_0x3c7ab02b(0xd96cea6f8c1cd908bb00feb22caea56b4f93ed2bfe979d31990eeb23fe613127); /* branch */ 

coverage_0x3c7ab02b(0x5ba28f98b70f3a16c248195c7703a14ecda5b0b83e18d968b71b7581d3d97b46); /* line */ 
            coverage_0x3c7ab02b(0x5cb5d1bafc367c459c20dfbc559cbcd401bf27b909eff5e2e703ba13853cf0b7); /* statement */ 
callFunctionSetApproval(account.owner, data);
        }
    }

    function getTradeCost(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory makerAccount,
        Account.Info memory /* takerAccount */,
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.Wei memory inputWei,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
        returns (Types.AssetAmount memory)
    {coverage_0x3c7ab02b(0x11d3895bb489bba72386a39c75531ef84de87e7d5c208b5e575e6d4e7d647020); /* function */ 

        // return zero if input amount is zero
coverage_0x3c7ab02b(0xa58d82ae68091776533355ea3945e7c9764932ab05f74b159f0c077ef205a7c4); /* line */ 
        coverage_0x3c7ab02b(0x71127cf3c5280d0454b019de0ac6ab96ed8dec28ff6e1d9199b7ebc0087a1e28); /* statement */ 
if (inputWei.isZero()) {coverage_0x3c7ab02b(0x38ee1dcfd09e355ae5ce7a76f23013f0941bb980c8c145aff2a354e1856c61fb); /* branch */ 

coverage_0x3c7ab02b(0xc3f4e9ec3c2342d4476f67b3ee2e670e2a80c95c6291b3b96d08522d125d91d2); /* line */ 
            coverage_0x3c7ab02b(0x3a823a5ee6d8c427abdc3b258b534c3e74f63549e43ef0bc9e4a0165176d09ab); /* statement */ 
return Types.AssetAmount({
                sign: true,
                denomination: Types.AssetDenomination.Par,
                ref: Types.AssetReference.Delta,
                value: 0
            });
        }else { coverage_0x3c7ab02b(0x94e863d4b50fcb07edd5cbaeb6f01edd7fceb562fe76d4cd2ce9cb29d7b36967); /* branch */ 
}

coverage_0x3c7ab02b(0xad34f05a3f968176e44520bcf1d34387cec80b43e020b914bec40690be7a9367); /* line */ 
        coverage_0x3c7ab02b(0x1c3cac6f38bad891fda3ec290813243fea584462c1e0ddd927d2d79e2b52f9e5); /* statement */ 
(uint256 owedMarketId, uint32 maxExpiry) = abi.decode(data, (uint256, uint32));

coverage_0x3c7ab02b(0x9807647da70dc246d28d8541c8e5d118a13998f6f1731091b4ab6ef8080cdf9b); /* line */ 
        coverage_0x3c7ab02b(0xbc97b9d4ba4d8fbff75717c06f5b450ba7a45acb3337ddc366638ff3cd27494c); /* statement */ 
uint32 expiry = getExpiry(makerAccount, owedMarketId);

        // validate expiry
coverage_0x3c7ab02b(0xde428d01aa7c43f574df2cff8a69389b8070de05a3b59f535192c9cd5e683aba); /* line */ 
        coverage_0x3c7ab02b(0x41f32cb5ab5cf24c88ea828609d57e771162c25dea070f8fd51708023dd5dfba); /* statement */ 
Require.that(
            expiry != 0,
            FILE,
            "Expiry not set",
            makerAccount.owner,
            makerAccount.number,
            owedMarketId
        );
coverage_0x3c7ab02b(0x32a73e9f29e14636c9ce30b5ebd567cdf268e447077537f0c239828b745ecfb4); /* line */ 
        coverage_0x3c7ab02b(0x41750d4b739921e9c5bdae1914fbcf0cdd2384f3381824ed44cdc05a0580f690); /* statement */ 
Require.that(
            expiry <= Time.currentTime(),
            FILE,
            "Borrow not yet expired",
            expiry
        );
coverage_0x3c7ab02b(0x42cdb975fff0ffec58fa187a975386d654659406665995ea5b3660f1e990dbd1); /* line */ 
        coverage_0x3c7ab02b(0x7154179815ed8d5d4b7030d4c8e5dc09e768187b4cb18648ee5c3c9d9bd91d1b); /* statement */ 
Require.that(
            expiry <= maxExpiry,
            FILE,
            "Expiry past maxExpiry",
            expiry
        );

coverage_0x3c7ab02b(0xdb3da77818c7ab3946a724d02ca44e141edcadc73364489a02a667f6be95a5a7); /* line */ 
        coverage_0x3c7ab02b(0xa782080f1e4190c1cc362295313fbac90eeb59de32d1722a1311ea415aae445f); /* statement */ 
return getTradeCostInternal(
            inputMarketId,
            outputMarketId,
            makerAccount,
            oldInputPar,
            newInputPar,
            inputWei,
            owedMarketId,
            expiry
        );
    }

    // ============ Getters ============

    function getExpiry(
        Account.Info memory account,
        uint256 marketId
    )
        public
        view
        returns (uint32)
    {coverage_0x3c7ab02b(0x3f12df30a10c11cc1316dc871b02972e8d741e200d1f5bd10d7c8653adfa576f); /* function */ 

coverage_0x3c7ab02b(0xc9de3bd22a3830094a72999bc3205fa03c46a6e09c96c10eeaf14c282dbfdab9); /* line */ 
        coverage_0x3c7ab02b(0x90e2a7dde341f2ee5b57bead2ed5fe9b2582fa9979da6579ab42efd769fdc4f1); /* statement */ 
return g_expiries[account.owner][account.number][marketId];
    }

    function getSpreadAdjustedPrices(
        uint256 heldMarketId,
        uint256 owedMarketId,
        uint32 expiry
    )
        public
        view
        returns (
            Monetary.Price memory,
            Monetary.Price memory
        )
    {coverage_0x3c7ab02b(0x6e52b4c5e5ad9cdb4e87f15f9bdf9f2eeaad131c4caa6966e29f1733fb2aee5f); /* function */ 

coverage_0x3c7ab02b(0x28b551f0d235f2a3cbfc63f4b7d6ad0ce66f0a9f984d0f0ca375a7399edfb7a2); /* line */ 
        coverage_0x3c7ab02b(0x0387cb1d472179414792e3e6bd31bab93fbc30276bdd9317395fb2c40c6c2c68); /* statement */ 
Decimal.D256 memory spread = SOLO_MARGIN.getLiquidationSpreadForPair(
            heldMarketId,
            owedMarketId
        );

coverage_0x3c7ab02b(0xe7ea86dea5596d476dbc0d449ffaca18d50752a4fd3bf9886037bfbac06e7037); /* line */ 
        coverage_0x3c7ab02b(0xd970b7667ecd04b966c7326df2dfc027890966af0094ba57259461e86fe1bd27); /* statement */ 
uint256 expiryAge = Time.currentTime().sub(expiry);

coverage_0x3c7ab02b(0xaa47c57f8c5120a3c59b30d8a6fb066667f0f7689bff217499a8a5343e445dc5); /* line */ 
        coverage_0x3c7ab02b(0xdad0f42e8d4506c8ac5985acf998b8bc2daf9848786cc78f68e38d9ba3e47d4f); /* statement */ 
if (expiryAge < g_expiryRampTime) {coverage_0x3c7ab02b(0x3d58f6dd8ad2aaef336bfc3d4b7fff6635154dca9f27c1cf59b939bf154e0127); /* branch */ 

coverage_0x3c7ab02b(0xbad38fa2c039f9d36f5194a2a0e451a5528d82bf3fccce12030f514f46e04ebd); /* line */ 
            coverage_0x3c7ab02b(0x718ddb3114ec8db60bb2dd539a4e9c52ce29ce8d84397a6b23fa2fdbf3723e49); /* statement */ 
spread.value = Math.getPartial(spread.value, expiryAge, g_expiryRampTime);
        }else { coverage_0x3c7ab02b(0x2d3dae25eb6cf3f7124817bce1be3866c9e046b87d120ae1afc8552b3cf29b92); /* branch */ 
}

coverage_0x3c7ab02b(0xa4105c03103ba1821e9255707a6ee3adef66fbee55b6ab8e6b88b0cf4e68909e); /* line */ 
        coverage_0x3c7ab02b(0xe8b046886781225755a9589034ae852915859c8c35ed21fe6f0f7c6db8552785); /* statement */ 
Monetary.Price memory heldPrice = SOLO_MARGIN.getMarketPrice(heldMarketId);
coverage_0x3c7ab02b(0xcb0cd38c35fabd24880cd26faf4f8e4df5cfeee733f0e3827847677d2d5d7f29); /* line */ 
        coverage_0x3c7ab02b(0x1a5414b4081a14965e04940524836f235df858b177c4593ad6db6df33ee831d3); /* statement */ 
Monetary.Price memory owedPrice = SOLO_MARGIN.getMarketPrice(owedMarketId);
coverage_0x3c7ab02b(0x51c4836297de252ed85904e5b31c2e4b3b415c846fa0f8db2d17624b806f5199); /* line */ 
        coverage_0x3c7ab02b(0xeb24e2e649a6104cc523243df54618671330d192548a4acfbd214729581dcf43); /* statement */ 
owedPrice.value = owedPrice.value.add(Decimal.mul(owedPrice.value, spread));

coverage_0x3c7ab02b(0xfdeb19e7375742b373aa5e57bc98ef730b9a7efe80fe7bc380e01a37ae1da28f); /* line */ 
        coverage_0x3c7ab02b(0x17f0a56ad51f2e7ccfb5f3325057c69111dfb6959da87592719be2af694c48a2); /* statement */ 
return (heldPrice, owedPrice);
    }

    // ============ Private Functions ============

    function callFunctionSetExpiry(
        address sender,
        bytes memory data
    )
        private
    {coverage_0x3c7ab02b(0x80208f8a9c2335c07b01dc867ceb59d71f3661f163e8e7d92b3f0523c854447c); /* function */ 

coverage_0x3c7ab02b(0x086d589063dc261ca6a6f056ee7534338c2a615a529f91c8a5d080f99f50841d); /* line */ 
        coverage_0x3c7ab02b(0xab021e3fbc09a8ac606b22aa5e10c5bbc4b9211a107ba704467b286b244aeb4e); /* statement */ 
(
            CallFunctionType callType,
            SetExpiryArg[] memory expiries
        ) = abi.decode(data, (CallFunctionType, SetExpiryArg[]));

coverage_0x3c7ab02b(0x6b6a1714641554df26100c6970f4c36c0ef041193995b627b26b2593a7bd94a3); /* line */ 
        coverage_0x3c7ab02b(0xffd868167e86b0c7bfebb69b562c8fb06f17077d00786aeae2ec50a2b25761aa); /* assertPre */ 
coverage_0x3c7ab02b(0x79be96d09509f8c66c70b3668334761fc7c4808b10b7d7aabc5126e23a9e4891); /* statement */ 
assert(callType == CallFunctionType.SetExpiry);coverage_0x3c7ab02b(0xd332e3ac5d3d8674cf02be6276dabc508ef5c76d3f90f386dd5e2e628f64c157); /* assertPost */ 


coverage_0x3c7ab02b(0x69081b23ca9fad18505c09c3618d70600ee402a80fe426bdf1eedac90ff22d3c); /* line */ 
        coverage_0x3c7ab02b(0x5bcb82f0db13370a0ce2a10db04edc3d86a3ab22b3eabd3d56ab5412cbf884f7); /* statement */ 
for (uint256 i = 0; i < expiries.length; i++) {
coverage_0x3c7ab02b(0x69ff40a309c828d56d1e9d0502f7a21c352aebc852d94ed66a594307af13db82); /* line */ 
            coverage_0x3c7ab02b(0x05ee44cc50112e0cba1087ad7f80ecdcab44adf018dd22381c92a35170216589); /* statement */ 
SetExpiryArg memory exp = expiries[i];
coverage_0x3c7ab02b(0xd73231871bc0c174a29cdbd00e57c81056478df8f7d6132446cd841725427fc5); /* line */ 
            coverage_0x3c7ab02b(0x524063dd4f63a26abf66693358f86886453dadda710d19f0d030404da3e67ac9); /* statement */ 
if (exp.account.owner != sender) {coverage_0x3c7ab02b(0x07a9ad0ea39d1f1245c688babed45547ffcc50c5d84f06174fc9a0c8d8e9ee6e); /* branch */ 

                // don't do anything if sender is not approved for this action
coverage_0x3c7ab02b(0x3486e9dd1cd3d61b167d2d5621760528b2d80ab1f6bd229b92a08d816fb23c35); /* line */ 
                coverage_0x3c7ab02b(0xd21399b3b2119df7047d2c9aa9f064741f144337c1e25e6f71349b00f26c9af1); /* statement */ 
uint32 minApprovedTimeDelta = g_approvedSender[exp.account.owner][sender];
coverage_0x3c7ab02b(0x184b86e5d292f956023b0000fb9c9ea249dad6b5d14155399360b11f90dc67e8); /* line */ 
                coverage_0x3c7ab02b(0xb2b0b0ebef0cb62584d6b477eab125f5b7c0743c208beabe99fcbf0e749810b4); /* statement */ 
if (minApprovedTimeDelta == 0 || exp.timeDelta < minApprovedTimeDelta) {coverage_0x3c7ab02b(0x3bf33fd860467b9acf240a1b7ec508e8a8f50c6d7351a48147b3e0bc82807d8f); /* branch */ 

coverage_0x3c7ab02b(0xd47296b448797af65ee8293c9d0063e0310b1cb3932facec7fba9a4e015835c2); /* line */ 
                    continue;
                }else { coverage_0x3c7ab02b(0xa9e7c18031b772b162a9780ba6a034ee7b3e239f185475a4d50c0bd827c0902a); /* branch */ 
}
            }else { coverage_0x3c7ab02b(0x7084a0aa3c70393cc4d5f41fb9b582b680b600b7602a39aa9f53200eb1580d08); /* branch */ 
}

            // if timeDelta is zero, interpret it as unset expiry
coverage_0x3c7ab02b(0xc519c28ee29e95961b8051ee7f246c68d6fbfc33e38d44ff9d54ab90310b5603); /* line */ 
            coverage_0x3c7ab02b(0x3baef824d5b203122f48b303f62c1b679e75c4ed38f23b3aa163fab8ebe9d79d); /* statement */ 
if (
                exp.timeDelta != 0 &&
                SOLO_MARGIN.getAccountPar(exp.account, exp.marketId).isNegative()
            ) {coverage_0x3c7ab02b(0x676b8a5b0a5bf6b81ce209087436903a9ccc4b75835b9406454a76c8b59436cb); /* branch */ 

                // only change non-zero values if forceUpdate is true
coverage_0x3c7ab02b(0x9db16acb7208d9e6e6747d18306d20ca3721afd88ba03f18d84f8337c34385d3); /* line */ 
                coverage_0x3c7ab02b(0x0a51cbc3a6719d8c7d7fdd5a1c76d9f2e8a3872f6642475a8d5c47118182c342); /* statement */ 
if (exp.forceUpdate || getExpiry(exp.account, exp.marketId) == 0) {coverage_0x3c7ab02b(0x09af6425d129f8b9df82578689961d37daaa0e91fa0ccac382c2f84922c97b1b); /* branch */ 

coverage_0x3c7ab02b(0xa827b45dad08eec665627afe99fe0c2eafe4e03171e2c7ebeb1a2e874250cfe6); /* line */ 
                    coverage_0x3c7ab02b(0xa1b8a80519bce7802c045aaf90c4ed8df147e7d584aff05e607c31f1d7ebdc77); /* statement */ 
uint32 newExpiryTime = Time.currentTime().add(exp.timeDelta).to32();
coverage_0x3c7ab02b(0xe9c072cd8a3464ed6a9bc57eca3f6b780563b5a908150f3fd689485f7cfe04bd); /* line */ 
                    coverage_0x3c7ab02b(0xf6352ea4d37eb6ad69eb0186aef327430a7d0fcfcf4b9904c3581c3eeb3a5a71); /* statement */ 
setExpiry(exp.account, exp.marketId, newExpiryTime);
                }else { coverage_0x3c7ab02b(0x0b82426d0d8e50db1e0a60064723136ce5c272a26d1c07924e1f6d0085a853cf); /* branch */ 
}
            } else {coverage_0x3c7ab02b(0x2701528b7d08be1934025384e34cdd79d14339ea838019706c5939f8e7ff1c59); /* branch */ 

                // timeDelta is zero or account has non-negative balance
coverage_0x3c7ab02b(0x3059cac49dcdcc6adae13eae151ecdf987404e8b7c2886d93897c8b4dc9ae334); /* line */ 
                coverage_0x3c7ab02b(0x548aba8519f14065d11d32654e2664986a777d73d2924eeffa440c7c02189611); /* statement */ 
setExpiry(exp.account, exp.marketId, 0);
            }
        }
    }

    function callFunctionSetApproval(
        address sender,
        bytes memory data
    )
        private
    {coverage_0x3c7ab02b(0x39ebf3f7d2d969da08aa0f01ca8364ad75da4b88991ea1e79df62a88ad264a9c); /* function */ 

coverage_0x3c7ab02b(0x57d2dbbb2c527d363aff4b455029f73ca339e7e644063dc41272e9a682083896); /* line */ 
        coverage_0x3c7ab02b(0x12b1c630809a80ca17d235a9ca4ed11f927d9e366228b57b3c10c3ad6507110d); /* statement */ 
(
            CallFunctionType callType,
            SetApprovalArg memory approvalArg
        ) = abi.decode(data, (CallFunctionType, SetApprovalArg));
coverage_0x3c7ab02b(0x37071db3c906bdc00be24174eb894e0616a755515d669ed72140faec371a8ba5); /* line */ 
        coverage_0x3c7ab02b(0x4f8d14edb319785ba4da0b27eb05e348408cea120d07862f0626a97232befba6); /* assertPre */ 
coverage_0x3c7ab02b(0x763331b39062d08154fda48c11e8fddfe686237d99e57bc23ae72d04020c082d); /* statement */ 
assert(callType == CallFunctionType.SetApproval);coverage_0x3c7ab02b(0x32b47376b407191208000720d7030b5ae11245208458c5dd17a117ecccf97f7a); /* assertPost */ 

coverage_0x3c7ab02b(0x45ceaeb04a4a91a753159df4007f3763585ed1c4dfbcd3db1777e2826a75f8f4); /* line */ 
        coverage_0x3c7ab02b(0x3c3c515a2c5154db3f301de31caa3d57ffcde2af0c569a36b16b5a498f9287b9); /* statement */ 
setApproval(sender, approvalArg.sender, approvalArg.minTimeDelta);
    }

    function getTradeCostInternal(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory makerAccount,
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.Wei memory inputWei,
        uint256 owedMarketId,
        uint32 expiry
    )
        private
        returns (Types.AssetAmount memory)
    {coverage_0x3c7ab02b(0x7095a73d2fc4e9545ecc7132cbe1c2d406a938b4abe72312b28e19b8b2bc808a); /* function */ 

coverage_0x3c7ab02b(0x8dd15eee927dfabd10ca579913e981b3960515b00ee6dcc5a02065317576e53b); /* line */ 
        coverage_0x3c7ab02b(0x60721b8294e4df85e7f228b24a727b42f4a80c87ab230146321f59c8f4474129); /* statement */ 
Types.AssetAmount memory output;
coverage_0x3c7ab02b(0x96f68f3a5e21acb4a6a182d1a4152c7ad47aa03bb8ee4b5c68b34adccbd08413); /* line */ 
        coverage_0x3c7ab02b(0x1d917878a8e4d91165db7a6b23dfab33b61a52c134ad13455c54e890d3b8405f); /* statement */ 
Types.Wei memory maxOutputWei = SOLO_MARGIN.getAccountWei(makerAccount, outputMarketId);

coverage_0x3c7ab02b(0xebdf27af2b8a7ef27bfac7ed9c62d0ee48e431b634c71a6dee9fd8d3c8687963); /* line */ 
        coverage_0x3c7ab02b(0x4539c4770e68c9094548488676702bee38a2c9ea253f14a14695906e6d2e4d39); /* statement */ 
if (inputWei.isPositive()) {coverage_0x3c7ab02b(0x5e489dd8f18f113fb3d47125fa69e344e65fcb4d0f52c5360198458bb9e763ac); /* branch */ 

coverage_0x3c7ab02b(0x265ea8fed625200227e6cdb62c35fcafe1bab299c612b0055830627f855b62ec); /* line */ 
            coverage_0x3c7ab02b(0xb51f5f1f782448779d544eabe5b67bdabc2251d855556c97a757ddc154eb4415); /* statement */ 
Require.that(
                inputMarketId == owedMarketId,
                FILE,
                "inputMarket mismatch",
                inputMarketId
            );
coverage_0x3c7ab02b(0x5a3ad4a6547dfbbb0ce8483efd2785fbd92da10d90492e008b9195128d78784b); /* line */ 
            coverage_0x3c7ab02b(0x24a6b64888e131914d9c8ef7e10b05903a87f00800d58a83670413456f435ac0); /* statement */ 
Require.that(
                !newInputPar.isPositive(),
                FILE,
                "Borrows cannot be overpaid",
                newInputPar.value
            );
coverage_0x3c7ab02b(0xffdf6d6995f4f868bb5e37b3419dbdf0b9e50d6a0cfafc65734304d2b6bbf907); /* line */ 
            coverage_0x3c7ab02b(0xa50a4da6ccd73f7a1d44b4ca932a8326a9ca5d48ff24eab561ad365f343d2893); /* assertPre */ 
coverage_0x3c7ab02b(0xfc4e5ee2068710c159d7fd6082e0dc6de9886a1fcae72759cdb62d79700b42f9); /* statement */ 
assert(oldInputPar.isNegative());coverage_0x3c7ab02b(0xb758a87b518b112e1aedc0eb22d06290d18b7b55a470d39c8909a89049b50c77); /* assertPost */ 

coverage_0x3c7ab02b(0xfdbe25194de2fdd76eb240478b12dfc5d214437a1a0bc45cf34d762106885a91); /* line */ 
            coverage_0x3c7ab02b(0xdf1f5f17fced006765d398fcd99ffd0d7b1ff151cec423ddb913fb823781ed3e); /* statement */ 
Require.that(
                maxOutputWei.isPositive(),
                FILE,
                "Collateral must be positive",
                outputMarketId,
                maxOutputWei.value
            );
coverage_0x3c7ab02b(0x397ebfd69fdd94e9d97fc796ddfeaebbcdf8c5fea69a600da7c94dc668d15a95); /* line */ 
            coverage_0x3c7ab02b(0x7d6826500a5522f4714e3cd5614bcbc0310da38556947a98be9b2442e3681cfb); /* statement */ 
output = owedWeiToHeldWei(
                inputWei,
                outputMarketId,
                inputMarketId,
                expiry
            );

            // clear expiry if borrow is fully repaid
coverage_0x3c7ab02b(0x3ad2e4a5a0ec05473dad3a55e02fe4a7201ad4daf7c6e931740deff60476fc90); /* line */ 
            coverage_0x3c7ab02b(0xc808c1fd7c4c0d1bbf7d77f78a3e3114795be53ebfe34bac5586dd5460490e10); /* statement */ 
if (newInputPar.isZero()) {coverage_0x3c7ab02b(0xf9f7f971e079d311202718aa5db35bb2dc670c2f60a1ee6113925246f145df83); /* branch */ 

coverage_0x3c7ab02b(0xcb9c940f2b2983ce28d47a6c31c29ef56a129dea5f5c90579bf8634130306c4a); /* line */ 
                coverage_0x3c7ab02b(0xf3032b7e0cbd1e5b94491159f76f42645f5bac993f08c3da7d9fdbf04a862166); /* statement */ 
setExpiry(makerAccount, owedMarketId, 0);
            }else { coverage_0x3c7ab02b(0xdd9940eae85303468496bceb4d775eeeeafa52b217669ad7dbad9b715cc393e1); /* branch */ 
}
        } else {coverage_0x3c7ab02b(0x7e2d9edf19315554b939a2fdf59e3e2a0a3c71a1ca1d7a6d75ea7cfc155d2b10); /* branch */ 

coverage_0x3c7ab02b(0x0b455bc61a09c05ba49fb1c5d80396eec7d76378ccb7748d2fa2f5df92b732fa); /* line */ 
            coverage_0x3c7ab02b(0xf4bd163554e5fca21d03ffd1afcb48b0c649d1f15997b8db20760f6b6512ee2b); /* statement */ 
Require.that(
                outputMarketId == owedMarketId,
                FILE,
                "outputMarket mismatch",
                outputMarketId
            );
coverage_0x3c7ab02b(0x32bddcad6d31a2eeddd350fafffd5619a46c6373043f03e1875ba1382fccfdbf); /* line */ 
            coverage_0x3c7ab02b(0xf8fc3b4cf79001bcab9df72ce92c99d597d71760f45336513ea8857228ca153b); /* statement */ 
Require.that(
                !newInputPar.isNegative(),
                FILE,
                "Collateral cannot be overused",
                newInputPar.value
            );
coverage_0x3c7ab02b(0xa11220eeb104b96af9e26470bf4443d8af11686dac1938a49b4301fb7b9a6151); /* line */ 
            coverage_0x3c7ab02b(0x8d44cedf4e9637a5ac59f867b003926683d7a2987da3e2d5693719f1450bc921); /* assertPre */ 
coverage_0x3c7ab02b(0x4f680d1d4fd5ee68b379bc77ce14ab9a20592982246a3d325187796c673275bf); /* statement */ 
assert(oldInputPar.isPositive());coverage_0x3c7ab02b(0xc32ee050fea5b59014719996c41963b63c981b8a5634d85aa9a1684194eb051d); /* assertPost */ 

coverage_0x3c7ab02b(0xb1fbf79f780f098f23a29a5115fb16368020ba2fc71b3805464a5dfe1bf1da24); /* line */ 
            coverage_0x3c7ab02b(0x2dc124cd267e2828c8e06fd98f559b89fa291a28c42a1664a62eecdb98030eb5); /* statement */ 
Require.that(
                maxOutputWei.isNegative(),
                FILE,
                "Borrows must be negative",
                outputMarketId,
                maxOutputWei.value
            );
coverage_0x3c7ab02b(0x8860d525237442fc3ea7c50fd389278642a4c3f30f12ce6678bd80641bcfbb89); /* line */ 
            coverage_0x3c7ab02b(0xb81c7e6e7bc5b808f0e129ea5920dd6ca4fd6c5a493488cb96a36777ad4bc722); /* statement */ 
output = heldWeiToOwedWei(
                inputWei,
                inputMarketId,
                outputMarketId,
                expiry
            );

            // clear expiry if borrow is fully repaid
coverage_0x3c7ab02b(0xde2a86fd760522121bd353895582765e4d6e31a9474c826c150da10f5ed8b582); /* line */ 
            coverage_0x3c7ab02b(0xaa60e991423a2013417138d80bd1872257bfe1439dd74511fe984c4efe24cd4c); /* statement */ 
if (output.value == maxOutputWei.value) {coverage_0x3c7ab02b(0xb49f148e4838d52b4a32db8e28d5e7cce2c07175c8215159b29b4cc7d0a2fd79); /* branch */ 

coverage_0x3c7ab02b(0xe7d47d20438379b786601207531fbaa397846e66f6bc227cecdd18999268f530); /* line */ 
                coverage_0x3c7ab02b(0xce5bea0ea5ca49cd51bfc64495c96e4d081d4b72e02c543fd2d90923e9695155); /* statement */ 
setExpiry(makerAccount, owedMarketId, 0);
            }else { coverage_0x3c7ab02b(0x6b27e3c2e524a5ae1cd7925376b0d9bdc2fe558cdbf52a3af0384df3a21bf00f); /* branch */ 
}
        }

coverage_0x3c7ab02b(0xba8ec7128f688822bc7917c14ac79a1260391723193ff0749863691bdeb43d24); /* line */ 
        coverage_0x3c7ab02b(0x104a3c11fc46850321ba072aa628cf98277767124c3a847beec6e4d3922e1c60); /* statement */ 
Require.that(
            output.value <= maxOutputWei.value,
            FILE,
            "outputMarket too small",
            output.value,
            maxOutputWei.value
        );
coverage_0x3c7ab02b(0xf66f11080ea396c80bf10eb81e33bddad1cbf6476966a14ece01631babdcde2c); /* line */ 
        coverage_0x3c7ab02b(0xca4d7fdca86da67279bc90d7d415de3c20600ee3a1802e603dd066bba68e9f0f); /* assertPre */ 
coverage_0x3c7ab02b(0xa0b1e9e69651145861c88aa201959cd2a54e83aafedbe099837eca493a07254f); /* statement */ 
assert(output.sign != maxOutputWei.sign);coverage_0x3c7ab02b(0xfcb43339c53cc466e126a184d782dbc8abc7cd17028749103bed1588d117a01e); /* assertPost */ 


coverage_0x3c7ab02b(0xf2fa96df5ca20bd8a99832ca92bb2264ec5894cff4106e6abf60a12a31157d5f); /* line */ 
        coverage_0x3c7ab02b(0x4cd0b8b9c434f609863857c08bd52a8ad1dbfc9153031241b07f848141850f0c); /* statement */ 
return output;
    }

    function setExpiry(
        Account.Info memory account,
        uint256 marketId,
        uint32 time
    )
        private
    {coverage_0x3c7ab02b(0x3612dd7a0a15b2d12f106e9f571cb7fabd512dc95b40dd1c000432be0f2e26d8); /* function */ 

coverage_0x3c7ab02b(0x4da56d02398d61606730001c9d37cc12ffdc46fa3747fb830a52fb12cbab8b6b); /* line */ 
        coverage_0x3c7ab02b(0x205b5a9e4b332b68869ee2200a8dfe4eaa6019b5a40265b2159116261d7baa69); /* statement */ 
g_expiries[account.owner][account.number][marketId] = time;
coverage_0x3c7ab02b(0x9fe16a3f5f932646fcb4076e679e4716e7fc3db54381dd14e48171073e556e80); /* line */ 
        coverage_0x3c7ab02b(0x1e2accbd9d9f2107e8bda37260faa220eec142e54af6209916be1f32a3799c0f); /* statement */ 
emit ExpirySet(
            account.owner,
            account.number,
            marketId,
            time
        );
    }

    function setApproval(
        address approver,
        address sender,
        uint32 minTimeDelta
    )
        private
    {coverage_0x3c7ab02b(0x64b9afefe34655fa5d5b12cd4c73ddf63cbcfd2f18ad3a5246e697e9a41fcf45); /* function */ 

coverage_0x3c7ab02b(0x77776e1a87fa0a488b537d8854e1d7705460f3f9a592773af6bb865bd0746a88); /* line */ 
        coverage_0x3c7ab02b(0xf121507fed3a21dbe4071ec68859db81fe40094199b30a8954685f86a3cde4cd); /* statement */ 
g_approvedSender[approver][sender] = minTimeDelta;
coverage_0x3c7ab02b(0x840c9c3322d50a717327e0906ff89bcc3a964c22ed19e0e691aedd1030f72c3d); /* line */ 
        coverage_0x3c7ab02b(0x614fa5e775e192b7552736477860348e99b1b4cec49fe6f97999551f00c17be2); /* statement */ 
emit LogSenderApproved(approver, sender, minTimeDelta);
    }

    function heldWeiToOwedWei(
        Types.Wei memory heldWei,
        uint256 heldMarketId,
        uint256 owedMarketId,
        uint32 expiry
    )
        private
        view
        returns (Types.AssetAmount memory)
    {coverage_0x3c7ab02b(0xd57792cfa68c4008d3603006f92d956121d743b1eace21dec43eb901d724b06c); /* function */ 

coverage_0x3c7ab02b(0x4e22dacb5b50de24901796aa877a8be3fa009342e72407271c9b0b3de71d01ba); /* line */ 
        coverage_0x3c7ab02b(0xb27f8fb839bc4f664ac44cd3e724419800b3bbf2c088b3b944c3e5ed11c7d89e); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = getSpreadAdjustedPrices(
            heldMarketId,
            owedMarketId,
            expiry
        );

coverage_0x3c7ab02b(0x2f4d1fc23a23da0a28b65c1e2d257c450499edd35cd848edf2427ec404b0600f); /* line */ 
        coverage_0x3c7ab02b(0x16e7103be3acaebe9461073d2b4e6e8309018956500195d3689ddb4cad46e636); /* statement */ 
uint256 owedAmount = Math.getPartialRoundUp(
            heldWei.value,
            heldPrice.value,
            owedPrice.value
        );

coverage_0x3c7ab02b(0x92fdecfb4a05d65b92e010167734f0e8feec51d93c0cc6a9cb83d5a4c35b4da7); /* line */ 
        coverage_0x3c7ab02b(0xb67c4d6914248ba4acf1a70edf437891099fca623dc8fd5e30b1b4130d265b98); /* statement */ 
return Types.AssetAmount({
            sign: true,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: owedAmount
        });
    }

    function owedWeiToHeldWei(
        Types.Wei memory owedWei,
        uint256 heldMarketId,
        uint256 owedMarketId,
        uint32 expiry
    )
        private
        view
        returns (Types.AssetAmount memory)
    {coverage_0x3c7ab02b(0xfc08c450a715baa0eee0f4d6b08f8147af4737b8350f091ac207a355c6c905e5); /* function */ 

coverage_0x3c7ab02b(0xc5ae8c570ff5b811c805c6c4fe0e5858117e37330d9b809e4c0fccec1761abe6); /* line */ 
        coverage_0x3c7ab02b(0x3ff18bd4afb8d9f48178ba6a44cac10294236e3df6278c3a073c367f6523a446); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = getSpreadAdjustedPrices(
            heldMarketId,
            owedMarketId,
            expiry
        );

coverage_0x3c7ab02b(0xec1d3bcb873910fa0389daa1b15a0f154ca8f1fbb14db54267b57eecac228651); /* line */ 
        coverage_0x3c7ab02b(0xb426cfb1510b3c5c5ad70e547df62dcdff5cd121ec3967456afd218bfb7d7f44); /* statement */ 
uint256 heldAmount = Math.getPartial(
            owedWei.value,
            owedPrice.value,
            heldPrice.value
        );

coverage_0x3c7ab02b(0x3b965481c5501f1343a37ab244200d620306e0d66d6fdadd2dd46a06c87b2e32); /* line */ 
        coverage_0x3c7ab02b(0x35ce05b5d66814afeadfcd58a5817f71ccba1db1a3d890dadc1fb83016cdf449); /* statement */ 
return Types.AssetAmount({
            sign: false,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: heldAmount
        });
    }
}
