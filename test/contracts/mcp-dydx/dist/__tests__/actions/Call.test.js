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
var BytesHelper_1 = require("../../src/lib/BytesHelper");
var Expect_1 = require("../../src/lib/Expect");
var types_1 = require("../../src/types");
var who;
var operator;
var solo;
var accounts;
var accountNumber = Constants_1.INTEGERS.ZERO;
var accountData = new bignumber_js_1.default(100);
var senderData = new bignumber_js_1.default(50);
var defaultGlob;
describe('Call', function () {
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
                    who = solo.getDefaultAccount();
                    operator = accounts[5];
                    defaultGlob = {
                        primaryAccountOwner: who,
                        primaryAccountId: accountNumber,
                        callee: solo.testing.callee.getAddress(),
                        data: [],
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
    it('Basic call test', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectCallOkay({
                        data: BytesHelper_1.toBytes(accountData, senderData),
                    })];
                case 1:
                    txResult = _a.sent();
                    return [4 /*yield*/, verifyDataIntegrity(who)];
                case 2:
                    _a.sent();
                    console.log("\tCall gas used: " + txResult.gasUsed);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for events', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, logs, operationLog, callLog;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(operator, { from: who })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectCallOkay({ data: BytesHelper_1.toBytes(accountData, senderData) }, { from: operator })];
                case 2:
                    txResult = _a.sent();
                    return [4 /*yield*/, verifyDataIntegrity(operator)];
                case 3:
                    _a.sent();
                    logs = solo.logs.parseLogs(txResult);
                    expect(logs.length).toEqual(2);
                    operationLog = logs[0];
                    expect(operationLog.name).toEqual('LogOperation');
                    expect(operationLog.args.sender).toEqual(operator);
                    callLog = logs[1];
                    expect(callLog.name).toEqual('LogCall');
                    expect(callLog.args.accountOwner).toEqual(who);
                    expect(callLog.args.accountNumber).toEqual(accountNumber);
                    expect(callLog.args.callee).toEqual(solo.testing.callee.getAddress());
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds and sets status to Normal', function () { return __awaiter(_this, void 0, void 0, function () {
        var status;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.testing.setAccountStatus(who, accountNumber, types_1.AccountStatus.Liquidating)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectCallOkay({
                            data: BytesHelper_1.toBytes(accountData, senderData),
                        })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, verifyDataIntegrity(who)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getAccountStatus(who, accountNumber)];
                case 4:
                    status = _a.sent();
                    expect(status).toEqual(types_1.AccountStatus.Normal);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for local operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.permissions.approveOperator(operator, { from: who })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectCallOkay({
                            data: BytesHelper_1.toBytes(accountData, senderData),
                        }, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, verifyDataIntegrity(operator)];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for global operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.admin.setGlobalOperator(operator, true, { from: accounts[0] })];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, expectCallOkay({
                            data: BytesHelper_1.toBytes(accountData, senderData),
                        }, { from: operator })];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, verifyDataIntegrity(operator)];
                case 3:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-operator', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectCallRevert({
                        data: BytesHelper_1.toBytes(accountData, senderData),
                    }, 'Storage: Unpermissioned operator', { from: operator })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
    it('Fails for non-ICallee contract', function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, expectCallRevert({
                        data: BytesHelper_1.toBytes(accountData, senderData),
                        callee: solo.testing.priceOracle.getAddress(),
                    })];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function expectCallOkay(glob, options) {
    return __awaiter(this, void 0, void 0, function () {
        var combinedGlob;
        return __generator(this, function (_a) {
            combinedGlob = __assign({}, defaultGlob, glob);
            return [2 /*return*/, solo.operation.initiate().call(combinedGlob).commit(options)];
        });
    });
}
function expectCallRevert(glob, reason, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(expectCallOkay(glob, options), reason)];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function verifyDataIntegrity(sender) {
    return __awaiter(this, void 0, void 0, function () {
        var _a, foundAccountData, foundSenderData;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.callee.getAccountData(who, accountNumber),
                        solo.testing.callee.getSenderData(sender),
                    ])];
                case 1:
                    _a = _b.sent(), foundAccountData = _a[0], foundSenderData = _a[1];
                    expect(foundAccountData).toEqual(accountData.toFixed(0));
                    expect(foundSenderData).toEqual(senderData.toFixed(0));
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=Call.test.js.map