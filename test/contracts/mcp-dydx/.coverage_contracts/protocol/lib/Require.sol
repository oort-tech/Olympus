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


/**
 * @title Require
 * @author dYdX
 *
 * Stringifies parameters to pretty-print revert messages. Costs more gas than regular require()
 */
library Require {
function coverage_0x87cd0472(bytes32 c__0x87cd0472) public pure {}


    // ============ Constants ============

    uint256 constant ASCII_ZERO = 48; // '0'
    uint256 constant ASCII_RELATIVE_ZERO = 87; // 'a' - 10
    uint256 constant ASCII_LOWER_EX = 120; // 'x'
    bytes2 constant COLON = 0x3a20; // ': '
    bytes2 constant COMMA = 0x2c20; // ', '
    bytes2 constant LPAREN = 0x203c; // ' <'
    byte constant RPAREN = 0x3e; // '>'
    uint256 constant FOUR_BIT_MASK = 0xf;

    // ============ Library Functions ============

    function that(
        bool must,
        bytes32 file,
        bytes32 reason
    )
        internal
        pure
    {coverage_0x87cd0472(0x9447daadd229585ae7a869be07f4839c467b8a1cb1d9e0bbd5a89a35fbc46793); /* function */ 

coverage_0x87cd0472(0x5dc085ae114d2bc91b9d018eb9e26904fe8d5b09a02bde5708c62c164cb2aa13); /* line */ 
        coverage_0x87cd0472(0x0e77a25a5c1beb34bcf305270db72624be20ec4e08929c323a73aefa36546f3b); /* statement */ 
if (!must) {coverage_0x87cd0472(0x7a6411b026a494962c1ee01468fddc758526386e8e1a56db5b5422849a47aa26); /* branch */ 

coverage_0x87cd0472(0x03e48846372414514e1d08bb4af4467cdb99bd8e00d00abbdfbbf0f718a795a0); /* line */ 
            coverage_0x87cd0472(0x49d8fc1a259db5d81e6eadcd44882c9f4102c99da77600e3620bf547f07c30e6); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason)
                    )
                )
            );
        }else { coverage_0x87cd0472(0x69e940bf4af7de11f888a5087e3923ebe1b17d8a64ab5644e3290f12de1754fb); /* branch */ 
}
    }

    function that(
        bool must,
        bytes32 file,
        bytes32 reason,
        uint256 payloadA
    )
        internal
        pure
    {coverage_0x87cd0472(0x52ed8e8ff3a29c63e53dcd60cd7a7814d991a44bd937b9d6c60c3967a638e93c); /* function */ 

coverage_0x87cd0472(0x8236613c0e437873075425bf8a7f6d2385efaaa910f211dab343049e7cbee80a); /* line */ 
        coverage_0x87cd0472(0x9a4e0fd6af88a9f47dfc67ef4fe0bee8aa6a19350653f37b98723ff1858d28b5); /* statement */ 
if (!must) {coverage_0x87cd0472(0xa6e8d3b0a8dec34e048c5285e839aaf9466e8dc1a9669631ac2a3d533036fe5c); /* branch */ 

coverage_0x87cd0472(0x7b0398b55b3a7d99825ea82fdb93adbe5a95d9bd700d299a1e47087659dc80b3); /* line */ 
            coverage_0x87cd0472(0xfb1910df91c2f538b96a4689f7b36d3745bf805c2da2895a30866a6cd969aa17); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason),
                        LPAREN,
                        stringify(payloadA),
                        RPAREN
                    )
                )
            );
        }else { coverage_0x87cd0472(0xa4184196e067a45f2ff72252fc4d04e10ba955e22ad763b9443344ffad074312); /* branch */ 
}
    }

    function that(
        bool must,
        bytes32 file,
        bytes32 reason,
        uint256 payloadA,
        uint256 payloadB
    )
        internal
        pure
    {coverage_0x87cd0472(0xe89b685796930aac64afcaad7a9a0d48264ec2666c96007682e92e783752b6cf); /* function */ 

coverage_0x87cd0472(0x77d45d440b8bbe3b11fd45a289cbfe7fa7e8609c0480ab6583020bf25141e4a5); /* line */ 
        coverage_0x87cd0472(0xad9dc1a14aa3fbabaf5f8292ead1f80ddc1a416dfe47d1f9e6c6a32fd9cea1ef); /* statement */ 
if (!must) {coverage_0x87cd0472(0xf4a057ed2f5f6092e265ff9697ca4adf9a6efc0f7b461c11e0fbe9d606d12107); /* branch */ 

coverage_0x87cd0472(0x5a3759797c265ac64bfb2e97fe1979905de4055913e1cb5e66fc33ff6f31a3b9); /* line */ 
            coverage_0x87cd0472(0xa6433fdc9ec0208382916daf902b713c3091ce486ce575d6848f8a33324c4206); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason),
                        LPAREN,
                        stringify(payloadA),
                        COMMA,
                        stringify(payloadB),
                        RPAREN
                    )
                )
            );
        }else { coverage_0x87cd0472(0xf939e6e401f897525f78da6a061f5539411448110f521280b9a9fbde7ba61ad1); /* branch */ 
}
    }

    function that(
        bool must,
        bytes32 file,
        bytes32 reason,
        address payloadA
    )
        internal
        pure
    {coverage_0x87cd0472(0xb614ced6e2aed1447103654ff842bce57a9473dc76f06b73bc8e4be63058e796); /* function */ 

coverage_0x87cd0472(0xf30e0236be20414c8a85e1e166012d7f9cf10a21ce964ab1710d733709b9a589); /* line */ 
        coverage_0x87cd0472(0x4930f3836b0726d84eef0004237046bbfc855db0352d68940f0a7f69951e5a35); /* statement */ 
if (!must) {coverage_0x87cd0472(0xdf688e5031002323f25ccd8f553074167c1e130af38bb05adfaa8605a2888b69); /* branch */ 

coverage_0x87cd0472(0x2f676651a02272a3412d391758ec8e03894a358be1ef96506b158c9c96b1e5b7); /* line */ 
            coverage_0x87cd0472(0x1e4a587017e709edfd9375a60deb8aa95bc2705a0198059cad43be8daa881b24); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason),
                        LPAREN,
                        stringify(payloadA),
                        RPAREN
                    )
                )
            );
        }else { coverage_0x87cd0472(0x204295ba79cb5e2ecbaa4ec12f28803f3841ca5751da37f1c3791c896dff3511); /* branch */ 
}
    }

    function that(
        bool must,
        bytes32 file,
        bytes32 reason,
        address payloadA,
        uint256 payloadB
    )
        internal
        pure
    {coverage_0x87cd0472(0x4d28610ecf58163374138e506ed62f714b181b99ba419b695ec64ab5421faffc); /* function */ 

coverage_0x87cd0472(0x1e11f899dbdfa1ce3144310c0c076fb1dad5dbcbd2555cecf04f45c55c2ce4fe); /* line */ 
        coverage_0x87cd0472(0x6852b30996d0be761b88a73e2e454ec9535ecb1c8b78f0aed388514ffaea343e); /* statement */ 
if (!must) {coverage_0x87cd0472(0x6f223b46775163a56ed2212ecae415f3ec8a70b40b62c2b39b0d7040674d065a); /* branch */ 

coverage_0x87cd0472(0x815a2457a92b8c2bcb16ec1f3b50f2dd8315be49a5ebcd96888dd7206293915e); /* line */ 
            coverage_0x87cd0472(0x023fe07e74b042ec7995b229c3d5ee19a353de9bd1af2a3ebbc1309a715343b5); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason),
                        LPAREN,
                        stringify(payloadA),
                        COMMA,
                        stringify(payloadB),
                        RPAREN
                    )
                )
            );
        }else { coverage_0x87cd0472(0x1f967f45ff34249c44fef8960fd51611cd66a4c916a4105017da714bc1913c79); /* branch */ 
}
    }

    function that(
        bool must,
        bytes32 file,
        bytes32 reason,
        address payloadA,
        uint256 payloadB,
        uint256 payloadC
    )
        internal
        pure
    {coverage_0x87cd0472(0x332deb0f9ff1fc247abe3b6593df31240aa1eb02e590aa17ad8d50017c2b8fb6); /* function */ 

coverage_0x87cd0472(0x1c2283e105959e28be52c54693ae0d28f466f3c998db75985450637fd2175b34); /* line */ 
        coverage_0x87cd0472(0x3bea09a5ccab87dda9e4d5f4420ac9c2d92da3d5d27d8457e85a28fb9c88ec04); /* statement */ 
if (!must) {coverage_0x87cd0472(0xe198b280c224ed95b652e70b0fc199af01ad24a16a1550201f54bcacd8a8e469); /* branch */ 

coverage_0x87cd0472(0x3c51793bacdf00f1167e6c5095f18c9e94a61306c2e482aeaa400f5b99b2970e); /* line */ 
            coverage_0x87cd0472(0x830760a086c2d8bf74d25028898c116b143f49f66baa10a1741fd79f364e0603); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason),
                        LPAREN,
                        stringify(payloadA),
                        COMMA,
                        stringify(payloadB),
                        COMMA,
                        stringify(payloadC),
                        RPAREN
                    )
                )
            );
        }else { coverage_0x87cd0472(0x739526cd02c2b8223f8b905246d05b8ad22555e30936a6fecbd5229a7beda147); /* branch */ 
}
    }

    function that(
        bool must,
        bytes32 file,
        bytes32 reason,
        bytes32 payloadA
    )
        internal
        pure
    {coverage_0x87cd0472(0x30444f65fb18a164f3ef0ba178a62f9579a5c943c560baebc5fbf4772e9a58b5); /* function */ 

coverage_0x87cd0472(0x75c8af462f33347f98ba812c51b838a71c23516ecace0143dafd045d0fd3a18c); /* line */ 
        coverage_0x87cd0472(0xc17c0b4d99ab14b77c296ee089f78c6ea6818baa43d7a8fae76c2ec06caec380); /* statement */ 
if (!must) {coverage_0x87cd0472(0x7f8dade92e43850f2945a497133009ffad02a919b8dd096040412d98598eadd7); /* branch */ 

coverage_0x87cd0472(0x70425c0fbef63d59a1c4a51b72c3a3f135084a87e302d35c6e252dd0d198cf79); /* line */ 
            coverage_0x87cd0472(0x0104117021c6f2e864e7b44f380e2339b82a80d3554e78dc5469e7155ee57f64); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason),
                        LPAREN,
                        stringify(payloadA),
                        RPAREN
                    )
                )
            );
        }else { coverage_0x87cd0472(0xb9cbb4a44326006083d4061eccd47bb8e94fef04432507bba91e2318669256bb); /* branch */ 
}
    }

    function that(
        bool must,
        bytes32 file,
        bytes32 reason,
        bytes32 payloadA,
        uint256 payloadB,
        uint256 payloadC
    )
        internal
        pure
    {coverage_0x87cd0472(0x483a29e76fcfa5096da31144ac39198ef1e3312956d897c878c699b46004463f); /* function */ 

coverage_0x87cd0472(0x4b7734f568119a6f200e00bfbe786d5b10630031ed7cc9b2ab89aad930ba87ae); /* line */ 
        coverage_0x87cd0472(0x54245e63d0ca9e06316e192a437eb9a29be47bfe31b7d907c57d378b2ee5fe1d); /* statement */ 
if (!must) {coverage_0x87cd0472(0x2d10569244731b6756ed38ceb1da469ba45fb67fdf7071e64617194cce4148ac); /* branch */ 

coverage_0x87cd0472(0xb077e3c1e71f5a1112388e6109a5a96675467faca64302b0635a7445f5610087); /* line */ 
            coverage_0x87cd0472(0xd6f42ecac03aff0adfa7297c000be24fa930709a8c355784a1ee4eabc778bcec); /* statement */ 
revert(
                string(
                    abi.encodePacked(
                        stringifyTruncated(file),
                        COLON,
                        stringifyTruncated(reason),
                        LPAREN,
                        stringify(payloadA),
                        COMMA,
                        stringify(payloadB),
                        COMMA,
                        stringify(payloadC),
                        RPAREN
                    )
                )
            );
        }else { coverage_0x87cd0472(0x86458df926e2812554b035745ccc5a30ec84f51f3a6a578c0da6381f3c2b0dec); /* branch */ 
}
    }

    // ============ Private Functions ============

    function stringifyTruncated(
        bytes32 input
    )
        private
        pure
        returns (bytes memory)
    {coverage_0x87cd0472(0xcff6f9c6277f7210446494808b69e987e2fd0f308693f2dabe181eb96aef6754); /* function */ 

        // put the input bytes into the result
coverage_0x87cd0472(0xadaa58429f7d7806e1be25819ea86f9947cbe0bd848c417e4c280288b1ee5276); /* line */ 
        coverage_0x87cd0472(0x91987059b7897fbaafb96acfec1cb1ba021dabc26b15256c47d596e8374f3651); /* statement */ 
bytes memory result = abi.encodePacked(input);

        // determine the length of the input by finding the location of the last non-zero byte
coverage_0x87cd0472(0x2e9d0bba12e2e664ac95db37da4d8aa54b10db96abc1753a92f21b11b2e6bc07); /* line */ 
        coverage_0x87cd0472(0x2f765fa9720edb6931d7d3f060afedd2d65a499002e0f736753e7e3422e7b6d1); /* statement */ 
for (uint256 i = 32; i > 0; ) {
            // reverse-for-loops with unsigned integer
            /* solium-disable-next-line security/no-modify-for-iter-var */
coverage_0x87cd0472(0xbd94db29cfb3bce4f756ecc50ef6854cabc5be833f0728fdf91984dde910b2f9); /* line */ 
            i--;

            // find the last non-zero byte in order to determine the length
coverage_0x87cd0472(0x4efe85656633a8ea2f35e579d0d7db3f556289a2977d922c6a554d52e72241e0); /* line */ 
            coverage_0x87cd0472(0xda8ba480b65d111d270586b5aaeda8350518c7c192ac0bcd6c11fb9a73561f91); /* statement */ 
if (result[i] != 0) {coverage_0x87cd0472(0xafd87ea108c256e8705100361361b9b26d6397e98ff05b496e33f50ff10ae6df); /* branch */ 

coverage_0x87cd0472(0xbd52d6acea28efc2262b43ffabc67c5d5e4e7248f1a587d4fa1464a31a176d4c); /* line */ 
                coverage_0x87cd0472(0xa6247fcf0916495dc5a5b3022bee2d74c9dd57d071a177ca19d060c03c7499d3); /* statement */ 
uint256 length = i + 1;

                /* solium-disable-next-line security/no-inline-assembly */
coverage_0x87cd0472(0xef8397bf362ca5aa51077b1daa4a16f4ded996c4c8b1e5fef089373127024d85); /* line */ 
                assembly {
                    mstore(result, length) // r.length = length;
                }

coverage_0x87cd0472(0xddb0099ff1a371b04bd6d740f969e72c4f9069fefca454122870eaf3ca1ff132); /* line */ 
                coverage_0x87cd0472(0x886e0020203b42e98b0038e56039490b1dbe3a37e646e9db7a44e6ed7c41100b); /* statement */ 
return result;
            }else { coverage_0x87cd0472(0x1ef2d50c220341f544311a9ab2afa7866be3b67839a8716ad460b800a813a478); /* branch */ 
}
        }

        // all bytes are zero
coverage_0x87cd0472(0x0c7b5f04f332e957f668629d4a1cb9273f85df9688905e00624fdc8ea4d3ebe4); /* line */ 
        coverage_0x87cd0472(0xbaa30fe52445dbe7b41e1561ecab2b3a0f52b415f652e6818d283725cda83dff); /* statement */ 
return new bytes(0);
    }

    function stringify(
        uint256 input
    )
        private
        pure
        returns (bytes memory)
    {coverage_0x87cd0472(0x1e6b711c81a906e9c9b1b404597a4bd3291d248bc8ad6a22fd3e858c03cf9b7d); /* function */ 

coverage_0x87cd0472(0x793c0ea55ac2d48c3161dda2591ec743bcdce5f154083538109047fd212dde2e); /* line */ 
        coverage_0x87cd0472(0x87aebcf08e5f828d32e78686151c62477540277bfb72e83231362ad189012288); /* statement */ 
if (input == 0) {coverage_0x87cd0472(0xec1ca2e14532d55610e53445fa8ffd60b33d866e650c8bfd4d7f0c28269f1c5b); /* branch */ 

coverage_0x87cd0472(0x00320ef106bc58f2983782660a59d25d36a046ed9eb801615cc867f4f41244a3); /* line */ 
            coverage_0x87cd0472(0x97f3edde39cc9f3bfee3dfd12231a65f1d5afbad1ce13025303eb2170985ad52); /* statement */ 
return "0";
        }else { coverage_0x87cd0472(0xf4ab42f38f139976cc1ae521b8100c2eadc4b0e37efccff6e5a40e2665a483d5); /* branch */ 
}

        // get the final string length
coverage_0x87cd0472(0xb7fe1f0bd680380e7695aa44a3e1e6f3ac3175ebd0f87e95a283913130effa8d); /* line */ 
        coverage_0x87cd0472(0xa78319f3892afc6991a90fc9d46f0c142b07bae3102f9fe77ee76470b27f7185); /* statement */ 
uint256 j = input;
coverage_0x87cd0472(0x38c7cfbd372e1236436bfcb795005bcaa4b28713e81cec0dbfd6fccf0540a352); /* line */ 
        coverage_0x87cd0472(0x9e0f3d16edb7ff5640caea3762d7ff0eb7d64618ae8043452a33d2615a7ef229); /* statement */ 
uint256 length;
coverage_0x87cd0472(0x93110010545d4b4b7237f3b18b4cadc9dc0371ba8bcd1fdce1da2f1fa7923157); /* line */ 
        coverage_0x87cd0472(0xd77b340e7d5289e41ac90f1c6e38c5fa4e1dc1ad3f44f5412a00286eacf1feac); /* statement */ 
while (j != 0) {
coverage_0x87cd0472(0x098fea04b86fd0c38ec57e152624fb696f314344a06e9c60c054837367cbced5); /* line */ 
            length++;
coverage_0x87cd0472(0x4c2b6f8f3345550b04ba0a5aa1b381969c93792d734de47967d118c267ff7d17); /* line */ 
            coverage_0x87cd0472(0x7050d12e23a50058813ec62b503d3a0bef4601701b5c8804e7d402532d662e70); /* statement */ 
j /= 10;
        }

        // allocate the string
coverage_0x87cd0472(0x63e6e4470f6585cc060cd98bcc9a1e1249d04d696bfe1120b833d64aed0e6d86); /* line */ 
        coverage_0x87cd0472(0xfc2e46c68bc17549ea58e6561577843ea566cb6823e10a37ebb9dc0f59339ddd); /* statement */ 
bytes memory bstr = new bytes(length);

        // populate the string starting with the least-significant character
coverage_0x87cd0472(0xd0e3fab22baffa32620ea8b58fb5780281d55a7393f1fde157862b4a65928a5f); /* line */ 
        coverage_0x87cd0472(0xc84002c721841d09c90c0a05e24b0f1701b5b8fb5ccfe35357aa014900a1ae92); /* statement */ 
j = input;
coverage_0x87cd0472(0x34b6e405101c3f2e74a0b3266c4a81d74354b43c39ce4ad1e21b4f531cc1f0ee); /* line */ 
        coverage_0x87cd0472(0x16bb4248d9274584260540044367a8ce033f7304ca1aeae2b2715839942644e4); /* statement */ 
for (uint256 i = length; i > 0; ) {
            // reverse-for-loops with unsigned integer
            /* solium-disable-next-line security/no-modify-for-iter-var */
coverage_0x87cd0472(0x5fc3b7072e0396b5a1661d19bf361b63de1332ec68ce73513b2e180ec3cf3834); /* line */ 
            i--;

            // take last decimal digit
coverage_0x87cd0472(0x89629cde011a0d70062b8f00520217224149d6dcd0f1da1968c8ea90f2aa1eeb); /* line */ 
            coverage_0x87cd0472(0xed5272e1a9c90725cc85eaf2eb46f86d56cfacb5b9450d114ce1d3ca398605c4); /* statement */ 
bstr[i] = byte(uint8(ASCII_ZERO + (j % 10)));

            // remove the last decimal digit
coverage_0x87cd0472(0x1a400229cf63440e463e0fb9d78201c93cc8fecbbc8d2d121e359c5c92fb0ade); /* line */ 
            coverage_0x87cd0472(0xfbe894d43014eb7a886099119f0c274f13d8bbaf316e02c2b8ea7476eee05ba9); /* statement */ 
j /= 10;
        }

coverage_0x87cd0472(0x7bb27b678351f97453c5ff1c3a8c0aabd00fe8f6e10543de3c324b81adf785d2); /* line */ 
        coverage_0x87cd0472(0x6a2691c4f49dd0a24e9dd78f580f653d7de9287951301b5483fe270387d36908); /* statement */ 
return bstr;
    }

    function stringify(
        address input
    )
        private
        pure
        returns (bytes memory)
    {coverage_0x87cd0472(0x0d72d8acc531b498168f7020cd91001eb2875fae4106b832338ce09ed99810cb); /* function */ 

coverage_0x87cd0472(0x03c06cf17d67b26e4dbdc6a886715263ebd99c6945c8a5ba5401018947d666ff); /* line */ 
        coverage_0x87cd0472(0x3aef20bc6b7a84076c62deaa124f298e47de02d3997caa9b6b9978e946a92f1b); /* statement */ 
uint256 z = uint256(input);

        // addresses are "0x" followed by 20 bytes of data which take up 2 characters each
coverage_0x87cd0472(0xc419bbaff2e9c85b0693937037cec8de251314ae38c62eeba0dfdafd6e4d3f6e); /* line */ 
        coverage_0x87cd0472(0x3ef2a4b2752fdd5f280c295f50ff25e35db728c4838f2eadc406d1d359a9073a); /* statement */ 
bytes memory result = new bytes(42);

        // populate the result with "0x"
coverage_0x87cd0472(0x2c43d5b335fc0edd6598e9d1c3cfa8c506f3c34e2c51029ec6f4bb1b96eecf32); /* line */ 
        coverage_0x87cd0472(0xb468719fd7d905daa96c57f87f003e1837564b687070a88b17c3c681504bd5ee); /* statement */ 
result[0] = byte(uint8(ASCII_ZERO));
coverage_0x87cd0472(0x0c128416fd86cbd79496e8131e5c5c2f236773973f53795e161eb0e5f5514cfb); /* line */ 
        coverage_0x87cd0472(0x95dc0ac8d2d06124a371bcce1562eab84eecfaaaa171ad4f5457fc8a0b237b55); /* statement */ 
result[1] = byte(uint8(ASCII_LOWER_EX));

        // for each byte (starting from the lowest byte), populate the result with two characters
coverage_0x87cd0472(0x9fdd0a37f1bab373b52d6f8940e72e98e555c783babcc6670209f2306a8a0732); /* line */ 
        coverage_0x87cd0472(0xe743b2a1ca4c2bee423f3f923f98da21c2c0fb2a34841132254b43add9a12afa); /* statement */ 
for (uint256 i = 0; i < 20; i++) {
            // each byte takes two characters
coverage_0x87cd0472(0x189c80ad2498647e35bc3ec33151b6e00d96d667de7b5423af43b31f845bdec6); /* line */ 
            coverage_0x87cd0472(0x1c6e31b9d65a69a96b8bd5865bc93401c4b3d2fe1aa99a7ef02fa5ca2263e9b5); /* statement */ 
uint256 shift = i * 2;

            // populate the least-significant character
coverage_0x87cd0472(0x0a4a3e6c0d4fbf2df09863afbbe36f958178790334f8ddd9001bb1978e519512); /* line */ 
            coverage_0x87cd0472(0x6d182295dc9238c3cc416c9c3a2483a21a1078962a259fd0d84925f24b47576a); /* statement */ 
result[41 - shift] = char(z & FOUR_BIT_MASK);
coverage_0x87cd0472(0xc0a30b6fea70b76ccd61d99550831084a285c7cd53687e8cbb19f6eb49374b5b); /* line */ 
            coverage_0x87cd0472(0xf3266927fd2fbb924e2b821bff99b9e3b5b89b1c8381414091102640cc316883); /* statement */ 
z = z >> 4;

            // populate the most-significant character
coverage_0x87cd0472(0x22f1962ad391e67e9e479aada8ab4b0a46fba82c018c53600bc3ca9655186334); /* line */ 
            coverage_0x87cd0472(0x83382d492260a3c7a9aad36c36f9ec5f3683098e5944d758e7e085deefea224b); /* statement */ 
result[40 - shift] = char(z & FOUR_BIT_MASK);
coverage_0x87cd0472(0xbb8562d4797d547f7d49b8c6f34217eb4132aa1ce12e35ffa55f9b38fad1e019); /* line */ 
            coverage_0x87cd0472(0x7cf70ec4362cce341a8dc9d8fe62218da2b88b7cfe178bdce1998312bfd4cf91); /* statement */ 
z = z >> 4;
        }

coverage_0x87cd0472(0xeaebc4e789448d2f96123f1b867f8deed676a7d4f195e0787f4da5189c8127da); /* line */ 
        coverage_0x87cd0472(0x1de6f9497424b492d82e1f98e3c8854200a3fa1671ea253cca0e07c2c5b22ecc); /* statement */ 
return result;
    }

    function stringify(
        bytes32 input
    )
        private
        pure
        returns (bytes memory)
    {coverage_0x87cd0472(0x2064af9ae8781c3651d3ed1bb58149dbfd0be3d65996b1f9addc06b579de36a1); /* function */ 

coverage_0x87cd0472(0x5988eceec0750b38c052b25d2ede3b5e0df2af699d074b9449d8d3f2fbc77703); /* line */ 
        coverage_0x87cd0472(0x8f3e50a7880570194e55f4ee8ca6116a013684799f3cbd108a7cde242ea407e4); /* statement */ 
uint256 z = uint256(input);

        // bytes32 are "0x" followed by 32 bytes of data which take up 2 characters each
coverage_0x87cd0472(0x6a84574e1ee74bc01bbfad6ef2048d51cf6939965b2daf9a4defe79c0e4ef275); /* line */ 
        coverage_0x87cd0472(0xdd905f63774c25b7be1957b48bd4540c8f8904f60f2f68cf5ed0e5f2826942bd); /* statement */ 
bytes memory result = new bytes(66);

        // populate the result with "0x"
coverage_0x87cd0472(0xd54bfc97c49b909e44d8563ed75c93d0f26f7bdf0716b282578e5787c94678f0); /* line */ 
        coverage_0x87cd0472(0x3399cb1cc76919569550f1c1610fe1dae8ddf13db838e7d65622f4e4e8daf1e1); /* statement */ 
result[0] = byte(uint8(ASCII_ZERO));
coverage_0x87cd0472(0x67b2031bb74315c5d0a5fa6dc401b22c3559fc8a9d9d0a8e06265f30c516f601); /* line */ 
        coverage_0x87cd0472(0xa6b05c849fe55fa40d7733be47f6eb973e42f6e212cbf7a8f09e658ec32a54f7); /* statement */ 
result[1] = byte(uint8(ASCII_LOWER_EX));

        // for each byte (starting from the lowest byte), populate the result with two characters
coverage_0x87cd0472(0x7d6ecd875ff3a886d6a50e66cf64271dfa8a156e44cc84f283390c8e6bdfd3c3); /* line */ 
        coverage_0x87cd0472(0x6d89757ec2cc3632f83385ea0c623db5321e3f19ea27bf665ee15d4bc3ba3ca4); /* statement */ 
for (uint256 i = 0; i < 32; i++) {
            // each byte takes two characters
coverage_0x87cd0472(0xb550865236a0f2601f2f9e10040bb864303dc0bdac19e53c92e1700f19ec7d71); /* line */ 
            coverage_0x87cd0472(0x6328f22dad58b13863d0e89a0ffef1d9d3243be80c55d282076de0dcc5bba188); /* statement */ 
uint256 shift = i * 2;

            // populate the least-significant character
coverage_0x87cd0472(0xd8f07a5ae0d8d83461925b557ed8f3a376e2ba027794efb81d8e256929ef9f5a); /* line */ 
            coverage_0x87cd0472(0x41f6f55fea8917999969b1170f9dcb19216193823ccb87ffd2fb736ab3ee3dcb); /* statement */ 
result[65 - shift] = char(z & FOUR_BIT_MASK);
coverage_0x87cd0472(0xfcb699ba430995b1ba3abaa44071cd7cc5aaccfee88dc1caf438cc8f4e88d437); /* line */ 
            coverage_0x87cd0472(0x4db73156e1cfb8c55ba25a884ea1b238c4458b2c8103f14c5815e942650c1953); /* statement */ 
z = z >> 4;

            // populate the most-significant character
coverage_0x87cd0472(0x5cc44752aeb9a0b128b68988a0f80a87fa78ecc592ae73a85f6d2bac2ea33a90); /* line */ 
            coverage_0x87cd0472(0xec05fb0b85ddc1f239b73705afb2b7b2223d501d431f649366b446e7853d5518); /* statement */ 
result[64 - shift] = char(z & FOUR_BIT_MASK);
coverage_0x87cd0472(0x4af8293360492fffd155985593d09e64155d0a4555be43076abecee8cd5bd635); /* line */ 
            coverage_0x87cd0472(0xa4282a0952fc239163e2c2f9921a72bdda1a23b4c5a3905acd877559d5245a8f); /* statement */ 
z = z >> 4;
        }

coverage_0x87cd0472(0x6205d5f3e2a9f94c41c3c37986e83e96e826e9fc83cf8c50a27246268903acfe); /* line */ 
        coverage_0x87cd0472(0xa8291290c8f15ded18e65431982d2882ca3c02c99b821a1dfcd909a51d7cad20); /* statement */ 
return result;
    }

    function char(
        uint256 input
    )
        private
        pure
        returns (byte)
    {coverage_0x87cd0472(0xc4002e8244eaddbbb5df6f52dbc0cb9da722dda33ed76d8e41c2f5668920127a); /* function */ 

        // return ASCII digit (0-9)
coverage_0x87cd0472(0x5b01120e9793b4fc1903bd1102563e537a92d94719b1e8f341fed74c7de37512); /* line */ 
        coverage_0x87cd0472(0x2ab6aef01eab7913d327cbc7a3196d3cd01d9466859054a148903b0da8002647); /* statement */ 
if (input < 10) {coverage_0x87cd0472(0x4904120e6f2cfc88a7e5a50022602cae350383cee8571bfe6540cddaebb88ded); /* branch */ 

coverage_0x87cd0472(0x4d72c771e38f58073775c28aa2adf8b3b75dfdf9f9661c4f3ecb87204ed02711); /* line */ 
            coverage_0x87cd0472(0xac0a2752803d4cecc8cf2f0efaf2117b96db205d72dc0339481dd8c6f34c10c5); /* statement */ 
return byte(uint8(input + ASCII_ZERO));
        }else { coverage_0x87cd0472(0xf8cfac9829274e18d4dd45880ee64b9a599fc6e95671a17cfddd6d08f10646ef); /* branch */ 
}

        // return ASCII letter (a-f)
coverage_0x87cd0472(0x1ec6ffbf5177776a0cee21d0753c39146838d8f7c73db4dd99d210659bc9dcda); /* line */ 
        coverage_0x87cd0472(0x7a39c91814ea8c5b8ab023f95f317775c192b696d4b67619acdedca4231b1755); /* statement */ 
return byte(uint8(input + ASCII_RELATIVE_ZERO));
    }
}
