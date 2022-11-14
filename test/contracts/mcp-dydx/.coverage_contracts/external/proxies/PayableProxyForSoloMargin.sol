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

import { WETH9 } from "canonical-weth/contracts/WETH9.sol";
import { ReentrancyGuard } from "openzeppelin-solidity/contracts/utils/ReentrancyGuard.sol";
import { SoloMargin } from "../../protocol/SoloMargin.sol";
import { Account } from "../../protocol/lib/Account.sol";
import { Actions } from "../../protocol/lib/Actions.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";


/**
 * @title PayableProxyForSoloMargin
 * @author dYdX
 *
 * Contract for wrapping/unwrapping ETH before/after interacting with Solo
 */
contract PayableProxyForSoloMargin is
    OnlySolo,
    ReentrancyGuard
{
function coverage_0x09d7c9db(bytes32 c__0x09d7c9db) public pure {}

    // ============ Constants ============

    bytes32 constant FILE = "PayableProxyForSoloMargin";

    // ============ Storage ============

    WETH9 public WETH;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        address payable weth
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x09d7c9db(0x9c048433a9e7660d7a5811d04e7c61087732477a4a6b7d7217f451759fc236b1); /* function */ 

coverage_0x09d7c9db(0x8626a9294163f236edff7d0e122e666b1b1d9cab0774ac35e55c5f8ba1e53d85); /* line */ 
        coverage_0x09d7c9db(0xde322f21f36de0b038db437c949d7ef9fab5fa51670a455276cb55bdff152bf8); /* statement */ 
WETH = WETH9(weth);
coverage_0x09d7c9db(0x5ec436bdebeb6b4d8b3a794d56b01a08e88f58fba7f70517fe8d315594a55539); /* line */ 
        coverage_0x09d7c9db(0xf4e1726aa92a4a8947c83b19f4bf37825fdc724c8f5d4d9adc684279cb3ceeef); /* statement */ 
WETH.approve(soloMargin, uint256(-1));
    }

    // ============ Public Functions ============

    /**
     * Fallback function. Disallows ether to be sent to this contract without data except when
     * unwrapping WETH.
     */
    function ()
        external
        payable
    {coverage_0x09d7c9db(0x32af0e10e1a62b84af2e91735e6ec00dcbbb946b019d5a1cda2ab5c4f05acd91); /* function */ 

coverage_0x09d7c9db(0x16330f7e59a07bf9a3383f4460e1eab2baec307e225e97601274642972c313ef); /* line */ 
        coverage_0x09d7c9db(0x1311d88b7853bdd1937383def472c2baadb4d882298fb7f3896b95ec2c8f235b); /* assertPre */ 
coverage_0x09d7c9db(0x5dbdd1458676785c87f460413e1917bf6c527c6b4a4d6422d183b790e7db6fe4); /* statement */ 
require( // coverage-disable-line
            msg.sender == address(WETH),
            "Cannot receive ETH"
        );coverage_0x09d7c9db(0x97ed72aae7a66e9e733d762e0397f8653666d2320ce4ffefee0e918acf4c0f02); /* assertPost */ 

    }

    function operate(
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions,
        address payable sendEthTo
    )
        public
        payable
        nonReentrant
    {coverage_0x09d7c9db(0x1de7e88cf7e0d7ba226b20e7be9675aa848d86fee3593b9a09f93a8c4ebe6dfd); /* function */ 

coverage_0x09d7c9db(0x8d2e8244501dc9bd4d38084a00726a5a72203296ae67c3a8fa49b7d38daa35de); /* line */ 
        coverage_0x09d7c9db(0x6881ddd6fa3e343a97c7f67c45cddef035c16dd44ae337c2fb1316165aba3497); /* statement */ 
WETH9 weth = WETH;

        // create WETH from ETH
coverage_0x09d7c9db(0x68ddfa7d4a5ddf6becd50afbcbc8c024c7225c861b421ca2c3541a90f3604c02); /* line */ 
        coverage_0x09d7c9db(0x14df53aae9937cf3b944473a89b9190d3d204170d436656d350c1a821f1da38f); /* statement */ 
if (msg.value != 0) {coverage_0x09d7c9db(0x666972eaf5fcd8c35153283f8800d87a7f64d278e742804c5afbfa41041eecee); /* branch */ 

coverage_0x09d7c9db(0x9cea92d5d229fdc8c6227bdc546d77a230f908765cdff69e1c47d876fbef5cc4); /* line */ 
            coverage_0x09d7c9db(0x9069bbe21ac3cd8534fe93049e8d10a9a94fb7273b7fb7098599d4c1b7a08f1a); /* statement */ 
weth.deposit.value(msg.value)();
        }else { coverage_0x09d7c9db(0x620a0e04e06e724bf3b398311ba26495780834d040bb1060ca4c3e91a6d06978); /* branch */ 
}

        // validate the input
coverage_0x09d7c9db(0xcb8add0ed72d64cadfbde6adaa42196233007b1d137d1751f50e940f234f6801); /* line */ 
        coverage_0x09d7c9db(0x20500380678025823e1f8450ba59c54d73e3f4a95f51370a3c2f484de01db1d0); /* statement */ 
for (uint256 i = 0; i < actions.length; i++) {
coverage_0x09d7c9db(0x5ad1cdcfed6a4dd1b0715fe197dadde1ad43f71a9ae5e2ed524d59877c2bbae7); /* line */ 
            coverage_0x09d7c9db(0xb3e6b3b89871d16354f5cc2e94198fdc2017b5031f32e8986460b3cde7efacc4); /* statement */ 
Actions.ActionArgs memory action = actions[i];

            // Can only operate on accounts owned by msg.sender
coverage_0x09d7c9db(0x78f5d9e075681513e07a606e758bc3e7ffadef36f17b485ba459d22a871818c4); /* line */ 
            coverage_0x09d7c9db(0x98c46ff203658f5e58694c22bb4a3484471eeb92e289600718606e809efa90eb); /* statement */ 
address owner1 = accounts[action.accountId].owner;
coverage_0x09d7c9db(0xcfea8ef8943cf7838e38040222f298e42708cff53698fa6bf7e8cda938df6d8e); /* line */ 
            coverage_0x09d7c9db(0xd1cd2dcd822872a923d2869a1486373fa490e105c260bb74202ea9407ba91eaf); /* statement */ 
Require.that(
                owner1 == msg.sender,
                FILE,
                "Sender must be primary account",
                owner1
            );

            // For a transfer both accounts must be owned by msg.sender
coverage_0x09d7c9db(0xa5a04bfe70c198b638e4537ccedbb81cf0fcc4772992b674b8ea6e206d01a5df); /* line */ 
            coverage_0x09d7c9db(0xe6c864ce5b0415b20d6efbdade8fa13ca3813a01727000cfdbedede41e0ca321); /* statement */ 
if (action.actionType == Actions.ActionType.Transfer) {coverage_0x09d7c9db(0xa916dca198e243753c5400c8ae6dee9d94ef6e942311d6123cc67614cc52c15a); /* branch */ 

coverage_0x09d7c9db(0x328ca7e8834112e4828ab518d517367021c41821a4939378cc09f049e0ac076d); /* line */ 
                coverage_0x09d7c9db(0x5754e1bd352a2537215edb5e9a931952bab343da509c461a8d3a23ab16ef0428); /* statement */ 
address owner2 = accounts[action.otherAccountId].owner;
coverage_0x09d7c9db(0xab101ac8dfd694d777c303c15dd62e7aa2b1be654ca8c6894fe164ce7b0af8ad); /* line */ 
                coverage_0x09d7c9db(0x62d84cf750fb702cda76f3d303fea1512948a41b3c8cc4fede31419e361779df); /* statement */ 
Require.that(
                    owner2 == msg.sender,
                    FILE,
                    "Sender must be secondary account",
                    owner2
                );
            }else { coverage_0x09d7c9db(0x27d2a604e7e776c39974f6911e3849dcf26b8e253b43c09bb32181d07bb094eb); /* branch */ 
}
        }

coverage_0x09d7c9db(0x427093864184bb87b706a70f7d26e4b97a4b4caf26e48da7b7015d0f6ebbbe75); /* line */ 
        coverage_0x09d7c9db(0x4ecb6a8b8e9106d73c627544ecf16949309dc995cccd2f7e2422bf2144f6d74d); /* statement */ 
SOLO_MARGIN.operate(accounts, actions);

        // return all remaining WETH to the sendEthTo as ETH
coverage_0x09d7c9db(0x9135b8115c65a838ad9e0e345ff846d8097c5cd77be00afc07d525b5723ea637); /* line */ 
        coverage_0x09d7c9db(0x125545e8a21da18dd03ddc032f49707e953d63770416c3ef675a847fc2da51a1); /* statement */ 
uint256 remainingWeth = weth.balanceOf(address(this));
coverage_0x09d7c9db(0x81fe6a9caf55bdd52847ab7545dd033619e589a6bbcd2fbadd91a21f4800f979); /* line */ 
        coverage_0x09d7c9db(0xd44f0e9cea79ce4940d2a407591df9895b844d9e26683f39044166e957b30bfd); /* statement */ 
if (remainingWeth != 0) {coverage_0x09d7c9db(0x3c8cfe042e35b505da89acdd0b762a004215ae0fc5139a6fdbfde1db4d4b0f84); /* branch */ 

coverage_0x09d7c9db(0xa63316bdf006e6cdad93e43d9944fe70aebc18f0ffc55dda2525fa7830068cb3); /* line */ 
            coverage_0x09d7c9db(0xd6204b6662098446c4a4a2fa3f40fbfbf37728b233f1cced2c8fcaade6a4b800); /* statement */ 
Require.that(
                sendEthTo != address(0),
                FILE,
                "Must set sendEthTo"
            );

coverage_0x09d7c9db(0xf34098ec6c7d90ac6de0ec279b0ffea0739cd694e61c09ff11854731f9c8d688); /* line */ 
            coverage_0x09d7c9db(0xb23881dee95e0621099e9aaab4cab6b49ecff7fbd32e74c47634b84cfb459bb3); /* statement */ 
weth.withdraw(remainingWeth);
coverage_0x09d7c9db(0x0f15d815a1d1b899abe9edd4dd0ae88c1269a0487d5d9e209ddc5f9d26dd3ea0); /* line */ 
            coverage_0x09d7c9db(0x7b1c4b10ad246d9eae4964a5f3368fe819d90833f8751ae1ea07fe2dfbbd4b3e); /* statement */ 
sendEthTo.transfer(remainingWeth);
        }else { coverage_0x09d7c9db(0x0437865aef8d100788a4a85a77af6c151438103d1697b38c54504921ef306f68); /* branch */ 
}
    }
}
