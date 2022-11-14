"use strict";
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
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
// JSON
var TestSoloMargin_json_1 = __importDefault(require("../../build/contracts/TestSoloMargin.json"));
var TokenA_json_1 = __importDefault(require("../../build/contracts/TokenA.json"));
var TokenB_json_1 = __importDefault(require("../../build/contracts/TokenB.json"));
var TokenC_json_1 = __importDefault(require("../../build/contracts/TokenC.json"));
var ErroringToken_json_1 = __importDefault(require("../../build/contracts/ErroringToken.json"));
var OmiseToken_json_1 = __importDefault(require("../../build/contracts/OmiseToken.json"));
var TestLib_json_1 = __importDefault(require("../../build/contracts/TestLib.json"));
var TestAutoTrader_json_1 = __importDefault(require("../../build/contracts/TestAutoTrader.json"));
var TestCallee_json_1 = __importDefault(require("../../build/contracts/TestCallee.json"));
var TestSimpleCallee_json_1 = __importDefault(require("../../build/contracts/TestSimpleCallee.json"));
var TestExchangeWrapper_json_1 = __importDefault(require("../../build/contracts/TestExchangeWrapper.json"));
var TestPriceOracle_json_1 = __importDefault(require("../../build/contracts/TestPriceOracle.json"));
var TestMakerOracle_json_1 = __importDefault(require("../../build/contracts/TestMakerOracle.json"));
var TestCurve_json_1 = __importDefault(require("../../build/contracts/TestCurve.json"));
var TestUniswapV2Pair_json_1 = __importDefault(require("../../build/contracts/TestUniswapV2Pair.json"));
var TestUniswapV2Pair2_json_1 = __importDefault(require("../../build/contracts/TestUniswapV2Pair2.json"));
var TestPolynomialInterestSetter_json_1 = __importDefault(require("../../build/contracts/TestPolynomialInterestSetter.json"));
var TestDoubleExponentInterestSetter_json_1 = __importDefault(require("../../build/contracts/TestDoubleExponentInterestSetter.json"));
var TestInterestSetter_json_1 = __importDefault(require("../../build/contracts/TestInterestSetter.json"));
var Contracts_1 = require("../../src/lib/Contracts");
var TestContracts = /** @class */ (function (_super) {
    __extends(TestContracts, _super);
    function TestContracts(provider, networkId, web3, options) {
        var _this = _super.call(this, provider, networkId, web3, options) || this;
        // Testing Contracts
        _this.testSoloMargin = new _this.web3.eth.Contract(TestSoloMargin_json_1.default.abi);
        _this.soloMargin = _this.testSoloMargin;
        _this.tokenA = new _this.web3.eth.Contract(TokenA_json_1.default.abi);
        _this.tokenB = new _this.web3.eth.Contract(TokenB_json_1.default.abi);
        _this.tokenC = new _this.web3.eth.Contract(TokenC_json_1.default.abi);
        _this.erroringToken = new _this.web3.eth.Contract(ErroringToken_json_1.default.abi);
        _this.omiseToken = new _this.web3.eth.Contract(OmiseToken_json_1.default.abi);
        _this.testLib = new _this.web3.eth.Contract(TestLib_json_1.default.abi);
        _this.testAutoTrader = new _this.web3.eth.Contract(TestAutoTrader_json_1.default.abi);
        _this.testCallee = new _this.web3.eth.Contract(TestCallee_json_1.default.abi);
        _this.testSimpleCallee = new _this.web3.eth.Contract(TestSimpleCallee_json_1.default.abi);
        _this.testExchangeWrapper = new _this.web3.eth.Contract(TestExchangeWrapper_json_1.default.abi);
        _this.testPriceOracle = new _this.web3.eth.Contract(TestPriceOracle_json_1.default.abi);
        _this.testMakerOracle = new _this.web3.eth.Contract(TestMakerOracle_json_1.default.abi);
        _this.testCurve = new _this.web3.eth.Contract(TestCurve_json_1.default.abi);
        _this.testUniswapV2Pair = new _this.web3.eth.Contract(TestUniswapV2Pair_json_1.default.abi);
        _this.testUniswapV2Pair2 = new _this.web3.eth.Contract(TestUniswapV2Pair2_json_1.default.abi);
        _this.testInterestSetter = new _this.web3.eth.Contract(TestInterestSetter_json_1.default.abi);
        _this.testPolynomialInterestSetter = new _this.web3.eth.Contract(TestPolynomialInterestSetter_json_1.default.abi);
        _this.testDoubleExponentInterestSetter = new _this.web3.eth.Contract(TestDoubleExponentInterestSetter_json_1.default.abi);
        _this.setProvider(provider, networkId);
        _this.setDefaultAccount(_this.web3.eth.defaultAccount);
        return _this;
    }
    TestContracts.prototype.setProvider = function (provider, networkId) {
        var _this = this;
        _super.prototype.setProvider.call(this, provider, networkId);
        // do not continue if not initialized
        if (!this.tokenA) {
            return;
        }
        this.soloMargin.setProvider(provider);
        var contracts = [
            // test contracts
            { contract: this.testSoloMargin, json: TestSoloMargin_json_1.default },
            { contract: this.tokenA, json: TokenA_json_1.default },
            { contract: this.tokenB, json: TokenB_json_1.default },
            { contract: this.tokenC, json: TokenC_json_1.default },
            { contract: this.erroringToken, json: ErroringToken_json_1.default },
            { contract: this.omiseToken, json: OmiseToken_json_1.default },
            { contract: this.testLib, json: TestLib_json_1.default },
            { contract: this.testAutoTrader, json: TestAutoTrader_json_1.default },
            { contract: this.testCallee, json: TestCallee_json_1.default },
            { contract: this.testSimpleCallee, json: TestSimpleCallee_json_1.default },
            { contract: this.testExchangeWrapper, json: TestExchangeWrapper_json_1.default },
            { contract: this.testPriceOracle, json: TestPriceOracle_json_1.default },
            { contract: this.testMakerOracle, json: TestMakerOracle_json_1.default },
            { contract: this.testCurve, json: TestCurve_json_1.default },
            { contract: this.testUniswapV2Pair, json: TestUniswapV2Pair_json_1.default },
            { contract: this.testUniswapV2Pair2, json: TestUniswapV2Pair2_json_1.default },
            { contract: this.testPolynomialInterestSetter, json: TestPolynomialInterestSetter_json_1.default },
            { contract: this.testDoubleExponentInterestSetter,
                json: TestDoubleExponentInterestSetter_json_1.default },
            { contract: this.testInterestSetter, json: TestInterestSetter_json_1.default },
        ];
        contracts.forEach(function (contract) { return _this.setContractProvider(contract.contract, contract.json, provider, networkId, null); });
    };
    TestContracts.prototype.setDefaultAccount = function (account) {
        _super.prototype.setDefaultAccount.call(this, account);
        // do not continue if not initialized
        if (!this.tokenA) {
            return;
        }
        // Test Contracts
        this.tokenA.options.from = account;
        this.tokenB.options.from = account;
        this.tokenC.options.from = account;
        this.erroringToken.options.from = account;
        this.omiseToken.options.from = account;
        this.testLib.options.from = account;
        this.testAutoTrader.options.from = account;
        this.testCallee.options.from = account;
        this.testSimpleCallee.options.from = account;
        this.testExchangeWrapper.options.from = account;
        this.testPriceOracle.options.from = account;
        this.testMakerOracle.options.from = account;
        this.testCurve.options.from = account;
        this.testUniswapV2Pair.options.from = account;
        this.testUniswapV2Pair2.options.from = account;
        this.testPolynomialInterestSetter.options.from = account;
        this.testDoubleExponentInterestSetter.options.from = account;
        this.testInterestSetter.options.from = account;
    };
    return TestContracts;
}(Contracts_1.Contracts));
exports.TestContracts = TestContracts;
//# sourceMappingURL=TestContracts.js.map