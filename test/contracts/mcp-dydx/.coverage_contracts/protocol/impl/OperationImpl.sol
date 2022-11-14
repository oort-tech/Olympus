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
import { IAutoTrader } from "../interfaces/IAutoTrader.sol";
import { ICallee } from "../interfaces/ICallee.sol";
import { Account } from "../lib/Account.sol";
import { Actions } from "../lib/Actions.sol";
import { Cache } from "../lib/Cache.sol";
import { Decimal } from "../lib/Decimal.sol";
import { Events } from "../lib/Events.sol";
import { Exchange } from "../lib/Exchange.sol";
import { Math } from "../lib/Math.sol";
import { Monetary } from "../lib/Monetary.sol";
import { Require } from "../lib/Require.sol";
import { Storage } from "../lib/Storage.sol";
import { Types } from "../lib/Types.sol";


/**
 * @title OperationImpl
 * @author dYdX
 *
 * Logic for processing actions
 */
library OperationImpl {
function coverage_0x690caa01(bytes32 c__0x690caa01) public pure {}

    using Cache for Cache.MarketCache;
    using SafeMath for uint256;
    using Storage for Storage.State;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "OperationImpl";

    // ============ Public Functions ============

    function operate(
        Storage.State storage state,
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions
    )
        public
    {coverage_0x690caa01(0xb7f5c1506ee4f3606b4570a393014c80434a76b980cef3b1e5dab070f3cd463e); /* function */ 

coverage_0x690caa01(0x08e2168e83f13c8640d88067ae775fa2af5392ed96d4e73d938279e062e9d5f2); /* line */ 
        coverage_0x690caa01(0xf4ab7a3b42f3b5aad5b9af5f0a7fd94e08018f34a4b288e67daf50ce2a2f0900); /* statement */ 
Events.logOperation();

coverage_0x690caa01(0x01798cd35f26ecb5887ec3b8cd9ead9bf97f401121e31f974cbdebe43fa66ec6); /* line */ 
        coverage_0x690caa01(0xcb100b2a9fe9272f5b7687b14d1461c41aced25dace2a1698f63a963d1529f6e); /* statement */ 
_verifyInputs(accounts, actions);

coverage_0x690caa01(0x199eeeb46b6e819d14f66ca96f52de46f799065eb30ad21881320075741aab3a); /* line */ 
        coverage_0x690caa01(0x7465d811eb814b645b73aaf54c8c5300c71c381e79666050996b6f02c7964a93); /* statement */ 
(
            bool[] memory primaryAccounts,
            Cache.MarketCache memory cache
        ) = _runPreprocessing(
            state,
            accounts,
            actions
        );

coverage_0x690caa01(0xaa3f519f7b0c0d2d6ad5540df3d1d970c535fe4c6ea636a7362cae4ddbe38393); /* line */ 
        coverage_0x690caa01(0xddce47afebac2f9a8d5e3f37930519ac87627f2584d8ff994e1a9335a5f11010); /* statement */ 
_runActions(
            state,
            accounts,
            actions,
            cache
        );

coverage_0x690caa01(0x86943288dc734ce27c67945dc0ca2b0fdde90d5c2a8f99905556d33a05b6d219); /* line */ 
        coverage_0x690caa01(0x4f600c8ef92fd14cd0118329259182586f79af3296fe839fc7a3ae7a619a9d4b); /* statement */ 
_verifyFinalState(
            state,
            accounts,
            primaryAccounts,
            cache
        );
    }

    // ============ Helper Functions ============

    function _verifyInputs(
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions
    )
        private
        pure
    {coverage_0x690caa01(0xd8b2b550724c1672d1831703789acdb5fd2a403280c9283abbb475de91b16b38); /* function */ 

coverage_0x690caa01(0xe916be94fd42e92eeb140fa8e4ecc444cddacb5ee1d9651359a1610a5f1f46d2); /* line */ 
        coverage_0x690caa01(0x507df1e4eb778c701a40a394a2c227114c04d868d707c60bde04f92f20e54973); /* statement */ 
Require.that(
            actions.length != 0,
            FILE,
            "Cannot have zero actions"
        );

coverage_0x690caa01(0x2413e39ea2beb37f2b2a32af44f1f92d19da9f9151db9ce56d14ab25cb4dc300); /* line */ 
        coverage_0x690caa01(0x2e390639b5ce813edee176685df36b6e56284240ca0b89661f9eb745d40bb405); /* statement */ 
Require.that(
            accounts.length != 0,
            FILE,
            "Cannot have zero accounts"
        );

coverage_0x690caa01(0xdfc166aab5c65b44229544d168ea7ddf7f7424ea84f948c749d64c9e840b328b); /* line */ 
        coverage_0x690caa01(0xa01a49fc69cb98e69fcb064bbcfd40527f638eb6010a5ce1fad2f9f1dea88faa); /* statement */ 
for (uint256 a = 0; a < accounts.length; a++) {
coverage_0x690caa01(0x1c242ef2a723df2e4d9f2ba870c08204633e3ac6ca8f56ae74d1f642628837d5); /* line */ 
            coverage_0x690caa01(0xa6c35b950818301a4bcb6dcea921e8dd1da4e65896588f2c5894da8ca9d4716d); /* statement */ 
for (uint256 b = a + 1; b < accounts.length; b++) {
coverage_0x690caa01(0x2b24cbd0b409899c09ee706996ba9763cd157467a3498e5a4aef9ebe1c2bbc26); /* line */ 
                coverage_0x690caa01(0xa00561d28c3b3c7e882ea8e0d493977411238b4a287848a438a70be6ffe7961f); /* statement */ 
Require.that(
                    !Account.equals(accounts[a], accounts[b]),
                    FILE,
                    "Cannot duplicate accounts",
                    a,
                    b
                );
            }
        }
    }

    function _runPreprocessing(
        Storage.State storage state,
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions
    )
        private
        returns (
            bool[] memory,
            Cache.MarketCache memory
        )
    {coverage_0x690caa01(0xeb8dbbcf5dec43ee948e1dee859de825245fafe02046d5f778502dc21ccd6804); /* function */ 

coverage_0x690caa01(0xdc1e78033f2351d169ea751897201fb3f5f21786137a2f07407eec190790b920); /* line */ 
        coverage_0x690caa01(0xd033b989562558202c6a8324e22c61eed048c9b33f8cc7eae90cb7200200fac7); /* statement */ 
uint256 numMarkets = state.numMarkets;
coverage_0x690caa01(0x417377280a4e8b00cb32f439b804501b488d8be7d259c7ed55bfb9eef22b28b9); /* line */ 
        coverage_0x690caa01(0x9d6ada1a128ed4531387e2f34341ce63933ca3a061e589932eef9d3e7cf07230); /* statement */ 
bool[] memory primaryAccounts = new bool[](accounts.length);
coverage_0x690caa01(0x26f9b2a705e56f2859074eab7586518ccc604545c67d1fb05a2a6fcfac0dd9bd); /* line */ 
        coverage_0x690caa01(0x5f4d3808f6e9503c944c76707cc1b307e635cb0251ac8326a2dbb4334525cbf6); /* statement */ 
Cache.MarketCache memory cache = Cache.create(numMarkets);

        // keep track of primary accounts and indexes that need updating
coverage_0x690caa01(0xa4070e6002519996024f1af11d3d00b98ec188a6d8810d315f0bb6558122482c); /* line */ 
        coverage_0x690caa01(0xe159e0164a29a9fa1302aeb4f7fe64a948bac324a392506030cb17ea606039bc); /* statement */ 
for (uint256 i = 0; i < actions.length; i++) {
coverage_0x690caa01(0xbd73ded59da7edc61f27f6ad5158ef0e878adae6dda8c5e58a6a480ff2a29893); /* line */ 
            coverage_0x690caa01(0x882e1f60e21fd551e5cf1f55a53da7350421a782b8b6560afa617f46b445a97e); /* statement */ 
Actions.ActionArgs memory arg = actions[i];
coverage_0x690caa01(0x0515466f7e4b5f906787eb3430590ab243ac93a8808ad1dcd1f6499c8cb546d8); /* line */ 
            coverage_0x690caa01(0xb3110fabb5aa21b8a9080f73cd7b6116b658fcc5e648e180d4819e236f72b313); /* statement */ 
Actions.ActionType actionType = arg.actionType;
coverage_0x690caa01(0x289f51a4da8affeb8145ebfa4d908cee9ccae020c9225616bf44fabaebcee27c); /* line */ 
            coverage_0x690caa01(0xf9e5ba5228aff8be388cf5b564d6bce5b885d8035525a3f9150d008d60e00e2d); /* statement */ 
Actions.MarketLayout marketLayout = Actions.getMarketLayout(actionType);
coverage_0x690caa01(0x3f3d221842890fa00883a62262377e9a7275e4d9824cb4767bf6bb960d33da02); /* line */ 
            coverage_0x690caa01(0x2719fb66c54260ed18e1f220e1e36ae58e8020e729a4a806279f519467b43434); /* statement */ 
Actions.AccountLayout accountLayout = Actions.getAccountLayout(actionType);

            // parse out primary accounts
coverage_0x690caa01(0xbb9f8db0000fcd234112811e6b6a499bfae7ace9134faea943126c6b057cc26a); /* line */ 
            coverage_0x690caa01(0x9ab752e1768352be94fe2e482ca4adcd73f6a9cc03651107224c456eee8fc6c5); /* statement */ 
if (accountLayout != Actions.AccountLayout.OnePrimary) {coverage_0x690caa01(0x6262fcf272c1792dead3d0ca0e3376bde3ded5fdfecf03e91d7ffed9158c89ca); /* branch */ 

coverage_0x690caa01(0xcec88f0ad7314d660e28f8e96029d4c1efcfcbd58b5384a3fc09338ac496353c); /* line */ 
                coverage_0x690caa01(0xb930ff0452242b3fc321ec29e6df5bad8c8d0db77a6906350aba9e9f02691513); /* statement */ 
Require.that(
                    arg.accountId != arg.otherAccountId,
                    FILE,
                    "Duplicate accounts in action",
                    i
                );
coverage_0x690caa01(0xaea0bac794d05139949b6ab79756e380db64b0bc1bb1ed644a34a370aca89660); /* line */ 
                coverage_0x690caa01(0x89f4aa830bbee63846c288ff50100dd7cb91bed2be76cbdfbb79cbc6f29a5d3d); /* statement */ 
if (accountLayout == Actions.AccountLayout.TwoPrimary) {coverage_0x690caa01(0x23f0038d80621db29b53424f74c45f51ba2cc058eb434adfc4e9f8b75bb53d86); /* branch */ 

coverage_0x690caa01(0x074447eca109741e97f9a03c702255c42e45e2d378c866b5999feca7f7d680cc); /* line */ 
                    coverage_0x690caa01(0x037be383e4fcc56ebc19920331aae6ca2a39d120322ec6fea04eeba9fce94d8b); /* statement */ 
primaryAccounts[arg.otherAccountId] = true;
                } else {coverage_0x690caa01(0x9903590aef1310c0d95be8ff8c865f685a5a83e88a3ab3bf59ecbdc6e88e4c45); /* branch */ 

coverage_0x690caa01(0xa91d4e1f14725eac2702a4d5d02d5990b90d51bfb2e058dda94f82f867a6474e); /* line */ 
                    coverage_0x690caa01(0xed2eddf60ae3d0f9c24fb7687471690b0f845e361b61d9910d274b147a5cd6d5); /* assertPre */ 
coverage_0x690caa01(0xb5870accac26db93c8fd06e58bb41fd303057a644d76a42f1651634ad0d98202); /* statement */ 
assert(accountLayout == Actions.AccountLayout.PrimaryAndSecondary);coverage_0x690caa01(0xd4c35b0f3dac295e131eab867e40a2775d311ec8230788dfc284c93928db4527); /* assertPost */ 

coverage_0x690caa01(0x8cc815102f697ed955a05bc3991a97d8708e5e51885f07eb29fed42731a91f42); /* line */ 
                    coverage_0x690caa01(0x1d6f1711bf9b8024bebf734adc5b8327a738a6b63ce1197bbbecf91102a5033e); /* statement */ 
Require.that(
                        !primaryAccounts[arg.otherAccountId],
                        FILE,
                        "Requires non-primary account",
                        arg.otherAccountId
                    );
                }
            }else { coverage_0x690caa01(0x9e0d76721924958286cb618d45f1ecb18cf9b29ddda4cf5c0000a278dcd911f7); /* branch */ 
}
coverage_0x690caa01(0xf09fe494eaaf6db615758f36609935e8c69674522632c635cc2556f2ab261192); /* line */ 
            coverage_0x690caa01(0x1de359ad4e9a333fd236d6e4fd03c2bed5fad275180931a00fd3c9abb0bedbd6); /* statement */ 
primaryAccounts[arg.accountId] = true;

            // keep track of indexes to update
coverage_0x690caa01(0xad94e2bff968bcf857f8657893edf5761fec455daa08f7daf9929349e43b1cc7); /* line */ 
            coverage_0x690caa01(0x2b8d09b752adea22189714d950a45952d4a7acd707863e07984e6bee92838fb6); /* statement */ 
if (marketLayout == Actions.MarketLayout.OneMarket) {coverage_0x690caa01(0x28db86f52dd19688b8d0b7165a0093a53ec98f7934d149237e43f5dbac62ded8); /* branch */ 

coverage_0x690caa01(0xb78f2cb0175b472e9e7032f967ec60a486f27de603b5d9ee0c48d6db112c405c); /* line */ 
                coverage_0x690caa01(0x74c4723bcbf8c7bee72c77eb9556ac3edd9964e6496ff645d4d5445dee4b6b29); /* statement */ 
_updateMarket(state, cache, arg.primaryMarketId);
            } else {coverage_0x690caa01(0xc9e960aad66120695a5573ebd3b3cfa0c7f5852b3eed4e9eecad521cb6000a57); /* statement */ 
coverage_0x690caa01(0x3e235c7eb62591c6b09181909ea493d615a61586bdbbf651b55f2ccef2f17028); /* branch */ 
if (marketLayout == Actions.MarketLayout.TwoMarkets) {coverage_0x690caa01(0x470bec5c286489c05462bfb750664a6c63c6909c5a7b406c7666dd98ec456091); /* branch */ 

coverage_0x690caa01(0xc3d4a064128954c4cdaf6055ce649b444e1ca4b47e508454bba362b4a4bf4b40); /* line */ 
                coverage_0x690caa01(0x52bc7f9deea0d652ce0c7b25f07d1e9baf7e3357a1d0f33a7681d7069337cfe9); /* statement */ 
Require.that(
                    arg.primaryMarketId != arg.secondaryMarketId,
                    FILE,
                    "Duplicate markets in action",
                    i
                );
coverage_0x690caa01(0xa2b1079ccdc0e3793b2cb6b47926076bb029f26080d45b389679abc310b52c7a); /* line */ 
                coverage_0x690caa01(0xbf978194b8136af6d781e7ac5253b287030ae455a5725d342692c25d046ea2b3); /* statement */ 
_updateMarket(state, cache, arg.primaryMarketId);
coverage_0x690caa01(0xdee49ea31d7c7ed3084f7a9dc6ab18f1232f80cd55f8bd56dd785e0fd97a62ff); /* line */ 
                coverage_0x690caa01(0x5f18d7604ede709e7b99acdb5efbf73fe053ec7fda0ee6dadef618b939344d57); /* statement */ 
_updateMarket(state, cache, arg.secondaryMarketId);
            } else {coverage_0x690caa01(0xfc8808701d803780314c1be4af368feaf0639541192ddebb434593b1473ad168); /* branch */ 

coverage_0x690caa01(0x36141c5188af8290fb0e04159fb881d672a115144c97947f5fcd2309ebb1b4b4); /* line */ 
                coverage_0x690caa01(0x08f06c7eb9d6a7f41e1fdd2ea7cf5dbbf848fb566d1e23098f95d899e2222fd3); /* assertPre */ 
coverage_0x690caa01(0x29dc95c56d75f4aa487792860b15a566beea3ba0f1fba98b42eebb6017626d4a); /* statement */ 
assert(marketLayout == Actions.MarketLayout.ZeroMarkets);coverage_0x690caa01(0x291c3431453a0b0b44aa3a8483858f3c4fddeefe0da904674bb3d6404c1162b8); /* assertPost */ 

            }}
        }

        // get any other markets for which an account has a balance
coverage_0x690caa01(0x605c84c9992fcd39fcbdf5b60751f7a724d3b26bd2a7bf8e419eb48ddc11da6e); /* line */ 
        coverage_0x690caa01(0x648b0be43c087db213cbe65f64d38fb53a8f946704aca8388a97fa22486d05be); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0x690caa01(0x87c7d313aab80469105fc1fc48e18f0cdffbcca6e6ac0a805e2b8d8193119a66); /* line */ 
            coverage_0x690caa01(0x9957c1f13c708f30f8675b868b5ca078e48c8654eec83e55ccdd3f7976fc8cd1); /* statement */ 
if (cache.hasMarket(m)) {coverage_0x690caa01(0x8484ce38e11c7f8b28e9990e29536b06d6810742f08392b306d6e7129d8f02b6); /* branch */ 

coverage_0x690caa01(0xa251293ef88bd0a66d3eb177e8b85e08095565787baeff4a2abe977cbedc1382); /* line */ 
                continue;
            }else { coverage_0x690caa01(0x6ec57d28245b050c19f4472137cde2ae8cca00925db97653d2166cf1a83d3879); /* branch */ 
}
coverage_0x690caa01(0xf09d15c2c443b7b67eea7252d28ea86e0401b478cd2fce7c580aa7c865975c78); /* line */ 
            coverage_0x690caa01(0xd97fffdab89d25d1849ef23fad463e380a4999c6ea595ececcf4ac8c53a744a6); /* statement */ 
for (uint256 a = 0; a < accounts.length; a++) {
coverage_0x690caa01(0xfb6831839446890c3da3be85609f39e02349ad64b56c527d2522972d05b9900f); /* line */ 
                coverage_0x690caa01(0x8b8b3bb2f3ae0515581e8bf6bbb221391fc3e4e63f3b9280b533fcc834e63ca6); /* statement */ 
if (!state.getPar(accounts[a], m).isZero()) {coverage_0x690caa01(0x4a0aa80702863e9287fa82541d7ce2aa0f398a1c87b686d67adcd90f4f24aa81); /* branch */ 

coverage_0x690caa01(0x010bc93a3884f014c7f812dd4732d63c8245120afd6e862af1af3a6dc5473aa5); /* line */ 
                    coverage_0x690caa01(0xf43cbcb388beaf09ee98741500cd173655381beed9354dad659e526d899b8510); /* statement */ 
_updateMarket(state, cache, m);
coverage_0x690caa01(0x734527ef1ff388062f7ab81c58f6643162303b36a676c66b2a0c76abd9932a1b); /* line */ 
                    break;
                }else { coverage_0x690caa01(0xe1fb75f417af5e978c3f71e21eda31a070154df15de543663c3e2415b0ccde7e); /* branch */ 
}
            }
        }

coverage_0x690caa01(0x0e333cc9ceb4f5a67b6dd22ded07d525bee43bb88cfe8fe197ca95a1f792d461); /* line */ 
        coverage_0x690caa01(0x53742eba628bb950ab82c6331d5c23ba071f353e8a80c7d207edb7ac2ddf48b3); /* statement */ 
return (primaryAccounts, cache);
    }

    function _updateMarket(
        Storage.State storage state,
        Cache.MarketCache memory cache,
        uint256 marketId
    )
        private
    {coverage_0x690caa01(0xc7f286d6284b90bf35a54b9930f1c6371e0a92bc1381d4d0d3dbf1740181766b); /* function */ 

coverage_0x690caa01(0xbf6c8f2dec0b4a8683e23efde1809af7a72dc72c958ce932dfc24d964f87191f); /* line */ 
        coverage_0x690caa01(0x60a2f12bfdcd2d74bd741d0e25e85a605c1d0cdc79c842fef5a58bc9535d4921); /* statement */ 
bool updated = cache.addMarket(state, marketId);
coverage_0x690caa01(0x82877cb6b2528bceb6f4d09605a3d12d9c958073cd98a937c10cd83d8ad86b96); /* line */ 
        coverage_0x690caa01(0xd898d8ef105465cf7e469bd6aa1bc34d063c345e3dfb9af92ffb54175d745305); /* statement */ 
if (updated) {coverage_0x690caa01(0xca22048703f42a61a347b21ef37e19c0b0c73b50093e1e72d6bc367e1109a2f8); /* branch */ 

coverage_0x690caa01(0x06eed5b296b5d58efe17fa11d66b20520441a523a4d6ad3dae9a2f3da158e9d5); /* line */ 
            coverage_0x690caa01(0x18b797e00c1b238c5ea9c6087a02ad5728098c398e47afcd4f7147c9b9cc20c1); /* statement */ 
Events.logIndexUpdate(marketId, state.updateIndex(marketId));
        }else { coverage_0x690caa01(0x0e16ffae7872d1f583bb01232b2d6cf9da9fc48c4ae56dde683934dd0d083853); /* branch */ 
}
    }

    function _runActions(
        Storage.State storage state,
        Account.Info[] memory accounts,
        Actions.ActionArgs[] memory actions,
        Cache.MarketCache memory cache
    )
        private
    {coverage_0x690caa01(0xe88da519421b7c055f31da7a96539e3623463a72875b4dcb6ab47ca436dbf3c3); /* function */ 

coverage_0x690caa01(0x5f750d414f50fb21e64e32aa4968a4988f515caf5e09c2ef5f84df901025dd4d); /* line */ 
        coverage_0x690caa01(0x513d736ef130f5a3e9e11657ba0a5429cc3a0753fe105b450edf0e3f72a89a19); /* statement */ 
for (uint256 i = 0; i < actions.length; i++) {
coverage_0x690caa01(0x5c4ce7c7681d61b5b19503369a3cb5a0162f4b21b47b99c8eb10bf25a7bdf168); /* line */ 
            coverage_0x690caa01(0x3df22710c989bfcbe2214c39435b405f2044c4d7f2dfa632d0a463f8a0d629f1); /* statement */ 
Actions.ActionArgs memory action = actions[i];
coverage_0x690caa01(0x0000227939ec67bfd8839f3f3b4c783ab86aeeeb13985b5ec7c47839a4fe9083); /* line */ 
            coverage_0x690caa01(0x795fc6291f04333ee1641c987f3b44c31575ab533c27ef698d2b7542ca9ebd98); /* statement */ 
Actions.ActionType actionType = action.actionType;

coverage_0x690caa01(0xa8aa5b4b8c4b2c631525bb833fd46f600472d7021e0cd7577eba9b38139f2ecb); /* line */ 
            coverage_0x690caa01(0x891970a694bdea3149a9d4eab97ae0ba2756cac845716a017a559a8419b8b183); /* statement */ 
if (actionType == Actions.ActionType.Deposit) {coverage_0x690caa01(0xe8aa64c58517c6738e4d0f22891a6bd115b7c05e39b46c72b0df933cf1605051); /* branch */ 

coverage_0x690caa01(0x4e21aa43af44a3dcdd4c8bb09fea9d30278b2cdbb5aa6c759d9374b2a6ee059b); /* line */ 
                coverage_0x690caa01(0x7ff926c2fe7ea20144aa98ba6e92fea44349bda33beb250438fd3c35503afb23); /* statement */ 
_deposit(state, Actions.parseDepositArgs(accounts, action));
            }
            else {coverage_0x690caa01(0x746fa522d60dfc0cc335833a287ea16cdd8f9a2a2e0240b55b55a8c9c99e680c); /* statement */ 
coverage_0x690caa01(0x22f055c3203e52436d6e47a5ceeb7cd5e60fc04a0f305b618decc2831116ca5d); /* branch */ 
if (actionType == Actions.ActionType.Withdraw) {coverage_0x690caa01(0x798fa480c0e39f5f569a58a954d88b890ed6639ee9b98798e0b6e2ac96367f52); /* branch */ 

coverage_0x690caa01(0x973064beae532fc859284be3e21df0d6317665ae521175a1044eb97f8ff2b76c); /* line */ 
                coverage_0x690caa01(0x40625d4c37c75ab0d2060d1690cb2089e98a72e73ccf8aa4da71344db3ff302e); /* statement */ 
_withdraw(state, Actions.parseWithdrawArgs(accounts, action));
            }
            else {coverage_0x690caa01(0xba4c38359df192e1f0578ac980325cb47b4f03615969df1b744f37bc606fdd9a); /* statement */ 
coverage_0x690caa01(0x23f57b206b3e4e377e4bfd1e0baf215b6116aba78a4266b9e77a490883f1e866); /* branch */ 
if (actionType == Actions.ActionType.Transfer) {coverage_0x690caa01(0xae15c874b572744791924d2a58bab90978c12986679e86092b709709a18a580c); /* branch */ 

coverage_0x690caa01(0x4ed9507bc1f45a927e4e8b2e0b1a66c926abc0d7ee20479b5506be26adaae46b); /* line */ 
                coverage_0x690caa01(0x3a41219ee9e3e011d8a9a3071d248d7ddaf344c787ee1b85efce8044227713e5); /* statement */ 
_transfer(state, Actions.parseTransferArgs(accounts, action));
            }
            else {coverage_0x690caa01(0x63873aa2bf915aaf98a167ffd167fdb8f71355871da51dc6a33ae131baca431d); /* statement */ 
coverage_0x690caa01(0xa2317983950d39c60ccd7be17919e3836f6561d0c9811704600775bc365d6836); /* branch */ 
if (actionType == Actions.ActionType.Buy) {coverage_0x690caa01(0xf6fbf46347aa70d4176669cd4049a272440cf753eb00e288066c1c432d4af927); /* branch */ 

coverage_0x690caa01(0xd5d32e3986792463d4274ad8b6272b670d1d408a8ce4de57f3d3c018681e2d6d); /* line */ 
                coverage_0x690caa01(0xcbe355c04b3408300ef164ce63728d1a9570783bad30221c3ac189c735289071); /* statement */ 
_buy(state, Actions.parseBuyArgs(accounts, action));
            }
            else {coverage_0x690caa01(0x8bd84cd191d7362eb6c3c6593032becdd51c64469477df95d8a7566781302c11); /* statement */ 
coverage_0x690caa01(0xc906303c79a391c0526b6c98107d087638b5d2b9360523610b533a798605a6a7); /* branch */ 
if (actionType == Actions.ActionType.Sell) {coverage_0x690caa01(0x1fab31ee8a2ae612cf756670daa59d4bbaa8018c011b3cdb24c0ab244306b274); /* branch */ 

coverage_0x690caa01(0x9d3a08050459e4e6173a90e29343c2afd7faebdfcc8bc1f61ba8ee366bb47f4d); /* line */ 
                coverage_0x690caa01(0x45eed03d9124f688648f9f01b5bb1a41581c0f30ab5e6a67070952476b01f05d); /* statement */ 
_sell(state, Actions.parseSellArgs(accounts, action));
            }
            else {coverage_0x690caa01(0x8ee6f905f0c584be3f2dcc6e0e11745674ee9436d3dcc08aadfcfc18218440b3); /* statement */ 
coverage_0x690caa01(0x8658895d9aac475b56c42236d79c4d9b172df1f9a50bfdcebeb6decfd5771260); /* branch */ 
if (actionType == Actions.ActionType.Trade) {coverage_0x690caa01(0x9a7e1de260011baf8503305710dfe1be1f6145cc907be4678a2b37193378cad8); /* branch */ 

coverage_0x690caa01(0x6382f3ed59d3c1e3211b0aa3e240fe9426809b3d1a5e91d3a4de2bfa0092390c); /* line */ 
                coverage_0x690caa01(0xef6f005c565832884248db463c49fbb6cae61bb3e4321d2f255a9e3cea70b222); /* statement */ 
_trade(state, Actions.parseTradeArgs(accounts, action));
            }
            else {coverage_0x690caa01(0xf40ce3dfee8b69b515a93a2166be2bde0d26e4f8a4b4de2391ad48040a266dd1); /* statement */ 
coverage_0x690caa01(0x51a5d4af9033a9c6bbad54f9f49a66dfb988b387c3f1b55f2cc3cb8410473494); /* branch */ 
if (actionType == Actions.ActionType.Liquidate) {coverage_0x690caa01(0x699844d890ee66c70132e88c2366e9eff147421672f61026136a89154eb95bbf); /* branch */ 

coverage_0x690caa01(0xc2dd000a412481346c9050ac0f3105e354defcdad047d8143637434905f11c12); /* line */ 
                coverage_0x690caa01(0x4969b58a2f909b264887714e94e6daf2bf52d9a608313e17779cf6dc97ccf804); /* statement */ 
_liquidate(state, Actions.parseLiquidateArgs(accounts, action), cache);
            }
            else {coverage_0x690caa01(0x283e35dc5468553d332f68f064ddf4205e2a4f07c1bf90e1462ffb619f5748a5); /* statement */ 
coverage_0x690caa01(0xe58dc436308d3c89f6512a5134e10b7a23bdc94c89db801939a4628ee7471d95); /* branch */ 
if (actionType == Actions.ActionType.Vaporize) {coverage_0x690caa01(0x06754662acd8aad9b7b096c26398426cb4a5c3567644f8eecb9f77bd89c34701); /* branch */ 

coverage_0x690caa01(0x96f446fd45e39f02ff0828654b50f2ae1db7998725370b42b12ac5f2248fb1f8); /* line */ 
                coverage_0x690caa01(0x6f8ec20d0ae12362b121a5717ef504b3b1f8f92f90e4a665ee31fc10d07362af); /* statement */ 
_vaporize(state, Actions.parseVaporizeArgs(accounts, action), cache);
            }
            else  {coverage_0x690caa01(0xd61c1c9744514268019e7425c05a7be2bdeab8fe44c91d9250e3044a7c1c0a19); /* branch */ 

coverage_0x690caa01(0xb731d349d8a160cb87d3096833e14914b6a9cca391027555b00205a98e525a24); /* line */ 
                coverage_0x690caa01(0xf0725cedd8f8aae4cb9b59d4e6b22cb70efc725ed3482a1f97a4b771fa9ad0fc); /* assertPre */ 
coverage_0x690caa01(0x4e576a749a2e5277a1852ca9969993fb9562a107265e8c00d24ada0fadc04680); /* statement */ 
assert(actionType == Actions.ActionType.Call);coverage_0x690caa01(0xfa367e91be3f55d132da63f0e034154ad8340e6915b845479391feb79232823d); /* assertPost */ 

coverage_0x690caa01(0x7125681834d05d3b2b616830dec5e8f78d8534f4e3062324df420de796289e39); /* line */ 
                coverage_0x690caa01(0x9b21f7789eda8ac83352bbdf0f94e0f42f03249f2580b55f861a61c9f2df2958); /* statement */ 
_call(state, Actions.parseCallArgs(accounts, action));
            }}}}}}}}
        }
    }

    function _verifyFinalState(
        Storage.State storage state,
        Account.Info[] memory accounts,
        bool[] memory primaryAccounts,
        Cache.MarketCache memory cache
    )
        private
    {coverage_0x690caa01(0x7c7b290864f2169f16c7f49c85cab2f913d67c59f3e04981a01d270dc1f48a68); /* function */ 

        // verify no increase in borrowPar for closing markets
coverage_0x690caa01(0x3448769418900b619050f6d064b7f38147e894be18347a407c804f11383adbff); /* line */ 
        coverage_0x690caa01(0x3e5e5dbf03e3693dcfe6298170adb4b297bdde131ff082db6d3d9b388767e384); /* statement */ 
uint256 numMarkets = cache.getNumMarkets();
coverage_0x690caa01(0x7819dcb03e3662423082470e61087b4da458edd2950667b3e3609898810d620c); /* line */ 
        coverage_0x690caa01(0x082a53d1d2942e50f1ae6d4959f69b5b6bfad75acd8c99cd94630614b6151fec); /* statement */ 
for (uint256 m = 0; m < numMarkets; m++) {
coverage_0x690caa01(0xbc214d3a10ce69eb1ab18119875d3e1fe07e6c78c9e4edd457b5efd97847bf2f); /* line */ 
            coverage_0x690caa01(0xe5811bbc117fa8f60bf8a0ac6649c696e9fba7f8c38ac2ee53ec75315b269373); /* statement */ 
if (cache.getIsClosing(m)) {coverage_0x690caa01(0xeb40727c892a2690b47657b1fabf4d8171b913b7098834408e905e601b86b1f9); /* branch */ 

coverage_0x690caa01(0xb1e691ef7830b36867091c93e47167ead113c4999c59b70493761c4552f2f7e7); /* line */ 
                coverage_0x690caa01(0x40fd7255bbebd69d4d8b52a2ec4ba622ec4bdc305d54b7b7f61dffdbc53580a6); /* statement */ 
Require.that(
                    state.getTotalPar(m).borrow <= cache.getBorrowPar(m),
                    FILE,
                    "Market is closing",
                    m
                );
            }else { coverage_0x690caa01(0xdaa788b421565b3d1b8ca43767865a044c8adc82b85f7b1b9ded903c82f992dc); /* branch */ 
}
        }

        // verify account collateralization
coverage_0x690caa01(0x1d4c8082ea58b153fa5d02e66a680e3acdb9e5d497dc21197c64266d95f04281); /* line */ 
        coverage_0x690caa01(0xda59ee960d12517e19921d009411b129e9619d455177aaccdea640889a8e90c7); /* statement */ 
for (uint256 a = 0; a < accounts.length; a++) {
coverage_0x690caa01(0xde3ac17aeb7a1f9c84ef0280be9440cd2f9c092c602e90b8ef1d2e8ca5e4e4c8); /* line */ 
            coverage_0x690caa01(0xbf1c1233eedf0a594461055412948d9bff4515900525869f4dfdd7c6d8eac05d); /* statement */ 
Account.Info memory account = accounts[a];

            // validate minBorrowedValue
coverage_0x690caa01(0x20e3c1a4be9e6b8cdb95dc3c92345566f95988af7bc5275268907b8ced5eeda4); /* line */ 
            coverage_0x690caa01(0xd85ca8e1d9cea1aac84f33c21a99ed7ea5c510bb9f20caa189dd3cc0d188a621); /* statement */ 
bool collateralized = state.isCollateralized(account, cache, true);

            // don't check collateralization for non-primary accounts
coverage_0x690caa01(0x0890da2e4a8e86910270960e6b8bfa6dd86687ad065c365b46795a68ca5704dc); /* line */ 
            coverage_0x690caa01(0x448406d6680e2e994e93b67f12bda4e27c87b8babdf39679a7c8054f9eeaabb3); /* statement */ 
if (!primaryAccounts[a]) {coverage_0x690caa01(0xf7a3e18ed0ff1b821a3c4ae75e8dbdd21b7095dc81c1e5664b9a8b0d2035b477); /* branch */ 

coverage_0x690caa01(0xd039687b7101682106d7dad23400fc8542941462c5077d0a0d92da46e80acad4); /* line */ 
                continue;
            }else { coverage_0x690caa01(0x65e73539b93c37348e146ab35c60b4d909ac5fe06570624eaf08a7f2289cb801); /* branch */ 
}

            // check collateralization for primary accounts
coverage_0x690caa01(0x2be816d2de73897cb1c4ac365f9cdfab006139b46fdff7983e87efb16bfd6c62); /* line */ 
            coverage_0x690caa01(0xe81d45c05ebdef81c2b20d7adfc266f2c9805918716ff6a487c49bca3f00723c); /* statement */ 
Require.that(
                collateralized,
                FILE,
                "Undercollateralized account",
                account.owner,
                account.number
            );

            // ensure status is normal for primary accounts
coverage_0x690caa01(0xdcc54117e80338679da25c02b342a7ec0404b536eb2422bc27bf7871bdd3fe58); /* line */ 
            coverage_0x690caa01(0xe2bca85dd56d19d375b231a244a8c363e3faa8e7822a832393ffb512c6aeb5bf); /* statement */ 
if (state.getStatus(account) != Account.Status.Normal) {coverage_0x690caa01(0x0bbc1714339ee73b67e0703ed2f532c431de50916ef0f55d55c1644e381e7ddf); /* branch */ 

coverage_0x690caa01(0x94672a7ee5b391153468fd6e5c361035d348004cd14db4c9525e17b21928d9bc); /* line */ 
                coverage_0x690caa01(0x48149a32d116c35efcc12599b60ee4bf60afb98baadc1b8ce5e8a0e6eca85c4f); /* statement */ 
state.setStatus(account, Account.Status.Normal);
            }else { coverage_0x690caa01(0xbb061835931f1d7cc3405c95aa10137fca4c37467feb5361362b6dcfaf2e56c5); /* branch */ 
}
        }
    }

    // ============ Action Functions ============

    function _deposit(
        Storage.State storage state,
        Actions.DepositArgs memory args
    )
        private
    {coverage_0x690caa01(0xb0b75cea3441faf112e2106a0565371bf7449600e0944d64405609b228d380f2); /* function */ 

coverage_0x690caa01(0x0dc0b78da4a723a6adbd0f8b880b5c5b936da8bc9e5a1157088e45cfb0c9c6f0); /* line */ 
        coverage_0x690caa01(0xbfbe9f68b8e186095e13a61d82d3495845dad42db7b0843edf912b0737f1ca14); /* statement */ 
state.requireIsOperator(args.account, msg.sender);

coverage_0x690caa01(0xb453ab599afc19183acf4c200bb0f77d0b53cf08750d8e67eef9bf66b1e48776); /* line */ 
        coverage_0x690caa01(0x33b4b09c36bb2fae2479705957f2d2de416b058b9db81370c0124de27673cd54); /* statement */ 
Require.that(
            args.from == msg.sender || args.from == args.account.owner,
            FILE,
            "Invalid deposit source",
            args.from
        );

coverage_0x690caa01(0xa07a3f4424da92afdf9792f01db9c2524b9b09ae1a74329aaf93f0cd5235e136); /* line */ 
        coverage_0x690caa01(0x82a3b06a3b89b6074da6c5c202cc1e05abdee24b0704f154f0b3412931cf53c4); /* statement */ 
(
            Types.Par memory newPar,
            Types.Wei memory deltaWei
        ) = state.getNewParAndDeltaWei(
            args.account,
            args.market,
            args.amount
        );

coverage_0x690caa01(0x8ecfa0e9dfafb0f4c82c226d49cda34fe72f674609b2c1687dced1ba34e9ce39); /* line */ 
        coverage_0x690caa01(0x0884288d2d7b8da5aa4cbb5483292fe20f0ffbc50b61d8e46f4753ff2f07d2e7); /* statement */ 
state.setPar(
            args.account,
            args.market,
            newPar
        );

        // requires a positive deltaWei
coverage_0x690caa01(0x15d5844c0107a356165b0793e84b3fb7e88983187d86f275fc75424a277c24ea); /* line */ 
        coverage_0x690caa01(0x71b034a5da163a811b8acf1123085a3284388f537545f5247152f58d9c5858e8); /* statement */ 
Exchange.transferIn(
            state.getToken(args.market),
            args.from,
            deltaWei
        );

coverage_0x690caa01(0x791ee88eb3525544a71182d2afa6e11b344c9a77b853d870f7b1d213780f62e2); /* line */ 
        coverage_0x690caa01(0x08cc1eacfd89b5e6d6037dc2e45270a2fbc982605c8a164bdb0d6acfe5f078e3); /* statement */ 
Events.logDeposit(
            state,
            args,
            deltaWei
        );
    }

    function _withdraw(
        Storage.State storage state,
        Actions.WithdrawArgs memory args
    )
        private
    {coverage_0x690caa01(0x6a13e5dd200d2d418ddc2be3847e18c786e17d697db89ca6078fd8ed9d693509); /* function */ 

coverage_0x690caa01(0xbb933003991f2cdbc79824cc6de84cf7f594344642550b1ca2daa04084ba56ba); /* line */ 
        coverage_0x690caa01(0xa5fda74fe3cc13c95389a3196bcbeb0d490f50cd53e74afb7f0ada83d1d36969); /* statement */ 
state.requireIsOperator(args.account, msg.sender);

coverage_0x690caa01(0x6f1d8336391e861a16723c666355e9d494b207b025335e43c9c9cdf63dd63241); /* line */ 
        coverage_0x690caa01(0xbc957ea8c03ea3bc42cce523fdb1e681a81b696241b1e794c491c8c57e4706b7); /* statement */ 
(
            Types.Par memory newPar,
            Types.Wei memory deltaWei
        ) = state.getNewParAndDeltaWei(
            args.account,
            args.market,
            args.amount
        );

coverage_0x690caa01(0xe9b77020532d22afbba2b745fb38a2dd6c7265e681943d8605511c4c3b06a009); /* line */ 
        coverage_0x690caa01(0x6b45ecd665750bec20e20036f2e56493dc4d96291fe41bad7f445c146f7929f2); /* statement */ 
state.setPar(
            args.account,
            args.market,
            newPar
        );

        // requires a negative deltaWei
coverage_0x690caa01(0xaeab6bfb4370b95c9988d698ccec5f2ba9cb4b93647c758e70a297199815e5d9); /* line */ 
        coverage_0x690caa01(0x0fada779d1bfd4d82237b7539b829dd346524c9e82929719ae203208ca7cd71f); /* statement */ 
Exchange.transferOut(
            state.getToken(args.market),
            args.to,
            deltaWei
        );

coverage_0x690caa01(0x19ca96dcff3eef87cb99bb77ab8889dfd8af9a6f04dfbc2cc77726837d69eb2c); /* line */ 
        coverage_0x690caa01(0x9548643c6ba08f20fd4a534f97dea311be753b4af6f4d48198a33ab2b1350e5f); /* statement */ 
Events.logWithdraw(
            state,
            args,
            deltaWei
        );
    }

    function _transfer(
        Storage.State storage state,
        Actions.TransferArgs memory args
    )
        private
    {coverage_0x690caa01(0xa3bf26505f2983cc8819528b03aa2d7407171cbb818ae9352e2cf52408a556ac); /* function */ 

coverage_0x690caa01(0xf50a53ea0988df17e4a801391b4d9b89322eabe97eed20e8ba18f435425ccd19); /* line */ 
        coverage_0x690caa01(0x20954b46a60ed7f27c246308f148c0d0bfec7aca164ef46b98df91fd19a57647); /* statement */ 
state.requireIsOperator(args.accountOne, msg.sender);
coverage_0x690caa01(0x0fd49a1627b7f3f9c536f1b44f2464636136fb02c68a866ce7fd198c15d92928); /* line */ 
        coverage_0x690caa01(0x4aa54857ea66a849963f206f86b5fa46cd0f7c5a103cdfd5ed23a1669c80ab0e); /* statement */ 
state.requireIsOperator(args.accountTwo, msg.sender);

coverage_0x690caa01(0xa289a0e67bcfb81821e8ae1e196d2ebbdf87979bf39ae830d190aeee43ce0977); /* line */ 
        coverage_0x690caa01(0x1b95159afebec187759980e77c0b453b6086778902fab284522c825684f05aaa); /* statement */ 
(
            Types.Par memory newPar,
            Types.Wei memory deltaWei
        ) = state.getNewParAndDeltaWei(
            args.accountOne,
            args.market,
            args.amount
        );

coverage_0x690caa01(0xe6100060ae9469a215a03d5bacebccd1d492196e0ea49bee250e115ccd976e9f); /* line */ 
        coverage_0x690caa01(0x89805a69aeb0d48a40f94c07720aa4f787bd027b740c2a2c9ebe0776227a4a22); /* statement */ 
state.setPar(
            args.accountOne,
            args.market,
            newPar
        );

coverage_0x690caa01(0xa9aa38ee6b85f86b8b393c8caed5b8fd21791f87f9c29f6811b6760f68dea31a); /* line */ 
        coverage_0x690caa01(0x6dbcf1c1c0e93f878c6b43b3178243a62addd4e4afa5c67da011130f7dae1508); /* statement */ 
state.setParFromDeltaWei(
            args.accountTwo,
            args.market,
            deltaWei.negative()
        );

coverage_0x690caa01(0xb8acdea340a535519169eb30c82df0daa845201fa1b32b7e4f54c748658986e9); /* line */ 
        coverage_0x690caa01(0xfb3e252ee501a11b3ca7220037cde56cdd8702bed3fea365ff0ee29d3a26c016); /* statement */ 
Events.logTransfer(
            state,
            args,
            deltaWei
        );
    }

    function _buy(
        Storage.State storage state,
        Actions.BuyArgs memory args
    )
        private
    {coverage_0x690caa01(0x95c4a781ae65be6bc108e402105601796fad243001b24841f6010c27fd1f52ba); /* function */ 

coverage_0x690caa01(0xdeb18dc27485364c930750d6eac2434dd600d5bc94bd9f5136a0f7a29383e666); /* line */ 
        coverage_0x690caa01(0xa29e649ae8a8eb4487cfcb41dd700b4594f9abc9f98c5a12aa5b37f5a885743c); /* statement */ 
state.requireIsOperator(args.account, msg.sender);

coverage_0x690caa01(0x697ea3582b6e1320ac416747c16a4d3898d3d3bfe51ef55a1921644b45273084); /* line */ 
        coverage_0x690caa01(0x7b3620bac42fcaac808f5f509b31947fbcdf4aa4121be90eae212b51033b16c5); /* statement */ 
address takerToken = state.getToken(args.takerMarket);
coverage_0x690caa01(0xd5470ac8a6f18d72f592b23cf4c2c38f7531c9aedd5677ad39cf0c27050a992b); /* line */ 
        coverage_0x690caa01(0x2e1f35ad5fe966f1366d4edbcc771f5fa4e6f43046b0a4738e44b7986a0bae78); /* statement */ 
address makerToken = state.getToken(args.makerMarket);

coverage_0x690caa01(0xfc8fbf85c42958cad9018ee3478f854fcb0d216a62802a61131f401ba2676bd3); /* line */ 
        coverage_0x690caa01(0x563de262eeb6081550cf9c5787c40e47b50b704098a206639a208162974bdc00); /* statement */ 
(
            Types.Par memory makerPar,
            Types.Wei memory makerWei
        ) = state.getNewParAndDeltaWei(
            args.account,
            args.makerMarket,
            args.amount
        );

coverage_0x690caa01(0xad7ea606849c2652856113254cf72f1630e8520750dcecbcf846ac657aab3c15); /* line */ 
        coverage_0x690caa01(0x1832894718a0768b2d052978d3235c1fdd71f679e02da119d4b4a8b37c90345b); /* statement */ 
Types.Wei memory takerWei = Exchange.getCost(
            args.exchangeWrapper,
            makerToken,
            takerToken,
            makerWei,
            args.orderData
        );

coverage_0x690caa01(0xc6ca846e6fb91f9a700af3e4b0897ac491ac2c8ca121c1a616d84daecf1a0655); /* line */ 
        coverage_0x690caa01(0xf741c2f7f265e840e2c47ee7c359a4e356b1d6bea977fe87f3c4c261fb836bd6); /* statement */ 
Types.Wei memory tokensReceived = Exchange.exchange(
            args.exchangeWrapper,
            args.account.owner,
            makerToken,
            takerToken,
            takerWei,
            args.orderData
        );

coverage_0x690caa01(0x287c787aef529a64bc5cad3b873ed59f3d96314fb7d926e723b002ac32b0059b); /* line */ 
        coverage_0x690caa01(0x3f49bf5daf5fe3666a07d7161d5b8973447f5474acda376c8da8eb2eef03226e); /* statement */ 
Require.that(
            tokensReceived.value >= makerWei.value,
            FILE,
            "Buy amount less than promised",
            tokensReceived.value,
            makerWei.value
        );

coverage_0x690caa01(0x85cd5990a85201d76546183cd2206d17b8f284cd815bb57d9f47366b4728d346); /* line */ 
        coverage_0x690caa01(0x1f1681691050a5c8476a7b5d5721686f0242f67426d6b92b8c49da4382b34c4f); /* statement */ 
state.setPar(
            args.account,
            args.makerMarket,
            makerPar
        );

coverage_0x690caa01(0x60b82152ea08aea93a8edb8ccb87df9725cbea7c24caba112122b12dd84486e6); /* line */ 
        coverage_0x690caa01(0x77125a43707a2c63434b980d3bd306e9b7adee178b5be7de6bf6f38400a5a116); /* statement */ 
state.setParFromDeltaWei(
            args.account,
            args.takerMarket,
            takerWei
        );

coverage_0x690caa01(0xe96e4f74ff9f07fe795773cbb025e2347966c02d19b68469f651578b7c83faa1); /* line */ 
        coverage_0x690caa01(0x037a4547ea8f6d7fe6c651389216684957835cff75ec8eea5506466a34066fd2); /* statement */ 
Events.logBuy(
            state,
            args,
            takerWei,
            makerWei
        );
    }

    function _sell(
        Storage.State storage state,
        Actions.SellArgs memory args
    )
        private
    {coverage_0x690caa01(0xd14a85442bf1a556374999b958f0928b9742c1ad23557a92dec82dd3a8d4def8); /* function */ 

coverage_0x690caa01(0x5d0232f145de2435f656ac19e0bb2fe1a6ad174ff06afb7fad0cf91770ac6508); /* line */ 
        coverage_0x690caa01(0xf8c2d8411a1fd4d832dba1f79e0165dcb40d534abe70c97dc208039b7575f0b6); /* statement */ 
state.requireIsOperator(args.account, msg.sender);

coverage_0x690caa01(0x5c580812d54652ec6fc0de02af51567df16bf1eab2848922be7b948b602172c1); /* line */ 
        coverage_0x690caa01(0x3777d97d4bbc9388753621569d298e188e0800c3415597dd51b924a036f60e4c); /* statement */ 
address takerToken = state.getToken(args.takerMarket);
coverage_0x690caa01(0x68706edcdcb0628f69441c4bad7f880e77fe57e3d8f0660570e567a46e5f2666); /* line */ 
        coverage_0x690caa01(0x7d254da81ef2b2f5737adcbcc425621e0a41f1d2ee1490c8499bf2c8b0e80edf); /* statement */ 
address makerToken = state.getToken(args.makerMarket);

coverage_0x690caa01(0x34da0b5c94625545786140a40ac6cd8b3578528cd50813c3263045d88d1b2079); /* line */ 
        coverage_0x690caa01(0x6fe108b7b1dbc63188aaf395bb64fde94178cc263ae95b4b247ed4170ff3822d); /* statement */ 
(
            Types.Par memory takerPar,
            Types.Wei memory takerWei
        ) = state.getNewParAndDeltaWei(
            args.account,
            args.takerMarket,
            args.amount
        );

coverage_0x690caa01(0x31cf7f6642f434aab894a933678dcf73012575381153f08d117ff50085a9ba56); /* line */ 
        coverage_0x690caa01(0x35767aaf6ebfee7550e19bcc2ce67367cad6385296f9a3c268599f0f73910cb3); /* statement */ 
Types.Wei memory makerWei = Exchange.exchange(
            args.exchangeWrapper,
            args.account.owner,
            makerToken,
            takerToken,
            takerWei,
            args.orderData
        );

coverage_0x690caa01(0x176b8cd446efbb3aba8065e9dfe986c170d7279d8ce5915dc95660135b09a36e); /* line */ 
        coverage_0x690caa01(0x7c93d001f8ed6fccc0190288f3c21b640fa154210422611359f88f7109b24922); /* statement */ 
state.setPar(
            args.account,
            args.takerMarket,
            takerPar
        );

coverage_0x690caa01(0x8ccd2e2005f7fb060f1b9973ac9571fbb04530643a9244cd03abdd8f5ee945f4); /* line */ 
        coverage_0x690caa01(0xb837579584db8e0555b9847e661cb437f553b99a6d747d24712930b561a637b4); /* statement */ 
state.setParFromDeltaWei(
            args.account,
            args.makerMarket,
            makerWei
        );

coverage_0x690caa01(0x5c28fa57e84db15b3babe78af5cd72d6ea0895b208aee479308ef728782314b3); /* line */ 
        coverage_0x690caa01(0x6256d9dae472d8f71440242217a8b15b735b9b5892c44840667b2250769a0f88); /* statement */ 
Events.logSell(
            state,
            args,
            takerWei,
            makerWei
        );
    }

    function _trade(
        Storage.State storage state,
        Actions.TradeArgs memory args
    )
        private
    {coverage_0x690caa01(0xd3619e54633c0834347858d2b5f3406686fbaa0c4cd4673fa6af786c7397a78b); /* function */ 

coverage_0x690caa01(0x49c0a38e886ad3ca1a50e84eed92f3215f719eef2097a86e8a06f9409c591f0b); /* line */ 
        coverage_0x690caa01(0x9dc3d62dec9920fea8fd84aa60dac689e641875ccddace93c0bd8ff813de9d82); /* statement */ 
state.requireIsOperator(args.takerAccount, msg.sender);
coverage_0x690caa01(0x15be2bcd1ded84a8c55943f0035e589561523ce8826f5c7dac53c106459b9ffb); /* line */ 
        coverage_0x690caa01(0x2835f1f19b430a7b12f00a931e4c7fe5b68d0a429421fe0d10c6f4ae047dda54); /* statement */ 
state.requireIsOperator(args.makerAccount, args.autoTrader);

coverage_0x690caa01(0xb30acfa2b3bddddab87e086f8ef357f525580b0dd9fa307a5a839ba3f3320bb4); /* line */ 
        coverage_0x690caa01(0xeb28fdd6bba6a2fd63026630f83b764b79691802fef10fefeb77b99561e3a135); /* statement */ 
Types.Par memory oldInputPar = state.getPar(
            args.makerAccount,
            args.inputMarket
        );
coverage_0x690caa01(0xe568c1e6a1fd31bd0cda6752817bab991f140e629cfc8ed39d2d70e66249f180); /* line */ 
        coverage_0x690caa01(0x001cb8c1ee1adbb466abf029e8259db6645b1259f3d115c1a396a9c944211092); /* statement */ 
(
            Types.Par memory newInputPar,
            Types.Wei memory inputWei
        ) = state.getNewParAndDeltaWei(
            args.makerAccount,
            args.inputMarket,
            args.amount
        );

coverage_0x690caa01(0x36306f5aa6ba9883c470d04300c6531a75c347225bcac0f3eda24a0535010b3c); /* line */ 
        coverage_0x690caa01(0x6bbb7d0f09eb7d8e57612f56514f6c1823df1c5d6b27bf6ee7f0667b204a9156); /* statement */ 
Types.AssetAmount memory outputAmount = IAutoTrader(args.autoTrader).getTradeCost(
            args.inputMarket,
            args.outputMarket,
            args.makerAccount,
            args.takerAccount,
            oldInputPar,
            newInputPar,
            inputWei,
            args.tradeData
        );

coverage_0x690caa01(0x5091662ae2ee5dbdd632a1e95077f02d7b70b39c4fd1cf289a033fe119eeb1d6); /* line */ 
        coverage_0x690caa01(0x2779965303335f203c649f15699fedff4cd7d3027f0424d85e652f905d5089ce); /* statement */ 
(
            Types.Par memory newOutputPar,
            Types.Wei memory outputWei
        ) = state.getNewParAndDeltaWei(
            args.makerAccount,
            args.outputMarket,
            outputAmount
        );

coverage_0x690caa01(0xe6b6b42ac2a428757ca08865225a0718a234e8c1368d368ff266e14374e34bb4); /* line */ 
        coverage_0x690caa01(0xfdf9e1ab4d41362852f2cc1371f73e83b84d63a5b76e4914dd8b65ee51c766c9); /* statement */ 
Require.that(
            outputWei.isZero() || inputWei.isZero() || outputWei.sign != inputWei.sign,
            FILE,
            "Trades cannot be one-sided"
        );

        // set the balance for the maker
coverage_0x690caa01(0x024c6094e173dad3e85320a483e0e891f83ab590ca9a7c3f6703b9060e72da5b); /* line */ 
        coverage_0x690caa01(0x5413ffcb101d69864d25f8f73146f8ee7df14705be5d4a4d2b52270dd5e649c3); /* statement */ 
state.setPar(
            args.makerAccount,
            args.inputMarket,
            newInputPar
        );
coverage_0x690caa01(0x6dabbabcfc936f4706c1ab0a5a486597b9cb43732a6460518bafadd4d6f3855b); /* line */ 
        coverage_0x690caa01(0xf5e1c6cdc0b86ff96c5336d29d92e33df8330977f0568364544ef6a323d1dc2c); /* statement */ 
state.setPar(
            args.makerAccount,
            args.outputMarket,
            newOutputPar
        );

        // set the balance for the taker
coverage_0x690caa01(0x0afc8b4fdcd8a3026d0698e605c2df82097f655a90b3bd3e6db7cba89acff0ae); /* line */ 
        coverage_0x690caa01(0x5b0f5ba8f41724d9bfc82ed680757dee3eb61efc0e82f47444d7bda4812dfec0); /* statement */ 
state.setParFromDeltaWei(
            args.takerAccount,
            args.inputMarket,
            inputWei.negative()
        );
coverage_0x690caa01(0xb1746071b1fa46af1b97ffb41cc762fd893a789451221c287f21837d0cae989e); /* line */ 
        coverage_0x690caa01(0xbef3ffbfc0d280958db06857797b8025a606c89fd4a7a32ccc3dfb6b7e875200); /* statement */ 
state.setParFromDeltaWei(
            args.takerAccount,
            args.outputMarket,
            outputWei.negative()
        );

coverage_0x690caa01(0x9e916fd87552faa3bafbe261bf19ba6edaf4b6070afcc672a132e7a01160a965); /* line */ 
        coverage_0x690caa01(0x8081eea2143a249069797a0f862316195fd038c366f9bfd4fe7e913d13f26044); /* statement */ 
Events.logTrade(
            state,
            args,
            inputWei,
            outputWei
        );
    }

    function _liquidate(
        Storage.State storage state,
        Actions.LiquidateArgs memory args,
        Cache.MarketCache memory cache
    )
        private
    {coverage_0x690caa01(0x5a66b52ca01b7faa3ca9bd03a3cd08f50cf596d6dab1ecb5f3fdbf2a2226186d); /* function */ 

coverage_0x690caa01(0x9f75a8e4d4aaf2c92ffb23055d58b8da49ddcfed441da3a22a260fc96daf162e); /* line */ 
        coverage_0x690caa01(0x40cf8eb27887fbddb5cf869a2fd99447acf16e73c16d0f97e38a10313f796c3d); /* statement */ 
state.requireIsOperator(args.solidAccount, msg.sender);

        // verify liquidatable
coverage_0x690caa01(0xa95b501a643647ba848b6adf9f5d82abcb232141186249da7581e568b10f356b); /* line */ 
        coverage_0x690caa01(0x3bd36631faa88ae1508247d50273c81a86781cab11bb736f26c91cda99d8b5ae); /* statement */ 
if (Account.Status.Liquid != state.getStatus(args.liquidAccount)) {coverage_0x690caa01(0x629c4d8bbbf74d5c297a953a93f90107f9f29191a919119bd90a4bff1fa513fb); /* branch */ 

coverage_0x690caa01(0x6572ebdf608daaaa74a53cd03f93bd2bfbf9faeac640e82a958b6cb53cff7470); /* line */ 
            coverage_0x690caa01(0x46777c9f1ec599bde85ce059e20894bbe0929f787d02973cb70b044ccc408d9c); /* statement */ 
Require.that(
                !state.isCollateralized(args.liquidAccount, cache, /* requireMinBorrow = */ false),
                FILE,
                "Unliquidatable account",
                args.liquidAccount.owner,
                args.liquidAccount.number
            );
coverage_0x690caa01(0x4ffa1e74932c887f6b09f34ea7d40d199e6bcceb0519dd47896c2e32416e9d39); /* line */ 
            coverage_0x690caa01(0xdb041ee6b3bc3a98445d67cab6d986d330d4d22ce94f9c41c274fda78b170186); /* statement */ 
state.setStatus(args.liquidAccount, Account.Status.Liquid);
        }else { coverage_0x690caa01(0xba6d3e63efb93b82005e8bb347df804213cc77875100f972c84aef8ec14efb6a); /* branch */ 
}

coverage_0x690caa01(0x2e767dbff450ccae392f8e4d810e9471e8c6d727df322bf6ea1d0529e2e625cd); /* line */ 
        coverage_0x690caa01(0x743faf17c4aefb758ca4c0acce8e2e79fb64e72437d7d8f03b3869f533890496); /* statement */ 
Types.Wei memory maxHeldWei = state.getWei(
            args.liquidAccount,
            args.heldMarket
        );

coverage_0x690caa01(0x9c45027f18e668450399e24a703742f17669dca97279e6eb1497791cc4748de0); /* line */ 
        coverage_0x690caa01(0xd3dde181f1d520f17cf8ef8c7aef5878392faacd5d3b0cf0ee4e46cfd6d34e17); /* statement */ 
Require.that(
            !maxHeldWei.isNegative(),
            FILE,
            "Collateral cannot be negative",
            args.liquidAccount.owner,
            args.liquidAccount.number,
            args.heldMarket
        );

coverage_0x690caa01(0x47f704668601c85ac70b0e8fbe83b39c3fefe93ffe9e981915698365e5894657); /* line */ 
        coverage_0x690caa01(0x7f2f7a8fc8cc80d28c8de74a25c0fdd925ee24253b29644228db6ec753f0b358); /* statement */ 
(
            Types.Par memory owedPar,
            Types.Wei memory owedWei
        ) = state.getNewParAndDeltaWeiForLiquidation(
            args.liquidAccount,
            args.owedMarket,
            args.amount
        );

coverage_0x690caa01(0xe944dd98de2368b88973792de3264450735a30948608136a518e5cd7ce7af88f); /* line */ 
        coverage_0x690caa01(0xc942bca810b6e2774065df9e5714c6061eb56c51b48cddbccfb06c57d4e3a2bf); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = _getLiquidationPrices(
            state,
            cache,
            args.heldMarket,
            args.owedMarket
        );

coverage_0x690caa01(0x8a2635e9bc7fd67d5d66a8bb13f04229e9e717e247ea9e4127b5e4f701827e35); /* line */ 
        coverage_0x690caa01(0x4e3022b7c180a806180728bafed48520d2b7367eeb356dbe7496d71db57940ce); /* statement */ 
Types.Wei memory heldWei = _owedWeiToHeldWei(owedWei, heldPrice, owedPrice);

        // if attempting to over-borrow the held asset, bound it by the maximum
coverage_0x690caa01(0xe58f4de84453de88be0eca465027e51147800842a8498afc61a00b736c19a7ca); /* line */ 
        coverage_0x690caa01(0x5791a22b53a06c06dbbf76ed089d7188e69449b7e0890ab2a752e6330ffb9a8b); /* statement */ 
if (heldWei.value > maxHeldWei.value) {coverage_0x690caa01(0x734cd7f54aa803d200564422236e7ddffbb7e34047ea6e051c0c85a132144f3c); /* branch */ 

coverage_0x690caa01(0x9567903eb7b77bade3e36dbab62530515ac59e6e5510af1bc9a0996d509c7e04); /* line */ 
            coverage_0x690caa01(0x1dc5f72750209bf39d50512e845f9199efb14a94769759020262a783e9ea3cff); /* statement */ 
heldWei = maxHeldWei.negative();
coverage_0x690caa01(0x1f8f8d49e11bcea98e9ff32cbe54564ab7531ec889a1939377295aa580f4bb50); /* line */ 
            coverage_0x690caa01(0xa2f7e972410a10a43d392eda2387375fae53e6eeff035db60218b86232953c7f); /* statement */ 
owedWei = _heldWeiToOwedWei(heldWei, heldPrice, owedPrice);

coverage_0x690caa01(0x9a5105821a3b8733f87d0b4e3e5db3aef6312e370fe737bd4249b8daf093d7c7); /* line */ 
            coverage_0x690caa01(0x098ff285960df86155047a25a9d928c283ee384dbb63965a4c852d10a8988bc8); /* statement */ 
state.setPar(
                args.liquidAccount,
                args.heldMarket,
                Types.zeroPar()
            );
coverage_0x690caa01(0x3f3cc094b6195eb69f9ba22deee99a4e70b648895215912041111c1195ffa48e); /* line */ 
            coverage_0x690caa01(0x3de29a3eb3ad2ee3ebb8a99d38b7ca3b56702583f9256c9bc55288f1cbaf3c97); /* statement */ 
state.setParFromDeltaWei(
                args.liquidAccount,
                args.owedMarket,
                owedWei
            );
        } else {coverage_0x690caa01(0xf52e9368422b7ab7a604831b40292d26a2e4ba48679d32fd1f61f95da5afa8e9); /* branch */ 

coverage_0x690caa01(0xb35cb188359468f150189e9946e14c176ce301c5e9d03cdbda767b30e2f5af5d); /* line */ 
            coverage_0x690caa01(0x5ce199b13c710618007418ce398bc2a7a7cb2f3dc357c338ee67df97642057f0); /* statement */ 
state.setPar(
                args.liquidAccount,
                args.owedMarket,
                owedPar
            );
coverage_0x690caa01(0x652aa6b9c5e171cb69146e82f114d6005bfa68874c870e4f4fb062d3b2126387); /* line */ 
            coverage_0x690caa01(0x2fe508e9c64b3b39eff4e40f1cec1d7aecda03e509031732ac293cdce5c99fa0); /* statement */ 
state.setParFromDeltaWei(
                args.liquidAccount,
                args.heldMarket,
                heldWei
            );
        }

        // set the balances for the solid account
coverage_0x690caa01(0xfa819e94d5c341191dd83b7e4c434cfeae486b1c11c9b0a7028b875a8c642a3f); /* line */ 
        coverage_0x690caa01(0xa600ca94ac04b6c4c2b6edbf374523b971091257120c6feb6c03dddda4a15ccf); /* statement */ 
state.setParFromDeltaWei(
            args.solidAccount,
            args.owedMarket,
            owedWei.negative()
        );
coverage_0x690caa01(0x948b4df653fd46cc348a20d48f2c2abd0768adcdc7186a1ff6a5cfd85e9aee01); /* line */ 
        coverage_0x690caa01(0x919f7fe63bc30a3a4912d70100e6cad39416983d1ea83527e63efee953db246a); /* statement */ 
state.setParFromDeltaWei(
            args.solidAccount,
            args.heldMarket,
            heldWei.negative()
        );

coverage_0x690caa01(0xe7c63358b03405dd9b488c93a9a4094a66a10a1aee4cde171246366ffcf41503); /* line */ 
        coverage_0x690caa01(0xe02e9831e42ba548ab734f0854142638b04e4e5229c17ad7b9d8b444e9010a03); /* statement */ 
Events.logLiquidate(
            state,
            args,
            heldWei,
            owedWei
        );
    }

    function _vaporize(
        Storage.State storage state,
        Actions.VaporizeArgs memory args,
        Cache.MarketCache memory cache
    )
        private
    {coverage_0x690caa01(0x64c6c7dff0f96a6a3b218c61500b1fc6b774f5dd2b6a8ef429b46c6279632913); /* function */ 

coverage_0x690caa01(0xa35c57c939a64bfb28599c8488b67fe1292d74468dc89581be2ce1b4538552d2); /* line */ 
        coverage_0x690caa01(0xbdb2957a340d30eb39312e0b11e264e9355ba2bf8854bca6eb3307e48966d4cc); /* statement */ 
state.requireIsOperator(args.solidAccount, msg.sender);

        // verify vaporizable
coverage_0x690caa01(0x5c88517bb958dae87624b1d24bd8aef41c6a8148628d3e9cfc7ad8555a12749c); /* line */ 
        coverage_0x690caa01(0x51318a71b2230d918e22f0f387b303cf3d4aa1d8bd6cfab57661b9ffd54a3b55); /* statement */ 
if (Account.Status.Vapor != state.getStatus(args.vaporAccount)) {coverage_0x690caa01(0xd2a296669c516d44292e3bc031822fa724e40797e2697aaf8ffafffce54332d1); /* branch */ 

coverage_0x690caa01(0x64db52e26fc69b3a4f7bbf8ab57bca071ac249d7332e468af9e46bf791facc56); /* line */ 
            coverage_0x690caa01(0x8dbc37fd11b702218db845cd25b9c2e10ff98619ba66a30bef072377e1080a49); /* statement */ 
Require.that(
                state.isVaporizable(args.vaporAccount, cache),
                FILE,
                "Unvaporizable account",
                args.vaporAccount.owner,
                args.vaporAccount.number
            );
coverage_0x690caa01(0xf13b7d9cdecd25c326ae9af42542f5bf001f66c06b43f6b85510574082b26414); /* line */ 
            coverage_0x690caa01(0x73e488a689e9592384f35bbd4a38e9506814dc0eeb20d2cc58f00c727092bf7f); /* statement */ 
state.setStatus(args.vaporAccount, Account.Status.Vapor);
        }else { coverage_0x690caa01(0xff535eb3d6ce9ac4198a346723768c68c1d13be08ffa60cea8e790e29f618d30); /* branch */ 
}

        // First, attempt to refund using the same token
coverage_0x690caa01(0x62571b575572e830a4d90964eb237c72b80d00d4c5f1f7701737efd3866d7ea7); /* line */ 
        coverage_0x690caa01(0x0be4eeb808df15d38015b21172c113eeee5f02042e9de3ea327de6eb0176815f); /* statement */ 
(
            bool fullyRepaid,
            Types.Wei memory excessWei
        ) = _vaporizeUsingExcess(state, args);
coverage_0x690caa01(0x1fe05ed78818825dbcdda74a78ce2f05ef685c3fa9d63115d1df7ab6c9aea006); /* line */ 
        coverage_0x690caa01(0x4896333a8ddec413a667d5e980dcece77352a29dd2a285594ca520aadb3fa9d5); /* statement */ 
if (fullyRepaid) {coverage_0x690caa01(0x9f7e845549be3400ff73ad3d63d15a2d516c9ebc4d8462f6c3e73ea4855ea2de); /* branch */ 

coverage_0x690caa01(0x5e781eb8b5e7c06222f4acc830b8497fbc4b374fcd66e264471c48f814dfb2a8); /* line */ 
            coverage_0x690caa01(0x37e2387a9e7834e8c00520cb565730ba5585920a330ae99e1713883b463076a0); /* statement */ 
Events.logVaporize(
                state,
                args,
                Types.zeroWei(),
                Types.zeroWei(),
                excessWei
            );
coverage_0x690caa01(0xa4930875f7ed10f91935bf4fefbbd5cdd7d56edebe858c06b1841d371d946eab); /* line */ 
            coverage_0x690caa01(0xc15bd4923f083acb9cf1ceeb1e4f3c9a53364958b69129aa6fba53bd5442d2d7); /* statement */ 
return;
        }else { coverage_0x690caa01(0xd0b156e513ff18d9230265e8f299d3dc1763694aede5955aa8ac5dc230339763); /* branch */ 
}

coverage_0x690caa01(0x68d27782a9bf511e0259a4c6c3e86dd65317b2ae856fcebd8dc0ceb4d8c46c13); /* line */ 
        coverage_0x690caa01(0x7485122deaa33b87a2caf2d948337fa4cba806984fb499609fd0bd66d9c656f5); /* statement */ 
Types.Wei memory maxHeldWei = state.getNumExcessTokens(args.heldMarket);

coverage_0x690caa01(0xd503bcc77d7e518753eef2602e562c100f688dd618b688eca7c489aad87d7578); /* line */ 
        coverage_0x690caa01(0xeae9d7cee75fe2b245fd659acf76dfbfeecc8cb4cbe6bf9e5918743aeac1b99c); /* statement */ 
Require.that(
            !maxHeldWei.isNegative(),
            FILE,
            "Excess cannot be negative",
            args.heldMarket
        );

coverage_0x690caa01(0xc2c20415644c71a3dbe43b382a9d226740b840ab255636c199aabf1791a6ff7e); /* line */ 
        coverage_0x690caa01(0xd83951b5824da13c7bf984bc6eb25dd857a7d95efb8e5786bad78782b50fc7ca); /* statement */ 
(
            Types.Par memory owedPar,
            Types.Wei memory owedWei
        ) = state.getNewParAndDeltaWeiForLiquidation(
            args.vaporAccount,
            args.owedMarket,
            args.amount
        );

coverage_0x690caa01(0x00043bdb016b7d2352b314ca4ddb3d199eaaa957243a49d025053e50cb5c6f90); /* line */ 
        coverage_0x690caa01(0x26aa406e93fabc5718bb39c6359dd34a02dba39801cab3d567e5fc81267035f1); /* statement */ 
(
            Monetary.Price memory heldPrice,
            Monetary.Price memory owedPrice
        ) = _getLiquidationPrices(
            state,
            cache,
            args.heldMarket,
            args.owedMarket
        );

coverage_0x690caa01(0x82a13db7aad5457e13d7ef083cbabddf67bd69d41baf256244d4465f436fee53); /* line */ 
        coverage_0x690caa01(0x65bfa4b5be70b011e6a2f7d0d3f4978d3da652064eee6fb71a4118775b91fa63); /* statement */ 
Types.Wei memory heldWei = _owedWeiToHeldWei(owedWei, heldPrice, owedPrice);

        // if attempting to over-borrow the held asset, bound it by the maximum
coverage_0x690caa01(0x3eaef24598f108b26fb6fa020330857bc9dd2427062ab7c4e20ef0e06a13f64e); /* line */ 
        coverage_0x690caa01(0x1304305c26268920033c6ade33f2a1f902fd95b76397bab7878ca847ff66966f); /* statement */ 
if (heldWei.value > maxHeldWei.value) {coverage_0x690caa01(0x2f1afce061d59c8d1843479bcd11621a3d0fd6648ff8dd231ebc7710e85543f8); /* branch */ 

coverage_0x690caa01(0xd8f6d5bad1c2a47728d79abc77468110988a09e54bcfff76f269b9299ace85a2); /* line */ 
            coverage_0x690caa01(0xf6b2cbc07d921fb56860ff0870f2770bc2d40f0f85c93a609efc7ad26fc65135); /* statement */ 
heldWei = maxHeldWei.negative();
coverage_0x690caa01(0x695b10ece575238e3c744a41ca117a82da7fe1ca8d280004fc02e18a2f4e4025); /* line */ 
            coverage_0x690caa01(0xe22cc1f9e93ebd7d42baa495dbe3668ccfcdcb2801aaeab2d1b505ceaf76e13c); /* statement */ 
owedWei = _heldWeiToOwedWei(heldWei, heldPrice, owedPrice);

coverage_0x690caa01(0x3b59c5b0684c948ad68ed734a3bf21497000f4841ce738873736f4073b69f630); /* line */ 
            coverage_0x690caa01(0xa486a7b414f847977bd8d5fecfe23c1f39c9927419541f8fc69e315474dc9864); /* statement */ 
state.setParFromDeltaWei(
                args.vaporAccount,
                args.owedMarket,
                owedWei
            );
        } else {coverage_0x690caa01(0xdb58d831810ffb1298c284d35a7b53e1ed9356e18cad00f1c9f3d084aa9d3d5c); /* branch */ 

coverage_0x690caa01(0xa69c82441d2b284e01009461e163e9bfd10f1c92751bcbb0476d1f020491b3a6); /* line */ 
            coverage_0x690caa01(0x9c8500ab5c7263f27fdc16adec104ecb72b71f29fb0640dca53c5dee9981e123); /* statement */ 
state.setPar(
                args.vaporAccount,
                args.owedMarket,
                owedPar
            );
        }

        // set the balances for the solid account
coverage_0x690caa01(0x51994301ca80d60cf2488144bfb49ec01161af04fa235be8c23f0212f218072f); /* line */ 
        coverage_0x690caa01(0xcc2ef838dca2221188c9a192dcf836930afe837a64a48b1b2d317797ce6043ae); /* statement */ 
state.setParFromDeltaWei(
            args.solidAccount,
            args.owedMarket,
            owedWei.negative()
        );
coverage_0x690caa01(0x59d17219195bafda21392965eb57912bb54d4299892414ade5b1118e3b9856d8); /* line */ 
        coverage_0x690caa01(0xf094cb498bed75ff275d98d172f93d0fc6b6830e9eb8a237224d352a6578635c); /* statement */ 
state.setParFromDeltaWei(
            args.solidAccount,
            args.heldMarket,
            heldWei.negative()
        );

coverage_0x690caa01(0x56e0a2f178fe11b5b21d2d3865fb2a8e8618a6182a5207c1aff9659e892653e7); /* line */ 
        coverage_0x690caa01(0x5b25da0370abf7204017cdcaaad0c8dada6aca5076e4c78ee77b3a177e9f6936); /* statement */ 
Events.logVaporize(
            state,
            args,
            heldWei,
            owedWei,
            excessWei
        );
    }

    function _call(
        Storage.State storage state,
        Actions.CallArgs memory args
    )
        private
    {coverage_0x690caa01(0x2608f6ff0a9137ef5acac0403294b90f9f4af4ce3695caf0c0182df3918e5f9a); /* function */ 

coverage_0x690caa01(0x89b008035aa16adb1a391b29d020da94e74baafa7aa2866a41333bf49f97fcea); /* line */ 
        coverage_0x690caa01(0xd2a5774ff9c4ad226d844f4100356b7148c92db61fd230c9f1eed3c54a116664); /* statement */ 
state.requireIsOperator(args.account, msg.sender);

coverage_0x690caa01(0xa7478e93e11eb8a1a86d58fc8ead5fbd89d81bec70632201306b343b5fd37cd2); /* line */ 
        coverage_0x690caa01(0xe4856b35bed0de93ba8669c77bbee2b458404b8851b53915050f58807374fc81); /* statement */ 
ICallee(args.callee).callFunction(
            msg.sender,
            args.account,
            args.data
        );

coverage_0x690caa01(0x971c3366e686ff511452842a84fca20e2be50a6b5d437043aea6ef73245fd7ef); /* line */ 
        coverage_0x690caa01(0xc9af2ce23478897895b251dc95b9416ac0648356378cd99e670315a385a0d4e5); /* statement */ 
Events.logCall(args);
    }

    // ============ Private Functions ============

    /**
     * For the purposes of liquidation or vaporization, get the value-equivalent amount of heldWei
     * given owedWei and the (spread-adjusted) prices of each asset.
     */
    function _owedWeiToHeldWei(
        Types.Wei memory owedWei,
        Monetary.Price memory heldPrice,
        Monetary.Price memory owedPrice
    )
        private
        pure
        returns (Types.Wei memory)
    {coverage_0x690caa01(0x6c633abafe956badbbde71158ad7bb24e65f124598075d2681ca82ed307079a3); /* function */ 

coverage_0x690caa01(0xe930260b5753b56a976ad8991c859bf9f74e3ffbdc22108c5ddda85cc9507ebd); /* line */ 
        coverage_0x690caa01(0x10802435084fa2dc18a5d23f456e91e8a5e417a0823f6177645cf7ac949ae54a); /* statement */ 
return Types.Wei({
            sign: false,
            value: Math.getPartial(owedWei.value, owedPrice.value, heldPrice.value)
        });
    }

    /**
     * For the purposes of liquidation or vaporization, get the value-equivalent amount of owedWei
     * given heldWei and the (spread-adjusted) prices of each asset.
     */
    function _heldWeiToOwedWei(
        Types.Wei memory heldWei,
        Monetary.Price memory heldPrice,
        Monetary.Price memory owedPrice
    )
        private
        pure
        returns (Types.Wei memory)
    {coverage_0x690caa01(0xeb427c3100b9c172f7235f3e1d5975ab6feaed767bc46fd7d76fded679ac537b); /* function */ 

coverage_0x690caa01(0xb67aab0520978c81d572f3184e3d31529fed7ca69f0ed5e8e54bdd4b555ccce0); /* line */ 
        coverage_0x690caa01(0xcf79419047ae2202924ffaaaa11402160ad7c104155a007fd7943aecccb223c7); /* statement */ 
return Types.Wei({
            sign: true,
            value: Math.getPartialRoundUp(heldWei.value, heldPrice.value, owedPrice.value)
        });
    }

    /**
     * Attempt to vaporize an account's balance using the excess tokens in the protocol. Return a
     * bool and a wei value. The boolean is true if and only if the balance was fully vaporized. The
     * Wei value is how many excess tokens were used to partially or fully vaporize the account's
     * negative balance.
     */
    function _vaporizeUsingExcess(
        Storage.State storage state,
        Actions.VaporizeArgs memory args
    )
        internal
        returns (bool, Types.Wei memory)
    {coverage_0x690caa01(0xba97ef9366bf0f5e7603c64591436ecd4933f8b17894c99b4a506d4e49ae4a4e); /* function */ 

coverage_0x690caa01(0xc6500f65878028903a62491c0037ea51ead4a440a2d5894e0874f99516e85a38); /* line */ 
        coverage_0x690caa01(0xee6b1cf7de6e2104135107c7182c31e7540a0dd23f5ca948d3fc6268eb0239ba); /* statement */ 
Types.Wei memory excessWei = state.getNumExcessTokens(args.owedMarket);

        // There are no excess funds, return zero
coverage_0x690caa01(0xf496faf4f73d51eb0868247ff7e3c173ec95bb67bacc18f472813cc9fad0c12e); /* line */ 
        coverage_0x690caa01(0xa55861c28953fe4c805c6aa604540bfdaaba90a3e841bf7da045488880fd78f2); /* statement */ 
if (!excessWei.isPositive()) {coverage_0x690caa01(0x3e20b965a9e0bf8c1322c37b361e12a41c0050b1b892a7c6dcbad17e8ca87471); /* branch */ 

coverage_0x690caa01(0x15a1a90a6168474b47c18360559c67d8e3d464fde9a83b480c4808644ba44c0f); /* line */ 
            coverage_0x690caa01(0xf0b32ce27ae7cfae9be34afaf6a1a63fc5e83709c4ac9a5a5fa06e1d08e8b761); /* statement */ 
return (false, Types.zeroWei());
        }else { coverage_0x690caa01(0x136c63a1bd61223859ccc9bfec8ba47ba536cb48a05e6c5c67629a6e0bb8bd3e); /* branch */ 
}

coverage_0x690caa01(0xaeeddb2882679ceb5364d3f3b55808c5ad2223ba6b3d0cc6f20a4bce687c2359); /* line */ 
        coverage_0x690caa01(0x1749ae5836039b865b294068c6d3d6ca5935cad7cbe113eceba7df8fd2bb3dc7); /* statement */ 
Types.Wei memory maxRefundWei = state.getWei(args.vaporAccount, args.owedMarket);
coverage_0x690caa01(0xcbf7a068536328ffdf8fc480f653c06c4ebe8e3bee22b0ac4051847fb8ac5653); /* line */ 
        coverage_0x690caa01(0x765bbb28905b080d6edabc251ef245ac644f8bbbf2ebfcda70aa8246c7e640b4); /* statement */ 
maxRefundWei.sign = true;

        // The account is fully vaporizable using excess funds
coverage_0x690caa01(0xdfee7a2900f1fc92e92617b41ad5546eb992d035f3c7921934f677a998c4577f); /* line */ 
        coverage_0x690caa01(0x64389474d498b686cf68cebed5281c7cb145480f85d8ff9d15e482e096dcf996); /* statement */ 
if (excessWei.value >= maxRefundWei.value) {coverage_0x690caa01(0x063dc3e68cffce66812282a7da40902e823196d6e05fb91c5536e2cd412fa546); /* branch */ 

coverage_0x690caa01(0xea8b7b99aec08fd3ea2756c9d7e1a248b8fd793166fe38b7cc00c3a1e44289e8); /* line */ 
            coverage_0x690caa01(0x985ae31ea84e0210456db9a8f9ee423859f292591be867db8f27aea314af2a2c); /* statement */ 
state.setPar(
                args.vaporAccount,
                args.owedMarket,
                Types.zeroPar()
            );
coverage_0x690caa01(0xe24035505459b0bde23e0ed318fb9735650252e256b8c4d17e40bb29b9db0c32); /* line */ 
            coverage_0x690caa01(0xf087b775497934a7edf627d7e310eceee35563627b1b66abaafb200021085005); /* statement */ 
return (true, maxRefundWei);
        }

        // The account is only partially vaporizable using excess funds
        else {coverage_0x690caa01(0xa0b5959d3057811bc0a30c19a6ccd23bd3faaa155feccbf45cb19233465aef9b); /* branch */ 

coverage_0x690caa01(0xab93547908075d01bbaf37324c9bdacf19adc7e8e83a00fd0cc2295ea39e1395); /* line */ 
            coverage_0x690caa01(0xba1c0ee1acffe361559e7f02fd84cd47ea70b999a1e4c9af1fc6463e09632b1a); /* statement */ 
state.setParFromDeltaWei(
                args.vaporAccount,
                args.owedMarket,
                excessWei
            );
coverage_0x690caa01(0x65a3ad01a7453ae31c2214e17a3a3cff47aba0f854e5d0b2948023db7c8736bd); /* line */ 
            coverage_0x690caa01(0x02e1050b7e9988cc288ed25e33d042f326da894d0e2259dfe57e295430a96d0d); /* statement */ 
return (false, excessWei);
        }
    }

    /**
     * Return the (spread-adjusted) prices of two assets for the purposes of liquidation or
     * vaporization.
     */
    function _getLiquidationPrices(
        Storage.State storage state,
        Cache.MarketCache memory cache,
        uint256 heldMarketId,
        uint256 owedMarketId
    )
        internal
        view
        returns (
            Monetary.Price memory,
            Monetary.Price memory
        )
    {coverage_0x690caa01(0x3a298eb195a04a3257ea0410ea6bd6f0b2875744375b7acad2fc5d3d0afdb995); /* function */ 

coverage_0x690caa01(0xf5130b54066d47d866070ed59774476439d554bf1a9b3f093c48b9e4af4ace8a); /* line */ 
        coverage_0x690caa01(0x31b3cd7b1eab2d1b41929ac65db0fed5b2f24b8ba52f55ae6a4e5cdcffba396e); /* statement */ 
uint256 originalPrice = cache.getPrice(owedMarketId).value;
coverage_0x690caa01(0xa555fa5b4a3e6cc2b0b964a1140ea61f4d23e1214b2b6d13e385079a61960720); /* line */ 
        coverage_0x690caa01(0xc8b4dbcb2005b6ca9f20e6b9427091ba19e0afd49eed26a81fb64eec459c0d83); /* statement */ 
Decimal.D256 memory spread = state.getLiquidationSpreadForPair(
            heldMarketId,
            owedMarketId
        );

coverage_0x690caa01(0x43ca7165650c77f37dd93a1b7d430ee916837fd783c91d2a5e82fc83488cf399); /* line */ 
        coverage_0x690caa01(0xab44df275d828051010984fbfd846483cd85b6712f4fa51cce1f90448708087f); /* statement */ 
Monetary.Price memory owedPrice = Monetary.Price({
            value: originalPrice.add(Decimal.mul(originalPrice, spread))
        });

coverage_0x690caa01(0x8994054fae79c3188309df5d86be980af0c379d76d1e9f1504e7c51a7a7e01dc); /* line */ 
        coverage_0x690caa01(0x546373975f4fa3ba754d91eeb55e6a4315d7de2763dbc2771ebf38174c0a71a9); /* statement */ 
return (cache.getPrice(heldMarketId), owedPrice);
    }
}
