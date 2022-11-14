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
var rando;
var startingExpiry;
var accountNumber1 = Constants_1.INTEGERS.ZERO;
var accountNumber2 = Constants_1.INTEGERS.ONE;
var heldMarket = Constants_1.INTEGERS.ZERO;
var owedMarket = Constants_1.INTEGERS.ONE;
var collateralMarket = new bignumber_js_1.default(2);
var par = new bignumber_js_1.default(10000);
var zero = new bignumber_js_1.default(0);
var premium = new bignumber_js_1.default('1.05');
var defaultPrice = new bignumber_js_1.default('1e40');
var defaultTimeDelta = new bignumber_js_1.default(1234);
var defaultGlob;
var heldGlob;
describe('ExpiryV2', function () {
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
                    rando = accounts[4];
                    defaultGlob = {
                        primaryAccountOwner: owner1,
                        primaryAccountId: accountNumber1,
                        otherAccountOwner: owner2,
                        otherAccountId: accountNumber2,
                        inputMarketId: owedMarket,
                        outputMarketId: heldMarket,
                        autoTrader: solo.contracts.expiryV2.options.address,
                        amount: {
                            value: zero,
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Target,
                        },
                        data: BytesHelper_1.toBytes(owedMarket, Constants_1.INTEGERS.ONES_31),
                    };
                    heldGlob = {
                        primaryAccountOwner: owner1,
                        primaryAccountId: accountNumber1,
                        otherAccountOwner: owner2,
                        otherAccountId: accountNumber2,
                        inputMarketId: heldMarket,
                        outputMarketId: owedMarket,
                        autoTrader: solo.contracts.expiryV2.options.address,
                        amount: {
                            value: zero,
                            denomination: types_1.AmountDenomination.Principal,
                            reference: types_1.AmountReference.Target,
                        },
                        data: BytesHelper_1.toBytes(owedMarket, Constants_1.INTEGERS.ONES_31),
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
                    return [4 /*yield*/, Promise.all([
                            setExpiryForSelf(Constants_1.INTEGERS.ONE, true),
                            solo.expiryV2.setApproval(owner1, defaultTimeDelta, { from: owner2 }),
                        ])];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                case 6:
                    startingExpiry = _a.sent();
                    return [4 /*yield*/, EVM_1.fastForward(60 * 60 * 24)];
                case 7:
                    _a.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 8:
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
    describe('setApproval', function () {
        it('Succeeds for zero', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, approval, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.expiryV2.setApproval(owner1, zero, { from: owner2 })];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, solo.expiryV2.getApproval(owner2, owner1)];
                    case 2:
                        approval = _a.sent();
                        expect(approval).toEqual(zero);
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogSenderApproved');
                        expect(log.args.approver).toEqual(owner2);
                        expect(log.args.sender).toEqual(owner1);
                        expect(log.args.minTimeDelta).toEqual(zero);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for non-zero', function () { return __awaiter(_this, void 0, void 0, function () {
            var defaultDelay, txResult, approval, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        defaultDelay = new bignumber_js_1.default(425);
                        return [4 /*yield*/, solo.expiryV2.setApproval(owner1, defaultDelay, { from: owner2 })];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, solo.expiryV2.getApproval(owner2, owner1)];
                    case 2:
                        approval = _a.sent();
                        expect(approval).toEqual(defaultDelay);
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogSenderApproved');
                        expect(log.args.approver).toEqual(owner2);
                        expect(log.args.sender).toEqual(owner1);
                        expect(log.args.minTimeDelta).toEqual(defaultDelay);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('callFunction (invalid)', function () {
        it('Fails for invalid callType', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().call({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            callee: solo.contracts.expiryV2.options.address,
                            data: BytesHelper_1.toBytes(2, 2, 2, 2),
                        }).commit())];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for zero bytes', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().call({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            callee: solo.contracts.expiryV2.options.address,
                            data: [],
                        }).commit())];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('callFunctionSetApproval', function () {
        it('Succeeds in setting approval', function () { return __awaiter(_this, void 0, void 0, function () {
            var minTimeDeltas, i, txResult, logs, log, actualMinTimeDelta;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        minTimeDeltas = [Constants_1.INTEGERS.ZERO, defaultTimeDelta];
                        i = 0;
                        _a.label = 1;
                    case 1:
                        if (!(i < minTimeDeltas.length)) return [3 /*break*/, 5];
                        return [4 /*yield*/, solo.operation.initiate().setApprovalForExpiryV2({
                                primaryAccountOwner: owner2,
                                primaryAccountId: Constants_1.INTEGERS.ZERO,
                                sender: owner1,
                                minTimeDelta: minTimeDeltas[i],
                            }).commit({ from: owner2 })];
                    case 2:
                        txResult = _a.sent();
                        logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogSenderApproved');
                        expect(log.args.approver).toEqual(owner2);
                        expect(log.args.sender).toEqual(owner1);
                        expect(log.args.minTimeDelta).toEqual(minTimeDeltas[i]);
                        return [4 /*yield*/, solo.expiryV2.getApproval(owner2, owner1)];
                    case 3:
                        actualMinTimeDelta = _a.sent();
                        expect(actualMinTimeDelta).toEqual(minTimeDeltas[i]);
                        _a.label = 4;
                    case 4:
                        i += 1;
                        return [3 /*break*/, 1];
                    case 5: return [2 /*return*/];
                }
            });
        }); });
    });
    describe('callFunctionSetExpiry (self)', function () {
        it('Succeeds in setting expiry', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForSelf(defaultTimeDelta, true)];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectExpiry(txResult, owner2, accountNumber2, owedMarket, defaultTimeDelta)];
                    case 2:
                        _a.sent();
                        console.log("\tSet expiry (self) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if not forceUpdate', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForSelf(defaultTimeDelta.div(2), false)];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectNoExpirySet(txResult)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Skips logs when necessary', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, noLogs, logs;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForSelf(defaultTimeDelta, true)];
                    case 1:
                        txResult = _a.sent();
                        noLogs = solo.logs.parseLogs(txResult, { skipExpiryLogs: true });
                        logs = solo.logs.parseLogs(txResult, { skipExpiryLogs: false });
                        expect(noLogs.filter(function (e) { return e.name === 'ExpirySet'; }).length).toEqual(0);
                        expect(logs.filter(function (e) { return e.name === 'ExpirySet'; }).length).not.toEqual(0);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Sets expiry to zero even if given positive delta (non-negative balances)', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, setExpiryForSelf(defaultTimeDelta, false)];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectExpiry(txResult, owner2, accountNumber2, owedMarket, zero)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Sets expiry to zero on purpose (non-negative balances)', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, setExpiryForSelf(zero, false)];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectExpiry(txResult, owner2, accountNumber2, owedMarket, zero)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('callFunctionSetExpiry (other)', function () {
        it('Succeeds in setting expiry', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForOther(defaultTimeDelta, true)];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectExpiry(txResult, owner2, accountNumber2, owedMarket, defaultTimeDelta)];
                    case 2:
                        _a.sent();
                        console.log("\tSet expiry (other) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if not minimum approved timeDelta', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForOther(defaultTimeDelta.div(2), true)];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectNoExpirySet(txResult)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if not forceUpdate', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForOther(defaultTimeDelta.div(2), false)];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectNoExpirySet(txResult)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Allows longer than minimum approved timeDelta', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForOther(defaultTimeDelta.times(2), true)];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectExpiry(txResult, owner2, accountNumber2, owedMarket, defaultTimeDelta.times(2))];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Do nothing if sender not approved', function () { return __awaiter(_this, void 0, void 0, function () {
            var timestamp1, txResult1, txResult2, timestamp2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                    case 1:
                        timestamp1 = _a.sent();
                        return [4 /*yield*/, solo.operation.initiate().setExpiryV2({
                                primaryAccountOwner: rando,
                                primaryAccountId: accountNumber1,
                                expiryV2Args: [{
                                        accountOwner: owner2,
                                        accountId: accountNumber2,
                                        marketId: owedMarket,
                                        timeDelta: defaultTimeDelta,
                                        forceUpdate: true,
                                    }],
                            }).commit({ from: rando })];
                    case 2:
                        txResult1 = _a.sent();
                        expect(solo.logs.parseLogs(txResult1, { skipOperationLogs: true }).length).toEqual(0);
                        return [4 /*yield*/, solo.operation.initiate().setExpiryV2({
                                primaryAccountOwner: rando,
                                primaryAccountId: accountNumber1,
                                expiryV2Args: [{
                                        accountOwner: owner2,
                                        accountId: accountNumber2,
                                        marketId: owedMarket,
                                        timeDelta: zero,
                                        forceUpdate: true,
                                    }],
                            }).commit({ from: rando })];
                    case 3:
                        txResult2 = _a.sent();
                        expect(solo.logs.parseLogs(txResult2, { skipOperationLogs: true }).length).toEqual(0);
                        return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                    case 4:
                        timestamp2 = _a.sent();
                        expect(timestamp2).toEqual(timestamp1);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Do nothing if sender not approved (non-negative balance)', function () { return __awaiter(_this, void 0, void 0, function () {
            var timestamp1, txResult1, txResult2, timestamp2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                    case 2:
                        timestamp1 = _a.sent();
                        return [4 /*yield*/, solo.operation.initiate().setExpiryV2({
                                primaryAccountOwner: rando,
                                primaryAccountId: accountNumber1,
                                expiryV2Args: [{
                                        accountOwner: owner2,
                                        accountId: accountNumber2,
                                        marketId: owedMarket,
                                        timeDelta: defaultTimeDelta,
                                        forceUpdate: true,
                                    }],
                            }).commit({ from: rando })];
                    case 3:
                        txResult1 = _a.sent();
                        expect(solo.logs.parseLogs(txResult1, { skipOperationLogs: true }).length).toEqual(0);
                        return [4 /*yield*/, solo.operation.initiate().setExpiryV2({
                                primaryAccountOwner: rando,
                                primaryAccountId: accountNumber1,
                                expiryV2Args: [{
                                        accountOwner: owner2,
                                        accountId: accountNumber2,
                                        marketId: owedMarket,
                                        timeDelta: zero,
                                        forceUpdate: true,
                                    }],
                            }).commit({ from: rando })];
                    case 4:
                        txResult2 = _a.sent();
                        expect(solo.logs.parseLogs(txResult2, { skipOperationLogs: true }).length).toEqual(0);
                        return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                    case 5:
                        timestamp2 = _a.sent();
                        expect(timestamp2).toEqual(timestamp1);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Set it for multiple', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, timestamp, logs, _a, expiry1, expiry2, expiry3, expiry4;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner1, accountNumber1, owedMarket, par.times(-1)),
                            solo.testing.setAccountBalance(owner1, accountNumber1, collateralMarket, par.times(4)),
                            solo.testing.setAccountBalance(rando, accountNumber1, heldMarket, par.times(-1)),
                            solo.expiryV2.setApproval(owner1, defaultTimeDelta, { from: rando }),
                        ])];
                    case 1:
                        _b.sent();
                        return [4 /*yield*/, solo.operation.initiate().setExpiryV2({
                                primaryAccountOwner: owner1,
                                primaryAccountId: accountNumber1,
                                expiryV2Args: [
                                    {
                                        accountOwner: owner2,
                                        accountId: accountNumber2,
                                        marketId: owedMarket,
                                        timeDelta: zero,
                                        forceUpdate: true,
                                    },
                                    {
                                        accountOwner: owner1,
                                        accountId: accountNumber1,
                                        marketId: owedMarket,
                                        timeDelta: defaultTimeDelta.div(2),
                                        forceUpdate: false,
                                    },
                                    {
                                        accountOwner: owner1,
                                        accountId: accountNumber1,
                                        marketId: owedMarket,
                                        timeDelta: defaultTimeDelta.div(2),
                                        forceUpdate: true,
                                    },
                                    {
                                        accountOwner: rando,
                                        accountId: accountNumber1,
                                        marketId: heldMarket,
                                        timeDelta: defaultTimeDelta.times(2),
                                        forceUpdate: true,
                                    },
                                    {
                                        accountOwner: rando,
                                        accountId: accountNumber1,
                                        marketId: owedMarket,
                                        timeDelta: defaultTimeDelta.div(2),
                                        forceUpdate: true,
                                    },
                                ],
                            }).commit({ from: owner1 })];
                    case 2:
                        txResult = _b.sent();
                        return [4 /*yield*/, solo.web3.eth.getBlock(txResult.blockNumber)];
                    case 3:
                        timestamp = (_b.sent()).timestamp;
                        logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                        expect(logs.length).toEqual(3);
                        expect(logs[0].name).toEqual('ExpirySet');
                        expect(logs[1].name).toEqual('ExpirySet');
                        expect(logs[2].name).toEqual('ExpirySet');
                        expect(logs[0].args.owner).toEqual(owner1);
                        expect(logs[0].args.number).toEqual(accountNumber1);
                        expect(logs[0].args.marketId).toEqual(owedMarket);
                        expect(logs[0].args.time).toEqual(defaultTimeDelta.div(2).plus(timestamp));
                        expect(logs[1].args.owner).toEqual(owner1);
                        expect(logs[1].args.number).toEqual(accountNumber1);
                        expect(logs[1].args.marketId).toEqual(owedMarket);
                        expect(logs[1].args.time).toEqual(defaultTimeDelta.div(2).plus(timestamp));
                        expect(logs[2].args.owner).toEqual(rando);
                        expect(logs[2].args.number).toEqual(accountNumber1);
                        expect(logs[2].args.marketId).toEqual(heldMarket);
                        expect(logs[2].args.time).toEqual(defaultTimeDelta.times(2).plus(timestamp));
                        return [4 /*yield*/, Promise.all([
                                solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket),
                                solo.expiryV2.getExpiry(owner1, accountNumber1, owedMarket),
                                solo.expiryV2.getExpiry(rando, accountNumber1, heldMarket),
                                solo.expiryV2.getExpiry(rando, accountNumber1, owedMarket),
                            ])];
                    case 4:
                        _a = _b.sent(), expiry1 = _a[0], expiry2 = _a[1], expiry3 = _a[2], expiry4 = _a[3];
                        expect(expiry1).toEqual(startingExpiry);
                        expect(expiry2).toEqual(defaultTimeDelta.div(2).plus(timestamp));
                        expect(expiry3).toEqual(defaultTimeDelta.times(2).plus(timestamp));
                        expect(expiry4).toEqual(zero);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Sets expiry to zero for non-negative balances', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, setExpiryForOther(defaultTimeDelta, false)];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectExpiry(txResult, owner2, accountNumber2, owedMarket, zero)];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Prevents setting expiry back to zero', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult1, txResult2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForOther(zero, false)];
                    case 1:
                        txResult1 = _a.sent();
                        return [4 /*yield*/, expectNoExpirySet(txResult1)];
                    case 2:
                        _a.sent();
                        // even for positive balances
                        return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, owedMarket, par)];
                    case 3:
                        // even for positive balances
                        _a.sent();
                        return [4 /*yield*/, setExpiryForOther(zero, false)];
                    case 4:
                        txResult2 = _a.sent();
                        return [4 /*yield*/, expectNoExpirySet(txResult2)];
                    case 5:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('expire account (heldAmount)', function () {
        beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in expiring', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectExpireOkay(heldGlob)];
                    case 1:
                        txResult = _b.sent();
                        logs = solo.logs.parseLogs(txResult);
                        logs.forEach(function (log) { return expect(log.name).not.toEqual('ExpirySet'); });
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
                        console.log("\tExpiring (held) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in expiring and setting expiry back to zero', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, expiryLog;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par.times(premium))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectExpireOkay(heldGlob)];
                    case 2:
                        txResult = _a.sent();
                        logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                        expect(logs.length).toEqual(1);
                        expiryLog = logs[0];
                        expect(expiryLog.name).toEqual('ExpirySet');
                        expect(expiryLog.args.owner).toEqual(owner2);
                        expect(expiryLog.args.number).toEqual(accountNumber2);
                        expect(expiryLog.args.marketId).toEqual(owedMarket);
                        expect(expiryLog.args.time).toEqual(zero);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in expiring part of a position', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectExpireOkay(__assign({}, heldGlob, { amount: {
                                value: par.div(2),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } }))];
                    case 1:
                        txResult = _b.sent();
                        logs = solo.logs.parseLogs(txResult);
                        logs.forEach(function (log) { return expect(log.name).not.toEqual('ExpirySet'); });
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
        it('Succeeds in expiring including premiums', function () { return __awaiter(_this, void 0, void 0, function () {
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
                        return [4 /*yield*/, expectExpireOkay(heldGlob)];
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
                        return [4 /*yield*/, expectExpireOkay(heldGlob)];
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
                        return [4 /*yield*/, expectExpireRevert(heldGlob, 'ExpiryV2: inputMarket mismatch')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for overusing collateral', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert(__assign({}, heldGlob, { amount: {
                                value: par.times(-1),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } }), 'ExpiryV2: Collateral cannot be overused')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for increasing the heldAmount', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert(__assign({}, heldGlob, { amount: {
                                value: par.times(4),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            } }), 'ExpiryV2: inputMarket mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for a zero expiry', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForSelf(zero, true)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectExpireRevert(heldGlob, 'ExpiryV2: Expiry not set')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for a future expiry', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForSelf(defaultTimeDelta, true)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectExpireRevert(heldGlob, 'ExpiryV2: Borrow not yet expired')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for an expiry past maxExpiry', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert(__assign({}, heldGlob, { data: BytesHelper_1.toBytes(owedMarket, defaultTimeDelta) }), 'ExpiryV2: Expiry past maxExpiry')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for invalid trade data', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert(__assign({}, heldGlob, { data: BytesHelper_1.toBytes(owedMarket) }))];
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
                        return [4 /*yield*/, expectExpireRevert(heldGlob, 'ExpiryV2: Borrows must be negative')];
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
                        return [4 /*yield*/, expectExpireRevert(heldGlob, 'ExpiryV2: Borrows must be negative')];
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
                        return [4 /*yield*/, expectExpireRevert(heldGlob, 'ExpiryV2: outputMarket too small')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('expire account (owedAmount)', function () {
        it('Succeeds in expiring', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, expiryLog, _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectExpireOkay({})];
                    case 1:
                        txResult = _b.sent();
                        logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                        expect(logs.length).toEqual(1);
                        expiryLog = logs[0];
                        expect(expiryLog.name).toEqual('ExpirySet');
                        expect(expiryLog.args.owner).toEqual(owner2);
                        expect(expiryLog.args.number).toEqual(accountNumber2);
                        expect(expiryLog.args.marketId).toEqual(owedMarket);
                        expect(expiryLog.args.time).toEqual(zero);
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
                        console.log("\tExpiring (owed) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds in expiring part of a position', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, expectExpireOkay({
                            amount: {
                                value: par.div(-2),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })];
                    case 1:
                        txResult = _b.sent();
                        logs = solo.logs.parseLogs(txResult);
                        logs.forEach(function (log) { return expect(log.name).not.toEqual('ExpirySet'); });
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
        it('Succeeds in expiring including premiums', function () { return __awaiter(_this, void 0, void 0, function () {
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
                        return [4 /*yield*/, expectExpireOkay({})];
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
        it('Fails for non-solo calls', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.expiryV2.methods.callFunction(owner1, {
                            owner: owner1,
                            number: accountNumber1.toFixed(0),
                        }, [])), 'OnlySolo: Only Solo can call function')];
                    case 1:
                        _a.sent();
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
                        return [4 /*yield*/, expectExpireOkay({})];
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
                        return [4 /*yield*/, expectExpireRevert({}, 'ExpiryV2: outputMarket mismatch')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for overpaying a borrow', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert({
                            amount: {
                                value: par,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        }, 'ExpiryV2: Borrows cannot be overpaid')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for increasing a borrow', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert({
                            amount: {
                                value: par.times(-2),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        }, 'ExpiryV2: outputMarket mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for a zero expiry', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForSelf(zero, true)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectExpireRevert({}, 'ExpiryV2: Expiry not set')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for a future expiry', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setExpiryForSelf(defaultTimeDelta, true)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectExpireRevert({}, 'ExpiryV2: Borrow not yet expired')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for an expiry past maxExpiry', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert({
                            data: BytesHelper_1.toBytes(owedMarket, defaultTimeDelta),
                        }, 'ExpiryV2: Expiry past maxExpiry')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for invalid trade data', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectExpireRevert({
                            data: BytesHelper_1.toBytes(owedMarket),
                        })];
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
                        return [4 /*yield*/, expectExpireRevert({}, 'ExpiryV2: Collateral must be positive')];
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
                        return [4 /*yield*/, expectExpireRevert({}, 'ExpiryV2: Collateral must be positive')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for overtaking collateral', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectExpireRevert({}, 'ExpiryV2: outputMarket too small')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('AccountOperation#fullyLiquidateExpiredAccountV2', function () {
        it('Succeeds for two assets', function () { return __awaiter(_this, void 0, void 0, function () {
            var prices, premiums, collateralPreferences, weis, expiryTimestamp, balances;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        prices = [
                            Constants_1.INTEGERS.ONES_31,
                            Constants_1.INTEGERS.ONES_31,
                            Constants_1.INTEGERS.ONES_31,
                        ];
                        premiums = [
                            Constants_1.INTEGERS.ZERO,
                            Constants_1.INTEGERS.ZERO,
                            Constants_1.INTEGERS.ZERO,
                        ];
                        collateralPreferences = [
                            owedMarket,
                            heldMarket,
                            collateralMarket,
                        ];
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(0)),
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(1)),
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(2)),
                            ])];
                    case 1:
                        weis = _a.sent();
                        return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                    case 2:
                        expiryTimestamp = _a.sent();
                        return [4 /*yield*/, solo.operation.initiate().fullyLiquidateExpiredAccountV2(owner1, accountNumber1, owner2, accountNumber2, owedMarket, expiryTimestamp, expiryTimestamp.plus(Constants_1.INTEGERS.ONE_DAY_IN_SECONDS), weis, prices, premiums, collateralPreferences).commit({ from: owner1 })];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, collateralMarket),
                            ])];
                    case 4:
                        balances = _a.sent();
                        expect(balances[0]).toEqual(zero);
                        expect(balances[1]).toEqual(par.times(2).minus(par.times(premium)));
                        expect(balances[2]).toEqual(par.times(4));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for three assets', function () { return __awaiter(_this, void 0, void 0, function () {
            var prices, premiums, collateralPreferences, weis, expiryTimestamp, balances, remainingOwed;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        prices = [
                            Constants_1.INTEGERS.ONES_31,
                            Constants_1.INTEGERS.ONES_31,
                            Constants_1.INTEGERS.ONES_31,
                        ];
                        premiums = [
                            Constants_1.INTEGERS.ZERO,
                            Constants_1.INTEGERS.ZERO,
                            Constants_1.INTEGERS.ZERO,
                        ];
                        collateralPreferences = [
                            owedMarket,
                            heldMarket,
                            collateralMarket,
                        ];
                        return [4 /*yield*/, Promise.all([
                                solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par),
                                solo.testing.setAccountBalance(owner2, accountNumber2, collateralMarket, par),
                            ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(0)),
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(1)),
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(2)),
                            ])];
                    case 2:
                        weis = _a.sent();
                        return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                    case 3:
                        expiryTimestamp = _a.sent();
                        return [4 /*yield*/, solo.operation.initiate().fullyLiquidateExpiredAccountV2(owner1, accountNumber1, owner2, accountNumber2, owedMarket, expiryTimestamp, expiryTimestamp.plus(Constants_1.INTEGERS.ONE_DAY_IN_SECONDS), weis, prices, premiums, collateralPreferences).commit({ from: owner1 })];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, collateralMarket),
                            ])];
                    case 5:
                        balances = _a.sent();
                        expect(balances[0]).toEqual(zero);
                        expect(balances[1]).toEqual(zero);
                        remainingOwed = par.minus(par.div(premium));
                        expect(balances[2]).toEqual(par.minus(remainingOwed.times(premium)).integerValue(bignumber_js_1.default.ROUND_UP));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for three assets (with premiums)', function () { return __awaiter(_this, void 0, void 0, function () {
            var prices, premiums, collateralPreferences, weis, expiryTimestamp, balances, firstPrem, secondPrem, remainingOwed;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        prices = [
                            Constants_1.INTEGERS.ONES_31,
                            Constants_1.INTEGERS.ONES_31,
                            Constants_1.INTEGERS.ONES_31,
                        ];
                        premiums = [
                            new bignumber_js_1.default('0.1'),
                            new bignumber_js_1.default('0.2'),
                            new bignumber_js_1.default('0.3'),
                        ];
                        collateralPreferences = [
                            owedMarket,
                            heldMarket,
                            collateralMarket,
                        ];
                        return [4 /*yield*/, Promise.all([
                                solo.admin.setSpreadPremium(heldMarket, premiums[0], { from: admin }),
                                solo.admin.setSpreadPremium(owedMarket, premiums[1], { from: admin }),
                                solo.admin.setSpreadPremium(collateralMarket, premiums[2], { from: admin }),
                                solo.testing.setAccountBalance(owner2, accountNumber2, heldMarket, par.times(premium)),
                                solo.testing.setAccountBalance(owner2, accountNumber2, collateralMarket, par),
                            ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(0)),
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(1)),
                                solo.getters.getAccountWei(owner2, accountNumber2, new bignumber_js_1.default(2)),
                            ])];
                    case 2:
                        weis = _a.sent();
                        return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                    case 3:
                        expiryTimestamp = _a.sent();
                        return [4 /*yield*/, solo.operation.initiate().fullyLiquidateExpiredAccountV2(owner1, accountNumber1, owner2, accountNumber2, owedMarket, expiryTimestamp, expiryTimestamp.plus(Constants_1.INTEGERS.ONE_DAY_IN_SECONDS), weis, prices, premiums, collateralPreferences).commit({ from: owner1 })];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.getters.getAccountPar(owner2, accountNumber2, owedMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, heldMarket),
                                solo.getters.getAccountPar(owner2, accountNumber2, collateralMarket),
                            ])];
                    case 5:
                        balances = _a.sent();
                        expect(balances[0]).toEqual(zero);
                        expect(balances[1]).toEqual(zero);
                        firstPrem = premium.minus(1).times('1.1').times('1.2').plus(1);
                        secondPrem = premium.minus(1).times('1.2').times('1.3').plus(1);
                        remainingOwed = par.minus(par.times(premium).div(firstPrem));
                        expect(balances[2]).toEqual(par.minus(remainingOwed.times(secondPrem)).integerValue(bignumber_js_1.default.ROUND_UP));
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#getSpreadAdjustedPrices', function () {
        it('Succeeds for recently expired positions', function () { return __awaiter(_this, void 0, void 0, function () {
            var timestamp, _a, _b, prices;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        _b = (_a = solo.web3.eth).getBlock;
                        return [4 /*yield*/, solo.web3.eth.getBlockNumber()];
                    case 1: return [4 /*yield*/, _b.apply(_a, [_c.sent()])];
                    case 2:
                        timestamp = (_c.sent()).timestamp;
                        return [4 /*yield*/, EVM_1.mineAvgBlock()];
                    case 3:
                        _c.sent();
                        return [4 /*yield*/, solo.expiryV2.getPrices(heldMarket, owedMarket, new bignumber_js_1.default(timestamp))];
                    case 4:
                        prices = _c.sent();
                        expect(prices.owedPrice.lt(defaultPrice.times(premium))).toEqual(true);
                        expect(prices.owedPrice.gt(defaultPrice)).toEqual(true);
                        expect(prices.heldPrice).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for very expired positions', function () { return __awaiter(_this, void 0, void 0, function () {
            var prices;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.expiryV2.getPrices(heldMarket, owedMarket, Constants_1.INTEGERS.ONE)];
                    case 1:
                        prices = _a.sent();
                        expect(prices.owedPrice).toEqual(defaultPrice.times(premium));
                        expect(prices.heldPrice).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#ownerSetExpiryRampTime', function () {
        it('Succeeds for owner', function () { return __awaiter(_this, void 0, void 0, function () {
            var oldValue, newValue;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.expiryV2.getRampTime()];
                    case 1:
                        oldValue = _a.sent();
                        expect(oldValue).toEqual(Constants_1.INTEGERS.ONE_HOUR_IN_SECONDS);
                        return [4 /*yield*/, solo.expiryV2.setRampTime(Constants_1.INTEGERS.ONE_DAY_IN_SECONDS, { from: admin })];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.expiryV2.getRampTime()];
                    case 3:
                        newValue = _a.sent();
                        expect(newValue).toEqual(Constants_1.INTEGERS.ONE_DAY_IN_SECONDS);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.expiryV2.setRampTime(Constants_1.INTEGERS.ONE_DAY_IN_SECONDS, { from: owner1 }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#liquidateExpiredAccount', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, held1, owed1, held2, owed2;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, solo.operation.initiate().liquidateExpiredAccountV2({
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
function setExpiryForSelf(timeDelta, forceUpdate, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.operation.initiate().setExpiryV2({
                    primaryAccountOwner: owner2,
                    primaryAccountId: accountNumber2,
                    expiryV2Args: [{
                            timeDelta: timeDelta,
                            forceUpdate: forceUpdate,
                            accountOwner: owner2,
                            accountId: accountNumber2,
                            marketId: owedMarket,
                        }],
                }).commit(__assign({}, options, { from: owner2 }))];
        });
    });
}
function setExpiryForOther(timeDelta, forceUpdate, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.operation.initiate().setExpiryV2({
                    primaryAccountOwner: owner1,
                    primaryAccountId: accountNumber1,
                    expiryV2Args: [{
                            timeDelta: timeDelta,
                            forceUpdate: forceUpdate,
                            accountOwner: owner2,
                            accountId: accountNumber2,
                            marketId: owedMarket,
                        }],
                }).commit(__assign({}, options, { from: owner1 }))];
        });
    });
}
function expectExpireOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob;
        return __generator(this, function (_a) {
            combinedGlob = __assign({}, defaultGlob, glob);
            return [2 /*return*/, solo.operation.initiate().trade(combinedGlob).commit(__assign({}, options, { from: owner1 }))];
        });
    });
}
function expectExpireRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectExpireOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function expectExpiry(txResult, owner, accountNumber, market, timeDelta) {
    return __awaiter(this, void 0, void 0, function () {
        var timestamp, expectedExpiryTime, logs, expirySetLog, expiry;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.web3.eth.getBlock(txResult.blockNumber)];
                case 1:
                    timestamp = (_a.sent()).timestamp;
                    expectedExpiryTime = timeDelta.isZero() ? zero : timeDelta.plus(timestamp);
                    logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                    expect(logs.length).toEqual(1);
                    expirySetLog = logs[0];
                    expect(expirySetLog.name).toEqual('ExpirySet');
                    expect(expirySetLog.args.owner).toEqual(owner);
                    expect(expirySetLog.args.number).toEqual(accountNumber);
                    expect(expirySetLog.args.marketId).toEqual(market);
                    expect(expirySetLog.args.time).toEqual(expectedExpiryTime);
                    return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                case 2:
                    expiry = _a.sent();
                    expect(expiry).toEqual(expectedExpiryTime);
                    return [2 /*return*/];
            }
        });
    });
}
function expectNoExpirySet(txResult) {
    return __awaiter(this, void 0, void 0, function () {
        var logs, expiry;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    logs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                    expect(logs.length).toEqual(0);
                    return [4 /*yield*/, solo.expiryV2.getExpiry(owner2, accountNumber2, owedMarket)];
                case 1:
                    expiry = _a.sent();
                    expect(expiry).toEqual(startingExpiry);
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=ExpiryV2.test.js.map