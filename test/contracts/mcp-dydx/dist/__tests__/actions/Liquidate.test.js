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
var liquidOwner;
var solidOwner;
var operator;
var solo;
var accounts;
var liquidAccountNumber = Constants_1.INTEGERS.ZERO;
var solidAccountNumber = Constants_1.INTEGERS.ONE;
var owedMarket = Constants_1.INTEGERS.ZERO;
var heldMarket = Constants_1.INTEGERS.ONE;
var otherMarket = new bignumber_js_1.default(2);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(10000);
var wei = new bignumber_js_1.default(15000);
var negPar = par.times(-1);
var negWei = wei.times(-1);
var collateralization = new bignumber_js_1.default('1.1');
var collatPar = par.times(collateralization);
var premium = new bignumber_js_1.default('1.05');
var remaining = collateralization.minus(premium);
var defaultGlob;
describe('Liquidate', function () {
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
                    liquidOwner = accounts[6];
                    operator = accounts[7];
                    defaultGlob = {
                        primaryAccountOwner: solidOwner,
                        primaryAccountId: solidAccountNumber,
                        liquidAccountOwner: liquidOwner,
                        liquidAccountId: liquidAccountNumber,
                        liquidMarketId: owedMarket,
                        payoutMarketId: heldMarket,
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
                    defaultIndex = {
                        lastUpdate: Constants_1.INTEGERS.ZERO,
                        borrow: wei.div(par),
                        supply: wei.div(par),
                    };
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setMarketIndex(owedMarket, defaultIndex),
                            solo.testing.setMarketIndex(heldMarket, defaultIndex),
                            solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, owedMarket, negPar),
                            solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, heldMarket, collatPar),
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
    it('Basic liquidate test', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateOkay({})];
                case 1:
                    txResult = _a.sent();
                    console.log("\tLiquidate gas used: " + txResult.gasUsed);
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(par.times(remaining), zero),
                        ])];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for events', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, heldIndex, owedIndex, logs, operationLog, owedIndexLog, heldIndexLog, liquidateLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(operator, { from: solidOwner })];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectLiquidateOkay({}, { from: operator })];
                case 2:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(heldMarket),
                            solo.getters.getMarketCachedIndex(owedMarket),
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(par.times(remaining), zero),
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
                    liquidateLog = logs[3];
                    expect(liquidateLog.name).toEqual('LogLiquidate');
                    expect(liquidateLog.args.solidAccountOwner).toEqual(solidOwner);
                    expect(liquidateLog.args.solidAccountNumber).toEqual(solidAccountNumber);
                    expect(liquidateLog.args.liquidAccountOwner).toEqual(liquidOwner);
                    expect(liquidateLog.args.liquidAccountNumber).toEqual(liquidAccountNumber);
                    expect(liquidateLog.args.heldMarket).toEqual(heldMarket);
                    expect(liquidateLog.args.owedMarket).toEqual(owedMarket);
                    expect(liquidateLog.args.solidHeldUpdate).toEqual({
                        newPar: par.times(premium),
                        deltaWei: wei.times(premium),
                    });
                    expect(liquidateLog.args.solidOwedUpdate).toEqual({
                        newPar: zero,
                        deltaWei: negWei,
                    });
                    expect(liquidateLog.args.liquidHeldUpdate).toEqual({
                        newPar: par.times(remaining),
                        deltaWei: negWei.times(premium),
                    });
                    expect(liquidateLog.args.liquidOwedUpdate).toEqual({
                        newPar: zero,
                        deltaWei: wei,
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds when partially liquidating', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateOkay({
                        amount: {
                            value: par.times(2),
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Delta,
                        },
                    })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(par.times(remaining), zero),
                        ])];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds when bound by owedToken', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateOkay({
                        amount: {
                            value: par.times(2),
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Delta,
                        },
                    })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(par.times(remaining), zero),
                        ])];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds when bound by heldToken', function () { return __awaiter(_this, void 0, void 0, function () {
        var amount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    amount = par.times(premium).div(2);
                    return [4 /*yield*/, solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, heldMarket, amount)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium).div(2), par.div(2)),
                            expectLiquidPars(zero, negPar.div(2)),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for solid account that takes on a negative balance', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.testing.setAccountBalance(solidOwner, solidAccountNumber, owedMarket, par.div(2))];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), negPar.div(2)),
                            expectLiquidPars(par.times(remaining), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for liquidating twice', function () { return __awaiter(_this, void 0, void 0, function () {
        var amount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    amount = par.times(2);
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, heldMarket, amount),
                            solo.testing.setAccountStatus(liquidOwner, liquidAccountNumber, types_1.AccountStatus.Liquidating),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, solo.operation.initiate()
                            .liquidate(__assign({}, defaultGlob, { amount: {
                                value: negPar.div(5),
                                reference: types_1.AmountReference.Target,
                                denomination: types_1.AmountDenomination.Principal,
                            } }))
                            .liquidate(defaultGlob)
                            .commit()];
                case 2: return [4 /*yield*/, _a.sent()];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(amount.minus(par.times(premium)), zero),
                        ])];
                case 4:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for account already marked with liquidating flag', function () { return __awaiter(_this, void 0, void 0, function () {
        var amount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    amount = par.times(2);
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, heldMarket, amount),
                            solo.testing.setAccountStatus(liquidOwner, liquidAccountNumber, types_1.AccountStatus.Liquidating),
                        ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(amount.minus(par.times(premium)), zero),
                        ])];
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
                case 0: return [4 /*yield*/, solo.testing.setAccountStatus(solidOwner, solidAccountNumber, types_1.AccountStatus.Liquidating)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateOkay({})];
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
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(operator, { from: solidOwner })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(par.times(remaining), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for global operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.admin.setGlobalOperator(operator, true, { from: accounts[0] })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateOkay({}, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(par.times(premium), zero),
                            expectLiquidPars(par.times(remaining), zero),
                        ])];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds (without effect) for zero collateral', function () { return __awaiter(_this, void 0, void 0, function () {
        var totalOtherPar;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateOkay({
                        payoutMarketId: otherMarket,
                    })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(zero, par),
                            expectLiquidPars(collatPar, negPar),
                        ])];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketWithInfo(otherMarket)];
                case 3:
                    totalOtherPar = _a.sent();
                    expect(totalOtherPar.market.totalPar.supply).toEqual(zero);
                    expect(totalOtherPar.market.totalPar.borrow).toEqual(zero);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds (without effect) for zero borrow', function () { return __awaiter(_this, void 0, void 0, function () {
        var totalOtherPar;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateOkay({
                        liquidMarketId: otherMarket,
                    })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            expectSolidPars(zero, par),
                            expectLiquidPars(collatPar, negPar),
                        ])];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketWithInfo(otherMarket)];
                case 3:
                    totalOtherPar = _a.sent();
                    expect(totalOtherPar.market.totalPar.supply).toEqual(zero);
                    expect(totalOtherPar.market.totalPar.borrow).toEqual(zero);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for over-collateralized account', function () { return __awaiter(_this, void 0, void 0, function () {
        var amount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    amount = par.times(2);
                    return [4 /*yield*/, solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, heldMarket, amount)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateRevert({}, 'OperationImpl: Unliquidatable account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateRevert({}, 'Storage: Unpermissioned operator', { from: operator })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if liquidating after account used as primary', function () { return __awaiter(_this, void 0, void 0, function () {
        var operation;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(solidOwner, { from: liquidOwner })];
                case 1:
                    _a.sent();
                    operation = solo.operation.initiate();
                    operation.deposit({
                        primaryAccountOwner: liquidOwner,
                        primaryAccountId: liquidAccountNumber,
                        marketId: owedMarket,
                        from: liquidOwner,
                        amount: {
                            value: par.div(2),
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Delta,
                        },
                    });
                    operation.liquidate(defaultGlob);
                    return [4 /*yield*/, Expect_1.expectThrow(operation.commit(), 'OperationImpl: Requires non-primary account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails if liquidating totally zero account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateRevert({
                        liquidAccountOwner: liquidOwner,
                        liquidAccountId: solidAccountNumber,
                    }, 'OperationImpl: Unliquidatable account')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for repeated market', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateRevert({ payoutMarketId: owedMarket }, 'OperationImpl: Duplicate markets in action')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for negative collateral', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, heldMarket, negPar)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateRevert({}, 'OperationImpl: Collateral cannot be negative')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for paying back market that is already positive', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, owedMarket, collatPar),
                        solo.testing.setAccountBalance(liquidOwner, liquidAccountNumber, heldMarket, negPar),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectLiquidateRevert({
                            payoutMarketId: otherMarket,
                        }, 'Storage: Owed balance cannot be positive')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for a negative delta', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateRevert({
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
    it('Fails to liquidate the same account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectLiquidateRevert({
                        liquidAccountOwner: solidOwner,
                        liquidAccountId: solidAccountNumber,
                    }, 'OperationImpl: Duplicate accounts in action')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function expectLiquidationFlagSet() {
    return __awaiter(this, void 0, void 0, function () {
        var status;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getAccountStatus(liquidOwner, liquidAccountNumber)];
                case 1:
                    status = _a.sent();
                    expect(status).toEqual(types_1.AccountStatus.Liquidating);
                    return [2 /*return*/];
            }
        });
    });
}
function expectLiquidateOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob, txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    combinedGlob = __assign({}, defaultGlob, glob);
                    return [4 /*yield*/, solo.operation.initiate().liquidate(combinedGlob).commit(options)];
                case 1:
                    txResult = _a.sent();
                    return [4 /*yield*/, expectLiquidationFlagSet()];
                case 2:
                    _a.sent();
                    return [2 /*return*/, txResult];
            }
        });
    });
}
function expectLiquidateRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectLiquidateOkay(glob, options), reason)];
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
function expectLiquidPars(expectedHeldPar, expectedOwedPar) {
    return __awaiter(this, void 0, void 0, function () {
        var balances;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getAccountBalances(liquidOwner, liquidAccountNumber)];
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
//# sourceMappingURL=Liquidate.test.js.map