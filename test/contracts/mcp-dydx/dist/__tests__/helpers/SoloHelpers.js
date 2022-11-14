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
Object.defineProperty(exports, "__esModule", { value: true });
var bignumber_js_1 = __importDefault(require("bignumber.js"));
var EVM_1 = require("./EVM");
var Constants_1 = require("../../src/lib/Constants");
function setupMarkets(solo, accounts, numMarkets) {
    if (numMarkets === void 0) { numMarkets = 3; }
    return __awaiter(this, void 0, void 0, function () {
        var priceOracle, interestSetter, price, marginPremium, spreadPremium, tokens, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    priceOracle = solo.testing.priceOracle.getAddress();
                    interestSetter = solo.testing.interestSetter.getAddress();
                    price = new bignumber_js_1.default('1e40');
                    marginPremium = new bignumber_js_1.default(0);
                    spreadPremium = new bignumber_js_1.default(0);
                    return [4 /*yield*/, Promise.all([
                            solo.testing.priceOracle.setPrice(solo.testing.tokenA.getAddress(), price),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenB.getAddress(), price),
                            solo.testing.priceOracle.setPrice(solo.testing.tokenC.getAddress(), price),
                            solo.testing.priceOracle.setPrice(Constants_1.ADDRESSES.ZERO, price),
                        ])];
                case 1:
                    _a.sent();
                    tokens = [
                        solo.testing.tokenA.getAddress(),
                        solo.testing.tokenB.getAddress(),
                        solo.testing.tokenC.getAddress(),
                        Constants_1.ADDRESSES.ZERO,
                    ];
                    i = 0;
                    _a.label = 2;
                case 2:
                    if (!(i < numMarkets && i < tokens.length)) return [3 /*break*/, 5];
                    return [4 /*yield*/, solo.admin.addMarket(tokens[i], priceOracle, interestSetter, marginPremium, spreadPremium, { from: accounts[0] })];
                case 3:
                    _a.sent();
                    _a.label = 4;
                case 4:
                    i += 1;
                    return [3 /*break*/, 2];
                case 5: return [4 /*yield*/, EVM_1.mineAvgBlock()];
                case 6:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
exports.setupMarkets = setupMarkets;
//# sourceMappingURL=SoloHelpers.js.map