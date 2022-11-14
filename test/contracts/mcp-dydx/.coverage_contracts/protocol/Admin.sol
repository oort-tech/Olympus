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
import { ReentrancyGuard } from "openzeppelin-solidity/contracts/utils/ReentrancyGuard.sol";
import { State } from "./State.sol";
import { AdminImpl } from "./impl/AdminImpl.sol";
import { IInterestSetter } from "./interfaces/IInterestSetter.sol";
import { IPriceOracle } from "./interfaces/IPriceOracle.sol";
import { Decimal } from "./lib/Decimal.sol";
import { Interest } from "./lib/Interest.sol";
import { Monetary } from "./lib/Monetary.sol";
import { Token } from "./lib/Token.sol";


/**
 * @title Admin
 * @author dYdX
 *
 * Public functions that allow the privileged owner address to manage Solo
 */
contract Admin is
    State,
    Ownable,
    ReentrancyGuard
{
function coverage_0xf83122b5(bytes32 c__0xf83122b5) public pure {}

    // ============ Token Functions ============

    /**
     * Withdraw an ERC20 token for which there is an associated market. Only excess tokens can be
     * withdrawn. The number of excess tokens is calculated by taking the current number of tokens
     * held in Solo, adding the number of tokens owed to Solo by borrowers, and subtracting the
     * number of tokens owed to suppliers by Solo.
     */
    function ownerWithdrawExcessTokens(
        uint256 marketId,
        address recipient
    )
        public
        onlyOwner
        nonReentrant
        returns (uint256)
    {coverage_0xf83122b5(0x9aceaebe9183249222fda132b5ceede1261eee39d8f17743ffa3888894f2dcfd); /* function */ 

coverage_0xf83122b5(0xf9ffc72580cd1ece3c80fcfffbc68687525e2c969aa8bfcf2f12aa0de9ae53c9); /* line */ 
        coverage_0xf83122b5(0x75ff78dd3499cc58b753a78d0f57948a6fa9c05adc6cf4e0d00498f78276924c); /* statement */ 
return AdminImpl.ownerWithdrawExcessTokens(
            g_state,
            marketId,
            recipient
        );
    }

    /**
     * Withdraw an ERC20 token for which there is no associated market.
     */
    function ownerWithdrawUnsupportedTokens(
        address token,
        address recipient
    )
        public
        onlyOwner
        nonReentrant
        returns (uint256)
    {coverage_0xf83122b5(0x3cf92f94397c208046cbedd55d12b12d89bcbc491d1b822ad978c5a6af467343); /* function */ 

coverage_0xf83122b5(0x969e4dbc49df1fa37a3f1d9d26e84c47c1bdca8c12510f74b54ffa26accd1ff4); /* line */ 
        coverage_0xf83122b5(0x3810462b543d4d96e154a01405deff87a94b62ec6b324a22435b071c5323876f); /* statement */ 
return AdminImpl.ownerWithdrawUnsupportedTokens(
            g_state,
            token,
            recipient
        );
    }

    // ============ Market Functions ============

    /**
     * Add a new market to Solo. Must be for a previously-unsupported ERC20 token.
     */
    function ownerAddMarket(
        address token,
        IPriceOracle priceOracle,
        IInterestSetter interestSetter,
        Decimal.D256 memory marginPremium,
        Decimal.D256 memory spreadPremium
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x6c7816cf15067a485024322bf6d59c9ac3de82ce885ca94698899729ee9558ad); /* function */ 

coverage_0xf83122b5(0xc28d0751142c6ac2dd24eacb18aeacd35f35534398a25bd203db54cdbf01b9f9); /* line */ 
        coverage_0xf83122b5(0xa74720cd17cde76bccd8eda06d2cefcebe37ebbd36f938545fd252b25e6eaed8); /* statement */ 
AdminImpl.ownerAddMarket(
            g_state,
            token,
            priceOracle,
            interestSetter,
            marginPremium,
            spreadPremium
        );
    }

    /**
     * Set (or unset) the status of a market to "closing". The borrowedValue of a market cannot
     * increase while its status is "closing".
     */
    function ownerSetIsClosing(
        uint256 marketId,
        bool isClosing
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x89ac76f7e37ea6234f5dbfcbfb89653251f7b4f0d4c64615c4e7dec170b92fd1); /* function */ 

coverage_0xf83122b5(0xb0b00b36a0a22d8602cbebf4f3c98f24ad2ea9e7eba32c5439f8b42d956b2f0e); /* line */ 
        coverage_0xf83122b5(0x8f734b930b4cb61e0bba47686208a021d9978d53e2a07a5c379f82b8a5b32215); /* statement */ 
AdminImpl.ownerSetIsClosing(
            g_state,
            marketId,
            isClosing
        );
    }

    /**
     * Set the price oracle for a market.
     */
    function ownerSetPriceOracle(
        uint256 marketId,
        IPriceOracle priceOracle
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x814dfa395b49fde9792cea20b7993019a7d2e9ebae06dd555cbeeb8d759ee55f); /* function */ 

coverage_0xf83122b5(0x2791b0e232b99c2be9378e95079a65ec4f259e6ce038369a155508891006b4db); /* line */ 
        coverage_0xf83122b5(0xd4dfc7ead6ec96be266b7b89827a05bfb7278b928b23d18b2e9795ee04babac6); /* statement */ 
AdminImpl.ownerSetPriceOracle(
            g_state,
            marketId,
            priceOracle
        );
    }

    /**
     * Set the interest-setter for a market.
     */
    function ownerSetInterestSetter(
        uint256 marketId,
        IInterestSetter interestSetter
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x7ce386231adb8eadfda3f8a979ef2c6b30e0b6cc83e21666f81bb2db31e21622); /* function */ 

coverage_0xf83122b5(0x2eb2da28d9141553779b5abf1e8d61c61ba01019929189120ed8f3a6e94a51b8); /* line */ 
        coverage_0xf83122b5(0x4a108edb7856c2dfe61466cf3d363d191c53f79eb70b90a3e94ccab96f3620d8); /* statement */ 
AdminImpl.ownerSetInterestSetter(
            g_state,
            marketId,
            interestSetter
        );
    }

    /**
     * Set a premium on the minimum margin-ratio for a market. This makes it so that any positions
     * that include this market require a higher collateralization to avoid being liquidated.
     */
    function ownerSetMarginPremium(
        uint256 marketId,
        Decimal.D256 memory marginPremium
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x2403c3c087f57da64f4915345a8247071402ade10a4e5dbe37912743cc1dcdf1); /* function */ 

coverage_0xf83122b5(0xee5bef009139609f179cae97b9a3383caaeaf4c512daa3df566745785645e83f); /* line */ 
        coverage_0xf83122b5(0xbe9c7130a747d7f3acb40df4c14b7206dfecc1ca6419871e59831558f6a090db); /* statement */ 
AdminImpl.ownerSetMarginPremium(
            g_state,
            marketId,
            marginPremium
        );
    }

    /**
     * Set a premium on the liquidation spread for a market. This makes it so that any liquidations
     * that include this market have a higher spread than the global default.
     */
    function ownerSetSpreadPremium(
        uint256 marketId,
        Decimal.D256 memory spreadPremium
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x3fc31717591087a14576108c50a02252cc63e4b56b8365716f3f92870d5fbb6c); /* function */ 

coverage_0xf83122b5(0x06c438fb21cdb0e29df7dbff9e987f9be52b5fc199cf4e33b04ca1202f0477d2); /* line */ 
        coverage_0xf83122b5(0x6281fe091a16b8d49f23d59779c6b928c532876dd733085d81141d0188b6ce4b); /* statement */ 
AdminImpl.ownerSetSpreadPremium(
            g_state,
            marketId,
            spreadPremium
        );
    }

    // ============ Risk Functions ============

    /**
     * Set the global minimum margin-ratio that every position must maintain to prevent being
     * liquidated.
     */
    function ownerSetMarginRatio(
        Decimal.D256 memory ratio
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x0ade38c05c2504486a50fb41a95492c277030a3d9c03b9abf07d5c1724cdacd7); /* function */ 

coverage_0xf83122b5(0x370148848a1773c87f79013478dc6cb1f371873993d46cc1f589a1c269912367); /* line */ 
        coverage_0xf83122b5(0x85bde57e43e9167b8e9ff0eb98663a6dfdd87e9714f3424c3df8469b5b592b05); /* statement */ 
AdminImpl.ownerSetMarginRatio(
            g_state,
            ratio
        );
    }

    /**
     * Set the global liquidation spread. This is the spread between oracle prices that incentivizes
     * the liquidation of risky positions.
     */
    function ownerSetLiquidationSpread(
        Decimal.D256 memory spread
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x66c326fe1acad8315bef257cbfdfba64c7ae7001e43e49b7ac38e46f42f9df97); /* function */ 

coverage_0xf83122b5(0xe68dbf5f8855634237c66fd213f338f4772fe4d161d5ab74494c7ba545b0c3d3); /* line */ 
        coverage_0xf83122b5(0xa7d59dc95cd0fecbc251e7bfbdf4425b24abec4e4ea55b156bf7dc73e1a33697); /* statement */ 
AdminImpl.ownerSetLiquidationSpread(
            g_state,
            spread
        );
    }

    /**
     * Set the global earnings-rate variable that determines what percentage of the interest paid
     * by borrowers gets passed-on to suppliers.
     */
    function ownerSetEarningsRate(
        Decimal.D256 memory earningsRate
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x7fb16d065f95e2b11a889a4e8e345119a8f6bd4b880b732cdcc2d14acee386cb); /* function */ 

coverage_0xf83122b5(0xbd67563caac6eedf637ec645d396b671cdc65ea3e76f5cdce57255cac40f1adc); /* line */ 
        coverage_0xf83122b5(0xd4fb5bc9f9beaea192064775fd3336cce63ef186bd74da240184c5b2d98d628e); /* statement */ 
AdminImpl.ownerSetEarningsRate(
            g_state,
            earningsRate
        );
    }

    /**
     * Set the global minimum-borrow value which is the minimum value of any new borrow on Solo.
     */
    function ownerSetMinBorrowedValue(
        Monetary.Value memory minBorrowedValue
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0x27ab2538d14423f86892071ba70b255e4c1cca9b0edad63fc9489de5119505cf); /* function */ 

coverage_0xf83122b5(0x99ed4309bc6f65dcc482aeafa028a48b4747cc24968c98bba21a4259044f74f7); /* line */ 
        coverage_0xf83122b5(0xdb2104dea16b825ef9afbc8e5f7c6faea8d5d27cb4cf14c5bcd0a854e50097c5); /* statement */ 
AdminImpl.ownerSetMinBorrowedValue(
            g_state,
            minBorrowedValue
        );
    }

    // ============ Global Operator Functions ============

    /**
     * Approve (or disapprove) an address that is permissioned to be an operator for all accounts in
     * Solo. Intended only to approve smart-contracts.
     */
    function ownerSetGlobalOperator(
        address operator,
        bool approved
    )
        public
        onlyOwner
        nonReentrant
    {coverage_0xf83122b5(0xd140bb9a86527f5d03ffcce3865c972850e4182e1dd0a5a2d363adf1e90ba2e9); /* function */ 

coverage_0xf83122b5(0x80c0801027c4a60a9c0e95091ef178b6fa774870d85d5ac1ef5d0f85fe6ae1b5); /* line */ 
        coverage_0xf83122b5(0xc9dbb675e5e49afd0e31bd36dab16c763579f85cc2e308cb0b9f67fa61782392); /* statement */ 
AdminImpl.ownerSetGlobalOperator(
            g_state,
            operator,
            approved
        );
    }
}
