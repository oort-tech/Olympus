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

import { Ownable } from "openzeppelin-solidity/contracts/ownership/Ownable.sol";
import { IAutoTrader } from "../../protocol/interfaces/IAutoTrader.sol";
import { Account } from "../../protocol/lib/Account.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Types } from "../../protocol/lib/Types.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";


/**
 * @title Refunder
 * @author dYdX
 *
 * Allows refunding a user for some amount of tokens for some market.
 */
contract Refunder is
    Ownable,
    OnlySolo,
    IAutoTrader
{
function coverage_0x7b6eaaf1(bytes32 c__0x7b6eaaf1) public pure {}

    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "Refunder";

    // ============ Events ============

    event LogGiverAdded(
        address giver
    );

    event LogGiverRemoved(
        address giver
    );

    event LogRefund(
        Account.Info account,
        uint256 marketId,
        uint256 amount
    );

    // ============ Storage ============

    // the addresses that are able to give funds
    mapping (address => bool) public g_givers;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        address[] memory givers
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x7b6eaaf1(0x681d6d05eeaffa4a4c89a8b0e6f7a7ab908586c76a233a9dd792cc32f733bf55); /* function */ 

coverage_0x7b6eaaf1(0x2185f488b259e9fb259f8e22303c0eb6e8536c3a2e52f9f62cb3334f76e4440b); /* line */ 
        coverage_0x7b6eaaf1(0xfc8e06b232fbbbe2f292660e999899519b072f8a5231526b93673558a87a3f72); /* statement */ 
for (uint256 i = 0; i < givers.length; i++) {
coverage_0x7b6eaaf1(0xda9f48c849922451283dcfafa083dddac502ca9395717ae0caf75272094af8b7); /* line */ 
            coverage_0x7b6eaaf1(0x2067d5a8e688040db78bd9deba4e29212e6f85de40aec4c9706e7c146cf7ebe1); /* statement */ 
g_givers[givers[i]] = true;
        }
    }

    // ============ Admin Functions ============

    function addGiver(
        address giver
    )
        external
        onlyOwner
    {coverage_0x7b6eaaf1(0x55fb222b7666e072660e14eb1c4afec0ebdfb5a1bda59e9840dd78629a3bf4e0); /* function */ 

coverage_0x7b6eaaf1(0xfb7c4b8e52baa70ba16087c6cb4f2cc04b84891093dabbdfd81cea98aafe8bc7); /* line */ 
        coverage_0x7b6eaaf1(0xbbc8ac0bf6d702d36d1788c290680a6b13e8f78e187ec26ce203a4e3b2686937); /* statement */ 
emit LogGiverAdded(giver);
coverage_0x7b6eaaf1(0xcca74dfc644513803b82a192768c3c4e91d39d282600a78b7b15bb187ed0eaec); /* line */ 
        coverage_0x7b6eaaf1(0x8fc225b68ffd082492ec2a8943887ee0f010e4fdb4eabd1feaa63e1eaa81ffa4); /* statement */ 
g_givers[giver] = true;
    }

    function removeGiver(
        address giver
    )
        external
        onlyOwner
    {coverage_0x7b6eaaf1(0xefb764026ece589c847f2cac275b77140db97597c6c3058d7f5cda19a496ea8a); /* function */ 

coverage_0x7b6eaaf1(0xfbe1fd413ec42c0047a49f5f87792049a1f5ddd86959f54e8213b59e7ed7c6e6); /* line */ 
        coverage_0x7b6eaaf1(0x6d95d73b13a80fd09f7d3452f2f7a3c0239195c424f4a21c19dae46c529ac79c); /* statement */ 
emit LogGiverRemoved(giver);
coverage_0x7b6eaaf1(0x5c870adf8f14e203031dfbac50758ccf16ddfecb6709606edcbafbbda39c7190); /* line */ 
        coverage_0x7b6eaaf1(0xc809b338855ce6e9c705e9b32505518a9c7e0f0264e6799daf5d604a00facd8f); /* statement */ 
g_givers[giver] = false;
    }

    // ============ Only-Solo Functions ============

    function getTradeCost(
        uint256 inputMarketId,
        uint256 /* outputMarketId */,
        Account.Info memory makerAccount,
        Account.Info memory takerAccount,
        Types.Par memory /* oldInputPar */,
        Types.Par memory /* newInputPar */,
        Types.Wei memory inputWei,
        bytes memory /* data */
    )
        public
        onlySolo(msg.sender)
        returns (Types.AssetAmount memory)
    {coverage_0x7b6eaaf1(0xbdc1112d7428fe667f7e5dab9b886898eade2a8648db6d0bfccedbb500896a9c); /* function */ 

coverage_0x7b6eaaf1(0x83d91a3c77f7f6c90c60eaf520a612a6e36d92096a3dbc4e9f9996f88606438f); /* line */ 
        coverage_0x7b6eaaf1(0xf14798ca1b392b731283c17971bdf1757203d31154230b3c0d3d7d053ba7469e); /* statement */ 
Require.that(
            g_givers[takerAccount.owner],
            FILE,
            "Giver not approved",
            takerAccount.owner
        );

coverage_0x7b6eaaf1(0x0af791270cf79ea306c6ce3b16161b1913d874b1ddf6ee6d887cb75df37f8748); /* line */ 
        coverage_0x7b6eaaf1(0x598ea2606adc21919680cb931f71ba0593bf25adc97b202dee00cb695b41fa0a); /* statement */ 
Require.that(
            inputWei.isPositive(),
            FILE,
            "Refund must be positive"
        );

coverage_0x7b6eaaf1(0x7a9100d399ba51dcd75cc4e3b3f7725665fc16852bc21467a18e44836b307c1f); /* line */ 
        coverage_0x7b6eaaf1(0x0b0e1a1c217b6044ad7e879773480ae7c40e8a337f80a2b5d106e49da03060f2); /* statement */ 
emit LogRefund(
            makerAccount,
            inputMarketId,
            inputWei.value
        );

coverage_0x7b6eaaf1(0xc23699a165489fe8f776a48550182d93db6dadf2641363613427132728f0c426); /* line */ 
        coverage_0x7b6eaaf1(0x4abf4c6e7dfa8fb2659866b75fe365aefe124bddbfab02ae48ecc1789ddacec0); /* statement */ 
return Types.AssetAmount({
            sign: false,
            denomination: Types.AssetDenomination.Par,
            ref: Types.AssetReference.Delta,
            value: 0
        });
    }
}
