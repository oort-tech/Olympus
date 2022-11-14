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
var exchange_wrappers_1 = require("@dydxprotocol/exchange-wrappers");
var Helpers_1 = require("../src/lib/Helpers");
var types_1 = require("../src/types");
var accountNumber = Constants_1.INTEGERS.ZERO;
var market = Constants_1.INTEGERS.ZERO;
var amount = new bignumber_js_1.default(10000);
var halfAmount = amount.div(2);
var zero = new bignumber_js_1.default(0);
describe('Integration', function () {
    var solo;
    var accounts;
    var snapshotId;
    var who;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    who = solo.getDefaultAccount();
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
    it('No interest increase when index update was this block', function () { return __awaiter(_this, void 0, void 0, function () {
        var _a, _b, blob, actualRate, result1, result2, block1, block2, numTries, tx1, tx2;
        return __generator(this, function (_c) {
            switch (_c.label) {
                case 0:
                    blob = {
                        primaryAccountOwner: who,
                        primaryAccountId: accountNumber,
                        marketId: market,
                        amount: {
                            value: zero,
                            denomination: types_1.AmountDenomination.Actual,
                            reference: types_1.AmountReference.Delta,
                        },
                        from: who,
                    };
                    actualRate = Helpers_1.stringToDecimal('1234');
                    return [4 /*yield*/, solo.testing.interestSetter.setInterestRate(solo.testing.tokenA.getAddress(), actualRate)];
                case 1:
                    _c.sent();
                    numTries = 0;
                    _c.label = 2;
                case 2: return [4 /*yield*/, solo.testing.evm.stopMining()];
                case 3:
                    _c.sent();
                    tx1 = solo.operation.initiate().deposit(blob).commit();
                    tx2 = solo.operation.initiate().deposit(blob).commit();
                    return [4 /*yield*/, solo.testing.evm.startMining()];
                case 4:
                    _c.sent();
                    return [4 /*yield*/, Promise.all([tx1, tx2])];
                case 5:
                    _a = _c.sent(), result1 = _a[0], result2 = _a[1];
                    return [4 /*yield*/, Promise.all([
                            solo.web3.eth.getBlock(result1.blockNumber),
                            solo.web3.eth.getBlock(result2.blockNumber),
                        ])];
                case 6:
                    _b = _c.sent(), block1 = _b[0], block2 = _b[1];
                    numTries += 1;
                    _c.label = 7;
                case 7:
                    if (block1.timestamp !== block2.timestamp || numTries > 10) return [3 /*break*/, 2];
                    _c.label = 8;
                case 8:
                    expect(block1.timestamp).toEqual(block2.timestamp);
                    expect(result1.events.LogIndexUpdate.returnValues).toEqual(result2.events.LogIndexUpdate.returnValues);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Deposit then Withdraw', function () { return __awaiter(_this, void 0, void 0, function () {
        var gasUsed, _a, walletTokenBalance, soloTokenBalance, accountBalances;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.tokenA.issueTo(amount, who),
                        solo.testing.tokenA.setMaximumSoloAllowance(who),
                    ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, solo.operation.initiate()
                            .deposit({
                            primaryAccountOwner: who,
                            primaryAccountId: accountNumber,
                            marketId: market,
                            amount: {
                                value: amount,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                            from: who,
                        })
                            .withdraw({
                            primaryAccountOwner: who,
                            primaryAccountId: accountNumber,
                            marketId: market,
                            amount: {
                                value: halfAmount.times(-1),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                            to: who,
                        })
                            .commit()];
                case 2:
                    gasUsed = (_b.sent()).gasUsed;
                    console.log("\tDeposit then Withdraw gas used: " + gasUsed);
                    return [4 /*yield*/, Promise.all([
                            solo.testing.tokenA.getBalance(who),
                            solo.testing.tokenA.getBalance(solo.contracts.soloMargin.options.address),
                            solo.getters.getAccountBalances(who, accountNumber),
                        ])];
                case 3:
                    _a = _b.sent(), walletTokenBalance = _a[0], soloTokenBalance = _a[1], accountBalances = _a[2];
                    expect(walletTokenBalance).toEqual(halfAmount);
                    expect(soloTokenBalance).toEqual(halfAmount);
                    accountBalances.forEach(function (balance, i) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        if (i === market.toNumber()) {
                            expected = halfAmount;
                        }
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Liquidate multiple times', function () { return __awaiter(_this, void 0, void 0, function () {
        var solidOwner, liquidOwner, solidNumber, liquidNumber, heldMarket1, heldMarket2, owedMarket, premium, _a, solidBalances, liquidBalances;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    solidOwner = solo.getDefaultAccount();
                    liquidOwner = accounts[2];
                    expect(solidOwner).not.toEqual(liquidOwner);
                    solidNumber = Constants_1.INTEGERS.ZERO;
                    liquidNumber = Constants_1.INTEGERS.ONE;
                    heldMarket1 = Constants_1.INTEGERS.ZERO;
                    heldMarket2 = new bignumber_js_1.default(2);
                    owedMarket = Constants_1.INTEGERS.ONE;
                    premium = new bignumber_js_1.default('1.05');
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(solidOwner, solidNumber, owedMarket, amount),
                            solo.testing.setAccountBalance(liquidOwner, liquidNumber, heldMarket1, amount.times(premium).div(2)),
                            solo.testing.setAccountBalance(liquidOwner, liquidNumber, heldMarket2, amount.times(premium).div(2)),
                            solo.testing.setAccountBalance(liquidOwner, liquidNumber, owedMarket, amount.times(-1)),
                        ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, solo.operation.initiate()
                            .liquidate({
                            primaryAccountOwner: solidOwner,
                            primaryAccountId: solidNumber,
                            liquidAccountOwner: liquidOwner,
                            liquidAccountId: liquidNumber,
                            liquidMarketId: owedMarket,
                            payoutMarketId: heldMarket1,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })
                            .liquidate({
                            primaryAccountOwner: solidOwner,
                            primaryAccountId: solidNumber,
                            liquidAccountOwner: liquidOwner,
                            liquidAccountId: liquidNumber,
                            liquidMarketId: owedMarket,
                            payoutMarketId: heldMarket2,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })
                            .commit()];
                case 2:
                    _b.sent();
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getAccountBalances(solidOwner, solidNumber),
                            solo.getters.getAccountBalances(liquidOwner, liquidNumber),
                        ])];
                case 3:
                    _a = _b.sent(), solidBalances = _a[0], liquidBalances = _a[1];
                    solidBalances.forEach(function (balance, i) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        if (i === heldMarket1.toNumber()
                            || i === heldMarket2.toNumber()) {
                            expected = amount.times(premium).div(2);
                        }
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    liquidBalances.forEach(function (balance, _) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Liquidate => Vaporize', function () { return __awaiter(_this, void 0, void 0, function () {
        var solidOwner, liquidOwner, solidNumber, liquidNumber, heldMarket, owedMarket, heldToken, premium, gasUsed, _a, solidBalances, liquidBalances;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    solidOwner = solo.getDefaultAccount();
                    liquidOwner = accounts[2];
                    expect(solidOwner).not.toEqual(liquidOwner);
                    solidNumber = Constants_1.INTEGERS.ZERO;
                    liquidNumber = Constants_1.INTEGERS.ONE;
                    heldMarket = Constants_1.INTEGERS.ZERO;
                    owedMarket = Constants_1.INTEGERS.ONE;
                    heldToken = solo.testing.tokenA;
                    premium = new bignumber_js_1.default('1.05');
                    return [4 /*yield*/, Promise.all([
                            // issue tokens
                            heldToken.issueTo(amount.times(2), solo.contracts.soloMargin.options.address),
                            // set balances
                            solo.testing.setAccountBalance(solidOwner, solidNumber, owedMarket, amount),
                            solo.testing.setAccountBalance(liquidOwner, liquidNumber, heldMarket, amount.times(premium).div(2)),
                            solo.testing.setAccountBalance(liquidOwner, liquidNumber, owedMarket, amount.times(-1)),
                        ])];
                case 1:
                    _b.sent();
                    return [4 /*yield*/, solo.operation.initiate()
                            .liquidate({
                            primaryAccountOwner: solidOwner,
                            primaryAccountId: solidNumber,
                            liquidAccountOwner: liquidOwner,
                            liquidAccountId: liquidNumber,
                            liquidMarketId: owedMarket,
                            payoutMarketId: heldMarket,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })
                            .vaporize({
                            primaryAccountOwner: solidOwner,
                            primaryAccountId: solidNumber,
                            vaporAccountOwner: liquidOwner,
                            vaporAccountId: liquidNumber,
                            vaporMarketId: owedMarket,
                            payoutMarketId: heldMarket,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })
                            .commit()];
                case 2:
                    gasUsed = (_b.sent()).gasUsed;
                    console.log("\tLiquidate => Vaporize gas used: " + gasUsed);
                    return [4 /*yield*/, Promise.all([
                            solo.getters.getAccountBalances(solidOwner, solidNumber),
                            solo.getters.getAccountBalances(liquidOwner, liquidNumber),
                        ])];
                case 3:
                    _a = _b.sent(), solidBalances = _a[0], liquidBalances = _a[1];
                    solidBalances.forEach(function (balance, i) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        if (i === heldMarket.toNumber()) {
                            expected = amount.times(premium);
                        }
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    liquidBalances.forEach(function (balance, _) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Liquidate => Exchange => Withdraw', function () { return __awaiter(_this, void 0, void 0, function () {
        var amount, solidOwner, liquidOwner, solidNumber, liquidNumber, heldMarket, owedMarket, heldToken, owedToken, collateralization, premium, testOrder, gasUsed, _a, ownerHeldTokenBalance, ownerOwedTokenBalance, wrapperHeldTokenBalance, wrapperOwedTokenBalance, soloHeldTokenBalance, soloOwedTokenBalance, solidBalances, liquidBalances;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    amount = new bignumber_js_1.default(100);
                    solidOwner = solo.getDefaultAccount();
                    liquidOwner = accounts[2];
                    expect(solidOwner).not.toEqual(liquidOwner);
                    solidNumber = Constants_1.INTEGERS.ZERO;
                    liquidNumber = Constants_1.INTEGERS.ONE;
                    heldMarket = Constants_1.INTEGERS.ZERO;
                    owedMarket = Constants_1.INTEGERS.ONE;
                    heldToken = solo.testing.tokenA;
                    owedToken = solo.testing.tokenB;
                    collateralization = new bignumber_js_1.default('1.1');
                    premium = new bignumber_js_1.default('1.05');
                    return [4 /*yield*/, Promise.all([
                            // issue tokens
                            heldToken.issueTo(amount.times(collateralization), solo.contracts.soloMargin.options.address),
                            owedToken.issueTo(amount, solo.testing.exchangeWrapper.getAddress()),
                            // set balances
                            solo.testing.setAccountBalance(liquidOwner, liquidNumber, heldMarket, amount.times(collateralization)),
                            solo.testing.setAccountBalance(liquidOwner, liquidNumber, owedMarket, amount.times(-1)),
                        ])];
                case 1:
                    _b.sent();
                    testOrder = {
                        type: exchange_wrappers_1.OrderType.Test,
                        exchangeWrapperAddress: solo.testing.exchangeWrapper.getAddress(),
                        originator: solidOwner,
                        makerToken: owedToken.getAddress(),
                        takerToken: heldToken.getAddress(),
                        makerAmount: amount,
                        takerAmount: amount,
                        allegedTakerAmount: amount,
                        desiredMakerAmount: amount,
                    };
                    return [4 /*yield*/, solo.operation.initiate()
                            .liquidate({
                            primaryAccountOwner: solidOwner,
                            primaryAccountId: solidNumber,
                            liquidAccountOwner: liquidOwner,
                            liquidAccountId: liquidNumber,
                            liquidMarketId: owedMarket,
                            payoutMarketId: heldMarket,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })
                            .buy({
                            primaryAccountOwner: solidOwner,
                            primaryAccountId: solidNumber,
                            takerMarketId: heldMarket,
                            makerMarketId: owedMarket,
                            order: testOrder,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })
                            .withdraw({
                            primaryAccountOwner: solidOwner,
                            primaryAccountId: solidNumber,
                            marketId: heldMarket,
                            amount: {
                                value: Constants_1.INTEGERS.ZERO,
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                            to: solidOwner,
                        })
                            .commit()];
                case 2:
                    gasUsed = (_b.sent()).gasUsed;
                    console.log("\tLiquidate => Exchange => Withdraw gas used: " + gasUsed);
                    return [4 /*yield*/, Promise.all([
                            heldToken.getBalance(solidOwner),
                            owedToken.getBalance(solidOwner),
                            heldToken.getBalance(solo.testing.exchangeWrapper.getAddress()),
                            owedToken.getBalance(solo.testing.exchangeWrapper.getAddress()),
                            heldToken.getBalance(solo.contracts.soloMargin.options.address),
                            owedToken.getBalance(solo.contracts.soloMargin.options.address),
                            solo.getters.getAccountBalances(solidOwner, solidNumber),
                            solo.getters.getAccountBalances(liquidOwner, liquidNumber),
                        ])];
                case 3:
                    _a = _b.sent(), ownerHeldTokenBalance = _a[0], ownerOwedTokenBalance = _a[1], wrapperHeldTokenBalance = _a[2], wrapperOwedTokenBalance = _a[3], soloHeldTokenBalance = _a[4], soloOwedTokenBalance = _a[5], solidBalances = _a[6], liquidBalances = _a[7];
                    expect(ownerHeldTokenBalance).toEqual(amount.times(premium.minus(1)));
                    expect(ownerOwedTokenBalance).toEqual(Constants_1.INTEGERS.ZERO);
                    expect(wrapperHeldTokenBalance).toEqual(Constants_1.INTEGERS.ZERO);
                    expect(wrapperOwedTokenBalance).toEqual(Constants_1.INTEGERS.ZERO);
                    expect(soloHeldTokenBalance).toEqual(amount.times(collateralization.minus(premium)));
                    expect(soloOwedTokenBalance).toEqual(amount);
                    solidBalances.forEach(function (balance, _) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    liquidBalances.forEach(function (balance, i) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        if (i === heldMarket.toNumber()) {
                            expected = amount.times(collateralization.minus(premium));
                        }
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Opening a new short position (deposit in heldToken)', function () { return __awaiter(_this, void 0, void 0, function () {
        var amount, owner, oneNumber, twoNumber, heldMarket, owedMarket, heldToken, owedToken, testOrder, gasUsed, _a, wrapperHeldTokenBalance, wrapperOwedTokenBalance, soloHeldTokenBalance, soloOwedTokenBalance, oneBalances, twoBalances;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    amount = new bignumber_js_1.default(100);
                    owner = solo.getDefaultAccount();
                    oneNumber = Constants_1.INTEGERS.ZERO;
                    twoNumber = Constants_1.INTEGERS.ONE;
                    heldMarket = Constants_1.INTEGERS.ZERO;
                    owedMarket = Constants_1.INTEGERS.ONE;
                    heldToken = solo.testing.tokenA;
                    owedToken = solo.testing.tokenB;
                    return [4 /*yield*/, Promise.all([
                            // issue tokens
                            owedToken.issueTo(amount, solo.contracts.soloMargin.options.address),
                            heldToken.issueTo(amount, solo.testing.exchangeWrapper.getAddress()),
                            // set balances
                            solo.testing.setAccountBalance(owner, oneNumber, heldMarket, amount),
                        ])];
                case 1:
                    _b.sent();
                    testOrder = {
                        type: exchange_wrappers_1.OrderType.Test,
                        exchangeWrapperAddress: solo.testing.exchangeWrapper.getAddress(),
                        originator: owner,
                        makerToken: heldToken.getAddress(),
                        takerToken: owedToken.getAddress(),
                        makerAmount: amount,
                        takerAmount: amount,
                        allegedTakerAmount: amount,
                        desiredMakerAmount: amount,
                    };
                    return [4 /*yield*/, solo.operation.initiate()
                            .transfer({
                            primaryAccountOwner: owner,
                            primaryAccountId: oneNumber,
                            toAccountOwner: owner,
                            toAccountId: twoNumber,
                            marketId: heldMarket,
                            amount: {
                                value: amount.times(-1),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Delta,
                            },
                        })
                            .sell({
                            primaryAccountOwner: owner,
                            primaryAccountId: twoNumber,
                            takerMarketId: owedMarket,
                            makerMarketId: heldMarket,
                            order: testOrder,
                            amount: {
                                value: amount.times(-1),
                                denomination: types_1.AmountDenomination.Actual,
                                reference: types_1.AmountReference.Target,
                            },
                        })
                            .commit()];
                case 2:
                    gasUsed = (_b.sent()).gasUsed;
                    console.log("\tOpen-Short (deposit in heldToken) gas used: " + gasUsed);
                    return [4 /*yield*/, Promise.all([
                            heldToken.getBalance(solo.testing.exchangeWrapper.getAddress()),
                            owedToken.getBalance(solo.testing.exchangeWrapper.getAddress()),
                            heldToken.getBalance(solo.contracts.soloMargin.options.address),
                            owedToken.getBalance(solo.contracts.soloMargin.options.address),
                            solo.getters.getAccountBalances(owner, oneNumber),
                            solo.getters.getAccountBalances(owner, twoNumber),
                        ])];
                case 3:
                    _a = _b.sent(), wrapperHeldTokenBalance = _a[0], wrapperOwedTokenBalance = _a[1], soloHeldTokenBalance = _a[2], soloOwedTokenBalance = _a[3], oneBalances = _a[4], twoBalances = _a[5];
                    expect(wrapperHeldTokenBalance).toEqual(Constants_1.INTEGERS.ZERO);
                    expect(wrapperOwedTokenBalance).toEqual(Constants_1.INTEGERS.ZERO);
                    expect(soloHeldTokenBalance).toEqual(amount);
                    expect(soloOwedTokenBalance).toEqual(Constants_1.INTEGERS.ZERO);
                    oneBalances.forEach(function (balance, _) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    twoBalances.forEach(function (balance, i) {
                        var expected = Constants_1.INTEGERS.ZERO;
                        if (i === heldMarket.toNumber()) {
                            expected = amount.times(2);
                        }
                        if (i === owedMarket.toNumber()) {
                            expected = amount.times(-1);
                        }
                        expect(balance.par).toEqual(expected);
                        expect(balance.wei).toEqual(expected);
                    });
                    return [2 /*return*/];
            }
        });
    }); });
    it('Skips logs when necessary', function () { return __awaiter(_this, void 0, void 0, function () {
        var blob, txResult, noLogs, logs;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    blob = {
                        primaryAccountOwner: who,
                        primaryAccountId: accountNumber,
                        marketId: market,
                        amount: {
                            value: zero,
                            denomination: types_1.AmountDenomination.Actual,
                            reference: types_1.AmountReference.Delta,
                        },
                        from: who,
                    };
                    return [4 /*yield*/, solo.operation.initiate().deposit(blob).commit()];
                case 1:
                    txResult = _a.sent();
                    noLogs = solo.logs.parseLogs(txResult, { skipOperationLogs: true });
                    logs = solo.logs.parseLogs(txResult, { skipOperationLogs: false });
                    expect(noLogs.length).toEqual(0);
                    expect(logs.length).not.toEqual(0);
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
//# sourceMappingURL=Integration.test.js.map