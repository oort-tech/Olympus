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
var CURVE_FEE_DENOMINATOR = 10000000000;
var DAI_DECIMALS = 18;
var USDC_DECIMALS = 6;
var solo;
var accounts;
var admin;
var poker;
var rando;
var marketMaker;
var defaultPrice = new bignumber_js_1.default('1e18');
describe('DaiPriceOracle', function () {
    var snapshotId;
    beforeAll(function () { return __awaiter(_this, void 0, void 0, function () {
        var r, tokenAmount;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, Solo_1.getSolo()];
                case 1:
                    r = _a.sent();
                    solo = r.solo;
                    accounts = r.accounts;
                    admin = accounts[0];
                    marketMaker = accounts[6];
                    poker = accounts[9];
                    rando = accounts[8];
                    return [4 /*yield*/, EVM_1.resetEVM()];
                case 2:
                    _a.sent();
                    tokenAmount = new bignumber_js_1.default('1e19');
                    return [4 /*yield*/, Promise.all([
                            solo.oracle.daiPriceOracle.setPokerAddress(poker, { from: admin }),
                            solo.testing.tokenB.issueTo(tokenAmount, marketMaker),
                            solo.weth.wrap(marketMaker, tokenAmount),
                        ])];
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
    describe('saiPriceOracle', function () {
        it('Returns the correct address', function () { return __awaiter(_this, void 0, void 0, function () {
            var saiPriceOracleAddress;
            return __generator(this, function (_a) {
                saiPriceOracleAddress = Constants_1.ADDRESSES.TEST_SAI_PRICE_ORACLE;
                expect(solo.contracts.saiPriceOracle.options.address).toEqual(saiPriceOracleAddress);
                expect(solo.contracts.saiPriceOracle._address).toEqual(saiPriceOracleAddress);
                return [2 /*return*/];
            });
        }); });
    });
    describe('getPrice', function () {
        it('Returns the default value', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 1:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Can be set as the oracle for a market', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.admin.addMarket(solo.testing.tokenA.getAddress(), solo.contracts.daiPriceOracle.options.address, solo.contracts.testInterestSetter.options.address, Constants_1.INTEGERS.ZERO, Constants_1.INTEGERS.ZERO, { from: admin })];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.getters.getMarketPrice(Constants_1.INTEGERS.ZERO)];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Matches priceInfo', function () { return __awaiter(_this, void 0, void 0, function () {
            var priceInfo, price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('0.98'),
                            setUniswapPrice('0.97'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPriceInfo()];
                    case 3:
                        priceInfo = _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 4:
                        price = _a.sent();
                        expect(price).toEqual(priceInfo.price);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('ownerSetPriceOracle', function () {
        it('Fails for non-owner', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.oracle.daiPriceOracle.setPokerAddress(marketMaker, { from: rando }))];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds', function () { return __awaiter(_this, void 0, void 0, function () {
            var oldPoker, newPoker;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, solo.oracle.daiPriceOracle.getPoker()];
                    case 1:
                        oldPoker = _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.setPokerAddress(marketMaker, { from: admin })];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPoker()];
                    case 3:
                        newPoker = _a.sent();
                        expect(newPoker).toEqual(marketMaker);
                        expect(newPoker).not.toEqual(oldPoker);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('updatePrice', function () {
        it('Does not update for non-poker', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('0.99'),
                            setUniswapPrice('1.00'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(updatePrice(null, null, { from: rando }), 'DaiPriceOracle: Only poker can call updatePrice')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Updates timestamp correctly', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, timestamp, priceInfo;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('0.99'),
                            setUniswapPrice('1.00'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, solo.web3.eth.getBlock(txResult.blockNumber)];
                    case 3:
                        timestamp = (_a.sent()).timestamp;
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPriceInfo()];
                    case 4:
                        priceInfo = _a.sent();
                        expect(priceInfo.lastUpdate).toEqual(new bignumber_js_1.default(timestamp));
                        console.log("\tUpdate Dai Price gas used: " + txResult.gasUsed);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Emits an event', function () { return __awaiter(_this, void 0, void 0, function () {
            var txResult, priceInfo, log;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('0.98'),
                            setUniswapPrice('0.97'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 2:
                        txResult = _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPriceInfo()];
                    case 3:
                        priceInfo = _a.sent();
                        log = txResult.events.PriceSet;
                        expect(log).not.toBeUndefined();
                        expect(new bignumber_js_1.default(log.returnValues.newPriceInfo.price)).toEqual(priceInfo.price);
                        expect(new bignumber_js_1.default(log.returnValues.newPriceInfo.lastUpdate)).toEqual(priceInfo.lastUpdate);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Matches getBoundedTargetPrice', function () { return __awaiter(_this, void 0, void 0, function () {
            var boundedPrice, price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('1.05'),
                            setUniswapPrice('1.07'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.fastForward(1000)];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getBoundedTargetPrice()];
                    case 3:
                        boundedPrice = _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 5:
                        price = _a.sent();
                        expect(price).toEqual(boundedPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Will migrate to the right price over many updates', function () { return __awaiter(_this, void 0, void 0, function () {
            var price, targetPrice;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('1.025'),
                            setUniswapPrice('1.04'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getTargetPrice()];
                    case 2:
                        targetPrice = _a.sent();
                        return [4 /*yield*/, EVM_1.fastForward(1000)];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 4:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 5:
                        price = _a.sent();
                        expect(price).not.toEqual(targetPrice);
                        return [4 /*yield*/, EVM_1.fastForward(1000)];
                    case 6:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 7:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 8:
                        price = _a.sent();
                        expect(price).not.toEqual(targetPrice);
                        return [4 /*yield*/, EVM_1.fastForward(1000)];
                    case 9:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 10:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 11:
                        price = _a.sent();
                        expect(price).toEqual(targetPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails below minimum', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('0.97'),
                            setUniswapPrice('0.96'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(updatePrice(defaultPrice, null), 'DaiPriceOracle: newPrice below minimum')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Fails above maximum', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('1.02'),
                            setUniswapPrice('1.04'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Expect_1.expectThrow(updatePrice(null, defaultPrice), 'DaiPriceOracle: newPrice above maximum')];
                    case 2:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getTargetPrice', function () {
        it('Succeeds for price = dollar', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('1.01'),
                            setUniswapPrice('0.99'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getTargetPrice()];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice);
                        return [4 /*yield*/, Promise.all([
                                setCurvePrice('0.99'),
                                setUniswapPrice('1.01'),
                            ])];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getTargetPrice()];
                    case 4:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for price < dollar', function () { return __awaiter(_this, void 0, void 0, function () {
            var price, curvePrice;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('0.95'),
                            setUniswapPrice('0.98'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getTargetPrice()];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice.times('0.98'));
                        return [4 /*yield*/, setUniswapPrice('0.50')];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getCurvePrice()];
                    case 4:
                        curvePrice = _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getTargetPrice()];
                    case 5:
                        price = _a.sent();
                        expect(price).toEqual(curvePrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Succeeds for price > dollar', function () { return __awaiter(_this, void 0, void 0, function () {
            var price, curvePrice;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('1.04'),
                            setUniswapPrice('1.02'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getTargetPrice()];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice.times('1.02'));
                        return [4 /*yield*/, setUniswapPrice('2.00')];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getCurvePrice()];
                    case 4:
                        curvePrice = _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getTargetPrice()];
                    case 5:
                        price = _a.sent();
                        expect(price).toEqual(curvePrice);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getBoundedTargetPrice', function () {
        it('Upper-bounded by maximum absolute deviation', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, EVM_1.fastForward(1000)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Promise.all([
                                setCurvePrice('1.10'),
                                setUniswapPrice('1.10'),
                            ])];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getBoundedTargetPrice()];
                    case 3:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice.times('1.01'));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Lower-bounded by maximum absolute deviation', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, EVM_1.fastForward(1000)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, Promise.all([
                                setCurvePrice('0.90'),
                                setUniswapPrice('0.90'),
                            ])];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getBoundedTargetPrice()];
                    case 3:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice.times('0.99'));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Upper-bounded by maximum deviation per second', function () { return __awaiter(_this, void 0, void 0, function () {
            var price, boundedPrice;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('1.10'),
                            setUniswapPrice('1.10'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.mineAvgBlock()];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 4:
                        price = _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getBoundedTargetPrice()];
                    case 5:
                        boundedPrice = _a.sent();
                        expect(boundedPrice.gt(price)).toEqual(true);
                        expect(boundedPrice.lt(price.times('1.01'))).toEqual(true);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Lower-bounded by maximum deviation per second', function () { return __awaiter(_this, void 0, void 0, function () {
            var price, boundedPrice;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            setCurvePrice('0.90'),
                            setUniswapPrice('0.90'),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, updatePrice()];
                    case 2:
                        _a.sent();
                        return [4 /*yield*/, EVM_1.mineAvgBlock()];
                    case 3:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getPrice()];
                    case 4:
                        price = _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getBoundedTargetPrice()];
                    case 5:
                        boundedPrice = _a.sent();
                        expect(boundedPrice.lt(price)).toEqual(true);
                        expect(boundedPrice.gt(price.times('0.99'))).toEqual(true);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getCurvePrice', function () {
        it('Returns the price, adjusting for the fee', function () { return __awaiter(_this, void 0, void 0, function () {
            var price, expectedPrice;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setCurvePrice('1.05')];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getCurvePrice()];
                    case 2:
                        price = _a.sent();
                        expectedPrice = new bignumber_js_1.default('1.05').shiftedBy(18);
                        expect(price.div(expectedPrice).minus(1).abs().toNumber()).toBeLessThan(1e-10);
                        return [2 /*return*/];
                }
            });
        }); });
    });
    describe('getUniswapPrice', function () {
        it('Fails for zero liquidity', function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Expect_1.expectThrow(solo.oracle.daiPriceOracle.getUniswapPrice())];
                    case 1:
                        _a.sent();
                        return [2 /*return*/];
                }
            });
        }); });
        it('Gets the right price for ETH-DAI = ETH-USDC', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setUniswapPrice(1)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getUniswapPrice()];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
        it('Gets the right price for ETH-DAI > ETH-USDC', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setUniswapPrice(0.975)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getUniswapPrice()];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(new bignumber_js_1.default(0.975).shiftedBy(18));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Gets the right price for ETH-DAI < ETH-USDC', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, setUniswapPrice(1.025)];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getUniswapPrice()];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(new bignumber_js_1.default(1.025).shiftedBy(18));
                        return [2 /*return*/];
                }
            });
        }); });
        it('Does not overflow when the pools hold on the order of $100B in value', function () { return __awaiter(_this, void 0, void 0, function () {
            var price;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, Promise.all([
                            // Suppose ETH has a price of $1.
                            setUniswapEthDaiBalances(new bignumber_js_1.default(100e9).shiftedBy(18), // ethAmt
                            new bignumber_js_1.default(100e9).shiftedBy(18)),
                            setUniswapEthUsdcBalances(new bignumber_js_1.default(100e9).shiftedBy(18), // ethAmt
                            new bignumber_js_1.default(100e9).shiftedBy(6)),
                        ])];
                    case 1:
                        _a.sent();
                        return [4 /*yield*/, solo.oracle.daiPriceOracle.getUniswapPrice()];
                    case 2:
                        price = _a.sent();
                        expect(price).toEqual(defaultPrice);
                        return [2 /*return*/];
                }
            });
        }); });
    });
});
// ============ Helper Functions ============
function setCurvePrice(price) {
    return __awaiter(this, void 0, void 0, function () {
        var fee, dy, feeAmount, dyWithFee;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.contracts.call(solo.contracts.testCurve.methods.fee())];
                case 1:
                    fee = _a.sent();
                    dy = new bignumber_js_1.default(1).div(price);
                    feeAmount = dy.times(fee).div(CURVE_FEE_DENOMINATOR);
                    dyWithFee = dy.minus(feeAmount);
                    return [4 /*yield*/, solo.contracts.send(solo.contracts.testCurve.methods.setDy(
                        // Curve will treat dx and dy in terms of the base units of the currencies, so shift the value
                        // to be returned by the difference between the decimals of DAI and USDC.
                        dyWithFee.shiftedBy(DAI_DECIMALS - USDC_DECIMALS).toFixed(0)))];
                case 2:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function setUniswapPrice(price) {
    return __awaiter(this, void 0, void 0, function () {
        var ethPrice;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    ethPrice = new bignumber_js_1.default(100);
                    return [4 /*yield*/, Promise.all([
                            // Apply an arbitrary constant factor to the balances of each pool.
                            setUniswapEthDaiBalances(new bignumber_js_1.default(1).times(1.23).shiftedBy(18), // ethAmt
                            ethPrice.times(1.23).shiftedBy(18)),
                            setUniswapEthUsdcBalances(new bignumber_js_1.default(1).times(2.34).shiftedBy(18), // ethAmt
                            ethPrice.times(price).times(2.34).shiftedBy(6)),
                        ])];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function setUniswapEthDaiBalances(ethAmt, daiAmt) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.testUniswapV2Pair.methods.setReserves(new bignumber_js_1.default(daiAmt).toFixed(0), new bignumber_js_1.default(ethAmt).toFixed(0)))];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function setUniswapEthUsdcBalances(ethAmt, usdcAmt) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4 /*yield*/, solo.contracts.send(solo.contracts.testUniswapV2Pair2.methods.setReserves(new bignumber_js_1.default(usdcAmt).toFixed(0), new bignumber_js_1.default(ethAmt).toFixed(0)))];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
function updatePrice(minimum, maximum, options) {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            return [2 /*return*/, solo.oracle.daiPriceOracle.updatePrice(minimum, maximum, options || { from: poker })];
        });
    });
}
//# sourceMappingURL=DaiPriceOracle.test.js.map