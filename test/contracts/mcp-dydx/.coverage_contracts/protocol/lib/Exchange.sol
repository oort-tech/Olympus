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
import { Token } from "./Token.sol";
import { Types } from "./Types.sol";
import { IExchangeWrapper } from "../interfaces/IExchangeWrapper.sol";


/**
 * @title Exchange
 * @author dYdX
 *
 * Library for transferring tokens and interacting with ExchangeWrappers by using the Wei struct
 */
library Exchange {
function coverage_0x4c1d851e(bytes32 c__0x4c1d851e) public pure {}

    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant FILE = "Exchange";

    // ============ Library Functions ============

    function transferOut(
        address token,
        address to,
        Types.Wei memory deltaWei
    )
        internal
    {coverage_0x4c1d851e(0x756fe97e5c0ec436fe36206a1c288827ace1215c3a166e11514ab47e8f33084a); /* function */ 

coverage_0x4c1d851e(0x35ff15889ec43f27e9a78a0006979432ac55743ab65b4675db21511dbb05d927); /* line */ 
        coverage_0x4c1d851e(0x6cbe0400fb0348d04aea61ab13b9f3b38f222de771063d333721d79a4624eb27); /* statement */ 
Require.that(
            !deltaWei.isPositive(),
            FILE,
            "Cannot transferOut positive",
            deltaWei.value
        );

coverage_0x4c1d851e(0xc5563d90732ce800ab5abb14f9fe9452273ab777e87154a131f10e1e257bbf0c); /* line */ 
        coverage_0x4c1d851e(0x50c1626ad2be97e91710afe615bb565723c6790d075b332869d9f6cb86f3e4d3); /* statement */ 
Token.transfer(
            token,
            to,
            deltaWei.value
        );
    }

    function transferIn(
        address token,
        address from,
        Types.Wei memory deltaWei
    )
        internal
    {coverage_0x4c1d851e(0x3946cfb30ef2c963ec3c480f61cb380e5e82fd75a335b91884fd28ad759fd661); /* function */ 

coverage_0x4c1d851e(0xd11a4327bf6fb5e6bc1939ac832b072c5f59128ad068ae5b9d265d6194fb3fb9); /* line */ 
        coverage_0x4c1d851e(0xe2ea017ae9d1af040de18ec318d2769b73b5933e4a9781395c0751c0e6117ff5); /* statement */ 
Require.that(
            !deltaWei.isNegative(),
            FILE,
            "Cannot transferIn negative",
            deltaWei.value
        );

coverage_0x4c1d851e(0x53a1befbd9370b5de951b3cd642e31918adca0ed2df3bffa23b8374c6512bc72); /* line */ 
        coverage_0x4c1d851e(0x64f70a193d6ab774073a5804f948f94388950070f42498c0f6b40b99d4955576); /* statement */ 
Token.transferFrom(
            token,
            from,
            address(this),
            deltaWei.value
        );
    }

    function getCost(
        address exchangeWrapper,
        address supplyToken,
        address borrowToken,
        Types.Wei memory desiredAmount,
        bytes memory orderData
    )
        internal
        view
        returns (Types.Wei memory)
    {coverage_0x4c1d851e(0x6b034db4a817704af0d1570549c0b4ca8b9adddadd9cae5a1d2a4d8c32180b02); /* function */ 

coverage_0x4c1d851e(0x5367b0c7e7531fd49da4dd79867a9ec6fd97a273f9af1e5f04e9fee57b334dba); /* line */ 
        coverage_0x4c1d851e(0xa32e2de1bdb534bcae05e1e332f4d48d6d142efeee31e4cb1d03c867102e40f5); /* statement */ 
Require.that(
            !desiredAmount.isNegative(),
            FILE,
            "Cannot getCost negative",
            desiredAmount.value
        );

coverage_0x4c1d851e(0xa7fc4325ef8a055d3b20e4f293a51fd94c5c743ecb35d0ba5b065af60aa791ce); /* line */ 
        coverage_0x4c1d851e(0x1b9b4df759f8315a31499b8ed8043a98e3b18ecbef48bc355df8feab86370e89); /* statement */ 
Types.Wei memory result;
coverage_0x4c1d851e(0x9faf754276f929a73f1c4bf70d526e26a3d8c76a8017e0f48f8212fe65701479); /* line */ 
        coverage_0x4c1d851e(0x2e87a720670628d199b6a35d73380af9e2d8ee545c81d410970443d873c2fc4e); /* statement */ 
result.sign = false;
coverage_0x4c1d851e(0x500cbd6eecc8256f79c5b9b316289a456ce1aced9fba34550e148fefce0cf83c); /* line */ 
        coverage_0x4c1d851e(0xf9bb29232c390d42576581a75b25e5183b7d2472346a6cbede078ff8ab69abd7); /* statement */ 
result.value = IExchangeWrapper(exchangeWrapper).getExchangeCost(
            supplyToken,
            borrowToken,
            desiredAmount.value,
            orderData
        );

coverage_0x4c1d851e(0x17f6f6af250082b71105dfa3ec55353d6b573ddf6a7e9e9b982f14ff542a5187); /* line */ 
        coverage_0x4c1d851e(0x7460847f219b6cde684c72cfe7eb5fb06726ddc1e3f6db764109567be377861e); /* statement */ 
return result;
    }

    function exchange(
        address exchangeWrapper,
        address accountOwner,
        address supplyToken,
        address borrowToken,
        Types.Wei memory requestedFillAmount,
        bytes memory orderData
    )
        internal
        returns (Types.Wei memory)
    {coverage_0x4c1d851e(0xae5f964e81e0fee8b9619daf587794da7807cbfafbbeadfccf42b4a8578eb7c2); /* function */ 

coverage_0x4c1d851e(0xa4ef47c533204549a83195f8707a846b26dbfc781afb208430fd0bcfc723649b); /* line */ 
        coverage_0x4c1d851e(0x75c2168eff88f65d3e40d9a8f5417bc3959bbf9575308c3c00fcef6e5e2692d0); /* statement */ 
Require.that(
            !requestedFillAmount.isPositive(),
            FILE,
            "Cannot exchange positive",
            requestedFillAmount.value
        );

coverage_0x4c1d851e(0x016e59e60f862f0d624c9aa101cb76a9d35ef8d7d61952d5f4b5bdd8dcfe3dd1); /* line */ 
        coverage_0x4c1d851e(0x8c287daf7f1fc9cd2be9fbc5660a036ac192794c5192e9428594e04c54124f4d); /* statement */ 
transferOut(borrowToken, exchangeWrapper, requestedFillAmount);

coverage_0x4c1d851e(0xf8b15a2b6df89c74c1cec3f38ef89cb4919111232a2dd1b9fbc5e7762aea1b3f); /* line */ 
        coverage_0x4c1d851e(0x152c2c9846ec7342eda8cec03d0f7112f30e022c4d93853fc5fa06a53b9067a6); /* statement */ 
Types.Wei memory result;
coverage_0x4c1d851e(0x4748788bf45c363f3b6188c1947a5ffe4ee5008173f3680b44cedf3b89822176); /* line */ 
        coverage_0x4c1d851e(0x83ffb9562bfda549c534c7fca6cb7a748d51ff3f345a66fa78d07d2be23b85b8); /* statement */ 
result.sign = true;
coverage_0x4c1d851e(0xe3bc023dcfad9c7468f6d0bc7a41879aac40b4655e3e1731a6227ea5543694e6); /* line */ 
        coverage_0x4c1d851e(0x4b1d1e1300d9a577e7d319ff56331b95d35e5f7afa2aace92c90949f10a13f52); /* statement */ 
result.value = IExchangeWrapper(exchangeWrapper).exchange(
            accountOwner,
            address(this),
            supplyToken,
            borrowToken,
            requestedFillAmount.value,
            orderData
        );

coverage_0x4c1d851e(0xffeabd2df227c4eacbc8bfb4a809bb9a97a5a3532426de73aeeda41ecfe739cb); /* line */ 
        coverage_0x4c1d851e(0x283d76bd2f95affc4c958109f86f0c91b1907e7b2d1af66a5ac885118b54b966); /* statement */ 
transferIn(supplyToken, exchangeWrapper, result);

coverage_0x4c1d851e(0xb0f4278f04ba9530bf37f83732ae58e7afa1fb0f0f844c2cb1ad94c77fe79d56); /* line */ 
        coverage_0x4c1d851e(0x06c7801886c13cb98c0f4473cc70a004f4edf574fb948ea749a0f6ceab89050f); /* statement */ 
return result;
    }
}
