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

import { State } from "./State.sol";


/**
 * @title Permission
 * @author dYdX
 *
 * Public function that allows other addresses to manage accounts
 */
contract Permission is
    State
{
function coverage_0x215a3c7e(bytes32 c__0x215a3c7e) public pure {}

    // ============ Events ============

    event LogOperatorSet(
        address indexed owner,
        address operator,
        bool trusted
    );

    // ============ Structs ============

    struct OperatorArg {
        address operator;
        bool trusted;
    }

    // ============ Public Functions ============

    /**
     * Approves/disapproves any number of operators. An operator is an external address that has the
     * same permissions to manipulate an account as the owner of the account. Operators are simply
     * addresses and therefore may either be externally-owned Ethereum accounts OR smart contracts.
     *
     * Operators are also able to act as AutoTrader contracts on behalf of the account owner if the
     * operator is a smart contract and implements the IAutoTrader interface.
     *
     * @param  args  A list of OperatorArgs which have an address and a boolean. The boolean value
     *               denotes whether to approve (true) or revoke approval (false) for that address.
     */
    function setOperators(
        OperatorArg[] memory args
    )
        public
    {coverage_0x215a3c7e(0x6c36502ec60ea9a8c3db30d242fc4bad41f9f39b91c7dc3a240bffa71bce6320); /* function */ 

coverage_0x215a3c7e(0x6154fdd02ce72c3f95cb049fd50f753f2a9237931d60153154cf5b2fc44bd510); /* line */ 
        coverage_0x215a3c7e(0x9862109abf7b5577a1a4e9944baec4de5d93df5d0b6905d9eaf01992725ca426); /* statement */ 
for (uint256 i = 0; i < args.length; i++) {
coverage_0x215a3c7e(0xfdf8e22c404775a0ae8b5151598e86a5bf73241a8f509aeae5adc6006bec3509); /* line */ 
            coverage_0x215a3c7e(0xc214f4b7c94574512f19410ea4c39819f88708c0d7c1dcb3d649eefffda54fab); /* statement */ 
address operator = args[i].operator;
coverage_0x215a3c7e(0x14667eb6d8879e0a0a7436c1b1d6ab38afd53fc298bc77d574c531cd45ccd8f6); /* line */ 
            coverage_0x215a3c7e(0xe8d58ac09eed083eddb2601eedf6c792cd281484c974f70ff14491eb77a977c7); /* statement */ 
bool trusted = args[i].trusted;
coverage_0x215a3c7e(0xc01b7afa291adc47d7f986361c35ac9fe407a2aa6b80acb954fb11d0411a306c); /* line */ 
            coverage_0x215a3c7e(0xb97a50a4337044cbb41c8ccbe7172da812f1e0781c17ddc868b1dfe86c7ca2d3); /* statement */ 
g_state.operators[msg.sender][operator] = trusted;
coverage_0x215a3c7e(0x88b6e2f309d37543db4a486772fb33d40b0864c2160bf31f375853fdb3cc1664); /* line */ 
            coverage_0x215a3c7e(0xce8f1b137477d9dea32d8cc5293f82f5e1fee462afecdd47505345a77b019fd2); /* statement */ 
emit LogOperatorSet(msg.sender, operator, trusted);
        }
    }
}
