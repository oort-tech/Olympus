"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : new P(function (resolve) { resolve(result.value); }).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var bignumber_js_1 = __importDefault(require("bignumber.js"));
var EVM_1 = require("./EVM");
var TestToken_1 = require("./TestToken");
var TestAutoTrader_1 = require("./TestAutoTrader");
var TestCallee_1 = require("./TestCallee");
var TestSimpleCallee_1 = require("./TestSimpleCallee");
var TestExchangeWrapper_1 = require("./TestExchangeWrapper");
var TestPriceOracle_1 = require("./TestPriceOracle");
var TestPolynomialInterestSetter_1 = require("./TestPolynomialInterestSetter");
var TestDoubleExponentInterestSetter_1 = require("./TestDoubleExponentInterestSetter");
var TestInterestSetter_1 = require("./TestInterestSetter");
var Helpers_1 = require("../../src/lib/Helpers");
var Testing = /** @class */ (function () {
    function Testing(provider, contracts, token) {
        this.contracts = contracts;
        this.evm = new EVM_1.EVM(provider);
        this.tokenA = new TestToken_1.TestToken(contracts, token, contracts.tokenA);
        this.tokenB = new TestToken_1.TestToken(contracts, token, contracts.tokenB);
        this.tokenC = new TestToken_1.TestToken(contracts, token, contracts.tokenC);
        this.erroringToken = new TestToken_1.TestToken(contracts, token, contracts.erroringToken);
        this.omiseToken = new TestToken_1.TestToken(contracts, token, contracts.omiseToken);
        this.autoTrader = new TestAutoTrader_1.TestAutoTrader(contracts);
        this.callee = new TestCallee_1.TestCallee(contracts);
        this.simpleCallee = new TestSimpleCallee_1.TestSimpleCallee(contracts);
        this.exchangeWrapper = new TestExchangeWrapper_1.TestExchangeWrapper(contracts);
        this.priceOracle = new TestPriceOracle_1.TestPriceOracle(contracts);
        this.polynomialInterestSetter = new TestPolynomialInterestSetter_1.TestPolynomialInterestSetter(contracts);
        this.doubleExponentInterestSetter = new TestDoubleExponentInterestSetter_1.TestDoubleExponentInterestSetter(contracts);
        this.interestSetter = new TestInterestSetter_1.TestInterestSetter(contracts);
    }
    Testing.prototype.setProvider = function (provider) {
        this.evm.setProvider(provider);
    };
    Testing.prototype.setAccountBalance = function (accountOwner, accountNumber, marketId, par, options) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                return [2 /*return*/, this.contracts.send(this.contracts.testSoloMargin.methods.setAccountBalance({
                        owner: accountOwner,
                        number: accountNumber.toFixed(0),
                    }, marketId.toFixed(0), {
                        sign: par.gt(0),
                        value: par.abs().toFixed(0),
                    }), options)];
            });
        });
    };
    Testing.prototype.setAccountStatus = function (accountOwner, accountNumber, status, options) {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                return [2 /*return*/, this.contracts.send(this.contracts.testSoloMargin.methods.setAccountStatus({
                        owner: accountOwner,
                        number: accountNumber.toFixed(0),
                    }, status), options)];
            });
        });
    };
    Testing.prototype.setMarketIndex = function (marketId, index, options) {
        return __awaiter(this, void 0, void 0, function () {
            var currentIndex;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        if (!index.lastUpdate.isZero()) return [3 /*break*/, 2];
                        return [4 /*yield*/, this.contracts.testSoloMargin.methods.getMarketCachedIndex(marketId.toFixed(0)).call()];
                    case 1:
                        currentIndex = _a.sent();
                        index.lastUpdate = new bignumber_js_1.default(currentIndex.lastUpdate);
                        _a.label = 2;
                    case 2: return [2 /*return*/, this.contracts.send(this.contracts.testSoloMargin.methods.setMarketIndex(marketId.toFixed(0), {
                            borrow: Helpers_1.decimalToString(index.borrow),
                            supply: Helpers_1.decimalToString(index.supply),
                            lastUpdate: index.lastUpdate.toFixed(0),
                        }), options)];
                }
            });
        });
    };
    return Testing;
}());
exports.Testing = Testing;
//# sourceMappingURL=Testing.js.map