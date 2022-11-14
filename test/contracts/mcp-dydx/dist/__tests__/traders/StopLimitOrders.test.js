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
var defaultMakerMarket = new bignumber_js_1.default(0);
var defaultTakerMarket = new bignumber_js_1.default(1);
var incorrectMarket = new bignumber_js_1.default(2);
var defaultMakerNumber = new bignumber_js_1.default(111);
var defaultTakerNumber = new bignumber_js_1.default(222);
var defaultMakerAmount = new bignumber_js_1.default('1e10');
var defaultTakerAmount = new bignumber_js_1.default('2e10');
var admin;
var defaultMakerAddress;
var defaultTakerAddress;
var rando;
var testOrder;
var decreaseOrder;
var reverseDecreaseOrder;
describe('StopLimitOrders', function () {
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r, _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _b.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    admin = accounts[0];
                    defaultMakerAddress = accounts[6];
                    defaultTakerAddress = accounts[7];
                    rando = accounts[9];
                    testOrder = {
                        makerMarket: defaultMakerMarket,
                        takerMarket: defaultTakerMarket,
                        makerAmount: defaultMakerAmount,
                        takerAmount: defaultTakerAmount,
                        makerAccountOwner: defaultMakerAddress,
                        makerAccountNumber: defaultMakerNumber,
                        takerAccountOwner: defaultTakerAddress,
                        takerAccountNumber: defaultTakerNumber,
                        triggerPrice: Constants_1.INTEGERS.ZERO,
                        decreaseOnly: false,
                        expiration: Constants_1.INTEGERS.ONES_31,
                        salt: new bignumber_js_1.default(100),
                        typedSignature: null,
                    };
                    _a = testOrder;
                    return [4 /*yield*/, solo.stopLimitOrders.signOrder(testOrder, types_1.SigningMethod.TypedData)];
                case 2:
                    _a.typedSignature =
                        _b.sent();
                    return [4 /*yield*/, getModifiedTestOrder({ decreaseOnly: true })];
                case 3:
                    decreaseOrder = _b.sent();
                    return [4 /*yield*/, getModifiedTestOrder({
                            makerMarket: defaultTakerMarket,
                            takerMarket: defaultMakerMarket,
                            makerAmount: defaultTakerAmount,
                            takerAmount: defaultMakerAmount,
                            decreaseOnly: true,
                        })];
                case 4:
                    reverseDecreaseOrder = _b.sent();
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 5:
                    _b.sent();
                    // set balances
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 6:
                    // set balances
                    _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(testOrder.takerAccountOwner, testOrder.takerAccountNumber, defaultTakerMarket, defaultTakerAmount),
                            solo.testing.setAccountBalance(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultMakerMarket, defaultMakerAmount),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenA.getAddress(), new bignumber_js_1.default('1e18')),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenB.getAddress(), new bignumber_js_1.default('2e18')),
                        ])];
                case 7:
                    _b.sent();
                    return [4 /*yield*/, EVM_1.mineAvgBlock()];
                case 8:
                    _b.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 9:
                    snapshotId = _b.sent();
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
                        return [4 /*yield*/, solo.stopLimitOrders.signOrder(order, types_1.SigningMethod.Hash)];
                    case 1:
                        _a.typedSignature = _b.sent();
                        expect(solo.stopLimitOrders.orderHasValidSignature(order)).toBe(true);
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
                        return [4 /*yield*/, solo.stopLimitOrders.signOrder(order, types_1.SigningMethod.TypedData)];
                    case 1:
                        _a.typedSignature = _b.sent();
                        expect(solo.stopLimitOrders.orderHasValidSignature(order)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Recognizes a bad signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var order;
            return __generator(this, function (_a) {
                order = __assign({}, testOrder);
                order.typedSignature = "0x" + '1b'.repeat(65) + "00";
                expect(solo.stopLimitOrders.orderHasValidSignature(order)).toBe(false);
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
                        return [4 /*yield*/, solo.stopLimitOrders.signCancelOrder(order, types_1.SigningMethod.Hash)];
                    case 1:
                        cancelSig = _a.sent();
                        expect(solo.stopLimitOrders.cancelOrderHasValidSignature(order, cancelSig)).toBe(true);
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
                        return [4 /*yield*/, solo.stopLimitOrders.signCancelOrder(order, types_1.SigningMethod.TypedData)];
                    case 1:
                        cancelSig = _a.sent();
                        expect(solo.stopLimitOrders.cancelOrderHasValidSignature(order, cancelSig)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Recognizes a bad signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var order, cancelSig;
            return __generator(this, function (_a) {
                order = __assign({}, testOrder);
                cancelSig = "0x" + '1b'.repeat(65) + "00";
                expect(solo.stopLimitOrders.cancelOrderHasValidSignature(order, cancelSig)).toBe(false);
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
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
                    case 1:
                        _a.apply(void 0, [_c.sent()]).toBe(true);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.stopLimitOrders.methods.shutDown(), { from: admin })];
                    case 2:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
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
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.stopLimitOrders.methods.shutDown(), { from: admin })];
                    case 1:
                        _c.sent();
                        _a = expect;
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
                    case 2:
                        _a.apply(void 0, [_c.sent()]).toBe(false);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.stopLimitOrders.methods.shutDown(), { from: admin })];
                    case 3:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
                    case 4:
                        _b.apply(void 0, [_c.sent()]).toBe(false);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.stopLimitOrders.methods.shutDown(), { from: rando }))];
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
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.stopLimitOrders.methods.shutDown(), { from: admin })];
                    case 1:
                        _c.sent();
                        _a = expect;
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
                    case 2:
                        _a.apply(void 0, [_c.sent()]).toBe(false);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.stopLimitOrders.methods.startUp(), { from: admin })];
                    case 3:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
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
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
                    case 1:
                        _a.apply(void 0, [_c.sent()]).toBe(true);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.stopLimitOrders.methods.startUp(), { from: admin })];
                    case 2:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.stopLimitOrders.isOperational()];
                    case 3:
                        _b.apply(void 0, [_c.sent()]).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.stopLimitOrders.methods.startUp(), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getTradeCost', function () {
        it('Succeeds for makerAmount specified', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillLimitOrder(testOrder, {
                            amount: defaultMakerAmount.times(-1),
                            denominatedInMakerAmount: true,
                        })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultMakerAmount, defaultTakerAmount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultMakerAmount)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for takerAmount specified', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, fillLimitOrder(testOrder, {
                            amount: defaultTakerAmount,
                            denominatedInMakerAmount: false,
                        })];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultMakerAmount, defaultTakerAmount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultMakerAmount)];
                    case 3:
                        _a.sent();
                        console.log("\tStopLimitOrder Trade gas used: " + txResult.gasUsed);
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
                        return [4 /*yield*/, fillLimitOrder(testOrderNoExpiry, {})];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultMakerAmount, defaultTakerAmount, Constants_1.INTEGERS.ZERO)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrderNoExpiry, defaultMakerAmount)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for no specific taker', function () { return __awaiter(_this, void 0, void 0, function () {
            var testOrderNoTaker;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getModifiedTestOrder({
                            takerAccountOwner: Constants_1.ADDRESSES.ZERO,
                            takerAccountNumber: Constants_1.INTEGERS.ZERO,
                        })];
                    case 1:
                        testOrderNoTaker = _a.sent();
                        return [4 /*yield*/, fillLimitOrder(testOrderNoTaker, {})];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultMakerAmount, defaultTakerAmount, Constants_1.INTEGERS.ZERO)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrderNoTaker, defaultMakerAmount)];
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
                    // approve order
                    return [4 /*yield*/, solo.stopLimitOrders.approveOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        // approve order
                        _a.sent();
                        testOrderNoSig = __assign({}, testOrder);
                        delete testOrderNoSig.typedSignature;
                        // verify okay
                        return [4 /*yield*/, solo.operation.initiate().fillPreApprovedStopLimitOrder(defaultTakerAddress, defaultTakerNumber, testOrderNoSig, defaultTakerAmount, false).commit({ from: defaultTakerAddress })];
                    case 2:
                        // verify okay
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultMakerAmount, defaultTakerAmount, Constants_1.INTEGERS.ZERO)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrderNoSig, defaultMakerAmount)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-Solo caller', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.stopLimitOrders.methods.getTradeCost(defaultMakerMarket.toFixed(0), defaultTakerMarket.toFixed(0), { owner: defaultMakerAddress, number: defaultMakerNumber.toFixed(0) }, { owner: defaultTakerAddress, number: defaultTakerNumber.toFixed(0) }, { sign: false, value: '0' }, { sign: false, value: '0' }, { sign: false, value: '0' }, []), { from: rando }), 'OnlySolo: Only Solo can call function')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if shutDown', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.stopLimitOrders.methods.shutDown(), { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, {}), 'StopLimitOrders: Contract is not operational')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for inputWei of zero', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, { amount: Constants_1.INTEGERS.ZERO }), 'StopLimitOrders: InputWei is zero')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for inputMarket mismatch', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, stopLimitOrderToTradeData(testOrder), { inputMarketId: incorrectMarket })).commit({ from: defaultTakerAddress }), 'StopLimitOrders: Market mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for outputMarket mismatch', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, stopLimitOrderToTradeData(testOrder), { outputMarketId: incorrectMarket })).commit({ from: defaultTakerAddress }), 'StopLimitOrders: Market mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for switching makerMarket and takerMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, stopLimitOrderToTradeData(testOrder), { inputMarketId: defaultMakerMarket, outputMarketId: defaultTakerMarket })).commit({ from: defaultTakerAddress }), 'StopLimitOrders: InputWei sign mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails to overfill order (makerAmount)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, {
                            amount: defaultMakerAmount.plus(5).times(-1),
                            denominatedInMakerAmount: true,
                        }), 'StopLimitOrders: Cannot overfill order')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails to overfill order (takerAmount)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, { amount: defaultTakerAmount.plus(5) }), 'StopLimitOrders: Cannot overfill order')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, {}), 'StopLimitOrders: Order canceled')];
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
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrderExpired, {}), 'StopLimitOrders: Order expired')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for incorrect taker account', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, { taker: rando }), 'StopLimitOrders: Order taker account mismatch')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, { takerNumber: defaultTakerNumber.plus(1) }), 'StopLimitOrders: Order taker account mismatch')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for incorrect maker account', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, stopLimitOrderToTradeData(testOrder), { otherAccountOwner: defaultTakerAddress })).commit({ from: defaultTakerAddress }), 'StopLimitOrders: Order maker account mismatch')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, stopLimitOrderToTradeData(testOrder), { otherAccountId: defaultMakerNumber.plus(1) })).commit({ from: defaultTakerAddress }), 'StopLimitOrders: Order maker account mismatch')];
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
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrderInvalidSig1, {}), 'TypedSignature: Invalid signature type')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrderInvalidSig2, {}), 'StopLimitOrders: Order invalid signature')];
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
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrderShortSig, {}), 'StopLimitOrders: Cannot parse signature from data')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for bad data length', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(__assign({}, testOrder, { typedSignature: '0x0000' }), {}), 'StopLimitOrders: Cannot parse order from data')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(__assign({}, testOrder, { typedSignature: "0x" + '00'.repeat(100) }), {}), 'StopLimitOrders: Cannot parse order from data')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for bad order data', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().trade(__assign({}, stopLimitOrderToTradeData(testOrder), { data: [[255], [255]] })).commit({ from: defaultTakerAddress }), 'StopLimitOrders: Cannot parse order from data')];
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
                            solo.contracts.call(solo.contracts.stopLimitOrders.methods.EIP712_DOMAIN_HASH()),
                            solo.contracts.call(solo.contracts.stopLimitOrders.methods.SOLO_MARGIN()),
                        ])];
                    case 1:
                        _a = _b.sent(), domainHash = _a[0], soloMarginAddress = _a[1];
                        expectedDomainHash = solo.stopLimitOrders.getDomainHash();
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
                        return [4 /*yield*/, solo.stopLimitOrders.approveOrder(testOrder, { from: approver })];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogStopLimitOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.stopLimitOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.makerMarket).toEqual(testOrder.makerMarket);
                        expect(log.args.takerMarket).toEqual(testOrder.takerMarket);
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
                        return [4 /*yield*/, solo.stopLimitOrders.approveOrder(testOrder, { from: approver })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.stopLimitOrders.approveOrder(testOrder, { from: approver })];
                    case 3:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 4:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogStopLimitOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.stopLimitOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.makerMarket).toEqual(testOrder.makerMarket);
                        expect(log.args.takerMarket).toEqual(testOrder.takerMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.stopLimitOrders.approveOrder(testOrder, { from: testOrder.makerAccountOwner }), 'StopLimitOrders: Cannot approve canceled order')];
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
                        return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: canceler })];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogStopLimitOrderCanceled');
                        expect(log.args.orderHash).toEqual(solo.stopLimitOrders.getOrderHash(testOrder));
                        expect(log.args.canceler).toEqual(canceler);
                        expect(log.args.makerMarket).toEqual(testOrder.makerMarket);
                        expect(log.args.takerMarket).toEqual(testOrder.takerMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for approved order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.stopLimitOrders.approveOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Approved)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
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
                    case 0: return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectStatus(testOrder, types_1.LimitOrderStatus.Canceled)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
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
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.stopLimitOrders.cancelOrder(testOrder, { from: rando }), 'StopLimitOrders: Canceler must be maker')];
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
                                primaryAccountOwner: testOrder.takerAccountOwner,
                                primaryAccountId: testOrder.takerAccountNumber,
                                callee: solo.contracts.stopLimitOrders.options.address,
                                data: BytesHelper_1.toBytes(badType, solo.stopLimitOrders.getOrderHash(testOrder)),
                            }).commit({ from: testOrder.takerAccountOwner }))];
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
                            primaryAccountOwner: testOrder.takerAccountOwner,
                            primaryAccountId: testOrder.takerAccountNumber,
                            callee: solo.contracts.stopLimitOrders.options.address,
                            data: BytesHelper_1.toBytes(solo.stopLimitOrders.getOrderHash(testOrder)),
                        }).commit({ from: testOrder.takerAccountOwner }), 'StopLimitOrders: Cannot parse CallFunctionData')];
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
                    return [2 /*return*/, solo.operation.initiate().approveStopLimitOrder({
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
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.stopLimitOrders.methods.callFunction(Constants_1.ADDRESSES.ZERO, {
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
                        expect(log.name).toEqual('LogStopLimitOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.stopLimitOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.makerMarket).toEqual(testOrder.makerMarket);
                        expect(log.args.takerMarket).toEqual(testOrder.takerMarket);
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
                        expect(log.name).toEqual('LogStopLimitOrderApproved');
                        expect(log.args.orderHash).toEqual(solo.stopLimitOrders.getOrderHash(testOrder));
                        expect(log.args.approver).toEqual(approver);
                        expect(log.args.makerMarket).toEqual(testOrder.makerMarket);
                        expect(log.args.takerMarket).toEqual(testOrder.takerMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for canceled order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(approveTestOrder(), 'StopLimitOrders: Cannot approve canceled order')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-maker', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(approveTestOrder(rando), 'StopLimitOrders: Approver must be maker')];
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
                    return [2 /*return*/, solo.operation.initiate().cancelStopLimitOrder({
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
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.stopLimitOrders.methods.callFunction(Constants_1.ADDRESSES.ZERO, {
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
                        expect(log.name).toEqual('LogStopLimitOrderCanceled');
                        expect(log.args.orderHash).toEqual(solo.stopLimitOrders.getOrderHash(testOrder));
                        expect(log.args.canceler).toEqual(canceler);
                        expect(log.args.makerMarket).toEqual(testOrder.makerMarket);
                        expect(log.args.takerMarket).toEqual(testOrder.takerMarket);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for approved order', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.stopLimitOrders.approveOrder(testOrder, { from: testOrder.makerAccountOwner })];
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
                    case 0: return [4 /*yield*/, solo.stopLimitOrders.cancelOrder(testOrder, { from: testOrder.makerAccountOwner })];
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
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(cancelTestOrder(rando), 'StopLimitOrders: Canceler must be maker')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('triggerPrice', function () {
        function getCurrentPrice() {
            return __awaiter(this, void 0, void 0, function () {
                var _a, makerPrice, takerPrice;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.getters.getMarketPrice(defaultMakerMarket),
                                solo.getters.getMarketPrice(defaultTakerMarket),
                            ])];
                        case 1:
                            _a = _b.sent(), makerPrice = _a[0], takerPrice = _a[1];
                            return [2 /*return*/, takerPrice.times('1e18').div(makerPrice)];
                    }
                });
            });
        }
        it('Succeeds for met triggerPrice', function () { return __awaiter(_this, void 0, void 0, function () {
            var triggerPrice, order, txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getCurrentPrice()];
                    case 1:
                        triggerPrice = (_a.sent()).integerValue(bignumber_js_1.default.ROUND_DOWN);
                        return [4 /*yield*/, getModifiedTestOrder({ triggerPrice: triggerPrice })];
                    case 2:
                        order = _a.sent();
                        return [4 /*yield*/, fillLimitOrder(order, { amount: defaultTakerAmount })];
                    case 3:
                        txResult = _a.sent();
                        console.log("\tStopLimitOrder Trade (w/ triggerPrice) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for unmet triggerPrice', function () { return __awaiter(_this, void 0, void 0, function () {
            var triggerPrice, order;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, getCurrentPrice()];
                    case 1:
                        triggerPrice = (_a.sent()).plus(1).integerValue(bignumber_js_1.default.ROUND_UP);
                        return [4 /*yield*/, getModifiedTestOrder({ triggerPrice: triggerPrice })];
                    case 2:
                        order = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(order, { amount: defaultTakerAmount }), 'StopLimitOrders: Order triggerPrice not triggered')];
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
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(testOrder.takerAccountOwner, testOrder.takerAccountNumber, defaultTakerMarket, defaultTakerAmount),
                                solo.testing.setAccountBalance(testOrder.takerAccountOwner, testOrder.takerAccountNumber, defaultMakerMarket, defaultMakerAmount),
                                solo.testing.setAccountBalance(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultMakerMarket, defaultMakerAmount.div(2)),
                                solo.testing.setAccountBalance(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultTakerMarket, defaultTakerAmount.div(-8)),
                            ])];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for output decreasing', function () { return __awaiter(_this, void 0, void 0, function () {
                var fillOptions;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            fillOptions = {
                                amount: defaultMakerAmount.div(-16),
                                denominatedInMakerAmount: true,
                            };
                            return [4 /*yield*/, fillLimitOrder(decreaseOrder, fillOptions)];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, fillLimitOrder(decreaseOrder, fillOptions)];
                        case 2:
                            _a.sent();
                            // cannot go past zero
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, fillOptions), 'StopLimitOrders: outputMarket not decreased')];
                        case 3:
                            // cannot go past zero
                            _a.sent();
                            return [4 /*yield*/, expectBalances(defaultTakerAmount.times(7).div(8), defaultMakerAmount.times(9).div(8), Constants_1.INTEGERS.ZERO, defaultMakerAmount.times(3).div(8))];
                        case 4:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for input decreasing', function () { return __awaiter(_this, void 0, void 0, function () {
                var fillOptions;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            fillOptions = {
                                amount: defaultTakerAmount.div(16),
                                denominatedInMakerAmount: false,
                            };
                            return [4 /*yield*/, fillLimitOrder(decreaseOrder, fillOptions)];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, fillLimitOrder(decreaseOrder, fillOptions)];
                        case 2:
                            _a.sent();
                            // cannot go past zero
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, fillOptions), 'StopLimitOrders: inputMarket not decreased')];
                        case 3:
                            // cannot go past zero
                            _a.sent();
                            return [4 /*yield*/, expectBalances(defaultTakerAmount.times(7).div(8), defaultMakerAmount.times(9).div(8), Constants_1.INTEGERS.ZERO, defaultMakerAmount.times(3).div(8))];
                        case 4:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when inputMarket crosses', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, {
                                amount: defaultMakerAmount.div(-4),
                                denominatedInMakerAmount: true,
                            }), 'StopLimitOrders: outputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, {
                                    amount: defaultMakerAmount.div(-2),
                                    denominatedInMakerAmount: true,
                                }), 'StopLimitOrders: outputMarket not decreased')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when outputMarket crosses', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, {
                                amount: defaultTakerAmount.div(4),
                                denominatedInMakerAmount: false,
                            }), 'StopLimitOrders: inputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, {
                                    amount: defaultTakerAmount.div(2),
                                    denominatedInMakerAmount: false,
                                }), 'StopLimitOrders: inputMarket not decreased')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when increasing position', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(reverseDecreaseOrder, {
                                amount: Constants_1.INTEGERS.ONE,
                                denominatedInMakerAmount: false,
                            }), 'StopLimitOrders: inputMarket not decreased')];
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
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(testOrder.takerAccountOwner, testOrder.takerAccountNumber, defaultTakerMarket, defaultMakerAmount),
                                solo.testing.setAccountBalance(testOrder.takerAccountOwner, testOrder.takerAccountNumber, defaultMakerMarket, defaultTakerAmount),
                                solo.testing.setAccountBalance(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultMakerMarket, defaultMakerAmount.div(-8)),
                                solo.testing.setAccountBalance(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultTakerMarket, defaultTakerAmount.div(2)),
                            ])];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for output decreasing', function () { return __awaiter(_this, void 0, void 0, function () {
                var fillOptions;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            fillOptions = {
                                amount: defaultTakerAmount.div(-16),
                                denominatedInMakerAmount: true,
                            };
                            return [4 /*yield*/, fillLimitOrder(reverseDecreaseOrder, fillOptions)];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, fillLimitOrder(reverseDecreaseOrder, fillOptions)];
                        case 2:
                            _a.sent();
                            // cannot go past zero
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(reverseDecreaseOrder, fillOptions), 'StopLimitOrders: outputMarket not decreased')];
                        case 3:
                            // cannot go past zero
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for input decreasing', function () { return __awaiter(_this, void 0, void 0, function () {
                var fillOptions;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            fillOptions = {
                                amount: defaultMakerAmount.div(16),
                                denominatedInMakerAmount: false,
                            };
                            return [4 /*yield*/, fillLimitOrder(reverseDecreaseOrder, fillOptions)];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, fillLimitOrder(reverseDecreaseOrder, fillOptions)];
                        case 2:
                            _a.sent();
                            // cannot go past zero
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(reverseDecreaseOrder, fillOptions), 'StopLimitOrders: inputMarket not decreased')];
                        case 3:
                            // cannot go past zero
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when inputMarket crosses', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(reverseDecreaseOrder, {
                                amount: defaultTakerAmount.div(-4),
                                denominatedInMakerAmount: true,
                            }), 'StopLimitOrders: outputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(reverseDecreaseOrder, {
                                    amount: defaultTakerAmount.div(-2),
                                    denominatedInMakerAmount: true,
                                }), 'StopLimitOrders: outputMarket not decreased')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when outputMarket crosses', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(reverseDecreaseOrder, {
                                amount: defaultMakerAmount.div(4),
                                denominatedInMakerAmount: false,
                            }), 'StopLimitOrders: inputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(reverseDecreaseOrder, {
                                    amount: defaultMakerAmount.div(2),
                                    denominatedInMakerAmount: false,
                                }), 'StopLimitOrders: inputMarket not decreased')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when increasing position', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, {
                                amount: Constants_1.INTEGERS.ONE,
                                denominatedInMakerAmount: false,
                            }), 'StopLimitOrders: inputMarket not decreased')];
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
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultMakerMarket, Constants_1.INTEGERS.ZERO),
                                solo.testing.setAccountBalance(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultTakerMarket, Constants_1.INTEGERS.ZERO),
                            ])];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails when position was originally zero', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(decreaseOrder, {
                                amount: Constants_1.INTEGERS.ONE,
                                denominatedInMakerAmount: false,
                            }), 'StopLimitOrders: inputMarket not decreased')];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
    });
    describe('integration', function () {
        it('Fills an order multiple times up to the limit', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: 
                    // fill once
                    return [4 /*yield*/, fillLimitOrder(testOrder, { amount: defaultTakerAmount.div(2) })];
                    case 1:
                        // fill once
                        _a.sent();
                        return [4 /*yield*/, expectBalances(defaultTakerAmount.div(2), defaultMakerAmount.div(2), defaultTakerAmount.div(2), defaultMakerAmount.div(2))];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultMakerAmount.div(2))];
                    case 3:
                        _a.sent();
                        // fill twice
                        return [4 /*yield*/, fillLimitOrder(testOrder, { amount: defaultTakerAmount.div(2) })];
                    case 4:
                        // fill twice
                        _a.sent();
                        return [4 /*yield*/, expectBalances(Constants_1.INTEGERS.ZERO, defaultMakerAmount, defaultTakerAmount, Constants_1.INTEGERS.ZERO)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultMakerAmount)];
                    case 6:
                        _a.sent();
                        // fail a third time
                        return [4 /*yield*/, Expect_1.expectThrow(fillLimitOrder(testOrder, { amount: defaultTakerAmount.div(2) }), 'StopLimitOrders: Cannot overfill order')];
                    case 7:
                        // fail a third time
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for logs', function () { return __awaiter(_this, void 0, void 0, function () {
            var orderHash, txResult1, logs1, logOrderTaken1, txResult2, logs2, logOrderTaken2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        orderHash = solo.stopLimitOrders.getOrderHash(testOrder);
                        return [4 /*yield*/, fillLimitOrder(testOrder, { amount: defaultTakerAmount.div(2) })];
                    case 1:
                        txResult1 = _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultMakerAmount.div(2))];
                    case 2:
                        _a.sent();
                        logs1 = solo.logs.parseLogs(txResult1);
                        expect(logs1.length).toEqual(5);
                        logOrderTaken1 = logs1[3];
                        expect(logOrderTaken1.name).toEqual('LogStopLimitOrderFilled');
                        expect(logOrderTaken1.args.orderHash).toEqual(orderHash);
                        expect(logOrderTaken1.args.orderMaker).toEqual(testOrder.makerAccountOwner);
                        expect(logOrderTaken1.args.makerFillAmount).toEqual(defaultMakerAmount.div(2));
                        expect(logOrderTaken1.args.totalMakerFilledAmount).toEqual(defaultMakerAmount.div(2));
                        // wait so that the indexes will update
                        return [4 /*yield*/, EVM_1.mineAvgBlock()];
                    case 3:
                        // wait so that the indexes will update
                        _a.sent();
                        return [4 /*yield*/, fillLimitOrder(testOrder, { amount: defaultTakerAmount.div(4) })];
                    case 4:
                        txResult2 = _a.sent();
                        return [4 /*yield*/, expectFilledAmount(testOrder, defaultMakerAmount.times(3).div(4))];
                    case 5:
                        _a.sent();
                        logs2 = solo.logs.parseLogs(txResult2);
                        expect(logs2.length).toEqual(5);
                        logOrderTaken2 = logs2[3];
                        expect(logOrderTaken2.name).toEqual('LogStopLimitOrderFilled');
                        expect(logOrderTaken2.args.orderHash).toEqual(orderHash);
                        expect(logOrderTaken2.args.orderMaker).toEqual(testOrder.makerAccountOwner);
                        expect(logOrderTaken2.args.makerFillAmount).toEqual(defaultMakerAmount.div(4));
                        expect(logOrderTaken2.args.totalMakerFilledAmount)
                            .toEqual(defaultMakerAmount.times(3).div(4));
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
                                solo.stopLimitOrders.approveOrder(testOrderApprove, { from: approver }),
                                solo.stopLimitOrders.cancelOrder(testOrderCancel, { from: canceler }),
                                fillLimitOrder(testOrder, { amount: defaultTakerAmount.div(2) }),
                            ])];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.stopLimitOrders.getOrderStates([
                                testOrder,
                                testOrderCancel,
                                testOrderApprove,
                            ])];
                    case 4:
                        states1 = _a.sent();
                        expect(states1).toEqual([
                            { status: types_1.LimitOrderStatus.Null, totalMakerFilledAmount: testOrder.makerAmount.div(2) },
                            { status: types_1.LimitOrderStatus.Canceled, totalMakerFilledAmount: Constants_1.INTEGERS.ZERO },
                            { status: types_1.LimitOrderStatus.Approved, totalMakerFilledAmount: Constants_1.INTEGERS.ZERO },
                        ]);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getAccountCollateralizationAfterMakingOrders', function () {
        it('succeeds for x/0=infinity', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                expect(solo.stopLimitOrders.getAccountCollateralizationAfterMakingOrders([defaultMakerAmount, Constants_1.INTEGERS.ZERO], [new bignumber_js_1.default(2), new bignumber_js_1.default(1)], [testOrder], [testOrder.makerAmount])).toEqual(new bignumber_js_1.default(Infinity));
                return [2 /*return*/];
            });
        }); });
        it('succeeds for 0/0=infinity', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                expect(solo.stopLimitOrders.getAccountCollateralizationAfterMakingOrders([Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO], [], [], [])).toEqual(new bignumber_js_1.default(Infinity));
                return [2 /*return*/];
            });
        }); });
        it('succeeds for zero', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                expect(solo.stopLimitOrders.getAccountCollateralizationAfterMakingOrders([Constants_1.INTEGERS.ZERO, defaultTakerAmount.times(-1)], [new bignumber_js_1.default(2), new bignumber_js_1.default(1)], [testOrder], [testOrder.makerAmount])).toEqual(new bignumber_js_1.default(0));
                return [2 /*return*/];
            });
        }); });
        it('succeeds for one', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                expect(solo.stopLimitOrders.getAccountCollateralizationAfterMakingOrders([Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO], [new bignumber_js_1.default(2), new bignumber_js_1.default(1)], [testOrder], [testOrder.makerAmount])).toEqual(new bignumber_js_1.default(1));
                return [2 /*return*/];
            });
        }); });
        it('succeeds for two orders', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                expect(solo.stopLimitOrders.getAccountCollateralizationAfterMakingOrders([Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO], [new bignumber_js_1.default(2), new bignumber_js_1.default(1)], [testOrder, testOrder], [testOrder.makerAmount.div(2), testOrder.makerAmount.div(2)])).toEqual(new bignumber_js_1.default(1));
                return [2 /*return*/];
            });
        }); });
    });
});
// ============ Helper Functions ============
function fillLimitOrder(order, _a) {
    var _b = _a.taker, taker = _b === void 0 ? defaultTakerAddress : _b, _c = _a.takerNumber, takerNumber = _c === void 0 ? defaultTakerNumber : _c, _d = _a.amount, amount = _d === void 0 ? defaultTakerAmount : _d, _e = _a.denominatedInMakerAmount, denominatedInMakerAmount = _e === void 0 ? false : _e;
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_f) {
            return [2 /*return*/, solo.operation.initiate().fillSignedStopLimitOrder(taker, takerNumber, order, amount, denominatedInMakerAmount).commit({ from: taker })];
        });
    });
}
function stopLimitOrderToTradeData(order) {
    return {
        primaryAccountOwner: order.takerAccountOwner,
        primaryAccountId: order.takerAccountNumber,
        autoTrader: solo.contracts.stopLimitOrders.options.address,
        inputMarketId: order.takerMarket,
        outputMarketId: order.makerMarket,
        otherAccountOwner: order.makerAccountOwner,
        otherAccountId: order.makerAccountNumber,
        amount: {
            denomination: types_1.AmountDenomination.Wei,
            reference: types_1.AmountReference.Delta,
            value: order.takerAmount,
        },
        data: BytesHelper_1.toBytes(solo.stopLimitOrders.signedOrderToBytes(order)),
    };
}
function expectBalances(takerTakerExpected, takerMakerExpected, makerTakerExpected, makerMakerExpected) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, takerTakerWei, takerMakerWei, makerTakerWei, makerMakerWei;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.getters.getAccountWei(testOrder.takerAccountOwner, testOrder.takerAccountNumber, defaultTakerMarket),
                        solo.getters.getAccountWei(testOrder.takerAccountOwner, testOrder.takerAccountNumber, defaultMakerMarket),
                        solo.getters.getAccountWei(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultTakerMarket),
                        solo.getters.getAccountWei(testOrder.makerAccountOwner, testOrder.makerAccountNumber, defaultMakerMarket),
                    ])];
                case 1:
                    _a = _b.sent(), takerTakerWei = _a[0], takerMakerWei = _a[1], makerTakerWei = _a[2], makerMakerWei = _a[3];
                    expect(takerTakerWei).toEqual(takerTakerExpected);
                    expect(takerMakerWei).toEqual(takerMakerExpected);
                    expect(makerTakerWei).toEqual(makerTakerExpected);
                    expect(makerMakerWei).toEqual(makerMakerExpected);
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
                case 0: return [4 /*yield*/, solo.stopLimitOrders.getOrderStates([order])];
                case 1:
                    states = _a.sent();
                    expect(states[0].totalMakerFilledAmount).toEqual(expectedFilledAmount);
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
                case 0: return [4 /*yield*/, solo.stopLimitOrders.getOrderStates([order])];
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
                    return [4 /*yield*/, solo.stopLimitOrders.signOrder(result, types_1.SigningMethod.TypedData)];
                case 1:
                    _a.typedSignature = _b.sent();
                    return [2 /*return*/, result];
            }
        });
    });
}
//# sourceMappingURL=StopLimitOrders.test.js.map