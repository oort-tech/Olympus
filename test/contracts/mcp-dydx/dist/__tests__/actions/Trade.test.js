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
var BytesHelper_1 = require("../../src/lib/BytesHelper");
var EVM_1 = require("../helpers/EVM");
var SoloHelpers_1 = require("../helpers/SoloHelpers");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var types_1 = require("../../src/types");
var who1;
var who2;
var operator;
var solo;
var accounts;
var accountNumber1 = new bignumber_js_1.default(111);
var accountNumber2 = new bignumber_js_1.default(222);
var collateralMkt = new bignumber_js_1.default(0);
var inputMkt = new bignumber_js_1.default(1);
var outputMkt = new bignumber_js_1.default(2);
var collateralAmount = new bignumber_js_1.default(1000000);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(100);
var wei = new bignumber_js_1.default(150);
var negPar = par.times(-1);
var negWei = wei.times(-1);
var defaultGlob;
var defaultData = {
    value: wei,
    denomination: types_1.AmountDenomination.Actual,
    reference: types_1.AmountReference.Delta,
};
var zeroGlob = { amount: {
        value: zero,
        denomination: types_1.AmountDenomination.Principal,
        reference: types_1.AmountReference.Delta,
    } };
var tradeId = new bignumber_js_1.default(1234);
describe('Trade', function () {
    var snapshotId;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r, defaultIndex;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    who1 = solo.getDefaultAccount();
                    who2 = accounts[5];
                    operator = accounts[6];
                    defaultGlob = {
                        primaryAccountOwner: who1,
                        primaryAccountId: accountNumber1,
                        otherAccountOwner: who2,
                        otherAccountId: accountNumber2,
                        inputMarketId: inputMkt,
                        outputMarketId: outputMkt,
                        autoTrader: solo.testing.autoTrader.getAddress(),
                        data: BytesHelper_1.toBytes(tradeId),
                        amount: {
                            value: negWei,
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
                    defaultIndex = {
                        lastUpdate: Constants_1.INTEGERS.ZERO,
                        borrow: wei.div(par),
                        supply: wei.div(par),
                    };
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setMarketIndex(inputMkt, defaultIndex),
                            solo.testing.setMarketIndex(outputMkt, defaultIndex),
                            solo.testing.setAccountBalance(who1, accountNumber1, collateralMkt, collateralAmount),
                            solo.testing.setAccountBalance(who2, accountNumber2, collateralMkt, collateralAmount),
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
    it('Basic trade test', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 2:
                    txResult = _a.sent();
                    console.log("\tTrade gas used: " + txResult.gasUsed);
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, negPar),
                            expectBalances2(negPar, par),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for events', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, inputIndex, outputIndex, collateralIndex, logs, operationLog, inputIndexLog, outputIndexLog, collateralIndexLog, tradeLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.permissions.approveOperator(operator, { from: who1 }),
                        approveTrader(),
                        setTradeData(),
                    ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectTradeOkay({}, { from: operator })];
                case 2:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(inputMkt),
                            solo.getters.getMarketCachedIndex(outputMkt),
                            solo.getters.getMarketCachedIndex(collateralMkt),
                            expectBalances1(par, negPar),
                            expectBalances2(negPar, par),
                        ])];
                case 3:
                    _a = _b.sent(), inputIndex = _a[0], outputIndex = _a[1], collateralIndex = _a[2];
                    logs = solo.logs.parseLogs(txResult);
                    expect(logs.length).toEqual(5);
                    operationLog = logs[0];
                    expect(operationLog.name).toEqual('LogOperation');
                    expect(operationLog.args.sender).toEqual(operator);
                    inputIndexLog = logs[1];
                    expect(inputIndexLog.name).toEqual('LogIndexUpdate');
                    expect(inputIndexLog.args.market).toEqual(inputMkt);
                    expect(inputIndexLog.args.index).toEqual(inputIndex);
                    outputIndexLog = logs[2];
                    expect(outputIndexLog.name).toEqual('LogIndexUpdate');
                    expect(outputIndexLog.args.market).toEqual(outputMkt);
                    expect(outputIndexLog.args.index).toEqual(outputIndex);
                    collateralIndexLog = logs[3];
                    expect(collateralIndexLog.name).toEqual('LogIndexUpdate');
                    expect(collateralIndexLog.args.market).toEqual(collateralMkt);
                    expect(collateralIndexLog.args.index).toEqual(collateralIndex);
                    tradeLog = logs[4];
                    expect(tradeLog.name).toEqual('LogTrade');
                    expect(tradeLog.args.takerAccountOwner).toEqual(who1);
                    expect(tradeLog.args.takerAccountNumber).toEqual(accountNumber1);
                    expect(tradeLog.args.makerAccountOwner).toEqual(who2);
                    expect(tradeLog.args.makerAccountNumber).toEqual(accountNumber2);
                    expect(tradeLog.args.inputMarket).toEqual(inputMkt);
                    expect(tradeLog.args.outputMarket).toEqual(outputMkt);
                    expect(tradeLog.args.takerInputUpdate).toEqual({ newPar: par, deltaWei: wei });
                    expect(tradeLog.args.takerOutputUpdate).toEqual({ newPar: negPar, deltaWei: negWei });
                    expect(tradeLog.args.makerInputUpdate).toEqual({ newPar: negPar, deltaWei: negWei });
                    expect(tradeLog.args.makerOutputUpdate).toEqual({ newPar: par, deltaWei: wei });
                    expect(tradeLog.args.autoTrader).toEqual(solo.testing.autoTrader.getAddress());
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for positive delta par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, approveTrader()];
                case 1:
                    _a.sent();
                    globs = [
                        { amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            } },
                        { amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            } },
                    ];
                    i = 0;
                    _a.label = 2;
                case 2:
                    if (!(i < globs.length)) return [3 /*break*/, 13];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 3:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar, zero),
                            expectBalances2(par, zero),
                        ])];
                case 5:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 6:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 7:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar, zero),
                            expectBalances2(par.times(2), par),
                        ])];
                case 8:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 9:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 10:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar, zero),
                            expectBalances2(zero, negPar),
                        ])];
                case 11:
                    _a.sent();
                    _a.label = 12;
                case 12:
                    i += 1;
                    return [3 /*break*/, 2];
                case 13:
                    i = 0;
                    _a.label = 14;
                case 14:
                    if (!(i < globs.length)) return [3 /*break*/, 25];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 15:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 16:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar),
                            expectBalances2(zero, par),
                        ])];
                case 17:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 18:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 19:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar),
                            expectBalances2(par, par.times(2)),
                        ])];
                case 20:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 21:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 22:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar),
                            expectBalances2(negPar, zero),
                        ])];
                case 23:
                    _a.sent();
                    _a.label = 24;
                case 24:
                    i += 1;
                    return [3 /*break*/, 14];
                case 25: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for negative delta par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, approveTrader()];
                case 1:
                    _a.sent();
                    globs = [
                        { amount: {
                                value: negPar,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            } },
                        { amount: {
                                value: negWei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            } },
                    ];
                    i = 0;
                    _a.label = 2;
                case 2:
                    if (!(i < globs.length)) return [3 /*break*/, 13];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 3:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, zero),
                            expectBalances2(negPar, zero),
                        ])];
                case 5:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 6:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 7:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, zero),
                            expectBalances2(zero, par),
                        ])];
                case 8:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 9:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 10:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, zero),
                            expectBalances2(negPar.times(2), negPar),
                        ])];
                case 11:
                    _a.sent();
                    _a.label = 12;
                case 12:
                    i += 1;
                    return [3 /*break*/, 2];
                case 13:
                    i = 0;
                    _a.label = 14;
                case 14:
                    if (!(i < globs.length)) return [3 /*break*/, 25];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 15:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 16:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par),
                            expectBalances2(zero, negPar),
                        ])];
                case 17:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 18:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 19:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par),
                            expectBalances2(par, zero),
                        ])];
                case 20:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 21:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 22:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par),
                            expectBalances2(negPar, negPar.times(2)),
                        ])];
                case 23:
                    _a.sent();
                    _a.label = 24;
                case 24:
                    i += 1;
                    return [3 /*break*/, 14];
                case 25: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for positive target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, approveTrader()];
                case 1:
                    _a.sent();
                    globs = [
                        { amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Target,
                            } },
                        { amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } },
                    ];
                    i = 0;
                    _a.label = 2;
                case 2:
                    if (!(i < globs.length)) return [3 /*break*/, 19];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 3:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar, zero),
                            expectBalances2(par, zero),
                        ])];
                case 5:
                    _a.sent();
                    // starting positive (<target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(par.div(2), par.div(2)),
                        ])];
                case 6:
                    // starting positive (<target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 7:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar.div(2), zero),
                            expectBalances2(par, par.div(2)),
                        ])];
                case 8:
                    _a.sent();
                    // starting positive (=target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 9:
                    // starting positive (=target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 10:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, zero),
                            expectBalances2(par, par),
                        ])];
                case 11:
                    _a.sent();
                    // starting positive (>target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(par.times(2), par.times(2)),
                        ])];
                case 12:
                    // starting positive (>target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 13:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, zero),
                            expectBalances2(par, par.times(2)),
                        ])];
                case 14:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 15:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 16:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar.times(2), zero),
                            expectBalances2(par, negPar),
                        ])];
                case 17:
                    _a.sent();
                    _a.label = 18;
                case 18:
                    i += 1;
                    return [3 /*break*/, 2];
                case 19:
                    i = 0;
                    _a.label = 20;
                case 20:
                    if (!(i < globs.length)) return [3 /*break*/, 37];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 21:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 22:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar),
                            expectBalances2(zero, par),
                        ])];
                case 23:
                    _a.sent();
                    // starting positive (<target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(par.div(2), par.div(2)),
                        ])];
                case 24:
                    // starting positive (<target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 25:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar.div(2)),
                            expectBalances2(par.div(2), par),
                        ])];
                case 26:
                    _a.sent();
                    // starting positive (=target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 27:
                    // starting positive (=target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 28:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, zero),
                            expectBalances2(par, par),
                        ])];
                case 29:
                    _a.sent();
                    // starting positive (>target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(par.times(2), par.times(2)),
                        ])];
                case 30:
                    // starting positive (>target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 31:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par),
                            expectBalances2(par.times(2), par),
                        ])];
                case 32:
                    _a.sent();
                    // starting negative
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 33:
                    // starting negative
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 34:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar.times(2)),
                            expectBalances2(negPar, par),
                        ])];
                case 35:
                    _a.sent();
                    _a.label = 36;
                case 36:
                    i += 1;
                    return [3 /*break*/, 20];
                case 37: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for negative target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, i, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, approveTrader()];
                case 1:
                    _a.sent();
                    globs = [
                        { amount: {
                                value: negPar,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Target,
                            } },
                        { amount: {
                                value: negWei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } },
                    ];
                    i = 0;
                    _a.label = 2;
                case 2:
                    if (!(i < globs.length)) return [3 /*break*/, 19];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 3:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, zero),
                            expectBalances2(negPar, zero),
                        ])];
                case 5:
                    _a.sent();
                    // starting negative (<target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar.div(2), negPar.div(2)),
                        ])];
                case 6:
                    // starting negative (<target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 7:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par.div(2), zero),
                            expectBalances2(negPar, negPar.div(2)),
                        ])];
                case 8:
                    _a.sent();
                    // starting negative (=target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 9:
                    // starting negative (=target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 10:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, zero),
                            expectBalances2(negPar, negPar),
                        ])];
                case 11:
                    _a.sent();
                    // starting negative (>target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar.times(2), negPar.times(2)),
                        ])];
                case 12:
                    // starting negative (>target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 13:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar, zero),
                            expectBalances2(negPar, negPar.times(2)),
                        ])];
                case 14:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, Promise.all([
                            setTradeData(zeroGlob.amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 15:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 16:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par.times(2), zero),
                            expectBalances2(negPar, par),
                        ])];
                case 17:
                    _a.sent();
                    _a.label = 18;
                case 18:
                    i += 1;
                    return [3 /*break*/, 2];
                case 19:
                    i = 0;
                    _a.label = 20;
                case 20:
                    if (!(i < globs.length)) return [3 /*break*/, 37];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(zero, zero),
                        ])];
                case 21:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 22:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par),
                            expectBalances2(zero, negPar),
                        ])];
                case 23:
                    _a.sent();
                    // starting negative (<target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar.div(2), negPar.div(2)),
                        ])];
                case 24:
                    // starting negative (<target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 25:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par.div(2)),
                            expectBalances2(negPar.div(2), negPar),
                        ])];
                case 26:
                    _a.sent();
                    // starting negative (=target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar, negPar),
                        ])];
                case 27:
                    // starting negative (=target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 28:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, zero),
                            expectBalances2(negPar, negPar),
                        ])];
                case 29:
                    _a.sent();
                    // starting negative (>target)
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(negPar.times(2), negPar.times(2)),
                        ])];
                case 30:
                    // starting negative (>target)
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 31:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar),
                            expectBalances2(negPar.times(2), negPar),
                        ])];
                case 32:
                    _a.sent();
                    // starting positive
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(zero, zero),
                            setBalances2(par, par),
                        ])];
                case 33:
                    // starting positive
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(zeroGlob)];
                case 34:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par.times(2)),
                            expectBalances2(par, negPar),
                        ])];
                case 35:
                    _a.sent();
                    _a.label = 36;
                case 36:
                    i += 1;
                    return [3 /*break*/, 20];
                case 37: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero target par/wei', function () { return __awaiter(_this, void 0, void 0, function () {
        var globs, start1, start2, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, approveTrader()];
                case 1:
                    _a.sent();
                    globs = [
                        { amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Target,
                            } },
                        { amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } },
                    ];
                    start1 = par.div(2);
                    start2 = negPar.div(2);
                    i = 0;
                    _a.label = 2;
                case 2:
                    if (!(i < globs.length)) return [3 /*break*/, 13];
                    // starting from zero
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(start1, start2),
                            setBalances2(zero, zero),
                        ])];
                case 3:
                    // starting from zero
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(start1, start2),
                            expectBalances2(zero, zero),
                        ])];
                case 5:
                    _a.sent();
                    // starting positive/negative
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(start1, start2),
                            setBalances2(par, negPar),
                        ])];
                case 6:
                    // starting positive/negative
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 7:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(start1.plus(par), start2.plus(negPar)),
                            expectBalances2(zero, zero),
                        ])];
                case 8:
                    _a.sent();
                    // starting negative/positive
                    return [4 /*yield*/, Promise.all([
                            setTradeData(globs[i].amount),
                            setBalances1(start1, start2),
                            setBalances2(negPar, par),
                        ])];
                case 9:
                    // starting negative/positive
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay(globs[i])];
                case 10:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(start1.plus(negPar), start2.plus(par)),
                            expectBalances2(zero, zero),
                        ])];
                case 11:
                    _a.sent();
                    _a.label = 12;
                case 12:
                    i += 1;
                    return [3 /*break*/, 2];
                case 13: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero input and output', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData({ value: zero }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, zero),
                            expectBalances2(zero, zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero input amount (positive output)', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData({ value: wei }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, negPar),
                            expectBalances2(zero, par),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero input amount (negative output)', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData({ value: negWei }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(zero, par),
                            expectBalances2(zero, negPar),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero output amount (positive input)', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData({ value: zero }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({
                            amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(negPar, zero),
                            expectBalances2(par, zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero output amount (negative input)', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData({ value: zero }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({
                            amount: {
                                value: negWei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, zero),
                            expectBalances2(negPar, zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds and sets status to Normal', function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, status1, status2;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountStatus(who1, accountNumber1, types_1.AccountStatus.Liquidating),
                        solo.testing.setAccountStatus(who2, accountNumber2, types_1.AccountStatus.Liquidating),
                        approveTrader(),
                        setTradeData(),
                    ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 2:
                    _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getAccountStatus(who1, accountNumber1),
                            solo.getters.getAccountStatus(who2, accountNumber2),
                        ])];
                case 3:
                    _a = _b.sent(), status1 = _a[0], status2 = _a[1];
                    expect(status1).toEqual(types_1.AccountStatus.Normal);
                    expect(status2).toEqual(types_1.AccountStatus.Normal);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for local operator sender', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        approveOperator(),
                        setTradeData(),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, negPar),
                            expectBalances2(negPar, par),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for global operator sender', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        solo.admin.setGlobalOperator(operator, true, { from: accounts[0] }),
                        setTradeData(),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectBalances1(par, negPar),
                            expectBalances2(negPar, par),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Verifies input market', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                        solo.testing.autoTrader.setRequireInputMarketId(outputMkt),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: input market mismatch')];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireInputMarketId(inputMkt)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 4:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Verifies output market', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                        solo.testing.autoTrader.setRequireOutputMarketId(inputMkt),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: output market mismatch')];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireOutputMarketId(outputMkt)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 4:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Verifies maker account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                        solo.testing.autoTrader.setRequireMakerAccount(who1, accountNumber2),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: maker account owner mismatch')];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireMakerAccount(who2, accountNumber1)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: maker account number mismatch')];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireMakerAccount(who2, accountNumber2)];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 6:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Verifies taker account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                        solo.testing.autoTrader.setRequireTakerAccount(who2, accountNumber1),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: taker account owner mismatch')];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireTakerAccount(who1, accountNumber2)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: taker account number mismatch')];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireTakerAccount(who1, accountNumber1)];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 6:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Verifies old input par', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                        setBalances2(par, zero),
                        solo.testing.autoTrader.setRequireOldInputPar(par.times(-1)),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: oldInputPar sign mismatch')];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireOldInputPar(par.times(2))];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: oldInputPar value mismatch')];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireOldInputPar(par)];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 6:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Verifies new input par', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                        solo.testing.autoTrader.setRequireNewInputPar(negPar.times(-1)),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: newInputPar sign mismatch')];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireNewInputPar(negPar.times(2))];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: newInputPar value mismatch')];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireNewInputPar(negPar)];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 6:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Verifies input wei', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                        solo.testing.autoTrader.setRequireInputWei(negWei.times(-1)),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: inputWei sign mismatch')];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireInputWei(negWei.times(2))];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'TestAutoTrader: inputWei value mismatch')];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.autoTrader.setRequireInputWei(negWei)];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, expectTradeOkay({})];
                case 6:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator sender', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData(),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'Storage: Unpermissioned operator', { from: operator })];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator autoTrader', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveOperator(),
                        setTradeData(),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'Storage: Unpermissioned operator')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for wrong-contract autoTrader', function () { return __awaiter(_this, void 0, void 0, function () {
        var otherContract;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    otherContract = solo.testing.exchangeWrapper.getAddress();
                    return [4 /*yield*/, solo.permissions.approveOperator(otherContract, { from: who1 })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({ autoTrader: otherContract })];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for one-sided trades', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        approveTrader(),
                        setTradeData({ value: negWei }),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectTradeRevert({}, 'OperationImpl: Trades cannot be one-sided')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails to trade to same account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectTradeRevert({
                        otherAccountOwner: who1,
                        otherAccountId: accountNumber1,
                    }, 'OperationImpl: Duplicate accounts in action')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function setBalances1(inputPar, outputPar) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, Promise.all([
                    solo.testing.setAccountBalance(who1, accountNumber1, inputMkt, inputPar),
                    solo.testing.setAccountBalance(who1, accountNumber1, outputMkt, outputPar),
                ])];
        });
    });
}
function setBalances2(inputPar, outputPar) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, Promise.all([
                    solo.testing.setAccountBalance(who2, accountNumber2, inputMkt, inputPar),
                    solo.testing.setAccountBalance(who2, accountNumber2, outputMkt, outputPar),
                ])];
        });
    });
}
function setTradeData(data) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedData;
        return __generator(this, function (_a) {
            combinedData = __assign({}, defaultData, data);
            return [2 /*return*/, solo.testing.autoTrader.setData(tradeId, combinedData)];
        });
    });
}
function expectBalances1(expectedInputPar, expectedOutputPar) {
    return __awaiter(this, void 0, void 0, function () {
        var balances;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getAccountBalances(who1, accountNumber1)];
                case 1:
                    balances = _a.sent();
                    expectBalances(balances, expectedInputPar, expectedOutputPar);
                    return [2 /*return*/];
            }
        });
    });
}
function expectBalances2(expectedInputPar, expectedOutputPar) {
    return __awaiter(this, void 0, void 0, function () {
        var balances;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getAccountBalances(who2, accountNumber2)];
                case 1:
                    balances = _a.sent();
                    expectBalances(balances, expectedInputPar, expectedOutputPar);
                    return [2 /*return*/];
            }
        });
    });
}
function expectBalances(balances, expectedInputPar, expectedOutputPar) {
    balances.forEach(function (balance, i) {
        if (i === inputMkt.toNumber()) {
            expect(balance.par).toEqual(expectedInputPar);
        }
        else if (i === outputMkt.toNumber()) {
            expect(balance.par).toEqual(expectedOutputPar);
        }
        else if (i === collateralMkt.toNumber()) {
            expect(balance.par).toEqual(collateralAmount);
        }
        else {
            expect(balance.par).toEqual(zero);
        }
    });
}
function approveTrader() {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.permissions.approveOperator(solo.testing.autoTrader.getAddress(), { from: who2 })];
        });
    });
}
function approveOperator() {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.permissions.approveOperator(operator, { from: who1 })];
        });
    });
}
function expectTradeOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob;
        return __generator(this, function (_a) {
            combinedGlob = __assign({}, defaultGlob, glob);
            return [2 /*return*/, solo.operation.initiate().trade(combinedGlob).commit(options)];
        });
    });
}
function expectTradeRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectTradeOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Trade.test.js.map