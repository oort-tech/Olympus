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
var _this = this;
Object.defineProperty(exports, "__esModule", { value: true });
var bignumber_js_1 = __importDefault(require("bignumber.js"));
var Solo_1 = require("./helpers/Solo");
var EVM_1 = require("./helpers/EVM");
var SoloHelpers_1 = require("./helpers/SoloHelpers");
var Constants_1 = require("../src/lib/Constants");
var Expect_1 = require("../src/lib/Expect");
var types_1 = require("../src/types");
var solo;
var accounts;
var admin;
var rando;
var operator;
var owner1;
var owner2;
var soloAddress;
var oracleAddress;
var setterAddress;
var prices = [
    new bignumber_js_1.default(123),
    new bignumber_js_1.default(456),
    new bignumber_js_1.default(789),
];
var rates = [
    new bignumber_js_1.default(101).div(Constants_1.INTEGERS.INTEREST_RATE_BASE),
    new bignumber_js_1.default(202).div(Constants_1.INTEGERS.INTEREST_RATE_BASE),
    new bignumber_js_1.default(303).div(Constants_1.INTEGERS.INTEREST_RATE_BASE),
];
var defaultPremium = new bignumber_js_1.default(0);
var highPremium = new bignumber_js_1.default('.2');
var market1 = new bignumber_js_1.default(0);
var market2 = new bignumber_js_1.default(1);
var market3 = new bignumber_js_1.default(2);
var invalidMarket = new bignumber_js_1.default(101);
var account1 = new bignumber_js_1.default(111);
var account2 = new bignumber_js_1.default(222);
var par = new bignumber_js_1.default(100000000);
var wei = new bignumber_js_1.default(150000000);
var defaultIndex = {
    lastUpdate: Constants_1.INTEGERS.ZERO,
    borrow: wei.div(par),
    supply: wei.div(par),
};
var zero = Constants_1.INTEGERS.ZERO;
var tokens;
var MARKET_OOB_ERROR = 'Getters: Market OOB';
describe('Getters', function () {
    var snapshotId;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _a.sent();
                    tokens = [
                        solo.testing.tokenA.getAddress(),
                        solo.testing.tokenB.getAddress(),
                        solo.testing.tokenC.getAddress(),
                    ];
                    soloAddress = solo.contracts.soloMargin.options.address;
                    oracleAddress = solo.testing.priceOracle.getAddress();
                    setterAddress = solo.testing.interestSetter.getAddress();
                    return [4 /*yield*/, Promise.all([
                            solo.testing.priceOracle.setPrice(tokens[0], prices[0]),
                            solo.testing.priceOracle.setPrice(tokens[1], prices[1]),
                            solo.testing.priceOracle.setPrice(tokens[2], prices[2]),
                            solo.testing.interestSetter.setInterestRate(tokens[0], rates[0]),
                            solo.testing.interestSetter.setInterestRate(tokens[1], rates[1]),
                            solo.testing.interestSetter.setInterestRate(tokens[2], rates[2]),
                            solo.testing.setMarketIndex(market1, defaultIndex),
                            solo.testing.setMarketIndex(market2, defaultIndex),
                            solo.testing.setMarketIndex(market3, defaultIndex),
                        ])];
                case 4:
                    _a.sent();
                    admin = accounts[0];
                    rando = accounts[5];
                    operator = accounts[6];
                    owner1 = accounts[7];
                    owner2 = accounts[8];
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 5:
                    snapshotId = _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, EVM_1.resetEVM(snapshotId)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    // ============ Getters for Risk ============
    describe('Risk', function () {
        var defaultParams = {
            earningsRate: new bignumber_js_1.default('0.9'),
            marginRatio: new bignumber_js_1.default('0.15'),
            liquidationSpread: new bignumber_js_1.default('0.05'),
            minBorrowedValue: new bignumber_js_1.default('5e16'),
        };
        var defaultLimits = {
            marginRatioMax: new bignumber_js_1.default('2.0'),
            liquidationSpreadMax: new bignumber_js_1.default('0.5'),
            earningsRateMax: new bignumber_js_1.default('1.0'),
            marginPremiumMax: new bignumber_js_1.default('2.0'),
            spreadPremiumMax: new bignumber_js_1.default('2.0'),
            minBorrowedValueMax: new bignumber_js_1.default('100e18'),
        };
        describe('#getMarginRatio', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var value1, value2;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getMarginRatio()];
                        case 1:
                            value1 = _a.sent();
                            expect(value1).toEqual(defaultParams.marginRatio);
                            return [4 /*yield*/, solo.admin.setMarginRatio(defaultLimits.marginRatioMax, { from: admin })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getMarginRatio()];
                        case 3:
                            value2 = _a.sent();
                            expect(value2).toEqual(defaultLimits.marginRatioMax);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getLiquidationSpread', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var value1, doubledSpread, value2;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getLiquidationSpread()];
                        case 1:
                            value1 = _a.sent();
                            expect(value1).toEqual(defaultParams.liquidationSpread);
                            doubledSpread = value1.times(2);
                            return [4 /*yield*/, solo.admin.setLiquidationSpread(doubledSpread, { from: admin })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getLiquidationSpread()];
                        case 3:
                            value2 = _a.sent();
                            expect(value2).toEqual(doubledSpread);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getEarningsRate', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var value1, value2;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getEarningsRate()];
                        case 1:
                            value1 = _a.sent();
                            expect(value1).toEqual(defaultParams.earningsRate);
                            return [4 /*yield*/, solo.admin.setEarningsRate(defaultLimits.earningsRateMax, { from: admin })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getEarningsRate()];
                        case 3:
                            value2 = _a.sent();
                            expect(value2).toEqual(defaultLimits.earningsRateMax);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMinBorrowedValue', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var value1, value2;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getMinBorrowedValue()];
                        case 1:
                            value1 = _a.sent();
                            expect(value1).toEqual(defaultParams.minBorrowedValue);
                            return [4 /*yield*/, solo.admin.setMinBorrowedValue(defaultLimits.minBorrowedValueMax, { from: admin })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getMinBorrowedValue()];
                        case 3:
                            value2 = _a.sent();
                            expect(value2).toEqual(defaultLimits.minBorrowedValueMax);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getRiskParams', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var params;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getRiskParams()];
                        case 1:
                            params = _a.sent();
                            expect(params.earningsRate).toEqual(defaultParams.earningsRate);
                            expect(params.marginRatio).toEqual(defaultParams.marginRatio);
                            expect(params.liquidationSpread).toEqual(defaultParams.liquidationSpread);
                            expect(params.minBorrowedValue).toEqual(defaultParams.minBorrowedValue);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getRiskLimits', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var limits;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getRiskLimits()];
                        case 1:
                            limits = _a.sent();
                            expect(limits.marginRatioMax).toEqual(defaultLimits.marginRatioMax);
                            expect(limits.liquidationSpreadMax).toEqual(defaultLimits.liquidationSpreadMax);
                            expect(limits.earningsRateMax).toEqual(defaultLimits.earningsRateMax);
                            expect(limits.marginPremiumMax).toEqual(defaultLimits.marginPremiumMax);
                            expect(limits.spreadPremiumMax).toEqual(defaultLimits.spreadPremiumMax);
                            expect(limits.minBorrowedValueMax).toEqual(defaultLimits.minBorrowedValueMax);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
    });
    // ============ Getters for Markets ============
    describe('Markets', function () {
        describe('#getNumMarkets', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var nm1, token, nm2;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getNumMarkets()];
                        case 1:
                            nm1 = _a.sent();
                            expect(nm1).toEqual(new bignumber_js_1.default(3));
                            token = Constants_1.ADDRESSES.TEST[0];
                            return [4 /*yield*/, solo.testing.priceOracle.setPrice(token, prices[0])];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, solo.admin.addMarket(token, oracleAddress, setterAddress, defaultPremium, defaultPremium, { from: admin })];
                        case 3:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumMarkets()];
                        case 4:
                            nm2 = _a.sent();
                            expect(nm2).toEqual(nm1.plus(1));
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketTokenAddress', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var actualTokens;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getMarketTokenAddress(market1),
                                solo.getters.getMarketTokenAddress(market2),
                                solo.getters.getMarketTokenAddress(market3),
                            ])];
                        case 1:
                            actualTokens = _a.sent();
                            expect(actualTokens[0]).toEqual(tokens[0]);
                            expect(actualTokens[1]).toEqual(tokens[1]);
                            expect(actualTokens[2]).toEqual(tokens[2]);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketTokenAddress(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketTotalPar', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var totals;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market2, par),
                                solo.testing.setAccountBalance(owner2, account2, market3, par.times(-1)),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getMarketTotalPar(market1),
                                    solo.getters.getMarketTotalPar(market2),
                                    solo.getters.getMarketTotalPar(market3),
                                ])];
                        case 2:
                            totals = _a.sent();
                            expect(totals[0].supply).toEqual(zero);
                            expect(totals[0].borrow).toEqual(zero);
                            expect(totals[1].supply).toEqual(par);
                            expect(totals[1].borrow).toEqual(zero);
                            expect(totals[2].supply).toEqual(zero);
                            expect(totals[2].borrow).toEqual(par);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketTotalPar(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketCachedIndex', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var block1, timestamp, index, block2, result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.web3.eth.getBlock('latest')];
                        case 1:
                            block1 = _a.sent();
                            timestamp = new bignumber_js_1.default(block1.timestamp);
                            index = {
                                lastUpdate: new bignumber_js_1.default(timestamp),
                                borrow: new bignumber_js_1.default('1.456'),
                                supply: new bignumber_js_1.default('1.123'),
                            };
                            return [4 /*yield*/, solo.testing.setMarketIndex(market2, index)];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, EVM_1.mineAvgBlock()];
                        case 3:
                            _a.sent();
                            return [4 /*yield*/, solo.web3.eth.getBlock('latest')];
                        case 4:
                            block2 = _a.sent();
                            expect(block2.timestamp).toBeGreaterThan(block1.timestamp);
                            return [4 /*yield*/, solo.getters.getMarketCachedIndex(market2)];
                        case 5:
                            result = _a.sent();
                            expect(result).toEqual(index);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketCachedIndex(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketCurrentIndex', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var block1, timestamp, index, result, block2, _a, totalPar, interestRate, earningsRate, expectedIndex;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, solo.web3.eth.getBlock('latest')];
                        case 1:
                            block1 = _b.sent();
                            timestamp = new bignumber_js_1.default(block1.timestamp);
                            index = {
                                lastUpdate: timestamp,
                                borrow: new bignumber_js_1.default('1.456'),
                                supply: new bignumber_js_1.default('1.123'),
                            };
                            return [4 /*yield*/, Promise.all([
                                    solo.testing.setMarketIndex(market2, index),
                                    solo.testing.setAccountBalance(owner1, account1, market2, par.times(2)),
                                    solo.testing.setAccountBalance(owner2, account2, market2, par.times(-1)),
                                ])];
                        case 2:
                            _b.sent();
                            return [4 /*yield*/, EVM_1.mineAvgBlock()];
                        case 3:
                            _b.sent();
                            return [4 /*yield*/, solo.getters.getMarketCurrentIndex(market2)];
                        case 4:
                            result = _b.sent();
                            return [4 /*yield*/, solo.web3.eth.getBlock('latest')];
                        case 5:
                            block2 = _b.sent();
                            expect(block2.timestamp).toBeGreaterThan(block1.timestamp);
                            expect(result.lastUpdate.toNumber()).toBeGreaterThanOrEqual(block2.timestamp);
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getMarketTotalPar(market2),
                                    solo.getters.getMarketInterestRate(market2),
                                    solo.getters.getEarningsRate(),
                                ])];
                        case 6:
                            _a = _b.sent(), totalPar = _a[0], interestRate = _a[1], earningsRate = _a[2];
                            expectedIndex = getExpectedCurrentIndex(index, result.lastUpdate, totalPar, interestRate, earningsRate);
                            expect(result).toEqual(expectedIndex);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketCurrentIndex(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketPriceOracle', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var actualOracles;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getMarketPriceOracle(market1),
                                solo.getters.getMarketPriceOracle(market2),
                                solo.getters.getMarketPriceOracle(market3),
                            ])];
                        case 1:
                            actualOracles = _a.sent();
                            expect(actualOracles[0]).toEqual(oracleAddress);
                            expect(actualOracles[1]).toEqual(oracleAddress);
                            expect(actualOracles[2]).toEqual(oracleAddress);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketPriceOracle(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketInterestSetter', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var actualSetters;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getMarketInterestSetter(market1),
                                solo.getters.getMarketInterestSetter(market2),
                                solo.getters.getMarketInterestSetter(market3),
                            ])];
                        case 1:
                            actualSetters = _a.sent();
                            expect(actualSetters[0]).toEqual(setterAddress);
                            expect(actualSetters[1]).toEqual(setterAddress);
                            expect(actualSetters[2]).toEqual(setterAddress);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketInterestSetter(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketMarginPremium', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.admin.setMarginPremium(market2, highPremium, { from: admin })];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getMarketMarginPremium(market1),
                                    solo.getters.getMarketMarginPremium(market2),
                                    solo.getters.getMarketMarginPremium(market3),
                                ])];
                        case 2:
                            result = _a.sent();
                            expect(result[0]).toEqual(defaultPremium);
                            expect(result[1]).toEqual(highPremium);
                            expect(result[2]).toEqual(defaultPremium);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketMarginPremium(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketSpreadPremium', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.admin.setSpreadPremium(market2, highPremium, { from: admin })];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getMarketSpreadPremium(market1),
                                    solo.getters.getMarketSpreadPremium(market2),
                                    solo.getters.getMarketSpreadPremium(market3),
                                ])];
                        case 2:
                            result = _a.sent();
                            expect(result[0]).toEqual(defaultPremium);
                            expect(result[1]).toEqual(highPremium);
                            expect(result[2]).toEqual(defaultPremium);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketSpreadPremium(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketIsClosing', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var actualClosing;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.admin.setIsClosing(market2, true, { from: admin })];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getMarketIsClosing(market1),
                                    solo.getters.getMarketIsClosing(market2),
                                    solo.getters.getMarketIsClosing(market3),
                                ])];
                        case 2:
                            actualClosing = _a.sent();
                            expect(actualClosing[0]).toEqual(false);
                            expect(actualClosing[1]).toEqual(true);
                            expect(actualClosing[2]).toEqual(false);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketIsClosing(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketPrice', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var actualPrices;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getMarketPrice(market1),
                                solo.getters.getMarketPrice(market2),
                                solo.getters.getMarketPrice(market3),
                            ])];
                        case 1:
                            actualPrices = _a.sent();
                            expect(actualPrices[0]).toEqual(prices[0]);
                            expect(actualPrices[1]).toEqual(prices[1]);
                            expect(actualPrices[2]).toEqual(prices[2]);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketPrice(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketInterestRate', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var actualRates;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getMarketInterestRate(market1),
                                solo.getters.getMarketInterestRate(market2),
                                solo.getters.getMarketInterestRate(market3),
                            ])];
                        case 1:
                            actualRates = _a.sent();
                            expect(actualRates[0]).toEqual(rates[0]);
                            expect(actualRates[1]).toEqual(rates[1]);
                            expect(actualRates[2]).toEqual(rates[2]);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketInterestRate(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarket', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var block, index, market, expectedPar;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.web3.eth.getBlock('latest')];
                        case 1:
                            block = _a.sent();
                            index = {
                                lastUpdate: new bignumber_js_1.default(block.timestamp),
                                borrow: new bignumber_js_1.default('1.456'),
                                supply: new bignumber_js_1.default('1.123'),
                            };
                            return [4 /*yield*/, Promise.all([
                                    solo.admin.setIsClosing(market2, true, { from: admin }),
                                    solo.admin.setMarginPremium(market2, highPremium, { from: admin }),
                                    solo.admin.setSpreadPremium(market2, highPremium.div(2), { from: admin }),
                                    solo.testing.setMarketIndex(market2, index),
                                    solo.testing.setAccountBalance(owner1, account1, market2, par.times(2)),
                                    solo.testing.setAccountBalance(owner2, account2, market2, par.times(-1)),
                                ])];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getMarket(market2)];
                        case 3:
                            market = _a.sent();
                            expect(market.index).toEqual(index);
                            expect(market.interestSetter).toEqual(setterAddress);
                            expect(market.marginPremium).toEqual(highPremium);
                            expect(market.spreadPremium).toEqual(highPremium.div(2));
                            expect(market.isClosing).toEqual(true);
                            expect(market.priceOracle).toEqual(oracleAddress);
                            expect(market.token).toEqual(tokens[1]);
                            expectedPar = {
                                borrow: par,
                                supply: par.times(2),
                            };
                            expect(market.totalPar).toEqual(expectedPar);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarket(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getMarketWithInfo', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var block, index, _a, earningsRate, market, marketwi, expectedCurrentIndex;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, solo.web3.eth.getBlock('latest')];
                        case 1:
                            block = _b.sent();
                            index = {
                                lastUpdate: new bignumber_js_1.default(block.timestamp),
                                borrow: new bignumber_js_1.default('1.456'),
                                supply: new bignumber_js_1.default('1.123'),
                            };
                            return [4 /*yield*/, Promise.all([
                                    solo.admin.setIsClosing(market2, true, { from: admin }),
                                    solo.admin.setMarginPremium(market2, highPremium, { from: admin }),
                                    solo.admin.setSpreadPremium(market2, highPremium.div(2), { from: admin }),
                                    solo.testing.setMarketIndex(market2, index),
                                    solo.testing.setAccountBalance(owner1, account1, market2, par.times(2)),
                                    solo.testing.setAccountBalance(owner2, account2, market2, par.times(-1)),
                                ])];
                        case 2:
                            _b.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getEarningsRate(),
                                    solo.getters.getMarket(market2),
                                    solo.getters.getMarketWithInfo(market2),
                                ])];
                        case 3:
                            _a = _b.sent(), earningsRate = _a[0], market = _a[1], marketwi = _a[2];
                            expect(marketwi.market).toEqual(market);
                            expectedCurrentIndex = getExpectedCurrentIndex(index, marketwi.currentIndex.lastUpdate, marketwi.market.totalPar, marketwi.currentInterestRate, earningsRate);
                            expect(marketwi.currentIndex).toEqual(expectedCurrentIndex);
                            expect(marketwi.currentPrice).toEqual(prices[1]);
                            expect(marketwi.currentInterestRate).toEqual(rates[1]);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getMarketWithInfo(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getNumExcessTokens', function () {
            it('Succeeds for all zeroes', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner1, account1, market2, par)];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(zero);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for zero (zero balance)', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(2)),
                                solo.testing.setAccountBalance(owner2, account1, market1, par.times(-1)),
                                solo.testing.setAccountBalance(owner2, account2, market1, par.times(-1)),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(zero);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for zero (non-zero balance)', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(2)),
                                solo.testing.setAccountBalance(owner2, account2, market1, par.times(-1)),
                                solo.testing.tokenA.issueTo(wei, soloAddress),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(zero);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for positive (zero balance)', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(1)),
                                solo.testing.setAccountBalance(owner2, account2, market1, par.times(-2)),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(wei);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for positive > balance', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(1)),
                                solo.testing.setAccountBalance(owner2, account2, market1, par.times(-2)),
                                solo.testing.tokenA.issueTo(wei, soloAddress),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(wei.times(2));
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for positive < balance', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(3)),
                                solo.testing.setAccountBalance(owner2, account2, market1, par.times(-2)),
                                solo.testing.tokenA.issueTo(wei.times(2), soloAddress),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(wei);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for negative (zero balance)', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(2)),
                                solo.testing.setAccountBalance(owner2, account2, market1, par.times(-1)),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(wei.times(-1));
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for negative (non-zero balance)', function () { return __awaiter(_this, void 0, void 0, function () {
                var result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(3)),
                                solo.testing.setAccountBalance(owner2, account2, market1, par.times(-1)),
                                solo.testing.tokenA.issueTo(wei, soloAddress),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getNumExcessTokens(market1)];
                        case 2:
                            result = _a.sent();
                            expect(result).toEqual(wei.times(-1));
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getNumExcessTokens(invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        function getExpectedCurrentIndex(oldIndex, newTimestamp, totalPar, interestRate, earningsRate) {
            var timeDiff = newTimestamp.minus(oldIndex.lastUpdate);
            expect(timeDiff.isPositive()).toBeTruthy();
            var borrowInterest = interestRate.times(timeDiff);
            expect(borrowInterest.isZero()).toBeFalsy();
            var borrowWei = totalPar.borrow.times(oldIndex.borrow).integerValue(bignumber_js_1.default.ROUND_UP);
            var supplyWei = totalPar.supply.times(oldIndex.supply).integerValue(bignumber_js_1.default.ROUND_DOWN);
            var supplyInterest = crop(borrowInterest.times(earningsRate));
            if (borrowWei.lt(supplyWei)) {
                supplyInterest =
                    crop(crop(supplyInterest.times(borrowWei)).div(supplyWei));
            }
            expect(supplyInterest.lte(borrowInterest)).toBeTruthy();
            return {
                supply: crop(oldIndex.supply.times(supplyInterest.plus(1))),
                borrow: crop(oldIndex.borrow.times(borrowInterest.plus(1))),
                lastUpdate: newTimestamp,
            };
        }
        function crop(b) {
            return b.times('1e18').integerValue(bignumber_js_1.default.ROUND_DOWN).div('1e18');
        }
    });
    // ============ Getters for Accounts ============
    describe('Accounts', function () {
        describe('#getAccountPar', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var _a, par1, par2;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par),
                                solo.testing.setAccountBalance(owner1, account1, market2, par.div(-2)),
                            ])];
                        case 1:
                            _b.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getAccountPar(owner1, account1, market1),
                                    solo.getters.getAccountPar(owner1, account1, market2),
                                ])];
                        case 2:
                            _a = _b.sent(), par1 = _a[0], par2 = _a[1];
                            expect(par1).toEqual(par);
                            expect(par2).toEqual(par.div(-2));
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getAccountPar(owner1, account1, invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getAccountWei', function () {
            it('Succeeds for zero interest', function () { return __awaiter(_this, void 0, void 0, function () {
                var _a, wei1, wei2;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.interestSetter.setInterestRate(tokens[0], zero),
                                solo.testing.interestSetter.setInterestRate(tokens[1], zero),
                                solo.testing.interestSetter.setInterestRate(tokens[2], zero),
                                solo.testing.setAccountBalance(owner1, account1, market1, par),
                                solo.testing.setAccountBalance(owner1, account1, market2, par.div(-2)),
                            ])];
                        case 1:
                            _b.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getAccountWei(owner1, account1, market1),
                                    solo.getters.getAccountWei(owner1, account1, market2),
                                ])];
                        case 2:
                            _a = _b.sent(), wei1 = _a[0], wei2 = _a[1];
                            expect(wei1).toEqual(wei);
                            expect(wei2).toEqual(wei.div(-2));
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for some interest', function () { return __awaiter(_this, void 0, void 0, function () {
                var interest, _a, weiA1, weiB1, _b, weiA2, weiB2;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0:
                            interest = new bignumber_js_1.default(2);
                            return [4 /*yield*/, Promise.all([
                                    solo.testing.interestSetter.setInterestRate(tokens[0], interest),
                                    solo.testing.interestSetter.setInterestRate(tokens[1], interest),
                                    solo.testing.interestSetter.setInterestRate(tokens[2], interest),
                                    solo.testing.setAccountBalance(owner1, account1, market1, par),
                                    solo.testing.setAccountBalance(owner2, account2, market1, par.div(-2)),
                                    solo.testing.setAccountBalance(owner1, account1, market2, par.div(-2)),
                                    solo.testing.setAccountBalance(owner2, account2, market2, par.div(4)),
                                ])];
                        case 1:
                            _c.sent();
                            return [4 /*yield*/, EVM_1.mineAvgBlock()];
                        case 2:
                            _c.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getAccountWei(owner1, account1, market1),
                                    solo.getters.getAccountWei(owner1, account1, market2),
                                ])];
                        case 3:
                            _a = _c.sent(), weiA1 = _a[0], weiB1 = _a[1];
                            expect(weiA1.gte(wei)).toBeTruthy();
                            expect(weiB1.lt(wei.div(-2))).toBeTruthy(); // lt is gt in the negative direction
                            return [4 /*yield*/, EVM_1.fastForward(86400)];
                        case 4:
                            _c.sent(); // one day
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getAccountWei(owner1, account1, market1),
                                    solo.getters.getAccountWei(owner1, account1, market2),
                                ])];
                        case 5:
                            _b = _c.sent(), weiA2 = _b[0], weiB2 = _b[1];
                            expect(weiA2.gt(weiA1)).toBeTruthy();
                            expect(weiB2.lt(weiB1)).toBeTruthy(); // lt is gt in the negative direction
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for market OOB', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.getters.getAccountWei(owner1, account1, invalidMarket), MARKET_OOB_ERROR)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getAccountStatus', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var status;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.getters.getAccountStatus(owner1, account1)];
                        case 1:
                            status = _a.sent();
                            expect(status).toEqual(types_1.AccountStatus.Normal);
                            return [4 /*yield*/, solo.testing.setAccountStatus(owner1, account1, types_1.AccountStatus.Liquidating)];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getAccountStatus(owner1, account1)];
                        case 3:
                            status = _a.sent();
                            expect(status).toEqual(types_1.AccountStatus.Liquidating);
                            return [4 /*yield*/, solo.testing.setAccountStatus(owner1, account1, types_1.AccountStatus.Vaporizing)];
                        case 4:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getAccountStatus(owner1, account1)];
                        case 5:
                            status = _a.sent();
                            expect(status).toEqual(types_1.AccountStatus.Vaporizing);
                            return [4 /*yield*/, solo.testing.setAccountStatus(owner1, account1, types_1.AccountStatus.Normal)];
                        case 6:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getAccountStatus(owner1, account1)];
                        case 7:
                            status = _a.sent();
                            expect(status).toEqual(types_1.AccountStatus.Normal);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getAccountValues', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var _a, values1, values2;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par),
                                solo.testing.setAccountBalance(owner1, account1, market2, par.div(-2)),
                            ])];
                        case 1:
                            _b.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getAccountValues(owner1, account1),
                                    solo.getters.getAccountValues(owner2, account2),
                                ])];
                        case 2:
                            _a = _b.sent(), values1 = _a[0], values2 = _a[1];
                            expect(values1.borrow).toEqual(prices[1].times(wei.div(2)));
                            expect(values1.supply).toEqual(prices[0].times(wei));
                            expect(values2.borrow).toEqual(zero);
                            expect(values2.supply).toEqual(zero);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getAdjustedAccountValues', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var rating1, rating2, _a, values1, values2;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0:
                            rating1 = new bignumber_js_1.default('1.2');
                            rating2 = new bignumber_js_1.default('1.5');
                            return [4 /*yield*/, Promise.all([
                                    solo.admin.setMarginPremium(market1, rating1.minus(1), { from: admin }),
                                    solo.admin.setMarginPremium(market2, rating2.minus(1), { from: admin }),
                                    solo.testing.setAccountBalance(owner1, account1, market1, par),
                                    solo.testing.setAccountBalance(owner1, account1, market2, par.div(-2)),
                                ])];
                        case 1:
                            _b.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getAdjustedAccountValues(owner1, account1),
                                    solo.getters.getAdjustedAccountValues(owner2, account2),
                                ])];
                        case 2:
                            _a = _b.sent(), values1 = _a[0], values2 = _a[1];
                            expect(values1.borrow).toEqual(prices[1].times(wei.div(2)).times(rating2));
                            expect(values1.supply).toEqual(prices[0].times(wei).div(rating1));
                            expect(values2.borrow).toEqual(zero);
                            expect(values2.supply).toEqual(zero);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getAccountBalances', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var _a, balances1, balances2;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par),
                                solo.testing.setAccountBalance(owner1, account1, market2, par.div(-2)),
                            ])];
                        case 1:
                            _b.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getAccountBalances(owner1, account1),
                                    solo.getters.getAccountBalances(owner2, account2),
                                ])];
                        case 2:
                            _a = _b.sent(), balances1 = _a[0], balances2 = _a[1];
                            balances1.forEach(function (balance, i) {
                                if (i === market1.toNumber()) {
                                    expect(balance.par).toEqual(par);
                                    expect(balance.wei).toEqual(wei);
                                }
                                else if (i === market2.toNumber()) {
                                    expect(balance.par).toEqual(par.div(-2));
                                    expect(balance.wei.lt(wei.div(-2))).toBeTruthy();
                                }
                                else {
                                    expect(balance.par).toEqual(zero);
                                    expect(balance.wei).toEqual(zero);
                                }
                                expect(balance.tokenAddress).toEqual(tokens[i]);
                            });
                            balances2.forEach(function (balance, i) {
                                expect(balance.par).toEqual(zero);
                                expect(balance.wei).toEqual(zero);
                                expect(balance.tokenAddress).toEqual(tokens[i]);
                            });
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#isAccountLiquidatable', function () {
            it('True for undercollateralized account', function () { return __awaiter(_this, void 0, void 0, function () {
                var liquidatable;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par),
                                solo.testing.setAccountBalance(owner1, account1, market2, par.times(-1)),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.isAccountLiquidatable(owner1, account1)];
                        case 2:
                            liquidatable = _a.sent();
                            expect(liquidatable).toBe(true);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('True for partially liquidated account', function () { return __awaiter(_this, void 0, void 0, function () {
                var liquidatable;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(-1)),
                                solo.testing.setAccountBalance(owner1, account1, market2, par.times(2)),
                                solo.testing.setAccountStatus(owner1, account1, types_1.AccountStatus.Liquidating),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.isAccountLiquidatable(owner1, account1)];
                        case 2:
                            liquidatable = _a.sent();
                            expect(liquidatable).toBe(true);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('False for collateralized account', function () { return __awaiter(_this, void 0, void 0, function () {
                var liquidatable;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(-1)),
                                solo.testing.setAccountBalance(owner1, account1, market2, par.times(2)),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.isAccountLiquidatable(owner1, account1)];
                        case 2:
                            liquidatable = _a.sent();
                            expect(liquidatable).toBe(false);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('False for vaporizable account', function () { return __awaiter(_this, void 0, void 0, function () {
                var liquidatable;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, account1, market1, par.times(-1)),
                                solo.testing.setAccountStatus(owner1, account1, types_1.AccountStatus.Liquidating),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.isAccountLiquidatable(owner1, account1)];
                        case 2:
                            liquidatable = _a.sent();
                            expect(liquidatable).toBe(false);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
    });
    // ============ Getters for Permissions ============
    describe('Permissions', function () {
        describe('#getIsLocalOperator', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var _a, _b, _c, b1, b2, b3, b4;
                return __generator(this, function (_d) {
                    switch (_d.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getIsLocalOperator(owner1, operator),
                                solo.getters.getIsLocalOperator(owner2, operator),
                                solo.getters.getIsLocalOperator(owner1, rando),
                                solo.getters.getIsLocalOperator(owner2, rando),
                            ])];
                        case 1:
                            _a = _d.sent(), b1 = _a[0], b2 = _a[1], b3 = _a[2], b4 = _a[3];
                            expect(b1).toEqual(false);
                            expect(b2).toEqual(false);
                            expect(b3).toEqual(false);
                            expect(b4).toEqual(false);
                            return [4 /*yield*/, Promise.all([
                                    solo.permissions.approveOperator(operator, { from: owner1 }),
                                    solo.permissions.disapproveOperator(operator, { from: owner2 }),
                                ])];
                        case 2:
                            _d.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getIsLocalOperator(owner1, operator),
                                    solo.getters.getIsLocalOperator(owner2, operator),
                                    solo.getters.getIsLocalOperator(owner1, rando),
                                    solo.getters.getIsLocalOperator(owner2, rando),
                                ])];
                        case 3:
                            _b = _d.sent(), b1 = _b[0], b2 = _b[1], b3 = _b[2], b4 = _b[3];
                            expect(b1).toEqual(true);
                            expect(b2).toEqual(false);
                            expect(b3).toEqual(false);
                            expect(b4).toEqual(false);
                            return [4 /*yield*/, Promise.all([
                                    solo.permissions.disapproveOperator(operator, { from: owner1 }),
                                    solo.permissions.approveOperator(operator, { from: owner2 }),
                                    solo.permissions.approveOperator(rando, { from: owner1 }),
                                ])];
                        case 4:
                            _d.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getIsLocalOperator(owner1, operator),
                                    solo.getters.getIsLocalOperator(owner2, operator),
                                    solo.getters.getIsLocalOperator(owner1, rando),
                                    solo.getters.getIsLocalOperator(owner2, rando),
                                ])];
                        case 5:
                            _c = _d.sent(), b1 = _c[0], b2 = _c[1], b3 = _c[2], b4 = _c[3];
                            expect(b1).toEqual(false);
                            expect(b2).toEqual(true);
                            expect(b3).toEqual(true);
                            expect(b4).toEqual(false);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('#getIsGlobalOperator', function () {
            it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
                var _a, _b, _c, _d, rando, operator, b1, b2;
                return __generator(this, function (_e) {
                    switch (_e.label) {
                        case 0:
                            rando = accounts[5];
                            operator = accounts[6];
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getIsGlobalOperator(operator),
                                    solo.getters.getIsGlobalOperator(rando),
                                ])];
                        case 1:
                            _a = _e.sent(), b1 = _a[0], b2 = _a[1];
                            expect(b1).toEqual(false);
                            expect(b2).toEqual(false);
                            return [4 /*yield*/, solo.admin.setGlobalOperator(operator, true, { from: admin })];
                        case 2:
                            _e.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getIsGlobalOperator(operator),
                                    solo.getters.getIsGlobalOperator(rando),
                                ])];
                        case 3:
                            _b = _e.sent(), b1 = _b[0], b2 = _b[1];
                            expect(b1).toEqual(true);
                            expect(b2).toEqual(false);
                            return [4 /*yield*/, solo.admin.setGlobalOperator(rando, true, { from: admin })];
                        case 4:
                            _e.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getIsGlobalOperator(operator),
                                    solo.getters.getIsGlobalOperator(rando),
                                ])];
                        case 5:
                            _c = _e.sent(), b1 = _c[0], b2 = _c[1];
                            expect(b1).toEqual(true);
                            expect(b2).toEqual(true);
                            return [4 /*yield*/, solo.admin.setGlobalOperator(operator, false, { from: admin })];
                        case 6:
                            _e.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.getters.getIsGlobalOperator(operator),
                                    solo.getters.getIsGlobalOperator(rando),
                                ])];
                        case 7:
                            _d = _e.sent(), b1 = _d[0], b2 = _d[1];
                            expect(b1).toEqual(false);
                            expect(b2).toEqual(true);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
    });
});
//# sourceMappingURL=Getters.test.js.map