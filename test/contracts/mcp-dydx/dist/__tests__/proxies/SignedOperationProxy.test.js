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
var exchange_wrappers_1 = require("@dydxprotocol/exchange-wrappers");
var Solo_1 = require("../helpers/Solo");
var EVM_1 = require("../helpers/EVM");
var SoloHelpers_1 = require("../helpers/SoloHelpers");
var types_1 = require("../../src/types");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var BytesHelper_1 = require("../../src/lib/BytesHelper");
var solo;
var accounts;
var snapshotId;
var defaultSender;
var defaultSigner;
var rando;
var admin;
var defaultExpiration = new bignumber_js_1.default(0);
var defaultSalt = new bignumber_js_1.default(425);
var defaultSignerNumber = new bignumber_js_1.default(111);
var defaultSenderNumber = new bignumber_js_1.default(222);
var randoNumber = new bignumber_js_1.default(333);
var defaultMarket = new bignumber_js_1.default(1);
var takerMarket = new bignumber_js_1.default(1);
var makerMarket = new bignumber_js_1.default(2);
var par = new bignumber_js_1.default('1e18');
var tradeId = new bignumber_js_1.default(1234);
var defaultAssetAmount = {
    value: Constants_1.INTEGERS.ZERO,
    denomination: types_1.AmountDenomination.Par,
    reference: types_1.AmountReference.Delta,
};
var signedDepositOperation;
var signedWithdrawOperation;
var signedTransferOperation;
var signedBuyOperation;
var signedSellOperation;
var signedTradeOperation;
var signedCallOperation;
var signedLiquidateOperation;
var signedVaporizeOperation;
describe('SignedOperationProxy', function () {
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r, vaporizableAccount, liquidatableAccount, exchangeWrapperAddress, testOrder, _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _b.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    admin = accounts[0];
                    defaultSender = accounts[5];
                    defaultSigner = accounts[6];
                    rando = accounts[7];
                    vaporizableAccount = {
                        owner: rando,
                        number: new bignumber_js_1.default(890),
                    };
                    liquidatableAccount = {
                        owner: rando,
                        number: new bignumber_js_1.default(891),
                    };
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _b.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _b.sent();
                    exchangeWrapperAddress = solo.testing.exchangeWrapper.getAddress();
                    _a = {
                        exchangeWrapperAddress: exchangeWrapperAddress,
                        type: exchange_wrappers_1.OrderType.Test,
                        originator: defaultSigner
                    };
                    return [4 /*yield*/, solo.getters.getMarketTokenAddress(makerMarket)];
                case 4:
                    _a.makerToken = _b.sent();
                    return [4 /*yield*/, solo.getters.getMarketTokenAddress(takerMarket)];
                case 5:
                    testOrder = (_a.takerToken = _b.sent(),
                        _a.makerAmount = Constants_1.INTEGERS.ZERO,
                        _a.takerAmount = Constants_1.INTEGERS.ZERO,
                        _a.desiredMakerAmount = Constants_1.INTEGERS.ZERO,
                        _a.allegedTakerAmount = Constants_1.INTEGERS.ZERO,
                        _a);
                    return [4 /*yield*/, Promise.all([
                            solo.testing.autoTrader.setData(tradeId, defaultAssetAmount),
                            solo.permissions.approveOperator(exchangeWrapperAddress, { from: rando }),
                            solo.permissions.approveOperator(solo.testing.autoTrader.getAddress(), { from: rando }),
                            solo.testing.setAccountBalance(vaporizableAccount.owner, vaporizableAccount.number, takerMarket, par.times(-1)),
                            solo.testing.setAccountBalance(liquidatableAccount.owner, liquidatableAccount.number, takerMarket, par.times(-1)),
                            solo.testing.setAccountBalance(liquidatableAccount.owner, liquidatableAccount.number, makerMarket, par),
                        ])];
                case 6:
                    _b.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 7:
                    snapshotId = _b.sent();
                    return [4 /*yield*/, createSignedOperation('deposit', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            from: defaultSigner,
                            marketId: defaultMarket,
                            amount: defaultAssetAmount,
                        })];
                case 8:
                    signedDepositOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('withdraw', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            to: defaultSigner,
                            marketId: defaultMarket,
                            amount: defaultAssetAmount,
                        })];
                case 9:
                    signedWithdrawOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('transfer', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            toAccountOwner: defaultSender,
                            toAccountId: defaultSenderNumber,
                            marketId: defaultMarket,
                            amount: defaultAssetAmount,
                        })];
                case 10:
                    signedTransferOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('buy', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            takerMarketId: takerMarket,
                            makerMarketId: makerMarket,
                            order: testOrder,
                            amount: defaultAssetAmount,
                        })];
                case 11:
                    signedBuyOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('sell', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            takerMarketId: takerMarket,
                            makerMarketId: makerMarket,
                            order: testOrder,
                            amount: defaultAssetAmount,
                        })];
                case 12:
                    signedSellOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('trade', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            otherAccountOwner: rando,
                            otherAccountId: randoNumber,
                            inputMarketId: takerMarket,
                            outputMarketId: makerMarket,
                            autoTrader: solo.testing.autoTrader.getAddress(),
                            data: BytesHelper_1.toBytes(tradeId),
                            amount: defaultAssetAmount,
                        })];
                case 13:
                    signedTradeOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('call', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            callee: solo.testing.callee.getAddress(),
                            data: BytesHelper_1.toBytes(33, 44),
                        })];
                case 14:
                    signedCallOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('liquidate', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            liquidAccountOwner: liquidatableAccount.owner,
                            liquidAccountId: liquidatableAccount.number,
                            liquidMarketId: takerMarket,
                            payoutMarketId: makerMarket,
                            amount: defaultAssetAmount,
                        })];
                case 15:
                    signedLiquidateOperation = _b.sent();
                    return [4 /*yield*/, createSignedOperation('vaporize', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            vaporAccountOwner: vaporizableAccount.owner,
                            vaporAccountId: vaporizableAccount.number,
                            vaporMarketId: takerMarket,
                            payoutMarketId: makerMarket,
                            amount: defaultAssetAmount,
                        })];
                case 16:
                    signedVaporizeOperation = _b.sent();
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
    describe('Signing Operations', function () {
        it('Succeeds for eth.sign', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        operation = __assign({}, signedTradeOperation);
                        _a = operation;
                        return [4 /*yield*/, solo.signedOperations.signOperation(operation, types_1.SigningMethod.Hash)];
                    case 1:
                        _a.typedSignature =
                            _b.sent();
                        expect(solo.signedOperations.operationHasValidSignature(operation)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for eth_signTypedData', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        operation = __assign({}, signedTradeOperation);
                        _a = operation;
                        return [4 /*yield*/, solo.signedOperations.signOperation(operation, types_1.SigningMethod.TypedData)];
                    case 1:
                        _a.typedSignature =
                            _b.sent();
                        expect(solo.signedOperations.operationHasValidSignature(operation)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Recognizes a bad signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation;
            return __generator(this, function (_a) {
                operation = __assign({}, signedTradeOperation);
                operation.typedSignature = "0x" + '1b'.repeat(65) + "00";
                expect(solo.signedOperations.operationHasValidSignature(operation)).toBe(false);
                return [2 /*return*/];
            });
        }); });
    });
    describe('Signing Cancel Operations', function () {
        it('Succeeds for eth.sign', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation, cancelSig;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        operation = __assign({}, signedTradeOperation);
                        return [4 /*yield*/, solo.signedOperations.signCancelOperation(operation, types_1.SigningMethod.Hash)];
                    case 1:
                        cancelSig = _a.sent();
                        expect(solo.signedOperations.cancelOperationHasValidSignature(operation, cancelSig)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for eth_signTypedData', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation, cancelSig;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        operation = __assign({}, signedTradeOperation);
                        return [4 /*yield*/, solo.signedOperations.signCancelOperation(operation, types_1.SigningMethod.TypedData)];
                    case 1:
                        cancelSig = _a.sent();
                        expect(solo.signedOperations.cancelOperationHasValidSignature(operation, cancelSig)).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Recognizes a bad signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation, cancelSig;
            return __generator(this, function (_a) {
                operation = __assign({}, signedTradeOperation);
                cancelSig = "0x" + '1b'.repeat(65) + "00";
                expect(solo.signedOperations.cancelOperationHasValidSignature(operation, cancelSig)).toBe(false);
                return [2 /*return*/];
            });
        }); });
    });
    describe('shutDown', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        _a = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 1:
                        _a.apply(void 0, [_c.sent()]).toBe(true);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.signedOperationProxy.methods.shutDown(), { from: admin })];
                    case 2:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 3:
                        _b.apply(void 0, [_c.sent()]).toBe(false);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds when it is already shutDown', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.signedOperationProxy.methods.shutDown(), { from: admin })];
                    case 1:
                        _c.sent();
                        _a = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 2:
                        _a.apply(void 0, [_c.sent()]).toBe(false);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.signedOperationProxy.methods.shutDown(), { from: admin })];
                    case 3:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 4:
                        _b.apply(void 0, [_c.sent()]).toBe(false);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.signedOperationProxy.methods.shutDown(), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('startUp', function () {
        it('Succeeds after being shutDown', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.signedOperationProxy.methods.shutDown(), { from: admin })];
                    case 1:
                        _c.sent();
                        _a = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 2:
                        _a.apply(void 0, [_c.sent()]).toBe(false);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.signedOperationProxy.methods.startUp(), { from: admin })];
                    case 3:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 4:
                        _b.apply(void 0, [_c.sent()]).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds when it is already operational', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        _a = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 1:
                        _a.apply(void 0, [_c.sent()]).toBe(true);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.signedOperationProxy.methods.startUp(), { from: admin })];
                    case 2:
                        _c.sent();
                        _b = expect;
                        return [4 /*yield*/, solo.signedOperations.isOperational()];
                    case 3:
                        _b.apply(void 0, [_c.sent()]).toBe(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.signedOperationProxy.methods.startUp(), { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('cancel', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult1, txResult2, logs1, logs2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectValid([signedWithdrawOperation])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.signedOperations.cancelOperation(signedWithdrawOperation)];
                    case 2:
                        txResult1 = _a.sent();
                        return [4 /*yield*/, expectInvalid([signedWithdrawOperation])];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.signedOperations.cancelOperation(signedWithdrawOperation)];
                    case 4:
                        txResult2 = _a.sent();
                        return [4 /*yield*/, expectInvalid([signedWithdrawOperation])];
                    case 5:
                        _a.sent();
                        logs1 = solo.logs.parseLogs(txResult1);
                        expect(logs1[0].name).toEqual('LogOperationCanceled');
                        expect(logs1[0].args).toEqual({
                            canceler: signedWithdrawOperation.signer,
                            operationHash: solo.signedOperations.getOperationHash(signedWithdrawOperation),
                        });
                        logs2 = solo.logs.parseLogs(txResult2);
                        expect(logs2[0].name).toEqual(logs1[0].name);
                        expect(logs2[0].args).toEqual(logs1[0].args);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for two-account operations', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult1, txResult2, logs1, logs2;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectValid([signedTransferOperation])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.signedOperations.cancelOperation(signedTransferOperation)];
                    case 2:
                        txResult1 = _a.sent();
                        return [4 /*yield*/, expectInvalid([signedTransferOperation])];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.signedOperations.cancelOperation(signedTransferOperation)];
                    case 4:
                        txResult2 = _a.sent();
                        return [4 /*yield*/, expectInvalid([signedTransferOperation])];
                    case 5:
                        _a.sent();
                        logs1 = solo.logs.parseLogs(txResult1);
                        expect(logs1[0].name).toEqual('LogOperationCanceled');
                        expect(logs1[0].args).toEqual({
                            canceler: signedTransferOperation.signer,
                            operationHash: solo.signedOperations.getOperationHash(signedTransferOperation),
                        });
                        logs2 = solo.logs.parseLogs(txResult2);
                        expect(logs2[0].name).toEqual(logs1[0].name);
                        expect(logs2[0].args).toEqual(logs1[0].args);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for non-signer', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.signedOperations.cancelOperation(signedTransferOperation, { from: rando }), 'SignedOperationProxy: Canceler must be signer')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Basic', function () {
        it('Succeeds for basic test', function () { return __awaiter(_this, void 0, void 0, function () {
            var actions, operation, typedSignature, signedOperation, txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        actions = [{
                                actionType: types_1.ActionType.Deposit,
                                primaryAccountOwner: defaultSigner,
                                primaryAccountNumber: defaultSignerNumber,
                                secondaryAccountOwner: Constants_1.ADDRESSES.ZERO,
                                secondaryAccountNumber: Constants_1.INTEGERS.ZERO,
                                primaryMarketId: defaultMarket,
                                secondaryMarketId: Constants_1.INTEGERS.ZERO,
                                otherAddress: defaultSigner,
                                data: '0x',
                                amount: {
                                    sign: false,
                                    ref: types_1.AmountReference.Delta,
                                    denomination: types_1.AmountDenomination.Actual,
                                    value: Constants_1.INTEGERS.ZERO,
                                },
                            }];
                        operation = {
                            actions: actions,
                            expiration: defaultExpiration,
                            salt: defaultSalt,
                            sender: defaultSender,
                            signer: defaultSigner,
                        };
                        return [4 /*yield*/, solo.signedOperations.signOperation(operation, types_1.SigningMethod.Hash)];
                    case 1:
                        typedSignature = _a.sent();
                        signedOperation = __assign({}, operation, { typedSignature: typedSignature });
                        return [4 /*yield*/, expectValid([signedOperation])];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(signedOperation)
                                .commit({ from: defaultSender })];
                    case 3:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogDeposit']);
                        return [4 /*yield*/, expectInvalid([signedOperation])];
                    case 4:
                        _a.sent();
                        console.log("\tSignedOperationProxy gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for deposit', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedDepositOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogDeposit']);
                        return [4 /*yield*/, expectInvalid([signedDepositOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for withdraw', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedWithdrawOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogWithdraw']);
                        return [4 /*yield*/, expectInvalid([signedWithdrawOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for transfer', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedTransferOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogTransfer']);
                        return [4 /*yield*/, expectInvalid([signedTransferOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for buy', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedBuyOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogBuy']);
                        return [4 /*yield*/, expectInvalid([signedBuyOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for sell', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedSellOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogSell']);
                        return [4 /*yield*/, expectInvalid([signedSellOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for trade', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedTradeOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogTrade']);
                        return [4 /*yield*/, expectInvalid([signedTradeOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for call (0 bytes)', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedCallOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogCall']);
                        return [4 /*yield*/, expectInvalid([signedCallOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for call (<32 bytes)', function () { return __awaiter(_this, void 0, void 0, function () {
            var signedCallShortOperation, txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, createSignedOperation('call', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            callee: solo.testing.simpleCallee.getAddress(),
                            data: [[1], [2], [3]],
                        })];
                    case 1:
                        signedCallShortOperation = _a.sent();
                        return [4 /*yield*/, solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(signedCallShortOperation)
                                .commit({ from: defaultSender })];
                    case 2:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogCall']);
                        return [4 /*yield*/, expectInvalid([signedCallShortOperation])];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for call (>32, <64 bytes)', function () { return __awaiter(_this, void 0, void 0, function () {
            var signedCallOddOperation, txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, createSignedOperation('call', {
                            primaryAccountId: defaultSignerNumber,
                            primaryAccountOwner: defaultSigner,
                            callee: solo.testing.simpleCallee.getAddress(),
                            data: BytesHelper_1.toBytes(1234).concat([[1], [2], [3]]),
                        })];
                    case 1:
                        signedCallOddOperation = _a.sent();
                        return [4 /*yield*/, solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(signedCallOddOperation)
                                .commit({ from: defaultSender })];
                    case 2:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogCall']);
                        return [4 /*yield*/, expectInvalid([signedCallOddOperation])];
                    case 3:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for liquidate', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedLiquidateOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogLiquidate']);
                        return [4 /*yield*/, expectInvalid([signedLiquidateOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for vaporize', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedVaporizeOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogVaporize']);
                        return [4 /*yield*/, expectInvalid([signedVaporizeOperation])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Failures for each actionType', function () {
        function randoifySignedOperation(signedOperation) {
            return __awaiter(this, void 0, void 0, function () {
                var randoifiedOperation, _a;
                return __generator(this, function (_b) {
                    switch (_b.label) {
                        case 0:
                            randoifiedOperation = __assign({}, signedOperation);
                            randoifiedOperation.actions = [{}];
                            randoifiedOperation.actions[0] = __assign({}, signedOperation.actions[0], { primaryAccountOwner: rando });
                            _a = randoifiedOperation;
                            return [4 /*yield*/, solo.signedOperations.signOperation(randoifiedOperation, types_1.SigningMethod.Hash)];
                        case 1:
                            _a.typedSignature =
                                _b.sent();
                            return [2 /*return*/, randoifiedOperation];
                    }
                });
            });
        }
        it('Fails for deposit', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedDepositOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for withdraw', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedWithdrawOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for transfer', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedTransferOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for buy', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedBuyOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for sell', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedSellOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for trade', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedTradeOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for call (0 bytes)', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedCallOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for liquidate', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedLiquidateOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for vaporize', function () { return __awaiter(_this, void 0, void 0, function () {
            var badOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, randoifySignedOperation(signedVaporizeOperation)];
                    case 1:
                        badOperation = _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(badOperation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Other failures', function () {
        it('Fails for expired operation', function () { return __awaiter(_this, void 0, void 0, function () {
            var expiredOperation, _a;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        expiredOperation = __assign({}, signedDepositOperation, { expiration: Constants_1.INTEGERS.ONE });
                        _a = expiredOperation;
                        return [4 /*yield*/, solo.signedOperations.signOperation(expiredOperation, types_1.SigningMethod.Hash)];
                    case 1:
                        _a.typedSignature =
                            _b.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(expiredOperation)
                                .commit({ from: defaultSender }), 'SignedOperationProxy: Signed operation is expired')];
                    case 2:
                        _b.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for msg.sender mismatch', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedDepositOperation)
                            .commit({ from: rando }), 'SignedOperationProxy: Operation sender mismatch')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for hash already used', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedWithdrawOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(signedWithdrawOperation)
                                .commit({ from: defaultSender }), 'SignedOperationProxy: Hash already used or canceled')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for hash canceled', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.signedOperations.cancelOperation(signedWithdrawOperation)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(signedWithdrawOperation)
                                .commit({ from: defaultSender }), 'SignedOperationProxy: Hash already used or canceled')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for authorization that overflows', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectAssertFailure(solo.contracts.send(solo.contracts.signedOperationProxy.methods.operate([{
                                owner: defaultSigner,
                                number: defaultSignerNumber.toFixed(0),
                            }], [{
                                actionType: types_1.ActionType.Deposit,
                                accountId: '0',
                                primaryMarketId: '0',
                                secondaryMarketId: '0',
                                otherAddress: Constants_1.ADDRESSES.ZERO,
                                otherAccountId: '0',
                                data: [],
                                amount: {
                                    sign: false,
                                    ref: types_1.AmountReference.Delta,
                                    denomination: types_1.AmountDenomination.Par,
                                    value: '0',
                                },
                            }], [{
                                numActions: '2',
                                header: {
                                    expiration: '0',
                                    salt: '0',
                                    sender: defaultSender,
                                    signer: defaultSigner,
                                },
                                signature: BytesHelper_1.toBytes(signedDepositOperation.typedSignature),
                            }]), { from: defaultSender }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for authorization past end-of-actions', function () { return __awaiter(_this, void 0, void 0, function () {
            var depositAction;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        depositAction = signedDepositOperation.actions[0];
                        return [4 /*yield*/, Expect_1.expectAssertFailure(solo.contracts.send(solo.contracts.signedOperationProxy.methods.operate([{
                                    owner: defaultSigner,
                                    number: defaultSignerNumber.toFixed(0),
                                }], [__assign({}, depositAction, { accountId: '0', otherAccountId: '0', primaryMarketId: depositAction.primaryMarketId.toFixed(0), secondaryMarketId: depositAction.secondaryMarketId.toFixed(0), data: [], amount: __assign({}, depositAction.amount, { value: depositAction.amount.value.toFixed(0) }) })], [
                                {
                                    numActions: '1',
                                    header: {
                                        expiration: '0',
                                        salt: '0',
                                        sender: defaultSender,
                                        signer: defaultSigner,
                                    },
                                    signature: BytesHelper_1.toBytes(signedDepositOperation.typedSignature),
                                },
                                {
                                    numActions: '1',
                                    header: {
                                        expiration: '0',
                                        salt: '0',
                                        sender: defaultSender,
                                        signer: defaultSigner,
                                    },
                                    signature: BytesHelper_1.toBytes(signedWithdrawOperation.typedSignature),
                                },
                            ]), { from: defaultSender }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if not all actions are signed', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.signedOperationProxy.methods.operate([{
                                owner: defaultSigner,
                                number: defaultSignerNumber.toFixed(0),
                            }], [{
                                actionType: types_1.ActionType.Deposit,
                                accountId: '0',
                                primaryMarketId: '0',
                                secondaryMarketId: '0',
                                otherAddress: Constants_1.ADDRESSES.ZERO,
                                otherAccountId: '0',
                                data: [],
                                amount: {
                                    sign: false,
                                    ref: types_1.AmountReference.Delta,
                                    denomination: types_1.AmountDenomination.Par,
                                    value: '0',
                                },
                            }], []), { from: defaultSender }), 'SignedOperationProxy: Not all actions are signed')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for incorrect signature', function () { return __awaiter(_this, void 0, void 0, function () {
            var invalidSigOperation;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        invalidSigOperation = __assign({}, signedDepositOperation, { salt: new bignumber_js_1.default(999) });
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(invalidSigOperation)
                                .commit({ from: defaultSender }), 'SignedOperationProxy: Invalid signature')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails if non-operational', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.signedOperationProxy.methods.shutDown(), { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(signedDepositOperation)
                                .commit({ from: defaultSender }), 'SignedOperationProxy: Contract is not operational')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Advanced', function () {
        it('Succeeds only for transfers involving sender or signer', function () { return __awaiter(_this, void 0, void 0, function () {
            var defaultBlob, goodTransfer1, goodTransfer2, goodTransfer3, goodTransfer4, allOperations, _a, _b, _i, o, operation, txResult;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        defaultBlob = {
                            primaryAccountId: defaultSignerNumber,
                            toAccountId: defaultSenderNumber,
                            marketId: defaultMarket,
                            amount: defaultAssetAmount,
                        };
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: defaultSigner, toAccountOwner: defaultSender }))];
                    case 1:
                        goodTransfer1 = _c.sent();
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: defaultSender, toAccountOwner: defaultSigner }))];
                    case 2:
                        goodTransfer2 = _c.sent();
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: defaultSigner, toAccountOwner: defaultSigner }))];
                    case 3:
                        goodTransfer3 = _c.sent();
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: defaultSender, toAccountOwner: defaultSender }))];
                    case 4:
                        goodTransfer4 = _c.sent();
                        allOperations = [goodTransfer1, goodTransfer2, goodTransfer3, goodTransfer4];
                        _a = [];
                        for (_b in allOperations)
                            _a.push(_b);
                        _i = 0;
                        _c.label = 5;
                    case 5:
                        if (!(_i < _a.length)) return [3 /*break*/, 8];
                        o = _a[_i];
                        operation = allOperations[o];
                        return [4 /*yield*/, solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(operation)
                                .commit({ from: defaultSender })];
                    case 6:
                        txResult = _c.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogTransfer']);
                        _c.label = 7;
                    case 7:
                        _i++;
                        return [3 /*break*/, 5];
                    case 8: return [2 /*return*/];
                }
            });
        }); });
        it('Fails for transfers involving non-sender or non-signer accounts', function () { return __awaiter(_this, void 0, void 0, function () {
            var defaultBlob, badTransfer1, badTransfer2, badTransfer3, badTransfer4, badTransfer5, allOperations, _a, _b, _i, o, operation;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        defaultBlob = {
                            primaryAccountId: defaultSignerNumber,
                            toAccountId: defaultSenderNumber,
                            marketId: defaultMarket,
                            amount: defaultAssetAmount,
                        };
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: rando, toAccountOwner: defaultSender }))];
                    case 1:
                        badTransfer1 = _c.sent();
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: defaultSender, toAccountOwner: rando }))];
                    case 2:
                        badTransfer2 = _c.sent();
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: defaultSigner, toAccountOwner: rando }))];
                    case 3:
                        badTransfer3 = _c.sent();
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: rando, toAccountOwner: defaultSigner }))];
                    case 4:
                        badTransfer4 = _c.sent();
                        return [4 /*yield*/, createSignedOperation('transfer', __assign({}, defaultBlob, { primaryAccountOwner: rando, toAccountOwner: rando }))];
                    case 5:
                        badTransfer5 = _c.sent();
                        allOperations = [badTransfer1, badTransfer2, badTransfer3, badTransfer4, badTransfer5];
                        _a = [];
                        for (_b in allOperations)
                            _a.push(_b);
                        _i = 0;
                        _c.label = 6;
                    case 6:
                        if (!(_i < _a.length)) return [3 /*break*/, 9];
                        o = _a[_i];
                        operation = allOperations[o];
                        return [4 /*yield*/, Expect_1.expectThrow(solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(operation)
                                .commit({ from: defaultSender }), "SignedOperationProxy: Signer not authorized <" + defaultSigner.toLowerCase() + ">")];
                    case 7:
                        _c.sent();
                        _c.label = 8;
                    case 8:
                        _i++;
                        return [3 /*break*/, 6];
                    case 9: return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for data with less than 32 bytes', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.operation
                            .initiate({ proxy: types_1.ProxyType.Signed })
                            .addSignedOperation(signedCallOperation)
                            .commit({ from: defaultSender })];
                    case 1:
                        txResult = _a.sent();
                        expectLogs(txResult, ['LogOperationExecuted', 'LogOperation', 'LogCall']);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for multiple actions in a single operation', function () { return __awaiter(_this, void 0, void 0, function () {
            var multiActionOperation, _a, txResult;
            return __generator(this, function (_b) {
                switch (_b.label) {
                    case 0:
                        multiActionOperation = __assign({}, signedDepositOperation);
                        multiActionOperation.actions =
                            multiActionOperation.actions.concat(signedWithdrawOperation.actions);
                        multiActionOperation.actions =
                            multiActionOperation.actions.concat(signedTransferOperation.actions);
                        multiActionOperation.actions =
                            multiActionOperation.actions.concat(signedCallOperation.actions);
                        _a = multiActionOperation;
                        return [4 /*yield*/, solo.signedOperations.signOperation(multiActionOperation, types_1.SigningMethod.Hash)];
                    case 1:
                        _a.typedSignature =
                            _b.sent();
                        return [4 /*yield*/, solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(multiActionOperation)
                                .commit({ from: defaultSender })];
                    case 2:
                        txResult = _b.sent();
                        expectLogs(txResult, [
                            'LogOperationExecuted',
                            'LogOperation',
                            'LogDeposit',
                            'LogWithdraw',
                            'LogTransfer',
                            'LogCall',
                        ]);
                        return [4 /*yield*/, expectInvalid([multiActionOperation])];
                    case 3:
                        _b.sent();
                        console.log("\tSignedOperationProxy (multiAction) gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for multiple signed operations from different signers', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation2, signedOperation2, _a, _b, txResult;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        operation2 = solo.operation.initiate().deposit({
                            primaryAccountOwner: rando,
                            primaryAccountId: randoNumber,
                            marketId: defaultMarket,
                            from: rando,
                            amount: defaultAssetAmount,
                        }).createSignableOperation({
                            sender: defaultSender,
                            signer: rando,
                        });
                        _a = [{}, operation2];
                        _b = {};
                        return [4 /*yield*/, solo.signedOperations.signOperation(operation2, types_1.SigningMethod.Hash)];
                    case 1:
                        signedOperation2 = __assign.apply(void 0, _a.concat([(_b.typedSignature = _c.sent(), _b)]));
                        return [4 /*yield*/, solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .addSignedOperation(signedWithdrawOperation)
                                .addSignedOperation(signedOperation2)
                                .commit({ from: defaultSender })];
                    case 2:
                        txResult = _c.sent();
                        expectLogs(txResult, [
                            'LogOperationExecuted',
                            'LogOperationExecuted',
                            'LogOperation',
                            'LogWithdraw',
                            'LogDeposit',
                        ]);
                        return [4 /*yield*/, expectInvalid([signedWithdrawOperation, signedOperation2])];
                    case 3:
                        _c.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for zero-length proofs', function () { return __awaiter(_this, void 0, void 0, function () {
            var emptyOperation, _a, _b, _c, _d, _e, _f, _g;
            return __generator(this, function (_h) {
                switch (_h.label) {
                    case 0:
                        emptyOperation = {
                            actions: [],
                            expiration: Constants_1.INTEGERS.ZERO,
                            salt: Constants_1.INTEGERS.ZERO,
                            sender: Constants_1.ADDRESSES.ZERO,
                            signer: defaultSender,
                        };
                        _b = (_a = solo.contracts).send;
                        _d = (_c = solo.contracts.signedOperationProxy.methods).operate;
                        _e = [[{
                                    owner: defaultSender,
                                    number: defaultSenderNumber.toFixed(0),
                                }],
                            [{
                                    actionType: types_1.ActionType.Deposit,
                                    accountId: '0',
                                    primaryMarketId: '0',
                                    secondaryMarketId: '0',
                                    otherAddress: defaultSender,
                                    otherAccountId: '0',
                                    data: [],
                                    amount: {
                                        sign: false,
                                        ref: types_1.AmountReference.Delta,
                                        denomination: types_1.AmountDenomination.Par,
                                        value: '0',
                                    },
                                }]];
                        _f = {
                            numActions: '0',
                            header: {
                                expiration: '0',
                                salt: '0',
                                sender: Constants_1.ADDRESSES.ZERO,
                                signer: defaultSender,
                            }
                        };
                        _g = BytesHelper_1.toBytes;
                        return [4 /*yield*/, solo.signedOperations.signOperation(emptyOperation, types_1.SigningMethod.Hash)];
                    case 1: return [4 /*yield*/, _b.apply(_a, [_d.apply(_c, _e.concat([[
                                    (_f.signature = _g.apply(void 0, [_h.sent()]),
                                        _f),
                                    {
                                        numActions: '1',
                                        header: {
                                            expiration: '0',
                                            salt: '0',
                                            sender: defaultSender,
                                            signer: defaultSender,
                                        },
                                        signature: [],
                                    }
                                ]])),
                            { from: defaultSender }])];
                    case 2:
                        _h.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for multiple signed operations from different signers interleaved', function () { return __awaiter(_this, void 0, void 0, function () {
            var operation2, signedOperation2, _a, _b, callData, txResult;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0:
                        operation2 = solo.operation.initiate().deposit({
                            primaryAccountOwner: rando,
                            primaryAccountId: randoNumber,
                            marketId: defaultMarket,
                            from: rando,
                            amount: defaultAssetAmount,
                        }).createSignableOperation({
                            sender: defaultSender,
                            signer: rando,
                        });
                        _a = [{}, operation2];
                        _b = {};
                        return [4 /*yield*/, solo.signedOperations.signOperation(operation2, types_1.SigningMethod.Hash)];
                    case 1:
                        signedOperation2 = __assign.apply(void 0, _a.concat([(_b.typedSignature = _c.sent(), _b)]));
                        callData = {
                            primaryAccountOwner: defaultSender,
                            primaryAccountId: defaultSenderNumber,
                            callee: solo.testing.simpleCallee.getAddress(),
                            data: [[1], [255]],
                        };
                        return [4 /*yield*/, solo.operation
                                .initiate({ proxy: types_1.ProxyType.Signed })
                                .call(callData)
                                .addSignedOperation(signedWithdrawOperation)
                                .call(callData)
                                .addSignedOperation(signedOperation2)
                                .call(callData)
                                .commit({ from: defaultSender })];
                    case 2:
                        txResult = _c.sent();
                        expectLogs(txResult, [
                            'LogOperationExecuted',
                            'LogOperationExecuted',
                            'LogOperation',
                            'LogCall',
                            'LogWithdraw',
                            'LogCall',
                            'LogDeposit',
                            'LogCall',
                        ]);
                        return [4 /*yield*/, expectInvalid([signedWithdrawOperation, signedOperation2])];
                    case 3:
                        _c.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
// ============ Helper Functions ============
function createSignedOperation(actionType, action) {
    return __awaiter(this, void 0, void 0, function () {
        var operation, _a, _b;
        return __generator(this, function (_c) {
            switch (_c.label) {
                case 0:
                    operation = solo.operation.initiate()[actionType](action).createSignableOperation({
                        sender: defaultSender,
                        signer: defaultSigner,
                    });
                    _a = [{}, operation];
                    _b = {};
                    return [4 /*yield*/, solo.signedOperations.signOperation(operation, types_1.SigningMethod.Hash)];
                case 1: return [2 /*return*/, __assign.apply(void 0, _a.concat([(_b.typedSignature = _c.sent(), _b)]))];
            }
        });
    });
}
function expectLogs(txResult, logTitles) {
    var logs = solo.logs.parseLogs(txResult);
    var actualTitles = logs.map(function (x) { return x.name; }).filter(function (x) { return x !== 'LogIndexUpdate'; });
    expect(actualTitles).toEqual(logTitles);
}
function expectInvalid(operations) {
    return __awaiter(this, void 0, void 0, function () {
        var _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    _a = expect;
                    return [4 /*yield*/, solo.signedOperations.getOperationsAreInvalid(operations)];
                case 1:
                    _a.apply(void 0, [_b.sent()]).toEqual(Array(operations.length).fill(true));
                    return [2 /*return*/];
            }
        });
    });
}
function expectValid(operations) {
    return __awaiter(this, void 0, void 0, function () {
        var _a;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    _a = expect;
                    return [4 /*yield*/, solo.signedOperations.getOperationsAreInvalid(operations)];
                case 1:
                    _a.apply(void 0, [_b.sent()]).toEqual(Array(operations.length).fill(false));
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=SignedOperationProxy.test.js.map