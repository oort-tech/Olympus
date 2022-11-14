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

import { TypedSignature } from "../external/lib/TypedSignature.sol";
import { Math } from "../protocol/lib/Math.sol";
import { Require } from "../protocol/lib/Require.sol";
import { Time } from "../protocol/lib/Time.sol";
import { Token } from "../protocol/lib/Token.sol";
import { Types } from "../protocol/lib/Types.sol";


/**
 * @title TestLib
 * @author dYdX
 *
 * Contract for testing pure library functions
 */
contract TestLib {
function coverage_0x1188f3de(bytes32 c__0x1188f3de) public pure {}


    // ============ Constants ============

    bytes32 constant FILE = "TestLib";

    // ============ TypedSignature Functions ============

    function TypedSignatureRecover(
        bytes32 hash,
        bytes calldata signatureWithType
    )
        external
        pure
        returns (address)
    {coverage_0x1188f3de(0x05625e63e6d5a5f979ca273e959458d71cfe241fdcdbddabf98efb2002e8ccf1); /* function */ 

coverage_0x1188f3de(0xa49e22c962bf8217273562de4d700bdd0baf07f3718e673127b15c2cd7ad3165); /* line */ 
        coverage_0x1188f3de(0x3ee1b57ecb7836c8d252dad190f1bc64783e571abaa0eef1e4e77964c76831fb); /* statement */ 
return TypedSignature.recover(hash, signatureWithType);
    }

    // ============ Math Functions ============

    function MathGetPartial(
        uint256 target,
        uint256 numerator,
        uint256 denominator
    )
        external
        pure
        returns (uint256)
    {coverage_0x1188f3de(0xa8e4e702eb8a4aa25d81a40a79178a5a910a88b64779ab304c5888a25b4dfaf4); /* function */ 

coverage_0x1188f3de(0x894ee0aab10cf0d63c0023d71ef590b2f21266b069431f5c8d57891255ef4b3e); /* line */ 
        coverage_0x1188f3de(0x642a01e88386576821ce22e59450c0e117b9325b81b3b823b057c40348c267db); /* statement */ 
return Math.getPartial(target, numerator, denominator);
    }

    function MathGetPartialRoundUp(
        uint256 target,
        uint256 numerator,
        uint256 denominator
    )
        external
        pure
        returns (uint256)
    {coverage_0x1188f3de(0x33ee6dc09d5585582e3edc874b41f792d67dbcee594191db271b21e24ebf7347); /* function */ 

coverage_0x1188f3de(0x645d1cbfa534a137b6ca3d2c2da6a3429c98da87be151c9f06c0dc751593269c); /* line */ 
        coverage_0x1188f3de(0x0e0b4f3e09288d0cfc2c1f238604dbec24b3c58c099203c92a1eeca12f257926); /* statement */ 
return Math.getPartialRoundUp(target, numerator, denominator);
    }

    function MathTo128(
        uint256 x
    )
        external
        pure
        returns (uint128)
    {coverage_0x1188f3de(0xdaeba512e2245057d3cbfea4f1140aa1705928a834c4f65fdf13c474fe8682b3); /* function */ 

coverage_0x1188f3de(0xf4641ffe5fb24479c6417829443a03e537baccb633f388df49ce3324c9f6f106); /* line */ 
        coverage_0x1188f3de(0xd044aa69e348df5aaea6aff60a6973dc3dfbdf9a00cb2bab25b20b91f0dd7ceb); /* statement */ 
return Math.to128(x);
    }

    function MathTo96(
        uint256 x
    )
        external
        pure
        returns (uint96)
    {coverage_0x1188f3de(0xc6925d0fed60f6e7eafd6632a8d99d3f5f9feb6d511c9f4b36df18eb159ecae5); /* function */ 

coverage_0x1188f3de(0xfb7257d647900702ef6c2bb1cbdbe43805e129673d9061a0f18dfae1da3817a8); /* line */ 
        coverage_0x1188f3de(0x4f67f3bc0228bb3464723fc467e85bf0961c54b62459d972afde554d2a31250a); /* statement */ 
return Math.to96(x);
    }

    function MathTo32(
        uint256 x
    )
        external
        pure
        returns (uint32)
    {coverage_0x1188f3de(0x48f58604f86dc05fe6d17dee182cad351af9e6a0e7d6bb0cbef007c8fbde47c9); /* function */ 

coverage_0x1188f3de(0xc7708bd760fd96fa308c9cc1df5559c55e1e4ffbd3f5241025422131dd71d0ba); /* line */ 
        coverage_0x1188f3de(0x310d705bdfb4f2a24ca2787d9789064aa550ca0894f6999fef268428d2eaf2fb); /* statement */ 
return Math.to32(x);
    }

    // ============ Require Functions ============

    function RequireThat0(
        bytes32 reason
    )
        external
        pure
    {coverage_0x1188f3de(0x8e34a08319c1939fb780cbf1813ec003560d2e67cf8395f3fc130be4061f479c); /* function */ 

coverage_0x1188f3de(0x15386825c379a9dfaac85d05b288b5c9315710fd4e15df05cadb78d3836c6f8e); /* line */ 
        coverage_0x1188f3de(0x38a806bfe6633cf03d855df03843e0f7a605f3c35adfc561e966303053c2359c); /* statement */ 
Require.that(
            false,
            FILE,
            reason
        );
    }

    function RequireThat1(
        bytes32 reason,
        uint256 payloadA
    )
        external
        pure
    {coverage_0x1188f3de(0x1fbcfa2b75118615c462e1e6df3f8ace619eb90913a8989c6c57151f7f895675); /* function */ 

coverage_0x1188f3de(0x4b068ec789055a445ba4654f66654de1e2c621ad5ef83e5039e6d7d8ba6bc963); /* line */ 
        coverage_0x1188f3de(0x3027faa7b31aaf959285f2410a4200d8312f7dffccad1491c5f61911e2a1ffe4); /* statement */ 
Require.that(
            false,
            FILE,
            reason,
            payloadA
        );
    }

    function RequireThat2(
        bytes32 reason,
        uint256 payloadA,
        uint256 payloadB
    )
        external
        pure
    {coverage_0x1188f3de(0xa489fb5a28193a4e62c7d8c9565d84f8b05310677d3da06e3c0bbaf5b12a97ff); /* function */ 

coverage_0x1188f3de(0xbf7c73eac0f329c99f87a3fc0f51f065a589233bea3feee72cc20eea95bf6e03); /* line */ 
        coverage_0x1188f3de(0xa3a5474e1da23545e1fcde8591fb2e3baa9e88157bdc6ff6f2899c85b8392548); /* statement */ 
Require.that(
            false,
            FILE,
            reason,
            payloadA,
            payloadB
        );
    }

    function RequireThatA0(
        bytes32 reason,
        address payloadA
    )
        external
        pure
    {coverage_0x1188f3de(0x7d0b71be195d4a2851933dbf39c15e4bd57d3e06b70986cad3f28cde7ffa7c38); /* function */ 

coverage_0x1188f3de(0x9dbe54a62f133c79f897cb2308911dfc0d85fc7807f3d63a1a888d90dffa2ebd); /* line */ 
        coverage_0x1188f3de(0x409794c0db2d0a2a1c080667ca9b008ad68e8e6610b8e1ffe8a0992ce9714f70); /* statement */ 
Require.that(
            false,
            FILE,
            reason,
            payloadA
        );
    }

    function RequireThatA1(
        bytes32 reason,
        address payloadA,
        uint256 payloadB
    )
        external
        pure
    {coverage_0x1188f3de(0x1453ca30de03eed3cdc75aca2c9ebfde26fc1cf2e2df4817decf55d983eb8bb1); /* function */ 

coverage_0x1188f3de(0x48ab3f13cd0f50abe7c7b073b18c95ebc18adb5f333304745a23be6666f3b4d3); /* line */ 
        coverage_0x1188f3de(0x0fffb3aa9738c9f228071d6715360142a085b5731b1aedfcda1a86f0a62cb0f0); /* statement */ 
Require.that(
            false,
            FILE,
            reason,
            payloadA,
            payloadB
        );
    }

    function RequireThatA2(
        bytes32 reason,
        address payloadA,
        uint256 payloadB,
        uint256 payloadC
    )
        external
        pure
    {coverage_0x1188f3de(0x0ccc54abb1a10b02dc6ef7d8c2680754df8bc1acb24ab1a5a13c51f2e9f3d1db); /* function */ 

coverage_0x1188f3de(0x55ba28807857d7b9a28ffcbd32562cafa74ab44e9d10710fc686c8257ba0b39e); /* line */ 
        coverage_0x1188f3de(0xc828fd67b1cc182a340112f2795d79365e29f8d3453c887c5b13e566c309d232); /* statement */ 
Require.that(
            false,
            FILE,
            reason,
            payloadA,
            payloadB,
            payloadC
        );
    }

    function RequireThatB0(
        bytes32 reason,
        bytes32 payloadA
    )
        external
        pure
    {coverage_0x1188f3de(0x5771f9bbfd3f9bd6b69ce307e5d7dfd9289cc81fe22c19016f6f80b32e15dd33); /* function */ 

coverage_0x1188f3de(0x89341fa3c9e7f87ab058c736641484964001f802cdfe0414fd0bbccd33cad509); /* line */ 
        coverage_0x1188f3de(0xd8684c902f72e4c3be24ba598e237386af33de1754fc2cae882597a518ac8978); /* statement */ 
Require.that(
            false,
            FILE,
            reason,
            payloadA
        );
    }

    function RequireThatB2(
        bytes32 reason,
        bytes32 payloadA,
        uint256 payloadB,
        uint256 payloadC
    )
        external
        pure
    {coverage_0x1188f3de(0x7cceddad55b1fa914606bb13775ecc9e5d410753e351673ee2d2c35981e4b72f); /* function */ 

coverage_0x1188f3de(0xd64c525d16f7b8bbdf661f023b1437d90d5e41119b6c88b8b24b8bd6f3049533); /* line */ 
        coverage_0x1188f3de(0xc7b5851e8c731d2fe394f2a7d5486f8f194a4700cbf2b52e19a4278b226ba431); /* statement */ 
Require.that(
            false,
            FILE,
            reason,
            payloadA,
            payloadB,
            payloadC
        );
    }

    // ============ Time Functions ============

    function TimeCurrentTime()
        external
        view
        returns (uint32)
    {coverage_0x1188f3de(0x33f5ba66888d4bebf4571cb6a0de0dea4f83b9990dba4a01e95e103f7023ac7f); /* function */ 

coverage_0x1188f3de(0x3b7e73db5f309fedac7bd6e484fc9dbd81a5903a5c3fd9356424a58f97180774); /* line */ 
        coverage_0x1188f3de(0x369a0773958f14cb011de31c041e98e803bd8950dff6f6b9598de8255ba16ec6); /* statement */ 
return Time.currentTime();
    }

    // ============ Token Functions ============

    function TokenBalanceOf(
        address token,
        address owner
    )
        external
        view
        returns (uint256)
    {coverage_0x1188f3de(0x57240a31cb0a2fbf94dddf957d8bf4b91c90bffc97ef06e5e8778af0248aacc5); /* function */ 

coverage_0x1188f3de(0xc05782c6a8f0f61a3cd94fb5c5cbca93ab025442dd023dfe572474a971739743); /* line */ 
        coverage_0x1188f3de(0x187807fe904c42126a8500ac8a62bfc85d845e63d305b871c65904bf6686f105); /* statement */ 
return Token.balanceOf(token, owner);
    }

    function TokenAllowance(
        address token,
        address owner,
        address spender
    )
        external
        view
        returns (uint256)
    {coverage_0x1188f3de(0x94205f6fcdc08700d04147bc476ddd5797d8f2af8b83c7d8fe96285fa96713c4); /* function */ 

coverage_0x1188f3de(0xca83edbeaf9e5c4d70deef0468e103f2caa7953d1dd89f29cea6ff384d1a33c3); /* line */ 
        coverage_0x1188f3de(0x9963a5ff19ecf5c462098e99d36a067abf263823251b18073a6b9a82576fd8bd); /* statement */ 
return Token.allowance(token, owner, spender);
    }

    function TokenApprove(
        address token,
        address spender,
        uint256 amount
    )
        external
    {coverage_0x1188f3de(0xc4c149461578de34e6b5d8da2ebbb689862e3c580d50a4bee043ab51a11799d6); /* function */ 

coverage_0x1188f3de(0x39e95d1aa5cda79c1951a86f2a718230a378ef618dc0ee167356747b008aa360); /* line */ 
        coverage_0x1188f3de(0xd5874807fbb79e7b928ad959867d87349599b24f23ae0f8f361000daf85dc97a); /* statement */ 
Token.approve(token, spender, amount);
    }

    function TokenApproveMax(
        address token,
        address spender
    )
        external
    {coverage_0x1188f3de(0x9fa147f6776889c2f2efe1d577fdd571f7c9957559c3ff2d2a0cdf4aa27a3614); /* function */ 

coverage_0x1188f3de(0x7cbedb8a0d642f7c1e70d92dbf78549af5892cadd9d5b9040891fb8be42f63fa); /* line */ 
        coverage_0x1188f3de(0xead4255a01e480fbae06cecc0d36f0516a20fa88c8e60299211a61a40a7282b5); /* statement */ 
Token.approveMax(token, spender);
    }

    function TokenTransfer(
        address token,
        address to,
        uint256 amount
    )
        external
    {coverage_0x1188f3de(0xc0dbe336cc829128df49cd6fdfe9fdc3144a4e91ac6678e5c6f441fcbae96156); /* function */ 

coverage_0x1188f3de(0x89760689e024aa5e185260b9c061aaffab219245f6cae194bba275d330d58a6f); /* line */ 
        coverage_0x1188f3de(0x4d4061aaa77b2b5cc0925714bf2c6e2abb759db79fd2af8676f16e70c0d24e3a); /* statement */ 
Token.transfer(token, to, amount);
    }

    function TokenTransferFrom(
        address token,
        address from,
        address to,
        uint256 amount
    )
        external
    {coverage_0x1188f3de(0xbe4f91af2bbf43bcc359af2e1edb6b54497a9ccaa6f4de35cbbf8d6866692eec); /* function */ 

coverage_0x1188f3de(0xa088d05e40b88944dc634882c95ac1d80b040af75baa33192f5a8091585ef8ac); /* line */ 
        coverage_0x1188f3de(0x4284f9b5cf674468c48ab4108d3de77081b13298404101ac081d1cdbe38ebac8); /* statement */ 
Token.transferFrom(
            token,
            from,
            to,
            amount
        );
    }

    // ============ Types Functions ============

    function TypesZeroPar()
        external
        pure
        returns (Types.Par memory)
    {coverage_0x1188f3de(0x6b9cb095db0aeede93e74c1cec819e0949d2095645b47cbe700951f488548039); /* function */ 

coverage_0x1188f3de(0x009434ce10cabdaa4613e6058862941f233c0fcefe7ec15c6b050ea434e366be); /* line */ 
        coverage_0x1188f3de(0xd813ae4e4bcc9507cbcaeb9e2bbaab0646731dfa2c441204f3303fb32c3b7fb4); /* statement */ 
return Types.zeroPar();
    }

    function TypesParSub(
        Types.Par memory a,
        Types.Par memory b
    )
        public
        pure
        returns (Types.Par memory)
    {coverage_0x1188f3de(0x9e4277e462550f8f59414843fa96e0cb3af95b2fe8f3d3bacf1cc627bb7c9367); /* function */ 

coverage_0x1188f3de(0xa90ca0a30367b24b376520add853b21bb65844b05de449363e7d3fc90ce4af3e); /* line */ 
        coverage_0x1188f3de(0xaa7da6fd76eb89c939e7024c528201529448f1a09a6514d5cb59ac639b7d96a3); /* statement */ 
return Types.sub(a, b);
    }

    function TypesParAdd(
        Types.Par memory a,
        Types.Par memory b
    )
        public
        pure
        returns (Types.Par memory)
    {coverage_0x1188f3de(0xeba742e7ad9ec1804dbdb11b4294bc03890100ad22757ecffc39e0ed499340f2); /* function */ 

coverage_0x1188f3de(0x47ae24de72fc70f785f0df222bd265ef28e1345a491fb3a2610604e354490f55); /* line */ 
        coverage_0x1188f3de(0x31e634926611eb7404c020ffe8f63a3444d0a7cdbecde33905816d34658ee728); /* statement */ 
return Types.add(a, b);
    }

    function TypesParEquals(
        Types.Par memory a,
        Types.Par memory b
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0x94b05e76bb214dd0396eb3a0649f8d4186c777690645dcd112d42456caadbb77); /* function */ 

coverage_0x1188f3de(0x9819d4944a3865aeed3eda4edfc4b1189f29ec264756cf10f4daace6abd67750); /* line */ 
        coverage_0x1188f3de(0xb2c3cda13ab0545b19af75edcf6005f87cedfb6e1e7c49b04985eaa89b321846); /* statement */ 
return Types.equals(a, b);
    }

    function TypesParNegative(
        Types.Par memory a
    )
        public
        pure
        returns (Types.Par memory)
    {coverage_0x1188f3de(0x64784c7102b0c1c8e453bf3629de819725b759be1267088e1659b3d04c263a2e); /* function */ 

coverage_0x1188f3de(0xd7754dad03768a274f63e844c6dc65538dbaccbd9acf5d6c2e7439e7094d1f2d); /* line */ 
        coverage_0x1188f3de(0xb3cb80e7fa2634d4aa72f4aab261960d0172e4f07fce8ada7782044eeaecb636); /* statement */ 
return Types.negative(a);
    }

    function TypesParIsNegative(
        Types.Par memory a
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0xe6bb1b6e8a85804f9450a40090a4bb73bfc0853d151976593f9418a0e90cedc3); /* function */ 

coverage_0x1188f3de(0xbf8aab2d2b072c77668e2ae20049001df354fa8d85021a8824973db2dcd32d55); /* line */ 
        coverage_0x1188f3de(0xa1d31bbff3ad8c565e8be2f3b5a1535c34671cce58d5d64483db6b9191ac0d37); /* statement */ 
return Types.isNegative(a);
    }

    function TypesParIsPositive(
        Types.Par memory a
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0x5970f8b07a2efccc98669e08884ce938383b8441e1391cb6cfb6ba5915596db4); /* function */ 

coverage_0x1188f3de(0xe17ffd29c4217f89d0ef7a11535f2a76bce54bd2c0681c9eb2cf7ce4ccff5040); /* line */ 
        coverage_0x1188f3de(0x65c7ceb0a2bd61e117deddbabcbc164c0675d26c9740c6d132bd74f54aaf444a); /* statement */ 
return Types.isPositive(a);
    }

    function TypesParIsZero(
        Types.Par memory a
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0xc73c7f03e786748f036615a79ff8a46a83ddb5965695dd5b0c79644772840be1); /* function */ 

coverage_0x1188f3de(0x15fb2a735edef73ccef79a337d1002e3a139e5eeedeb28d67e8f0b9d8bcf734b); /* line */ 
        coverage_0x1188f3de(0x314725cdeed9007ca405538b244ec6853883448cf450060397f72d55072c923d); /* statement */ 
return Types.isZero(a);
    }

    function TypesZeroWei()
        external
        pure
        returns (Types.Wei memory)
    {coverage_0x1188f3de(0xaa060b4726df08a995082a03adad2e6acd4304616850e3f6b9975c340adae1b2); /* function */ 

coverage_0x1188f3de(0x9ea4879f95d8d3f79f32549a7002949219c67180fabd2ffd83b998276598feb1); /* line */ 
        coverage_0x1188f3de(0xa23c0930f3159d71af2268b155b5445bba85f2f75865a9c9db19e4ebf0ac8210); /* statement */ 
return Types.zeroWei();
    }

    function TypesWeiSub(
        Types.Wei memory a,
        Types.Wei memory b
    )
        public
        pure
        returns (Types.Wei memory)
    {coverage_0x1188f3de(0x42530eb559591185b3ab76c07794e3a528b486e96c0b2ff52579ef234186fd60); /* function */ 

coverage_0x1188f3de(0xfd3a03fbab963692256dad0f4af2ffe758a205bc4e6f499736476eea84973bfd); /* line */ 
        coverage_0x1188f3de(0x71eaa592ca732f367c802253b82adba489a0c047818f766ad5df848fa561061d); /* statement */ 
return Types.sub(a, b);
    }

    function TypesWeiAdd(
        Types.Wei memory a,
        Types.Wei memory b
    )
        public
        pure
        returns (Types.Wei memory)
    {coverage_0x1188f3de(0x925008fa59f89b41f8867fbd85d3a546dfc44852e638cb3059084871ddd78c6f); /* function */ 

coverage_0x1188f3de(0x7e924cd509024eaa0c0fa6e57306d28a56b577b3eaa7cac075fcb0f8295817e1); /* line */ 
        coverage_0x1188f3de(0x0761e608a1daf00b810cc178d0dbe30d87a165a39f5c26164572d4e1fc7a1d2d); /* statement */ 
return Types.add(a, b);
    }

    function TypesWeiEquals(
        Types.Wei memory a,
        Types.Wei memory b
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0x4c587c1c89c26b2065d53f8a4c43f2da778245e1098407c01188d44ec8d52f68); /* function */ 

coverage_0x1188f3de(0xb4ce3d3831f18209da7b05c01e1afdd6d04ec10704920a95e3314c73b26f2fc9); /* line */ 
        coverage_0x1188f3de(0xec208c1516cf92177ca4dcb777983171324ff5380a0afb11caff5cfe54871a5d); /* statement */ 
return Types.equals(a, b);
    }

    function TypesWeiNegative(
        Types.Wei memory a
    )
        public
        pure
        returns (Types.Wei memory)
    {coverage_0x1188f3de(0x62812336640733946fe69aa7d766f81e18f48587fc46bf7b9ff690a5c2a5b132); /* function */ 

coverage_0x1188f3de(0x460f9f8df0b137c741df9bf344d8cefddea0405e7e795be572c0d9586234c849); /* line */ 
        coverage_0x1188f3de(0x562edf6f4a6c2f2d2e4b4731b2b4c491255e96c07400e771acd19081d857e888); /* statement */ 
return Types.negative(a);
    }

    function TypesWeiIsNegative(
        Types.Wei memory a
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0xfdb72b894d7598d26e7b3f6f21fda49271fa2cff63d1961008f0cc708638857e); /* function */ 

coverage_0x1188f3de(0xf97d19b80b3d52964a04113a3e6257b659e1ea6a7616de7ee4c7d4b9f6b7d353); /* line */ 
        coverage_0x1188f3de(0x497bc11110c9ddc0dd149ab6aa9df42b836795be648ee115ec1686275ff912a1); /* statement */ 
return Types.isNegative(a);
    }

    function TypesWeiIsPositive(
        Types.Wei memory a
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0x429dad9ff0e49df52e4e800efed1dc4e2d5f6ca94292e3ce3fe39fccffbe27ff); /* function */ 

coverage_0x1188f3de(0x77c070e920a48d4d106ef5922fa27f4c5cbb18db08dbba6be5f86544964fb707); /* line */ 
        coverage_0x1188f3de(0x3a9d847cd5fc39bbccbd0428b86807fd044549564b1503085a0c3ffb899dcd21); /* statement */ 
return Types.isPositive(a);
    }

    function TypesWeiIsZero(
        Types.Wei memory a
    )
        public
        pure
        returns (bool)
    {coverage_0x1188f3de(0xa8bdd40759e8d13f2fa75873eb4dec22bf3f19a61939268c48be9fa1b6f38e37); /* function */ 

coverage_0x1188f3de(0xf886049b1cb82c76ebaee4e51518f9c132e6b051a7023422ef379e19fafff61a); /* line */ 
        coverage_0x1188f3de(0x3686dc2cdcf36aaae7d54700d8a34edbedc4d0d6860f8b1ce8eeafa04f3415a0); /* statement */ 
return Types.isZero(a);
    }
}
