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

import { Account } from "./Account.sol";
import { Actions } from "./Actions.sol";
import { Interest } from "./Interest.sol";
import { Storage } from "./Storage.sol";
import { Types } from "./Types.sol";


/**
 * @title Events
 * @author dYdX
 *
 * Library to parse and emit logs from which the state of all accounts and indexes can be followed
 */
library Events {
function coverage_0xfeb72d7d(bytes32 c__0xfeb72d7d) public pure {}

    using Types for Types.Wei;
    using Storage for Storage.State;

    // ============ Events ============

    event LogIndexUpdate(
        uint256 indexed market,
        Interest.Index index
    );

    event LogOperation(
        address sender
    );

    event LogDeposit(
        address indexed accountOwner,
        uint256 accountNumber,
        uint256 market,
        BalanceUpdate update,
        address from
    );

    event LogWithdraw(
        address indexed accountOwner,
        uint256 accountNumber,
        uint256 market,
        BalanceUpdate update,
        address to
    );

    event LogTransfer(
        address indexed accountOneOwner,
        uint256 accountOneNumber,
        address indexed accountTwoOwner,
        uint256 accountTwoNumber,
        uint256 market,
        BalanceUpdate updateOne,
        BalanceUpdate updateTwo
    );

    event LogBuy(
        address indexed accountOwner,
        uint256 accountNumber,
        uint256 takerMarket,
        uint256 makerMarket,
        BalanceUpdate takerUpdate,
        BalanceUpdate makerUpdate,
        address exchangeWrapper
    );

    event LogSell(
        address indexed accountOwner,
        uint256 accountNumber,
        uint256 takerMarket,
        uint256 makerMarket,
        BalanceUpdate takerUpdate,
        BalanceUpdate makerUpdate,
        address exchangeWrapper
    );

    event LogTrade(
        address indexed takerAccountOwner,
        uint256 takerAccountNumber,
        address indexed makerAccountOwner,
        uint256 makerAccountNumber,
        uint256 inputMarket,
        uint256 outputMarket,
        BalanceUpdate takerInputUpdate,
        BalanceUpdate takerOutputUpdate,
        BalanceUpdate makerInputUpdate,
        BalanceUpdate makerOutputUpdate,
        address autoTrader
    );

    event LogCall(
        address indexed accountOwner,
        uint256 accountNumber,
        address callee
    );

    event LogLiquidate(
        address indexed solidAccountOwner,
        uint256 solidAccountNumber,
        address indexed liquidAccountOwner,
        uint256 liquidAccountNumber,
        uint256 heldMarket,
        uint256 owedMarket,
        BalanceUpdate solidHeldUpdate,
        BalanceUpdate solidOwedUpdate,
        BalanceUpdate liquidHeldUpdate,
        BalanceUpdate liquidOwedUpdate
    );

    event LogVaporize(
        address indexed solidAccountOwner,
        uint256 solidAccountNumber,
        address indexed vaporAccountOwner,
        uint256 vaporAccountNumber,
        uint256 heldMarket,
        uint256 owedMarket,
        BalanceUpdate solidHeldUpdate,
        BalanceUpdate solidOwedUpdate,
        BalanceUpdate vaporOwedUpdate
    );

    // ============ Structs ============

    struct BalanceUpdate {
        Types.Wei deltaWei;
        Types.Par newPar;
    }

    // ============ Internal Functions ============

    function logIndexUpdate(
        uint256 marketId,
        Interest.Index memory index
    )
        internal
    {coverage_0xfeb72d7d(0xb37d3f66a79ba84c8b1171ab6a4d2c0b7770a673bdfab008898a8554a45d9a3d); /* function */ 

coverage_0xfeb72d7d(0xd93e57a781bda8d6729a3fb59f57b54d21ba85cb2bbe3f3f9bf0aa44b65106f9); /* line */ 
        coverage_0xfeb72d7d(0x2bde25292222f09aadb2780d256864e0f7d8ae7f38ca314c16e5fd24f12a8d3d); /* statement */ 
emit LogIndexUpdate(
            marketId,
            index
        );
    }

    function logOperation()
        internal
    {coverage_0xfeb72d7d(0x8cd45b53fe36adeb3cd366f50f4973f57979045cfb394acd57640ccbadc9bc21); /* function */ 

coverage_0xfeb72d7d(0x8186aed5f2d6630853021c9310450b627733fcebd650a5627f9ccde4b2a9fc33); /* line */ 
        coverage_0xfeb72d7d(0xb743d0532f3c97d2b1427ee9044d096dc1f16aca56942aa4adbe3b3860f4425b); /* statement */ 
emit LogOperation(msg.sender);
    }

    function logDeposit(
        Storage.State storage state,
        Actions.DepositArgs memory args,
        Types.Wei memory deltaWei
    )
        internal
    {coverage_0xfeb72d7d(0x0204dde4a5b9b7338b2bb3a360436d8bf620cbe26b47b2ce6fe6496c4a9bc697); /* function */ 

coverage_0xfeb72d7d(0xcecfccd5e65467f0b3b6053fb83f3f8f8ca8f212f177fa117381c752dca35604); /* line */ 
        coverage_0xfeb72d7d(0xf10e13b0aee856e6cceb375cd450395d59f8c14248e2a80b1607b4b8fd0b165e); /* statement */ 
emit LogDeposit(
            args.account.owner,
            args.account.number,
            args.market,
            getBalanceUpdate(
                state,
                args.account,
                args.market,
                deltaWei
            ),
            args.from
        );
    }

    function logWithdraw(
        Storage.State storage state,
        Actions.WithdrawArgs memory args,
        Types.Wei memory deltaWei
    )
        internal
    {coverage_0xfeb72d7d(0x4baabeb6161364d6240bfe1f32f86a9dd7bba3f160e2d9c285ed6660abfded0b); /* function */ 

coverage_0xfeb72d7d(0x620a029c8f325e6c0f81c7487c15da79003c6d0e1467df4f640ba59f3f7baef6); /* line */ 
        coverage_0xfeb72d7d(0x161add7134dc80c44fbfff75e642b6b92ac9be7c02424db8171dce03a4b354ef); /* statement */ 
emit LogWithdraw(
            args.account.owner,
            args.account.number,
            args.market,
            getBalanceUpdate(
                state,
                args.account,
                args.market,
                deltaWei
            ),
            args.to
        );
    }

    function logTransfer(
        Storage.State storage state,
        Actions.TransferArgs memory args,
        Types.Wei memory deltaWei
    )
        internal
    {coverage_0xfeb72d7d(0x273d8a66f831b9587436048da259a177c08021b51b468df94f034671e5657cb9); /* function */ 

coverage_0xfeb72d7d(0x2cd6d26cd936638fc6f7db043c2115363d304d90c74ecbfa1dfde2ed842c0953); /* line */ 
        coverage_0xfeb72d7d(0x23263f398398fcd8be6ac4f7019bfebeedbcb11afbf5330aee261cd29fd6701b); /* statement */ 
emit LogTransfer(
            args.accountOne.owner,
            args.accountOne.number,
            args.accountTwo.owner,
            args.accountTwo.number,
            args.market,
            getBalanceUpdate(
                state,
                args.accountOne,
                args.market,
                deltaWei
            ),
            getBalanceUpdate(
                state,
                args.accountTwo,
                args.market,
                deltaWei.negative()
            )
        );
    }

    function logBuy(
        Storage.State storage state,
        Actions.BuyArgs memory args,
        Types.Wei memory takerWei,
        Types.Wei memory makerWei
    )
        internal
    {coverage_0xfeb72d7d(0xf176006dea8d677702b8ec5bcac01f7b2117146ebdcf1389b3d9c4186f4ac4bf); /* function */ 

coverage_0xfeb72d7d(0x4c6da447c8b15c9b646695606570888eda20c1169bca447b7ac2d215522baa12); /* line */ 
        coverage_0xfeb72d7d(0x4bf5aff8dc975ec6a46018dd3bef781d37627e418d6530ab23918f81a90c75fb); /* statement */ 
emit LogBuy(
            args.account.owner,
            args.account.number,
            args.takerMarket,
            args.makerMarket,
            getBalanceUpdate(
                state,
                args.account,
                args.takerMarket,
                takerWei
            ),
            getBalanceUpdate(
                state,
                args.account,
                args.makerMarket,
                makerWei
            ),
            args.exchangeWrapper
        );
    }

    function logSell(
        Storage.State storage state,
        Actions.SellArgs memory args,
        Types.Wei memory takerWei,
        Types.Wei memory makerWei
    )
        internal
    {coverage_0xfeb72d7d(0x149fced3ab68d26d71c7c432c7eaa8e64ee7f5d0f9766302bb7a6234bfe8e2f3); /* function */ 

coverage_0xfeb72d7d(0x51394416c088b2777527930c91575516c2cf089118e1387e4b06695aad6a051d); /* line */ 
        coverage_0xfeb72d7d(0x429d9fa19e0c06babd7ed7f83dbedacb42829592ae7e6489d448b6187d550f98); /* statement */ 
emit LogSell(
            args.account.owner,
            args.account.number,
            args.takerMarket,
            args.makerMarket,
            getBalanceUpdate(
                state,
                args.account,
                args.takerMarket,
                takerWei
            ),
            getBalanceUpdate(
                state,
                args.account,
                args.makerMarket,
                makerWei
            ),
            args.exchangeWrapper
        );
    }

    function logTrade(
        Storage.State storage state,
        Actions.TradeArgs memory args,
        Types.Wei memory inputWei,
        Types.Wei memory outputWei
    )
        internal
    {coverage_0xfeb72d7d(0x091fd0b84027b09e19c845101917a50f9639537285546438c1630ceff34b1f1f); /* function */ 

coverage_0xfeb72d7d(0x1d2eea4cf069aff3e2d69dbc56465f08df71efba85208e06c78373536fe0d6c5); /* line */ 
        coverage_0xfeb72d7d(0x331273ff1a769f620d321737834d69e5f6bf7870d55c20289f67692a903b3926); /* statement */ 
BalanceUpdate[4] memory updates = [
            getBalanceUpdate(
                state,
                args.takerAccount,
                args.inputMarket,
                inputWei.negative()
            ),
            getBalanceUpdate(
                state,
                args.takerAccount,
                args.outputMarket,
                outputWei.negative()
            ),
            getBalanceUpdate(
                state,
                args.makerAccount,
                args.inputMarket,
                inputWei
            ),
            getBalanceUpdate(
                state,
                args.makerAccount,
                args.outputMarket,
                outputWei
            )
        ];

coverage_0xfeb72d7d(0xd54a5235cbc747073be73159a2fe75432081a827c76694538ef1fbed0221a949); /* line */ 
        coverage_0xfeb72d7d(0x87acfc5db689c55176f5c6772abaaf0d31030d855d9d6f18c7e32e3d3f8a1a01); /* statement */ 
emit LogTrade(
            args.takerAccount.owner,
            args.takerAccount.number,
            args.makerAccount.owner,
            args.makerAccount.number,
            args.inputMarket,
            args.outputMarket,
            updates[0],
            updates[1],
            updates[2],
            updates[3],
            args.autoTrader
        );
    }

    function logCall(
        Actions.CallArgs memory args
    )
        internal
    {coverage_0xfeb72d7d(0x9add3e4a3c33fc80aed478d6a83e6bbba4e00b5ad591f7ba018d8451209bd2b5); /* function */ 

coverage_0xfeb72d7d(0x1692651aba62799c02a6c81ce67bf9753c75fb4fbe694b61217eb9b9b9beaf99); /* line */ 
        coverage_0xfeb72d7d(0x204c7b0e477fcf7841e208ee69f9ec20c60d1cdb63f0067f2617ed2e7fca547a); /* statement */ 
emit LogCall(
            args.account.owner,
            args.account.number,
            args.callee
        );
    }

    function logLiquidate(
        Storage.State storage state,
        Actions.LiquidateArgs memory args,
        Types.Wei memory heldWei,
        Types.Wei memory owedWei
    )
        internal
    {coverage_0xfeb72d7d(0xbff8a3a59ecd608fc2d08081eec5080e775248d4934f76aa6e007f0f82eddd19); /* function */ 

coverage_0xfeb72d7d(0xc440d0a911593035821670521ee7cdcef7680ae90ade958c417bc79c873eabd7); /* line */ 
        coverage_0xfeb72d7d(0x63fbfb8a5b2a3727e8aff66f9661a36639a6fff5ad702e8fe21df048365939d3); /* statement */ 
BalanceUpdate memory solidHeldUpdate = getBalanceUpdate(
            state,
            args.solidAccount,
            args.heldMarket,
            heldWei.negative()
        );
coverage_0xfeb72d7d(0xcf868199779d12e9c021a2e30aed5653f0b0875183bbe5cae079192306378e95); /* line */ 
        coverage_0xfeb72d7d(0xf961c0659d44024aa82d3ce85c17888d347ed20039d2da1ed97ccbd0ec0d4746); /* statement */ 
BalanceUpdate memory solidOwedUpdate = getBalanceUpdate(
            state,
            args.solidAccount,
            args.owedMarket,
            owedWei.negative()
        );
coverage_0xfeb72d7d(0x96571e5cf479b4fb22e682c0aab45c2f08f17e74d68efffea0a40ca40de432d7); /* line */ 
        coverage_0xfeb72d7d(0x4c23ee5c7e3b7a674289ef43f21b65acecbed7ddea1e7e73606290f10ff79529); /* statement */ 
BalanceUpdate memory liquidHeldUpdate = getBalanceUpdate(
            state,
            args.liquidAccount,
            args.heldMarket,
            heldWei
        );
coverage_0xfeb72d7d(0x3aba5ddb1f7e764c766055a52cc81668587832c4eea04356a1cf695c8ef160c6); /* line */ 
        coverage_0xfeb72d7d(0xa8a2bb493f2a3f5860bce7be0684a20245985a7349cd7924f8601dca74aa65b8); /* statement */ 
BalanceUpdate memory liquidOwedUpdate = getBalanceUpdate(
            state,
            args.liquidAccount,
            args.owedMarket,
            owedWei
        );

coverage_0xfeb72d7d(0x6e77aced3c014e64d478249f4b1e3a842541e38118749be912b3b2f572d02939); /* line */ 
        coverage_0xfeb72d7d(0x89e4f2604d475fe4fcfd688d1a2046f768a65d98fd95e5e757d345a4580e7174); /* statement */ 
emit LogLiquidate(
            args.solidAccount.owner,
            args.solidAccount.number,
            args.liquidAccount.owner,
            args.liquidAccount.number,
            args.heldMarket,
            args.owedMarket,
            solidHeldUpdate,
            solidOwedUpdate,
            liquidHeldUpdate,
            liquidOwedUpdate
        );
    }

    function logVaporize(
        Storage.State storage state,
        Actions.VaporizeArgs memory args,
        Types.Wei memory heldWei,
        Types.Wei memory owedWei,
        Types.Wei memory excessWei
    )
        internal
    {coverage_0xfeb72d7d(0x67b317a4964faeadb2d522c2b94d5fc8ca6a890c21dfd96335e05c1351d63782); /* function */ 

coverage_0xfeb72d7d(0x53c3ea6a2c2209475f53ed820b34fd8e79eee13b3983b566500c6599158eda41); /* line */ 
        coverage_0xfeb72d7d(0xd20967c06c85e64556e21df2f1ec7d3045e85bf3b7eb049df719737d85fee188); /* statement */ 
BalanceUpdate memory solidHeldUpdate = getBalanceUpdate(
            state,
            args.solidAccount,
            args.heldMarket,
            heldWei.negative()
        );
coverage_0xfeb72d7d(0x8ac06e2574af08ea4fa3c77e1e8e7d32c31ef73788178ff5d795c2e12b246f26); /* line */ 
        coverage_0xfeb72d7d(0x9a3ee70a903f6f3640cc0a530b47f4891cedaf88e4c1dc153b26752183970f90); /* statement */ 
BalanceUpdate memory solidOwedUpdate = getBalanceUpdate(
            state,
            args.solidAccount,
            args.owedMarket,
            owedWei.negative()
        );
coverage_0xfeb72d7d(0x13d74ea6ebe850b803ac00645005da600fe66e35baabd654f7f0f94535d7d8f3); /* line */ 
        coverage_0xfeb72d7d(0xec7e1e7e0339bc6733aeef3db8987c2ae04be8af1e49d9be7ed87b323bba6d41); /* statement */ 
BalanceUpdate memory vaporOwedUpdate = getBalanceUpdate(
            state,
            args.vaporAccount,
            args.owedMarket,
            owedWei.add(excessWei)
        );

coverage_0xfeb72d7d(0x51c3d1e2b25dba2b2aebee8d0f615d961917fe002369db32fcc443e2b6fd19d2); /* line */ 
        coverage_0xfeb72d7d(0x66666db364da6c04977cbb742d219c4e071ad909ff330e124d0f69c7d6b0a118); /* statement */ 
emit LogVaporize(
            args.solidAccount.owner,
            args.solidAccount.number,
            args.vaporAccount.owner,
            args.vaporAccount.number,
            args.heldMarket,
            args.owedMarket,
            solidHeldUpdate,
            solidOwedUpdate,
            vaporOwedUpdate
        );
    }

    // ============ Private Functions ============

    function getBalanceUpdate(
        Storage.State storage state,
        Account.Info memory account,
        uint256 market,
        Types.Wei memory deltaWei
    )
        private
        view
        returns (BalanceUpdate memory)
    {coverage_0xfeb72d7d(0xca036ec6fa639a379ae122784b3a2dc821635dfd99d66cb84b4aeb5989fa1401); /* function */ 

coverage_0xfeb72d7d(0x90af6e65ea6632b39c1717d8f4dd33c02c0f6e96f0cc744741dc0a784d3b7f92); /* line */ 
        coverage_0xfeb72d7d(0x513bd667c60b19e71fc8067cd744c5315660115b54c55e3469b37bd6ee36f851); /* statement */ 
return BalanceUpdate({
            deltaWei: deltaWei,
            newPar: state.getPar(account, market)
        });
    }
}
