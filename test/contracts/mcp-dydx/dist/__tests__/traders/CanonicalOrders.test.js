"use strict";
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
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
var Solo_1 = require("../helpers/Solo");
var EVM_1 = require("../helpers/EVM");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var SoloHelpers_1 = require("../helpers/SoloHelpers");
var BytesHelper_1 = require("../../src/lib/BytesHelper");
var types_1 = require("../../src/types");
var solo;
var accounts;
var snapshotId;
var BIP = new bignumber_js_1.default('1e-4');
var MINIMAL_PRICE_INCREMENT = new bignumber_js_1.default('1e-18');
var MINIMAL_FEE_INCREMENT = new bignumber_js_1.default('1e-18');
var baseMarket = new bignumber_js_1.default(0);
var quoteMarket = new bignumber_js_1.default(1);
var incorrectMarket = new bignumber_js_1.default(2);
var defaultMakerNumber = new bignumber_js_1.default(111);
var defaultTakerNumber = new bignumber_js_1.default(222);
var defaultAmount = new bignumber_js_1.default('16e18');
var defaultPrice = new bignumber_js_1.default('160');
var defaultQuoteAmount = defaultAmount.times(defaultPrice);
var defaultLimitFee = BIP.times(20);
var admin;
var defaultMakerAddress;
var defaultTakerAddress;
var rando;
var testOrder;
var noFeeOrder;
var negativeFeeOrder;
var sellOrder;
var decreaseOrder;
var reverseDecreaseOrder;
describe('CanonicalOrders', function () {
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, r, _b;
        return __generator(this, function (_c) {
            switch (_c.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _c.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    admin = accounts[0];
                    defaultMakerAddress = accounts[6];
                    return [4 /*yield*/, solo.canonicalOrders.getTakerAddress()];
                case 2:
                    defaultTakerAddress = _c.sent();
                    rando = accounts[9];
                    testOrder = {
                        baseMarket: baseMarket,
                        quoteMarket: quoteMarket,
                        isBuy: true,
                        isDecreaseOnly: false,
                        amount: defaultAmount,
                        limitPrice: defaultPrice,
                        triggerPrice: Constants_1.INTEGERS.ZERO,
                        limitFee: defaultLimitFee,
                        makerAccountOwner: defaultMakerAddress,
                        makerAccountNumber: defaultMakerNumber,
                        expiration: Constants_1.INTEGERS.ONES_31,
                        salt: new bignumber_js_1.default(100),
                        typedSignature: null,
                    };
                    _b = testOrder;
                    return [4 /*yield*/, solo.canonicalOrders.signOrder(testOrder, types_1.SigningMethod.TypedData)];
                case 3:
                    _b.typedSignature =
                        _c.sent();
                    return [4 /*yield*/, Promise.all([
                            getModifiedTestOrder({ isBuy: false }),
                            getModifiedTestOrder({ limitFee: Constants_1.INTEGERS.ZERO }),
                            getModifiedTestOrder({ limitFee: defaultLimitFee.negated() }),
                            getModifiedTestOrder({ isDecreaseOnly: true }),
                            getModifiedTestOrder({ isBuy: false, isDecreaseOnly: true }),
                        ])];
                case 4:
                    _a = _c.sent(), sellOrder = _a[0], noFeeOrder = _a[1], negativeFeeOrder = _a[2], decreaseOrder = _a[3], reverseDecreaseOrder = _a[4];
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 5:
                    _c.sent();
                    // set balances
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 6:
                    // set balances
                    _c.sent();
                    return [4 /*yield*/, Promise.all([
                            setBalances(defaultAmount, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO, defaultQuoteAmount),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenA.getAddress(), defaultPrice),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenB.getAddress(), Constants_1.INTEGERS.ONE),
                        ])];
                case 7: return [4 /*yield*/, _c.sent()];
                case 8:
                    _c.sent();
                    return [4 /*yield*/, EVM_1.mineAvgBlock()];
                case 9:
                    _c.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 10:
                    snapshotId = _c.sent();
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
    describe('Signing Orders', function () {
        it('Succeeds for eth.sign', function () { return __awaiter(_this, void 0, void 0, function () {
            var order, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        order = __assign({}, testOrder);
                        _a = order;
                        return [4 /*yield*/, solo.canonicalOrders.signOrder(order, types_1.SigningMethod.Hash)];
                    case 1:
                        _a.typedSignature = _b.sent();
                        expect(solo.canonicalOrders.orderHasValidSignature(order)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for eth_signTypedData', function () { return __awaiter(_this, void 0, void 0, function () {
            var order, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        order = __assign({}, testOrder);
                        _a = order;
                        return [4 /*yield*/, solo.canonicalOrders.signOrder(order, types_1.SigningMethod.TypedData)];
                    case 1:
                        _a.typedSignature = _b.sent();
                        expect(solo.canonicalOrders.orderHasValidSignature(order)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Recognizes a bad signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var order;
            return __generator(this, function (_a) {
                order = __assign({}, testOrder);
                order.typedSignature = "0x" + '1b'.repeat(65) + "00";
                expect(solo.canonicalOrders.orderHasValidSignature(order)).toBe(false);
                return [2 /*return*/];
            });
        }); });
    });
    describe('Signing CancelOrders', function () {
        it('Succeeds for eth.sign', function () { return __awaiter(_this, void 0, void 0, function () {
            var order, cancelSig;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        order = __assign({}, testOrder);
                        return [4 /*yield*/, solo.canonicalOrders.signCancelOrder(order, types_1.SigningMethod.Hash)];
                    case 1:
                        cancelSig = _a.sent();
                        expect(solo.canonicalOrders.cancelOrderHasValidSignature(order, cancelSig)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for eth_signTypedData', function () { return __awaiter(_this, void 0, void 0, function () {
            var order, cancelSig;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        order = __assign({}, testOrder);
                        return [4 /*yield*/, solo.canonicalOrders.signCancelOrder(order, types_1.SigningMethod.TypedData)];
                    case 1:
                        cancelSig = _a.sent();
                        expect(solo.canonicalOrders.cancelOrderHasValidSignature(order, cancelSig)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Recognizes a bad signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var order, cancelSig;
            return __generator(this, function (_a) {
                order = __assign({}, testOrder);
                cancelSig = "0x" + '1b'.repeat(65) + "00";
                expect(solo.canonicalOrders.cancelOrderHasValidSignature(order, cancelSig)).toBe(false);
                return [2 /*return*/];
            });
        }); });
    });
    describe('shutDown', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        _a = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 1:
                        _a.apply(void 0, [_c.sent()]).toBe(true);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.canonicalOrders.methods.shutDown(), { from: admin })];
                    case 2:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 3:
                        _b.apply(void 0, [_c.sent()]).toBe(false);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds when it is already shutDown', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.canonicalOrders.methods.shutDown(), { from: admin })];
                    case 1:
                        _c.sent();
                        _a = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 2:
                        _a.apply(void 0, [_c.sent()]).toBe(false);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.canonicalOrders.methods.shutDown(), { from: admin })];
                    case 3:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 4:
                        _b.apply(void 0, [_c.sent()]).toBe(false);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.canonicalOrders.methods.shutDown(), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('startUp', function () {
        it('Succeeds after being shutDown', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.canonicalOrders.methods.shutDown(), { from: admin })];
                    case 1:
                        _c.sent();
                        _a = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 2:
                        _a.apply(void 0, [_c.sent()]).toBe(false);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.canonicalOrders.methods.startUp(), { from: admin })];
                    case 3:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 4:
                        _b.apply(void 0, [_c.sent()]).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds when it is already operational', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        _a = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 1:
                        _a.apply(void 0, [_c.sent()]).toBe(true);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.canonicalOrders.methods.startUp(), { from: admin })];
                    case 2:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.canonicalOrders.isOperational()];
                    case 3:
                        _b.apply(void 0, [_c.sent()]).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.canonicalOrders.methods.startUp(), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('setTakerAddress', function () {
        it('Succeeds for owner', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        _a = expect;
                        return [4 /*yield*/, solo.canonicalOrders.getTakerAddress()];
                    case 1:
                        _a.apply(void 0, [_c.sent()]).toBe(admin);
                        return [4 /*yield*/, solo.canonicalOrders.setTakerAddress(rando, { from: admin })];
                    case 2:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.canonicalOrders.getTakerAddress()];
                    case 3:
                        _b.apply(void 0, [_c.sent()]).toBe(rando);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.canonicalOrders.setTakerAddress(rando, { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getTradeCost', function () {
        it('Succeeds for makerAmount specified', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(testOrder, {})];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultAmount)];
                    case 3:
                        _a.sent();
                        console.log("\tCanonicalOrder Trade gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for takerAmount specified', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { inputMarketId: quoteMarket, outputMarketId: baseMarket, amount: {
                                denomination: types_1.AmountDenomination.Wei,
                                reference: types_1.AmountReference.Delta,
                                value: defaultQuoteAmount.negated(),
                            } })).commit({ from: defaultTakerAddress })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultAmount)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for zero expiry', function () { return __awaiter(_this, void 0, void 0, function () {
            var testOrderNoExpiry;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getModifiedTestOrder({
                            expiration: Constants_1.INTEGERS.ZERO,
                        })];
                    case 1:
                        testOrderNoExpiry = _a.sent();
                        return [4 /*yield*/, fillOrder(testOrderNoExpiry, {})];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrderNoExpiry, defaultAmount)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for pre-approved order', function () { return __awaiter(_this, void 0, void 0, function () {
            var testOrderNoSig;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        testOrderNoSig = __assign({}, testOrder);
                        delete testOrderNoSig.typedSignature;
                        // approve order
                        return [4 /*yield*/, solo.canonicalOrders.approveOrder(testOrderNoSig, { from: testOrder.makerAccountOwner })];
                    case 1:
                        // approve order
                        _a.sent();
                        // verify okay
                        return [4 /*yield*/, fillOrder(testOrderNoSig, {})];
                    case 2:
                        // verify okay
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrderNoSig, defaultAmount)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-Solo caller', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.canonicalOrders.methods.getTradeCost(baseMarket.toFixed(0), quoteMarket.toFixed(0), { owner: defaultMakerAddress, number: defaultMakerNumber.toFixed(0) }, { owner: defaultTakerAddress, number: defaultTakerNumber.toFixed(0) }, { sign: false, value: '0' }, { sign: false, value: '0' }, { sign: false, value: '0' }, []), { from: rando }), 'OnlySolo: Only Solo can call function')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if shutDown', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.canonicalOrders.methods.shutDown(), { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, {}), 'CanonicalOrders: Contract is not operational')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for inputWei of zero', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, { amount: Constants_1.INTEGERS.ZERO }), 'CanonicalOrders: InputWei is zero')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for inputMarket mismatch', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { inputMarketId: incorrectMarket })).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Market mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for outputMarket mismatch', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { outputMarketId: incorrectMarket })).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Market mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for switching makerMarket and takerMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { inputMarketId: quoteMarket, outputMarketId: baseMarket })).commit({ from: defaultTakerAddress }), 'CanonicalOrders: InputWei sign mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails to overfill order for output amount', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { inputMarketId: quoteMarket, outputMarketId: baseMarket, amount: {
                                denomination: types_1.AmountDenomination.Wei,
                                reference: types_1.AmountReference.Delta,
                                value: defaultQuoteAmount.times(-1.01),
                            } })).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Cannot overfill order')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails to overfill order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, { amount: testOrder.amount.plus(5) }), 'CanonicalOrders: Cannot overfill order')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, {}), 'CanonicalOrders: Order canceled')];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for expired order', function () { return __awaiter(_this, void 0, void 0, function () {
            var testOrderExpired;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getModifiedTestOrder({
                            expiration: Constants_1.INTEGERS.ONE,
                        })];
                    case 1:
                        testOrderExpired = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrderExpired, {}), 'CanonicalOrders: Order expired')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for incorrect taker', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, { taker: rando }), 'CanonicalOrders: Order taker mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for incorrect maker account', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { otherAccountOwner: defaultTakerAddress })).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Order maker account mismatch')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { otherAccountId: defaultMakerNumber.plus(1) })).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Order maker account mismatch')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for invalid signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var invalidSignature1, invalidSignature2, testOrderInvalidSig1, testOrderInvalidSig2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        invalidSignature1 = "0x" + '00'.repeat(65) + "05";
                        invalidSignature2 = "0x" + '00'.repeat(65) + "01";
                        testOrderInvalidSig1 = __assign({}, testOrder, { typedSignature: invalidSignature1 });
                        testOrderInvalidSig2 = __assign({}, testOrder, { typedSignature: invalidSignature2 });
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrderInvalidSig1, {}), 'TypedSignature: Invalid signature type')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrderInvalidSig2, {}), 'CanonicalOrders: Order invalid signature')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for no signature data', function () { return __awaiter(_this, void 0, void 0, function () {
            var testOrderShortSig;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        testOrderShortSig = __assign({}, testOrder, { typedSignature: '0x' });
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrderShortSig, {}), 'CanonicalOrders: Cannot parse signature from data')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for bad data length', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(__assign({}, testOrder, { typedSignature: '0x0000' }), {}), 'CanonicalOrders: Cannot parse order from data')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(__assign({}, testOrder, { typedSignature: "0x" + '00'.repeat(100) }), {}), 'CanonicalOrders: Cannot parse order from data')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for bad order data', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, orderToTradeData(testOrder), { data: [[255], [255]] })).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Cannot parse order from data')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('constructor', function () {
        it('Sets constants correctly', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, domainHash, soloMarginAddress, expectedDomainHash;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.call(solo.contracts.canonicalOrders.methods.EIP712_DOMAIN_HASH()),
                            solo.contracts.call(solo.contracts.canonicalOrders.methods.SOLO_MARGIN()),
                        ])];
                    case 1:
                        _a = _b.sent(), domainHash = _a[0], soloMarginAddress = _a[1];
                        expectedDomainHash = solo.canonicalOrders.getDomainHash();
                        expect(domainHash).toEqual(expectedDomainHash);
                        expect(soloMarginAddress).toEqual(solo.contracts.soloMargin.options.address);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('approveOrder', function () {
        it('Succeeds for null order', function () { return __awaiter(_this, void 0, void 0, function () {
            var approver, txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        approver = testOrder.makerAccountOwner;
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Null)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.canonicalOrders.approveOrder(testOrder, { from: approver })];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogCanonicalOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.canonicalOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.baseMarket).toEqual(testOrder.baseMarket);
                        expect(log.args.quoteMarket).toEqual(testOrder.quoteMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for approved order', function () { return __awaiter(_this, void 0, void 0, function () {
            var approver, txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        approver = testOrder.makerAccountOwner;
                        return [4 /*yield*/, solo.canonicalOrders.approveOrder(testOrder, { from: approver })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.canonicalOrders.approveOrder(testOrder, { from: approver })];
                    case 3:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 4:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogCanonicalOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.canonicalOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.baseMarket).toEqual(testOrder.baseMarket);
                        expect(log.args.quoteMarket).toEqual(testOrder.quoteMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.canonicalOrders.approveOrder(testOrder, { from: testOrder.makerAccountOwner }), 'CanonicalOrders: Cannot approve canceled order')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('cancelOrder', function () {
        it('Succeeds for null order', function () { return __awaiter(_this, void 0, void 0, function () {
            var canceler, txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        canceler = testOrder.makerAccountOwner;
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Null)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: canceler })];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogCanonicalOrderCanceled');
                        expect(log.args.orderHash).toEqual(solo.canonicalOrders.getOrderHash(testOrder));
                        expect(log.args.canceler).toEqual(canceler);
                        expect(log.args.baseMarket).toEqual(testOrder.baseMarket);
                        expect(log.args.quoteMarket).toEqual(testOrder.quoteMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for approved order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.canonicalOrders.approveOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-maker', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.canonicalOrders.cancelOrder(testOrder, { from: rando }), 'CanonicalOrders: Canceler must be maker')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('callFunction: bad data', function () {
        it('Fails for bad callFunction type', function () { return __awaiter(_this, void 0, void 0, function () {
            var badType;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        badType = new bignumber_js_1.default(2);
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().call({
                                primaryAccountOwner: defaultTakerAddress,
                                primaryAccountId: defaultTakerNumber,
                                callee: solo.contracts.canonicalOrders.options.address,
                                data: BytesHelper_1.toBytes(badType, solo.canonicalOrders.getOrderHash(testOrder)),
                            }).commit({ from: defaultTakerAddress }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for too-short data', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().call({
                            primaryAccountOwner: defaultTakerAddress,
                            primaryAccountId: defaultTakerNumber,
                            callee: solo.contracts.canonicalOrders.options.address,
                            data: BytesHelper_1.toBytes(solo.canonicalOrders.getOrderHash(testOrder)),
                        }).commit({ from: defaultTakerAddress }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('callFunction: approve', function () {
        function approveTestOrder(from) {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    return [2 /*return*/, solo.operation.initiate().approveCanonicalOrder({
                            primaryAccountOwner: from || testOrder.makerAccountOwner,
                            primaryAccountId: testOrder.makerAccountNumber,
                            order: testOrder,
                        }).commit({ from: from || testOrder.makerAccountOwner })];
                });
            });
        }
        it('Fails for non-Solo caller', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.canonicalOrders.methods.callFunction(Constants_1.ADDRESSES.ZERO, {
                            owner: testOrder.makerAccountOwner,
                            number: testOrder.makerAccountNumber.toFixed(0),
                        }, []), { from: rando }), 'OnlySolo: Only Solo can call function')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for null order', function () { return __awaiter(_this, void 0, void 0, function () {
            var approver, txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        approver = testOrder.makerAccountOwner;
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Null)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, approveTestOrder()];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(4);
                        log = logs[2];
                        expect(log.name).toEqual('LogCanonicalOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.canonicalOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.baseMarket).toEqual(testOrder.baseMarket);
                        expect(log.args.quoteMarket).toEqual(testOrder.quoteMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for approved order', function () { return __awaiter(_this, void 0, void 0, function () {
            var approver, txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        approver = testOrder.makerAccountOwner;
                        return [4 /*yield*/, approveTestOrder()];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, approveTestOrder()];
                    case 3:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 4:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(4);
                        log = logs[2];
                        expect(log.name).toEqual('LogCanonicalOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.canonicalOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.baseMarket).toEqual(testOrder.baseMarket);
                        expect(log.args.quoteMarket).toEqual(testOrder.quoteMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(approveTestOrder(), 'CanonicalOrders: Cannot approve canceled order')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-maker', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(approveTestOrder(rando), 'CanonicalOrders: Approver must be maker')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('callFunction: cancel', function () {
        function cancelTestOrder(from) {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    return [2 /*return*/, solo.operation.initiate().cancelCanonicalOrder({
                            primaryAccountOwner: from || testOrder.makerAccountOwner,
                            primaryAccountId: testOrder.makerAccountNumber,
                            order: testOrder,
                        }).commit({ from: from || testOrder.makerAccountOwner })];
                });
            });
        }
        it('Fails for non-Solo caller', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.canonicalOrders.methods.callFunction(Constants_1.ADDRESSES.ZERO, {
                            owner: testOrder.makerAccountOwner,
                            number: testOrder.makerAccountNumber.toFixed(0),
                        }, []), { from: rando }), 'OnlySolo: Only Solo can call function')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for null order', function () { return __awaiter(_this, void 0, void 0, function () {
            var canceler, txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        canceler = testOrder.makerAccountOwner;
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Null)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, cancelTestOrder()];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(4);
                        log = logs[2];
                        expect(log.name).toEqual('LogCanonicalOrderCanceled');
                        expect(log.args.orderHash).toEqual(solo.canonicalOrders.getOrderHash(testOrder));
                        expect(log.args.canceler).toEqual(canceler);
                        expect(log.args.baseMarket).toEqual(testOrder.baseMarket);
                        expect(log.args.quoteMarket).toEqual(testOrder.quoteMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for approved order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.canonicalOrders.approveOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, cancelTestOrder()];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.canonicalOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, cancelTestOrder()];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-maker', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(cancelTestOrder(rando), 'CanonicalOrders: Canceler must be maker')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('', function () {
        function getCurrentPrice() {
            return __awaiter(this, void 0, void 0, function () {
                var _a, basePrice, quotePrice;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getMarketPrice(baseMarket),
                                solo.getters.getMarketPrice(quoteMarket),
                            ])];
                        case 1:
                            _a = _b.sent(), basePrice = _a[0], quotePrice = _a[1];
                            return [2 /*return*/, basePrice.div(quotePrice)];
                    }
                });
            });
        }
        it('Succeeds for met triggerPrice (buy)', function () { return __awaiter(_this, void 0, void 0, function () {
            var triggerPrice, order, txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getCurrentPrice()];
                    case 1:
                        triggerPrice = _a.sent();
                        return [4 /*yield*/, getModifiedTestOrder({ triggerPrice: triggerPrice })];
                    case 2:
                        order = _a.sent();
                        return [4 /*yield*/, fillOrder(order, {})];
                    case 3:
                        txResult = _a.sent();
                        console.log("\tCanonicalOrder Trade (w/ triggerPrice) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for unmet triggerPrice (buy)', function () { return __awaiter(_this, void 0, void 0, function () {
            var triggerPrice, order;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getCurrentPrice()];
                    case 1:
                        triggerPrice = (_a.sent()).plus(MINIMAL_PRICE_INCREMENT);
                        return [4 /*yield*/, getModifiedTestOrder({ triggerPrice: triggerPrice })];
                    case 2:
                        order = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(order, {}), 'CanonicalOrders: Order triggerPrice not triggered')];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for met triggerPrice (sell)', function () { return __awaiter(_this, void 0, void 0, function () {
            var triggerPrice, order;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, getCurrentPrice()];
                    case 2:
                        triggerPrice = _a.sent();
                        return [4 /*yield*/, getModifiedTestOrder({ triggerPrice: triggerPrice, isBuy: false })];
                    case 3:
                        order = _a.sent();
                        return [4 /*yield*/, fillOrder(order, {})];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for unmet triggerPrice (sell)', function () { return __awaiter(_this, void 0, void 0, function () {
            var triggerPrice, order;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getCurrentPrice()];
                    case 1:
                        triggerPrice = (_a.sent()).minus(MINIMAL_PRICE_INCREMENT);
                        return [4 /*yield*/, getModifiedTestOrder({ triggerPrice: triggerPrice, isBuy: false })];
                    case 2:
                        order = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(order, {}), 'CanonicalOrders: Order triggerPrice not triggered')];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('decreaseOnly', function () {
        describe('long position', function () {
            beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, setBalances(defaultAmount, defaultQuoteAmount, defaultAmount.div(2), defaultQuoteAmount.div(-8))];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for full close', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.operation.initiate().fillDecreaseOnlyCanonicalOrder(defaultTakerAddress, defaultTakerNumber, reverseDecreaseOrder, reverseDecreaseOrder.limitPrice, Constants_1.INTEGERS.ZERO).commit({ from: defaultTakerAddress })];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, expectBalances(defaultAmount.times(9).div(8), defaultQuoteAmount.times(7).div(8), defaultAmount.times(3).div(8), Constants_1.INTEGERS.ZERO)];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for decreasing', function () { return __awaiter(_this, void 0, void 0, function () {
                var fillOptions;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            fillOptions = { amount: defaultAmount.div(16) };
                            return [4 /*yield*/, fillOrder(reverseDecreaseOrder, fillOptions)];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, fillOrder(reverseDecreaseOrder, fillOptions)];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances(defaultAmount.times(9).div(8), defaultQuoteAmount.times(7).div(8), defaultAmount.times(3).div(8), Constants_1.INTEGERS.ZERO)];
                        case 3:
                            _a.sent();
                            // cannot go past zero
                            return [4 /*yield*/, Expect_1.expectThrow(fillOrder(reverseDecreaseOrder, fillOptions), 'CanonicalOrders: outputMarket not decreased')];
                        case 4:
                            // cannot go past zero
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when market crosses', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(reverseDecreaseOrder, { amount: defaultAmount.div(4) }), 'CanonicalOrders: outputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(fillOrder(reverseDecreaseOrder, { amount: defaultAmount.div(2) }), 'CanonicalOrders: outputMarket not decreased')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when increasing position', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(decreaseOrder, { amount: Constants_1.INTEGERS.ONE }), 'CanonicalOrders: inputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('short position', function () {
            beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, setBalances(defaultAmount, defaultQuoteAmount, defaultAmount.div(-8), defaultQuoteAmount.div(2))];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for full close', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, solo.operation.initiate().fillDecreaseOnlyCanonicalOrder(defaultTakerAddress, defaultTakerNumber, decreaseOrder, decreaseOrder.limitPrice, Constants_1.INTEGERS.ZERO).commit({ from: defaultTakerAddress })];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, expectBalances(defaultAmount.times(7).div(8), defaultQuoteAmount.times(9).div(8), Constants_1.INTEGERS.ZERO, defaultQuoteAmount.times(3).div(8))];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for decreasing', function () { return __awaiter(_this, void 0, void 0, function () {
                var fillOptions;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            fillOptions = { amount: defaultAmount.div(16) };
                            return [4 /*yield*/, fillOrder(decreaseOrder, fillOptions)];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, fillOrder(decreaseOrder, fillOptions)];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances(defaultAmount.times(7).div(8), defaultQuoteAmount.times(9).div(8), Constants_1.INTEGERS.ZERO, defaultQuoteAmount.times(3).div(8))];
                        case 3:
                            _a.sent();
                            // cannot go past zero
                            return [4 /*yield*/, Expect_1.expectThrow(fillOrder(decreaseOrder, fillOptions), 'CanonicalOrders: inputMarket not decreased')];
                        case 4:
                            // cannot go past zero
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when market crosses', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(decreaseOrder, { amount: defaultAmount.div(4) }), 'CanonicalOrders: inputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(fillOrder(decreaseOrder, { amount: defaultAmount.div(2) }), 'CanonicalOrders: inputMarket not decreased')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when increasing position', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(reverseDecreaseOrder, { amount: Constants_1.INTEGERS.ONE }), 'CanonicalOrders: inputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('zero position', function () {
            beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, setBalances(defaultAmount, defaultQuoteAmount, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO)];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when position was originally zero', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(decreaseOrder, { amount: Constants_1.INTEGERS.ONE }), 'CanonicalOrders: inputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
    });
    describe('prices', function () {
        it('Cannot violate limitPrice (buy)', function () { return __awaiter(_this, void 0, void 0, function () {
            var buyOrder;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        buyOrder = testOrder;
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(buyOrder, { price: buyOrder.limitPrice.plus(MINIMAL_PRICE_INCREMENT) }), 'CanonicalOrders: Fill invalid price')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Cannot violate limitPrice (sell)', function () { return __awaiter(_this, void 0, void 0, function () {
            var sellOrder;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getModifiedTestOrder({ isBuy: false })];
                    case 1:
                        sellOrder = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(sellOrder, { price: sellOrder.limitPrice.minus(MINIMAL_PRICE_INCREMENT) }), 'CanonicalOrders: Fill invalid price')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Can buy at reduced price', function () { return __awaiter(_this, void 0, void 0, function () {
            var buyOrder;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        buyOrder = testOrder;
                        return [4 /*yield*/, fillOrder(buyOrder, { price: buyOrder.limitPrice.div(2) })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount.div(2), defaultAmount, defaultQuoteAmount.div(2))];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Can sell at increased price', function () { return __awaiter(_this, void 0, void 0, function () {
            var sellOrder;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount.times(2), defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, getModifiedTestOrder({ isBuy: false })];
                    case 2:
                        sellOrder = _a.sent();
                        return [4 /*yield*/, fillOrder(sellOrder, { price: sellOrder.limitPrice.times(2) })];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(defaultAmount, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO, defaultQuoteAmount.times(2))];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('fees', function () {
        var INVALID_FEE_MESSAGE = 'CanonicalOrders: Fill invalid fee';
        it('Cannot violate fees', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, { fee: testOrder.limitFee.plus(MINIMAL_FEE_INCREMENT) }), INVALID_FEE_MESSAGE)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(noFeeOrder, { fee: Constants_1.INTEGERS.ONE }), INVALID_FEE_MESSAGE)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(negativeFeeOrder, { fee: Constants_1.INTEGERS.ZERO }), INVALID_FEE_MESSAGE)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(negativeFeeOrder, { fee: negativeFeeOrder.limitFee.plus(MINIMAL_FEE_INCREMENT) }), INVALID_FEE_MESSAGE)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Can take zero fee for a zero fee order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(noFeeOrder, { fee: Constants_1.INTEGERS.ZERO })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Can take negative fee for a zero fee order', function () { return __awaiter(_this, void 0, void 0, function () {
            var feeAmount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(noFeeOrder, { fee: testOrder.limitFee.negated() })];
                    case 1:
                        _a.sent();
                        feeAmount = defaultQuoteAmount.times(defaultLimitFee);
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount.minus(feeAmount), defaultAmount, feeAmount)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Can take a satisfying negative fee', function () { return __awaiter(_this, void 0, void 0, function () {
            var feeAmount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(negativeFeeOrder, { fee: negativeFeeOrder.limitFee })];
                    case 1:
                        _a.sent();
                        feeAmount = defaultQuoteAmount.times(defaultLimitFee);
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount.minus(feeAmount), defaultAmount, feeAmount)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Can take an extra-negative fee', function () { return __awaiter(_this, void 0, void 0, function () {
            var feeAmount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(negativeFeeOrder, { fee: negativeFeeOrder.limitFee.times(2) })];
                    case 1:
                        _a.sent();
                        feeAmount = defaultQuoteAmount.times(defaultLimitFee).times(2);
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount.minus(feeAmount), defaultAmount, feeAmount)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Positive fees work properly for buys', function () { return __awaiter(_this, void 0, void 0, function () {
            var feeAmount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(testOrder, { fee: testOrder.limitFee })];
                    case 1:
                        _a.sent();
                        feeAmount = defaultQuoteAmount.times(defaultLimitFee);
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount.plus(feeAmount), defaultAmount, feeAmount.negated())];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Zero fees work properly for buys', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(testOrder, { fee: Constants_1.INTEGERS.ZERO })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Negative fees work properly for buys', function () { return __awaiter(_this, void 0, void 0, function () {
            var feeAmount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillOrder(testOrder, { fee: testOrder.limitFee.negated() })];
                    case 1:
                        _a.sent();
                        feeAmount = defaultQuoteAmount.times(defaultLimitFee);
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount.minus(feeAmount), defaultAmount, feeAmount)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Positive fees work properly for sells', function () { return __awaiter(_this, void 0, void 0, function () {
            var feeAmount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, fillOrder(sellOrder, { fee: sellOrder.limitFee })];
                    case 2:
                        _a.sent();
                        feeAmount = defaultQuoteAmount.times(defaultLimitFee);
                        return [4 /*yield*/, expectBalances(defaultAmount, feeAmount, Constants_1.INTEGERS.ZERO, defaultQuoteAmount.minus(feeAmount))];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Zero fees work properly for sells', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, fillOrder(sellOrder, { fee: Constants_1.INTEGERS.ZERO })];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(defaultAmount, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO, defaultQuoteAmount)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Negative fees work properly for sells', function () { return __awaiter(_this, void 0, void 0, function () {
            var feeAmount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, fillOrder(sellOrder, { fee: sellOrder.limitFee.negated() })];
                    case 2:
                        _a.sent();
                        feeAmount = defaultQuoteAmount.times(defaultLimitFee);
                        return [4 /*yield*/, expectBalances(defaultAmount, feeAmount.negated(), Constants_1.INTEGERS.ZERO, defaultQuoteAmount.plus(feeAmount))];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('loading fillArgs', function () {
        it('Succeeds in loading in a price', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation.initiate().setCanonicalOrderFillArgs(defaultTakerAddress, defaultTakerNumber, testOrder.limitPrice, Constants_1.INTEGERS.ZERO).fillCanonicalOrder(defaultTakerAddress, defaultTakerNumber, testOrder, testOrder.amount, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO).commit({ from: defaultTakerAddress })];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        console.log("\tCanonicalOrder Trade (w/ setting fillArgs) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Cannot load in a null price', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, { price: Constants_1.INTEGERS.ZERO }), 'CanonicalOrders: FillArgs loaded price is zero')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Cannot load in a price past the limit', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().setCanonicalOrderFillArgs(defaultTakerAddress, defaultTakerNumber, testOrder.limitPrice.times(2), Constants_1.INTEGERS.ZERO).fillCanonicalOrder(defaultTakerAddress, defaultTakerNumber, testOrder, testOrder.amount, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Fill invalid price')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Cannot load in a fee past the limit', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().setCanonicalOrderFillArgs(defaultTakerAddress, defaultTakerNumber, testOrder.limitPrice, testOrder.limitFee.times(2)).fillCanonicalOrder(defaultTakerAddress, defaultTakerNumber, testOrder, testOrder.amount, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO).commit({ from: defaultTakerAddress }), 'CanonicalOrders: Fill invalid fee')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('integration', function () {
        it('Fills an order multiple times up to the limit', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: 
                    // fill once
                    return [4 /*yield*/, fillOrder(testOrder, { amount: defaultAmount.div(2) })];
                    case 1:
                        // fill once
                        _a.sent();
                        return [4 /*yield*/, expectBalances(defaultAmount.div(2), defaultQuoteAmount.div(2), defaultAmount.div(2), defaultQuoteAmount.div(2))];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultAmount.div(2))];
                    case 3:
                        _a.sent();
                        // fill twice
                        return [4 /*yield*/, fillOrder(testOrder, { amount: defaultAmount.div(2) })];
                    case 4:
                        // fill twice
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultQuoteAmount, defaultAmount, Constants_1.INTEGERS.ZERO)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultAmount)];
                    case 6:
                        _a.sent();
                        // fail a third time
                        return [4 /*yield*/, Expect_1.expectThrow(fillOrder(testOrder, { amount: defaultAmount.div(2) }), 'CanonicalOrders: Cannot overfill order')];
                    case 7:
                        // fail a third time
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for logs', function () { return __awaiter(_this, void 0, void 0, function () {
            var order, orderHash, txResult1, logs1, logOrderTaken1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getModifiedTestOrder({ triggerPrice: Constants_1.INTEGERS.ONE })];
                    case 1:
                        order = _a.sent();
                        orderHash = solo.canonicalOrders.getOrderHash(order);
                        return [4 /*yield*/, fillOrder(order, { amount: defaultAmount.div(2) })];
                    case 2:
                        txResult1 = _a.sent();
                        return [4 /*yield*/, expectFilledAmount(order, defaultAmount.div(2))];
                    case 3:
                        _a.sent();
                        logs1 = solo.logs.parseLogs(txResult1);
                        expect(logs1.length).toEqual(5);
                        logOrderTaken1 = logs1[3];
                        expect(logOrderTaken1.name).toEqual('LogCanonicalOrderFilled');
                        expect(logOrderTaken1.args.orderHash).toEqual(orderHash);
                        expect(logOrderTaken1.args.orderMaker).toEqual(order.makerAccountOwner);
                        expect(logOrderTaken1.args.fillAmount).toEqual(defaultAmount.div(2));
                        expect(logOrderTaken1.args.isBuy).toEqual(true);
                        expect(logOrderTaken1.args.isDecreaseOnly).toEqual(false);
                        expect(logOrderTaken1.args.isNegativeLimitFee).toEqual(false);
                        expect(logOrderTaken1.args.triggerPrice).toEqual(new bignumber_js_1.default('1e18'));
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getOrderStates', function () {
        it('Succeeds for multiple orders', function () { return __awaiter(_this, void 0, void 0, function () {
            var canceler, approver, testOrderCancel, testOrderApprove, states1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        canceler = accounts[0];
                        approver = accounts[1];
                        return [4 /*yield*/, getModifiedTestOrder({ makerAccountOwner: canceler })];
                    case 1:
                        testOrderCancel = _a.sent();
                        return [4 /*yield*/, getModifiedTestOrder({ makerAccountOwner: approver })];
                    case 2:
                        testOrderApprove = _a.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.canonicalOrders.approveOrder(testOrderApprove, { from: approver }),
                                solo.canonicalOrders.cancelOrder(testOrderCancel, { from: canceler }),
                                fillOrder(testOrder, { amount: defaultAmount.div(2) }),
                            ])];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.canonicalOrders.getOrderStates([
                                testOrder,
                                testOrderCancel,
                                testOrderApprove,
                            ])];
                    case 4:
                        states1 = _a.sent();
                        expect(states1).toEqual([
                            { status: types_1.LimitOrderStatus.Null, totalFilledAmount: testOrder.amount.div(2) },
                            { status: types_1.LimitOrderStatus.Canceled, totalFilledAmount: Constants_1.INTEGERS.ZERO },
                            { status: types_1.LimitOrderStatus.Approved, totalFilledAmount: Constants_1.INTEGERS.ZERO },
                        ]);
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
// ============ Helper Functions ============
function fillOrder(order, _a) {
    var _b = _a.taker, taker = _b === void 0 ? defaultTakerAddress : _b, _c = _a.takerNumber, takerNumber = _c === void 0 ? defaultTakerNumber : _c, _d = _a.amount, amount = _d === void 0 ? order.amount : _d, _e = _a.price, price = _e === void 0 ? order.limitPrice : _e, _f = _a.fee, fee = _f === void 0 ? Constants_1.INTEGERS.ZERO : _f;
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_g) {
            return [2 /*return*/, solo.operation.initiate().fillCanonicalOrder(taker, takerNumber, order, amount, price, fee).commit({ from: taker })];
        });
    });
}
function orderToTradeData(order, price, fee) {
    if (price === void 0) { price = order.limitPrice; }
    if (fee === void 0) { fee = Constants_1.INTEGERS.ZERO; }
    return {
        primaryAccountOwner: defaultTakerAddress,
        primaryAccountId: defaultTakerNumber,
        autoTrader: solo.contracts.canonicalOrders.options.address,
        inputMarketId: order.baseMarket,
        outputMarketId: order.quoteMarket,
        otherAccountOwner: order.makerAccountOwner,
        otherAccountId: order.makerAccountNumber,
        amount: {
            denomination: types_1.AmountDenomination.Wei,
            reference: types_1.AmountReference.Delta,
            value: order.isBuy ? order.amount : order.amount.negated(),
        },
        data: BytesHelper_1.hexStringToBytes(solo.canonicalOrders.orderToBytes(order, price, fee)),
    };
}
function setBalances(takerBase, takerQuote, makerBase, makerQuote) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountBalance(defaultTakerAddress, defaultTakerNumber, baseMarket, takerBase),
                        solo.testing.setAccountBalance(defaultTakerAddress, defaultTakerNumber, quoteMarket, takerQuote),
                        solo.testing.setAccountBalance(defaultMakerAddress, defaultMakerNumber, baseMarket, makerBase),
                        solo.testing.setAccountBalance(defaultMakerAddress, defaultMakerNumber, quoteMarket, makerQuote),
                    ])];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function expectBalances(takerBaseExpected, takerQuoteExpected, makerBaseExpected, makerQuoteExpected) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, takerBaseWei, takerQuoteWei, makerBaseWei, makerQuoteWei;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.getters.getAccountWei(defaultTakerAddress, defaultTakerNumber, baseMarket),
                        solo.getters.getAccountWei(defaultTakerAddress, defaultTakerNumber, quoteMarket),
                        solo.getters.getAccountWei(defaultMakerAddress, defaultMakerNumber, baseMarket),
                        solo.getters.getAccountWei(defaultMakerAddress, defaultMakerNumber, quoteMarket),
                    ])];
                case 1:
                    _a = _b.sent(), takerBaseWei = _a[0], takerQuoteWei = _a[1], makerBaseWei = _a[2], makerQuoteWei = _a[3];
                    expect(takerBaseWei).toEqual(takerBaseExpected);
                    expect(takerQuoteWei).toEqual(takerQuoteExpected);
                    expect(makerBaseWei).toEqual(makerBaseExpected);
                    expect(makerQuoteWei).toEqual(makerQuoteExpected);
                    return [2 /*return*/];
            }
        });
    });
}
function expectFilledAmount(order, expectedFilledAmount) {
    return __awaiter(this, void 0, void 0, function () {
        var states;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.canonicalOrders.getOrderStates([order])];
                case 1:
                    states = _a.sent();
                    expect(states[0].totalFilledAmount).toEqual(expectedFilledAmount);
                    return [2 /*return*/];
            }
        });
    });
}
function expectStatus(order, expectedStatus) {
    return __awaiter(this, void 0, void 0, function () {
        var states;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.canonicalOrders.getOrderStates([order])];
                case 1:
                    states = _a.sent();
                    expect(states[0].status).toEqual(expectedStatus);
                    return [2 /*return*/];
            }
        });
    });
}
function getModifiedTestOrder(params) {
    return __awaiter(this, void 0, void 0, function () {
        var result, _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    result = __assign({}, testOrder, params);
                    _a = result;
                    return [4 /*yield*/, solo.canonicalOrders.signOrder(result, types_1.SigningMethod.TypedData)];
                case 1:
                    _a.typedSignature = _b.sent();
                    return [2 /*return*/, result];
            }
        });
    });
}
//# sourceMappingURL=CanonicalOrders.test.js.map