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


contract TestToken {
function coverage_0xaf324ebe(bytes32 c__0xaf324ebe) public pure {}

    using SafeMath for uint256;

    uint256 supply;
    mapping (address => uint256) balances;
    mapping (address => mapping (address => uint256)) allowed;

    event Transfer(address token, address from, address to, uint256 value);
    event Approval(address token, address owner, address spender, uint256 value);
    event Issue(address token, address owner, uint256 value);

    // Allow anyone to get new token
    function issue(uint256 amount) public {coverage_0xaf324ebe(0x9efe072d7041f2223a375322cc5d5d2862b1b283a036328a7ca5e5a5ba499022); /* function */ 

coverage_0xaf324ebe(0x7fcc5f6311eb984c60213a8b812e0fdc1af5339e9119065b03f3d7ea15fcaf28); /* line */ 
        coverage_0xaf324ebe(0x777e5014c03bfc47a367afc47d0ae4afca50e70d4cafeca67598bbee053d83d9); /* statement */ 
issueTo(msg.sender, amount);
    }

    function issueTo(address who, uint256 amount) public {coverage_0xaf324ebe(0x09bb82901f45cfc409c7935a2e31a21399ce65164e6d98d16621259999e9f567); /* function */ 

coverage_0xaf324ebe(0xe118b4fb16c1846adf2f4a3a1941d4f6923f48dc912b19bcd43c4da39a542a01); /* line */ 
        coverage_0xaf324ebe(0x7b0190a2aa957d88641f356a2c33d91fecdffb375bda8d2e6c08efdef875f211); /* statement */ 
supply = supply.add(amount);
coverage_0xaf324ebe(0x4e8f519ca993afe16bdf46ebc5ff455373038998a7d775dd75ef4c0bb7894236); /* line */ 
        coverage_0xaf324ebe(0x173b2b14baf5a9b8dd5722514e91afe259457848f4bf437ee2f7a00f8c6e10aa); /* statement */ 
balances[who] = balances[who].add(amount);
coverage_0xaf324ebe(0xdecb305280503251836f03e63c425c009f133324c5f8532a7e2ba6911b9776ed); /* line */ 
        coverage_0xaf324ebe(0x94eae0b1ce8d0213b1eaf4747f107c40d6ef1ed36c9597d2dc655003a8f82ca4); /* statement */ 
emit Issue(address(this), who, amount);
    }

    function totalSupply() public view returns (uint256) {coverage_0xaf324ebe(0xd288c284f1dba3e0f55c148b27b9a79db7c6e64c3e9c0dd2dc1c5a4c528d2a57); /* function */ 

coverage_0xaf324ebe(0x2856376c527e62425f1ce06c9225697c803cef79297679fb9866864dd27904ac); /* line */ 
        coverage_0xaf324ebe(0x27c54065085720fa1430e7799020ea8dad3ed9ba8c6d6ae09150907a7e1563d7); /* statement */ 
return supply;
    }

    function balanceOf(address who) public view returns (uint256) {coverage_0xaf324ebe(0x3279ff432b469cb82269995646884bcf91a461a7436ce281e1de872c341c4080); /* function */ 

coverage_0xaf324ebe(0x77313280969fa97875cf571bed300ad3ef4d7c7407a6abd633ee3e4ec412cae2); /* line */ 
        coverage_0xaf324ebe(0x8793c42dea0c32df245b26e351096e42824475e07e9a70f0dc03f6927b8ea536); /* statement */ 
return balances[who];
    }

    function allowance(address owner, address spender) public view returns (uint256) {coverage_0xaf324ebe(0x1b6979af49c9cfd1630a8fd2ced9b89eed022202afdccaf313342b27ca3dbeef); /* function */ 

coverage_0xaf324ebe(0x49a7337999cbac9c8bb9818401a7532dd015b503384cb8a925cc6dd8780b353a); /* line */ 
        coverage_0xaf324ebe(0xebbb5993b0e69277063f924c0b743723d302915d997637d6a86a8b8b75eecd2d); /* statement */ 
return allowed[owner][spender];
    }

    function symbol() public pure returns (string memory) {coverage_0xaf324ebe(0xaf935ae90fa01653149b7dfd6fd5d2bfaf524a7607db571e61e4dbd7eaaa6dff); /* function */ 

coverage_0xaf324ebe(0x3a213a3c5075abdf9c45d30b067d94cd38a942e02e98ae3714092d9dac4623c7); /* line */ 
        coverage_0xaf324ebe(0x967c9d4ceaaeb522d1eab1885e9e056d2c26fa81338f21fb947f939688773a8b); /* statement */ 
return "TEST";
    }

    function name() public pure returns (string memory) {coverage_0xaf324ebe(0xe5521fdc0e287712866e73b2071f75ae983fe57bf5ceafb49e7fa1673703d357); /* function */ 

coverage_0xaf324ebe(0x3c46eef0b1df0b20c6bd00defa86fecec71165caff4c4ee89145da015bf92552); /* line */ 
        coverage_0xaf324ebe(0x070b214f509482897b0bf848da536bfa108ff155ec3d2856ebbb924f0bc85472); /* statement */ 
return "Test Token";
    }

    function decimals() public pure returns (uint8) {coverage_0xaf324ebe(0x92af78b70c4fec331ba34937ca7a3f3255477ce4336c460341cda7d341275340); /* function */ 

coverage_0xaf324ebe(0xac368ec151e775ab425dfe298ccffa815eed03a91da0f8c89e87598a5fb2c69d); /* line */ 
        coverage_0xaf324ebe(0x420afa4ad484a7e995eda4a398eeab949c68bf9c06eedf1d05abb28e5a027ba4); /* statement */ 
return 18;
    }

    function transfer(address to, uint256 value) public returns (bool) {coverage_0xaf324ebe(0x935a404aeb94612217bc91be4c1ed3e5faffb508f45ddbb11c94161ed5dd766f); /* function */ 

coverage_0xaf324ebe(0x56d263c18c9558cd9e09950f9004492cf8ff3e4269b9487cb6a068b1dcf8f0c6); /* line */ 
        coverage_0xaf324ebe(0x4dc5e9a7f1483f76a500cef0e83b660e1cee1930bf41fda72a6056f11c51d2fb); /* statement */ 
if (balances[msg.sender] >= value) {coverage_0xaf324ebe(0x943668f996ddfb519fd3b8796afa469c93007150c24f8f898530e93457902c06); /* branch */ 

coverage_0xaf324ebe(0x1637b3107dc69053b60481c7a81ab00abc2b647c49e30c8b1d6d329a3c9bc366); /* line */ 
            coverage_0xaf324ebe(0xe90116964e46a8ec257a8153f2ff05f1a07b0119fdd56ca50c86099ee3ab9341); /* statement */ 
balances[msg.sender] = balances[msg.sender].sub(value);
coverage_0xaf324ebe(0xfc0cec15efefedc44acc0956eb347a7ee8d9d032e49c7c56af053df9cc37128b); /* line */ 
            coverage_0xaf324ebe(0x8ec8b35efe93165d6eff7f8d4f0ad7669e3c20ff31a472f32ffc404b1165393f); /* statement */ 
balances[to] = balances[to].add(value);
coverage_0xaf324ebe(0xfce2880fb454d307b742479c4bc273e1ad3410240efce192e9b37402fbe6f671); /* line */ 
            coverage_0xaf324ebe(0xd79310096e8b04ca2fff96a11db2b53f76cbc9e2462dcd2a39f50b14db79d35f); /* statement */ 
emit Transfer(
                address(this),
                msg.sender,
                to,
                value
            );
coverage_0xaf324ebe(0xc675185972c6b848be95aec3ea5e2fc4b7c7dedaab10474280e89a44a4869173); /* line */ 
            coverage_0xaf324ebe(0x9758d7476294fb3aef8ccc7adf8912e05e3d0614b53ca80efd3d7d8d4726ef3e); /* statement */ 
return true;
        } else {coverage_0xaf324ebe(0x71b76268577f48d85cbd28e4a5d1d5f67c14243c4797cdc9450cc8081a9db54b); /* branch */ 

coverage_0xaf324ebe(0xe7afe458aad68333f26293e2323001fbd1ebad709ce08cfded1c22eb7f5da12c); /* line */ 
            coverage_0xaf324ebe(0xf6ed4a0e423536472f5e163bf5fa2b11e06303b11c039ccab9f0be08e427f84c); /* statement */ 
return false;
        }
    }

    function transferFrom(address from, address to, uint256 value) public returns (bool) {coverage_0xaf324ebe(0xd1a044c0714e43646462638c013674699d70a20470e1201328ff8ab8495daed7); /* function */ 

coverage_0xaf324ebe(0x036479edddeb05399747d4ef9821b778dae910c3859845cf5fe62f0e77bb829b); /* line */ 
        coverage_0xaf324ebe(0x839750d6add39a8b694f97db0ab155c7abdaf58ee61e6f03dd501f94baf47b9e); /* statement */ 
if (balances[from] >= value && allowed[from][msg.sender] >= value) {coverage_0xaf324ebe(0x5a15279f8877745f845a1c359991628cc2a96cf03d294d0222e21c24de4f627e); /* branch */ 

coverage_0xaf324ebe(0xddd760362c71a4c41500c9bb8ff336bcf367c1f14cd15a16b20e39b951a86fe1); /* line */ 
            coverage_0xaf324ebe(0x692aa224cc58bc8280a7519391112e87bc8c413e370ea2e572b2f466b3fe1cc2); /* statement */ 
balances[to] = balances[to].add(value);
coverage_0xaf324ebe(0x1fdcbc622bab8999de5579682a702bff2a02a042944a24221280fc36f2ff02db); /* line */ 
            coverage_0xaf324ebe(0xad96f02c043210df157b89a3f73de8147f9efa2fd464d86c8be8a73c3e9bbd7e); /* statement */ 
balances[from] = balances[from].sub(value);
coverage_0xaf324ebe(0x29791736093319edbff41cb04bd3100addd5fe4673c0d481f958763785b0a6cc); /* line */ 
            coverage_0xaf324ebe(0x2eb27fd47f468803e4a1c06352c8be5fe8aaa905be2992db612590a22ff18168); /* statement */ 
allowed[from][msg.sender] = allowed[from][msg.sender].sub(value);
coverage_0xaf324ebe(0xf18a3d1195d2f7569c62c8f2c5f135ffd756f69902a94e54ed9092cba222bd20); /* line */ 
            coverage_0xaf324ebe(0x3f7b201e18934753cd29d48bbe49cec44a43d6103bbd0e09770496cd56b52dd5); /* statement */ 
emit Transfer(
                address(this),
                from,
                to,
                value
            );
coverage_0xaf324ebe(0x9491e7bdf33d905a84e52d7e2c2468de56d98d4d3cdbf38c6b7b5e62899d34a3); /* line */ 
            coverage_0xaf324ebe(0xa26a8c5d1e87689f83be37aa9033138fb4a8c185e435119fcf761c3acaa6b223); /* statement */ 
return true;
        } else {coverage_0xaf324ebe(0xe11c0b6058d6331cddf2b6d2da1b15a146d9d3c70a402e699e8522b4141dd1fd); /* branch */ 

coverage_0xaf324ebe(0xaa051db771cd0cb22b58fb6a43c396fec0de2769dea582dacf50ff525a3fad8d); /* line */ 
            coverage_0xaf324ebe(0xa24309aa29700c199aa3752b55608f7536b53b3cd6f942562075f94f2ffcc92f); /* statement */ 
return false;
        }
    }

    function approve(address spender, uint256 value) public returns (bool) {coverage_0xaf324ebe(0x1d0e1301bffa435b68bd62ad71b854494b3406b32cb7ae1bd496da65a4a0065e); /* function */ 

coverage_0xaf324ebe(0xfb26f4505039bf571dcd6e799031e57992055e7f0927efc1012457ce78cdf3c7); /* line */ 
        coverage_0xaf324ebe(0x4e50eb0f1fd809ef1a0f0f7e27c2210e51ba31ad0094d209f4f64179940a43ba); /* statement */ 
allowed[msg.sender][spender] = value;
coverage_0xaf324ebe(0x51dd800905c23ea3d9b550d3d53594c0056fb1a984ed3d6baf2a400879d438fb); /* line */ 
        coverage_0xaf324ebe(0x64c1108cfb295ee88298f6dadaa42cf89cc66aacd331500f25d69c8983cf7923); /* statement */ 
emit Approval(
            address(this),
            msg.sender,
            spender,
            value
        );
coverage_0xaf324ebe(0xfd2cb9f5c8280d1354b3373e79a9d48b920e7c5012d99525704b51021b987c92); /* line */ 
        coverage_0xaf324ebe(0x9669ba25338d268ade864f93c8f479e5ab6c47d974817b8d58c0366965aca674); /* statement */ 
return true;
    }
}
