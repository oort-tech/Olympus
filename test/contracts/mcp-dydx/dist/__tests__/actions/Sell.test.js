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
var exchange_wrappers_1 = require("@dydxprotocol/exchange-wrappers");
var Expect_1 = require("../../src/lib/Expect");
var types_1 = require("../../src/types");
var who;
var operator;
var solo;
var accounts;
var accountNumber = Constants_1.INTEGERS.ZERO;
var makerMarket = Constants_1.INTEGERS.ZERO;
var takerMarket = Constants_1.INTEGERS.ONE;
var collateralMarket = new bignumber_js_1.default(2);
var collateralAmount = new bignumber_js_1.default(1000000);
var zero = new bignumber_js_1.default(0);
var makerPar = new bignumber_js_1.default(100);
var makerWei = new bignumber_js_1.default(150);
var takerPar = new bignumber_js_1.default(200);
var takerWei = new bignumber_js_1.default(300);
var makerToken;
var takerToken;
var defaultGlob;
var testOrder;
var EXCHANGE_ADDRESS;
describe('Sell', function () {
    var snapshotId;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r, defaultIndex;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    EXCHANGE_ADDRESS = solo.testing.exchangeWrapper.getExchangeAddress();
                    accounts = r.accounts;
                    who = solo.getDefaultAccount();
                    operator = accounts[6];
                    makerToken = solo.testing.tokenA;
                    takerToken = solo.testing.tokenB;
                    testOrder = {
                        type: exchange_wrappers_1.OrderType.Test,
                        exchangeWrapperAddress: solo.testing.exchangeWrapper.getAddress(),
                        originator: who,
                        makerToken: makerToken.getAddress(),
                        takerToken: takerToken.getAddress(),
                        makerAmount: makerWei,
                        takerAmount: takerWei,
                        allegedTakerAmount: takerWei,
                        desiredMakerAmount: makerWei,
                    };
                    defaultGlob = {
                        primaryAccountOwner: who,
                        primaryAccountId: accountNumber,
                        takerMarketId: takerMarket,
                        makerMarketId: makerMarket,
                        order: testOrder,
                        amount: {
                            value: takerWei.times(-1),
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
                        borrow: takerWei.div(takerPar),
                        supply: takerWei.div(takerPar),
                    };
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setMarketIndex(makerMarket, defaultIndex),
                            solo.testing.setMarketIndex(takerMarket, defaultIndex),
                            solo.testing.setAccountBalance(who, accountNumber, collateralMarket, collateralAmount),
                            solo.testing.tokenA.setMaximumSoloAllowance(who),
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
    it('Basic sell test', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, _b, _c;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueMakerTokenToWrapper(makerWei),
                        issueTakerTokenToSolo(takerWei),
                        setTakerBalance(takerPar),
                    ])];
                case 1:
                    _d.sent();
                    return [4 /*yield*/, expectSellOkay({})];
                case 2:
                    txResult = _d.sent();
                    console.log("\tSell gas used: " + txResult.gasUsed);
                    _b = (_a = Promise).all;
                    return [4 /*yield*/, expectPars(makerPar, zero)];
                case 3:
                    _c = [
                        _d.sent()
                    ];
                    return [4 /*yield*/, expectSoloBalances(makerWei, zero)];
                case 4:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectWrapperBalances(zero, zero)];
                case 5:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectExchangeBalances(zero, takerWei)];
                case 6: return [4 /*yield*/, _b.apply(_a, [_c.concat([
                            _d.sent()
                        ])])];
                case 7:
                    _d.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for events', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, _a, makerIndex, takerIndex, collateralIndex, logs, operationLog, takerIndexLog, makerIndexLog, collateralIndexLog, sellLog;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.permissions.approveOperator(operator, { from: who }),
                        issueMakerTokenToWrapper(makerWei),
                        issueTakerTokenToSolo(takerWei),
                        setTakerBalance(takerPar),
                    ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, expectSellOkay({}, { from: operator })];
                case 2:
                    txResult = _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getMarketCachedIndex(makerMarket),
                            solo.getters.getMarketCachedIndex(takerMarket),
                            solo.getters.getMarketCachedIndex(collateralMarket),
                            expectPars(makerPar, zero),
                            expectSoloBalances(makerWei, zero),
                            expectWrapperBalances(zero, zero),
                            expectExchangeBalances(zero, takerWei),
                        ])];
                case 3:
                    _a = _b.sent(), makerIndex = _a[0], takerIndex = _a[1], collateralIndex = _a[2];
                    logs = solo.logs.parseLogs(txResult);
                    expect(logs.length).toEqual(5);
                    operationLog = logs[0];
                    expect(operationLog.name).toEqual('LogOperation');
                    expect(operationLog.args.sender).toEqual(operator);
                    takerIndexLog = logs[1];
                    expect(takerIndexLog.name).toEqual('LogIndexUpdate');
                    expect(takerIndexLog.args.market).toEqual(takerMarket);
                    expect(takerIndexLog.args.index).toEqual(takerIndex);
                    makerIndexLog = logs[2];
                    expect(makerIndexLog.name).toEqual('LogIndexUpdate');
                    expect(makerIndexLog.args.market).toEqual(makerMarket);
                    expect(makerIndexLog.args.index).toEqual(makerIndex);
                    collateralIndexLog = logs[3];
                    expect(collateralIndexLog.name).toEqual('LogIndexUpdate');
                    expect(collateralIndexLog.args.market).toEqual(collateralMarket);
                    expect(collateralIndexLog.args.index).toEqual(collateralIndex);
                    sellLog = logs[4];
                    expect(sellLog.name).toEqual('LogSell');
                    expect(sellLog.args.accountOwner).toEqual(who);
                    expect(sellLog.args.accountNumber).toEqual(accountNumber);
                    expect(sellLog.args.takerMarket).toEqual(takerMarket);
                    expect(sellLog.args.makerMarket).toEqual(makerMarket);
                    expect(sellLog.args.takerUpdate).toEqual({ newPar: zero, deltaWei: takerWei.times(-1) });
                    expect(sellLog.args.makerUpdate).toEqual({ newPar: makerPar, deltaWei: makerWei });
                    expect(sellLog.args.exchangeWrapper).toEqual(solo.testing.exchangeWrapper.getAddress());
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero makerAmount', function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, _b, _c;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueTakerTokenToSolo(takerWei),
                        setTakerBalance(takerPar),
                    ])];
                case 1:
                    _d.sent();
                    return [4 /*yield*/, expectSellOkay({
                            order: __assign({}, testOrder, { makerAmount: zero }),
                        })];
                case 2:
                    _d.sent();
                    _b = (_a = Promise).all;
                    return [4 /*yield*/, expectPars(zero, zero)];
                case 3:
                    _c = [
                        _d.sent()
                    ];
                    return [4 /*yield*/, expectSoloBalances(zero, zero)];
                case 4:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectWrapperBalances(zero, zero)];
                case 5:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectExchangeBalances(zero, takerWei)];
                case 6: return [4 /*yield*/, _b.apply(_a, [_c.concat([
                            _d.sent()
                        ])])];
                case 7:
                    _d.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for zero takerAmount', function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, _b, _c;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueMakerTokenToWrapper(makerWei),
                        setTakerBalance(takerPar),
                    ])];
                case 1:
                    _d.sent();
                    return [4 /*yield*/, expectSellOkay({
                            order: __assign({}, testOrder, { takerAmount: zero }),
                            amount: {
                                value: zero,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })];
                case 2:
                    _d.sent();
                    _b = (_a = Promise).all;
                    return [4 /*yield*/, expectPars(makerPar, takerPar)];
                case 3:
                    _c = [
                        _d.sent()
                    ];
                    return [4 /*yield*/, expectSoloBalances(makerWei, zero)];
                case 4:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectWrapperBalances(zero, zero)];
                case 5:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectExchangeBalances(zero, zero)];
                case 6: return [4 /*yield*/, _b.apply(_a, [_c.concat([
                            _d.sent()
                        ])])];
                case 7:
                    _d.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds and sets status to Normal', function () { return __awaiter(_this, void 0, void 0, function () {
        var status;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueMakerTokenToWrapper(makerWei),
                        issueTakerTokenToSolo(takerWei),
                        setTakerBalance(takerPar),
                        solo.testing.setAccountStatus(who, accountNumber, types_1.AccountStatus.Liquidating),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectSellOkay({})];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getAccountStatus(who, accountNumber)];
                case 3:
                    status = _a.sent();
                    expect(status).toEqual(types_1.AccountStatus.Normal);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for local operator', function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, _b, _c;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueMakerTokenToWrapper(makerWei),
                        issueTakerTokenToSolo(takerWei),
                        setTakerBalance(takerPar),
                        solo.permissions.approveOperator(operator, { from: who }),
                    ])];
                case 1:
                    _d.sent();
                    return [4 /*yield*/, expectSellOkay({}, { from: operator })];
                case 2:
                    _d.sent();
                    _b = (_a = Promise).all;
                    return [4 /*yield*/, expectPars(makerPar, zero)];
                case 3:
                    _c = [
                        _d.sent()
                    ];
                    return [4 /*yield*/, expectSoloBalances(makerWei, zero)];
                case 4:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectWrapperBalances(zero, zero)];
                case 5:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectExchangeBalances(zero, takerWei)];
                case 6: return [4 /*yield*/, _b.apply(_a, [_c.concat([
                            _d.sent()
                        ])])];
                case 7:
                    _d.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for global operator', function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, _b, _c;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueMakerTokenToWrapper(makerWei),
                        issueTakerTokenToSolo(takerWei),
                        setTakerBalance(takerPar),
                        solo.admin.setGlobalOperator(operator, true, { from: accounts[0] }),
                    ])];
                case 1:
                    _d.sent();
                    return [4 /*yield*/, expectSellOkay({}, { from: operator })];
                case 2:
                    _d.sent();
                    _b = (_a = Promise).all;
                    return [4 /*yield*/, expectPars(makerPar, zero)];
                case 3:
                    _c = [
                        _d.sent()
                    ];
                    return [4 /*yield*/, expectSoloBalances(makerWei, zero)];
                case 4:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectWrapperBalances(zero, zero)];
                case 5:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, expectExchangeBalances(zero, takerWei)];
                case 6: return [4 /*yield*/, _b.apply(_a, [_c.concat([
                            _d.sent()
                        ])])];
                case 7:
                    _d.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectSellRevert({}, 'Storage: Unpermissioned operator', { from: operator })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for positive takerAmount', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectSellRevert({
                        amount: {
                            value: takerWei,
                            denomination: types_1.AmountDenomination.Actual,
                            reference: types_1.AmountReference.Delta,
                        },
                    }, 'Exchange: Cannot exchange positive')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for takerToken equals makerToken', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectSellRevert({
                        takerMarketId: makerMarket,
                        order: __assign({}, testOrder, { takerToken: makerToken.getAddress() }),
                    }, 'OperationImpl: Duplicate markets in action')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for Solo without enough tokens', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueMakerTokenToWrapper(makerWei),
                        issueTakerTokenToSolo(takerWei.div(2)),
                        setTakerBalance(takerPar),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectSellRevert({}, 'Token: Transfer failed')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for exchangeWrapper without enough tokens', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        issueMakerTokenToWrapper(makerWei.div(2)),
                        issueTakerTokenToSolo(takerWei),
                        setTakerBalance(takerPar),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectSellRevert({}, 'Token: TransferFrom failed')];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function expectPars(expectedMakerPar, expectedTakerPar) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, makerBalance, balances;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        makerToken.getBalance(solo.contracts.soloMargin.options.address),
                        solo.getters.getAccountBalances(who, accountNumber),
                    ])];
                case 1:
                    _a = _b.sent(), makerBalance = _a[0], balances = _a[1];
                    expect(makerBalance).toEqual(expectedMakerPar.times(makerWei).div(makerPar));
                    balances.forEach(function (balance, i) {
                        if (i === makerMarket.toNumber()) {
                            expect(balance.par).toEqual(expectedMakerPar);
                        }
                        else if (i === takerMarket.toNumber()) {
                            expect(balance.par).toEqual(expectedTakerPar);
                        }
                        else if (i === collateralMarket.toNumber()) {
                            expect(balance.par).toEqual(collateralAmount);
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
function expectWrapperBalances(expectedMakerWei, expectedTakerWei) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, makerWei, takerWei;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        makerToken.getBalance(solo.testing.exchangeWrapper.getAddress()),
                        takerToken.getBalance(solo.testing.exchangeWrapper.getAddress()),
                    ])];
                case 1:
                    _a = _b.sent(), makerWei = _a[0], takerWei = _a[1];
                    expect(makerWei).toEqual(expectedMakerWei);
                    expect(takerWei).toEqual(expectedTakerWei);
                    return [2 /*return*/];
            }
        });
    });
}
function expectExchangeBalances(expectedMakerWei, expectedTakerWei) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, makerWei, takerWei;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        makerToken.getBalance(EXCHANGE_ADDRESS),
                        takerToken.getBalance(EXCHANGE_ADDRESS),
                    ])];
                case 1:
                    _a = _b.sent(), makerWei = _a[0], takerWei = _a[1];
                    expect(makerWei).toEqual(expectedMakerWei);
                    expect(takerWei).toEqual(expectedTakerWei);
                    return [2 /*return*/];
            }
        });
    });
}
function expectSoloBalances(expectedMakerWei, expectedTakerWei) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, makerWei, takerWei;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        makerToken.getBalance(solo.contracts.soloMargin.options.address),
                        takerToken.getBalance(solo.contracts.soloMargin.options.address),
                    ])];
                case 1:
                    _a = _b.sent(), makerWei = _a[0], takerWei = _a[1];
                    expect(makerWei).toEqual(expectedMakerWei);
                    expect(takerWei).toEqual(expectedTakerWei);
                    return [2 /*return*/];
            }
        });
    });
}
function issueMakerTokenToWrapper(amount) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, makerToken.issueTo(amount, solo.testing.exchangeWrapper.getAddress())];
        });
    });
}
function issueTakerTokenToSolo(amount) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, takerToken.issueTo(amount, solo.contracts.soloMargin.options.address)];
        });
    });
}
function setTakerBalance(par) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.testing.setAccountBalance(who, accountNumber, takerMarket, par)];
        });
    });
}
function expectSellOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob;
        return __generator(this, function (_a) {
            combinedGlob = __assign({}, defaultGlob, glob);
            return [2 /*return*/, solo.operation.initiate().sell(combinedGlob).commit(options)];
        });
    });
}
function expectSellRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectSellOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Sell.test.js.map