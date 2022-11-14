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
var EVM_1 = require("../helpers/EVM");
var Constants_1 = require("../../src/lib/Constants");
var Expect_1 = require("../../src/lib/Expect");
var Helpers_1 = require("../../src/lib/Helpers");
var solo;
var owner;
var admin;
var accountNumber1 = new bignumber_js_1.default(111);
var accountNumber2 = new bignumber_js_1.default(222);
var zero = new bignumber_js_1.default(0);
var par = new bignumber_js_1.default(10000);
var negPar = par.times(-1);
var defaultPrice = new bignumber_js_1.default(10000);
var maximumRate = new bignumber_js_1.default(31709791983).div('1e18');
var defaultCoefficients = [0, 10, 10, 0, 0, 80];
var defaultMaxAPR = new bignumber_js_1.default('1.00');
describe('PolynomialInterestSetter', function () {
    var snapshotId;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    owner = solo.getDefaultAccount();
                    admin = r.accounts[0];
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.testing.priceOracle.setPrice(solo.testing.tokenA.getAddress(), defaultPrice)];
                case 3:
                    _a.sent();
                    return [4 /*yield*/, solo.admin.addMarket(solo.testing.tokenA.getAddress(), solo.testing.priceOracle.getAddress(), solo.testing.polynomialInterestSetter.getAddress(), zero, zero, { from: admin })];
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
    it('Succeeds for 0/0', function () { return __awaiter(_this, void 0, void 0, function () {
        var rate;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.getters.getMarketInterestRate(zero)];
                case 1:
                    rate = _a.sent();
                    expect(rate).toEqual(zero);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for 0/100', function () { return __awaiter(_this, void 0, void 0, function () {
        var rate;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner, accountNumber1, zero, par)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketInterestRate(zero)];
                case 2:
                    rate = _a.sent();
                    expect(rate).toEqual(zero);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for 100/0', function () { return __awaiter(_this, void 0, void 0, function () {
        var rate;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.testing.setAccountBalance(owner, accountNumber1, zero, negPar)];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketInterestRate(zero)];
                case 2:
                    rate = _a.sent();
                    expect(rate).toEqual(maximumRate);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for 100/100', function () { return __awaiter(_this, void 0, void 0, function () {
        var rate;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountBalance(owner, accountNumber1, zero, par),
                        solo.testing.setAccountBalance(owner, accountNumber2, zero, negPar),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketInterestRate(zero)];
                case 2:
                    rate = _a.sent();
                    expect(rate).toEqual(maximumRate);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for 200/100', function () { return __awaiter(_this, void 0, void 0, function () {
        var rate;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountBalance(owner, accountNumber1, zero, par),
                        solo.testing.setAccountBalance(owner, accountNumber2, zero, negPar.times(2)),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketInterestRate(zero)];
                case 2:
                    rate = _a.sent();
                    expect(rate).toEqual(maximumRate);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for 50/100', function () { return __awaiter(_this, void 0, void 0, function () {
        var rate;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Promise.all([
                        solo.testing.setAccountBalance(owner, accountNumber1, zero, par),
                        solo.testing.setAccountBalance(owner, accountNumber2, zero, negPar.div(2)),
                    ])];
                case 1:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketInterestRate(zero)];
                case 2:
                    rate = _a.sent();
                    expect(rate).toEqual(Helpers_1.getInterestPerSecondForPolynomial(defaultMaxAPR, defaultCoefficients, { totalBorrowed: par.div(2), totalSupply: par }));
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for 100% (javscript)', function () { return __awaiter(_this, void 0, void 0, function () {
        var res1, res2;
        return __generator(this, function (_a) {
            res1 = Helpers_1.getInterestPerSecondForPolynomial(defaultMaxAPR, defaultCoefficients, { totalBorrowed: par, totalSupply: par });
            res2 = Helpers_1.getInterestPerSecondForPolynomial(defaultMaxAPR, defaultCoefficients, { totalBorrowed: par.times(2), totalSupply: par });
            expect(maximumRate).toEqual(res1);
            expect(maximumRate).toEqual(res2);
            return [2 /*return*/];
        });
    }); });
    it('Succeeds for gas', function () { return __awaiter(_this, void 0, void 0, function () {
        var baseGasCost, getRateFunction, totalCosts, _a, _b, _c, costs;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0:
                    baseGasCost = 21000;
                    getRateFunction = solo.contracts.testPolynomialInterestSetter.methods.getInterestRate;
                    _b = (_a = Promise).all;
                    return [4 /*yield*/, getRateFunction(Constants_1.ADDRESSES.ZERO, '0', '0').estimateGas()];
                case 1:
                    _c = [
                        _d.sent()
                    ];
                    return [4 /*yield*/, getRateFunction(Constants_1.ADDRESSES.ZERO, '1', '1').estimateGas()];
                case 2:
                    _c = _c.concat([
                        _d.sent()
                    ]);
                    return [4 /*yield*/, getRateFunction(Constants_1.ADDRESSES.ZERO, '1', '2').estimateGas()];
                case 3: return [4 /*yield*/, _b.apply(_a, [_c.concat([
                            _d.sent()
                        ])])];
                case 4:
                    totalCosts = _d.sent();
                    costs = totalCosts.map(function (x) { return x - baseGasCost; });
                    console.log("\tInterest calculation gas used: " + costs[0] + ", " + costs[1] + ", " + costs[2]);
                    return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for bunch of utilization numbers', function () { return __awaiter(_this, void 0, void 0, function () {
        var i, utilization, rate;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i <= 100)) return [3 /*break*/, 5];
                    utilization = new bignumber_js_1.default(i).div(100);
                    return [4 /*yield*/, Promise.all([
                            solo.testing.setAccountBalance(owner, accountNumber1, zero, par),
                            solo.testing.setAccountBalance(owner, accountNumber2, zero, negPar.times(utilization)),
                        ])];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.getters.getMarketInterestRate(zero)];
                case 3:
                    rate = _a.sent();
                    expect(rate).toEqual(Helpers_1.getInterestPerSecondForPolynomial(defaultMaxAPR, defaultCoefficients, {
                        totalBorrowed: par.times(utilization),
                        totalSupply: par,
                    }));
                    _a.label = 4;
                case 4:
                    i += 5;
                    return [3 /*break*/, 1];
                case 5: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for setting/getting coefficients', function () { return __awaiter(_this, void 0, void 0, function () {
        var testCases, i, coefficients;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    testCases = [
                        [],
                        [100],
                        [10, 20, 30, 40],
                        [40, 30, 20, 10],
                        [0, 0, 0, 0, 0, 100],
                        [0, 30, 0, 40, 0, 30],
                    ];
                    i = 0;
                    _a.label = 1;
                case 1:
                    if (!(i < testCases.length)) return [3 /*break*/, 5];
                    coefficients = testCases[i];
                    return [4 /*yield*/, setCoefficients(zero, coefficients)];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, expectCoefficients(coefficients)];
                case 3:
                    _a.sent();
                    _a.label = 4;
                case 4:
                    i += 1;
                    return [3 /*break*/, 1];
                case 5: return [2 /*return*/];
            }
        });
    }); });
    it('Succeeds for setting/getting maxAPR', function () { return __awaiter(_this, void 0, void 0, function () {
        var maxAPR1, newAPR, maxAPR2;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.contracts.testPolynomialInterestSetter.methods.getMaxAPR().call()];
                case 1:
                    maxAPR1 = _a.sent();
                    expect(maxAPR1).toEqual(new bignumber_js_1.default('1e18').toFixed(0));
                    newAPR = new bignumber_js_1.default('1.5e18').toFixed(0);
                    expect(newAPR).not.toEqual(maxAPR1);
                    return [4 /*yield*/, solo.contracts.send(solo.contracts.testPolynomialInterestSetter.methods.setParameters({
                            maxAPR: newAPR,
                            coefficients: '100',
                        }))];
                case 2:
                    _a.sent();
                    return [4 /*yield*/, solo.contracts.testPolynomialInterestSetter.methods.getMaxAPR().call()];
                case 3:
                    maxAPR2 = _a.sent();
                    expect(maxAPR2).toEqual(newAPR);
                    return [2 /*return*/];
            }
        });
    }); });
    it("Fails to deploy contracts whose coefficients don't add to 100", function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.contracts.testPolynomialInterestSetter.methods.createNew({
                        maxAPR: '0',
                        coefficients: Helpers_1.coefficientsToString([10, 0, 10]),
                    }).call(), 'Coefficients must sum to 100')];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    }); });
});
// ============ Helper Functions ============
function expectCoefficients(expected) {
    return __awaiter(this, void 0, void 0, function () {
        var coefficients;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.contracts.testPolynomialInterestSetter.methods.getCoefficients().call()];
                case 1:
                    coefficients = _a.sent();
                    expect(coefficients).toEqual(expected.map(function (x) { return x.toString(); }));
                    return [2 /*return*/];
            }
        });
    });
}
function setCoefficients(maximumRate, coefficients) {
    return __awaiter(this, void 0, void 0, function () {
        var coefficientsString;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    coefficientsString = Helpers_1.coefficientsToString(coefficients);
                    return [4 /*yield*/, solo.contracts.send(solo.contracts.testPolynomialInterestSetter.methods.setParameters({
                            maxAPR: maximumRate.toFixed(0),
                            coefficients: coefficientsString,
                        }))];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
//# sourceMappingURL=PolynomialInterestSetter.test.js.map