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
var SoloHelpers_1 = require("../helpers/SoloHelpers");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var types_1 = require("../../src/types");
var owner1;
var owner2;
var admin;
var operator;
var accountNumber1 = new bignumber_js_1.default(133);
var accountNumber2 = new bignumber_js_1.default(244);
var solo;
var accounts;
var market = Constants_1.INTEGERS.ZERO;
var collateralMarket = new bignumber_js_1.default(2);
var collateralAmount = new bignumber_js_1.default(1000000);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(100);
var wei = new bignumber_js_1.default(150);
var negPar = par.times(-1);
var negWei = wei.times(-1);
var defaultGlob;
describe('Transfer', function () {
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
                    admin = accounts[0];
                    owner1 = accounts[5];
                    owner2 = accounts[6];
                    operator = solo.getDefaultAccount();
                    defaultGlob = {
                        primaryAccountOwner: owner1,
                        primaryAccountId: accountNumber1,
                        toAccountOwner: owner2,
                        toAccountId: accountNumber2,
                        marketId: market,
                        amount: {
                            value: zero,
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Target,
                        },
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setMarketIndex(market, {
                                lastUpdate: Constants_1.INTEGERS.ZERO,
                                borrow: new bignumber_js_1.default(1.5),
                                supply: new bignumber_js_1.default(1.5),
                            }),
                            solo.permissions.approveOperator(operator, { from: owner1 }),
                            solo.permissions.approveOperator(operator, { from: owner2 }),
                            solo.testing.setAccountBalance(owner1, accountNumber1, collateralMarket, collateralAmount),
                            solo.testing.setAccountBalance(owner2, accountNumber2, collateralMarket, collateralAmount),
                        ])];
                case 4:
                    _a.sent();
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
    it('Basic transfer test', function () { return __awaiter(_this, void 0, void 0, function () {
        var fullAmount, halfAmount, txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    fullAmount = new bignumber_js_1.default(100);
                    halfAmount = new bignumber_js_1.default(50);
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setMarketIndex(market, {
                                lastUpdate: Constants_1.INTEGERS.ZERO,
                                borrow: Constants_1.INTEGERS.ONE,
                                supply: Constants_1.INTEGERS.ONE,
                            }),
                            solo.testing.setAccountBalance(owner1, accountNumber1, market, fullAmount),
                            solo.testing.setAccountBalance(owner2, accountNumber2, market, fullAmount),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay({
                            amount: {
                                value: halfAmount,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    txResult = _a.sent();
                    return [4 /*yield*/, expectBalances(fullAmount.plus(halfAmount), fullAmount.plus(halfAmount), fullAmount.minus(halfAmount), fullAmount.minus(halfAmount))];
                case 3:
                    _a.sent();
                    console.log("\tTransfer gas used: " + txResult.gasUsed);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for events', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, marketIndex, collateralIndex, logs, operationLog, marketIndexLog, collateralIndexLog, transferLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, solo.admin.setGlobalOperator(operator, true, { from: admin })];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectTransferOkay({
                            amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        }, { from: operator })];
                case 2:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(market),
                            solo.getters.getMarketCachedIndex(collateralMarket),
                            expectBalances(par, wei, negPar, negWei),
                        ])];
                case 3:
                    _a = _b.sent(), marketIndex = _a[0], collateralIndex = _a[1];
                    logs = solo.logs.parseLogs(txResult);
                    expect(logs.length).toEqual(4);
                    operationLog = logs[0];
                    expect(operationLog.name).toEqual('LogOperation');
                    expect(operationLog.args.sender).toEqual(operator);
                    marketIndexLog = logs[1];
                    expect(marketIndexLog.name).toEqual('LogIndexUpdate');
                    expect(marketIndexLog.args.market).toEqual(market);
                    expect(marketIndexLog.args.index).toEqual(marketIndex);
                    collateralIndexLog = logs[2];
                    expect(collateralIndexLog.name).toEqual('LogIndexUpdate');
                    expect(collateralIndexLog.args.market).toEqual(collateralMarket);
                    expect(collateralIndexLog.args.index).toEqual(collateralIndex);
                    transferLog = logs[3];
                    expect(transferLog.name).toEqual('LogTransfer');
                    expect(transferLog.args.accountOneOwner).toEqual(owner1);
                    expect(transferLog.args.accountOneNumber).toEqual(accountNumber1);
                    expect(transferLog.args.accountTwoOwner).toEqual(owner2);
                    expect(transferLog.args.accountTwoNumber).toEqual(accountNumber2);
                    expect(transferLog.args.market).toEqual(market);
                    expect(transferLog.args.updateOne).toEqual({ newPar: par, deltaWei: wei });
                    expect(transferLog.args.updateTwo).toEqual({ newPar: negPar, deltaWei: negWei });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for positive delta par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    globs = [
                        {
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        },
                        {
                            amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        },
                    ];
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i < globs.length)) return [3 /*break*/, 18];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalances(zero, zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, negPar, negWei)];
                case 4:
                    _a.sent();
                    // starting negative (>par)
                    return [4 /*yield*/, setAccountBalances(negPar.times(2), zero)];
                case 5:
                    // starting negative (>par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, negPar, negWei)];
                case 7:
                    _a.sent();
                    // starting negative (=par)
                    return [4 /*yield*/, setAccountBalances(negPar, zero)];
                case 8:
                    // starting negative (=par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, negPar, negWei)];
                case 10:
                    _a.sent();
                    // starting negative (<par)
                    return [4 /*yield*/, setAccountBalances(negPar.div(2), zero)];
                case 11:
                    // starting negative (<par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 12:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par.div(2), wei.div(2), negPar, negWei)];
                case 13:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalances(par, zero)];
                case 14:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 15:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par.times(2), wei.times(2), negPar, negWei)];
                case 16:
                    _a.sent();
                    _a.label = 17;
                case 17:
                    i += 1;
                    return [3 /*break*/, 1];
                case 18: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero delta par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    globs = [
                        {
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        },
                        {
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        },
                    ];
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i < globs.length)) return [3 /*break*/, 12];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalances(zero, zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, zero)];
                case 4:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalances(par, negPar)];
                case 5:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, negPar, negWei)];
                case 7:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, setAccountBalances(negPar, par)];
                case 8:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, par, wei)];
                case 10:
                    _a.sent();
                    _a.label = 11;
                case 11:
                    i += 1;
                    return [3 /*break*/, 1];
                case 12: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for negative delta par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    globs = [
                        {
                            amount: {
                                value: negPar,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        },
                        {
                            amount: {
                                value: negWei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        },
                    ];
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i < globs.length)) return [3 /*break*/, 18];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalances(zero, zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, par, wei)];
                case 4:
                    _a.sent();
                    // starting positive (>par)
                    return [4 /*yield*/, setAccountBalances(par.times(2), zero)];
                case 5:
                    // starting positive (>par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, par, wei)];
                case 7:
                    _a.sent();
                    // starting positive (=par)
                    return [4 /*yield*/, setAccountBalances(par, zero)];
                case 8:
                    // starting positive (=par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, par, wei)];
                case 10:
                    _a.sent();
                    // starting positive (<par)
                    return [4 /*yield*/, setAccountBalances(par.div(2), zero)];
                case 11:
                    // starting positive (<par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 12:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar.div(2), negWei.div(2), par, wei)];
                case 13:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, setAccountBalances(negPar, zero)];
                case 14:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 15:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar.times(2), negWei.times(2), par, wei)];
                case 16:
                    _a.sent();
                    _a.label = 17;
                case 17:
                    i += 1;
                    return [3 /*break*/, 1];
                case 18: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for positive target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    globs = [
                        {
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Target,
                            },
                        },
                        {
                            amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        },
                    ];
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i < globs.length)) return [3 /*break*/, 18];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalances(zero, zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, negPar, negWei)];
                case 4:
                    _a.sent();
                    // starting positive (>par)
                    return [4 /*yield*/, setAccountBalances(par.times(2), zero)];
                case 5:
                    // starting positive (>par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, par, wei)];
                case 7:
                    _a.sent();
                    // starting positive (=par)
                    return [4 /*yield*/, setAccountBalances(par, zero)];
                case 8:
                    // starting positive (=par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, zero)];
                case 10:
                    _a.sent();
                    // starting positive (<par)
                    return [4 /*yield*/, setAccountBalances(par.div(2), zero)];
                case 11:
                    // starting positive (<par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 12:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, negPar.div(2), negWei.div(2))];
                case 13:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, setAccountBalances(negPar, zero)];
                case 14:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 15:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, negPar.times(2), negWei.times(2))];
                case 16:
                    _a.sent();
                    _a.label = 17;
                case 17:
                    i += 1;
                    return [3 /*break*/, 1];
                case 18: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    globs = [
                        {
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Target,
                            },
                        },
                        {
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        },
                    ];
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i < globs.length)) return [3 /*break*/, 12];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalances(zero, zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, zero)];
                case 4:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, setAccountBalances(negPar, zero)];
                case 5:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, negPar, negWei)];
                case 7:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalances(par, zero)];
                case 8:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, par, wei)];
                case 10:
                    _a.sent();
                    _a.label = 11;
                case 11:
                    i += 1;
                    return [3 /*break*/, 1];
                case 12: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for negative target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    globs = [
                        {
                            amount: {
                                value: negPar,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Target,
                            },
                        },
                        {
                            amount: {
                                value: negWei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        },
                    ];
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i < globs.length)) return [3 /*break*/, 18];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalances(zero, zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, par, wei)];
                case 4:
                    _a.sent();
                    // starting negative (>par)
                    return [4 /*yield*/, setAccountBalances(negPar.times(2), zero)];
                case 5:
                    // starting negative (>par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, negPar, negWei)];
                case 7:
                    _a.sent();
                    // starting negative (=par)
                    return [4 /*yield*/, setAccountBalances(negPar, zero)];
                case 8:
                    // starting negative (=par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, zero, zero)];
                case 10:
                    _a.sent();
                    // starting negative (<par)
                    return [4 /*yield*/, setAccountBalances(negPar.div(2), zero)];
                case 11:
                    // starting negative (<par)
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 12:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, par.div(2), wei.div(2))];
                case 13:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalances(par, zero)];
                case 14:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay(globs[i])];
                case 15:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, par.times(2), wei.times(2))];
                case 16:
                    _a.sent();
                    _a.label = 17;
                case 17:
                    i += 1;
                    return [3 /*break*/, 1];
                case 18: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds and sets status to Normal', function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, status1, status2;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountStatus(owner1, accountNumber1, types_1.AccountStatus.Liquidating),
                        solo.testing.setAccountStatus(owner2, accountNumber2, types_1.AccountStatus.Liquidating),
                    ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectTransferOkay({})];
                case 2:
                    _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getAccountStatus(owner1, accountNumber1),
                            solo.getters.getAccountStatus(owner2, accountNumber2),
                        ])];
                case 3:
                    _a = _b.sent(), status1 = _a[0], status2 = _a[1];
                    expect(status1).toEqual(types_1.AccountStatus.Normal);
                    expect(status2).toEqual(types_1.AccountStatus.Normal);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for global operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.permissions.disapproveOperator(operator, { from: owner1 }),
                        solo.permissions.disapproveOperator(operator, { from: owner2 }),
                        solo.admin.setGlobalOperator(operator, true, { from: admin }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTransferOkay({})];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for owner of both accounts', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectTransferOkay({
                        toAccountOwner: owner1,
                    })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator on first account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.disapproveOperator(operator, { from: owner1 })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTransferRevert({
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        }, 'Storage: Unpermissioned operator')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator on second account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.disapproveOperator(operator, { from: owner2 })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTransferRevert({
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        }, 'Storage: Unpermissioned operator')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails to transfer to same account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectTransferRevert({
                        toAccountOwner: owner1,
                        toAccountId: accountNumber1,
                    }, 'OperationImpl: Duplicate accounts in action')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function setAccountBalances(amount1, amount2) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountBalance(owner1, accountNumber1, market, amount1),
                        solo.testing.setAccountBalance(owner2, accountNumber2, market, amount2),
                    ])];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function expectBalances(par1, wei1, par2, wei2) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, accountBalances1, accountBalances2;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.getters.getAccountBalances(owner1, accountNumber1),
                        solo.getters.getAccountBalances(owner2, accountNumber2),
                    ])];
                case 1:
                    _a = _b.sent(), accountBalances1 = _a[0], accountBalances2 = _a[1];
                    accountBalances1.forEach(function (balance, i) {
                        var expected = { par: zero, wei: zero };
                        if (i === market.toNumber()) {
                            expected = { par: par1, wei: wei1 };
                        }
                        else if (i === collateralMarket.toNumber()) {
                            expected = { par: collateralAmount, wei: collateralAmount };
                        }
                        expect(balance.par).toEqual(expected.par);
                        expect(balance.wei).toEqual(expected.wei);
                    });
                    accountBalances2.forEach(function (balance, i) {
                        var expected = { par: zero, wei: zero };
                        if (i === market.toNumber()) {
                            expected = { par: par2, wei: wei2 };
                        }
                        else if (i === collateralMarket.toNumber()) {
                            expected = { par: collateralAmount, wei: collateralAmount };
                        }
                        expect(balance.par).toEqual(expected.par);
                        expect(balance.wei).toEqual(expected.wei);
                    });
                    return [2 /*return*/];
            }
        });
    });
}
function expectTransferOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob;
        return __generator(this, function (_a) {
            combinedGlob = __assign({}, defaultGlob, glob);
            return [2 /*return*/, solo.operation.initiate().transfer(combinedGlob).commit(options)];
        });
    });
}
function expectTransferRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectTransferOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Transfer.test.js.map