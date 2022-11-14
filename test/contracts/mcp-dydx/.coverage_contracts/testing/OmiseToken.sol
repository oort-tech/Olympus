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


contract OmiseToken {
function coverage_0x67b8af4b(bytes32 c__0x67b8af4b) public pure {}

    using SafeMath for uint256;

    uint256 supply;
    mapping (address => uint256) balances;
    mapping (address => mapping (address => uint256)) allowed;

    event Transfer(address token, address from, address to, uint256 value);
    event Approval(address token, address owner, address spender, uint256 value);
    event Issue(address token, address owner, uint256 value);

    // Allow anyone to get new token
    function issue(uint256 amount) public {coverage_0x67b8af4b(0x430b5d57520393438caa2eb56e7f073f6d3da30707efa2169c23fd804c11b55d); /* function */ 

coverage_0x67b8af4b(0xc7df8ccf8a520c59cb01f7d5d58b0a24c5445df916138af45870a4ad1ea352df); /* line */ 
        coverage_0x67b8af4b(0xdeda26045b3550c02fd10b808adc9056f7dda20022b9c8549b2da41807e6067e); /* statement */ 
issueTo(msg.sender, amount);
    }

    function issueTo(address who, uint256 amount) public {coverage_0x67b8af4b(0x71319df9baa9c1514ef74920b935d74479c66bfd1054add41f56fa3e094222e8); /* function */ 

coverage_0x67b8af4b(0xae192fc488adf5293f842f550dc2091c07f97cfd370429bc30a87e09248ae020); /* line */ 
        coverage_0x67b8af4b(0x2843120b819ae385c17cb36d017a23992886e77700bfb32834e8c19a3b3f6dbb); /* statement */ 
supply = supply.add(amount);
coverage_0x67b8af4b(0xc8ed0e2118ff98194b7591f683cba324d971ab6387ea6067addbd1216e34ab96); /* line */ 
        coverage_0x67b8af4b(0x1fc53d8623b527e74ff7cbdabf45d8c76238187b7e7fc8cb9c3f3c4d4907e80c); /* statement */ 
balances[who] = balances[who].add(amount);
coverage_0x67b8af4b(0x40684b0b47271ced48a07054556d0a1d41452fe5539ed9b81bbb382d17d06ef3); /* line */ 
        coverage_0x67b8af4b(0x7aa49e272f6b384dff4c3a2554c4fef4bf43ade0c2053e5b512cdeac8fecba5e); /* statement */ 
emit Issue(address(this), who, amount);
    }

    function totalSupply() public view returns (uint256) {coverage_0x67b8af4b(0x3bcf4ff6887a58d5bd6ee3978dc2c23907b38cb7146677972e7a02ecb55d9ed1); /* function */ 

coverage_0x67b8af4b(0x3cb803beaf06fac386d1294050821484c3005e39e7c1f54abd1c8d7fbd2d32c1); /* line */ 
        coverage_0x67b8af4b(0x7dec89d628cbcb9980a3b24eebafddf7c939988fd2c83945141b863f8435d063); /* statement */ 
return supply;
    }

    function balanceOf(address who) public view returns (uint256) {coverage_0x67b8af4b(0xaadaf0106c2d28820c7aa7d3ed29567458bb29f2a8dbbb430a8e0fe3bb7b064f); /* function */ 

coverage_0x67b8af4b(0x789a64dbd85d6276b4d6e8ff1213b16761a516eb3b6e7e9cb348e6364be56acf); /* line */ 
        coverage_0x67b8af4b(0xbd0c4eae97145c2fe2453cc16d8a1196361fbe098a601b4e582394e30a48f1cc); /* statement */ 
return balances[who];
    }

    function allowance(address owner, address spender) public view returns (uint256) {coverage_0x67b8af4b(0x5bf44cc684349dd6e0e9f98ea28f88140fce007626c12fec1dd12bb3bc35c35b); /* function */ 

coverage_0x67b8af4b(0x4e048db6db4b69b11703f91fc2a11a4c659b2f8b211938ea8d1dea715611ba6e); /* line */ 
        coverage_0x67b8af4b(0xf747c1e6c8073b9f39fcf9c58f99bbcdcd96d4a66bb805051819f07828d4c617); /* statement */ 
return allowed[owner][spender];
    }

    function symbol() public pure returns (string memory) {coverage_0x67b8af4b(0x043b22c24ccb3b0aac4dc724b1e56a7bdd2e91addb82b11c5be46c2bff066635); /* function */ 

coverage_0x67b8af4b(0xab050f8b493b28c540278714042a8e8debef05d567eb31db0428567b159ac981); /* line */ 
        coverage_0x67b8af4b(0x088e9cc55c093106995e328197bef511d05093f9ac2b5774818d67f62b0de81a); /* statement */ 
return "TOMG";
    }

    function name() public pure returns (string memory) {coverage_0x67b8af4b(0xc3dae33b5f742b9a4390e157787cf4332f54344bc44eb56dbc3999cf15571b7c); /* function */ 

coverage_0x67b8af4b(0xd843380848e4937b2ef230d2bba8402e9ee9feca85f75f9e1327404581aa6c80); /* line */ 
        coverage_0x67b8af4b(0x5ad4164e867db4141c68a3afb18782145db3e5c081ad46434fb893de1e7ba0e9); /* statement */ 
return "Test Omise";
    }

    function decimals() public pure returns (uint8) {coverage_0x67b8af4b(0x251a1032e7b9a2a05feb9ceb814ebb1cde4d921bf3e651b308d986df0f2d4008); /* function */ 

coverage_0x67b8af4b(0x6991b5b82aa672b9b70646aa9a39b8965bb1231637ef55d8e2137356e5da6925); /* line */ 
        coverage_0x67b8af4b(0x625f82807d34f55e88aa32632d34c74e590049f2cc869bbf58764283563f6947); /* statement */ 
return 18;
    }

    function transfer(address to, uint256 value) public {coverage_0x67b8af4b(0x416a69d7020c4d67a31f221edd85cd814e404d83c9845ac40a9740ea339d41e6); /* function */ 

coverage_0x67b8af4b(0x38bd81ffc95ad69c7612d8a271d594266689c9a0f8f8a7d0e1b185f1007f28d2); /* line */ 
        coverage_0x67b8af4b(0x52307b03d4fba38c4e2a81efeba5c939cf6e0c086c6d847173e1b61cc3b69c32); /* assertPre */ 
coverage_0x67b8af4b(0x0d5f6533ba87bec17a4951455799eeb44d790097945dc8771a00edcf4c861148); /* statement */ 
require(balances[msg.sender] >= value);coverage_0x67b8af4b(0xe74b120b04d33b56930c580fc7f1b8af88f5c92c10141238f8b0e8bd05bd7dae); /* assertPost */ 


coverage_0x67b8af4b(0x37a9d984cbfdde08344bdadb16f8eb19d48f925d08b032371b24efd3db5dba29); /* line */ 
        coverage_0x67b8af4b(0xa36a82e300744565fe3a21988556ff173229a3ab67ea4cabb5d924b0c83f0112); /* statement */ 
balances[msg.sender] -= value;
coverage_0x67b8af4b(0x8a3775ab93677704b768bbd588827709dc72b33be384bbed4e4e6efc859dfb8b); /* line */ 
        coverage_0x67b8af4b(0x06b68ec1dd9e3c5258536114341515d859dac58edd8bd55f806477e5d4a93ecb); /* statement */ 
balances[to] = balances[to].add(value);
coverage_0x67b8af4b(0xb135d4669aa5454a2151ee2eeedea84a23a036c0e66efc6ecd964cd1b14dccb7); /* line */ 
        coverage_0x67b8af4b(0x0e432ee094cbb65679fb59e4813b8139425227cfca200af49cb9ba9acec31f64); /* statement */ 
emit Transfer(
            address(this),
            msg.sender,
            to,
            value
        );
    }

    function transferFrom(address from, address to, uint256 value) public {coverage_0x67b8af4b(0xfae2dee4566680c54c99acd2d81c607c673f84ad7fc94d8b736ccf67f9c0b7ff); /* function */ 

coverage_0x67b8af4b(0xe8a41f30d400839b19d9614a771bc252afef900d463822efd24673a78b186388); /* line */ 
        coverage_0x67b8af4b(0x5e314433ddb34967cfea86c49c6e9051547b722f03703afcc41e391ea4c2c93d); /* assertPre */ 
coverage_0x67b8af4b(0x5d37ed5dd0afd584b88fa914ce7701c2e09199036358a0bd5f8527ee9e55452a); /* statement */ 
require(balances[from] >= value && allowed[from][msg.sender] >= value);coverage_0x67b8af4b(0xd8b442108b289befc6a6f78e9590683aa0e1cde1394b4498652c209bd61f8864); /* assertPost */ 


coverage_0x67b8af4b(0x0f0fc54a45b342f1d8d1b7dc8a06373c4bd816a5fa3c41579c59b8d730b2945c); /* line */ 
        coverage_0x67b8af4b(0x16eeb7390430094dc582234778edab030761290ba729bc9226acb637fc243b15); /* statement */ 
balances[to] = balances[to].add(value);
coverage_0x67b8af4b(0x9a8c1b42a7170a7097e92073240c8b924efdf41107b105673fc599b10ba56c71); /* line */ 
        coverage_0x67b8af4b(0x2f08d3515e51b51cb6772f051d1c5c2a03dd276eee008f1222223c34de3921ee); /* statement */ 
balances[from] = balances[from].sub(value);
coverage_0x67b8af4b(0xf5820248201d71e8d82273bd4b5ac21c4c561a11e0b3c008277d17be4d2df885); /* line */ 
        coverage_0x67b8af4b(0x8be300eac6152ca21d1c71da0a75642b41f6bb0ca1e4d5423ab41bc062aa47fc); /* statement */ 
allowed[from][msg.sender] = allowed[from][msg.sender].sub(value);
coverage_0x67b8af4b(0xace061764503c1464eaeabcb2cc493a41b674bd58de70e5a77faf77708f8903a); /* line */ 
        coverage_0x67b8af4b(0xdc8e965a069c054e6ed8ddfcc30e247e28ef5a5d24a489e0e2b5ac15692ed007); /* statement */ 
emit Transfer(
            address(this),
            from,
            to,
            value
        );
    }

    function approve(address spender, uint256 value) public {coverage_0x67b8af4b(0xb317bdc30337d8dc1634ab1716e64ef1d8a782206c6b3204c954811f0adb2a93); /* function */ 

coverage_0x67b8af4b(0x647f2046cd9d11875cade713985cc42f7ca43de3b583f1f53ca4f79138d8570e); /* line */ 
        coverage_0x67b8af4b(0x89a8d7d0f122d78c8e2a09eae0c0a6bc3b903a7d8c325f17273b55ff5e705700); /* statement */ 
allowed[msg.sender][spender] = value;
coverage_0x67b8af4b(0x3fe8d9b03f7ca3dca0004f797e0e08c499cbeb1c7e5265d2ef40231e4d654516); /* line */ 
        coverage_0x67b8af4b(0x99b65df620ef334fa464974adefc836b075814e4d08b8e4fcade661b184c84de); /* statement */ 
emit Approval(
            address(this),
            msg.sender,
            spender,
            value
        );
    }
}
