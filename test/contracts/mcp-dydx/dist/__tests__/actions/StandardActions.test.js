"use strict";
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
var types_1 = require("../../src/types");
var solo;
var tokens;
var accountOwner;
var amount = new bignumber_js_1.default(123456);
var accountNumber = Constants_1.INTEGERS.ZERO;
var markets = [
    types_1.MarketId.ETH,
    types_1.MarketId.WETH,
    types_1.MarketId.SAI,
    types_1.MarketId.USDC,
];
describe('StandardActions', function () {
    var snapshotId;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r, soloAddress;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    accountOwner = r.accounts[5];
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    soloAddress = solo.contracts.testSoloMargin.options.address;
                    // setup markets
                    return [4 /*yield*/, solo.testing.priceOracle.setPrice(solo.weth.getAddress(), new bignumber_js_1.default('1e40'))];
                case 3:
                    // setup markets
                    _a.sent();
                    return [4 /*yield*/, solo.admin.addMarket(solo.weth.getAddress(), solo.testing.priceOracle.getAddress(), solo.testing.interestSetter.getAddress(), Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO, { from: r.accounts[0] })];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, r.accounts, 2)];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketTokenAddress(new bignumber_js_1.default(0)),
                            solo.getters.getMarketTokenAddress(new bignumber_js_1.default(1)),
                            solo.getters.getMarketTokenAddress(new bignumber_js_1.default(2)),
                        ])];
                case 6:
                    tokens = _a.sent();
                    // set balances
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(accountOwner, accountNumber, new bignumber_js_1.default(0), amount.times(2)),
                            solo.testing.setAccountBalance(accountOwner, accountNumber, new bignumber_js_1.default(1), amount.times(2)),
                            solo.testing.setAccountBalance(accountOwner, accountNumber, new bignumber_js_1.default(2), amount.times(2)),
                        ])];
                case 7:
                    // set balances
                    _a.sent();
                    // give tokens
                    return [4 /*yield*/, Promise.all([
                            solo.weth.wrap(accountOwner, amount.times(3)),
                            solo.testing.tokenA.issueTo(amount, accountOwner),
                            solo.testing.tokenB.issueTo(amount, accountOwner),
                            solo.testing.tokenA.issueTo(amount.times(2), soloAddress),
                            solo.testing.tokenB.issueTo(amount.times(2), soloAddress),
                        ])];
                case 8:
                    // give tokens
                    _a.sent();
                    return [4 /*yield*/, solo.weth.transfer(accountOwner, soloAddress, amount.times(2))];
                case 9:
                    _a.sent();
                    // set allowances
                    return [4 /*yield*/, Promise.all([
                            solo.weth.setMaximumSoloAllowance(accountOwner),
                            solo.testing.tokenA.setMaximumSoloAllowance(accountOwner),
                            solo.testing.tokenB.setMaximumSoloAllowance(accountOwner),
                        ])];
                case 10:
                    // set allowances
                    _a.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 11:
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
    describe('deposit', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var i, balance0, marketId, balance1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        i = 0;
                        _a.label = 1;
                    case 1:
                        if (!(i < markets.length)) return [3 /*break*/, 10];
                        return [4 /*yield*/, EVM_1.resetEVM(snapshotId)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, getBalance()];
                    case 3:
                        balance0 = _a.sent();
                        marketId = markets[i];
                        return [4 /*yield*/, solo.standardActions.deposit({
                                accountNumber: accountNumber,
                                accountOwner: accountOwner,
                                amount: amount,
                                marketId: marketId,
                                options: { gasPrice: '0x00' },
                            })];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, expectAccountWei(marketId, amount.times(3))];
                    case 5:
                        _a.sent();
                        if (!marketId.eq(types_1.MarketId.ETH)) return [3 /*break*/, 7];
                        return [4 /*yield*/, getBalance()];
                    case 6:
                        balance1 = _a.sent();
                        expect(balance1.lt(balance0)).toEqual(true);
                        return [3 /*break*/, 9];
                    case 7: return [4 /*yield*/, expectTokens(marketId, Constants_1.INTEGERS.ZERO)];
                    case 8:
                        _a.sent();
                        _a.label = 9;
                    case 9:
                        i += 1;
                        return [3 /*break*/, 1];
                    case 10: return [2 /*return*/];
                }
            });
        }); });
    });
    describe('withdraw', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var i, balance0, marketId, balance1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        i = 0;
                        _a.label = 1;
                    case 1:
                        if (!(i < markets.length)) return [3 /*break*/, 10];
                        return [4 /*yield*/, EVM_1.resetEVM(snapshotId)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, getBalance()];
                    case 3:
                        balance0 = _a.sent();
                        marketId = markets[i];
                        return [4 /*yield*/, solo.standardActions.withdraw({
                                accountNumber: accountNumber,
                                accountOwner: accountOwner,
                                amount: amount,
                                marketId: marketId,
                                options: { gasPrice: '0x00' },
                            })];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, expectAccountWei(marketId, amount)];
                    case 5:
                        _a.sent();
                        if (!marketId.eq(types_1.MarketId.ETH)) return [3 /*break*/, 7];
                        return [4 /*yield*/, getBalance()];
                    case 6:
                        balance1 = _a.sent();
                        expect(balance1.gt(balance0)).toEqual(true);
                        return [3 /*break*/, 9];
                    case 7: return [4 /*yield*/, expectTokens(marketId, amount.times(2))];
                    case 8:
                        _a.sent();
                        _a.label = 9;
                    case 9:
                        i += 1;
                        return [3 /*break*/, 1];
                    case 10: return [2 /*return*/];
                }
            });
        }); });
    });
    describe('withdrawToZero', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var i, balance0, marketId, balance1;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        i = 0;
                        _a.label = 1;
                    case 1:
                        if (!(i < markets.length)) return [3 /*break*/, 11];
                        return [4 /*yield*/, EVM_1.resetEVM(snapshotId)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, getBalance()];
                    case 3:
                        balance0 = _a.sent();
                        marketId = markets[i];
                        return [4 /*yield*/, solo.testing.setAccountBalance(accountOwner, accountNumber, marketId, amount.times(2))];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, solo.standardActions.withdrawToZero({
                                accountNumber: accountNumber,
                                accountOwner: accountOwner,
                                marketId: marketId,
                                options: { gasPrice: '0x00' },
                            })];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, expectAccountWei(marketId, Constants_1.INTEGERS.ZERO)];
                    case 6:
                        _a.sent();
                        if (!marketId.eq(types_1.MarketId.ETH)) return [3 /*break*/, 8];
                        return [4 /*yield*/, getBalance()];
                    case 7:
                        balance1 = _a.sent();
                        expect(balance1.gt(balance0)).toEqual(true);
                        return [3 /*break*/, 10];
                    case 8: return [4 /*yield*/, expectTokens(marketId, amount.times(3))];
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
    });
});
function realify(marketId) {
    return marketId.isNegative() ? Constants_1.INTEGERS.ZERO : marketId;
}
function getBalance() {
    return __awaiter(this, void 0, void 0, function () {
        var _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    _a = bignumber_js_1.default.bind;
                    return [4 /*yield*/, solo.web3.eth.getBalance(accountOwner)];
                case 1: return [2 /*return*/, new (_a.apply(bignumber_js_1.default, [void 0, _b.sent()]))()];
            }
        });
    });
}
function expectAccountWei(marketId, expectedWei) {
    return __awaiter(this, void 0, void 0, function () {
        var accountWei;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getAccountWei(accountOwner, accountNumber, realify(marketId))];
                case 1:
                    accountWei = _a.sent();
                    expect(accountWei).toEqual(expectedWei);
                    return [2 /*return*/];
            }
        });
    });
}
function expectTokens(marketId, amount) {
    return __awaiter(this, void 0, void 0, function () {
        var accountTokens;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.token.getBalance(tokens[marketId.toNumber()], accountOwner)];
                case 1:
                    accountTokens = _a.sent();
                    expect(accountTokens).toEqual(amount);
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=StandardActions.test.js.map