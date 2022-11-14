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
var BytesHelper_1 = require("../../src/lib/BytesHelper");
var exchange_wrappers_1 = require("@dydxprotocol/exchange-wrappers");
var types_1 = require("../../src/types");
var solo;
var accounts;
var snapshotId;
var admin;
var owner1;
var owner2;
var operator;
var testOrder;
var amount = new bignumber_js_1.default(10000);
var accountNumber1 = new bignumber_js_1.default(111);
var accountNumber2 = new bignumber_js_1.default(222);
var market1 = Constants_1.INTEGERS.ZERO;
var market2 = Constants_1.INTEGERS.ONE;
var wethMarket = new bignumber_js_1.default(3);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(100);
var negPar = par.times(-1);
var PRIMARY_REVERT_REASON = 'PayableProxyForSoloMargin: Sender must be primary account';
var SECONDARY_REVERT_REASON = 'PayableProxyForSoloMargin: Sender must be secondary account';
var tradeId = new bignumber_js_1.default('5678');
var amountBlob = {
    value: zero,
    reference: types_1.AmountReference.Target,
    denomination: types_1.AmountDenomination.Principal,
};
var bigBlob;
describe('PayableProxy', function () {
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
                    owner1 = solo.getDefaultAccount();
                    owner2 = accounts[3];
                    operator = accounts[6];
                    testOrder = {
                        type: exchange_wrappers_1.OrderType.Test,
                        exchangeWrapperAddress: solo.testing.exchangeWrapper.getAddress(),
                        originator: operator,
                        makerToken: solo.testing.tokenA.getAddress(),
                        takerToken: solo.testing.tokenB.getAddress(),
                        makerAmount: zero,
                        takerAmount: zero,
                        allegedTakerAmount: zero,
                        desiredMakerAmount: zero,
                    };
                    bigBlob = {
                        amount: amountBlob,
                        primaryAccountOwner: owner1,
                        primaryAccountId: accountNumber1,
                        toAccountOwner: owner2,
                        toAccountId: accountNumber2,
                        otherAccountOwner: owner2,
                        otherAccountId: accountNumber2,
                        liquidAccountOwner: owner2,
                        liquidAccountId: accountNumber2,
                        vaporAccountOwner: owner2,
                        vaporAccountId: accountNumber2,
                        marketId: market1,
                        makerMarketId: market1,
                        inputMarketId: market1,
                        liquidMarketId: market1,
                        vaporMarketId: market1,
                        takerMarketId: market2,
                        outputMarketId: market2,
                        payoutMarketId: market2,
                        from: owner1,
                        to: owner1,
                        order: testOrder,
                        autoTrader: solo.testing.autoTrader.getAddress(),
                        data: BytesHelper_1.toBytes(tradeId),
                        callee: solo.testing.callee.getAddress(),
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            SoloHelpers_1.setupMarkets(solo, accounts),
                            solo.testing.autoTrader.setData(tradeId, amountBlob),
                            solo.testing.priceOracle.setPrice(solo.weth.getAddress(), new bignumber_js_1.default('1e40')),
                        ])];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, solo.admin.addMarket(solo.weth.getAddress(), solo.testing.priceOracle.getAddress(), solo.testing.interestSetter.getAddress(), zero, zero, { from: admin })];
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
    it('Fails for other accounts', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: 
                // deposit
                return [4 /*yield*/, Expect_1.expectThrow(newOperation().deposit(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 1:
                    // deposit
                    _a.sent();
                    // withdraw
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().withdraw(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 2:
                    // withdraw
                    _a.sent();
                    // transfer
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().transfer(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 3:
                    // transfer
                    _a.sent();
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().transfer(bigBlob).commit(), SECONDARY_REVERT_REASON)];
                case 4:
                    _a.sent();
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().transfer(bigBlob).commit({ from: owner2 }), PRIMARY_REVERT_REASON)];
                case 5:
                    _a.sent();
                    // buy
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().buy(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 6:
                    // buy
                    _a.sent();
                    // sell
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().sell(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 7:
                    // sell
                    _a.sent();
                    // trade
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().trade(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 8:
                    // trade
                    _a.sent();
                    // liquidate
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().liquidate(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 9:
                    // liquidate
                    _a.sent();
                    // vaporize
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().vaporize(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 10:
                    // vaporize
                    _a.sent();
                    // call
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation().call(bigBlob).commit({ from: operator }), PRIMARY_REVERT_REASON)];
                case 11:
                    // call
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for withdrawing to zero', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.weth.wrap(owner1, amount)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.weth.transfer(owner1, solo.contracts.soloMargin.options.address, amount),
                            solo.testing.setAccountBalance(owner1, accountNumber1, wethMarket, amount),
                        ])];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, Expect_1.expectThrow(newOperation(Constants_1.ADDRESSES.ZERO).withdraw(__assign({}, bigBlob, { marketId: wethMarket, to: solo.contracts.payableProxy.options.address })).commit(), 'PayableProxyForSoloMargin: Must set sendEthTo')];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for other accounts', function () { return __awaiter(_this, void 0, void 0, function () {
        var exchangeBlob;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.permissions.approveOperator(solo.testing.autoTrader.getAddress(), { from: owner1 }),
                        solo.testing.tokenB.issueTo(par, solo.contracts.soloMargin.options.address),
                        solo.testing.setAccountBalance(owner1, accountNumber1, new bignumber_js_1.default(2), par),
                        solo.testing.setAccountBalance(owner2, accountNumber2, market1, negPar),
                        solo.testing.setAccountBalance(owner2, accountNumber2, market2, par),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, newOperation().deposit(bigBlob).commit()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, newOperation().withdraw(bigBlob).commit()];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, newOperation().transfer(__assign({}, bigBlob, { toAccountOwner: owner1, toAccountId: accountNumber2 })).commit()];
                case 4:
                    _a.sent();
                    exchangeBlob = __assign({}, bigBlob, { order: __assign({}, testOrder, { originator: owner1 }) });
                    return [4 /*yield*/, newOperation().buy(exchangeBlob).commit()];
                case 5:
                    _a.sent();
                    return [4 /*yield*/, newOperation().sell(exchangeBlob).commit()];
                case 6:
                    _a.sent();
                    return [4 /*yield*/, newOperation().trade(__assign({}, bigBlob, { otherAccountOwner: owner1, otherAccountId: accountNumber2 })).commit()];
                case 7:
                    _a.sent();
                    return [4 /*yield*/, newOperation().call(__assign({}, bigBlob, { data: BytesHelper_1.toBytes(tradeId, tradeId) })).commit()];
                case 8:
                    _a.sent();
                    return [4 /*yield*/, newOperation().liquidate(bigBlob).commit()];
                case 9:
                    _a.sent();
                    return [4 /*yield*/, newOperation().vaporize(bigBlob).commit()];
                case 10:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for wrapping ETH', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, newOperation().deposit(__assign({}, bigBlob, { amount: {
                            value: amount,
                            reference: types_1.AmountReference.Delta,
                            denomination: types_1.AmountDenomination.Actual,
                        }, marketId: wethMarket, from: solo.contracts.payableProxy.options.address })).commit({ from: owner1, value: amount.toNumber() })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for un-wrapping ETH', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.weth.wrap(owner1, amount)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.weth.transfer(owner1, solo.contracts.soloMargin.options.address, amount),
                            solo.testing.setAccountBalance(owner1, accountNumber1, wethMarket, amount),
                        ])];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, newOperation().withdraw(__assign({}, bigBlob, { amount: {
                                value: amount.times(-1),
                                reference: types_1.AmountReference.Delta,
                                denomination: types_1.AmountDenomination.Actual,
                            }, marketId: wethMarket, to: solo.contracts.payableProxy.options.address })).commit({ from: owner1, value: amount.toNumber() })];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for wrapping and un-wrapping ETH', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, newOperation().deposit(__assign({}, bigBlob, { amount: {
                            value: amount,
                            reference: types_1.AmountReference.Delta,
                            denomination: types_1.AmountDenomination.Actual,
                        }, marketId: wethMarket, from: solo.contracts.payableProxy.options.address })).commit({ from: owner1, value: amount.times(2).toNumber() })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function newOperation(sendEthTo) {
    return solo.operation.initiate({ usePayableProxy: true, sendEthTo: sendEthTo || owner1 });
}
//# sourceMappingURL=PayableProxy.test.js.map