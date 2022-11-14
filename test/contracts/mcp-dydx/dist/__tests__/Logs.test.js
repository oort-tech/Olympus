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
var solo;
describe('Logs', function () {
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds in parsing txResult.logs', function () { return __awaiter(_this, void 0, void 0, function () {
        var txResult, logs;
        return __generator(this, function (_a) {
            txResult = {
                logs: [
                    {
                        address: '0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2',
                        blockHash: '0x81441018c1131afd6f7ceec2077257f4ecfc3325d56b375bf370008d17a20d65',
                        blockNumber: 7492404,
                        data: '0x00000000000000000000000000000000000000000000000000038d7ea4c68000',
                        logIndex: 119,
                        removed: false,
                        topics: [
                            '0xe1fffcc4923d04b559f4d29a8bfc6cda04eb5b0d3c460751c2402c5c5cc9109c',
                            '0x0000000000000000000000006e86dc68723d9811f67d9f6acfec6ec9d3818527',
                        ],
                        transactionHash: '0xfbb9bc794809a190e7a18278181128d53ed41cec7bf34667e7052edfbff8ad69',
                        transactionIndex: 152,
                        transactionLogIndex: '0x0',
                        type: 'mined',
                        id: 'log_21ca9c63',
                    },
                    {
                        address: solo.contracts.testSoloMargin.options.address,
                        blockHash: '0x81441018c1131afd6f7ceec2077257f4ecfc3325d56b375bf370008d17a20d65',
                        blockNumber: 7492404,
                        data: '0x0000000000000000000000006e86dc68723d9811f67d9f6acfec6ec9d3818527',
                        logIndex: 120,
                        removed: false,
                        topics: [
                            '0x91b01baeee3a24b590d112613814d86801005c7ef9353e7fc1eaeaf33ccf83b0',
                        ],
                        transactionHash: '0xfbb9bc794809a190e7a18278181128d53ed41cec7bf34667e7052edfbff8ad69',
                        transactionIndex: 152,
                        transactionLogIndex: '0x1',
                        type: 'mined',
                        id: 'log_0ffe7292',
                    },
                    {
                        address: '0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2',
                        blockHash: '0x81441018c1131afd6f7ceec2077257f4ecfc3325d56b375bf370008d17a20d65',
                        blockNumber: 7492404,
                        data: '0x00000000000000000000000000000000000000000000000000038d7ea4c68000',
                        logIndex: 122,
                        removed: false,
                        topics: [
                            '0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef',
                            '0x0000000000000000000000006e86dc68723d9811f67d9f6acfec6ec9d3818527',
                            '0x00000000000000000000000022cb2d5de2009c9afd321bdf8759743665e45844',
                        ],
                        transactionHash: '0xfbb9bc794809a190e7a18278181128d53ed41cec7bf34667e7052edfbff8ad69',
                        transactionIndex: 152,
                        transactionLogIndex: '0x3',
                        type: 'mined',
                        id: 'log_e4f19380',
                    },
                    {
                        address: solo.contracts.testSoloMargin.options.address,
                        blockHash: '0x81441018c1131afd6f7ceec2077257f4ecfc3325d56b375bf370008d17a20d65',
                        blockNumber: 7492404,
                        data: '0x0000000000000000000000000000000000000000000000000000000000000000' +
                            '0000000000000000000000000000000000000000000000000000000000000000' +
                            '0000000000000000000000000000000000000000000000000000000000000001' +
                            '00000000000000000000000000000000000000000000000000038d7ea4c68000' +
                            '0000000000000000000000000000000000000000000000000000000000000001' +
                            '00000000000000000000000000000000000000000000000000038d7ea4c68000' +
                            '0000000000000000000000006e86dc68723d9811f67d9f6acfec6ec9d3818527',
                        logIndex: 123,
                        removed: false,
                        topics: [
                            '0x2bad8bc95088af2c247b30fa2b2e6a0886f88625e0945cd3051008e0e270198f',
                            '0x0000000000000000000000006a08b12aa520d319768e0d3a779af8660794c5e1',
                        ],
                        transactionHash: '0xfbb9bc794809a190e7a18278181128d53ed41cec7bf34667e7052edfbff8ad69',
                        transactionIndex: 152,
                        transactionLogIndex: '0x4',
                        type: 'mined',
                        id: 'log_9aab3f86',
                    },
                ],
            };
            logs = solo.logs.parseLogs(txResult);
            expect(logs.length).not.toEqual(0);
            return [2 /*return*/];
        });
    }); });
});
//# sourceMappingURL=Logs.test.js.map