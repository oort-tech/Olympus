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
var Solo_1 = require("./helpers/Solo");
var EVM_1 = require("./helpers/EVM");
var SoloHelpers_1 = require("./helpers/SoloHelpers");
var Constants_1 = require("../src/lib/Constants");
var Helpers_1 = require("../src/lib/Helpers");
var Expect_1 = require("../src/lib/Expect");
var txr;
var solo;
var accounts;
var admin;
var nonAdmin;
var operator;
var riskLimits;
var riskParams;
var soloAddress;
var oracleAddress;
var setterAddress;
var smallestDecimal = Helpers_1.stringToDecimal('1');
var defaultPrice = new bignumber_js_1.default(999);
var invalidPrice = new bignumber_js_1.default(0);
var defaultRate = new bignumber_js_1.default(0);
var defaultPremium = new bignumber_js_1.default(0);
var highPremium = new bignumber_js_1.default('0.2');
var defaultMarket = new bignumber_js_1.default(1);
var secondaryMarket = new bignumber_js_1.default(0);
var invalidMarket = new bignumber_js_1.default(101);
describe('Admin', function () {
    var snapshotId;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, r;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _b.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    admin = accounts[0];
                    nonAdmin = accounts[2];
                    operator = accounts[6];
                    expect(admin).not.toEqual(nonAdmin);
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getRiskLimits(),
                            solo.getters.getRiskParams(),
                            SoloHelpers_1.setupMarkets(solo, accounts, 2),
                        ])];
                case 3:
                    _a = _b.sent(), riskLimits = _a[0], riskParams = _a[1];
                    soloAddress = solo.contracts.soloMargin.options.address;
                    oracleAddress = solo.testing.priceOracle.getAddress();
                    setterAddress = solo.testing.interestSetter.getAddress();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 4:
                    snapshotId = _b.sent();
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
    // ============ Token Functions ============
    describe('#ownerWithdrawExcessTokens', function () {
        var recipient = Constants_1.ADDRESSES.TEST[1];
        var owner = Constants_1.ADDRESSES.TEST[0];
        var account1 = Constants_1.INTEGERS.ZERO;
        var account2 = Constants_1.INTEGERS.ONE;
        var market = Constants_1.INTEGERS.ZERO;
        var amount = new bignumber_js_1.default(100);
        it('Succeeds even if has more tokens than enough', function () { return __awaiter(_this, void 0, void 0, function () {
            var excess;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: 
                    // has 2X tokens but has X excess
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner, account1, market, amount.times(2)),
                            solo.testing.setAccountBalance(owner, account2, market, amount.times(-1)),
                            solo.testing.tokenA.issueTo(amount.times(2), soloAddress),
                        ])];
                    case 1:
                        // has 2X tokens but has X excess
                        _a.sent();
                        return [4 /*yield*/, solo.getters.getNumExcessTokens(market)];
                    case 2:
                        excess = _a.sent();
                        expect(excess).toEqual(amount);
                        return [4 /*yield*/, solo.admin.withdrawExcessTokens(market, recipient, { from: admin })];
                    case 3:
                        txr = _a.sent();
                        return [4 /*yield*/, expectBalances(txr, amount, amount)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds even if existing tokens arent enough', function () { return __awaiter(_this, void 0, void 0, function () {
            var excess;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: 
                    // has X tokens but has 3X excess
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner, account1, market, amount.times(-3)),
                            solo.testing.setAccountBalance(owner, account2, market, amount.times(1)),
                            solo.testing.tokenA.issueTo(amount, soloAddress),
                        ])];
                    case 1:
                        // has X tokens but has 3X excess
                        _a.sent();
                        return [4 /*yield*/, solo.getters.getNumExcessTokens(market)];
                    case 2:
                        excess = _a.sent();
                        expect(excess).toEqual(amount.times(3));
                        return [4 /*yield*/, solo.admin.withdrawExcessTokens(market, recipient, { from: admin })];
                    case 3:
                        txr = _a.sent();
                        return [4 /*yield*/, expectBalances(txr, Constants_1.INTEGERS.ZERO, amount)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for zero available', function () { return __awaiter(_this, void 0, void 0, function () {
            var excess;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner, account1, market, amount.times(-2)),
                            solo.testing.setAccountBalance(owner, account2, market, amount.times(1)),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.getters.getNumExcessTokens(market)];
                    case 2:
                        excess = _a.sent();
                        expect(excess).toEqual(amount);
                        return [4 /*yield*/, solo.admin.withdrawExcessTokens(market, recipient, { from: admin })];
                    case 3:
                        txr = _a.sent();
                        return [4 /*yield*/, expectBalances(txr, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for zero excess', function () { return __awaiter(_this, void 0, void 0, function () {
            var excess;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner, account1, market, amount.times(-1)),
                            solo.testing.setAccountBalance(owner, account2, market, amount.times(2)),
                            solo.testing.tokenA.issueTo(amount, soloAddress),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.getters.getNumExcessTokens(market)];
                    case 2:
                        excess = _a.sent();
                        expect(excess).toEqual(Constants_1.INTEGERS.ZERO);
                        return [4 /*yield*/, solo.admin.withdrawExcessTokens(market, recipient, { from: admin })];
                    case 3:
                        txr = _a.sent();
                        return [4 /*yield*/, expectBalances(txr, amount, Constants_1.INTEGERS.ZERO)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for negative excess', function () { return __awaiter(_this, void 0, void 0, function () {
            var excess;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner, account1, market, amount.times(-1)),
                            solo.testing.setAccountBalance(owner, account2, market, amount.times(3)),
                            solo.testing.tokenA.issueTo(amount, soloAddress),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.getters.getNumExcessTokens(market)];
                    case 2:
                        excess = _a.sent();
                        expect(excess).toEqual(amount.times(-1));
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.withdrawExcessTokens(market, recipient, { from: admin }), 'AdminImpl: Negative excess')];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-existent market', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.withdrawExcessTokens(invalidMarket, recipient, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.withdrawExcessTokens(market, recipient, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectBalances(txResult, expectedSolo, expectedRecipient) {
            return __awaiter(this, void 0, void 0, function () {
                var token, logs, log, _a, soloBalance, recipientBalance;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0:
                            if (txResult) {
                                token = solo.testing.tokenA.getAddress();
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogWithdrawExcessTokens');
                                expect(log.args.token).toEqual(token);
                                expect(log.args.amount).toEqual(expectedRecipient);
                            }
                            return [4 /*yield*/, Promise.all([
                                    solo.testing.tokenA.getBalance(soloAddress),
                                    solo.testing.tokenA.getBalance(recipient),
                                ])];
                        case 1:
                            _a = _b.sent(), soloBalance = _a[0], recipientBalance = _a[1];
                            expect(soloBalance).toEqual(expectedSolo);
                            expect(recipientBalance).toEqual(expectedRecipient);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    describe('#ownerWithdrawUnsupportedTokens', function () {
        var recipient = Constants_1.ADDRESSES.TEST[1];
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var amount;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        amount = new bignumber_js_1.default(100);
                        return [4 /*yield*/, solo.testing.tokenC.issueTo(amount, soloAddress)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectBalances(null, amount, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.withdrawUnsupportedTokens(solo.testing.tokenC.getAddress(), recipient, { from: admin })];
                    case 3:
                        txr = _a.sent();
                        return [4 /*yield*/, expectBalances(txr, Constants_1.INTEGERS.ZERO, amount)];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for zero tokens', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.admin.withdrawUnsupportedTokens(solo.testing.tokenC.getAddress(), recipient, { from: admin })];
                    case 1:
                        txr = _a.sent();
                        return [4 /*yield*/, expectBalances(txr, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for token with existing market', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.withdrawUnsupportedTokens(solo.testing.tokenA.getAddress(), recipient, { from: admin }), 'AdminImpl: Market exists')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.withdrawUnsupportedTokens(Constants_1.ADDRESSES.TEST[1], recipient, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectBalances(txResult, expectedSolo, expectedRecipient) {
            return __awaiter(this, void 0, void 0, function () {
                var token, logs, log, _a, soloBalance, recipientBalance;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0:
                            if (txResult) {
                                token = solo.testing.tokenC.getAddress();
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogWithdrawExcessTokens');
                                expect(log.args.token).toEqual(token);
                                expect(log.args.amount).toEqual(expectedRecipient);
                            }
                            return [4 /*yield*/, Promise.all([
                                    solo.testing.tokenC.getBalance(soloAddress),
                                    solo.testing.tokenC.getBalance(recipient),
                                ])];
                        case 1:
                            _a = _b.sent(), soloBalance = _a[0], recipientBalance = _a[1];
                            expect(soloBalance).toEqual(expectedSolo);
                            expect(recipientBalance).toEqual(expectedRecipient);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    // ============ Market Functions ============
    describe('#ownerAddMarket', function () {
        var token = Constants_1.ADDRESSES.TEST[2];
        it('Successfully adds a market', function () { return __awaiter(_this, void 0, void 0, function () {
            var marginPremium, spreadPremium, txResult, timestamp, numMarkets, marketId, marketInfo, logs, addLog, oracleLog, setterLog, marginPremiumLog, spreadPremiumLog;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.priceOracle.setPrice(token, defaultPrice)];
                    case 1:
                        _a.sent();
                        marginPremium = new bignumber_js_1.default('0.11');
                        spreadPremium = new bignumber_js_1.default('0.22');
                        return [4 /*yield*/, solo.admin.addMarket(token, oracleAddress, setterAddress, marginPremium, spreadPremium, { from: admin })];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, solo.web3.eth.getBlock(txResult.blockNumber)];
                    case 3:
                        timestamp = (_a.sent()).timestamp;
                        return [4 /*yield*/, solo.getters.getNumMarkets()];
                    case 4:
                        numMarkets = _a.sent();
                        marketId = numMarkets.minus(1);
                        return [4 /*yield*/, solo.getters.getMarketWithInfo(marketId)];
                    case 5:
                        marketInfo = _a.sent();
                        expect(marketInfo.market.token.toLowerCase()).toEqual(token);
                        expect(marketInfo.market.priceOracle).toEqual(oracleAddress);
                        expect(marketInfo.market.interestSetter).toEqual(setterAddress);
                        expect(marketInfo.market.marginPremium).toEqual(marginPremium);
                        expect(marketInfo.market.spreadPremium).toEqual(spreadPremium);
                        expect(marketInfo.market.isClosing).toEqual(false);
                        expect(marketInfo.market.totalPar.borrow).toEqual(Constants_1.INTEGERS.ZERO);
                        expect(marketInfo.market.totalPar.supply).toEqual(Constants_1.INTEGERS.ZERO);
                        expect(marketInfo.market.index.borrow).toEqual(Constants_1.INTEGERS.ONE);
                        expect(marketInfo.market.index.supply).toEqual(Constants_1.INTEGERS.ONE);
                        expect(marketInfo.market.index.lastUpdate).toEqual(new bignumber_js_1.default(timestamp));
                        expect(marketInfo.currentPrice).toEqual(defaultPrice);
                        expect(marketInfo.currentInterestRate).toEqual(Constants_1.INTEGERS.ZERO);
                        expect(marketInfo.currentIndex.borrow).toEqual(Constants_1.INTEGERS.ONE);
                        expect(marketInfo.currentIndex.supply).toEqual(Constants_1.INTEGERS.ONE);
                        expect(marketInfo.market.index.lastUpdate).toEqual(new bignumber_js_1.default(timestamp));
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(5);
                        addLog = logs[0];
                        expect(addLog.name).toEqual('LogAddMarket');
                        expect(addLog.args.marketId).toEqual(marketId);
                        expect(addLog.args.token.toLowerCase()).toEqual(token);
                        oracleLog = logs[1];
                        expect(oracleLog.name).toEqual('LogSetPriceOracle');
                        expect(oracleLog.args.marketId).toEqual(marketId);
                        expect(oracleLog.args.priceOracle).toEqual(oracleAddress);
                        setterLog = logs[2];
                        expect(setterLog.name).toEqual('LogSetInterestSetter');
                        expect(setterLog.args.marketId).toEqual(marketId);
                        expect(setterLog.args.interestSetter).toEqual(setterAddress);
                        marginPremiumLog = logs[3];
                        expect(marginPremiumLog.name).toEqual('LogSetMarginPremium');
                        expect(marginPremiumLog.args.marketId).toEqual(marketId);
                        expect(marginPremiumLog.args.marginPremium).toEqual(marginPremium);
                        spreadPremiumLog = logs[4];
                        expect(spreadPremiumLog.name).toEqual('LogSetSpreadPremium');
                        expect(spreadPremiumLog.args.marketId).toEqual(marketId);
                        expect(spreadPremiumLog.args.spreadPremium).toEqual(spreadPremium);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails to add a market of the same token', function () { return __awaiter(_this, void 0, void 0, function () {
            var token;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        token = solo.testing.tokenA.getAddress();
                        return [4 /*yield*/, solo.testing.priceOracle.setPrice(token, defaultPrice)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.addMarket(token, oracleAddress, setterAddress, defaultPremium, defaultPremium, { from: admin }), 'AdminImpl: Market exists')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for broken price', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.testing.priceOracle.setPrice(token, invalidPrice)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.addMarket(token, oracleAddress, setterAddress, defaultPremium, defaultPremium, { from: admin }), 'AdminImpl: Invalid oracle price')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for broken marginPremium', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.testing.priceOracle.setPrice(token, defaultPrice),
                            solo.testing.interestSetter.setInterestRate(token, defaultRate),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.addMarket(token, oracleAddress, setterAddress, riskLimits.marginPremiumMax.plus(smallestDecimal), defaultPremium, { from: admin }), 'AdminImpl: Margin premium too high')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for broken spreadPremium', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.testing.priceOracle.setPrice(token, defaultPrice),
                            solo.testing.interestSetter.setInterestRate(token, defaultRate),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.addMarket(token, oracleAddress, setterAddress, defaultPremium, riskLimits.spreadPremiumMax.plus(smallestDecimal), { from: admin }), 'AdminImpl: Spread premium too high')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.testing.priceOracle.setPrice(token, defaultPrice),
                            solo.testing.interestSetter.setInterestRate(token, defaultRate),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.addMarket(token, oracleAddress, setterAddress, defaultPremium, defaultPremium, { from: nonAdmin }))];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#ownerSetIsClosing', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectIsClosing(null, false)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setIsClosing(defaultMarket, false, { from: admin })];
                    case 2:
                        // set to false again
                        txr = _a.sent();
                        return [4 /*yield*/, expectIsClosing(txr, false)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setIsClosing(defaultMarket, true, { from: admin })];
                    case 4:
                        // set to true
                        txr = _a.sent();
                        return [4 /*yield*/, expectIsClosing(txr, true)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setIsClosing(defaultMarket, true, { from: admin })];
                    case 6:
                        // set to true again
                        txr = _a.sent();
                        return [4 /*yield*/, expectIsClosing(txr, true)];
                    case 7:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setIsClosing(defaultMarket, false, { from: admin })];
                    case 8:
                        // set to false
                        txr = _a.sent();
                        return [4 /*yield*/, expectIsClosing(txr, false)];
                    case 9:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for index OOB', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setIsClosing(invalidMarket, true, { from: admin }), 'AdminImpl: Market OOB')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setIsClosing(defaultMarket, true, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectIsClosing(txResult, b) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, isClosing;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetIsClosing');
                                expect(log.args.marketId).toEqual(defaultMarket);
                                expect(log.args.isClosing).toEqual(b);
                            }
                            return [4 /*yield*/, solo.getters.getMarketIsClosing(defaultMarket)];
                        case 1:
                            isClosing = _a.sent();
                            expect(isClosing).toEqual(b);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    describe('#ownerSetPriceOracle', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var token, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.getters.getMarketTokenAddress(defaultMarket)];
                    case 1:
                        token = _a.sent();
                        return [4 /*yield*/, solo.testing.priceOracle.setPrice(token, defaultPrice)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setPriceOracle(defaultMarket, oracleAddress, { from: admin })];
                    case 3:
                        txr = _a.sent();
                        logs = solo.logs.parseLogs(txr);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogSetPriceOracle');
                        expect(log.args.marketId).toEqual(defaultMarket);
                        expect(log.args.priceOracle).toEqual(oracleAddress);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for broken price', function () { return __awaiter(_this, void 0, void 0, function () {
            var token;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.getters.getMarketTokenAddress(defaultMarket)];
                    case 1:
                        token = _a.sent();
                        return [4 /*yield*/, solo.testing.priceOracle.setPrice(token, invalidPrice)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setPriceOracle(defaultMarket, oracleAddress, { from: admin }), 'AdminImpl: Invalid oracle price')];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for contract without proper function', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setPriceOracle(defaultMarket, setterAddress, { from: admin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for index OOB', function () { return __awaiter(_this, void 0, void 0, function () {
            var numMarkets;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.getters.getNumMarkets()];
                    case 1:
                        numMarkets = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setPriceOracle(numMarkets, setterAddress, { from: admin }), 'AdminImpl: Market OOB')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setPriceOracle(defaultMarket, oracleAddress, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#ownerSetInterestSetter', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var token, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.getters.getMarketTokenAddress(defaultMarket)];
                    case 1:
                        token = _a.sent();
                        return [4 /*yield*/, solo.testing.interestSetter.setInterestRate(token, defaultRate)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setInterestSetter(defaultMarket, setterAddress, { from: admin })];
                    case 3:
                        txr = _a.sent();
                        logs = solo.logs.parseLogs(txr);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogSetInterestSetter');
                        expect(log.args.marketId).toEqual(defaultMarket);
                        expect(log.args.interestSetter).toEqual(setterAddress);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for contract without proper function', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setInterestSetter(defaultMarket, oracleAddress, { from: admin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for index OOB', function () { return __awaiter(_this, void 0, void 0, function () {
            var numMarkets;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.getters.getNumMarkets()];
                    case 1:
                        numMarkets = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setInterestSetter(numMarkets, setterAddress, { from: admin }), 'AdminImpl: Market OOB')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setInterestSetter(defaultMarket, setterAddress, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#ownerSetMarginPremium', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectMarginPremium(null, defaultPremium)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginPremium(defaultMarket, defaultPremium, { from: admin })];
                    case 2:
                        // set to default
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginPremium(txr, defaultPremium)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginPremium(defaultMarket, highPremium, { from: admin })];
                    case 4:
                        // set risky
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginPremium(txr, highPremium)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginPremium(defaultMarket, highPremium, { from: admin })];
                    case 6:
                        // set to risky again
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginPremium(txr, highPremium)];
                    case 7:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginPremium(defaultMarket, defaultPremium, { from: admin })];
                    case 8:
                        // set back to default
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginPremium(txr, defaultPremium)];
                    case 9:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for index OOB', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMarginPremium(invalidMarket, highPremium, { from: admin }), 'AdminImpl: Market OOB')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMarginPremium(defaultMarket, highPremium, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for too-high value', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMarginPremium(defaultMarket, riskLimits.marginPremiumMax.plus(smallestDecimal), { from: admin }), 'AdminImpl: Margin premium too high')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectMarginPremium(txResult, e) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, premium;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetMarginPremium');
                                expect(log.args.marginPremium).toEqual(e);
                            }
                            return [4 /*yield*/, solo.getters.getMarketMarginPremium(defaultMarket)];
                        case 1:
                            premium = _a.sent();
                            expect(premium).toEqual(e);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    describe('#ownerSetSpreadPremium', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectSpreadPremium(null, defaultPremium)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setSpreadPremium(defaultMarket, defaultPremium, { from: admin })];
                    case 2:
                        // set to default
                        txr = _a.sent();
                        return [4 /*yield*/, expectSpreadPremium(txr, defaultPremium)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setSpreadPremium(defaultMarket, highPremium, { from: admin })];
                    case 4:
                        // set risky
                        txr = _a.sent();
                        return [4 /*yield*/, expectSpreadPremium(txr, highPremium)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setSpreadPremium(defaultMarket, highPremium, { from: admin })];
                    case 6:
                        // set to risky again
                        txr = _a.sent();
                        return [4 /*yield*/, expectSpreadPremium(txr, highPremium)];
                    case 7:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setSpreadPremium(defaultMarket, defaultPremium, { from: admin })];
                    case 8:
                        // set back to default
                        txr = _a.sent();
                        return [4 /*yield*/, expectSpreadPremium(txr, defaultPremium)];
                    case 9:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for two markets', function () { return __awaiter(_this, void 0, void 0, function () {
            var premium1, premium2, result, expected;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        premium1 = new bignumber_js_1.default('0.2');
                        premium2 = new bignumber_js_1.default('0.3');
                        return [4 /*yield*/, Promise.all([
                                solo.admin.setSpreadPremium(defaultMarket, premium1, { from: admin }),
                                solo.admin.setSpreadPremium(secondaryMarket, premium2, { from: admin }),
                            ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.getters.getLiquidationSpreadForPair(defaultMarket, secondaryMarket)];
                    case 2:
                        result = _a.sent();
                        expected = riskParams.liquidationSpread.times(premium1.plus(1)).times(premium2.plus(1));
                        expect(result).toEqual(expected);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for index OOB', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setSpreadPremium(invalidMarket, highPremium, { from: admin }), 'AdminImpl: Market OOB')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setSpreadPremium(defaultMarket, highPremium, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for too-high value', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setSpreadPremium(defaultMarket, riskLimits.spreadPremiumMax.plus(smallestDecimal), { from: admin }), 'AdminImpl: Spread premium too high')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectSpreadPremium(txResult, e) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, premium;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetSpreadPremium');
                                expect(log.args.spreadPremium).toEqual(e);
                            }
                            return [4 /*yield*/, solo.getters.getMarketSpreadPremium(defaultMarket)];
                        case 1:
                            premium = _a.sent();
                            expect(premium).toEqual(e);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    // ============ Risk Functions ============
    describe('#ownerSetMarginRatio', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectMarginRatio(null, riskParams.marginRatio)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginRatio(riskParams.marginRatio, { from: admin })];
                    case 2:
                        // keep same
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginRatio(txr, riskParams.marginRatio)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginRatio(riskLimits.marginRatioMax, { from: admin })];
                    case 4:
                        // set to max
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginRatio(txr, riskLimits.marginRatioMax)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginRatio(riskParams.marginRatio, { from: admin })];
                    case 6:
                        // set back to original
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginRatio(txr, riskParams.marginRatio)];
                    case 7:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for value <= spread', function () { return __awaiter(_this, void 0, void 0, function () {
            var error, liquidationSpread;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        error = 'AdminImpl: Ratio cannot be <= spread';
                        liquidationSpread = smallestDecimal.times(10);
                        return [4 /*yield*/, solo.admin.setLiquidationSpread(liquidationSpread, { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMarginRatio(liquidationSpread.plus(smallestDecimal), { from: admin })];
                    case 2:
                        // passes when above the spread
                        txr = _a.sent();
                        return [4 /*yield*/, expectMarginRatio(txr, liquidationSpread.plus(smallestDecimal))];
                    case 3:
                        _a.sent();
                        // revert when equal to the spread
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMarginRatio(liquidationSpread, { from: admin }), error)];
                    case 4:
                        // revert when equal to the spread
                        _a.sent();
                        // revert when below the spread
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMarginRatio(liquidationSpread.minus(smallestDecimal), { from: admin }), error)];
                    case 5:
                        // revert when below the spread
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for too-high value', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMarginRatio(riskLimits.marginRatioMax.plus(smallestDecimal), { from: admin }), 'AdminImpl: Ratio too high')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMarginRatio(riskParams.marginRatio, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectMarginRatio(txResult, e) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetMarginRatio');
                                expect(log.args.marginRatio).toEqual(e);
                            }
                            return [4 /*yield*/, solo.getters.getMarginRatio()];
                        case 1:
                            result = _a.sent();
                            expect(result).toEqual(e);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    describe('#ownerSetLiquidationSpread', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: 
                    // setup
                    return [4 /*yield*/, solo.admin.setMarginRatio(riskLimits.marginRatioMax, { from: admin })];
                    case 1:
                        // setup
                        _a.sent();
                        return [4 /*yield*/, expectLiquidationSpread(null, riskParams.liquidationSpread)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setLiquidationSpread(riskParams.liquidationSpread, { from: admin })];
                    case 3:
                        // keep same
                        txr = _a.sent();
                        return [4 /*yield*/, expectLiquidationSpread(txr, riskParams.liquidationSpread)];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setLiquidationSpread(riskLimits.liquidationSpreadMax, { from: admin })];
                    case 5:
                        // set to max
                        txr = _a.sent();
                        return [4 /*yield*/, expectLiquidationSpread(txr, riskLimits.liquidationSpreadMax)];
                    case 6:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setLiquidationSpread(riskParams.liquidationSpread, { from: admin })];
                    case 7:
                        // set back to original
                        txr = _a.sent();
                        return [4 /*yield*/, expectLiquidationSpread(txr, riskParams.liquidationSpread)];
                    case 8:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for value >= ratio', function () { return __awaiter(_this, void 0, void 0, function () {
            var error, marginRatio;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        error = 'AdminImpl: Spread cannot be >= ratio';
                        marginRatio = new bignumber_js_1.default('0.1');
                        return [4 /*yield*/, solo.admin.setMarginRatio(marginRatio, { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setLiquidationSpread(marginRatio.minus(smallestDecimal), { from: admin })];
                    case 2:
                        // passes when below the ratio
                        txr = _a.sent();
                        return [4 /*yield*/, expectLiquidationSpread(txr, marginRatio.minus(smallestDecimal))];
                    case 3:
                        _a.sent();
                        // reverts when equal to the ratio
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setLiquidationSpread(marginRatio, { from: admin }), error)];
                    case 4:
                        // reverts when equal to the ratio
                        _a.sent();
                        // reverts when above the ratio
                        return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setLiquidationSpread(marginRatio.plus(smallestDecimal), { from: admin }), error)];
                    case 5:
                        // reverts when above the ratio
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for too-high value', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setLiquidationSpread(riskLimits.liquidationSpreadMax.plus(smallestDecimal), { from: admin }), 'AdminImpl: Spread too high')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setLiquidationSpread(riskParams.liquidationSpread, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectLiquidationSpread(txResult, e) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetLiquidationSpread');
                                expect(log.args.liquidationSpread).toEqual(e);
                            }
                            return [4 /*yield*/, solo.getters.getLiquidationSpread()];
                        case 1:
                            result = _a.sent();
                            expect(result).toEqual(e);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    describe('#ownerSetEarningsRate', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectEarningsRate(null, riskParams.earningsRate)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setEarningsRate(riskParams.earningsRate, { from: admin })];
                    case 2:
                        // keep same
                        txr = _a.sent();
                        return [4 /*yield*/, expectEarningsRate(txr, riskParams.earningsRate)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setEarningsRate(riskLimits.earningsRateMax, { from: admin })];
                    case 4:
                        // set to max
                        txr = _a.sent();
                        return [4 /*yield*/, expectEarningsRate(txr, riskLimits.earningsRateMax)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setEarningsRate(riskParams.earningsRate, { from: admin })];
                    case 6:
                        // set back to original
                        txr = _a.sent();
                        return [4 /*yield*/, expectEarningsRate(txr, riskParams.earningsRate)];
                    case 7:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for too-high value', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setEarningsRate(riskLimits.earningsRateMax.plus(tenToNeg18), { from: admin }), 'AdminImpl: Rate too high')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setEarningsRate(riskParams.earningsRate, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        var tenToNeg18 = '0.000000000000000001';
        function expectEarningsRate(txResult, e) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetEarningsRate');
                                expect(log.args.earningsRate).toEqual(e);
                            }
                            return [4 /*yield*/, solo.getters.getEarningsRate()];
                        case 1:
                            result = _a.sent();
                            expect(result).toEqual(e);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    describe('#ownerSetMinBorrowedValue', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectMinBorrowedValue(null, riskParams.minBorrowedValue)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMinBorrowedValue(riskParams.minBorrowedValue, { from: admin })];
                    case 2:
                        // keep same
                        txr = _a.sent();
                        return [4 /*yield*/, expectMinBorrowedValue(txr, riskParams.minBorrowedValue)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMinBorrowedValue(riskLimits.minBorrowedValueMax, { from: admin })];
                    case 4:
                        // set to max
                        txr = _a.sent();
                        return [4 /*yield*/, expectMinBorrowedValue(txr, riskLimits.minBorrowedValueMax)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setMinBorrowedValue(riskParams.minBorrowedValue, { from: admin })];
                    case 6:
                        // set back to original
                        txr = _a.sent();
                        return [4 /*yield*/, expectMinBorrowedValue(txr, riskParams.minBorrowedValue)];
                    case 7:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for too-high value', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMinBorrowedValue(riskLimits.minBorrowedValueMax.plus(1), { from: admin }), 'AdminImpl: Value too high')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setMinBorrowedValue(riskParams.minBorrowedValue, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectMinBorrowedValue(txResult, e) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetMinBorrowedValue');
                                expect(log.args.minBorrowedValue).toEqual(e);
                            }
                            return [4 /*yield*/, solo.getters.getMinBorrowedValue()];
                        case 1:
                            result = _a.sent();
                            expect(result).toEqual(e);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    // ============ Global Operator Functions ============
    describe('#ownerSetGlobalOperator', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectGlobalOperatorToBe(null, false)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setGlobalOperator(operator, false, { from: admin })];
                    case 2:
                        txr = _a.sent();
                        return [4 /*yield*/, expectGlobalOperatorToBe(txr, false)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setGlobalOperator(operator, true, { from: admin })];
                    case 4:
                        txr = _a.sent();
                        return [4 /*yield*/, expectGlobalOperatorToBe(txr, true)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setGlobalOperator(operator, true, { from: admin })];
                    case 6:
                        txr = _a.sent();
                        return [4 /*yield*/, expectGlobalOperatorToBe(txr, true)];
                    case 7:
                        _a.sent();
                        return [4 /*yield*/, solo.admin.setGlobalOperator(operator, false, { from: admin })];
                    case 8:
                        txr = _a.sent();
                        return [4 /*yield*/, expectGlobalOperatorToBe(txr, false)];
                    case 9:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-admin', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.admin.setGlobalOperator(operator, true, { from: nonAdmin }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        function expectGlobalOperatorToBe(txResult, b) {
            return __awaiter(this, void 0, void 0, function () {
                var logs, log, result;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            if (txResult) {
                                logs = solo.logs.parseLogs(txResult);
                                expect(logs.length).toEqual(1);
                                log = logs[0];
                                expect(log.name).toEqual('LogSetGlobalOperator');
                                expect(log.args.operator).toEqual(operator);
                                expect(log.args.approved).toEqual(b);
                            }
                            return [4 /*yield*/, solo.getters.getIsGlobalOperator(operator)];
                        case 1:
                            result = _a.sent();
                            expect(result).toEqual(b);
                            return [2 /*return*/];
                    }
                });
            });
        }
    });
    // ============ Other ============
    describe('Logs', function () {
        it('Skips logs when necessary', function () { return __awaiter(_this, void 0, void 0, function () {
            var logs;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.admin.setGlobalOperator(operator, false, { from: admin })];
                    case 1:
                        txr = _a.sent();
                        logs = solo.logs.parseLogs(txr, { skipAdminLogs: true });
                        expect(logs.length).toEqual(0);
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
//# sourceMappingURL=Admin.test.js.map