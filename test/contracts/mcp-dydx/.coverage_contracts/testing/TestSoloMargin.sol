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
import { SoloMargin } from "../protocol/SoloMargin.sol";
import { Account } from "../protocol/lib/Account.sol";
import { Interest } from "../protocol/lib/Interest.sol";
import { Math } from "../protocol/lib/Math.sol";
import { Storage } from "../protocol/lib/Storage.sol";
import { Types } from "../protocol/lib/Types.sol";


contract TestSoloMargin is
    SoloMargin
{
function coverage_0x95c812f5(bytes32 c__0x95c812f5) public pure {}

    using Math for uint256;
    using SafeMath for uint256;

    // ============ Constructor ============

    constructor (
        Storage.RiskParams memory rp,
        Storage.RiskLimits memory rl
    )
        public
        SoloMargin(rp, rl)
    {coverage_0x95c812f5(0x390907a6f61aa5f076fef0a947cf0dba49eee46b6d0d433d58d9cd010b1b98c0); /* function */ 
}

    // ============ Testing Functions ============

    function setAccountBalance(
        Account.Info memory account,
        uint256 market,
        Types.Par memory newPar
    )
        public
    {coverage_0x95c812f5(0x469c78aee0f4cd0e98035cae01de9b95173536bd84d30fc506aae88f406482c4); /* function */ 

coverage_0x95c812f5(0x1b58582eb69450118144d10092879c2e9175682780f3c9b018c792e0ca91fa73); /* line */ 
        coverage_0x95c812f5(0x91e841801ca06f594323d600d1c9d145c4ee57211b4bb38172671cf39f7ba86c); /* statement */ 
Types.Par memory oldPar = g_state.accounts[account.owner][account.number].balances[market];
coverage_0x95c812f5(0xec512a48b2ef7f72b56b423defbea3c92b2155d33a2be3ae7c6a85025e5375a0); /* line */ 
        coverage_0x95c812f5(0x39cb09cb1e75ea66be587b0bd5b1b16a3d60a9b3dc206aa628877c50f2a28bc0); /* statement */ 
Types.TotalPar memory totalPar = g_state.markets[market].totalPar;

        // roll-back oldPar
coverage_0x95c812f5(0xa236854e6f06d14d0b8c30be718adc62c43c791661764e0b21b71b4d2e69b1b2); /* line */ 
        coverage_0x95c812f5(0x9c296fb0b5966f4edf93b97dd67385236dd8d877cf42ddc3e4781455fe39f501); /* statement */ 
if (oldPar.sign) {coverage_0x95c812f5(0x7521458e71395059773333ce167ce6f0346672953b782f46cc08c6ce3e4f303e); /* branch */ 

coverage_0x95c812f5(0x7366ce1c06b7fe963b5f9819b4b70d127f85b17b7ce95905421e6a8190e8e3d4); /* line */ 
            coverage_0x95c812f5(0xf220f60acb2f00ce0c27e3c3da0533c59cc5d8bda4eaf96cd630088494a7b9cd); /* statement */ 
totalPar.supply = uint256(totalPar.supply).sub(oldPar.value).to128();
        } else {coverage_0x95c812f5(0x5e1520e8b3adb3a9935d892d30e09d22639bd4e5ef045e7ab260131b025e0df0); /* branch */ 

coverage_0x95c812f5(0x50bc290580a6730ac77318e4987786fb0b8b44ddfbd7966b181216fa26e7bec6); /* line */ 
            coverage_0x95c812f5(0x480296ef7a9983f87a6447d9d2716d6750ae5992143fef6f823b700a2fab838e); /* statement */ 
totalPar.borrow = uint256(totalPar.borrow).sub(oldPar.value).to128();
        }

        // roll-forward newPar
coverage_0x95c812f5(0xf34c4cef899afab43bedecaf06c3cb26a731d714758b57790deaafeaf4e67c98); /* line */ 
        coverage_0x95c812f5(0x741527ed1c56f809b9792df335022a4409f89d2c40afb6b7c9dddf3b488543bb); /* statement */ 
if (newPar.sign) {coverage_0x95c812f5(0xeffdc3c25a3e4a3f7f6743711eed90ae370cd99a6f3f2d5875fa6408392fc683); /* branch */ 

coverage_0x95c812f5(0xa05ed1f044bbd2dbf1628e87f0c8daa93dd3e615e6d1d14771ac6db466093d52); /* line */ 
            coverage_0x95c812f5(0xbc6a92456f2463d5c334a2d22e5f31332c9eff177045f867b9a299192c7c7f15); /* statement */ 
totalPar.supply = uint256(totalPar.supply).add(newPar.value).to128();
        } else {coverage_0x95c812f5(0xbfd7779a36b6b31a3008168667f055988c910aa95f958f20b012e505733ee64e); /* branch */ 

coverage_0x95c812f5(0x47e728b5d1bdddff52568971a33035b9885de508df9e93f274c73071db0e07fc); /* line */ 
            coverage_0x95c812f5(0x832b9a5853e213600e5441ed94be59cb19b130572b790d48b9056c5320f0059a); /* statement */ 
totalPar.borrow = uint256(totalPar.borrow).add(newPar.value).to128();
        }

coverage_0x95c812f5(0x9100cf2487bd0fd6c2171ed3631fc5d39755ef8417eaaa5fcba6142cf19354c0); /* line */ 
        coverage_0x95c812f5(0x9bbc6bf9b58ea2577533a4b39dad42c9d89ed890239a02d8e68224498cd60e48); /* statement */ 
g_state.markets[market].totalPar = totalPar;
coverage_0x95c812f5(0x650dadb31de0923aaaec30f20df5ba5c20fe25337827d17a6e28b506a23c0236); /* line */ 
        coverage_0x95c812f5(0x3f22cf8140de9b8de6d8f6475f649b514ce4b8ad1646bb986bde32050d243310); /* statement */ 
g_state.accounts[account.owner][account.number].balances[market] = newPar;
    }

    function setAccountStatus(
        Account.Info memory account,
        Account.Status status
    )
        public
    {coverage_0x95c812f5(0xc8397113862a19cfeb054e4dbbf77b3c230879c7e63e0f89dbe6feb926a58617); /* function */ 

coverage_0x95c812f5(0x9566c899c642db24de4a29478c9963c441f0127f95cabb6b50ca2fc3df34e412); /* line */ 
        coverage_0x95c812f5(0x4af37b159504c2870980a9cb25b2fdcd7a942b1d4517857ebf2459bd9e158c4d); /* statement */ 
g_state.accounts[account.owner][account.number].status = status;
    }

    function setMarketIndex(
        uint256 market,
        Interest.Index memory index
    )
        public
    {coverage_0x95c812f5(0xd5f1b0775faf0332e3a13156ad9512e4b8c2ac2185d52a132743f2b6f52d848a); /* function */ 

coverage_0x95c812f5(0x5e3050f70c21814a1440b0903f002c526468b8395ee362313cf5d645d61677c7); /* line */ 
        coverage_0x95c812f5(0x9d8f15894df1aa4ac469537b38f5490958c4e6bc3e73aa95aded607a73572c41); /* statement */ 
Interest.Index memory oldIndex = g_state.markets[market].index;

coverage_0x95c812f5(0x9efea2184e11636b8652b01a183e9016e041ac14069a99ad84e5a3bdbc01807a); /* line */ 
        coverage_0x95c812f5(0x1410f084b1f0c270ebc88d2d9ff934fd819ee9255166f2116501ad682344b96d); /* statement */ 
if (index.borrow == 0) {coverage_0x95c812f5(0x23bfa1684c0965862348672decde5ea84ffefacf77cd55d30f1c59e0fd530437); /* branch */ 

coverage_0x95c812f5(0x7d36138a654e9f66bf73bf6c66922a9803d801cee4812de2c3c2ed3de1a63c55); /* line */ 
            coverage_0x95c812f5(0x365fd034fc257209bef87521b1604effed2591b89ce226df0fbbb0515a387ed5); /* statement */ 
index.borrow = oldIndex.borrow;
        }else { coverage_0x95c812f5(0x5897bbbbf9c430fb3d89a1f01eda33dfb52a682761d61056a2ac8dcdab49acac); /* branch */ 
}
coverage_0x95c812f5(0xcc7cb6cb8e26d3131a450c4046e965870b20f58a5eedb89d0fa0bb20cc092077); /* line */ 
        coverage_0x95c812f5(0x8bd61d2cdbc2fa699884c092a2b4fb19816bf2450bc031d490703a25a588b859); /* statement */ 
if (index.supply == 0) {coverage_0x95c812f5(0xe3cb6a3867607db6dcb0454e95781afb83552afb016db7a6545ecc572d554b19); /* branch */ 

coverage_0x95c812f5(0x53cb6acd8f4ce68459ea6fc093d9ec4b80099906217281a745e1a3a9d7f270ce); /* line */ 
            coverage_0x95c812f5(0x791378accae8923c85f4763a7899733e14de25b1253b46bde99156d7e92dc3d2); /* statement */ 
index.supply = oldIndex.supply;
        }else { coverage_0x95c812f5(0x5439d161a5798b72e07daa200d31752f2dfd2ced33ba32706fff05e3ed793157); /* branch */ 
}

coverage_0x95c812f5(0x792b9b06cea475fa92ed8f80fdafe60de238b7112e38fccaf6b9a0e60c8acb68); /* line */ 
        coverage_0x95c812f5(0x56bb6d50b21642403f86082501532f9f207ba0462311a5325cb556325891199b); /* statement */ 
g_state.markets[market].index = index;
    }
}
