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
var who;
var operator;
var solo;
var accounts;
var accountNumber = Constants_1.INTEGERS.ZERO;
var market = Constants_1.INTEGERS.ZERO;
var collateralMarket = new bignumber_js_1.default(2);
var collateralAmount = new bignumber_js_1.default(1000000);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(100);
var wei = new bignumber_js_1.default(150);
var negPar = par.times(-1);
var negWei = wei.times(-1);
var defaultGlob;
var CANNOT_DEPOSIT_NEGATIVE = 'Exchange: Cannot transferIn negative';
var cachedWeis = {
    walletWei: zero,
    soloWei: zero,
};
var defaultIndex = {
    lastUpdate: Constants_1.INTEGERS.ZERO,
    borrow: wei.div(par),
    supply: wei.div(par),
};
describe('Deposit', function () {
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
                    who = solo.getDefaultAccount();
                    operator = accounts[6];
                    defaultGlob = {
                        primaryAccountOwner: who,
                        primaryAccountId: accountNumber,
                        marketId: market,
                        from: who,
                        amount: {
                            value: wei,
                            denomination: types_1.AmountDenomination.Actual,
                            reference: types_1.AmountReference.Delta,
                        },
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setMarketIndex(market, defaultIndex),
                            solo.testing.setAccountBalance(who, accountNumber, collateralMarket, collateralAmount),
                            solo.testing.tokenA.setMaximumSoloAllowance(who),
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
                    cachedWeis.walletWei = zero;
                    cachedWeis.soloWei = zero;
                    return [2 /*return*/];
            }
        });
    }); });
    it('Basic deposit test', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, issueTokensToUser(wei)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({})];
                case 2:
                    txResult = _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, wei)];
                case 3:
                    _a.sent();
                    console.log("\tDeposit gas used: " + txResult.gasUsed);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for events', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, marketIndex, collateralIndex, logs, operationLog, marketIndexLog, collateralIndexLog, depositLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.tokenA.issueTo(wei, operator),
                        solo.testing.tokenA.setMaximumSoloAllowance(operator),
                        solo.permissions.approveOperator(operator, { from: who }),
                        solo.testing.setAccountBalance(who, accountNumber, market, par),
                    ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectDepositOkay({ from: operator }, { from: operator })];
                case 2:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(market),
                            solo.getters.getMarketCachedIndex(collateralMarket),
                            expectBalances(par.times(2), wei.times(2), zero, wei),
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
                    depositLog = logs[3];
                    expect(depositLog.name).toEqual('LogDeposit');
                    expect(depositLog.args.accountOwner).toEqual(who);
                    expect(depositLog.args.accountNumber).toEqual(accountNumber);
                    expect(depositLog.args.market).toEqual(market);
                    expect(depositLog.args.update).toEqual({ newPar: par.times(2), deltaWei: wei });
                    expect(depositLog.args.from).toEqual(operator);
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
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(zero),
                            issueTokensToUser(wei),
                        ])];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, wei)];
                case 4:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(par),
                            issueTokensToUser(wei),
                        ])];
                case 5:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par.times(2), wei.times(2), zero, wei)];
                case 7:
                    _a.sent();
                    // starting negative (>par)
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(negPar.times(2)),
                            issueTokensToUser(wei),
                        ])];
                case 8:
                    // starting negative (>par)
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, zero, wei)];
                case 10:
                    _a.sent();
                    // starting negative (=par)
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(negPar),
                            issueTokensToUser(wei),
                        ])];
                case 11:
                    // starting negative (=par)
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 12:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, wei)];
                case 13:
                    _a.sent();
                    // starting negative (<par)
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(negPar.div(2)),
                            issueTokensToUser(wei),
                        ])];
                case 14:
                    // starting negative (<par)
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 15:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par.div(2), wei.div(2), zero, wei)];
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
                    return [4 /*yield*/, setAccountBalance(zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, zero)];
                case 4:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalance(par)];
                case 5:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, zero)];
                case 7:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, setAccountBalance(negPar)];
                case 8:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, zero, zero)];
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
    it('Fails for negative delta par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var reason, globs, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    reason = CANNOT_DEPOSIT_NEGATIVE;
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
                    return [4 /*yield*/, issueTokensToUser(wei)];
                case 1:
                    _a.sent();
                    i = 0;
                    _a.label = 2;
                case 2:
                    if (!(i < globs.length)) return [3 /*break*/, 10];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalance(zero)];
                case 3:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], reason)];
                case 4:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalance(par)];
                case 5:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], reason)];
                case 6:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, setAccountBalance(negPar)];
                case 7:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], reason)];
                case 8:
                    _a.sent();
                    _a.label = 9;
                case 9:
                    i += 1;
                    return [3 /*break*/, 2];
                case 10: return [2 /*return*/];
            }
        });
    }); });
    it('Mixed for positive target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
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
                    if (!(i < globs.length)) return [3 /*break*/, 17];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(zero),
                            issueTokensToUser(wei),
                        ])];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, wei)];
                case 4:
                    _a.sent();
                    // starting positive (<target)
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(par.div(2)),
                            issueTokensToUser(wei.div(2)),
                        ])];
                case 5:
                    // starting positive (<target)
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, wei.div(2))];
                case 7:
                    _a.sent();
                    // starting positive (=target)
                    return [4 /*yield*/, setAccountBalance(par)];
                case 8:
                    // starting positive (=target)
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, zero)];
                case 10:
                    _a.sent();
                    // starting positive (>target)
                    return [4 /*yield*/, setAccountBalance(par.times(2))];
                case 11:
                    // starting positive (>target)
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], CANNOT_DEPOSIT_NEGATIVE)];
                case 12:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(negPar),
                            issueTokensToUser(wei.times(2)),
                        ])];
                case 13:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 14:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, wei, zero, wei.times(2))];
                case 15:
                    _a.sent();
                    _a.label = 16;
                case 16:
                    i += 1;
                    return [3 /*break*/, 1];
                case 17: return [2 /*return*/];
            }
        });
    }); });
    it('Mixed for zero target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
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
                    if (!(i < globs.length)) return [3 /*break*/, 11];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalance(zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 3:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, zero)];
                case 4:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalance(par)];
                case 5:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], CANNOT_DEPOSIT_NEGATIVE)];
                case 6:
                    // starting positive
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(negPar),
                            issueTokensToUser(wei),
                        ])];
                case 7:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 8:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, wei)];
                case 9:
                    _a.sent();
                    _a.label = 10;
                case 10:
                    i += 1;
                    return [3 /*break*/, 1];
                case 11: return [2 /*return*/];
            }
        });
    }); });
    it('Mixed for negative target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
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
                    if (!(i < globs.length)) return [3 /*break*/, 15];
                    // starting from zero
                    return [4 /*yield*/, setAccountBalance(zero)];
                case 2:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], CANNOT_DEPOSIT_NEGATIVE)];
                case 3:
                    _a.sent();
                    // starting negative (<target)
                    return [4 /*yield*/, Promise.all([
                            setAccountBalance(negPar.times(2)),
                            issueTokensToUser(wei),
                        ])];
                case 4:
                    // starting negative (<target)
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, zero, wei)];
                case 6:
                    _a.sent();
                    // starting negative (=target)
                    return [4 /*yield*/, setAccountBalance(negPar)];
                case 7:
                    // starting negative (=target)
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay(globs[i])];
                case 8:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(negPar, negWei, zero, zero)];
                case 9:
                    _a.sent();
                    // starting negative (>target)
                    return [4 /*yield*/, setAccountBalance(negPar.div(2))];
                case 10:
                    // starting negative (>target)
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], CANNOT_DEPOSIT_NEGATIVE)];
                case 11:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, setAccountBalance(par)];
                case 12:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert(globs[i], CANNOT_DEPOSIT_NEGATIVE)];
                case 13:
                    _a.sent();
                    _a.label = 14;
                case 14:
                    i += 1;
                    return [3 /*break*/, 1];
                case 15: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for lending in par', function () { return __awaiter(_this, void 0, void 0, function () {
        var supplyIndex, expectedWei;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    supplyIndex = new bignumber_js_1.default('3.99');
                    expectedWei = par.times(supplyIndex).integerValue(bignumber_js_1.default.ROUND_DOWN);
                    return [4 /*yield*/, Promise.all([
                            issueTokensToUser(expectedWei),
                            solo.testing.setMarketIndex(market, {
                                lastUpdate: Constants_1.INTEGERS.ZERO,
                                borrow: Constants_1.INTEGERS.ONE,
                                supply: supplyIndex,
                            }),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, expectedWei, zero, expectedWei)];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for lending in wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var supplyIndex, expectedWei;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    supplyIndex = new bignumber_js_1.default('3.99');
                    expectedWei = par.times(supplyIndex).integerValue(bignumber_js_1.default.ROUND_DOWN);
                    return [4 /*yield*/, Promise.all([
                            issueTokensToUser(expectedWei),
                            solo.testing.setMarketIndex(market, {
                                lastUpdate: Constants_1.INTEGERS.ZERO,
                                borrow: Constants_1.INTEGERS.ONE,
                                supply: supplyIndex,
                            }),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({
                            amount: {
                                value: expectedWei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(par, expectedWei, zero, expectedWei)];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for repaying in par', function () { return __awaiter(_this, void 0, void 0, function () {
        var borrowIndex, expectedWei;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    borrowIndex = new bignumber_js_1.default('1.99');
                    expectedWei = par.times(borrowIndex).integerValue(bignumber_js_1.default.ROUND_UP);
                    return [4 /*yield*/, Promise.all([
                            issueTokensToUser(expectedWei),
                            solo.testing.setMarketIndex(market, {
                                lastUpdate: Constants_1.INTEGERS.ZERO,
                                borrow: borrowIndex,
                                supply: Constants_1.INTEGERS.ONE,
                            }),
                            solo.testing.setAccountBalance(who, accountNumber, market, negPar),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, expectedWei)];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for repaying in wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var borrowIndex, expectedWei;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    borrowIndex = new bignumber_js_1.default('1.99');
                    expectedWei = par.times(borrowIndex).integerValue(bignumber_js_1.default.ROUND_UP);
                    return [4 /*yield*/, Promise.all([
                            issueTokensToUser(expectedWei),
                            solo.testing.setMarketIndex(market, {
                                lastUpdate: Constants_1.INTEGERS.ZERO,
                                borrow: borrowIndex,
                                supply: Constants_1.INTEGERS.ONE,
                            }),
                            solo.testing.setAccountBalance(who, accountNumber, market, negPar),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({
                            amount: {
                                value: expectedWei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, expectBalances(zero, zero, zero, expectedWei)];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds and sets status to Normal', function () { return __awaiter(_this, void 0, void 0, function () {
        var status;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueTokensToUser(wei),
                        solo.testing.setAccountStatus(who, accountNumber, types_1.AccountStatus.Liquidating),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getAccountStatus(who, accountNumber)];
                case 3:
                    status = _a.sent();
                    expect(status).toEqual(types_1.AccountStatus.Normal);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for local operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueTokensToUser(wei),
                        solo.permissions.approveOperator(operator),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for global operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueTokensToUser(wei),
                        solo.admin.setGlobalOperator(operator, true, { from: accounts[0] }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, issueTokensToUser(wei)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectDepositRevert({}, 'Storage: Unpermissioned operator', { from: operator })];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for from random address', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectDepositRevert({ from: operator }, 'OperationImpl: Invalid deposit source')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if depositing more tokens than owned', function () { return __awaiter(_this, void 0, void 0, function () {
        var glob;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    glob = {
                        amount: {
                            value: wei,
                            denomination: types_1.AmountDenomination.Actual,
                            reference: types_1.AmountReference.Delta,
                        },
                    };
                    return [4 /*yield*/, expectDepositRevert(glob, 'Token: TransferFrom failed')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function setAccountBalance(amount) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.testing.setAccountBalance(who, accountNumber, market, amount)];
        });
    });
}
function issueTokensToUser(amount) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.testing.tokenA.issueTo(amount, who)];
        });
    });
}
function expectBalances(expectedPar, expectedWei, walletWei, soloWei) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, accountBalances, walletTokenBalance, soloTokenBalance;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.getters.getAccountBalances(who, accountNumber),
                        solo.testing.tokenA.getBalance(who),
                        solo.testing.tokenA.getBalance(solo.contracts.soloMargin.options.address),
                    ])];
                case 1:
                    _a = _b.sent(), accountBalances = _a[0], walletTokenBalance = _a[1], soloTokenBalance = _a[2];
                    accountBalances.forEach(function (balance, i) {
                        var expected = { par: zero, wei: zero };
                        if (i === market.toNumber()) {
                            expected = { par: expectedPar, wei: expectedWei };
                        }
                        else if (i === collateralMarket.toNumber()) {
                            expected = {
                                par: collateralAmount,
                                wei: collateralAmount,
                            };
                        }
                        expect(balance.par).toEqual(expected.par);
                        expect(balance.wei).toEqual(expected.wei);
                    });
                    expect(walletTokenBalance.minus(cachedWeis.walletWei)).toEqual(walletWei);
                    expect(soloTokenBalance.minus(cachedWeis.soloWei)).toEqual(soloWei);
                    cachedWeis.walletWei = walletTokenBalance;
                    cachedWeis.soloWei = soloTokenBalance;
                    return [2 /*return*/];
            }
        });
    });
}
function expectDepositOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob;
        return __generator(this, function (_a) {
            combinedGlob = __assign({}, defaultGlob, glob);
            return [2 /*return*/, solo.operation.initiate().deposit(combinedGlob).commit(options)];
        });
    });
}
function expectDepositRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectDepositOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Deposit.test.js.map