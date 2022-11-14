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
 * @title Expiry
 * @author dYdX
 *
 * Sets the negative balance for an account to expire at a certain time. This allows any other
 * account to repay that negative balance after expiry using any positive balance in the same
 * account. The arbitrage incentive is the same as liquidation in the base protocol.
 */
contract Expiry is
    Ownable,
    OnlySolo,
    ICallee,
    IAutoTrader
{
function coverage_0x6ab20b04(bytes32 c__0x6ab20b04) public pure {}

    using SafeMath for uint32;
    using SafeMath for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "Expiry";

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

    // ============ Storage ============

    // owner => number => market => time
    mapping (address => mapping (uint256 => mapping (uint256 => uint32))) g_expiries;

    // time over which the liquidation ratio goes from zero to maximum
    uint256 public g_expiryRampTime;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        uint256 expiryRampTime
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x6ab20b04(0xbc4422505b89ac152608164c1cd75fa77db7ec1d765f272d2f703ed250897995); /* function */ 

coverage_0x6ab20b04(0xffc4bdf900770f3118f7552945dde200a288b9dca6079c533dac386ce5dced2c); /* line */ 
        coverage_0x6ab20b04(0x39800608734331054ead761637f7f64669371873c8f345d7f94a88f8e5bd15bb); /* statement */ 
g_expiryRampTime = expiryRampTime;
    }

    // ============ Admin Functions ============

    function ownerSetExpiryRampTime(
        uint256 newExpiryRampTime
    )
        external
        onlyOwner
    {coverage_0x6ab20b04(0x2573798a6c7a3715e29a8c40c20eb0fc2565698eb7852904bda0203278774751); /* function */ 

coverage_0x6ab20b04(0x7922dc78b7d352a2b0971f8799a0e7e1891153498cdb4a9a15a0c94a44c1a728); /* line */ 
        coverage_0x6ab20b04(0x71b1be60fca3923fdb752529147386a8ab9608134b68e6f3d43288a66ad73ffd); /* statement */ 
emit LogExpiryRampTimeSet(newExpiryRampTime);
coverage_0x6ab20b04(0xfb00166d97cc5aae65489663041dd6051499d4a7c90cc44013b705189d6ab183); /* line */ 
        coverage_0x6ab20b04(0x8cc403a3cf27ac350b82273030229ba4f281e280269fc91095855bcf19e37662); /* statement */ 
g_expiryRampTime = newExpiryRampTime;
    }

    // ============ Only-Solo Functions ============

    function callFunction(
        address /* sender */,
        Account.Info memory account,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
    {coverage_0x6ab20b04(0xec50822b8445acfef4cc81213b0f4055f5bdeafc7111e6c9583da3ac5c744ba7); /* function */ 

coverage_0x6ab20b04(0xfe349f05ab9941a0670fdbad81b59b3474d6199d5801191b4b2f2be334132c5d); /* line */ 
        coverage_0x6ab20b04(0x1f742fbe52d71ea8d7a622250385ebe08a72ca09578a3dabcd574a6015a07f94); /* statement */ 
(
            uint256 marketId,
            uint32 expiryTime
        ) = parseCallArgs(data);

        // don't set expiry time for accounts with positive balance
coverage_0x6ab20b04(0xf4c569ab16438669e52548edd70174c1d186e656648c9b309470d815e78008df); /* line */ 
        coverage_0x6ab20b04(0x309cc5304a4c57bdf14df1853ab8f974db61b06e171639f600c08afa901992af); /* statement */ 
if (expiryTime != 0 && !SOLO_MARGIN.getAccountPar(account, marketId).isNegative()) {coverage_0x6ab20b04(0xb98d6e89b96a2d7604a214edf1d2e02e5cac9b2b18bee70a0525b40d647035f7); /* branch */ 

coverage_0x6ab20b04(0xbfcb7be99082d1d80e3c8b96c8f500ed77fbb7f096128e25a421a67b8159e086); /* line */ 
            coverage_0x6ab20b04(0x1d039735eb098591c0abed342d582c432accd7497a3548aa1a5b9e29d4276525); /* statement */ 
return;
        }else { coverage_0x6ab20b04(0x2be1c1e123e296966402c0544e0dea7cad2a535fafc0d1430b396f9c4d53ab92); /* branch */ 
}

coverage_0x6ab20b04(0xbf4e1327de0985d65c29a995c29e54e167d15042fadcfbabe4db111004861e3b); /* line */ 
        coverage_0x6ab20b04(0x6e57338a4eeb28b31dfb27327274cc463f7b5736d3d9b7a2f36e1015c0f67344); /* statement */ 
setExpiry(account, marketId, expiryTime);
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
    {coverage_0x6ab20b04(0x03b1419cf07246bd8d47e2acdded01e8deeb6b314937e6e505862e28122120d7); /* function */ 

        // return zero if input amount is zero
coverage_0x6ab20b04(0x366fb34916a7800631bd0bd1d0c19d75b5f7a29cc7d7b517c2714da74c4166bc); /* line */ 
        coverage_0x6ab20b04(0xe2e120dc5ea7e48bff2c27ef25f8556238679795e7af2b9f703a0c09d46fc999); /* statement */ 
if (inputWei.isZero()) {coverage_0x6ab20b04(0x6a27ed51e327e16dad52ab83894c69d788bc095df4a40c66ba614e7e012a82b1); /* branch */ 

coverage_0x6ab20b04(0xae2f9853e563613e6a45196f2042320fb8aab374b3eee29183bdf6f3651924fd); /* line */ 
            coverage_0x6ab20b04(0x3c2033e4aaa25ed0439d40e989f0c5424f2e56894ef4d996ad73fb2b0e17441e); /* statement */ 
return Types.AssetAmount({
                sign: true,
                denomination: Types.AssetDenomination.Par,
                ref: Types.AssetReference.Delta,
                value: 0
            });
        }else { coverage_0x6ab20b04(0x197c7e7ca07aa50528c2f437eebceab0d96f341d066ab007efd5d47c4e2c95be); /* branch */ 
}

coverage_0x6ab20b04(0x24b85c0b26103a263a2cf2c0b730e9323b369251c6c96ae24cd2d0bf47bc7ed7); /* line */ 
        coverage_0x6ab20b04(0x9d68ee77ea83cac4d4227917ed71861dfe93b0c307ef93a5c67e280c205ef416); /* statement */ 
(
            uint256 owedMarketId,
            uint32 maxExpiry
        ) = parseTradeArgs(data);

coverage_0x6ab20b04(0xcb123574a655fdf77c60f5bf329b5c4378eb3066fc01009baba19fdfc872db37); /* line */ 
        coverage_0x6ab20b04(0x28d2cea398cd667bb181963cb385f6f9eb9985866a16b72145e7578c9afe7050); /* statement */ 
uint32 expiry = getExpiry(makerAccount, owedMarketId);

        // validate expiry
coverage_0x6ab20b04(0x49db1ec7bef2fe78e34967c4024a21c90f5c5b6af5891ebb02c757248afa2ed5); /* line */ 
        coverage_0x6ab20b04(0x11101b2318056826af3278c53e369051f9c4e0863dc9199efd7dcfddb95d2cdc); /* statement */ 
Require.that(
            expiry != 0,
            FILE,
            "Expiry not set",
            makerAccount.owner,
            makerAccount.number,
            owedMarketId
        );
coverage_0x6ab20b04(0xd2043bbc9e8a44c2c11de423f5bce9dd25bb12e7159dc34c8287000da83676c9); /* line */ 
        coverage_0x6ab20b04(0x363703261acf429469ff2b4108cff074d027789ef35012cb1e8e6d8d505c9ec3); /* statement */ 
Require.that(
            expiry <= Time.currentTime(),
            FILE,
            "Borrow not yet expired",
            expiry
        );
coverage_0x6ab20b04(0x2ec127feafec47c301f9559629f91bb9627a22346c96bd43e10d1816a067d6d6); /* line */ 
        coverage_0x6ab20b04(0x21c4893163bbbff9158b06d85f66f9e9e5fb7e37896672059be1d5dbd5c7d888); /* statement */ 
Require.that(
            expiry <= maxExpiry,
            FILE,
            "Expiry past maxExpiry",
            expiry
        );

coverage_0x6ab20b04(0xb7d0fbe92fba6fc2b520d3c00d064d5f8c35393433018c978823d0580875ad76); /* line */ 
        coverage_0x6ab20b04(0x6333a4dde4e5eb138787bb0a5d576d7303f8d9b5387fc869ec7efc3edd658dd7); /* statement */ 
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
    {coverage_0x6ab20b04(0x6117166965140f1e4b162cf9ab69818a56d4391326a37a353201fff13c42204e); /* function */ 

coverage_0x6ab20b04(0x02c63b81547a2297146867c5836d0acaf57f2908083ca2a0a5ca65896727566d); /* line */ 
        coverage_0x6ab20b04(0xc11d9f7c5b8a34c64318c38ea6038c356cfb49cef3b9230a76c709f373330b41); /* statement */ 
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
    {coverage_0x6ab20b04(0x29cce6c0c094ec4d1fdf654b38a8db901e8984667758019c71396e18da604e5d); /* function */ 

coverage_0x6ab20b04(0xc5f0d063a3496c9a59d58a42b4ba4dc895db06475d292addc83703e183dc6e18); /* line */ 
        coverage_0x6ab20b04(0x0ce6eee88bf895947f9fe399a723d386e10fdb6f3be01c91fc69ece8b3f13f3b); /* statement */ 
Decimal.D256 memory spread = SOLO_MARGIN.getLiquidationSpreadForPair(
            heldMarketId,
            owedMarketId
        );

coverage_0x6ab20b04(0x4753c379a71df7842a3cdfcef53e5ccc4205b99155f2f15c94b5eeeb7dad8103); /* line */ 
        coverage_0x6ab20b04(0x33b8c929474084882df21f8b9e4c5c319ee2a53447f1e444d8402a9ad12cf77f); /* statement */ 
uint256 expiryAge = Time.currentTime().sub(expiry);

coverage_0x6ab20b04(0xdbb70d47dad483f6b5705dbf1e2b5f7c5a27bb15c680ef36b93919ee66a174b8); /* line */ 
        coverage_0x6ab20b04(0x9c0a8004602a91e212b0b403ccda43006021d0d1b09f85da24cdec55a4b7e02e); /* statement */ 
if (expiryAge < g_expiryRampTime) {coverage_0x6ab20b04(0x6f97a107dc104259c404e9f72d7eff74dd0e3635f1655fd61f5c4a8d3bc92224); /* branch */ 

coverage_0x6ab20b04(0xe6c873ee6b3df31aef3c65afb21cf239bd55c37a71b7779da1be163e7611c145); /* line */ 
            coverage_0x6ab20b04(0xea4d14847b1da33192f0b65ade905943c35436a6687a72f96eaabbdcbc262e61); /* statement */ 
spread.value = Math.getPartial(spread.value, expiryAge, g_expiryRampTime);
        }else { coverage_0x6ab20b04(0xeb85373364c99b87ca45f6b0665d170d6b24e762d074a879e52429fce302e8f2); /* branch */ 
}

coverage_0x6ab20b04(0xf70ecb759f8f08269a88a2c8ac09742bfd4983c5e490e679bccea10599f48cf2); /* line */ 
        coverage_0x6ab20b04(0x955bec9ea2b84f5134587f7f968639f7b0c37f66cada9f0fa4c89a7325cfb687); /* statement */ 
Monetary.Price memory heldPrice = SOLO_MARGIN.getMarketPrice(heldMarketId);
coverage_0x6ab20b04(0x42680768d174444b9efea957d82d0286200b9792560ab11971c9a2e08971466e); /* line */ 
        coverage_0x6ab20b04(0xd7514a5967bada474ed5f99abacb50aad7eb22f976e44f62edbc6fc42606d375); /* statement */ 
Monetary.Price memory owedPrice = SOLO_MARGIN.getMarketPrice(owedMarketId);
coverage_0x6ab20b04(0x496db89115e7efb6da16efdd11bf218c212de6736e324b461b7a1b3b31ba5981); /* line */ 
        coverage_0x6ab20b04(0x44fc11a5ac1c8dce670d969304ecc2690643eddaceb5449c392ae4eeb2cca03f); /* statement */ 
owedPrice.value = owedPrice.value.add(Decimal.mul(owedPrice.value, spread));

coverage_0x6ab20b04(0x581ca9c966457dfa8bd5615e72174cf4aa5524b7e9a983caf40df7291734942f); /* line */ 
        coverage_0x6ab20b04(0x47fb7a112897b85fe507bdce48d8b1bf91c66a5b37b7e71ec3e209d6898d91eb); /* statement */ 
return (heldPrice, owedPrice);
    }

    // ============ Private Functions ============

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
    {coverage_0x6ab20b04(0x3b25f1c89d89def4e2e31ba4b3db51fe22acd7a7d9c87d3e071eb04fde36aa27); /* function */ 

coverage_0x6ab20b04(0xafc7fd3cc363f44ba5d941bb5d9a667497d504df1077441dc6ca249c24b21477); /* line */ 
        coverage_0x6ab20b04(0xaf24394c96054cb38bcbad857dee6cdae224e4440de1a7f17d2c91b370d0c9a9); /* statement */ 
Types.AssetAmount memory output;
coverage_0x6ab20b04(0xf9585c4192dd83ece48d5e90125484a24a6cff0daf4ff66740108657bd224dc4); /* line */ 
        coverage_0x6ab20b04(0xe126639d48566136ebce8c291426a32ff6c6a72c272ae0d55f1b1019d1dd04ae); /* statement */ 
Types.Wei memory maxOutputWei = SOLO_MARGIN.getAccountWei(makerAccount, outputMarketId);

coverage_0x6ab20b04(0x9919731106b99d2d195b4a39674f6aabf6de44e6e30b8fcc1cba5549a5b2b3c1); /* line */ 
        coverage_0x6ab20b04(0x0e7b0768d1ad147df808a822358748d579ffdb9cd54a85e3d32e2054988485f9); /* statement */ 
if (inputWei.isPositive()) {coverage_0x6ab20b04(0xe2fb2045fdbcfd966dddb4495e3e326abdca581ce9a0614320fd99525aa49d71); /* branch */ 

coverage_0x6ab20b04(0x65aef0c4bcbdb3ec68a3a062cb5ed0882d5dd73cadbe304c9536235441c45c4f); /* line */ 
            coverage_0x6ab20b04(0xe6b14aca15611159d386ea93f353ffe4eb292ba4833f6e2bbfdaed8bb679aeed); /* statement */ 
Require.that(
                inputMarketId == owedMarketId,
                FILE,
                "inputMarket mismatch",
                inputMarketId
            );
coverage_0x6ab20b04(0x83f3468762446683d487aa816b10bf9df9d78dc9a0dc72b401a641fde86f6165); /* line */ 
            coverage_0x6ab20b04(0xdfad7067cbee65aba1c78cc173583a4008ab30b2e133688264758566cc5e499f); /* statement */ 
Require.that(
                !newInputPar.isPositive(),
                FILE,
                "Borrows cannot be overpaid",
                newInputPar.value
            );
coverage_0x6ab20b04(0xeb9f7f8b8a7eb712854f548bc015eb00041f5a676defe23ba58b7fd365b536c8); /* line */ 
            coverage_0x6ab20b04(0x6c481c8ea022bbf11cb2cec6683d51098b5cfa2f5935fad3a1db0ea1dbe0c787); /* assertPre */ 
coverage_0x6ab20b04(0x0a53e8a8b0f370c73580150f0d335ec6b5caa4e1c478c1df85eb4d439c9a2f69); /* statement */ 
assert(oldInputPar.isNegative());coverage_0x6ab20b04(0x9f52dd815b8c0387e9e128bc7d00888cf9aec1c366f24a6ea896d3c12514b26e); /* assertPost */ 

coverage_0x6ab20b04(0x5dcc2d2aee33a60cb3bf113105fc7007800ad89b99f01700b9670412d0323ab3); /* line */ 
            coverage_0x6ab20b04(0x3d9fb0004fbc70bb23e287506550c68ce7a603d2fe919e20b97a453c62bd552a); /* statement */ 
Require.that(
                maxOutputWei.isPositive(),
                FILE,
                "Collateral must be positive",
                outputMarketId,
                maxOutputWei.value
            );
coverage_0x6ab20b04(0xbf3ff92e0a689be4a6bc2f4d644b29024344383926b5957937461e152f9ba2d4); /* line */ 
            coverage_0x6ab20b04(0x488a5fa01d34d9674454d838b17a4c61766a51fe588b7ebfa83676e440071498); /* statement */ 
output = owedWeiToHeldWei(
                inputWei,
                outputMarketId,
                inputMarketId,
                expiry
            );

            // clear expiry if borrow is fully repaid
coverage_0x6ab20b04(0x2d831a45a7b099186f132b2427c0a9bd31e7a7f66264801c2fd6ef94a5d63375); /* line */ 
            coverage_0x6ab20b04(0x405ef306cab35d87d3dfc01337459a25e035832638227f410204341ca8df87bb); /* statement */ 
if (newInputPar.isZero()) {coverage_0x6ab20b04(0x681a049db82b9d59e9930b1075ddd942f079e09a1a187613eb1975f2d442c83b); /* branch */ 

coverage_0x6ab20b04(0x295cc116d456f2f61ee68d43111b9c546bb6d5caf36b273c31cebc4aa0bbb86f); /* line */ 
                coverage_0x6ab20b04(0x05f10fb4ce88f163582b394cd9d9e4230b7479e84e4f12a5bac6cab6d2cee7fd); /* statement */ 
setExpiry(makerAccount, owedMarketId, 0);
            }else { coverage_0x6ab20b04(0xf44338fcb74f46719e6ded864c153d4ffbc26c3ae9a02fcfc98f494fce3819af); /* branch */ 
}
        } else {coverage_0x6ab20b04(0x6d68bc7fa22de0c7505a5bb0890b218b9079429b3de18b13447d16ac50b288e7); /* branch */ 

coverage_0x6ab20b04(0x1078819a9fc870f75b2c86a55c69298fc9194255f9bfbaff05d300820fc7bc2a); /* line */ 
            coverage_0x6ab20b04(0x38c0158d47d8f55d17991321ee0313ea1f3ba1f98c77ed91d039ef2e6a2ade2c); /* statement */ 
Require.that(
                outputMarketId == owedMarketId,
                FILE,
                "outputMarket mismatch",
                outputMarketId
            );
coverage_0x6ab20b04(0x88ac2b40be39ab9981abb8626d4bfa56609d8140208945026e621e578a7e97d6); /* line */ 
            coverage_0x6ab20b04(0x1e024dbbc2614bca1c83491eb912eac8573a67ac953368b61bce0d2900b5f46e); /* statement */ 
Require.that(
                !newInputPar.isNegative(),
                FILE,
                "Collateral cannot be overused",
                newInputPar.value
            );
coverage_0x6ab20b04(0x573e065b7810374c7323daad480aa6a0a0d9447ed86297e6647021c6f847ab8d); /* line */ 
            coverage_0x6ab20b04(0xf5e36fc73d1f0c93df8d3b1ed3c703cdab98d29a3bef2d6f1877563180b56cb5); /* assertPre */ 
coverage_0x6ab20b04(0x5494cb66f19a40740be23e7d736f7b32c8e938d38dc14eae0b37880f4c4bc51c); /* statement */ 
assert(oldInputPar.isPositive());coverage_0x6ab20b04(0x1168f864d7aa6d1e2b5c1cb4c722f71ca8a3ec09aa8f9e9bb05cb3ceab4d7486); /* assertPost */ 

coverage_0x6ab20b04(0xc6d250df8237b1e1ed33edb6e495f1d789e3fdabec0db68cf1e7dc6a16b73eac); /* line */ 
            coverage_0x6ab20b04(0x52a21e5798e0a30f83d924d164f4da71c37e7104e17f9caa12894863da69d555); /* statement */ 
Require.that(
                maxOutputWei.isNegative(),
                FILE,
                "Borrows must be negative",
                outputMarketId,
                maxOutputWei.value
            );
coverage_0x6ab20b04(0xcbf66dba506459b8b95958d775ebc8ba2a02a5fecac9923637da797f8f0f2054); /* line */ 
            coverage_0x6ab20b04(0x8847de19a615cdec70647433b2ae92934834e1d565b2deecb613a6aba447fd77); /* statement */ 
output = heldWeiToOwedWei(
                inputWei,
                inputMarketId,
                outputMarketId,
                expiry
            );

            // clear expiry if borrow is fully repaid
coverage_0x6ab20b04(0x4de771d1bcecf2a7dfdd3e4f3282d8cc47815ff8025186ad65e68c460fe78538); /* line */ 
            coverage_0x6ab20b04(0x1c63e5d08ceb398520e5261c6e93db111dfbfa92556e1549e05aabc146ccb03c); /* statement */ 
if (output.value == maxOutputWei.value) {coverage_0x6ab20b04(0xa485f8e4d317496839770c941de1722dffdff5c16e95559a75ea1c234520dcab); /* branch */ 

coverage_0x6ab20b04(0xc59070229467576b8f903d1c342681280e4b4466838c836697f1616a5ec2e8a2); /* line */ 
                coverage_0x6ab20b04(0x4fdb278b99449febb19d7817d31d22458ecb1c33f71e5187bf9dd7a9e25b6851); /* statement */ 
setExpiry(makerAccount, owedMarketId, 0);
            }else { coverage_0x6ab20b04(0x96c7f59818c56fd3dbeb4f0c83bd61127e32f71ac95d0b9c11f3ba20e4f5965f); /* branch */ 
}
        }

coverage_0x6ab20b04(0xdd00cf01599dc6ad2dde18e26177320c866afdc87cff075d1bb06a6f31bb1c6b); /* line */ 
        coverage_0x6ab20b04(0x0fc631f28915b174466620697482681319dc01ed9f864225fda92a1b8d687ae5); /* statement */ 
Require.that(
            output.value <= maxOutputWei.value,
            FILE,
            "outputMarket too small",
            output.value,
            maxOutputWei.value
        );
coverage_0x6ab20b04(0x412580c375c371aaaa493ec1b6e60783b140b4b774508fe23193e21b00731293); /* line */ 
        coverage_0x6ab20b04(0x416199172365261a36a6be6f8e4ff0ef17e82d4b48fe88b52e5628a315647a76); /* assertPre */ 
coverage_0x6ab20b04(0x730e1c7c0f2722d61133777dc64bc1d0a8679eb48444a2fd7ee2254ed1c5c2d7); /* statement */ 
assert(output.sign != maxOutputWei.sign);coverage_0x6ab20b04(0x2d0212b5e4bf177b915a59beac447aab71983b86eef9886cdaf195150aebbdc4); /* assertPost */ 


coverage_0x6ab20b04(0xa821fa646fd66dede4404cfbfa14b95dd2f4248b1ded887dbb8df33ad04a5b7c); /* line */ 
        coverage_0x6ab20b04(0x50022bd0efd7e02b38ce09191ba699872d7d7635c986715f31ce433d9a5e6a03); /* statement */ 
return output;
    }

    function setExpiry(
        Account.Info memory account,
        uint256 marketId,
        uint32 time
    )
        private
    {coverage_0x6ab20b04(0x3ba331d455ebf4d281c55475d86963d0704b769dfa4e17476c5dc4610a5803e3); /* function */ 

coverage_0x6ab20b04(0xce43fb577a22564e06a6ba4f2c91b704f867865a135dfef2f5fb15d001df7135); /* line */ 
        coverage_0x6ab20b04(0x6b7ac6d7682afb0c34208c990602f269663cc0e65e5ad732b75ec4074430dc0e); /* statement */ 
g_expiries[account.owner][account.number][marketId] = time;

coverage_0x6ab20b04(0x29026eca63adea05d9fd2d49d7e128dfa613d3a98395b3d14d0fd5b9450795d1); /* line */ 
        coverage_0x6ab20b04(0x5a3bf16265bbb5597947213627f2546f2b1991e65d74f4bee7d2d1e42eddd6fb); /* statement */ 
emit ExpirySet(
            account.owner,
            account.number,
            marketId,
            time
        );
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
    {coverage_0x6ab20b04(0x50ebb63bf38b63cf194bee4df1b70905dd99da9dae554636dc541d80c7563715); /* function */ 

coverage_0x6ab20b04(0x55fc262306734df77908e1aab0d56c0b71b076403c4f018ac1be0e0adc0f734d); /* line */ 
        coverage_0x6ab20b04(0x90d653be8c465be7ef9ad572fd73f71237605fc8cf3d8094ace44bb63cc984a5); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = getSpreadAdjustedPrices(
            heldMarketId,
            owedMarketId,
            expiry
        );

coverage_0x6ab20b04(0xa6c72d09087d41d557c7428d9f9d321f270885718358a54d02f551b50f34dce0); /* line */ 
        coverage_0x6ab20b04(0x00fb1a44013b80150c32803499a0afdceff5c5618627b2f976128ade246baf40); /* statement */ 
uint256 owedAmount = Math.getPartialRoundUp(
            heldWei.value,
            heldPrice.value,
            owedPrice.value
        );

coverage_0x6ab20b04(0x907ddb32dc97538ff7fe9b536f0cf57782ab574e87d4ef4a0cc18ccde40153e1); /* line */ 
        coverage_0x6ab20b04(0x3943228981782283090792b9454ab0c6575c0029bac70a5a701dd9f35ba06b10); /* statement */ 
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
    {coverage_0x6ab20b04(0x0f6b5ad09f6fcfe99705b299c640a1b6b689099eb49af5e01ee5a7c6cf6944de); /* function */ 

coverage_0x6ab20b04(0xb035287733ccf57919068ad7b73628146ffa314bbd7b8888fc995a6537a61a94); /* line */ 
        coverage_0x6ab20b04(0x936348afa8644159900d23861955ab1da81e0d8e0991e715f1c5e44f74b90dab); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = getSpreadAdjustedPrices(
            heldMarketId,
            owedMarketId,
            expiry
        );

coverage_0x6ab20b04(0x3915cb6989ab007697f3d8d34a764af13c43fc16dbd87a81c26d2821c03a45ee); /* line */ 
        coverage_0x6ab20b04(0x57e1c6454086f803ffe0602cdf430eca7ae822c92ea97d2c29713160b7d5fe82); /* statement */ 
uint256 heldAmount = Math.getPartial(
            owedWei.value,
            owedPrice.value,
            heldPrice.value
        );

coverage_0x6ab20b04(0x23d3fdff33efd19d5e541ff706235acefc0fe1025da8b8aa1bdfb3479d9bece0); /* line */ 
        coverage_0x6ab20b04(0x9b9ac769b5befd6ae7a74c0e35df918f26b019e0db206ce8aba6d65a42355bd2); /* statement */ 
return Types.AssetAmount({
            sign: false,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: heldAmount
        });
    }

    function parseCallArgs(
        bytes memory data
    )
        private
        pure
        returns (
            uint256,
            uint32
        )
    {coverage_0x6ab20b04(0x31f9a529a29e5e04c8523e5330ccaac82e74bae685180157c5d4742761267bb4); /* function */ 

coverage_0x6ab20b04(0xa36a2992f31127cec799397c59be99d6d6360d6740b9bdfb6f8c84a22d221df2); /* line */ 
        coverage_0x6ab20b04(0x5b6359cf8b790126c435bf1786e73daf8b1513eda5f9cb360dd719f0238ed7c5); /* statement */ 
Require.that(
            data.length == 64,
            FILE,
            "Call data invalid length",
            data.length
        );

coverage_0x6ab20b04(0x1d9c6666bd46c1712d3192eda32decd2b3a700a97a6377d648fd7357e734d48f); /* line */ 
        coverage_0x6ab20b04(0xac29fd0d7b02cc5ba6d92a89b70825ae0adc252c1d1cb4399f0928c9bdbaf2da); /* statement */ 
uint256 marketId;
coverage_0x6ab20b04(0x3468ffa3fe7065c59695702b53a8eb1542d69daceb96309e57cda1de30e6b672); /* line */ 
        coverage_0x6ab20b04(0xf763234e797efd7ee971bcd7b844025162d6b6104dfa912cd7a6790579b77bb6); /* statement */ 
uint256 rawExpiry;

        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x6ab20b04(0x5441843e8d36fcf5c78c693face1e7ad118ea53dab98485a5b6126b271e27de2); /* line */ 
        assembly {
            marketId := mload(add(data, 32))
            rawExpiry := mload(add(data, 64))
        }

coverage_0x6ab20b04(0xf44689f0e3ff122f901e0f97f3918b8bcf5067c8fd8b9175de1113920d6b0c59); /* line */ 
        coverage_0x6ab20b04(0x8151f79f095a2ff4aed84f34c4b2ae8345c4c65f898c2f8961c42e997be8919c); /* statement */ 
return (
            marketId,
            Math.to32(rawExpiry)
        );
    }

    function parseTradeArgs(
        bytes memory data
    )
        private
        pure
        returns (
            uint256,
            uint32
        )
    {coverage_0x6ab20b04(0x12e3b5177b3d24a04fc132578cd9030766f8b5141bc2296cfdba90259ad0aac9); /* function */ 

coverage_0x6ab20b04(0x4751195c892904c72fa9ff3a6aec28b5d2c5f0aa572d878d8bf1c34c5ce5b7c1); /* line */ 
        coverage_0x6ab20b04(0x27a0e0b1dfe7de4de8c8efd5ceed9f5afdbdd6af2ef55827a193c51792585c27); /* statement */ 
Require.that(
            data.length == 64,
            FILE,
            "Trade data invalid length",
            data.length
        );

coverage_0x6ab20b04(0x5d30315f3904c5e13334f93bb65ead20476914856f3d16eb279099d52f5d3a87); /* line */ 
        coverage_0x6ab20b04(0xc5c32eb8fc8a83144b7964ea31569e022da8bfebc35a2a5073a97b96338e1ceb); /* statement */ 
uint256 owedMarketId;
coverage_0x6ab20b04(0x4259c638ce49d2b37592c5f65b0ef80265be79f35c757a8ffa1e8db269362dd6); /* line */ 
        coverage_0x6ab20b04(0x4a01094caed357cec69b8bffecc6797c1e4daaad418838b8189978f455be1789); /* statement */ 
uint256 rawExpiry;

        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x6ab20b04(0x8186fb92fd19b8d95357a66758d0a97f79e05ea8b4989de08eb79e9977583674); /* line */ 
        assembly {
            owedMarketId := mload(add(data, 32))
            rawExpiry := mload(add(data, 64))
        }

coverage_0x6ab20b04(0xea83b11e88681756ad13ff1a21f9b03c3d1913475c4aa9a43b236dd8d98a48d2); /* line */ 
        coverage_0x6ab20b04(0x53efe4d10bcf37c1eaa118dad92ba05b94ddfacf6d6f83db4722c253c60fe1dc); /* statement */ 
return (
            owedMarketId,
            Math.to32(rawExpiry)
        );
    }
}
