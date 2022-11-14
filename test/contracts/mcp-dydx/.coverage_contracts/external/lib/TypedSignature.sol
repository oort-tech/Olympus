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

import { Require } from "../../protocol/lib/Require.sol";


/**
 * @title TypedSignature
 * @author dYdX
 *
 * Library to unparse typed signatures
 */
library TypedSignature {
function coverage_0xe47c54d1(bytes32 c__0xe47c54d1) public pure {}


    // ============ Constants ============

    bytes32 constant private FILE = "TypedSignature";

    // prepended message with the length of the signed hash in decimal
    bytes constant private PREPEND_DEC = "\x19Ethereum Signed Message:\n32";

    // prepended message with the length of the signed hash in hexadecimal
    bytes constant private PREPEND_HEX = "\x19Ethereum Signed Message:\n\x20";

    // Number of bytes in a typed signature
    uint256 constant private NUM_SIGNATURE_BYTES = 66;

    // ============ Enums ============

    // Different RPC providers may implement signing methods differently, so we allow different
    // signature types depending on the string prepended to a hash before it was signed.
    enum SignatureType {
        NoPrepend,   // No string was prepended.
        Decimal,     // PREPEND_DEC was prepended.
        Hexadecimal, // PREPEND_HEX was prepended.
        Invalid      // Not a valid type. Used for bound-checking.
    }

    // ============ Functions ============

    /**
     * Gives the address of the signer of a hash. Also allows for the commonly prepended string of
     * '\x19Ethereum Signed Message:\n' + message.length
     *
     * @param  hash               Hash that was signed (does not include prepended message)
     * @param  signatureWithType  Type and ECDSA signature with structure: {32:r}{32:s}{1:v}{1:type}
     * @return                    address of the signer of the hash
     */
    function recover(
        bytes32 hash,
        bytes memory signatureWithType
    )
        internal
        pure
        returns (address)
    {coverage_0xe47c54d1(0x041759c648a01fa1f81006a89f92189da7177cb4498d6077110fd1285fd3ffda); /* function */ 

coverage_0xe47c54d1(0x95f9cfe1b429cdfbad2f42ff4c66c670da57831006ea88f4599263697eb4a103); /* line */ 
        coverage_0xe47c54d1(0xb173c92a6ea8a2fa0fa8c7c04349090934c31a33470fbe1289a4535bc6ee0727); /* statement */ 
Require.that(
            signatureWithType.length == NUM_SIGNATURE_BYTES,
            FILE,
            "Invalid signature length"
        );

coverage_0xe47c54d1(0x626c10b495681ca07af2e8e2ecb7c0bf2feb791404d610648d7c511b5960c265); /* line */ 
        coverage_0xe47c54d1(0x04fced0597408f8ce24eb19b1a7660bf60f572d28c43b450bfbcd633a2679e40); /* statement */ 
bytes32 r;
coverage_0xe47c54d1(0xf0081e42246f8a4302dc98ad045a95f9f20fc077fe91c7c11154898f465b667b); /* line */ 
        coverage_0xe47c54d1(0xee1f2c89c22430a93c51dde88a68c6f0dc0a94788782ee79e926fe01f4d1495b); /* statement */ 
bytes32 s;
coverage_0xe47c54d1(0x798ce28762d269c02ae8fdfda21e6df1f6bd8886561727fa9a70eb38855d1ee4); /* line */ 
        coverage_0xe47c54d1(0x2dc0f4c44c6ae175b2954ec496d9e27aa4c441fe13e74da97b0f3f72ab21d039); /* statement */ 
uint8 v;
coverage_0xe47c54d1(0xc5668a640db28fdb05a1768baede6396542f9594bd4a5000f210421e33d07fd9); /* line */ 
        coverage_0xe47c54d1(0x745f7811bdba18a40cc7bf2a34608160107f23668fc2d87f29125e38090ff21e); /* statement */ 
uint8 rawSigType;

        /* solium-disable-next-line security/no-inline-assembly */
coverage_0xe47c54d1(0x315c59acec3bfa6bde7fe46c4cfcb3b8d75997defcad9feb819a9c3f1a965958); /* line */ 
        assembly {
            r := mload(add(signatureWithType, 0x20))
            s := mload(add(signatureWithType, 0x40))
            let lastSlot := mload(add(signatureWithType, 0x60))
            v := byte(0, lastSlot)
            rawSigType := byte(1, lastSlot)
        }

coverage_0xe47c54d1(0x6d2dfccc4e0f579189e4361210230436c3695bc44b0d2a2e1f2bb755e779d185); /* line */ 
        coverage_0xe47c54d1(0x753a6250ad8d4a5993e47e8a32e22eaab4ad127ba289be6d1a283f8d94b0b1a5); /* statement */ 
Require.that(
            rawSigType < uint8(SignatureType.Invalid),
            FILE,
            "Invalid signature type"
        );

coverage_0xe47c54d1(0xc13a3233e520a840ccfa6394c99b73a9ea53a8a47df603f8dbf4ff668d5a126d); /* line */ 
        coverage_0xe47c54d1(0x47092933d98051731d28aba412bf496244ef92d481419e616b36b5e8c6ce3689); /* statement */ 
SignatureType sigType = SignatureType(rawSigType);

coverage_0xe47c54d1(0x13b174ffe6a3734a8e5c8aa6e852617441a4252736618e5159a6370b082b4adb); /* line */ 
        coverage_0xe47c54d1(0x67d0a7cf23e13f8ef301e0f73da16f98518edc41fe10057b230e1eea66045818); /* statement */ 
bytes32 signedHash;
coverage_0xe47c54d1(0x863cf707e03ef9672214cb9f6943c7c287f1f39349955f6860628781d322fc94); /* line */ 
        coverage_0xe47c54d1(0xd16bd059fc5244f2376ea3b6e71212d85625ce06c5de6dc35da00d8de65f293e); /* statement */ 
if (sigType == SignatureType.NoPrepend) {coverage_0xe47c54d1(0x9ca1107982d9df7772a9ee2d4b6c9cac7ff4f90e88f68bce6d1d1de106e29c2e); /* branch */ 

coverage_0xe47c54d1(0x12644b0de1c6ddc40acae15482f324ac08a0db4f9f2b64cdc9939975f56bfa69); /* line */ 
            coverage_0xe47c54d1(0x0c44bc845ca5d66aeb7ab1b034a86b3787b4891ddecdddb1c7809c8947ea41bd); /* statement */ 
signedHash = hash;
        } else {coverage_0xe47c54d1(0x18f6d336d61c04197490f97620430ccc0e5f5800684b631c5358c9a8fdff1670); /* statement */ 
coverage_0xe47c54d1(0xed190b4ac4c663ca60decd118b0860adf36dc0477394c6042c6e868062585e29); /* branch */ 
if (sigType == SignatureType.Decimal) {coverage_0xe47c54d1(0x3c6d382b4e47cf985dc4adf178e047c6dcc487e656b123fdffab69723c2a25b6); /* branch */ 

coverage_0xe47c54d1(0x1bbde6432323766bac362375eb573ddb03be6f0bf8a1966a6cde2f2db47da51e); /* line */ 
            coverage_0xe47c54d1(0xfd8dd6a31b1d723e69b057e3f0e9d2d501aa6c50345d5c09a70ad9c0b960ca09); /* statement */ 
signedHash = keccak256(abi.encodePacked(PREPEND_DEC, hash));
        } else {coverage_0xe47c54d1(0xcac9e7f17e26f1c5fd79500839a1d10023456dec9b031f6ebfd377efb555c19b); /* branch */ 

coverage_0xe47c54d1(0x076b7dea97d57f5c158d4da0a87f3d90ca83c8f9922bb1ff22542c4b2359a6c8); /* line */ 
            coverage_0xe47c54d1(0x2e7c0386e8dd443eb4b9e22caff8dc9718ac98f7ea2569fa28623674f7b3a36f); /* assertPre */ 
coverage_0xe47c54d1(0x7d5ebf01fea120b8fde2a8d72ff8aff863f53dab51b21d46c79487160bd13cd2); /* statement */ 
assert(sigType == SignatureType.Hexadecimal);coverage_0xe47c54d1(0xf62e2eddf3e41fadbf3abf37d585298f27d0bcf9492542eb4c1071d3b6b98475); /* assertPost */ 

coverage_0xe47c54d1(0x1a6ce5b645e86e8e758002d4c9699c5b4b1f6a579bbb1eea542887155085380f); /* line */ 
            coverage_0xe47c54d1(0xbb350ece8720c08c924fde8eb0f0ad2de16eeec33b2f29e62da612150e94c236); /* statement */ 
signedHash = keccak256(abi.encodePacked(PREPEND_HEX, hash));
        }}

coverage_0xe47c54d1(0x8e236f6704617bad823ab84e836c27b1d806890cb2427bdb0343d5cc732724c8); /* line */ 
        coverage_0xe47c54d1(0xd3579bfe82d54b674de4a42a5d52a07d3a2c3daadc90a1d1692f02c809111b0b); /* statement */ 
return ecrecover(
            signedHash,
            v,
            r,
            s
        );
    }
}
