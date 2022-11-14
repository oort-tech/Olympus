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
var Deploy_1 = require("../helpers/Deploy");
var EVM_1 = require("../helpers/EVM");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var PartiallyDelayedMultiSig_json_1 = __importDefault(require("../../build/contracts/PartiallyDelayedMultiSig.json"));
var TestCounter_json_1 = __importDefault(require("../../build/contracts/TestCounter.json"));
var multiSig;
var testCounterA;
var testCounterB;
var solo;
var accounts;
var owner1;
var owner2;
var owner3;
var rando;
var counterAAddress;
var counterBAddress;
var functionOneSelector = '0x181b3bb3';
var functionTwoSelector = '0x935272a2';
var functionThreeSelector = '0x8e0137b9';
var fallbackSelector = '0x00000000';
describe('PartiallyDelayedMultiSig', function () {
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
                    owner1 = accounts[1];
                    owner2 = accounts[2];
                    owner3 = accounts[3];
                    rando = accounts[4];
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _b.sent();
                    return [4 /*yield*/, Promise.all([
                            Deploy_1.deployContract(solo, TestCounter_json_1.default),
                            Deploy_1.deployContract(solo, TestCounter_json_1.default),
                        ])];
                case 3:
                    // deploy contracts
                    _a = _b.sent(), testCounterA = _a[0], testCounterB = _a[1];
                    counterAAddress = testCounterA.options.address;
                    counterBAddress = testCounterB.options.address;
                    return [4 /*yield*/, Deploy_1.deployContract(solo, PartiallyDelayedMultiSig_json_1.default, [
                            [owner1, owner2, owner3],
                            '2',
                            '120',
                            [counterAAddress, Constants_1.ADDRESSES.ZERO, counterBAddress],
                            [functionOneSelector, functionTwoSelector, fallbackSelector],
                        ])];
                case 4:
                    multiSig = _b.sent();
                    // synchronously submit all transaction from owner1
                    return [4 /*yield*/, submitTransaction(counterAAddress, functionOneData())];
                case 5:
                    // synchronously submit all transaction from owner1
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterBAddress, functionOneData())];
                case 6:
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterAAddress, numberToFunctionTwoData(1))];
                case 7:
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterBAddress, numberToFunctionTwoData(1))];
                case 8:
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterAAddress, numbersToFunctionThreeData(2, 3))];
                case 9:
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterBAddress, numbersToFunctionThreeData(2, 3))];
                case 10:
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterAAddress, [])];
                case 11:
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterBAddress, [])];
                case 12:
                    _b.sent();
                    return [4 /*yield*/, submitTransaction(counterAAddress, [[0]])];
                case 13:
                    _b.sent();
                    // approve all transactions from owner2
                    return [4 /*yield*/, Promise.all([
                            confirmTransaction(0),
                            confirmTransaction(1),
                            confirmTransaction(2),
                            confirmTransaction(3),
                            confirmTransaction(4),
                            confirmTransaction(5),
                            confirmTransaction(6),
                            confirmTransaction(7),
                            confirmTransaction(8),
                        ])];
                case 14:
                    // approve all transactions from owner2
                    _b.sent();
                    return [4 /*yield*/, EVM_1.snapshot()];
                case 15:
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
    describe('#constructor', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var owners, required, secondsTimeLocked;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.call(multiSig.methods.getOwners())];
                    case 1:
                        owners = _a.sent();
                        expect(owners).toEqual([owner1, owner2, owner3]);
                        return [4 /*yield*/, solo.contracts.call(multiSig.methods.required())];
                    case 2:
                        required = _a.sent();
                        expect(required).toEqual('2');
                        return [4 /*yield*/, solo.contracts.call(multiSig.methods.secondsTimeLocked())];
                    case 3:
                        secondsTimeLocked = _a.sent();
                        expect(secondsTimeLocked).toEqual('120');
                        return [4 /*yield*/, Promise.all([
                                expectInstantData(counterAAddress, functionOneSelector, true),
                                expectInstantData(counterBAddress, functionOneSelector, false),
                                expectInstantData(Constants_1.ADDRESSES.ZERO, functionOneSelector, false),
                                expectInstantData(counterAAddress, functionTwoSelector, false),
                                expectInstantData(counterBAddress, functionTwoSelector, false),
                                expectInstantData(Constants_1.ADDRESSES.ZERO, functionTwoSelector, true),
                                expectInstantData(counterAAddress, functionThreeSelector, false),
                                expectInstantData(counterBAddress, functionThreeSelector, false),
                                expectInstantData(Constants_1.ADDRESSES.ZERO, functionThreeSelector, false),
                                expectInstantData(counterAAddress, fallbackSelector, false),
                                expectInstantData(counterBAddress, fallbackSelector, true),
                                expectInstantData(Constants_1.ADDRESSES.ZERO, fallbackSelector, false),
                            ])];
                    case 4:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for array mismatch', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(Deploy_1.deployContract(solo, PartiallyDelayedMultiSig_json_1.default, [
                            [owner1, owner2, owner3],
                            '2',
                            '120',
                            [counterAAddress, Constants_1.ADDRESSES.ZERO, counterBAddress],
                            [functionOneSelector, functionTwoSelector],
                        ]), 'ADDRESS_AND_SELECTOR_MISMATCH')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#setSelector', function () {
        it('Succeeds for false', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectInstantData(counterAAddress, functionOneSelector, true)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, submitTransaction(multiSig.options.address, setSelectorData(counterAAddress, functionOneSelector, false))];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, confirmTransaction(9)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.fastForward(120)];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, executeTransaction(9)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, expectInstantData(counterAAddress, functionOneSelector, false)];
                    case 6:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for true', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectInstantData(counterBAddress, functionThreeSelector, false)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, submitTransaction(multiSig.options.address, setSelectorData(counterBAddress, functionThreeSelector, true))];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, confirmTransaction(9)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.fastForward(120)];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, executeTransaction(9)];
                    case 5:
                        _a.sent();
                        return [4 /*yield*/, expectInstantData(counterBAddress, functionThreeSelector, true)];
                    case 6:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for external sender', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(multiSig.methods.setSelector(Constants_1.ADDRESSES.ZERO, '0x00000000', true)))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#executeTransaction (slow)', function () {
        it('Fails for before timelock', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(executeTransaction(5), 'TIME_LOCK_INCOMPLETE')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for past timelock', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, EVM_1.fastForward(120)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, executeTransaction(5)];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#executeTransaction (fast, specific)', function () {
        it('Succeeds for specific address', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, executeTransaction(0)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for other addresses', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(executeTransaction(1), 'TIME_LOCK_INCOMPLETE')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#executeTransaction (fast, all)', function () {
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, executeTransaction(3)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for rando', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(executeTransaction(3, rando))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#executeTransaction (fallback)', function () {
        it('Succeeds for specific address', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, executeTransaction(7)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails for other addresses', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(executeTransaction(6), 'TIME_LOCK_INCOMPLETE')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('#executeTransaction (short data)', function () {
        it('Fails', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(executeTransaction(8), 'TIME_LOCK_INCOMPLETE')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
// ============ Helper Functions ============
function submitTransaction(destination, data) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.contracts.send(multiSig.methods.submitTransaction(destination, '0', // value
                data), { from: owner1 })];
        });
    });
}
function confirmTransaction(n) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.contracts.send(multiSig.methods.confirmTransaction(n.toString()), { from: owner2 })];
        });
    });
}
function setSelectorData(destination, selector, approved) {
    var data = multiSig.methods.setSelector(destination, selector, approved).encodeABI();
    return hexToBytes(data);
}
function functionOneData() {
    var data = testCounterA.methods.functionOne().encodeABI();
    return hexToBytes(data);
}
function numberToFunctionTwoData(n) {
    var data = testCounterA.methods.functionTwo(n.toString()).encodeABI();
    return hexToBytes(data);
}
function numbersToFunctionThreeData(n1, n2) {
    var data = testCounterA.methods.functionThree(n1.toString(), n2.toString()).encodeABI();
    return hexToBytes(data);
}
function hexToBytes(hex) {
    return hex.toLowerCase().match(/.{1,2}/g).slice(1).map(function (x) { return [new bignumber_js_1.default(x, 16).toNumber()]; });
}
function executeTransaction(n, from) {
    return __awaiter(this, void 0, void 0, function () {
        var txResult, transaction;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.contracts.send(multiSig.methods.executeTransaction(n.toString()), {
                        from: from || owner3,
                        gas: '5000000',
                    })];
                case 1:
                    txResult = _a.sent();
                    return [4 /*yield*/, solo.contracts.call(multiSig.methods.transactions(n.toString()))];
                case 2:
                    transaction = _a.sent();
                    expect(transaction.executed).toEqual(true);
                    return [2 /*return*/, txResult];
            }
        });
    });
}
function expectInstantData(dest, selector, expected) {
    return __awaiter(this, void 0, void 0, function () {
        var result;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.contracts.call(multiSig.methods.instantData(dest, selector))];
                case 1:
                    result = _a.sent();
                    expect(result).toEqual(expected);
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=PartiallyDelayedMultisig.test.js.map