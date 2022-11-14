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
var _this = this;
Object.defineProperty(exports, "__esModule", { value: true });
var Solo_1 = require("./helpers/Solo");
var EVM_1 = require("./helpers/EVM");
var SoloHelpers_1 = require("./helpers/SoloHelpers");
var solo;
var accounts;
var operator1;
var operator2;
var operator3;
var owner;
describe('Permission', function () {
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
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, SoloHelpers_1.setupMarkets(solo, accounts)];
                case 3:
                    _a.sent();
                    owner = solo.getDefaultAccount();
                    operator1 = accounts[6];
                    operator2 = accounts[7];
                    operator3 = accounts[8];
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
    // ============ Getters for Risk ============
    describe('setOperators', function () {
        it('Succeeds for single approve', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, expectOperator(operator1, false)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.permissions.approveOperator(operator1, { from: owner })];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectOperator(operator1, true)];
                    case 3:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogOperatorSet');
                        expect(log.args.owner).toEqual(owner);
                        expect(log.args.operator).toEqual(operator1);
                        expect(log.args.trusted).toEqual(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for single disapprove', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.permissions.approveOperator(operator1, { from: owner })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, expectOperator(operator1, true)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.permissions.disapproveOperator(operator1, { from: owner })];
                    case 3:
                        txResult = _a.sent();
                        return [4 /*yield*/, expectOperator(operator1, false)];
                    case 4:
                        _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(1);
                        log = logs[0];
                        expect(log.name).toEqual('LogOperatorSet');
                        expect(log.args.owner).toEqual(owner);
                        expect(log.args.operator).toEqual(operator1);
                        expect(log.args.trusted).toEqual(false);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for multiple approve/disapprove', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, log1, log2, log3;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.permissions.setOperators([
                            { operator: operator1, trusted: true },
                            { operator: operator2, trusted: false },
                            { operator: operator3, trusted: true },
                        ])];
                    case 1:
                        txResult = _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(3);
                        log1 = logs[0], log2 = logs[1], log3 = logs[2];
                        expect(log1.name).toEqual('LogOperatorSet');
                        expect(log1.args.owner).toEqual(owner);
                        expect(log1.args.operator).toEqual(operator1);
                        expect(log1.args.trusted).toEqual(true);
                        expect(log2.name).toEqual('LogOperatorSet');
                        expect(log2.args.owner).toEqual(owner);
                        expect(log2.args.operator).toEqual(operator2);
                        expect(log2.args.trusted).toEqual(false);
                        expect(log3.name).toEqual('LogOperatorSet');
                        expect(log3.args.owner).toEqual(owner);
                        expect(log3.args.operator).toEqual(operator3);
                        expect(log3.args.trusted).toEqual(true);
                        return [4 /*yield*/, Promise.all([
                                expectOperator(operator1, true),
                                expectOperator(operator2, false),
                                expectOperator(operator3, true),
                            ])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for multiple repeated approve/disapprove', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs, log1, log2, log3, log4;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.permissions.setOperators([
                            { operator: operator1, trusted: true },
                            { operator: operator1, trusted: false },
                            { operator: operator2, trusted: true },
                            { operator: operator2, trusted: true },
                        ])];
                    case 1:
                        txResult = _a.sent();
                        logs = solo.logs.parseLogs(txResult);
                        expect(logs.length).toEqual(4);
                        log1 = logs[0], log2 = logs[1], log3 = logs[2], log4 = logs[3];
                        expect(log1.name).toEqual('LogOperatorSet');
                        expect(log1.args.owner).toEqual(owner);
                        expect(log1.args.operator).toEqual(operator1);
                        expect(log1.args.trusted).toEqual(true);
                        expect(log2.name).toEqual('LogOperatorSet');
                        expect(log2.args.owner).toEqual(owner);
                        expect(log2.args.operator).toEqual(operator1);
                        expect(log2.args.trusted).toEqual(false);
                        expect(log3.name).toEqual('LogOperatorSet');
                        expect(log3.args.owner).toEqual(owner);
                        expect(log3.args.operator).toEqual(operator2);
                        expect(log3.args.trusted).toEqual(true);
                        expect(log4.name).toEqual('LogOperatorSet');
                        expect(log4.args.owner).toEqual(owner);
                        expect(log4.args.operator).toEqual(operator2);
                        expect(log4.args.trusted).toEqual(true);
                        return [4 /*yield*/, Promise.all([
                                expectOperator(operator1, false),
                                expectOperator(operator2, true),
                            ])];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Skips logs when necessary', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, logs;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.permissions.approveOperator(operator1, { from: owner })];
                    case 1:
                        txResult = _a.sent();
                        logs = solo.logs.parseLogs(txResult, { skipPermissionLogs: true });
                        expect(logs.length).toEqual(0);
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
function expectOperator(operator, b) {
    return __awaiter(this, void 0, void 0, function () {
        var result;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getIsLocalOperator(owner, operator)];
                case 1:
                    result = _a.sent();
                    expect(result).toEqual(b);
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Permission.test.js.map