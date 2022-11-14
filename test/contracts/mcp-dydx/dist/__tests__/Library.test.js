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
var web3_1 = __importDefault(require("web3"));
var Solo_1 = require("./helpers/Solo");
var EVM_1 = require("./helpers/EVM");
var Expect_1 = require("../src/lib/Expect");
var Constants_1 = require("../src/lib/Constants");
var BytesHelper_1 = require("../src/lib/BytesHelper");
var SignatureHelper_1 = require("../src/lib/SignatureHelper");
var solo;
var owner;
var zero = '0';
var amount = '100';
var addr = Constants_1.ADDRESSES.TEST[0];
describe('Library', function () {
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    owner = solo.getDefaultAccount();
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    describe('TypedSignature', function () {
        var hash = '0x1234567812345678123456781234567812345678123456781234567812345678';
        var r = '0x30755ed65396facf86c53e6217c52b4daebe72aa4941d89635409de4c9c7f946';
        var s = '0x6d4e9aaec7977f05e923889b33c0d0dd27d7226b6e6f56ce737465c5cfd04be4';
        var v = '0x1b';
        var signature = "" + r + BytesHelper_1.stripHexPrefix(s) + BytesHelper_1.stripHexPrefix(v);
        function recover(hash, typedSignature) {
            return __awaiter(this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    return [2 /*return*/, solo.contracts.call(solo.contracts.testLib.methods.TypedSignatureRecover(web3_1.default.utils.hexToBytes(hash), web3_1.default.utils.hexToBytes(typedSignature).map(function (x) { return [x]; })))];
                });
            });
        }
        describe('recover', function () {
            it('fails for invalid signature length', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(recover(hash, hash.slice(0, -2)), 'TypedSignature: Invalid signature length')];
                        case 1:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('fails for invalid signature type', function () { return __awaiter(_this, void 0, void 0, function () {
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0: return [4 /*yield*/, Expect_1.expectThrow(recover(hash, "0x" + '00'.repeat(65) + "04"), 'TypedSignature: Invalid signature type')];
                        case 1:
                            _a.sent();
                            return [4 /*yield*/, Expect_1.expectThrow(recover(hash, "0x" + '00'.repeat(65) + "05"), 'TypedSignature: Invalid signature type')];
                        case 2:
                            _a.sent();
                            return [2 /*return*/];
                    }
                });
            }); });
            it('succeeds for no prepend', function () { return __awaiter(_this, void 0, void 0, function () {
                var signer, recoveredAddress;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            signer = solo.web3.eth.accounts.recover({ r: r, s: s, v: v, messageHash: hash });
                            return [4 /*yield*/, recover(hash, SignatureHelper_1.createTypedSignature(signature, SignatureHelper_1.SIGNATURE_TYPES.NO_PREPEND))];
                        case 1:
                            recoveredAddress = _a.sent();
                            expect(recoveredAddress).toEqual(signer);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('succeeds for decimal prepend', function () { return __awaiter(_this, void 0, void 0, function () {
                var decHash, signer, recoveredAddress;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            decHash = web3_1.default.utils.soliditySha3({ t: 'string', v: SignatureHelper_1.PREPEND_DEC }, { t: 'bytes32', v: hash });
                            signer = solo.web3.eth.accounts.recover({ r: r, s: s, v: v, messageHash: decHash });
                            return [4 /*yield*/, recover(hash, SignatureHelper_1.createTypedSignature(signature, SignatureHelper_1.SIGNATURE_TYPES.DECIMAL))];
                        case 1:
                            recoveredAddress = _a.sent();
                            expect(recoveredAddress).toEqual(signer);
                            return [2 /*return*/];
                    }
                });
            }); });
            it('succeeds for hexadecimal prepend', function () { return __awaiter(_this, void 0, void 0, function () {
                var hexHash, signer, recoveredAddress;
                return __generator(this, function (_a) {
                    switch (_a.label) {
                        case 0:
                            hexHash = web3_1.default.utils.soliditySha3({ t: 'string', v: SignatureHelper_1.PREPEND_HEX }, { t: 'bytes32', v: hash });
                            signer = solo.web3.eth.accounts.recover({ r: r, s: s, v: v, messageHash: hexHash });
                            return [4 /*yield*/, recover(hash, SignatureHelper_1.createTypedSignature(signature, SignatureHelper_1.SIGNATURE_TYPES.HEXADECIMAL))];
                        case 1:
                            recoveredAddress = _a.sent();
                            expect(recoveredAddress).toEqual(signer);
                            return [2 /*return*/];
                    }
                });
            }); });
        });
    });
    describe('Math', function () {
        var BN_DOWN = bignumber_js_1.default.clone({ ROUNDING_MODE: 1 });
        var BN_UP = bignumber_js_1.default.clone({ ROUNDING_MODE: 0 });
        var large = Constants_1.INTEGERS.ONES_255.div('1.5').toFixed(0);
        var tests = [
            [1, 1, 1],
            [2, 0, 3],
            [0, 3, 2],
            [2, 3, 4],
            [1241, 249835, 89234],
            [1289, 12431, 1],
            [1, 12341, 98],
            [12, 1, 878978],
            [0, 0, 1],
            [1, 1, 999],
            [998, 2, 999],
            [40, 50, 21],
        ];
        it('getPartial', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all(tests.map(function (args) { return solo.contracts.testLib.methods.MathGetPartial(args[0], args[1], args[2]).call(); }))];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual(tests.map(function (args) { return new BN_DOWN(args[0]).times(args[1]).div(args[2]).toFixed(0); }));
                        return [2 /*return*/];
                }
            });
        }); });
        it('getPartial reverts', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.MathGetPartial(1, 1, 0).call())];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.MathGetPartial(large, large, 1).call())];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('getPartialRoundUp', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all(tests.map(function (args) { return solo.contracts.testLib.methods.MathGetPartialRoundUp(args[0], args[1], args[2]).call(); }))];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual(tests.map(function (args) { return new BN_UP(args[0]).times(args[1]).div(args[2]).toFixed(0); }));
                        return [2 /*return*/];
                }
            });
        }); });
        it('getPartialRoundUp reverts', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.MathGetPartialRoundUp(1, 1, 0).call())];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.MathGetPartialRoundUp(large, large, 1).call())];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('to128', function () { return __awaiter(_this, void 0, void 0, function () {
            var large, small, result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        large = '340282366920938463463374607431768211456';
                        small = '340282366920938463463374607431768211455';
                        return [4 /*yield*/, solo.contracts.testLib.methods.MathTo128(small).call()];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual(small);
                        return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.MathTo128(large).call())];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('to96', function () { return __awaiter(_this, void 0, void 0, function () {
            var large, small, result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        large = '79228162514264337593543950336';
                        small = '79228162514264337593543950335';
                        return [4 /*yield*/, solo.contracts.testLib.methods.MathTo96(small).call()];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual(small);
                        return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.MathTo96(large).call())];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('to32', function () { return __awaiter(_this, void 0, void 0, function () {
            var large, small, result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0:
                        large = '4294967296';
                        small = '4294967295';
                        return [4 /*yield*/, solo.contracts.testLib.methods.MathTo32(small).call()];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual(small);
                        return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.MathTo32(large).call())];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Require', function () {
        var bytes32Hex = "0x" + '0123456789abcdef'.repeat(4);
        var emptyReason = '0x0000000000000000000000000000000000000000000000000000000000000000';
        var reason1 = '0x5468697320497320746865205465787420526561736f6e2e3031323334353637';
        var reasonString1 = 'This Is the Text Reason.01234567';
        var reason2 = '0x53686f727420526561736f6e2030393800000000000000000000000000000000';
        var reasonString2 = 'Short Reason 098';
        var arg1 = '0';
        var arg2 = '1234567890987654321';
        var arg3 = Constants_1.INTEGERS.ONES_255.toFixed(0);
        it('that (emptyString)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThat1(emptyReason, arg1).call(), "TestLib:  <" + arg1 + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (0 args)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThat0(reason1).call(), "TestLib: " + reasonString1)];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (1 args)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThat1(reason2, arg1).call(), "TestLib: " + reasonString2 + " <" + arg1 + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (2 args)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThat2(reason1, arg2, arg3).call(), "TestLib: " + reasonString1 + " <" + arg2 + ", " + arg3 + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (address arg)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThatA0(reason2, addr).call(), "TestLib: " + reasonString2 + " <" + addr + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (1 address, 1 number)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThatA1(reason2, addr, arg1).call(), "TestLib: " + reasonString2 + " <" + addr + ", " + arg1 + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (1 address, 2 numbers)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThatA2(reason2, addr, arg1, arg3).call(), "TestLib: " + reasonString2 + " <" + addr + ", " + arg1 + ", " + arg3 + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (bytes32 arg)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThatB0(reason1, bytes32Hex).call(), "TestLib: " + reasonString1 + " <" + bytes32Hex + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('that (1 bytes32, 2 numbers)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testLib.methods.RequireThatB2(reason2, bytes32Hex, arg1, arg3).call(), "TestLib: " + reasonString2 + " <" + bytes32Hex + ", " + arg1 + ", " + arg3 + ">")];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Time', function () {
        it('currentTime', function () { return __awaiter(_this, void 0, void 0, function () {
            var _a, block1, time1, _b, block2, time2;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.web3.eth.getBlock('latest'),
                            solo.contracts.testLib.methods.TimeCurrentTime().call(),
                        ])];
                    case 1:
                        _a = _c.sent(), block1 = _a[0], time1 = _a[1];
                        return [4 /*yield*/, EVM_1.mineAvgBlock()];
                    case 2:
                        _c.sent();
                        return [4 /*yield*/, Promise.all([
                                solo.web3.eth.getBlock('latest'),
                                solo.contracts.testLib.methods.TimeCurrentTime().call(),
                            ])];
                    case 3:
                        _b = _c.sent(), block2 = _b[0], time2 = _b[1];
                        expect(new bignumber_js_1.default(time1).toNumber()).toBeGreaterThanOrEqual(block1.timestamp);
                        expect(new bignumber_js_1.default(time2).toNumber()).toBeGreaterThanOrEqual(block2.timestamp);
                        expect(block2.timestamp).toBeGreaterThanOrEqual(block1.timestamp + 15);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Token', function () {
        var result;
        var token;
        var errorToken;
        var omise;
        var libAddr;
        beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                token = solo.contracts.tokenA.options.address;
                errorToken = solo.contracts.erroringToken.options.address;
                omise = solo.contracts.omiseToken.options.address;
                libAddr = solo.contracts.testLib.options.address;
                return [2 /*return*/];
            });
        }); });
        it('balanceOf (normal)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(token, addr).call()];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual(zero);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.tokenA.methods.issueTo(addr, amount))];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(token, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('balanceOf (omise)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(omise, addr).call()];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual(zero);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.omiseToken.methods.issueTo(addr, amount))];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(omise, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('allowance (normal)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(token, owner, addr).call()];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual(zero);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.tokenA.methods.approve(addr, amount), { from: owner })];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(token, owner, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('allowance (omise)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(omise, owner, addr).call()];
                    case 1:
                        result = _a.sent();
                        expect(result).toEqual(zero);
                        return [4 /*yield*/, solo.contracts.send(solo.contracts.omiseToken.methods.approve(addr, amount), { from: owner })];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(omise, owner, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('approve (normal)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.testLib.methods.TokenApprove(token, addr, amount))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(token, libAddr, addr).call()];
                    case 2:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('approve (error)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.testLib.methods.TokenApprove(errorToken, addr, amount)), 'Token: Approve failed')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('approve (omise)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.testLib.methods.TokenApprove(omise, addr, amount))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(omise, libAddr, addr).call()];
                    case 2:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('approveMax (normal)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.testLib.methods.TokenApproveMax(token, addr))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(token, libAddr, addr).call()];
                    case 2:
                        result = _a.sent();
                        expect(result).toEqual(Constants_1.INTEGERS.ONES_255.toFixed(0));
                        return [2 /*return*/];
                }
            });
        }); });
        it('approveMax (error)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.testLib.methods.TokenApproveMax(errorToken, addr)), 'Token: Approve failed')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('approveMax (omise)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.testLib.methods.TokenApproveMax(omise, addr))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenAllowance(omise, libAddr, addr).call()];
                    case 2:
                        result = _a.sent();
                        expect(result).toEqual(Constants_1.INTEGERS.ONES_255.toFixed(0));
                        return [2 /*return*/];
                }
            });
        }); });
        it('transfer (normal)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.tokenA.methods.issueTo(libAddr, amount))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenTransfer(token, addr, amount)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(token, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('transfer (omise)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.omiseToken.methods.issueTo(libAddr, amount))];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenTransfer(omise, addr, amount)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(omise, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('transfer (error)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.testLib.methods.TokenTransfer(errorToken, addr, amount)), 'Token: Transfer failed')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('transferFrom (normal)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.send(solo.contracts.tokenA.methods.issueTo(owner, amount)),
                            solo.contracts.send(solo.contracts.tokenA.methods.approve(libAddr, amount), { from: owner }),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenTransferFrom(token, owner, addr, amount)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(token, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
        it('transferFrom (error)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.send(solo.contracts.testLib.methods.TokenTransferFrom(errorToken, owner, addr, amount)), 'Token: TransferFrom failed')];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('transferFrom (omise)', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.send(solo.contracts.omiseToken.methods.issueTo(owner, amount)),
                            solo.contracts.send(solo.contracts.omiseToken.methods.approve(libAddr, amount), { from: owner }),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenTransferFrom(omise, owner, addr, amount)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.contracts.testLib.methods.TokenBalanceOf(omise, addr).call()];
                    case 3:
                        result = _a.sent();
                        expect(result).toEqual(amount);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('Types', function () {
        var lo = '10';
        var hi = '20';
        var negZo = { sign: false, value: zero };
        var posZo = { sign: true, value: zero };
        var negLo = { sign: false, value: lo };
        var posLo = { sign: true, value: lo };
        var negHi = { sign: false, value: hi };
        var posHi = { sign: true, value: hi };
        it('zeroPar', function () { return __awaiter(_this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.testLib.methods.TypesZeroPar().call()];
                    case 1:
                        result = _a.sent();
                        expect(result.sign).toStrictEqual(false);
                        expect(result.value).toEqual(zero);
                        return [2 /*return*/];
                }
            });
        }); });
        it('parSub', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesParSub(posLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParSub(posLo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParSub(posZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParSub(posZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParSub(negZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParSub(negZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParSub(negLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParSub(negLo, negZo).call(),
                        ])];
                    case 1:
                        // sub zero
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, posLo, posZo, posZo, negZo, negZo, negLo, negLo]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesParSub(posLo, posHi).call(),
                                solo.contracts.testLib.methods.TypesParSub(posLo, posLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(posZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(negZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(posHi, posLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(negLo, posLo).call(),
                            ])];
                    case 2:
                        // sub positive
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([negLo, posZo, negLo, negLo, posLo, negHi]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesParSub(negLo, negHi).call(),
                                solo.contracts.testLib.methods.TypesParSub(negLo, negLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(negZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(posZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(negHi, negLo).call(),
                                solo.contracts.testLib.methods.TypesParSub(posLo, negLo).call(),
                            ])];
                    case 3:
                        // sub negative
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, negZo, posLo, posLo, negLo, posHi]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('parAdd', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesParAdd(posLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParAdd(posLo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParAdd(posZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParAdd(posZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParAdd(negZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParAdd(negZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParAdd(negLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParAdd(negLo, negZo).call(),
                        ])];
                    case 1:
                        // add zero
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, posLo, posZo, posZo, negZo, negZo, negLo, negLo]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesParAdd(negLo, posHi).call(),
                                solo.contracts.testLib.methods.TypesParAdd(negLo, posLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(negZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(posZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(negHi, posLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(posLo, posLo).call(),
                            ])];
                    case 2:
                        // add positive
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, negZo, posLo, posLo, negLo, posHi]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesParAdd(posLo, negHi).call(),
                                solo.contracts.testLib.methods.TypesParAdd(posLo, negLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(posZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(negZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(posHi, negLo).call(),
                                solo.contracts.testLib.methods.TypesParAdd(negLo, negLo).call(),
                            ])];
                    case 3:
                        // add negative
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([negLo, posZo, negLo, negLo, posLo, negHi]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('parEquals', function () { return __awaiter(_this, void 0, void 0, function () {
            var trues, falses;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesParEquals(posHi, posHi).call(),
                            solo.contracts.testLib.methods.TypesParEquals(posLo, posLo).call(),
                            solo.contracts.testLib.methods.TypesParEquals(posZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesParEquals(posZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParEquals(negZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesParEquals(negLo, negLo).call(),
                            solo.contracts.testLib.methods.TypesParEquals(negHi, negHi).call(),
                        ])];
                    case 1:
                        trues = _a.sent();
                        expect(trues).toEqual([true, true, true, true, true, true, true]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesParEquals(posHi, posLo).call(),
                                solo.contracts.testLib.methods.TypesParEquals(posLo, negLo).call(),
                                solo.contracts.testLib.methods.TypesParEquals(posHi, negHi).call(),
                                solo.contracts.testLib.methods.TypesParEquals(posZo, negHi).call(),
                                solo.contracts.testLib.methods.TypesParEquals(negHi, negLo).call(),
                                solo.contracts.testLib.methods.TypesParEquals(negLo, posLo).call(),
                                solo.contracts.testLib.methods.TypesParEquals(negLo, posHi).call(),
                            ])];
                    case 2:
                        falses = _a.sent();
                        expect(falses).toEqual([false, false, false, false, false, false, false]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('parNegative', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesParNegative(posHi).call(),
                            solo.contracts.testLib.methods.TypesParNegative(posLo).call(),
                            solo.contracts.testLib.methods.TypesParNegative(posZo).call(),
                            solo.contracts.testLib.methods.TypesParNegative(negZo).call(),
                            solo.contracts.testLib.methods.TypesParNegative(negLo).call(),
                            solo.contracts.testLib.methods.TypesParNegative(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([negHi, negLo, negZo, posZo, posLo, posHi]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('parIsNegative', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesParIsNegative(posHi).call(),
                            solo.contracts.testLib.methods.TypesParIsNegative(posLo).call(),
                            solo.contracts.testLib.methods.TypesParIsNegative(posZo).call(),
                            solo.contracts.testLib.methods.TypesParIsNegative(negZo).call(),
                            solo.contracts.testLib.methods.TypesParIsNegative(negLo).call(),
                            solo.contracts.testLib.methods.TypesParIsNegative(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual([false, false, false, false, true, true]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('parIsPositive', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesParIsPositive(posHi).call(),
                            solo.contracts.testLib.methods.TypesParIsPositive(posLo).call(),
                            solo.contracts.testLib.methods.TypesParIsPositive(posZo).call(),
                            solo.contracts.testLib.methods.TypesParIsPositive(negZo).call(),
                            solo.contracts.testLib.methods.TypesParIsPositive(negLo).call(),
                            solo.contracts.testLib.methods.TypesParIsPositive(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual([true, true, false, false, false, false]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('parIsZero', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesParIsZero(posHi).call(),
                            solo.contracts.testLib.methods.TypesParIsZero(posLo).call(),
                            solo.contracts.testLib.methods.TypesParIsZero(posZo).call(),
                            solo.contracts.testLib.methods.TypesParIsZero(negZo).call(),
                            solo.contracts.testLib.methods.TypesParIsZero(negLo).call(),
                            solo.contracts.testLib.methods.TypesParIsZero(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual([false, false, true, true, false, false]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('zeroWei', function () { return __awaiter(_this, void 0, void 0, function () {
            var result;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.contracts.testLib.methods.TypesZeroWei().call()];
                    case 1:
                        result = _a.sent();
                        expect(result.sign).toStrictEqual(false);
                        expect(result.value).toEqual(zero);
                        return [2 /*return*/];
                }
            });
        }); });
        it('weiSub', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesWeiSub(posLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiSub(posLo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiSub(posZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiSub(posZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiSub(negZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiSub(negZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiSub(negLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiSub(negLo, negZo).call(),
                        ])];
                    case 1:
                        // sub zero
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, posLo, posZo, posZo, negZo, negZo, negLo, negLo]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesWeiSub(posLo, posHi).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(posLo, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(posZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(negZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(posHi, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(negLo, posLo).call(),
                            ])];
                    case 2:
                        // sub positive
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([negLo, posZo, negLo, negLo, posLo, negHi]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesWeiSub(negLo, negHi).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(negLo, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(negZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(posZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(negHi, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiSub(posLo, negLo).call(),
                            ])];
                    case 3:
                        // sub negative
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, negZo, posLo, posLo, negLo, posHi]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('weiAdd', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesWeiAdd(posLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiAdd(posLo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiAdd(posZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiAdd(posZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiAdd(negZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiAdd(negZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiAdd(negLo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiAdd(negLo, negZo).call(),
                        ])];
                    case 1:
                        // add zero
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, posLo, posZo, posZo, negZo, negZo, negLo, negLo]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesWeiAdd(negLo, posHi).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(negLo, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(negZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(posZo, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(negHi, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(posLo, posLo).call(),
                            ])];
                    case 2:
                        // add positive
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([posLo, negZo, posLo, posLo, negLo, posHi]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesWeiAdd(posLo, negHi).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(posLo, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(posZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(negZo, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(posHi, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiAdd(negLo, negLo).call(),
                            ])];
                    case 3:
                        // add negative
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([negLo, posZo, negLo, negLo, posLo, negHi]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('weiEquals', function () { return __awaiter(_this, void 0, void 0, function () {
            var trues, falses;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesWeiEquals(posHi, posHi).call(),
                            solo.contracts.testLib.methods.TypesWeiEquals(posLo, posLo).call(),
                            solo.contracts.testLib.methods.TypesWeiEquals(posZo, posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiEquals(posZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiEquals(negZo, negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiEquals(negLo, negLo).call(),
                            solo.contracts.testLib.methods.TypesWeiEquals(negHi, negHi).call(),
                        ])];
                    case 1:
                        trues = _a.sent();
                        expect(trues).toEqual([true, true, true, true, true, true, true]);
                        return [4 /*yield*/, Promise.all([
                                solo.contracts.testLib.methods.TypesWeiEquals(posHi, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiEquals(posLo, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiEquals(posHi, negHi).call(),
                                solo.contracts.testLib.methods.TypesWeiEquals(posZo, negHi).call(),
                                solo.contracts.testLib.methods.TypesWeiEquals(negHi, negLo).call(),
                                solo.contracts.testLib.methods.TypesWeiEquals(negLo, posLo).call(),
                                solo.contracts.testLib.methods.TypesWeiEquals(negLo, posHi).call(),
                            ])];
                    case 2:
                        falses = _a.sent();
                        expect(falses).toEqual([false, false, false, false, false, false, false]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('weiNegative', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesWeiNegative(posHi).call(),
                            solo.contracts.testLib.methods.TypesWeiNegative(posLo).call(),
                            solo.contracts.testLib.methods.TypesWeiNegative(posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiNegative(negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiNegative(negLo).call(),
                            solo.contracts.testLib.methods.TypesWeiNegative(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results.map(parse)).toEqual([negHi, negLo, negZo, posZo, posLo, posHi]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('weiIsNegative', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesWeiIsNegative(posHi).call(),
                            solo.contracts.testLib.methods.TypesWeiIsNegative(posLo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsNegative(posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsNegative(negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsNegative(negLo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsNegative(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual([false, false, false, false, true, true]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('weiIsPositive', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesWeiIsPositive(posHi).call(),
                            solo.contracts.testLib.methods.TypesWeiIsPositive(posLo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsPositive(posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsPositive(negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsPositive(negLo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsPositive(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual([true, true, false, false, false, false]);
                        return [2 /*return*/];
                }
            });
        }); });
        it('weiIsZero', function () { return __awaiter(_this, void 0, void 0, function () {
            var results;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            solo.contracts.testLib.methods.TypesWeiIsZero(posHi).call(),
                            solo.contracts.testLib.methods.TypesWeiIsZero(posLo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsZero(posZo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsZero(negZo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsZero(negLo).call(),
                            solo.contracts.testLib.methods.TypesWeiIsZero(negHi).call(),
                        ])];
                    case 1:
                        results = _a.sent();
                        expect(results).toEqual([false, false, true, true, false, false]);
                        return [2 /*return*/];
                }
            });
        }); });
        function parse(value) {
            return { sign: value[0], value: value[1] };
        }
    });
});
//# sourceMappingURL=Library.test.js.map