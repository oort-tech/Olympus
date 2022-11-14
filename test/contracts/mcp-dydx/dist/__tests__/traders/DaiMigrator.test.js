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
var types_1 = require("../../src/types");
var DaiMigrator_json_1 = __importDefault(require("../../build/contracts/DaiMigrator.json"));
var solo;
var accounts;
var snapshotId;
var admin;
var migrator;
var user;
var rando;
var migratorNumber = Constants_1.INTEGERS.ZERO;
var userNumber = Constants_1.INTEGERS.ONE;
var saiMarket = new bignumber_js_1.default(1);
var daiMarket = new bignumber_js_1.default(3);
var defaultGlob;
var weiString = types_1.AmountDenomination.Wei.toString();
var deltaString = types_1.AmountReference.Delta.toString();
describe('DaiMigrator', function () {
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r, numMarkets;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    admin = accounts[0];
                    migrator = accounts[2];
                    user = accounts[3];
                    rando = accounts[9];
                    defaultGlob = {
                        primaryAccountOwner: migrator,
                        primaryAccountId: migratorNumber,
                        userAccountOwner: user,
                        userAccountId: userNumber,
                        amount: {
                            value: Constants_1.INTEGERS.ZERO,
                            denomination: types_1.AmountDenomination.Par,
                            reference: types_1.AmountReference.Target,
                        },
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    numMarkets = 4;
                    return [4 /*yield*/, Promise.all([
                            SoloHelpers_1.setupMarkets(solo, accounts, numMarkets),
                            solo.contracts.send(solo.contracts.daiMigrator.methods.addMigrator(migrator), { from: admin }),
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
            var createdContract, _a, migratorIsMigrator, randoIsMigrator, userIsMigrator;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0: return [4 /*yield*/, Deploy_1.deployContract(solo, DaiMigrator_json_1.default, [[migrator, rando]])];
                    case 1:
                        createdContract = _b.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.call(createdContract.methods.g_migrators(migrator)),
                                solo.contracts.call(createdContract.methods.g_migrators(rando)),
                                solo.contracts.call(createdContract.methods.g_migrators(user)),
                            ])];
                    case 2:
                        _a = _b.sent(), migratorIsMigrator = _a[0], randoIsMigrator = _a[1], userIsMigrator = _a[2];
                        expect(migratorIsMigrator).toEqual(true);
                        expect(randoIsMigrator).toEqual(true);
                        expect(userIsMigrator).toEqual(false);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('addMigrator', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var isMigrator;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.daiMigrator.methods.addMigrator(rando), { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.call(solo.contracts.daiMigrator.methods.g_migrators(rando))];
                    case 2:
                        isMigrator = _a.sent();
                        expect(isMigrator).toEqual(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.daiMigrator.methods.addMigrator(rando), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('removeMigrator', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var isMigrator;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.daiMigrator.methods.removeMigrator(migrator), { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.call(solo.contracts.daiMigrator.methods.g_migrators(migrator))];
                    case 2:
                        isMigrator = _a.sent();
                        expect(isMigrator).toEqual(false);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.daiMigrator.methods.removeMigrator(migrator), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getTradeCost', function () {
        it('Succeeds for zero', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation.initiate()
                            .daiMigrate(defaultGlob)
                            .commit({ from: migrator })];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for pos->zero', function () { return __awaiter(_this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(1),
                            newInputParBN: new bignumber_js_1.default(0),
                            inputWeiBN: new bignumber_js_1.default(-1),
                        })];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual({
                            sign: true,
                            denomination: weiString,
                            ref: deltaString,
                            value: '1',
                        });
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for pos->less pos', function () { return __awaiter(_this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(2),
                            newInputParBN: new bignumber_js_1.default(1),
                            inputWeiBN: new bignumber_js_1.default(-1),
                        })];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual({
                            sign: true,
                            denomination: weiString,
                            ref: deltaString,
                            value: '1',
                        });
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for neg->zero', function () { return __awaiter(_this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(-1),
                            newInputParBN: new bignumber_js_1.default(0),
                            inputWeiBN: new bignumber_js_1.default(1),
                        })];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual({
                            sign: false,
                            denomination: weiString,
                            ref: deltaString,
                            value: '1',
                        });
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for neg->less neg', function () { return __awaiter(_this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(-2),
                            newInputParBN: new bignumber_js_1.default(-1),
                            inputWeiBN: new bignumber_js_1.default(1),
                        })];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual({
                            sign: false,
                            denomination: weiString,
                            ref: deltaString,
                            value: '1',
                        });
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for pos->neg', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(1),
                            newInputParBN: new bignumber_js_1.default(-1),
                            inputWeiBN: new bignumber_js_1.default(-2),
                        }), 'DaiMigrator: newInputPar cannot be negative')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for neg->pos', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(-1),
                            newInputParBN: new bignumber_js_1.default(1),
                            inputWeiBN: new bignumber_js_1.default(2),
                        }), 'DaiMigrator: newInputPar cannot be positive')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for neg->more neg', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(-1),
                            newInputParBN: new bignumber_js_1.default(-2),
                            inputWeiBN: new bignumber_js_1.default(-1),
                        }), 'DaiMigrator: inputWei must be positive')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for pos->more pos', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(1),
                            newInputParBN: new bignumber_js_1.default(2),
                            inputWeiBN: new bignumber_js_1.default(1),
                        }), 'DaiMigrator: inputWei must be negative')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for zero->pos', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(0),
                            newInputParBN: new bignumber_js_1.default(1),
                            inputWeiBN: new bignumber_js_1.default(1),
                        }), 'DaiMigrator: inputWei must be zero')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for zero->neg', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(callTradeCost({
                            oldInputParBN: new bignumber_js_1.default(0),
                            newInputParBN: new bignumber_js_1.default(-1),
                            inputWeiBN: new bignumber_js_1.default(-1),
                        }), 'DaiMigrator: inputWei must be zero')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for invalid markets', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(callTradeCost({
                            saiMarketBN: daiMarket,
                            daiMarketBN: saiMarket,
                            oldInputParBN: new bignumber_js_1.default(2),
                            newInputParBN: new bignumber_js_1.default(1),
                            inputWeiBN: new bignumber_js_1.default(-1),
                        }), 'DaiMigrator: Invalid markets')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-migrator', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate().daiMigrate(__assign({}, defaultGlob, { primaryAccountOwner: rando })).commit({ from: rando }), "DaiMigrator: Migrator not approved <" + rando.toLowerCase() + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
function callTradeCost(_a) {
    var _b = _a.saiMarketBN, saiMarketBN = _b === void 0 ? saiMarket : _b, _c = _a.daiMarketBN, daiMarketBN = _c === void 0 ? daiMarket : _c, _d = _a.migratorAccount, migratorAccount = _d === void 0 ? {
        owner: migrator,
        number: migratorNumber.toFixed(0),
    } : _d, oldInputParBN = _a.oldInputParBN, newInputParBN = _a.newInputParBN, inputWeiBN = _a.inputWeiBN;
    return __awaiter(this, void 0, void 0, function () {
        var result;
        return __generator(this, function (_e) {
            switch (_e.label) {
                case 0: return [4 /*yield*/, solo.contracts.daiMigrator.methods.getTradeCost(saiMarketBN.toFixed(0), daiMarketBN.toFixed(0), {
                        owner: user,
                        number: '0',
                    }, migratorAccount, bnToValue(oldInputParBN), bnToValue(newInputParBN), bnToValue(inputWeiBN), []).call()];
                case 1:
                    result = _e.sent();
                    return [2 /*return*/, {
                            sign: result[0],
                            denomination: result[1],
                            ref: result[2],
                            value: result[3],
                        }];
            }
        });
    });
}
function bnToValue(bn) {
    return {
        sign: bn.gt(0),
        value: bn.abs().toFixed(0),
    };
}
//# sourceMappingURL=DaiMigrator.test.js.map