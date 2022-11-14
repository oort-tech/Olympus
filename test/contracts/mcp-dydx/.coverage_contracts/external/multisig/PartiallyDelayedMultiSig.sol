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

import { DelayedMultiSig } from "./DelayedMultiSig.sol";


/**
 * @title PartiallyDelayedMultiSig
 * @author dYdX
 *
 * Multi-Signature Wallet with delay in execution except for some function selectors.
 */
contract PartiallyDelayedMultiSig is
    DelayedMultiSig
{
function coverage_0x67cdc2b0(bytes32 c__0x67cdc2b0) public pure {}

    // ============ Events ============

    event SelectorSet(address destination, bytes4 selector, bool approved);

    // ============ Constants ============

    bytes4 constant internal BYTES_ZERO = bytes4(0x0);

    // ============ Storage ============

    // destination => function selector => can bypass timelock
    mapping (address => mapping (bytes4 => bool)) public instantData;

    // ============ Modifiers ============

    // Overrides old modifier that requires a timelock for every transaction
    modifier pastTimeLock(
        uint256 transactionId
    ) {coverage_0x67cdc2b0(0x2ea5c366de39d1872e67253edb8d9bbd3f850e2dc6356f8abb990cb6c7234c32); /* function */ 

        // if the function selector is not exempt from timelock, then require timelock
coverage_0x67cdc2b0(0xa3aa6c38d18c17165ac2e46405c7146839a65f19a31bd2bea36dcbb846ca4954); /* line */ 
        coverage_0x67cdc2b0(0x14b2e8a5135d71793ef0f9b4a03fe9ab55fd15a5d87668f34e06f4fa1dabb12f); /* assertPre */ 
coverage_0x67cdc2b0(0x4b076204e29d59056685fabb3fffa67bb0fbe3b405cfd35f1cb076baa1efcf40); /* statement */ 
require(
            block.timestamp >= confirmationTimes[transactionId] + secondsTimeLocked
            || txCanBeExecutedInstantly(transactionId),
            "TIME_LOCK_INCOMPLETE"
        );coverage_0x67cdc2b0(0x759578108000bcdb588193ceec3a5414c086d2e8962fdd4f4f91fcc50cc90a12); /* assertPost */ 

coverage_0x67cdc2b0(0x74e70564d58efc5741fc7d4b5e101a694d120979f6dd7542f3f9803ec053174b); /* line */ 
        _;
    }

    // ============ Constructor ============

    /**
     * Contract constructor sets initial owners, required number of confirmations, and time lock.
     *
     * @param  _owners               List of initial owners.
     * @param  _required             Number of required confirmations.
     * @param  _secondsTimeLocked    Duration needed after a transaction is confirmed and before it
     *                               becomes executable, in seconds.
     * @param  _noDelayDestinations  List of destinations that correspond with the selectors.
     *                               Zero address allows the function selector to be used with any
     *                               address.
     * @param  _noDelaySelectors     All function selectors that do not require a delay to execute.
     *                               Fallback function is 0x00000000.
     */
    constructor (
        address[] memory _owners,
        uint256 _required,
        uint32 _secondsTimeLocked,
        address[] memory _noDelayDestinations,
        bytes4[] memory _noDelaySelectors
    )
        public
        DelayedMultiSig(_owners, _required, _secondsTimeLocked)
    {coverage_0x67cdc2b0(0xa2060ff46620e69bcc7ccaf7ba08bf120313d80561b4959a55311e85c9588ac8); /* function */ 

coverage_0x67cdc2b0(0x4d26a11831b8d6936e4989145d847dbc9f0f9a2e708c511ff69c5c20c4f91480); /* line */ 
        coverage_0x67cdc2b0(0x9a8162c3236e97daeee07ab7f46b1e3e742d74df46c69b808ff877726c2d5649); /* assertPre */ 
coverage_0x67cdc2b0(0x7286d9fd5bbf1728f0b8fff17079858f625792f70e536ea8a06560171feb67e2); /* statement */ 
require(
            _noDelayDestinations.length == _noDelaySelectors.length,
            "ADDRESS_AND_SELECTOR_MISMATCH"
        );coverage_0x67cdc2b0(0x510385a2e62efa934b9c6bf115c4cb264efb8cadf24ab9720b404461510a9707); /* assertPost */ 


coverage_0x67cdc2b0(0x5364c61aeb527716fa4f95350fa32e3648f7740b1f1542cdac8eecf4bd46a73d); /* line */ 
        coverage_0x67cdc2b0(0x39cf2d9c2620c1f78b05a6a220e3a804d6210d96dc67499c8242ab99f241779c); /* statement */ 
for (uint256 i = 0; i < _noDelaySelectors.length; i++) {
coverage_0x67cdc2b0(0x568e0d417d73e080829b6bf3356c5144d5b95a74c6b4391c2a477013e55161e2); /* line */ 
            coverage_0x67cdc2b0(0x148828df89f06fd31147fdbd7550c7d4c8dc50b586b0c9a0c1e092d19775bffc); /* statement */ 
address destination = _noDelayDestinations[i];
coverage_0x67cdc2b0(0x8aed6c25a497e06d19c80707819c3f94f6fd8ef9ed41b23d49abac7ceca9e934); /* line */ 
            coverage_0x67cdc2b0(0xa35e71ea904c7eac25c5a0557741a2cb1d69675b4fe48aeefa25858c8b8199c3); /* statement */ 
bytes4 selector = _noDelaySelectors[i];
coverage_0x67cdc2b0(0x80e96d333c919b040ca9feb051578c34944bbcd2d7cd45e03eb395cd72f8e84c); /* line */ 
            coverage_0x67cdc2b0(0xa32b98e1e4eb3806c0334438c8e4be5238351ed9f33f3ea8a828429956542a7b); /* statement */ 
instantData[destination][selector] = true;
coverage_0x67cdc2b0(0x034cb0841dfce0f97b29a77d7bcc3ed720fd9330cf885e7d7a0e93f4fc52a2bb); /* line */ 
            coverage_0x67cdc2b0(0x59344f0c4cc8113460bb819430cc7e3a5a28afd580d9fdeb2919ef05f93a18e9); /* statement */ 
emit SelectorSet(destination, selector, true);
        }
    }

    // ============ Wallet-Only Functions ============

    /**
     * Adds or removes functions that can be executed instantly. Transaction must be sent by wallet.
     *
     * @param  destination  Destination address of function. Zero address allows the function to be
     *                      sent to any address.
     * @param  selector     4-byte selector of the function. Fallback function is 0x00000000.
     * @param  approved     True if adding approval, false if removing approval.
     */
    function setSelector(
        address destination,
        bytes4 selector,
        bool approved
    )
        public
        onlyWallet
    {coverage_0x67cdc2b0(0xc3b3e492fa01bb961b5af915821cfc430a8fb2302a03a23cb4ca5260dd2d84a8); /* function */ 

coverage_0x67cdc2b0(0x55e016ea0a5e14321c0812f8a3643465c27ba1f0b23c5a24742bf28d3670536d); /* line */ 
        coverage_0x67cdc2b0(0x0203ce6b8c9dff359d55a9dd76da35dd87787297355ec6d79c514552bcb9fe33); /* statement */ 
instantData[destination][selector] = approved;
coverage_0x67cdc2b0(0xad619dedb30ac4f001de1e22239f03fb327150ebdd4d0c9fc66ac5f3d2a5c3e0); /* line */ 
        coverage_0x67cdc2b0(0x307a5bcf5d1ac32530d04811cbfa45bc7cfacc472e3b7b3c6fd9bfdc07db747d); /* statement */ 
emit SelectorSet(destination, selector, approved);
    }

    // ============ Helper Functions ============

    /**
     * Returns true if transaction can be executed instantly (without timelock).
     */
    function txCanBeExecutedInstantly(
        uint256 transactionId
    )
        internal
        view
        returns (bool)
    {coverage_0x67cdc2b0(0xdeed7b3ba337a3944df83f4097de61d1168e2bb1d77bb445bc83bc8fc8d9c8f0); /* function */ 

        // get transaction from storage
coverage_0x67cdc2b0(0x7dcfcccbc28466db190f9be898dd51ac48082a50065de31769ad6a1d0d9e25b5); /* line */ 
        coverage_0x67cdc2b0(0xd43eb61e8f9e47283476a9a1d512675a5829f5de55241514d1f0520cf1777b8b); /* statement */ 
Transaction memory txn = transactions[transactionId];
coverage_0x67cdc2b0(0x6eb65ab4f079df22956427e69876ea38ae29cbbf311c6fe9bf7dee8fc604e7b2); /* line */ 
        coverage_0x67cdc2b0(0x85dedb921259eb180a2450b0f1a45271b35180b5151b5a83d588b062871dcf0d); /* statement */ 
address dest = txn.destination;
coverage_0x67cdc2b0(0xdd2e0b76360deefd6b4c40f53aa4cb0106f79ea1b284047be506a8c4f9cb24c0); /* line */ 
        coverage_0x67cdc2b0(0xf964fe635a313aa649528a8b5ff884f7d2d2eae2c5016b3a64ef9f4dd90dde97); /* statement */ 
bytes memory data = txn.data;

        // fallback function
coverage_0x67cdc2b0(0x61885702fc4b6b3e122dbac1640814c0596bb355540b240b9b61b5d604d9264e); /* line */ 
        coverage_0x67cdc2b0(0x02f89211ad58a7c664e21e8a2135a9feaa92fc9365ce3d01b0668eba730d639a); /* statement */ 
if (data.length == 0) {coverage_0x67cdc2b0(0x96aeaaa4be6d90c7c7f7bf34ab9c28d24544780fabfae870001760f877e8b617); /* branch */ 

coverage_0x67cdc2b0(0x1082cbd4b59976ae286d060779f03ef3ff05fe3ee0e55b73bfe8b73b8c69c5f0); /* line */ 
            coverage_0x67cdc2b0(0xd6be3b2455d892c3e3eca9a06c77418c84523bb846cc15b7232582c8ecd1771d); /* statement */ 
return selectorCanBeExecutedInstantly(dest, BYTES_ZERO);
        }else { coverage_0x67cdc2b0(0xc0d6946620dc783c1905db393c415ffd73449079ec0107e929684288b6558489); /* branch */ 
}

        // invalid function selector
coverage_0x67cdc2b0(0x96b00c809b39e24806818267bb9b8dd81bd67515939cbe63646ceb2a7ec2490a); /* line */ 
        coverage_0x67cdc2b0(0x8532ab437a5e1fac93accddc7c629b107df4ba36d9e255b25139549d362220e3); /* statement */ 
if (data.length < 4) {coverage_0x67cdc2b0(0x13a07386491d35aba2e5aae3213d3b7bb68566307408dd4b21fa1bb1b6c92202); /* branch */ 

coverage_0x67cdc2b0(0x58cd3ab9dad41169e8e8e5b4282175a1af234e4ed36021ac44d7e600b5a897d3); /* line */ 
            coverage_0x67cdc2b0(0x1be777684ce93670590dc9966190f2be34c4894bcecd58d4725288d302e4c3d2); /* statement */ 
return false;
        }else { coverage_0x67cdc2b0(0x29b7269998f890e8f16f2e1f33ff6f75138d0ce94ac0823f45f54615f0db1ac9); /* branch */ 
}

        // check first four bytes (function selector)
coverage_0x67cdc2b0(0x622c7a732623f5e0432e7180761692c9cd809d337f6be5bd336250175a175433); /* line */ 
        coverage_0x67cdc2b0(0x01df36bd901fe8602aa9eea867f4868210476ed0963e33031266ac083562b53a); /* statement */ 
bytes32 rawData;
        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x67cdc2b0(0x517b26f2c6f0554887fe93b61210d6a37db87cd5a71af1fdfe0e411397e67952); /* line */ 
        assembly {
            rawData := mload(add(data, 32))
        }
coverage_0x67cdc2b0(0x4bbafbb64a8726eb644eeb9d31e6d76f24b00cfc1b84f32bdbc23133bd4a6ab7); /* line */ 
        coverage_0x67cdc2b0(0xbd0a7fb3771461a50dc1843b5fbfbef443cf4f1e5f1e809197beccf9153e96a2); /* statement */ 
bytes4 selector = bytes4(rawData);

coverage_0x67cdc2b0(0xcbc076b5abe77c53183430672ea57d9bc78b528e81c93483719e21c0975aab1b); /* line */ 
        coverage_0x67cdc2b0(0x7c892977e95106a0e3e249b5da23ec15b95e1885e321b2a3f33b21c1342f56f4); /* statement */ 
return selectorCanBeExecutedInstantly(dest, selector);
    }

    /**
     * Function selector is in instantData for address dest (or for address zero).
     */
    function selectorCanBeExecutedInstantly(
        address destination,
        bytes4 selector
    )
        internal
        view
        returns (bool)
    {coverage_0x67cdc2b0(0xebfdfa07e308bcee3f42e33c4f4f1724fed900efc2ac79e6550a5f465cc0e144); /* function */ 

coverage_0x67cdc2b0(0x5f20812d70e00c24ca29e9d6ddaf69a78582f076ca872bb61262eb29e4ca4672); /* line */ 
        coverage_0x67cdc2b0(0x493738eb5adbb4a309427c0cb65d4ef866d719941020052586561e5e6251ff49); /* statement */ 
return instantData[destination][selector]
            || instantData[ADDRESS_ZERO][selector];
    }
}
