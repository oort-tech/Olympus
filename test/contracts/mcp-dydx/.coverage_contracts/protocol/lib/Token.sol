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

import { Require } from "./Require.sol";
import { IErc20 } from "../interfaces/IErc20.sol";


/**
 * @title Token
 * @author dYdX
 *
 * This library contains basic functions for interacting with ERC20 tokens. Modified to work with
 * tokens that don't adhere strictly to the ERC20 standard (for example tokens that don't return a
 * boolean value on success).
 */
library Token {
function coverage_0x419d3006(bytes32 c__0x419d3006) public pure {}


    // ============ Constants ============

    bytes32 constant FILE = "Token";

    // ============ Library Functions ============

    function balanceOf(
        address token,
        address owner
    )
        internal
        view
        returns (uint256)
    {coverage_0x419d3006(0x2d0fbcab07ab99e54516e2b5c3dda4adee56ff55cf5d36006be57c4a152294ee); /* function */ 

coverage_0x419d3006(0x13d73e716c24e1815b9e8a47fdfe2dd2801a8468db579cddb3f2205032274c7c); /* line */ 
        coverage_0x419d3006(0x5e3c148844afcf6c714a067599fde704f2d3a2101b45e0b9c0af374beba3c727); /* statement */ 
return IErc20(token).balanceOf(owner);
    }

    function allowance(
        address token,
        address owner,
        address spender
    )
        internal
        view
        returns (uint256)
    {coverage_0x419d3006(0x7206d889bf2b0980459934795342be4b0302d99759b4ce3f6836a85b6320df87); /* function */ 

coverage_0x419d3006(0x8ee93441d545ad6d1585e4224ac38bd258128b34dedaa6809faf4a65f4eec11b); /* line */ 
        coverage_0x419d3006(0x58df30772d3d4522fb7758ad7faea9ba1877707c6b050100f5333a79785f63af); /* statement */ 
return IErc20(token).allowance(owner, spender);
    }

    function approve(
        address token,
        address spender,
        uint256 amount
    )
        internal
    {coverage_0x419d3006(0x5d014a556381551115d3950803f33ab915ae5acdb0bd35ed7905b5ae781f5f9b); /* function */ 

coverage_0x419d3006(0x1146fa0075b37da392bc722b17ff34d4abac7311ceb10b23d998da270dfd142d); /* line */ 
        coverage_0x419d3006(0x908f3e042c990728bdd29a7993cf0bc4052c6a77d2cca9161bc56283c62b7d1c); /* statement */ 
IErc20(token).approve(spender, amount);

coverage_0x419d3006(0x2d279474d100e31989c3d3a36a4f3b83da4fb3f4c67c5d6378faea21bdc44d2a); /* line */ 
        coverage_0x419d3006(0xffd1705d032cd62a8a22205aba08d2fb8810d5939da39208bb5b4d77d3174696); /* statement */ 
Require.that(
            checkSuccess(),
            FILE,
            "Approve failed"
        );
    }

    function approveMax(
        address token,
        address spender
    )
        internal
    {coverage_0x419d3006(0x671237d98984027950dc909a1f68b67ecbb9f4c3c0d983ad4b6913ffeaf7f15e); /* function */ 

coverage_0x419d3006(0xf427c9ecb48b79593606a09989d68f5da40987651e01a4096a961538de678c99); /* line */ 
        coverage_0x419d3006(0xf171fe27a2014b0bc45a5431cb55b24591871dd965357cb98c0b3aa63651981c); /* statement */ 
approve(
            token,
            spender,
            uint256(-1)
        );
    }

    function transfer(
        address token,
        address to,
        uint256 amount
    )
        internal
    {coverage_0x419d3006(0x36d7c82964c929def948f0fff7d7ecb749b2b3478f97e22a42ed02d7c4009fe1); /* function */ 

coverage_0x419d3006(0xb57f6aebed32102f25c4b29e33d189676df7166ec2b602d03c8c500e0c3d248e); /* line */ 
        coverage_0x419d3006(0x56a940266ed98b6083f79594f46a7d67f0a9336ba9578b22b93f70ddb5b7612f); /* statement */ 
if (amount == 0 || to == address(this)) {coverage_0x419d3006(0xdc7bd96d5f6e706e72c3557c384a03723f980c893cbbaaf459087fc38be172e3); /* branch */ 

coverage_0x419d3006(0x97620b01158104ab51aaf472aec1bf9277cb569ae8565386b3c35c99c0dc398a); /* line */ 
            coverage_0x419d3006(0x317601e627f840124b445becc65cb1c3f6a787faa9a38af36322738a0a856ffe); /* statement */ 
return;
        }else { coverage_0x419d3006(0xa7eb282944190a67d3463be9ce5ae72b70b6bcfb4e3e0d5885bd122d0805cea4); /* branch */ 
}

coverage_0x419d3006(0x3cfa857f44742d32b9f1087ed98e44a9f988511f4998178679452f669ad61550); /* line */ 
        coverage_0x419d3006(0x4af1f037500b65c494400c1ed1889ab5c3f3cc762e4908482e8eb6afdcd9e616); /* statement */ 
IErc20(token).transfer(to, amount);

coverage_0x419d3006(0x0c9aa8989c3883c5d5b25c6040a580c6b53307fca5068f3a4f95ef4a15ecf320); /* line */ 
        coverage_0x419d3006(0x8bf2940bcfacd99ce75cf3398ae570a2d846deaf329035d2fd4910fa34001dda); /* statement */ 
Require.that(
            checkSuccess(),
            FILE,
            "Transfer failed"
        );
    }

    function transferFrom(
        address token,
        address from,
        address to,
        uint256 amount
    )
        internal
    {coverage_0x419d3006(0x9d49fc6a4597ac6a098f9e70e004cabc84b3ff2dcf4e648eb4fd7923e01deef6); /* function */ 

coverage_0x419d3006(0x4d3565d3a77871b62f3f37a7bc9b7f2a9b616c220b36dfd41378d1913dc5cd91); /* line */ 
        coverage_0x419d3006(0x6de648230e767a2a0f6d1e8bfe7cf22822e92e631209a16c9980bec0b6761de9); /* statement */ 
if (amount == 0 || to == from) {coverage_0x419d3006(0x4b7b077abd8866d35e3b66a1b02d5b8495b4d5e76fbc37ecee291293820af3a7); /* branch */ 

coverage_0x419d3006(0xb352e7a19275f09a91b2b9ad398711a90f5b7b804612c14b7be79c9c7bf38dd4); /* line */ 
            coverage_0x419d3006(0x7ef34f244855516cc9fc22a5c5c7d1c75f7652a239014ba1c30064e0fd88f74f); /* statement */ 
return;
        }else { coverage_0x419d3006(0xdad723d531074f78602e57846d7cfd9c6a5bb4f218d15b2842eadd84fb7fd937); /* branch */ 
}

coverage_0x419d3006(0xb73a58963d13165de0b64e67c27d935dd1482b7c5e74bb89f2c57882e79b83cf); /* line */ 
        coverage_0x419d3006(0xbeea5faf0a91db8b9fe83500bbec1bca6d7052c717e23f6fbbaebd880892def0); /* statement */ 
IErc20(token).transferFrom(from, to, amount);

coverage_0x419d3006(0x12e12da9b9e5bb9c5d96808ed549b34d74429198c0a124f26aafca95f14c9eda); /* line */ 
        coverage_0x419d3006(0xeda5e988447e8657068f31d626f9e0818dd5a261423e212390aa7cc5e2c5da73); /* statement */ 
Require.that(
            checkSuccess(),
            FILE,
            "TransferFrom failed"
        );
    }

    // ============ Private Functions ============

    /**
     * Check the return value of the previous function up to 32 bytes. Return true if the previous
     * function returned 0 bytes or 32 bytes that are not all-zero.
     */
    function checkSuccess(
    )
        private
        pure
        returns (bool)
    {coverage_0x419d3006(0x72012b92c93fbbb294db35cb9233da6a465ed91d42221bc6fd98a34bbcec3622); /* function */ 

coverage_0x419d3006(0xb565a7c7c3b2fde710eef6eb16eba9e73b9f0bb665419bbcc3e2f0a8a5657468); /* line */ 
        coverage_0x419d3006(0x37197abb1842503ef60cd3fadd3ee47e679c47a3fd448ac435a1fbb338f1a72e); /* statement */ 
uint256 returnValue = 0;

        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x419d3006(0xf174d2dbb65f958168f0ffeae87b138bc887a438058fa60a3a110398d60494a0); /* line */ 
        assembly {
            // check number of bytes returned from last function call
            switch returndatasize

            // no bytes returned: assume success
            case 0x0 {
                returnValue := 1
            }

            // 32 bytes returned: check if non-zero
            case 0x20 {
                // copy 32 bytes into scratch space
                returndatacopy(0x0, 0x0, 0x20)

                // load those bytes into returnValue
                returnValue := mload(0x0)
            }

            // not sure what was returned: don't mark as success
            default { }
        }

coverage_0x419d3006(0x2bd96b10c01390eb1ea785dc0978a4f67c3687fe3132631db53e9b28dd545b43); /* line */ 
        coverage_0x419d3006(0xe32fbb72ef39abcb0673bae1b52f5c57dcb889bd256eec9a87f1f2a268c13dc2); /* statement */ 
return returnValue != 0;
    }
}
