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
var Solo_1 = require("../helpers/Solo");
var EVM_1 = require("../helpers/EVM");
var SoloHelpers_1 = require("../helpers/SoloHelpers");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var types_1 = require("../../src/types");
var solo;
var accounts;
var snapshotId;
var admin;
var owner1;
var owner2;
var operator;
var accountNumber1 = new bignumber_js_1.default(111);
var accountNumber2 = new bignumber_js_1.default(222);
var market1 = Constants_1.INTEGERS.ZERO;
var market2 = Constants_1.INTEGERS.ONE;
var market3 = new bignumber_js_1.default(2);
var market4 = new bignumber_js_1.default(3);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(10000);
var negPar = par.times(-1);
var minLiquidatorRatio = new bignumber_js_1.default('0.25');
var prices = [
    new bignumber_js_1.default('1e20'),
    new bignumber_js_1.default('1e18'),
    new bignumber_js_1.default('1e18'),
    new bignumber_js_1.default('1e21'),
];
describe('LiquidatorProxyV1ForSoloMargin', function () {
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    admin = accounts[0];
                    owner1 = solo.getDefaultAccount();
                    owner2 = accounts[3];
                    operator = accounts[6];
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.testing.priceOracle.setPrice(solo.testing.tokenA.getAddress(), prices[0]),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenB.getAddress(), prices[1]),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenC.getAddress(), prices[2]),
                            solo.testing.priceOracle.setPrice(solo.weth.getAddress(), prices[3]),
                            solo.permissions.approveOperator(operator, { from: owner1 }),
                            solo.permissions.approveOperator(solo.contracts.liquidatorProxyV1.options.address, { from: owner1 }),
                        ])];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, solo.admin.addMarket(solo.weth.getAddress(), solo.testing.priceOracle.getAddress(), solo.testing.interestSetter.getAddress(), zero, zero, { from: admin })];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, EVM_1.mineAvgBlock()];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 7:
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
    describe('#liquidate', function () {
        describe('Success cases', function () {
            it('Succeeds for one owed, one held', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, setUpBasicBalances()];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([zero, par.times('105')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, one held (held first)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('100')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, par.times('1.1')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, negPar.times('100')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([par.times('1.05'), zero], [par.times('.05'), zero])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, one held (undercollateralized)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, setUpBasicBalances()];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('95'))];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 3:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([par.times('0.0952'), par.times('95')], [negPar.times('0.0952'), zero])];
                        case 4:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, many held', function () { return __awaiter(_this, void 0, void 0, function () {
                var txResult;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('60')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market3, par.times('50')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            txResult = _a.sent();
                            return [4 /*yield*/, expectBalances([zero, par.times('60'), par.times('44.9925')], [zero, zero, par.times('5.0075')])];
                        case 3:
                            _a.sent();
                            console.log("\tLiquidatorProxyV1 gas used (1 owed, 2 held): " + txResult.gasUsed);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for many owed, one held', function () { return __awaiter(_this, void 0, void 0, function () {
                var txResult;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('100')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, negPar.times('50')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market3, par.times('165')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            txResult = _a.sent();
                            return [4 /*yield*/, expectBalances([zero, par.times('50'), par.times('157.5')], [zero, zero, par.times('7.5')])];
                        case 3:
                            _a.sent();
                            console.log("\tLiquidatorProxyV1 gas used (2 owed, 1 held): " + txResult.gasUsed);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for many owed, many held', function () { return __awaiter(_this, void 0, void 0, function () {
                var txResult;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('150')),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market4, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, par.times('0.525')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, negPar.times('100')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market3, par.times('170')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market4, negPar.times('0.1')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            txResult = _a.sent();
                            return [4 /*yield*/, expectBalances([par.times('0.525'), par.times('50'), par.times('157.5'), par.times('0.9')], [zero, zero, par.times('12.5'), zero])];
                        case 3:
                            _a.sent();
                            console.log("\tLiquidatorProxyV1 gas used (2 owed, 2 held): " + txResult.gasUsed);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for liquid account collateralized but in liquid status', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('150')),
                                solo.testing.setAccountStatus(owner2, accountNumber2, types_1.AccountStatus.Liquidating),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([zero, par.times('105')], [zero, par.times('45')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('Success cases for various initial liquidator balances', function () {
            it('Succeeds for one owed, one held (liquidator balance is zero)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market4, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar, par.times('105')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, one held (liquidator balance is posHeld/negOwed)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, negPar),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('500')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar.times(2), par.times('605')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, one held (liquidator balance is negatives)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, negPar.div(2)),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('50')),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market4, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar.times('1.5'), par.times('55')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, one held (liquidator balance is positives)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par.div(2)),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('50')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar.div(2), par.times('155')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, one held (liquidator balance is !posHeld>!negOwed)', function () { return __awaiter(_this, void 0, void 0, function () {
                var txResult;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par.div(2)),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('100')),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market4, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            txResult = _a.sent();
                            return [4 /*yield*/, expectBalances([negPar.div(2), par.times('5')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            console.log("\tLiquidatorProxyV1 gas used: " + txResult.gasUsed);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for one owed, one held (liquidator balance is !posHeld<!negOwed)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('50')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([zero, par.times('55')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('Limited by minLiquidatorRatio', function () {
            it('Liquidates as much as it can (to 1.25) but no more', function () { return __awaiter(_this, void 0, void 0, function () {
                var liquidatorValues;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, negPar.div(2)),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('65')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar.times('0.625'), par.times('78.125')], [negPar.times('0.875'), par.times('96.875')])];
                        case 3:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getAccountValues(owner1, accountNumber1)];
                        case 4:
                            liquidatorValues = _a.sent();
                            expect(liquidatorValues.supply).toEqual(liquidatorValues.borrow.times('1.25'));
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Liquidates to negOwed/posHeld and then to 1.25', function () { return __awaiter(_this, void 0, void 0, function () {
                var liquidatorValues;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par.times('0.2')),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('10')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar.times('0.55'), par.times('68.75')], [negPar.times('0.25'), par.times('31.25')])];
                        case 3:
                            _a.sent();
                            return [4 /*yield*/, solo.getters.getAccountValues(owner1, accountNumber1)];
                        case 4:
                            liquidatorValues = _a.sent();
                            expect(liquidatorValues.supply).toEqual(liquidatorValues.borrow.times('1.25'));
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Liquidates to zero', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('105')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([zero, zero], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Liquidates even if it starts below 1.25', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par.times('2.4')),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('200')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([par.times('1.4'), negPar.times('95')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Does not liquidate below 1.25', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, negPar.div(2)),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('60')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar.div(2), par.times('60')], [negPar, par.times('110')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Does not liquidate at 1.25', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, negPar),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('125')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([negPar, par.times('125')], [negPar, par.times('110')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('Follows minValueLiquidated', function () {
            it('Succeeds for less than valueLiquidatable', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.liquidatorProxy.liquidate(owner1, accountNumber1, owner2, accountNumber2, minLiquidatorRatio, par.times(prices[0]), [market1], [market2], { from: operator })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([zero, par.times('105')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Succeeds for less than valueLiquidatable (even if liquidAccount is small)', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.liquidatorProxy.liquidate(owner1, accountNumber1, owner2, accountNumber2, minLiquidatorRatio, par.times(prices[0]).times(5), [market1], [market2], { from: operator })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([zero, par.times('105')], [zero, par.times('5')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Reverts if cannot liquidate enough', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par.times('0.2')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(solo.liquidatorProxy.liquidate(owner1, accountNumber1, owner2, accountNumber2, minLiquidatorRatio, par.times(prices[0]).times(2), [market1], [market2], { from: operator }), 'LiquidatorProxyV1ForSoloMargin: Not enough liquidatable value')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Reverts if cannot liquidate even 1', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('125')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(solo.liquidatorProxy.liquidate(owner1, accountNumber1, owner2, accountNumber2, minLiquidatorRatio, new bignumber_js_1.default(1), [market1], [market2], { from: operator }), 'LiquidatorProxyV1ForSoloMargin: Not enough liquidatable value')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('Follows preferences', function () {
            it('Liquidates the most specified markets first', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market4, par.times('0.02')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market3, negPar.times('100')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market4, par.times('0.11')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.liquidatorProxy.liquidate(owner1, accountNumber1, owner2, accountNumber2, minLiquidatorRatio, zero, [market3, market1], [market4, market2], { from: operator })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([zero, zero, negPar.times('100'), par.times('0.125')], [negPar, par.times('110'), zero, par.times('.005')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Does not liquidate unspecified markets', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                                solo.testing.setAccountBalance(owner1, accountNumber1, market2, par.times('100')),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.liquidatorProxy.liquidate(owner1, accountNumber1, owner2, accountNumber2, minLiquidatorRatio, zero, [market2], [market1], { from: operator })];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, expectBalances([par, par.times('100')], [negPar, par.times('110')])];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('Failure cases', function () {
            it('Fails for msg.sender is non-operator', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                setUpBasicBalances(),
                                solo.permissions.disapproveOperator(operator, { from: owner1 }),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(liquidate(), 'LiquidatorProxyV1ForSoloMargin: Sender not operator')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for proxy is non-operator', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Promise.all([
                                setUpBasicBalances(),
                                solo.permissions.disapproveOperator(solo.contracts.liquidatorProxyV1.options.address, { from: owner1 }),
                            ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(liquidate(), 'Storage: Unpermissioned operator')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for liquid account no supply', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, setUpBasicBalances()];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, market2, zero)];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(liquidate(), 'LiquidatorProxyV1ForSoloMargin: Liquid account no supply')];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('Fails for liquid account not liquidatable', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, setUpBasicBalances()];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('115'))];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(liquidate(), 'LiquidatorProxyV1ForSoloMargin: Liquid account not liquidatable')];
                        case 3:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
        describe('Interest cases', function () {
            it('Liquidates properly even if the indexes have changed', function () { return __awaiter(_this, void 0, void 0, function () {
                var rate;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            rate = new bignumber_js_1.default(1).div(Constants_1.INTEGERS.ONE_YEAR_IN_SECONDS);
                            return [4 /*yield*/, Promise.all([
                                    solo.testing.interestSetter.setInterestRate(solo.testing.tokenA.getAddress(), rate),
                                    solo.testing.interestSetter.setInterestRate(solo.testing.tokenB.getAddress(), rate),
                                    solo.testing.setMarketIndex(market1, {
                                        borrow: new bignumber_js_1.default('1.2'),
                                        supply: new bignumber_js_1.default('1.1'),
                                        lastUpdate: zero,
                                    }),
                                    solo.testing.setMarketIndex(market2, {
                                        borrow: new bignumber_js_1.default('1.2'),
                                        supply: new bignumber_js_1.default('1.1'),
                                        lastUpdate: zero,
                                    }),
                                ])];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, EVM_1.fastForward(3600)];
                        case 2:
                            _a.sent();
                            return [4 /*yield*/, Promise.all([
                                    solo.testing.setAccountBalance(owner1, accountNumber1, market1, par.div(2)),
                                    solo.testing.setAccountBalance(owner1, accountNumber1, market2, negPar.times('30')),
                                    solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                                    solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                                ])];
                        case 3:
                            _a.sent();
                            return [4 /*yield*/, liquidate()];
                        case 4:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
        });
    });
});
// ============ Helper Functions ============
function setUpBasicBalances() {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountBalance(owner1, accountNumber1, market1, par),
                        solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                        solo.testing.setAccountBalance(owner2, accountNumber2, market2, par.times('110')),
                    ])];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function liquidate() {
    return __awaiter(this, void 0, void 0, function () {
        var preferences, txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    preferences = [
                        market1,
                        market2,
                        market3,
                        market4,
                    ];
                    return [4 /*yield*/, solo.liquidatorProxy.liquidate(owner1, accountNumber1, owner2, accountNumber2, minLiquidatorRatio, zero, preferences, preferences, { from: operator })];
                case 1:
                    txResult = _a.sent();
                    return [2 /*return*/, txResult];
            }
        });
    });
}
function expectBalances(liquidatorBalances, liquidBalances) {
    return __awaiter(this, void 0, void 0, function () {
        var bal1, bal2, i, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.getters.getAccountPar(owner1, accountNumber1, market1),
                        solo.getters.getAccountPar(owner1, accountNumber1, market2),
                        solo.getters.getAccountPar(owner1, accountNumber1, market3),
                        solo.getters.getAccountPar(owner1, accountNumber1, market4),
                    ])];
                case 1:
                    bal1 = _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getAccountPar(owner2, accountNumber2, market1),
                            solo.getters.getAccountPar(owner2, accountNumber2, market2),
                            solo.getters.getAccountPar(owner2, accountNumber2, market3),
                            solo.getters.getAccountPar(owner2, accountNumber2, market4),
                        ])];
                case 2:
                    bal2 = _a.sent();
                    for (i = 0; i < liquidatorBalances.length; i += 1) {
                        expect(bal1[i]).toEqual(liquidatorBalances[i]);
                    }
                    for (i = 0; i < liquidBalances.length; i += 1) {
                        expect(bal2[i]).toEqual(liquidBalances[i]);
                    }
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=LiquidatorProxyV1ForSoloMargin.test.js.map