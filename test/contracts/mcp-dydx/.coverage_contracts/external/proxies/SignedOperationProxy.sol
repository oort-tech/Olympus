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
import { Account } from "../../protocol/lib/Account.sol";
import { Actions } from "../../protocol/lib/Actions.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Types } from "../../protocol/lib/Types.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";
import { TypedSignature } from "../lib/TypedSignature.sol";


/**
 * @title SignedOperationProxy
 * @author dYdX
 *
 * Contract for sending operations on behalf of others
 */
contract SignedOperationProxy is
    OnlySolo,
    Ownable
{
function coverage_0x9d2f76df(bytes32 c__0x9d2f76df) public pure {}

    using SafeMath for uint256;

    // ============ Constants ============

    bytes32 constant private FILE = "SignedOperationProxy";

    // EIP191 header for EIP712 prefix
    bytes2 constant private EIP191_HEADER = 0x1901;

    // EIP712 Domain Name value
    string constant private EIP712_DOMAIN_NAME = "SignedOperationProxy";

    // EIP712 Domain Version value
    string constant private EIP712_DOMAIN_VERSION = "1.1";

    // EIP712 encodeType of EIP712Domain
    bytes constant private EIP712_DOMAIN_STRING = abi.encodePacked(
        "EIP712Domain(",
        "string name,",
        "string version,",
        "uint256 chainId,",
        "address verifyingContract",
        ")"
    );

    // EIP712 encodeType of Operation
    bytes constant private EIP712_OPERATION_STRING = abi.encodePacked(
        "Operation(",
        "Action[] actions,",
        "uint256 expiration,",
        "uint256 salt,",
        "address sender,",
        "address signer",
        ")"
    );

    // EIP712 encodeType of Action
    bytes constant private EIP712_ACTION_STRING = abi.encodePacked(
        "Action(",
        "uint8 actionType,",
        "address accountOwner,",
        "uint256 accountNumber,",
        "AssetAmount assetAmount,",
        "uint256 primaryMarketId,",
        "uint256 secondaryMarketId,",
        "address otherAddress,",
        "address otherAccountOwner,",
        "uint256 otherAccountNumber,",
        "bytes data",
        ")"
    );

    // EIP712 encodeType of AssetAmount
    bytes constant private EIP712_ASSET_AMOUNT_STRING = abi.encodePacked(
        "AssetAmount(",
        "bool sign,",
        "uint8 denomination,",
        "uint8 ref,",
        "uint256 value",
        ")"
    );

    // EIP712 typeHash of EIP712Domain
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_DOMAIN_SEPARATOR_SCHEMA_HASH = keccak256(abi.encodePacked(
        EIP712_DOMAIN_STRING
    ));

    // EIP712 typeHash of Operation
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_OPERATION_HASH = keccak256(abi.encodePacked(
        EIP712_OPERATION_STRING,
        EIP712_ACTION_STRING,
        EIP712_ASSET_AMOUNT_STRING
    ));

    // EIP712 typeHash of Action
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_ACTION_HASH = keccak256(abi.encodePacked(
        EIP712_ACTION_STRING,
        EIP712_ASSET_AMOUNT_STRING
    ));

    // EIP712 typeHash of AssetAmount
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_ASSET_AMOUNT_HASH = keccak256(abi.encodePacked(
        EIP712_ASSET_AMOUNT_STRING
    ));

    // ============ Structs ============

    struct OperationHeader {
        uint256 expiration;
        uint256 salt;
        address sender;
        address signer;
    }

    struct Authorization {
        uint256 numActions;
        OperationHeader header;
        bytes signature;
    }

    // ============ Events ============

    event ContractStatusSet(
        bool operational
    );

    event LogOperationExecuted(
        bytes32 indexed operationHash,
        address indexed signer,
        address indexed sender
    );

    event LogOperationCanceled(
        bytes32 indexed operationHash,
        address indexed canceler
    );

    // ============ Immutable Storage ============

    // Hash of the EIP712 Domain Separator data
    bytes32 public EIP712_DOMAIN_HASH;

    // ============ Mutable Storage ============

     // true if this contract can process operationss
    bool public g_isOperational;

    // operation hash => was executed (or canceled)
    mapping (bytes32 => bool) public g_invalidated;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        uint256 chainId
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x9d2f76df(0x2e8af101faf5ed8de35a5e3b5edc3c8066746132d85ff8dbbea7fbbfae99c1e2); /* function */ 

coverage_0x9d2f76df(0xe4e035f741b8346cb66dca7211a5b6cbcc69e80250f20979e11b61af441875d8); /* line */ 
        coverage_0x9d2f76df(0x6b023f71b2a09f8ce7fca294edd40620dc282aae4735f07cf01fee2e516a0b65); /* statement */ 
g_isOperational = true;

        /* solium-disable-next-line indentation */
coverage_0x9d2f76df(0xf2a4fb13f51041910ba6d948959b2e1fef3151c8fdc18ba30684c01ae28c8f44); /* line */ 
        coverage_0x9d2f76df(0xf21338e9d8fbff85115ef4d7e5b6b791891f044eaa15cd96aa8e9f5115c829cc); /* statement */ 
EIP712_DOMAIN_HASH = keccak256(abi.encode(
            EIP712_DOMAIN_SEPARATOR_SCHEMA_HASH,
            keccak256(bytes(EIP712_DOMAIN_NAME)),
            keccak256(bytes(EIP712_DOMAIN_VERSION)),
            chainId,
            address(this)
        ));
    }

    // ============ Admin Functions ============

     /**
     * The owner can shut down the exchange.
     */
    function shutDown()
        external
        onlyOwner
    {coverage_0x9d2f76df(0x16c659889d63311d3921b2833a620a6ea08b0e3faaa4c0dc154e1678eebfe5e1); /* function */ 

coverage_0x9d2f76df(0x28e6089b8a16ef75df93638e83411b1be4b112b06302497990083edcd6fbd60b); /* line */ 
        coverage_0x9d2f76df(0x24f02681749e2138de4248130aeacfb057c57a5bdf4706fc1021a4cbd18ccf41); /* statement */ 
g_isOperational = false;
coverage_0x9d2f76df(0x77982e8f8e2b69b20c34651290fd1c16d730b49d3b4887968d8698103d8a244d); /* line */ 
        coverage_0x9d2f76df(0x04991a119d4ad1a1b2b55891f5d856f309d59b3e9619a91be5ef303f7c8fb4a5); /* statement */ 
emit ContractStatusSet(false);
    }

     /**
     * The owner can start back up the exchange.
     */
    function startUp()
        external
        onlyOwner
    {coverage_0x9d2f76df(0x0436f0f322d586960a981ef0fd7a67cf097e55c0769903376aac01236e22602a); /* function */ 

coverage_0x9d2f76df(0xbb637e326f7ddb12caee19a5515054a4ca661e529d626eb481347c2046eb8a25); /* line */ 
        coverage_0x9d2f76df(0x6fc60710b1309615039bae6c44748116286b6a1a398b79bd6c9d3b893eb3ed59); /* statement */ 
g_isOperational = true;
coverage_0x9d2f76df(0xb29c943c9c57c6d69f6385a29eac2171453164e8846bc307e80246ff8a7337c2); /* line */ 
        coverage_0x9d2f76df(0x43f6eccf9ad89ab8d2215422658bc6d9f4d6bc7621856f3caa6bf9f603a7f33b); /* statement */ 
emit ContractStatusSet(true);
    }

    // ============ Public Functions ============

    /**
     * Allows a signer to permanently cancel an operation on-chain.
     *
     * @param  accounts  The accounts involved in the operation
     * @param  actions   The actions involved in the operation
     * @param  auth      The unsigned authorization of the operation
     */
    function cancel(
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions,
        Authorization memory auth
    )
        public
    {coverage_0x9d2f76df(0x34d077dfa782df142e03c7c74ab8435d289d6a8d11f63e6bfb0d3f636db760ff); /* function */ 

coverage_0x9d2f76df(0xfe81c1949f78fca6e8c81a6fa9edc5dd046487e98f8505ee79ad64a559e9b739); /* line */ 
        coverage_0x9d2f76df(0x9d66e9b271ab03012d2829c894a996c1283cf3197ebbfce3ad1089932edb7d24); /* statement */ 
bytes32 operationHash = getOperationHash(
            accounts,
            actions,
            auth,
            0
        );
coverage_0x9d2f76df(0x28be463cd9b31548c2a92663825ac9b7e183134c6b192a315eddae297d5e1ace); /* line */ 
        coverage_0x9d2f76df(0xde0448244539e03a05f96e380ede777c9deafdec906ab129016986cc8731f5c2); /* statement */ 
Require.that(
            auth.header.signer == msg.sender,
            FILE,
            "Canceler must be signer"
        );
coverage_0x9d2f76df(0x885a7554bf6f273c0de724a76e09f2aa702f019d6a84cf0a461082ee5a8031a0); /* line */ 
        coverage_0x9d2f76df(0x4cedaa31e16dc9421b3ac50d97ce5cf89a86f1c74240c5ebac892014f89a5f21); /* statement */ 
g_invalidated[operationHash] = true;
coverage_0x9d2f76df(0x953bff6c536ab0c3f8308eef42b2e06e6325f2f4630815ea60528e1ac8a0dcbc); /* line */ 
        coverage_0x9d2f76df(0x354c1c3a3fd5f7f398d517039209d69e5e28493900399844ceec92a7ed4b51db); /* statement */ 
emit LogOperationCanceled(operationHash, msg.sender);
    }

    /**
     * Submits an operation to SoloMargin. Actions for accounts that the msg.sender does not control
     * must be authorized by a signed message. Each authorization can apply to multiple actions at
     * once which must occur in-order next to each other. An empty authorization must be supplied
     * explicitly for each group of actions that do not require a signed message.
     *
     * @param  accounts  The accounts to forward to SoloMargin.operate()
     * @param  actions   The actions to forward to SoloMargin.operate()
     * @param  auths     The signed authorizations for each group of actions
     *                   (or unsigned if msg.sender is already authorized)
     */
    function operate(
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions,
        Authorization[] memory auths
    )
        public
    {coverage_0x9d2f76df(0x4ad8fde5ad5fa01ff7319793eff74a4ba2ab4b14d4cfd9ba52f3b969e5c11c93); /* function */ 

coverage_0x9d2f76df(0xb414d1c700f60e75954cf260025a905afebe1aa1ffa0ffac45fc229dd9032995); /* line */ 
        coverage_0x9d2f76df(0x800cb2b108a24f9ae02edaa32e709d797923c313c1ca6dfe1c14b07ea389f800); /* statement */ 
Require.that(
            g_isOperational,
            FILE,
            "Contract is not operational"
        );

        // cache the index of the first action for this auth
coverage_0x9d2f76df(0x3f971dd08a54ac1d959d87c84db3aaacbf29d60458d957bd0084b59aaced6768); /* line */ 
        coverage_0x9d2f76df(0xb9883f25ddf0032c61d196e241258eec346198a97f8c47fb8129061b0e9afa1e); /* statement */ 
uint256 actionStartIdx = 0;

        // loop over all auths
coverage_0x9d2f76df(0x85ae40fc94477f1112e31bc8e534677cc8e477adaea54df05cdd2b46d70d2335); /* line */ 
        coverage_0x9d2f76df(0x819366a56b1373d73c1397f902470077d6ec5e65e1e61db8ce74184dd4f5f853); /* statement */ 
for (uint256 authIdx = 0; authIdx < auths.length; authIdx++) {
coverage_0x9d2f76df(0xdd87f09d1a10417bf04946162a961bfebec363988ce48d84706f25c870a60b15); /* line */ 
            coverage_0x9d2f76df(0x3a92ef687f1faf90a2ec9a2183d16130ac09c8654c947d2879ff0ab43ed60648); /* statement */ 
Authorization memory auth = auths[authIdx];

            // require that the message is not expired
coverage_0x9d2f76df(0x86191fa252fe6cb10e9f176419542c15d1ed98b98df8c591b4cda17e4b2aeed2); /* line */ 
            coverage_0x9d2f76df(0x4882a551c0563fc45f940b7c5bc5770faad6fb426e6d14c363db82bb617fcadf); /* statement */ 
Require.that(
                auth.header.expiration == 0 || auth.header.expiration >= block.timestamp,
                FILE,
                "Signed operation is expired",
                authIdx
            );

            // require that the sender matches the authorization
coverage_0x9d2f76df(0xb472ba0f164bcfcb76fa280ad917bd4e85b44b45a2e2a69d73f6c58ae8332afb); /* line */ 
            coverage_0x9d2f76df(0xe3e695fb330d22d38ff17cf87c5b7f421ae89a2544217b3c557b5715994837c0); /* statement */ 
Require.that(
                auth.header.sender == address(0) || auth.header.sender == msg.sender,
                FILE,
                "Operation sender mismatch",
                authIdx
            );

            // consider the signer to be msg.sender unless there is a signature
coverage_0x9d2f76df(0x293a11e55b443dcc6dff5fb6f1616e2d6d4266e17a85d896602c88fad717cc14); /* line */ 
            coverage_0x9d2f76df(0x617d0a032253c7dab988ea609515981d018ecc0eec6b291eaef99ae1fc375af8); /* statement */ 
address signer = msg.sender;

            // if there is a signature, then validate it
coverage_0x9d2f76df(0x539eb89027a9463e31711907c62a806549567dc4d1297914acfa6b1bd7ff9539); /* line */ 
            coverage_0x9d2f76df(0x5b01f7468533516ab60d4878bfa4bc7512d342e7a5d136d00ecd35f6a1789a13); /* statement */ 
if (auth.signature.length != 0) {coverage_0x9d2f76df(0x985187ecd9eb6955ce3c22084fb7c1ce9f0a683beeaa1b0bb4d36598d392fabc); /* branch */ 

                // get the hash of the operation
coverage_0x9d2f76df(0xb9a7c3187cc56c9748ea5706fb21d3652566e4f6b21067ae2039d12f13f97c3c); /* line */ 
                coverage_0x9d2f76df(0x70fe821c4261a8dccb32e9f553f97883fe2abd193ecf31517a3a52bc2f1ab845); /* statement */ 
bytes32 operationHash = getOperationHash(
                    accounts,
                    actions,
                    auth,
                    actionStartIdx
                );

                // require that this message is still valid
coverage_0x9d2f76df(0x05ebe3de025cc0c5980b6874c643bf57cb6e8f32c1c6ef74b5c44cd75f9fe0a7); /* line */ 
                coverage_0x9d2f76df(0xd08d6eb8e5b26c5d92d8056c1218608b068237e209192e64b3bc7e2ca08182cb); /* statement */ 
Require.that(
                    !g_invalidated[operationHash],
                    FILE,
                    "Hash already used or canceled",
                    operationHash
                );

                // get the signer
coverage_0x9d2f76df(0xacd3ea67740dba1dd4486067d25931a3b53404f66075c3f7cae88115604bb90f); /* line */ 
                coverage_0x9d2f76df(0x0b8b2ea4563c64251aee0bced80c7c08c39e600cb23412f9b0ebd91ec274c228); /* statement */ 
signer = TypedSignature.recover(operationHash, auth.signature);

                // require that this signer matches the authorization
coverage_0x9d2f76df(0x5552c6df72d0732f5b57d65d0505e6e83e0f99d449957f4bc9206fc92402ef26); /* line */ 
                coverage_0x9d2f76df(0x66eeaecea9414c04b496343f2dbba10b23b4ab257e2511bffc2727258e16ae04); /* statement */ 
Require.that(
                    auth.header.signer == signer,
                    FILE,
                    "Invalid signature"
                );

                // consider this operationHash to be used (and therefore no longer valid)
coverage_0x9d2f76df(0x05de70173d8d3905370775fd919661989ebe42e1a405922ca6324147bdab4f06); /* line */ 
                coverage_0x9d2f76df(0x54e23c1e01f5e46640fe20913de1934263e212ab91034c0041ac5ebe37233d3f); /* statement */ 
g_invalidated[operationHash] = true;
coverage_0x9d2f76df(0x34fb72be9468a2c91643e7f07ca81c09804c62cc80a6466aac9b1923a75194d5); /* line */ 
                coverage_0x9d2f76df(0xe67b6eb2d1b1dc12be477bf2578e77ced63e4e0174a134a7041e8f6bc649d112); /* statement */ 
emit LogOperationExecuted(operationHash, signer, msg.sender);
            }else { coverage_0x9d2f76df(0xdee728c41dab864966f7579b30d6d2fd732c6e609467a881b91f8cf732805eda); /* branch */ 
}

            // cache the index of the first action after this auth
coverage_0x9d2f76df(0xcfd775579dc9a941f848a1777acde03a64241be3176d4750e78959a3485bb140); /* line */ 
            coverage_0x9d2f76df(0x288c0164c0e148498392a3aca46eaadb45660ffa1c8fda112d3362e539621833); /* statement */ 
uint256 actionEndIdx = actionStartIdx.add(auth.numActions);

            // loop over all actions for which this auth applies
coverage_0x9d2f76df(0x2e31fc4849f5b6aa0cc25b7291847e49df1dd597e1f7890f39243cf3af3cb3b0); /* line */ 
            coverage_0x9d2f76df(0xc6f05fdaf33102158ea9db79d2226a8de8e0b4c03e8ad60d862a58b093c847c9); /* statement */ 
for (uint256 actionIdx = actionStartIdx; actionIdx < actionEndIdx; actionIdx++) {
                // validate primary account
coverage_0x9d2f76df(0x4f7bb706eb534afbe601e50b95ef78fec8140f696054e32c82b56652e6978b6a); /* line */ 
                coverage_0x9d2f76df(0xadc2877774d9c63b816bd285974e188fa988ed81420789bb5a73c33ff7d6c15d); /* statement */ 
Actions.ActionArgs memory action = actions[actionIdx];
coverage_0x9d2f76df(0x611053775811bbd68e4c04415f0ae685aaeb9560e4e5b8fa975d81a368a83190); /* line */ 
                coverage_0x9d2f76df(0xa225e71f819c71e7bb402692a5f4b0307ee581e8ff9f05561d3faa4942d39086); /* statement */ 
validateAccountOwner(accounts[action.accountId].owner, signer);

                // validate second account in the case of a transfer
coverage_0x9d2f76df(0x4f841d48854bb6c99d9ede84d3d5d16afda5d3da51df76f9d73e4780f8e8973b); /* line */ 
                coverage_0x9d2f76df(0x6afc866b7c711af0c89d22a8202e13ba01708530f2d4adf7b0067549b6f62e10); /* statement */ 
if (action.actionType == Actions.ActionType.Transfer) {coverage_0x9d2f76df(0xc69e3bd34b878c8b0f82e2130f153a06c5695f7a9d8a7cfc5fd29bf65090b28b); /* branch */ 

coverage_0x9d2f76df(0xa20bd86fd37d029e69fdc51eaa4a0b0fc50e64f2591d9ad9dfba2e50b0e62d10); /* line */ 
                    coverage_0x9d2f76df(0x23646f8989b106b42b99de368e0bfade3d5e2370ca9c3935b6940a12f9d00684); /* statement */ 
validateAccountOwner(accounts[action.otherAccountId].owner, signer);
                }else { coverage_0x9d2f76df(0x0eed9f7d10f63359f612b36cb4f694e0df2146213d853d18237041aa501322f6); /* branch */ 
}
            }

            // update actionStartIdx
coverage_0x9d2f76df(0x41ae360fb52b4a75ba67afcb3ced9e7a2ea871308ce5c27bc09b939f5960f8af); /* line */ 
            coverage_0x9d2f76df(0x4d989fc4106375e8c43e55ae3313f8b885b62faf4ef7f99dd8a61a3b54850d83); /* statement */ 
actionStartIdx = actionEndIdx;
        }

        // require that all actions are signed or from msg.sender
coverage_0x9d2f76df(0xc03e5f6656f5a59ef3df508b05a3dff32b8386c7bb4f8c1c1d66e35ef2fdd36e); /* line */ 
        coverage_0x9d2f76df(0x0d76cbeb7736d99c05919c24a086914804db13e11b600398df9af5020f1b10d3); /* statement */ 
Require.that(
            actionStartIdx == actions.length,
            FILE,
            "Not all actions are signed"
        );

        // send the operation
coverage_0x9d2f76df(0xed5195d7d06c1b97cea5ae1bcb8a9cfa8cc21302fa79a83f894abeffc376d56b); /* line */ 
        coverage_0x9d2f76df(0x39f596416171bdbb21020811baede20bb651b80e45162f5ee89523cee6608b05); /* statement */ 
SOLO_MARGIN.operate(accounts, actions);
    }

    // ============ Getters ============

    /**
     * Returns a bool for each operation. True if the operation is invalid (from being canceled or
     * previously executed).
     */
    function getOperationsAreInvalid(
        bytes32[] memory operationHashes
    )
        public
        view
        returns(bool[] memory)
    {coverage_0x9d2f76df(0x3956468366dd2ecc0ba0b0096b2e5c3fc9b179c1e890bc1f7659e35f8db123d2); /* function */ 

coverage_0x9d2f76df(0x7acfec368dc660885264606a8dd36be2bb5a6b390bb0383b496022cebd02cd73); /* line */ 
        coverage_0x9d2f76df(0xd806c6be414c0e4f7ba393b9f14a80532a6476702f39b6e92dbb6a675454dc4b); /* statement */ 
uint256 numOperations = operationHashes.length;
coverage_0x9d2f76df(0x5404c19f67bf702e8f1ed133431bfe676c2e54b2d6d9937557570d2bda590fd6); /* line */ 
        coverage_0x9d2f76df(0xac07b2afa974574c8cb3f58929026e8191f9c6deadd8e6ae3de5feed95a2b026); /* statement */ 
bool[] memory output = new bool[](numOperations);

coverage_0x9d2f76df(0x39e11ad6d5433a701b9b43ef4a14e76443f82d492148eaa2543138376d6c00af); /* line */ 
        coverage_0x9d2f76df(0x023f7af612bf2536a7e1ed09aa87b35d8471d423848eb70f2de982541034e662); /* statement */ 
for (uint256 i = 0; i < numOperations; i++) {
coverage_0x9d2f76df(0x714d5fd414612fa77a7c12feef15b1b7cf0ac829633d9b788bd43ca1999fa242); /* line */ 
            coverage_0x9d2f76df(0x4e56dd5c2f02a42aa2d9623bbe0bdf5e0286459fa2f118e5c7262c54cafe7f88); /* statement */ 
output[i] = g_invalidated[operationHashes[i]];
        }
coverage_0x9d2f76df(0x25b2896e1cc4931be6157c6341a4d75075868ec225aa2e8937b596729479abab); /* line */ 
        coverage_0x9d2f76df(0xba8905cdf80515eb84e248e7c867a22bfad0b8392e5b00b47a366a3268c0df77); /* statement */ 
return output;
    }

    // ============ Private Helper Functions ============

    /**
     * Validates that either the signer or the msg.sender are the accountOwner (or that either are
     * localOperators of the accountOwner).
     */
    function validateAccountOwner(
        address accountOwner,
        address signer
    )
        private
        view
    {coverage_0x9d2f76df(0xf4ce313be666f9d2484c14f64457ad8779a5ef39305ee78d14658fc24fe74062); /* function */ 

coverage_0x9d2f76df(0xc0d368c886b4801127ab4947be8ba87410c6f5e367ff95d1ec6754ce2667fb4a); /* line */ 
        coverage_0x9d2f76df(0x22718274060eeb2c414af5c3f33818427c492d2b87de93ff2d5c5aa3c3e84fb6); /* statement */ 
bool valid =
            msg.sender == accountOwner
            || signer == accountOwner
            || SOLO_MARGIN.getIsLocalOperator(accountOwner, msg.sender)
            || SOLO_MARGIN.getIsLocalOperator(accountOwner, signer);

coverage_0x9d2f76df(0xbc80b99fd06b2fd9d94038cefc9f00d28d94083ff841a0b740d2ebc688c2604c); /* line */ 
        coverage_0x9d2f76df(0x2a931664dc636ecdcc5336bf4cb56e07cf5a44ea2f1ba80034fe6c20ebd4179e); /* statement */ 
Require.that(
            valid,
            FILE,
            "Signer not authorized",
            signer
        );
    }

    /**
     * Returns the EIP712 hash of an Operation message.
     */
    function getOperationHash(
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions,
        Authorization memory auth,
        uint256 startIdx
    )
        private
        view
        returns (bytes32)
    {coverage_0x9d2f76df(0xfc9642f19e260b93ddc27f2722a473fb48b47043cff2738976524fcd967a7395); /* function */ 

        // get the bytes32 hash of each action, then packed together
coverage_0x9d2f76df(0x39d8a34402321a8260d62b41e102257f45f7a134dead31a66628db066087ba20); /* line */ 
        coverage_0x9d2f76df(0xfbd8dc77707ad340b02625c0928c0544e2f9b61a2f03550fa11068d8e7ee5d12); /* statement */ 
bytes32 actionsEncoding = getActionsEncoding(
            accounts,
            actions,
            auth,
            startIdx
        );

        // compute the EIP712 hashStruct of an Operation struct
        /* solium-disable-next-line indentation */
coverage_0x9d2f76df(0x371acad8475867204b6297f1c7bc55d168cde9f3615a870153a9efe003f22dbe); /* line */ 
        coverage_0x9d2f76df(0x0a7b8693590ec4823f2ccd53d90491dc9b6dd4bd361f939e508f4952ebb8cae8); /* statement */ 
bytes32 structHash = keccak256(abi.encode(
            EIP712_OPERATION_HASH,
            actionsEncoding,
            auth.header
        ));

        // compute eip712 compliant hash
        /* solium-disable-next-line indentation */
coverage_0x9d2f76df(0x74ef8f2af04c336049cb12470d25c085c93f107c13be3e5ac99dfe7219deefe6); /* line */ 
        coverage_0x9d2f76df(0x6f118f3365e95882c902e778f858a9ccdb88644957d203a6de52b409d14399ea); /* statement */ 
return keccak256(abi.encodePacked(
            EIP191_HEADER,
            EIP712_DOMAIN_HASH,
            structHash
        ));
    }

    /**
     * Returns the EIP712 encodeData of an Action struct array.
     */
    function getActionsEncoding(
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions,
        Authorization memory auth,
        uint256 startIdx
    )
        private
        pure
        returns (bytes32)
    {coverage_0x9d2f76df(0x10a20be966969e009ec433223b9a21922f77341c83be1baf38b8f4f815d82fc2); /* function */ 

        // store hash of each action
coverage_0x9d2f76df(0x1b1e285196eec917f7bbea64b972ed004f5f0b8a0ec8876039c5240247056eaf); /* line */ 
        coverage_0x9d2f76df(0xa22a83205d6af15e1a9c7fa5295709618fbf8108f291d675746252680ca48eef); /* statement */ 
bytes32[] memory actionsBytes = new bytes32[](auth.numActions);

        // for each action that corresponds to the auth
coverage_0x9d2f76df(0xd1e69112e503bdc8ce2922a56986fc7d6e6986c4f0dcb6dd97a351c8c1480252); /* line */ 
        coverage_0x9d2f76df(0x9ca4a8ca8216e156b224d51e7d24c73f5e92063c75529631f31fd361fc0aca1a); /* statement */ 
for (uint256 i = 0; i < auth.numActions; i++) {
coverage_0x9d2f76df(0xf466c62cd5c5f557dbc97bc76d6b70302a63a5847693ec73a0bd14fc4b5ef217); /* line */ 
            coverage_0x9d2f76df(0x8469d016ae609bedf580132003570499b4978ee70b53a33f7cd61e11147b3f88); /* statement */ 
Actions.ActionArgs memory action = actions[startIdx + i];

            // if action type has no second account, assume null account
coverage_0x9d2f76df(0x34cf02fdaaef975e4fc24c0bd20bc4ec6cbf97fc981d01bac0e770b4d8a51066); /* line */ 
            coverage_0x9d2f76df(0xa607502f59e1d684f486a38543af50ef7b1c322027963e3628ae5763878d8a47); /* statement */ 
Account.Info memory otherAccount =
                (Actions.getAccountLayout(action.actionType) == Actions.AccountLayout.OnePrimary)
                ? Account.Info({ owner: address(0), number: 0 })
                : accounts[action.otherAccountId];

            // compute the individual hash for the action
            /* solium-disable-next-line indentation */
coverage_0x9d2f76df(0x498a676e65d7f3bb99cf03182334947b1eee0ede89804b86fcbaa0c158767a5a); /* line */ 
            coverage_0x9d2f76df(0x834af759703f4e3cd576d0178e57b6cc08472a3f5a172df391672e1ee32ab59e); /* statement */ 
actionsBytes[i] = getActionHash(
                action,
                accounts[action.accountId],
                otherAccount
            );
        }

coverage_0x9d2f76df(0xc6c6c58f978f8221342e78a774570880a03ce5398dcf02a8389972b63e0445da); /* line */ 
        coverage_0x9d2f76df(0xfed419dd21757bce77bbf78e049a34ce94dd675bd423b55904e046f8324a32e6); /* statement */ 
return keccak256(abi.encodePacked(actionsBytes));
    }

    /**
     * Returns the EIP712 hashStruct of an Action struct.
     */
    function getActionHash(
        Actions.ActionArgs memory action,
        Account.Info memory primaryAccount,
        Account.Info memory secondaryAccount
    )
        private
        pure
        returns (bytes32)
    {coverage_0x9d2f76df(0xb9247d4eefa086d65d357410e1f8c681eec3d691842160e954c61713c338f87e); /* function */ 

        /* solium-disable-next-line indentation */
coverage_0x9d2f76df(0x95b3e47315ccc7bd44fa79c69e643eed7a022c10fc3a9ee0e2c3364b09e8daf9); /* line */ 
        coverage_0x9d2f76df(0xdd57e98fdb1162b7f29e792a388d45fd351071c7cd32135ac2b9fb0fb2a89671); /* statement */ 
return keccak256(abi.encode(
            EIP712_ACTION_HASH,
            action.actionType,
            primaryAccount.owner,
            primaryAccount.number,
            getAssetAmountHash(action.amount),
            action.primaryMarketId,
            action.secondaryMarketId,
            action.otherAddress,
            secondaryAccount.owner,
            secondaryAccount.number,
            keccak256(action.data)
        ));
    }

    /**
     * Returns the EIP712 hashStruct of an AssetAmount struct.
     */
    function getAssetAmountHash(
        Types.AssetAmount memory amount
    )
        private
        pure
        returns (bytes32)
    {coverage_0x9d2f76df(0x535b3a4824db1c826734fe7ce5d3c4a2cc458c917fa115cb376112b5752eac5e); /* function */ 

        /* solium-disable-next-line indentation */
coverage_0x9d2f76df(0xa65e57e86ae8c523289109b2cc6c5109df54dd504f20685e644232950f51fe13); /* line */ 
        coverage_0x9d2f76df(0xc80751eaf1e41c78bd0dc916d5e0e41d5e106cbb742044dda0fbdea5a7922e21); /* statement */ 
return keccak256(abi.encode(
            EIP712_ASSET_AMOUNT_HASH,
            amount.sign,
            amount.denomination,
            amount.ref,
            amount.value
        ));
    }
}
