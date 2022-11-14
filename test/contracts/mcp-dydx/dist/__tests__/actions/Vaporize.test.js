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
var exchange_wrappers_1 = require("@dydxprotocol/exchange-wrappers");
var Solo_1 = require("../helpers/Solo");
var EVM_1 = require("../helpers/EVM");
var SoloHelpers_1 = require("../helpers/SoloHelpers");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var types_1 = require("../../src/types");
var vaporOwner;
var solidOwner;
var operator;
var solo;
var accounts;
var vaporAccountNumber = Constants_1.INTEGERS.ZERO;
var solidAccountNumber = Constants_1.INTEGERS.ONE;
var owedMarket = Constants_1.INTEGERS.ZERO;
var heldMarket = Constants_1.INTEGERS.ONE;
var otherMarket = new bignumber_js_1.default(2);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(10000);
var wei = new bignumber_js_1.default(15000);
var negPar = par.times(-1);
var negWei = wei.times(-1);
var premium = new bignumber_js_1.default('1.05');
var testOrder = {
    type: exchange_wrappers_1.OrderType.Test,
    exchangeWrapperAddress: Constants_1.ADDRESSES.ZERO,
    originator: Constants_1.ADDRESSES.ZERO,
    makerToken: Constants_1.ADDRESSES.ZERO,
    takerToken: Constants_1.ADDRESSES.ZERO,
    makerAmount: Constants_1.INTEGERS.ZERO,
    takerAmount: Constants_1.INTEGERS.ZERO,
    allegedTakerAmount: Constants_1.INTEGERS.ZERO,
    desiredMakerAmount: Constants_1.INTEGERS.ZERO,
};
var defaultAmount = {
    value: zero,
    denomination: types_1.AmountDenomination.Principal,
    reference: types_1.AmountReference.Target,
};
var defaultGlob;
describe('Vaporize', function () {
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
                    solidOwner = solo.getDefaultAccount();
                    vaporOwner = accounts[6];
                    operator = accounts[7];
                    defaultGlob = {
                        primaryAccountOwner: solidOwner,
                        primaryAccountId: solidAccountNumber,
                        vaporAccountOwner: vaporOwner,
                        vaporAccountId: vaporAccountNumber,
                        vaporMarketId: owedMarket,
                        payoutMarketId: heldMarket,
                        amount: defaultAmount,
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
                            solo.testing.setMarketIndex(owedMarket, defaultIndex),
                            solo.testing.setMarketIndex(heldMarket, defaultIndex),
                            solo.testing.setAccountBalance(vaporOwner, vaporAccountNumber, owedMarket, negPar),
                            solo.testing.setAccountBalance(solidOwner, solidAccountNumber, owedMarket, par),
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
    it('Basic vaporize test', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, issueHeldTokensToSolo(wei.times(premium))];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectExcessHeldToken(wei.times(premium))];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 3:
                    txResult = _a.sent();
                    console.log("\tVaporize gas used: " + txResult.gasUsed);
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), zero),
                        ])];
                case 4:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for events', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, heldIndex, owedIndex, logs, operationLog, owedIndexLog, heldIndexLog, vaporizeLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueHeldTokensToSolo(wei.times(premium)),
                        solo.permissions.approveOperator(operator, { from: solidOwner }),
                    ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectVaporizeOkay({}, { from: operator })];
                case 2:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(heldMarket),
                            solo.getters.getMarketCachedIndex(owedMarket),
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), zero),
                        ])];
                case 3:
                    _a = _b.sent(), heldIndex = _a[0], owedIndex = _a[1];
                    logs = solo.logs.parseLogs(txResult);
                    expect(logs.length).toEqual(4);
                    operationLog = logs[0];
                    expect(operationLog.name).toEqual('LogOperation');
                    expect(operationLog.args.sender).toEqual(operator);
                    owedIndexLog = logs[1];
                    expect(owedIndexLog.name).toEqual('LogIndexUpdate');
                    expect(owedIndexLog.args.market).toEqual(owedMarket);
                    expect(owedIndexLog.args.index).toEqual(owedIndex);
                    heldIndexLog = logs[2];
                    expect(heldIndexLog.name).toEqual('LogIndexUpdate');
                    expect(heldIndexLog.args.market).toEqual(heldMarket);
                    expect(heldIndexLog.args.index).toEqual(heldIndex);
                    vaporizeLog = logs[3];
                    expect(vaporizeLog.name).toEqual('LogVaporize');
                    expect(vaporizeLog.args.solidAccountOwner).toEqual(solidOwner);
                    expect(vaporizeLog.args.solidAccountNumber).toEqual(solidAccountNumber);
                    expect(vaporizeLog.args.vaporAccountOwner).toEqual(vaporOwner);
                    expect(vaporizeLog.args.vaporAccountNumber).toEqual(vaporAccountNumber);
                    expect(vaporizeLog.args.heldMarket).toEqual(heldMarket);
                    expect(vaporizeLog.args.owedMarket).toEqual(owedMarket);
                    expect(vaporizeLog.args.solidHeldUpdate).toEqual({
                        newPar: par.times(premium),
                        deltaWei: wei.times(premium),
                    });
                    expect(vaporizeLog.args.solidOwedUpdate).toEqual({
                        newPar: zero,
                        deltaWei: negWei,
                    });
                    expect(vaporizeLog.args.vaporOwedUpdate).toEqual({
                        newPar: zero,
                        deltaWei: wei,
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for unvaporizable account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.testing.setAccountBalance(vaporOwner, vaporAccountNumber, heldMarket, par)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeRevert({}, 'OperationImpl: Unvaporizable account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds if enough excess owedTokens', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, heldIndex, owedIndex, logs, operationLog, owedIndexLog, heldIndexLog, vaporizeLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, issueOwedTokensToSolo(wei)];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectExcessOwedToken(wei)];
                case 2:
                    _b.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 3:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(heldMarket),
                            solo.getters.getMarketCachedIndex(owedMarket),
                            expectExcessOwedToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(zero, par),
                        ])];
                case 4:
                    _a = _b.sent(), heldIndex = _a[0], owedIndex = _a[1];
                    logs = solo.logs.parseLogs(txResult);
                    expect(logs.length).toEqual(4);
                    operationLog = logs[0];
                    expect(operationLog.name).toEqual('LogOperation');
                    expect(operationLog.args.sender).toEqual(solidOwner);
                    owedIndexLog = logs[1];
                    expect(owedIndexLog.name).toEqual('LogIndexUpdate');
                    expect(owedIndexLog.args.market).toEqual(owedMarket);
                    expect(owedIndexLog.args.index).toEqual(owedIndex);
                    heldIndexLog = logs[2];
                    expect(heldIndexLog.name).toEqual('LogIndexUpdate');
                    expect(heldIndexLog.args.market).toEqual(heldMarket);
                    expect(heldIndexLog.args.index).toEqual(heldIndex);
                    vaporizeLog = logs[3];
                    expect(vaporizeLog.name).toEqual('LogVaporize');
                    expect(vaporizeLog.args.solidAccountOwner).toEqual(solidOwner);
                    expect(vaporizeLog.args.solidAccountNumber).toEqual(solidAccountNumber);
                    expect(vaporizeLog.args.vaporAccountOwner).toEqual(vaporOwner);
                    expect(vaporizeLog.args.vaporAccountNumber).toEqual(vaporAccountNumber);
                    expect(vaporizeLog.args.heldMarket).toEqual(heldMarket);
                    expect(vaporizeLog.args.owedMarket).toEqual(owedMarket);
                    expect(vaporizeLog.args.solidHeldUpdate).toEqual({
                        newPar: zero,
                        deltaWei: zero,
                    });
                    expect(vaporizeLog.args.solidOwedUpdate).toEqual({
                        newPar: par,
                        deltaWei: zero,
                    });
                    expect(vaporizeLog.args.vaporOwedUpdate).toEqual({
                        newPar: zero,
                        deltaWei: wei,
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds if half excess owedTokens', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount, txResult, _a, heldIndex, owedIndex, logs, operationLog, owedIndexLog, heldIndexLog, vaporizeLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, Promise.all([
                            issueHeldTokensToSolo(payoutAmount),
                            issueOwedTokensToSolo(wei.div(2)),
                        ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 2:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(heldMarket),
                            solo.getters.getMarketCachedIndex(owedMarket),
                            expectExcessHeldToken(payoutAmount.div(2)),
                            expectExcessOwedToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium).div(2), par.div(2)),
                        ])];
                case 3:
                    _a = _b.sent(), heldIndex = _a[0], owedIndex = _a[1];
                    logs = solo.logs.parseLogs(txResult);
                    expect(logs.length).toEqual(4);
                    operationLog = logs[0];
                    expect(operationLog.name).toEqual('LogOperation');
                    expect(operationLog.args.sender).toEqual(solidOwner);
                    owedIndexLog = logs[1];
                    expect(owedIndexLog.name).toEqual('LogIndexUpdate');
                    expect(owedIndexLog.args.market).toEqual(owedMarket);
                    expect(owedIndexLog.args.index).toEqual(owedIndex);
                    heldIndexLog = logs[2];
                    expect(heldIndexLog.name).toEqual('LogIndexUpdate');
                    expect(heldIndexLog.args.market).toEqual(heldMarket);
                    expect(heldIndexLog.args.index).toEqual(heldIndex);
                    vaporizeLog = logs[3];
                    expect(vaporizeLog.name).toEqual('LogVaporize');
                    expect(vaporizeLog.args.solidAccountOwner).toEqual(solidOwner);
                    expect(vaporizeLog.args.solidAccountNumber).toEqual(solidAccountNumber);
                    expect(vaporizeLog.args.vaporAccountOwner).toEqual(vaporOwner);
                    expect(vaporizeLog.args.vaporAccountNumber).toEqual(vaporAccountNumber);
                    expect(vaporizeLog.args.heldMarket).toEqual(heldMarket);
                    expect(vaporizeLog.args.owedMarket).toEqual(owedMarket);
                    expect(vaporizeLog.args.solidHeldUpdate).toEqual({
                        newPar: par.times(premium).div(2),
                        deltaWei: wei.times(premium).div(2),
                    });
                    expect(vaporizeLog.args.solidOwedUpdate).toEqual({
                        newPar: par.div(2),
                        deltaWei: negWei.div(2),
                    });
                    expect(vaporizeLog.args.vaporOwedUpdate).toEqual({
                        newPar: zero,
                        deltaWei: wei,
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds when bound by owedToken', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, issueHeldTokensToSolo(payoutAmount.times(2))];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({
                            amount: {
                                value: par.times(2),
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(payoutAmount),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds when bound by heldToken', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium).div(2);
                    return [4 /*yield*/, issueHeldTokensToSolo(payoutAmount)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, negPar.div(2)),
                            expectSolidPars(par.times(premium).div(2), par.div(2)),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for account already marked with liquidating flag', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, Promise.all([
                            issueHeldTokensToSolo(payoutAmount),
                            solo.testing.setAccountStatus(vaporOwner, vaporAccountNumber, types_1.AccountStatus.Liquidating),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for account already marked with vaporizing flag', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, Promise.all([
                            issueHeldTokensToSolo(payoutAmount),
                            solo.testing.setAccountStatus(vaporOwner, vaporAccountNumber, types_1.AccountStatus.Vaporizing),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for solid account that takes on a negative balance', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, Promise.all([
                            issueHeldTokensToSolo(payoutAmount),
                            solo.testing.setAccountBalance(solidOwner, solidAccountNumber, owedMarket, par.div(2)),
                            // need another positive balance so there is zero (or negative) excess owedToken
                            solo.testing.setAccountBalance(operator, solidAccountNumber, owedMarket, par),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), negPar.div(2)),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds and sets status to Normal', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount, status;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, Promise.all([
                            issueHeldTokensToSolo(payoutAmount),
                            solo.testing.setAccountStatus(solidOwner, solidAccountNumber, types_1.AccountStatus.Liquidating),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getAccountStatus(solidOwner, solidAccountNumber)];
                case 3:
                    status = _a.sent();
                    expect(status).toEqual(types_1.AccountStatus.Normal);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for local operator', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, Promise.all([
                            issueHeldTokensToSolo(payoutAmount),
                            solo.permissions.approveOperator(operator, { from: solidOwner }),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for global operator', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, Promise.all([
                            issueHeldTokensToSolo(payoutAmount),
                            solo.admin.setGlobalOperator(operator, true, { from: accounts[0] }),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, zero),
                            expectSolidPars(par.times(premium), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds (without effect) for zero excess', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectExcessHeldToken(zero)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(zero),
                            expectVaporPars(zero, negPar),
                            expectSolidPars(zero, par),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds (without effect) for zero borrow', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, issueHeldTokensToSolo(payoutAmount)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeOkay({
                            vaporMarketId: otherMarket,
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectExcessHeldToken(payoutAmount),
                            expectVaporPars(zero, negPar),
                            expectSolidPars(zero, par),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator', function () { return __awaiter(_this, void 0, void 0, function () {
        var payoutAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    payoutAmount = wei.times(premium);
                    return [4 /*yield*/, issueHeldTokensToSolo(payoutAmount)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeRevert({}, 'Storage: Unpermissioned operator', { from: operator })];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to deposit', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.deposit({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        marketId: owedMarket,
                        from: vaporOwner,
                        amount: defaultAmount,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to withdraw', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.withdraw({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        marketId: owedMarket,
                        to: vaporOwner,
                        amount: defaultAmount,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to buy', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.buy({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        makerMarketId: owedMarket,
                        takerMarketId: heldMarket,
                        amount: defaultAmount,
                        order: testOrder,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to sell', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.sell({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        makerMarketId: owedMarket,
                        takerMarketId: heldMarket,
                        amount: defaultAmount,
                        order: testOrder,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to transfer (account one)', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.transfer({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        marketId: owedMarket,
                        toAccountOwner: solidOwner,
                        toAccountId: solidAccountNumber,
                        amount: defaultAmount,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to transfer (account two)', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.transfer({
                        primaryAccountOwner: solidOwner,
                        primaryAccountId: solidAccountNumber,
                        marketId: owedMarket,
                        toAccountOwner: vaporOwner,
                        toAccountId: vaporAccountNumber,
                        amount: defaultAmount,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to trade (account one)', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.trade({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        otherAccountOwner: solidOwner,
                        otherAccountId: solidAccountNumber,
                        inputMarketId: owedMarket,
                        outputMarketId: heldMarket,
                        autoTrader: Constants_1.ADDRESSES.ZERO,
                        data: [],
                        amount: defaultAmount,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to trade (account two)', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.trade({
                        primaryAccountOwner: solidOwner,
                        primaryAccountId: solidAccountNumber,
                        otherAccountOwner: vaporOwner,
                        otherAccountId: vaporAccountNumber,
                        inputMarketId: owedMarket,
                        outputMarketId: heldMarket,
                        autoTrader: Constants_1.ADDRESSES.ZERO,
                        data: [],
                        amount: defaultAmount,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to liquidate', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.liquidate({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        liquidAccountOwner: Constants_1.ADDRESSES.ZERO,
                        liquidAccountId: Constants_1.INTEGERS.ZERO,
                        liquidMarketId: owedMarket,
                        payoutMarketId: heldMarket,
                        amount: defaultAmount,
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to vaporize', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.vaporize(__assign({}, defaultGlob, { primaryAccountOwner: vaporOwner, primaryAccountId: vaporAccountNumber, vaporAccountOwner: Constants_1.ADDRESSES.ZERO, vaporAccountId: Constants_1.INTEGERS.ZERO }));
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing after account used to call', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: vaporOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.call({
                        primaryAccountOwner: vaporOwner,
                        primaryAccountId: vaporAccountNumber,
                        callee: Constants_1.ADDRESSES.ZERO,
                        data: [],
                    });
                    operation.vaporize(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if vaporizing totally zero account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectVaporizeRevert({
                        vaporAccountOwner: operator,
                        vaporAccountId: zero,
                    }, 'OperationImpl: Unvaporizable account')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for repeated market', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectVaporizeRevert({ payoutMarketId: owedMarket }, 'OperationImpl: Duplicate markets in action')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for negative excess heldTokens', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.testing.setAccountBalance(solidOwner, solidAccountNumber, heldMarket, par)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectExcessHeldToken(negWei)];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, expectVaporizeRevert({}, 'OperationImpl: Excess cannot be negative')];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for a negative delta', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectVaporizeRevert({
                        amount: {
                            value: negPar.times(2),
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Target,
                        },
                    }, 'Storage: Owed balance cannot increase')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails to vaporize the same account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectVaporizeRevert({
                        vaporAccountOwner: solidOwner,
                        vaporAccountId: solidAccountNumber,
                    }, 'OperationImpl: Duplicate accounts in action')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function issueHeldTokensToSolo(amount) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.testing.tokenB.issueTo(amount, solo.contracts.soloMargin.options.address)];
        });
    });
}
function issueOwedTokensToSolo(amount) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.testing.tokenA.issueTo(amount, solo.contracts.soloMargin.options.address)];
        });
    });
}
function expectVaporizeOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob;
        return __generator(this, function (_a) {
            combinedGlob = __assign({}, defaultGlob, glob);
            return [2 /*return*/, solo.operation.initiate().vaporize(combinedGlob).commit(options)];
        });
    });
}
function expectVaporizeRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectVaporizeOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function expectSolidPars(expectedHeldPar, expectedOwedPar) {
    return __awaiter(this, void 0, void 0, function () {
        var balances;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getAccountBalances(solidOwner, solidAccountNumber)];
                case 1:
                    balances = _a.sent();
                    balances.forEach(function (balance, i) {
                        if (i === heldMarket.toNumber()) {
                            expect(balance.par).toEqual(expectedHeldPar);
                        }
                        else if (i === owedMarket.toNumber()) {
                            expect(balance.par).toEqual(expectedOwedPar);
                        }
                        else {
                            expect(balance.par).toEqual(zero);
                        }
                    });
                    return [2 /*return*/];
            }
        });
    });
}
function expectVaporPars(expectedHeldPar, expectedOwedPar) {
    return __awaiter(this, void 0, void 0, function () {
        var balances;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getAccountBalances(vaporOwner, vaporAccountNumber)];
                case 1:
                    balances = _a.sent();
                    balances.forEach(function (balance, i) {
                        if (i === heldMarket.toNumber()) {
                            expect(balance.par).toEqual(expectedHeldPar);
                        }
                        else if (i === owedMarket.toNumber()) {
                            expect(balance.par).toEqual(expectedOwedPar);
                        }
                        else {
                            expect(balance.par).toEqual(zero);
                        }
                    });
                    return [2 /*return*/];
            }
        });
    });
}
function expectExcessHeldToken(expected) {
    return __awaiter(this, void 0, void 0, function () {
        var actual;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getNumExcessTokens(heldMarket)];
                case 1:
                    actual = _a.sent();
                    expect(actual).toEqual(expected);
                    return [2 /*return*/];
            }
        });
    });
}
function expectExcessOwedToken(expected) {
    return __awaiter(this, void 0, void 0, function () {
        var actual;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getNumExcessTokens(owedMarket)];
                case 1:
                    actual = _a.sent();
                    expect(actual).toEqual(expected);
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Vaporize.test.js.map