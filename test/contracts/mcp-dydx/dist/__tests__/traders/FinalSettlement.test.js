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
var BytesHelper_1 = require("../../src/lib/BytesHelper");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var types_1 = require("../../src/types");
var solo;
var accounts;
var snapshotId;
var admin;
var owner1;
var owner2;
var accountNumber1 = Constants_1.INTEGERS.ZERO;
var accountNumber2 = Constants_1.INTEGERS.ONE;
var heldMarket = Constants_1.INTEGERS.ZERO;
var owedMarket = Constants_1.INTEGERS.ONE;
var collateralMarket = new bignumber_js_1.default(2);
var par = new bignumber_js_1.default(10000);
var zero = new bignumber_js_1.default(0);
var premium = new bignumber_js_1.default('1.05');
var defaultPrice = new bignumber_js_1.default('1e40');
var defaultGlob;
var heldGlob;
describe('FinalSettlement', function () {
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
                    owner1 = accounts[2];
                    owner2 = accounts[3];
                    defaultGlob = {
                        primaryAccountOwner: owner1,
                        primaryAccountId: accountNumber1,
                        otherAccountOwner: owner2,
                        otherAccountId: accountNumber2,
                        inputMarketId: owedMarket,
                        outputMarketId: heldMarket,
                        autoTrader: solo.contracts.finalSettlement.options.address,
                        amount: {
                            value: zero,
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Target,
                        },
                        data: BytesHelper_1.toBytes(owedMarket),
                    };
                    heldGlob = {
                        primaryAccountOwner: owner1,
                        primaryAccountId: accountNumber1,
                        otherAccountOwner: owner2,
                        otherAccountId: accountNumber2,
                        inputMarketId: heldMarket,
                        outputMarketId: owedMarket,
                        autoTrader: solo.contracts.finalSettlement.options.address,
                        amount: {
                            value: zero,
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Target,
                        },
                        data: BytesHelper_1.toBytes(owedMarket),
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par.times(-1)),
                            solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par.times(2)),
                            solo.testing.setAccountBalance(owner1, accountNumber1, owedMarket, par),
                            solo.testing.setAccountBalance(owner2, accountNumber2, collateralMarket, par.times(4)),
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
    describe('#getRampTime', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        _a = expect;
                        return [4 /*yield*/, solo.finalSettlement.getRampTime()];
                    case 1:
                        _a.apply(void 0, [_b.sent()]).toEqual(new bignumber_js_1.default(60 * 60 * 24 * 28));
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#initialize', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var startTime, timestamp, _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _c.sent();
                        return [4 /*yield*/, solo.finalSettlement.getStartTime()];
                    case 2:
                        startTime = _c.sent();
                        _b = (_a = solo.web3.eth).getBlock;
                        return [4 /*yield*/, solo.web3.eth.getBlockNumber()];
                    case 3: return [4 /*yield*/, _b.apply(_a, [_c.sent()])];
                    case 4:
                        timestamp = (_c.sent()).timestamp;
                        expect(startTime).toEqual(new bignumber_js_1.default(timestamp));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if already initialized', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.finalSettlement.initialize(), 'FinalSettlement: Already initialized')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if not a global operator', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.admin.setGlobalOperator(solo.contracts.finalSettlement.options.address, false, { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.finalSettlement.initialize(), 'FinalSettlement: Not a global operator')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('settle account (heldAmount)', function () {
        beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.fastForward(60 * 60 * 24 * 28)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in settling', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, _a, held1, owed1, held2, owed2, logs, settleLog;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectSettlementOkay(heldGlob)];
                    case 1:
                        txResult = _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                                solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                            ])];
                    case 2:
                        _a = _b.sent(), held1 = _a[0], owed1 = _a[1], held2 = _a[2], owed2 = _a[3];
                        expect(owed1).toEqual(par.minus(par.div(premium)).integerValue(bignumber_js_1.default.ROUND_DOWN));
                        expect(owed2).toEqual(owed1.times(-1));
                        expect(held1).toEqual(par);
                        expect(held2).toEqual(zero);
                        logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                        expect(logs.length).toEqual(1);
                        settleLog = logs[0];
                        expect(settleLog.name).toEqual('Settlement');
                        expect(settleLog.args.makerAddress).toEqual(owner2);
                        expect(settleLog.args.takerAddress).toEqual(owner1);
                        expect(settleLog.args.heldMarketId).toEqual(heldMarket);
                        expect(settleLog.args.owedMarketId).toEqual(owedMarket);
                        expect(settleLog.args.heldWei).toEqual(par);
                        expect(settleLog.args.owedWei).toEqual(par.div(premium).integerValue(bignumber_js_1.default.ROUND_UP));
                        console.log("\tFinalSettlement (held) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in settling part of a position', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectSettlementOkay(__assign({}, heldGlob, { amount: {
                                value: par.div(2),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } }))];
                    case 1:
                        _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                                solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                            ])];
                    case 2:
                        _a = _b.sent(), held1 = _a[0], owed1 = _a[1], held2 = _a[2], owed2 = _a[3];
                        expect(owed1).toEqual(par.minus(par.div(premium).div(2)).integerValue(bignumber_js_1.default.ROUND_DOWN));
                        expect(owed2).toEqual(owed1.times(-1));
                        expect(held1).toEqual(par.div(2));
                        expect(held2).toEqual(par.minus(held1));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in settling including premiums', function () { return __awaiter(_this, void 0, void 0, function () {
            var owedPremium, heldPremium, adjustedPremium, _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        owedPremium = new bignumber_js_1.default('0.5');
                        heldPremium = new bignumber_js_1.default('1.0');
                        adjustedPremium = premium.minus(1).times(owedPremium.plus(1)).times(heldPremium.plus(1)).plus(1);
                        return [4 /*yield*/, Promise.all([
                                solo.admin.setSpreadPremium(owedMarket, owedPremium, { from: admin }),
                                solo.admin.setSpreadPremium(heldMarket, heldPremium, { from: admin }),
                            ])];
                    case 1:
                        _b.sent();
                        return [4 /*yield*/, expectSettlementOkay(heldGlob)];
                    case 2:
                        _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                                solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                            ])];
                    case 3:
                        _a = _b.sent(), held1 = _a[0], owed1 = _a[1], held2 = _a[2], owed2 = _a[3];
                        expect(owed1).toEqual(par.minus(par.div(adjustedPremium)).integerValue(bignumber_js_1.default.ROUND_DOWN));
                        expect(owed2).toEqual(owed1.times(-1));
                        expect(held1).toEqual(par);
                        expect(held2).toEqual(zero);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for zero inputMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            var getAllBalances, start, end;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        getAllBalances = [
                            solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                            solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                            solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                            solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                        ];
                        return [4 /*yield*/, Promise.all(getAllBalances)];
                    case 1:
                        start = _a.sent();
                        return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, zero)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementOkay(heldGlob, {}, false)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, Promise.all(getAllBalances)];
                    case 4:
                        end = _a.sent();
                        expect(start).toEqual(end);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for negative inputMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par.times(-1))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert(heldGlob, 'FinalSettlement: inputMarket mismatch')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for overusing collateral', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectSettlementRevert(__assign({}, heldGlob, { amount: {
                                value: par.times(-1),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } }), 'FinalSettlement: Collateral cannot be overused')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for increasing the heldAmount', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectSettlementRevert(__assign({}, heldGlob, { amount: {
                                value: par.times(4),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } }), 'FinalSettlement: inputMarket mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if not initialized', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, EVM_1.resetEVM(snapshotId)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert(heldGlob, 'FinalSettlement: Contract must be initialized')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for invalid trade data', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectSettlementRevert(__assign({}, heldGlob, { data: [] }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for zero owedMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, zero)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert(heldGlob, 'FinalSettlement: Borrows must be negative')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for positive owedMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert(heldGlob, 'FinalSettlement: Borrows must be negative')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for over-repaying the borrow', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par.times(2))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert(heldGlob, 'FinalSettlement: outputMarket too small')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('settle account (owedAmount)', function () {
        beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.fastForward(60 * 60 * 24 * 28)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in settling', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, _a, held1, owed1, held2, owed2, logs, settleLog;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectSettlementOkay({})];
                    case 1:
                        txResult = _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                                solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                            ])];
                    case 2:
                        _a = _b.sent(), held1 = _a[0], owed1 = _a[1], held2 = _a[2], owed2 = _a[3];
                        expect(owed1).toEqual(zero);
                        expect(owed2).toEqual(zero);
                        expect(held1).toEqual(par.times(premium));
                        expect(held2).toEqual(par.times(2).minus(held1));
                        logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                        expect(logs.length).toEqual(1);
                        settleLog = logs[0];
                        expect(settleLog.name).toEqual('Settlement');
                        expect(settleLog.args.makerAddress).toEqual(owner2);
                        expect(settleLog.args.takerAddress).toEqual(owner1);
                        expect(settleLog.args.heldMarketId).toEqual(heldMarket);
                        expect(settleLog.args.owedMarketId).toEqual(owedMarket);
                        expect(settleLog.args.heldWei).toEqual(par.times(premium));
                        expect(settleLog.args.owedWei).toEqual(par);
                        console.log("\tFinalSettlement (owed) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in settling part of a position', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectSettlementOkay({
                            amount: {
                                value: par.div(-2),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })];
                    case 1:
                        _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                                solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                            ])];
                    case 2:
                        _a = _b.sent(), held1 = _a[0], owed1 = _a[1], held2 = _a[2], owed2 = _a[3];
                        expect(owed1).toEqual(par.div(2));
                        expect(owed2).toEqual(par.div(-2));
                        expect(held1).toEqual(par.times(premium).div(2));
                        expect(held2).toEqual(par.times(2).minus(held1));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in settling including premiums', function () { return __awaiter(_this, void 0, void 0, function () {
            var owedPremium, heldPremium, adjustedPremium, _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        owedPremium = new bignumber_js_1.default('0.5');
                        heldPremium = new bignumber_js_1.default('1.0');
                        adjustedPremium = premium.minus(1).times(owedPremium.plus(1)).times(heldPremium.plus(1)).plus(1);
                        return [4 /*yield*/, Promise.all([
                                solo.admin.setSpreadPremium(owedMarket, owedPremium, { from: admin }),
                                solo.admin.setSpreadPremium(heldMarket, heldPremium, { from: admin }),
                            ])];
                    case 1:
                        _b.sent();
                        return [4 /*yield*/, expectSettlementOkay({})];
                    case 2:
                        _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                                solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                            ])];
                    case 3:
                        _a = _b.sent(), held1 = _a[0], owed1 = _a[1], held2 = _a[2], owed2 = _a[3];
                        expect(owed1).toEqual(zero);
                        expect(owed2).toEqual(zero);
                        expect(held1).toEqual(par.times(adjustedPremium));
                        expect(held2).toEqual(par.times(2).minus(held1));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for zero inputMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            var getAllBalances, start, end;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        getAllBalances = [
                            solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                            solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                            solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                            solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                        ];
                        return [4 /*yield*/, Promise.all(getAllBalances)];
                    case 1:
                        start = _a.sent();
                        return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, zero)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementOkay({}, {}, false)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, Promise.all(getAllBalances)];
                    case 4:
                        end = _a.sent();
                        expect(start).toEqual(end);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for positive inputMarket', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert({}, 'FinalSettlement: outputMarket mismatch')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for overpaying a borrow', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectSettlementRevert({
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        }, 'FinalSettlement: Borrows cannot be overpaid')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for increasing a borrow', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectSettlementRevert({
                            amount: {
                                value: par.times(-2),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        }, 'FinalSettlement: outputMarket mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if not initialized', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, EVM_1.resetEVM(snapshotId)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert({}, 'FinalSettlement: Contract must be initialized')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for invalid trade data', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectSettlementRevert({ data: [] })];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for zero collateral', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, zero)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert({}, 'FinalSettlement: Collateral must be positive')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for negative collateral', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par.times(-1))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert({}, 'FinalSettlement: Collateral must be positive')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for overtaking collateral', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par.div(2))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectSettlementRevert({}, 'FinalSettlement: outputMarket too small')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#getSpreadAdjustedPrices', function () {
        it('Succeeds at initialization', function () { return __awaiter(_this, void 0, void 0, function () {
            var timestamp, _a, _b, prices;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _c.sent();
                        _b = (_a = solo.web3.eth).getBlock;
                        return [4 /*yield*/, solo.web3.eth.getBlockNumber()];
                    case 2: return [4 /*yield*/, _b.apply(_a, [_c.sent()])];
                    case 3:
                        timestamp = (_c.sent()).timestamp;
                        return [4 /*yield*/, solo.finalSettlement.getPrices(heldMarket, owedMarket, new bignumber_js_1.default(timestamp))];
                    case 4:
                        prices = _c.sent();
                        expect(prices.owedPrice.eq(defaultPrice)).toEqual(true);
                        expect(prices.heldPrice).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds when recently initialized', function () { return __awaiter(_this, void 0, void 0, function () {
            var timestamp, _a, _b, prices;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _c.sent();
                        _b = (_a = solo.web3.eth).getBlock;
                        return [4 /*yield*/, solo.web3.eth.getBlockNumber()];
                    case 2: return [4 /*yield*/, _b.apply(_a, [_c.sent()])];
                    case 3:
                        timestamp = (_c.sent()).timestamp;
                        return [4 /*yield*/, EVM_1.mineAvgBlock()];
                    case 4:
                        _c.sent();
                        return [4 /*yield*/, solo.finalSettlement.getPrices(heldMarket, owedMarket, new bignumber_js_1.default(timestamp + 60 * 60))];
                    case 5:
                        prices = _c.sent();
                        expect(prices.owedPrice.lt(defaultPrice.times(premium))).toEqual(true);
                        expect(prices.owedPrice.gt(defaultPrice)).toEqual(true);
                        expect(prices.heldPrice).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds when initialized a long time ago', function () { return __awaiter(_this, void 0, void 0, function () {
            var timestamp, _a, _b, prices1, prices2;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _c.sent();
                        _b = (_a = solo.web3.eth).getBlock;
                        return [4 /*yield*/, solo.web3.eth.getBlockNumber()];
                    case 2: return [4 /*yield*/, _b.apply(_a, [_c.sent()])];
                    case 3:
                        timestamp = (_c.sent()).timestamp;
                        return [4 /*yield*/, EVM_1.mineAvgBlock()];
                    case 4:
                        _c.sent();
                        return [4 /*yield*/, solo.finalSettlement.getPrices(heldMarket, owedMarket, new bignumber_js_1.default(timestamp + 60 * 60 * 24 * 28))];
                    case 5:
                        prices1 = _c.sent();
                        expect(prices1.owedPrice).toEqual(defaultPrice.times(premium));
                        expect(prices1.heldPrice).toEqual(defaultPrice);
                        return [4 /*yield*/, solo.finalSettlement.getPrices(heldMarket, owedMarket, new bignumber_js_1.default(timestamp + 60 * 60 * 24 * 365))];
                    case 6:
                        prices2 = _c.sent();
                        expect(prices2).toEqual(prices1);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails when not yet initialized', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.finalSettlement.getPrices(heldMarket, owedMarket, new bignumber_js_1.default(10)), 'FinalSettlement: Not initialized')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('accountOperation#finalSettlement', function () {
        beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.finalSettlement.initialize()];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.fastForward(60 * 60 * 24 * 28)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, solo.operation.initiate().finalSettlement({
                            liquidMarketId: owedMarket,
                            payoutMarketId: heldMarket,
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            liquidAccountOwner: owner2,
                            liquidAccountId: accountNumber2,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Principal,
                                reference: types_1.AmountReference.Target,
                            },
                        }).commit({ from: owner1 })];
                    case 1:
                        _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner1, accountNumber1, heldMarket),
                                solo.getters.getAccountPar(owner1, accountNumber1, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                            ])];
                    case 2:
                        _a = _b.sent(), held1 = _a[0], owed1 = _a[1], held2 = _a[2], owed2 = _a[3];
                        expect(owed1).toEqual(zero);
                        expect(owed2).toEqual(zero);
                        expect(held1).toEqual(par.times(premium));
                        expect(held2).toEqual(par.times(2).minus(held1));
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
// ============ Helper Functions ============
function expectSettlementOkay(glob, options, expectLogs) {
    if (expectLogs === void 0) { expectLogs = true; }
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob, txResult, logs;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    combinedGlob = __assign({}, defaultGlob, glob);
                    return [4 /*yield*/, solo.operation
                            .initiate()
                            .trade(combinedGlob)
                            .commit(__assign({}, options, { from: owner1 }))];
                case 1:
                    txResult = _a.sent();
                    if (expectLogs) {
                        logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                        expect(logs.length).toEqual(1);
                        expect(logs[0].name).toEqual('Settlement');
                    }
                    return [2 /*return*/, txResult];
            }
        });
    });
}
function expectSettlementRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectSettlementOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=FinalSettlement.test.js.map