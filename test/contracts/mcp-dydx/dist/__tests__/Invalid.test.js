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
var Solo_1 = require("./helpers/Solo");
var EVM_1 = require("./helpers/EVM");
var SoloHelpers_1 = require("./helpers/SoloHelpers");
var Expect_1 = require("../src/lib/Expect");
var Constants_1 = require("../src/lib/Constants");
var types_1 = require("../src/types");
var solo;
var accounts;
var owner1;
var account1;
var accountNumber1 = new bignumber_js_1.default(11);
var market1 = new bignumber_js_1.default(1);
var market2 = new bignumber_js_1.default(2);
var wei = new bignumber_js_1.default(150);
var zero = new bignumber_js_1.default(0);
var zeroAction = {
    amount: {
        sign: false,
        denomination: types_1.AmountDenomination.Actual,
        ref: types_1.AmountReference.Delta,
        value: '0',
    },
    accountId: '0',
    actionType: types_1.ActionType.Deposit,
    primaryMarketId: '0',
    secondaryMarketId: '0',
    otherAddress: Constants_1.ADDRESSES.ZERO,
    otherAccountId: '0',
    data: [],
};
describe('Invalid', function () {
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
                    owner1 = accounts[2];
                    account1 = {
                        owner: owner1,
                        number: accountNumber1.toFixed(0),
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 4:
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
    it('Fails for invalid denomination', function () { return __awaiter(_this, void 0, void 0, function () {
        var invalidDenomination;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.tokenA.issueTo(wei, owner1),
                        solo.testing.tokenA.setMaximumSoloAllowance(owner1),
                    ])];
                case 1:
                    _a.sent();
                    invalidDenomination = 2;
                    return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate()
                            .deposit({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            marketId: market1,
                            amount: {
                                value: wei,
                                denomination: invalidDenomination,
                                reference: types_1.AmountReference.Delta,
                            },
                            from: owner1,
                        })
                            .commit({ from: owner1 }))];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for invalid reference', function () { return __awaiter(_this, void 0, void 0, function () {
        var invalidReference;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.tokenA.issueTo(wei, owner1),
                        solo.testing.tokenA.setMaximumSoloAllowance(owner1),
                    ])];
                case 1:
                    _a.sent();
                    invalidReference = 2;
                    return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate()
                            .deposit({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            marketId: market1,
                            amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: invalidReference,
                            },
                            from: owner1,
                        })
                            .commit({ from: owner1 }))];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for invalid action type', function () { return __awaiter(_this, void 0, void 0, function () {
        var invalidActionType, action;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    invalidActionType = 9;
                    action = __assign({}, zeroAction, { actionType: invalidActionType });
                    return [4 /*yield*/, Expect_1.expectThrow(operate([account1], [action]))];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for zero actions and zero accounts', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(operate([], []), 'OperationImpl: Cannot have zero actions')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for zero actions', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(operate([account1], []), 'OperationImpl: Cannot have zero actions')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for zero accounts', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(operate([], [zeroAction]), 'OperationImpl: Cannot have zero accounts')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for actions that use OOB account', function () { return __awaiter(_this, void 0, void 0, function () {
        var action;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    action = __assign({}, zeroAction, { accountId: '1' });
                    return [4 /*yield*/, Expect_1.expectThrow(operate([account1, account1], [action]), 'OperationImpl: Cannot duplicate accounts')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for duplicate accounts', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(operate([account1, account1], [zeroAction]), 'OperationImpl: Cannot duplicate accounts')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for zero price', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.tokenA.issueTo(wei, owner1),
                        solo.testing.tokenA.setMaximumSoloAllowance(owner1),
                        solo.testing.priceOracle.setPrice(solo.testing.tokenA.getAddress(), zero),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Expect_1.expectThrow(operate([account1], [zeroAction]), 'Storage: Price cannot be zero')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for time past max uint32', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, EVM_1.fastForward(4294967296)];
                case 1:
                    _a.sent(); // 2^32
                    return [4 /*yield*/, Expect_1.expectThrow(operate([account1], [zeroAction]), 'Math: Unsafe cast to uint32')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for borrow amount less than the minimum', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.tokenB.issueTo(wei.times(2), owner1),
                        solo.testing.tokenB.setMaximumSoloAllowance(owner1),
                        solo.testing.tokenC.issueTo(wei, solo.contracts.soloMargin.options.address),
                        solo.testing.priceOracle.setPrice(solo.testing.tokenB.getAddress(), Constants_1.INTEGERS.ONE),
                        solo.testing.priceOracle.setPrice(solo.testing.tokenC.getAddress(), Constants_1.INTEGERS.ONE),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate()
                            .deposit({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            marketId: market1,
                            amount: {
                                value: wei.times(2),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                            from: owner1,
                        })
                            .withdraw({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            marketId: market2,
                            amount: {
                                value: wei.times(-1),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                            to: owner1,
                        })
                            .commit({ from: owner1 }), 'Storage: Borrow value too low')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for undercollateralized account', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.tokenB.issueTo(wei, owner1),
                        solo.testing.tokenB.setMaximumSoloAllowance(owner1),
                        solo.testing.tokenC.issueTo(wei, solo.contracts.soloMargin.options.address),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Expect_1.expectThrow(solo.operation.initiate()
                            .deposit({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            marketId: market1,
                            amount: {
                                value: wei,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                            from: owner1,
                        })
                            .withdraw({
                            primaryAccountOwner: owner1,
                            primaryAccountId: accountNumber1,
                            marketId: market2,
                            amount: {
                                value: wei.times(-1),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                            to: owner1,
                        })
                            .commit({ from: owner1 }), 'OperationImpl: Undercollateralized account')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function operate(accounts, actions) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.contracts.send(solo.contracts.soloMargin.methods.operate(accounts, actions))];
        });
    });
}
//# sourceMappingURL=Invalid.test.js.map