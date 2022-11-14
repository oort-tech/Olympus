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
import { Math } from "./Math.sol";


/**
 * @title Types
 * @author dYdX
 *
 * Library for interacting with the basic structs used in Solo
 */
library Types {
function coverage_0x797e760f(bytes32 c__0x797e760f) public pure {}

    using Math for uint256;

    // ============ AssetAmount ============

    enum AssetDenomination {
        Wei, // the amount is denominated in wei
        Par  // the amount is denominated in par
    }

    enum AssetReference {
        Delta, // the amount is given as a delta from the current value
        Target // the amount is given as an exact number to end up at
    }

    struct AssetAmount {
        bool sign; // true if positive
        AssetDenomination denomination;
        AssetReference ref;
        uint256 value;
    }

    // ============ Par (Principal Amount) ============

    // Total borrow and supply values for a market
    struct TotalPar {
        uint128 borrow;
        uint128 supply;
    }

    // Individual principal amount for an account
    struct Par {
        bool sign; // true if positive
        uint128 value;
    }

    function zeroPar()
        internal
        pure
        returns (Par memory)
    {coverage_0x797e760f(0x382c7e926d7ea5fc886f6ad2bc10c0af5f01e4420320118504cd061d905885e1); /* function */ 

coverage_0x797e760f(0x93f037d4c78ea59c5f49da2a8857e330fc23b0331e77c616ab163d0979842b0d); /* line */ 
        coverage_0x797e760f(0x8326e6d65b77180be1accfb0b57252ab418d876fc8de9f30345cc95ec5c1bf81); /* statement */ 
return Par({
            sign: false,
            value: 0
        });
    }

    function sub(
        Par memory a,
        Par memory b
    )
        internal
        pure
        returns (Par memory)
    {coverage_0x797e760f(0xe9887cee6b5e9e4ce84519e7bfc1baf5823c6a21bb1e904a6202f63a3536c9a5); /* function */ 

coverage_0x797e760f(0xce48dc27fb614bf891b092dcdd65d390b5286ef9b2d289f53e6037e934decb29); /* line */ 
        coverage_0x797e760f(0xbb90e0f4e7d81a7739942656bf42b57339a07151c5d3f1f1372fe068f0d91030); /* statement */ 
return add(a, negative(b));
    }

    function add(
        Par memory a,
        Par memory b
    )
        internal
        pure
        returns (Par memory)
    {coverage_0x797e760f(0x7bf13c96a62ffc876e57c7043e54a420ea1e3d35f68d0a562bc0a739ea63415c); /* function */ 

coverage_0x797e760f(0x3e6dc741eaae74a6e5a407e6e0c12d0f4d64c4eb731b477124ed006680cbcdd9); /* line */ 
        coverage_0x797e760f(0x52a62035bce3022654305612146eb36dbbec1208d539e6279bd900e4a0b3a6d7); /* statement */ 
Par memory result;
coverage_0x797e760f(0xd1a98b30bd8a0820568df20a99aa70129fc5d90dcfd2bb3e0ab36db29fb6426d); /* line */ 
        coverage_0x797e760f(0x807a74aa17a0a7d52413c13579bdb50f56681eb2b6e5e12041a2be45232417cc); /* statement */ 
if (a.sign == b.sign) {coverage_0x797e760f(0x1486960381b0ddc99863a41d711671e8815a866f1eb97c61aa5a8dc5aec8ddff); /* branch */ 

coverage_0x797e760f(0x1cf31157fbdd6c6a66df409a29cca47582a0d7e6bbffdc8aad1d7dd92a20ac90); /* line */ 
            coverage_0x797e760f(0x8a622d86122edc0b46043ca06dcdfb009633b60db4929bba652036edeb70c639); /* statement */ 
result.sign = a.sign;
coverage_0x797e760f(0xe94556ca64bd02370c08421a2ab7d6d961b51fdf4775d02ec74f2b09ebdcc396); /* line */ 
            coverage_0x797e760f(0x34f709c9717cd791bce5404800cb98b3332b8ee73858dbfd4a56eba29fd461ac); /* statement */ 
result.value = SafeMath.add(a.value, b.value).to128();
        } else {coverage_0x797e760f(0x582baa79b5237b83c57d749bd2ddd18d411d6e1eccf6904dee0733a41f1f0be2); /* branch */ 

coverage_0x797e760f(0xd40d12f524a9c792e0c399865159877315f5a43ea430e20e88c50d47a4867045); /* line */ 
            coverage_0x797e760f(0x5aa696a8cbf259315384a0f80e316b7e8d5da8c40cf336cfcbfefe0cba675664); /* statement */ 
if (a.value >= b.value) {coverage_0x797e760f(0x8867941a83cf3a46a5dfc4efcc6f8fc75fdc0610f4c4665877bb301954fd7905); /* branch */ 

coverage_0x797e760f(0xb72b9356b407946bde39f120f06e56a17b76aa1ee4dc670964e1509472d6bbfe); /* line */ 
                coverage_0x797e760f(0x62fc47cf1a3115bdebd53b71cdf61bcb21cef67930bd7ba430861c0e06a02f58); /* statement */ 
result.sign = a.sign;
coverage_0x797e760f(0x8eb9de626d533db1e858e09f54b4f08714aacc7e4c29577897b3faaa5f653896); /* line */ 
                coverage_0x797e760f(0x72ea943ad0e8404750ee53eea9e3665739e689462b5a62b83494a9b7b8156f21); /* statement */ 
result.value = SafeMath.sub(a.value, b.value).to128();
            } else {coverage_0x797e760f(0x5b19f0e41cc2f6392d880428eaca24c80b06cf12df96e2169b299b819a2f0419); /* branch */ 

coverage_0x797e760f(0x1340599f036ac2c6001c449c354dfe21db6f569e51c57a605a88de72b724b1e5); /* line */ 
                coverage_0x797e760f(0xa8258b16774513025c3053a31893ab7882ee146b4ccdd3aeea0156a0dfa45d5e); /* statement */ 
result.sign = b.sign;
coverage_0x797e760f(0xfd7630596f225c64108121cbadbdf175c0ae3bfb7ee4bcbdd33ed84d65272390); /* line */ 
                coverage_0x797e760f(0x60a72dbcdc98aeab172edf9dbb0523ee85b62f8e7416e9009e6f8f41b36f4ae3); /* statement */ 
result.value = SafeMath.sub(b.value, a.value).to128();
            }
        }
coverage_0x797e760f(0x98ac7eae5d34d159b08a4661198476256c8d86341f2990331bd3f57ad4794be6); /* line */ 
        coverage_0x797e760f(0xfe151c3526831a6369eec13a0035043b078d910abfd5a5e30aed4ca3fdb6b0f1); /* statement */ 
return result;
    }

    function equals(
        Par memory a,
        Par memory b
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0x79556db2b9a761a1134cd9a95afafdc1aaea7a19933296bbf89ec4107003aae6); /* function */ 

coverage_0x797e760f(0x2d647048b60c94e38e75768cd0a977139c29d5cedaa92d4388393d2f374d105a); /* line */ 
        coverage_0x797e760f(0xd00e698f5fe8f23d7891cc81be25e6f48a022fe7be129c3d758275aa652d3dc1); /* statement */ 
if (a.value == b.value) {coverage_0x797e760f(0xf26a9ca3eea108a0ee3590c8bb1834a04f3cd916d54c93d8437705e9a5743fc3); /* branch */ 

coverage_0x797e760f(0x2cb9108f327c9ac01bbb36aa997dc5e13adbc51609bcc914d0d44ff14bbd4ec6); /* line */ 
            coverage_0x797e760f(0x413d35205d2b292d6e99f4dc5414bf73a67108a9d2b2c6a906453631c283d1ce); /* statement */ 
if (a.value == 0) {coverage_0x797e760f(0xce142daaf08c8c921faf1c24dae1112ba676de07d584fddfeaf6fe0de0344f9b); /* branch */ 

coverage_0x797e760f(0xbceb912e1700dcad90830d8c0f35341b0ecb078d3ac618693969570ad6cab5ee); /* line */ 
                coverage_0x797e760f(0xea4439bc0a00d8b5bae3c2fa6f8d759710d7e33b62fda9fd7e1bb1252ffa5bdb); /* statement */ 
return true;
            }else { coverage_0x797e760f(0x5fa4b43760b1c8a54b461e6bee92044d9c8267cf0cb00d1454e3dc4c41304e04); /* branch */ 
}
coverage_0x797e760f(0xf74a9ce070ebdc56987321bca8cb78e3e97a271d7eb9a0899dca1c637553f6f5); /* line */ 
            coverage_0x797e760f(0xd1a2725d41a9ede24487af6fb05452f2e0caf1a014847ec0b58b2cb554db0cd0); /* statement */ 
return a.sign == b.sign;
        }else { coverage_0x797e760f(0xd5fd96ea1c404a85fde301f5aaaf84867c10ac22103b4b44587e1b940c2d05a3); /* branch */ 
}
coverage_0x797e760f(0x5372b1f92ca1c121eac9abc9b291cb4941f9afc4cd710c5699d37cad8983949a); /* line */ 
        coverage_0x797e760f(0x048807cbc9b65f79317a40f4735eb1b161873ee26d0f28da9665121778188bde); /* statement */ 
return false;
    }

    function negative(
        Par memory a
    )
        internal
        pure
        returns (Par memory)
    {coverage_0x797e760f(0x29e903433e96ba48d92b82d8b985d3853bee6b65d06208162406bf74276bebab); /* function */ 

coverage_0x797e760f(0x2dc634f8d2efa5a5ef73681ad71f3241b0384fce4d8a2ceccfd2e059f2a5aef0); /* line */ 
        coverage_0x797e760f(0x6c6ac5d15996f4412a99428765cfe04a3e904d6b7892d3fb33849a0cccd4d039); /* statement */ 
return Par({
            sign: !a.sign,
            value: a.value
        });
    }

    function isNegative(
        Par memory a
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0x6132ce0f183562c345b3da9e41ee6142350bc34c11f61879751dc3b0796ceef5); /* function */ 

coverage_0x797e760f(0x7711f2d025864750287a93606df8d3c430f784b40c4cb6193e95c19ea8800038); /* line */ 
        coverage_0x797e760f(0x17b30c28e84c155c75050d83d6945c551891da8be2f0bdf64dead537fb701027); /* statement */ 
return !a.sign && a.value > 0;
    }

    function isPositive(
        Par memory a
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0xaebbb72bbd7843a482569286d65ff6ea43dd39b7ebb1a75420b96e7aff1d9279); /* function */ 

coverage_0x797e760f(0xc190ad9a0f1f39febacbcb48f38bc9fc40156248c9b56349bf038e2e6181f59c); /* line */ 
        coverage_0x797e760f(0xd5d6abd23eb2a8ed4a3b5711f9c880a4707fe0527ca665a34c6459622ca5df59); /* statement */ 
return a.sign && a.value > 0;
    }

    function isZero(
        Par memory a
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0xd8c17cdf7486f91939ecfbfa42815eaed48244e631d861b7e818a9078fca4635); /* function */ 

coverage_0x797e760f(0xe5351343afb90cf533e4972ddf7ce9cd69200a7689708e30dbe7a8159034c07b); /* line */ 
        coverage_0x797e760f(0x1edfa556e5b35a494fe87d3ca14ad970fe121fe3ff8a5550e3921a3a5b5a5bc2); /* statement */ 
return a.value == 0;
    }

    // ============ Wei (Token Amount) ============

    // Individual token amount for an account
    struct Wei {
        bool sign; // true if positive
        uint256 value;
    }

    function zeroWei()
        internal
        pure
        returns (Wei memory)
    {coverage_0x797e760f(0xfa4f2d356cc69fb2f98090f452fa64440e4dc37d615581480815aee8a26e2442); /* function */ 

coverage_0x797e760f(0x3695fef5dedcb12fc76c67869152c564abdd4bbad5aaa78a821cc31d9ab7e532); /* line */ 
        coverage_0x797e760f(0x7c002e4e1635c7f5ba51432c293c4d7605781f7b5c6b4bd5e1d66b1489ce92f5); /* statement */ 
return Wei({
            sign: false,
            value: 0
        });
    }

    function sub(
        Wei memory a,
        Wei memory b
    )
        internal
        pure
        returns (Wei memory)
    {coverage_0x797e760f(0x31af4a1fe51bff1031a70256ba039745d90fa290ee7aaa53ae707c5eb6beb0fd); /* function */ 

coverage_0x797e760f(0x60a19a58126bb63ed130104df566a017d50b058ba7fd2e24b8a39e7858e736b0); /* line */ 
        coverage_0x797e760f(0x88844d40d89a79b1fe255727e62c39c5507b94c6afb8b41e5116ae3710f0eefb); /* statement */ 
return add(a, negative(b));
    }

    function add(
        Wei memory a,
        Wei memory b
    )
        internal
        pure
        returns (Wei memory)
    {coverage_0x797e760f(0xb874fb7d3693905580d8112358b1a0d63f2c7e14dd8c67f7f024c638a9a37f11); /* function */ 

coverage_0x797e760f(0x273cf66e3d3146b66ff1b32c79e4d3bc65ebbde53ef2d3aa7541b38d8ebe6846); /* line */ 
        coverage_0x797e760f(0xfe99162eb0473cfe7abcd366a84bb4c1f971eab30d03eed7d60048930b35fb8b); /* statement */ 
Wei memory result;
coverage_0x797e760f(0xd6ba6eba401b28050fb98d09f013cb70e2624099f1a73e21196a972f9b40b8de); /* line */ 
        coverage_0x797e760f(0x927bbae02d708648b70b2cac758e50bea0c086a86c76f2d219f28d120384ecff); /* statement */ 
if (a.sign == b.sign) {coverage_0x797e760f(0x7ea3eeae2200889239d12f18f5ce4142452ad13d06350f9af2b967ca096a2ef5); /* branch */ 

coverage_0x797e760f(0x17758d0e46f1d56786d91d46e528f12fdcac559425d2770fad008951aa4bdb6f); /* line */ 
            coverage_0x797e760f(0x02ac8e3e44f5a0fe05f155ab8249ecdc8f66531fcc1b0f026088da1186db3194); /* statement */ 
result.sign = a.sign;
coverage_0x797e760f(0x13793a4341693fa6f0ca0b9ec0be792e5a6d34f210104a8760e01e94c1d63f6a); /* line */ 
            coverage_0x797e760f(0x56f421f3d555558c15ca4927fc4c0fdc98e212deba015ebe191f0af377bf04e6); /* statement */ 
result.value = SafeMath.add(a.value, b.value);
        } else {coverage_0x797e760f(0xade68fecb0f4df7e483d1d3fda50baa9b19d39cbd85f5194ebd022a385960bd8); /* branch */ 

coverage_0x797e760f(0x956917d08de5aeabbd89d887810b47920e3f014e0c11b54931a257e963855606); /* line */ 
            coverage_0x797e760f(0x823aa5e2276bdd03a7a489bdbe1913407341d5447dcf4d9c43725281481a8893); /* statement */ 
if (a.value >= b.value) {coverage_0x797e760f(0x98c9b0baeb888ef3d2df4d9b1ec2eb9dfc64e9dea1a1a9be5f43ffc13967f700); /* branch */ 

coverage_0x797e760f(0xb3f443f882cb4ea810421ccc98013711a8275107a8c51e68cff4e88b9bf58929); /* line */ 
                coverage_0x797e760f(0x6e15d24e16e52cd4bb1ac73261e19cef424f1ec8594a8ca12b567bbec00bf313); /* statement */ 
result.sign = a.sign;
coverage_0x797e760f(0x478aa8689acedbe92948a993942600145bd9055a137668848ea6c0ca432165d2); /* line */ 
                coverage_0x797e760f(0x01e2be29d48519a35ba6cc797c93612ed9eaa73d546cb61ff4af064a16abc24b); /* statement */ 
result.value = SafeMath.sub(a.value, b.value);
            } else {coverage_0x797e760f(0x89f09023e8d242fad22028f0a63a5b6d24e947c7ba85d30c7e9facb10db52773); /* branch */ 

coverage_0x797e760f(0x8ad64ecc9be9a9bf36f4a203367461c5eb6151031ca6b090848bc2504afe235c); /* line */ 
                coverage_0x797e760f(0x36d340035a95487e8a9b2f38e734f3638f3499e561548992a035fd518e08804f); /* statement */ 
result.sign = b.sign;
coverage_0x797e760f(0x0bc9837327979e67d1d0895e3b2c84907ce35d427174b83aac3eb4321030a15d); /* line */ 
                coverage_0x797e760f(0x76936ab9fb03baf45c8433955f0e1478c503c8789f9bb10e03bd21695916ca96); /* statement */ 
result.value = SafeMath.sub(b.value, a.value);
            }
        }
coverage_0x797e760f(0x153eee82edbced8e5f0d18aace8c42cc79b71d72e0efe63424339ac05830879f); /* line */ 
        coverage_0x797e760f(0xad4c2735dd61009691bab69172e34f2a94c1ae6c5fb1c7e4f4251a2422bb50d7); /* statement */ 
return result;
    }

    function equals(
        Wei memory a,
        Wei memory b
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0x432f413251c7c39f06b83efbbd738e593a2508cab4e9bee32b8c9ca6fd30c2c7); /* function */ 

coverage_0x797e760f(0x287da693eb4f03893d089c7c90e79acfebde2048fa39cce86c7956451bc50d71); /* line */ 
        coverage_0x797e760f(0xb6cf7996e35d89b65ec26a9ffb7bd784614ee64a05c2f940a41be2be5aaf6fe6); /* statement */ 
if (a.value == b.value) {coverage_0x797e760f(0x1a1fd0a2361af30acb21c4b0e848c5d0b2f46b5234c3e20df25a3c4d19c70f5d); /* branch */ 

coverage_0x797e760f(0x5094963302b5379fd558c2db7eb1be39971dfae63374265a54dec789c0858042); /* line */ 
            coverage_0x797e760f(0xc796c7b4f1e9d556641ee7405dee4c7599ec14682294b655c53548a1c1172d8a); /* statement */ 
if (a.value == 0) {coverage_0x797e760f(0x6d18745fb5da3ae811b8a7853275cb2353055479de231dfeb9667b17ed2ab1d4); /* branch */ 

coverage_0x797e760f(0x44fd927350fdba5b2982001cb511ca7047e80b7dbe4e7eae203db51743848c3a); /* line */ 
                coverage_0x797e760f(0x9c9de3418120726faf2a84183d77001ddeddf0dd304d6ccfac200e9b60dc2250); /* statement */ 
return true;
            }else { coverage_0x797e760f(0x93c2273fe155b700bf1b1f709be226a5c998b3fb3d197730db36e9b58754aede); /* branch */ 
}
coverage_0x797e760f(0x1d4465c6fe7a5d35a9b739662ea70f860f015648204dbd75dc276478185b2cbe); /* line */ 
            coverage_0x797e760f(0x89bae9ccff83e2a5f9bffaa8c6f271adad9d8576bd0f588b1a37216f4129d392); /* statement */ 
return a.sign == b.sign;
        }else { coverage_0x797e760f(0x9a49dbff7cdc320a5aa679a426a7ac7b7b5fc5a1bf247166df04ac4d441dbc03); /* branch */ 
}
coverage_0x797e760f(0x752f0348832802ceb705d508db8383baba474b60e7a33e8a80c89831063ddd88); /* line */ 
        coverage_0x797e760f(0x597e26ba5ab00d0aa2864d83ec0abb1b4518fb08c9bdf5ea49d0f6d7e8cf8beb); /* statement */ 
return false;
    }

    function negative(
        Wei memory a
    )
        internal
        pure
        returns (Wei memory)
    {coverage_0x797e760f(0x416001e81ac47251aaf9cb04fb9c90b0d7ac7212a552b7a8744ff1bc254ead59); /* function */ 

coverage_0x797e760f(0x06a845da74701fd2c491551ebea0bbf9a1a310459815b3f061a60a57b461ac97); /* line */ 
        coverage_0x797e760f(0x95ec537a38186e2f60ad7f970564dc165b416a70d516b697ac868ef67cda81d7); /* statement */ 
return Wei({
            sign: !a.sign,
            value: a.value
        });
    }

    function isNegative(
        Wei memory a
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0x289212671f7bf3096b5c4adc48df86b99b265b879fc167f0e8fbd264184b4642); /* function */ 

coverage_0x797e760f(0x1b498845ea29dd5c517474d0091151ecb5ccef12022a31cce716fd4412601acf); /* line */ 
        coverage_0x797e760f(0xe5b87cc6b34a693e649089a16bee3a61bf3d4f4491c15340ef62f8a65e3668c2); /* statement */ 
return !a.sign && a.value > 0;
    }

    function isPositive(
        Wei memory a
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0xfcc1ffbe8be403ae3381a63b17a662656c00987066dc62dd029526a40f992128); /* function */ 

coverage_0x797e760f(0x9a0f7fbb30cb28ba12a5cb721b5d00ab9668ed8d5fbe3c5bcf1239294c5f608c); /* line */ 
        coverage_0x797e760f(0x865f8c9d623cd27f9ff747d6426bd1f1600122977783a61e7facbcd770197112); /* statement */ 
return a.sign && a.value > 0;
    }

    function isZero(
        Wei memory a
    )
        internal
        pure
        returns (bool)
    {coverage_0x797e760f(0x07b296e3efca230e7a302c9fe0f6200b597869fba2c2e3e7c0453b2b4d167428); /* function */ 

coverage_0x797e760f(0xc6befd7c7e33f94fa4191ac48bcedd2f977a65fcfd98345e7393cd35265e85bb); /* line */ 
        coverage_0x797e760f(0x027a83f7ca100b4c4b232df0031fd77b2236ac0e9c8944c8e852e0693aa45deb); /* statement */ 
return a.value == 0;
    }
}
