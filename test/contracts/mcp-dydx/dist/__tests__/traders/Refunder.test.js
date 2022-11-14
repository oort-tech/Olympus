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
var Deploy_1 = require("../helpers/Deploy");
var EVM_1 = require("../helpers/EVM");
var Constants_1 = require("../../src/lib/Constants");
var SoloHelpers_1 = require("../helpers/SoloHelpers");
var Expect_1 = require("../../src/lib/Expect");
var Refunder_json_1 = __importDefault(require("../../build/contracts/Refunder.json"));
var solo;
var accounts;
var snapshotId;
var admin;
var giver;
var receiver;
var rando;
var giverNumber = Constants_1.INTEGERS.ZERO;
var receiverNumber = Constants_1.INTEGERS.ONE;
var giveMarket = Constants_1.INTEGERS.ZERO;
var blankMarket = Constants_1.INTEGERS.ONE;
var wei = new bignumber_js_1.default(10000);
var zero = new bignumber_js_1.default(0);
var defaultGlob;
describe('Refunder', function () {
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
                    giver = accounts[2];
                    receiver = accounts[3];
                    rando = accounts[9];
                    defaultGlob = {
                        wei: wei,
                        primaryAccountOwner: giver,
                        primaryAccountId: giverNumber,
                        receiverAccountOwner: receiver,
                        receiverAccountId: receiverNumber,
                        refundMarketId: giveMarket,
                        otherMarketId: blankMarket,
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            SoloHelpers_1.setupMarkets(solo, accounts),
                            solo.contracts.send(solo.contracts.refunder.methods.addGiver(giver), { from: admin }),
                        ])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, EVM_1.mineAvgBlock()];
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
    describe('constructor', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var createdContract, _a, giverIsGiver, randoIsGiver, receiverIsGiver, gottenSolo;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, Deploy_1.deployContract(solo, Refunder_json_1.default, [
                            solo.contracts.soloMargin.options.address,
                            [giver, rando],
                        ])];
                    case 1:
                        createdContract = _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.call(createdContract.methods.g_givers(giver)),
                                solo.contracts.call(createdContract.methods.g_givers(rando)),
                                solo.contracts.call(createdContract.methods.g_givers(receiver)),
                                solo.contracts.call(createdContract.methods.SOLO_MARGIN()),
                            ])];
                    case 2:
                        _a = _b.sent(), giverIsGiver = _a[0], randoIsGiver = _a[1], receiverIsGiver = _a[2], gottenSolo = _a[3];
                        expect(giverIsGiver).toEqual(true);
                        expect(randoIsGiver).toEqual(true);
                        expect(receiverIsGiver).toEqual(false);
                        expect(gottenSolo).toEqual(solo.contracts.soloMargin.options.address);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('addGiver', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, isGiver, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.refunder.methods.addGiver(rando), { from: admin })];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, solo.contracts.call(solo.contracts.refunder.methods.g_givers(rando))];
                    case 2:
                        isGiver = _a.sent();
                        expect(isGiver).toEqual(true);
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogGiverAdded');
                        expect(log.args.giver).toEqual(rando);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.refunder.methods.addGiver(rando), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('removeGiver', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, isGiver, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.refunder.methods.removeGiver(giver), { from: admin })];
                    case 1:
                        txResult = _a.sent();
                        return [4 /*yield*/, solo.contracts.call(solo.contracts.refunder.methods.g_givers(giver))];
                    case 2:
                        isGiver = _a.sent();
                        expect(isGiver).toEqual(false);
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogGiverRemoved');
                        expect(log.args.giver).toEqual(giver);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.refunder.methods.removeGiver(giver), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getTradeCost', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.setAccountBalance(giver, giverNumber, giveMarket, wei)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.operation.initiate()
                                .refund(defaultGlob)
                                .commit({ from: giver })];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectBalances(wei, zero, zero, zero)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult, {
                            skipRefunderLogs: false,
                            skipAdminLogs: true,
                            skipExpiryLogs: true,
                            skipFinalSettlementLogs: true,
                            skipOperationLogs: true,
                            skipPermissionLogs: true,
                            skipLimitOrdersLogs: true,
                            skipSignedOperationProxyLogs: true,
                        });
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogRefund');
                        expect(log.args.account).toEqual({
                            owner: receiver,
                            number: receiverNumber,
                        });
                        expect(log.args.marketId).toEqual(giveMarket);
                        expect(log.args.amount).toEqual(wei);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for zero', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().refund(__assign({}, defaultGlob, { wei: zero })).commit({ from: giver }), 'Refunder: Refund must be positive')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for negative', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().refund(__assign({}, defaultGlob, { wei: wei.times(-1) })).commit({ from: giver }), 'Refunder: Refund must be positive')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-approved giver', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().refund(__assign({}, defaultGlob, { primaryAccountOwner: rando })).commit({ from: rando }), "Refunder: Giver not approved <" + rando.toLowerCase() + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-solo caller', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.refunder.methods.getTradeCost('0', '1', {
                            owner: receiver,
                            number: receiverNumber.toFixed(0),
                        }, {
                            owner: giver,
                            number: giverNumber.toFixed(0),
                        }, {
                            sign: false,
                            value: '0',
                        }, {
                            sign: false,
                            value: '0',
                        }, {
                            sign: true,
                            value: '100',
                        }, []), { from: rando }), 'OnlySolo: Only Solo can call function')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
function expectBalances(expectedRG, expectedGG, expectedRB, expectedGB) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, rg, gg, rb, gb;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.getters.getAccountWei(receiver, receiverNumber, giveMarket),
                        solo.getters.getAccountWei(giver, giverNumber, giveMarket),
                        solo.getters.getAccountWei(receiver, receiverNumber, blankMarket),
                        solo.getters.getAccountWei(giver, giverNumber, blankMarket),
                    ])];
                case 1:
                    _a = _b.sent(), rg = _a[0], gg = _a[1], rb = _a[2], gb = _a[3];
                    expect(rg).toEqual(expectedRG);
                    expect(gg).toEqual(expectedGG);
                    expect(rb).toEqual(expectedRB);
                    expect(gb).toEqual(expectedGB);
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Refunder.test.js.map