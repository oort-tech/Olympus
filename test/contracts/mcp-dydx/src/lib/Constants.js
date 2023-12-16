"use strict";
exports.__esModule = true;
var bignumber_js_1 = require("bignumber.js");
exports.SUBTRACT_GAS_LIMIT = 100000;
var ONE_MINUTE_IN_SECONDS = new bignumber_js_1["default"](60);
var ONE_HOUR_IN_SECONDS = ONE_MINUTE_IN_SECONDS.times(60);
var ONE_DAY_IN_SECONDS = ONE_HOUR_IN_SECONDS.times(24);
var ONE_YEAR_IN_SECONDS = ONE_DAY_IN_SECONDS.times(365);
exports.INTEGERS = {
    ONE_MINUTE_IN_SECONDS: ONE_MINUTE_IN_SECONDS,
    ONE_HOUR_IN_SECONDS: ONE_HOUR_IN_SECONDS,
    ONE_DAY_IN_SECONDS: ONE_DAY_IN_SECONDS,
    ONE_YEAR_IN_SECONDS: ONE_YEAR_IN_SECONDS,
    ZERO: new bignumber_js_1["default"](0),
    ONE: new bignumber_js_1["default"](1),
    ONES_31: new bignumber_js_1["default"]('4294967295'),
    ONES_127: new bignumber_js_1["default"]('340282366920938463463374607431768211455'),
    ONES_255: new bignumber_js_1["default"]('115792089237316195423570985008687907853269984665640564039457584007913129639935'),
    INTEREST_RATE_BASE: new bignumber_js_1["default"]('1e18')
};
exports.ADDRESSES = {
    ZERO: '0x0000000000000000000000000000000000000000',
    TEST: [
        '0x06012c8cf97bead5deae237070f9587f8e7a266d',
        '0x22012c8cf97bead5deae237070f9587f8e7a266d',
        '0x33012c8cf97bead5deae237070f9587f8e7a266d',
        '0x44012c8cf97bead5deae237070f9587f8e7a266d',
        '0x55012c8cf97bead5deae237070f9587f8e7a266d',
        '0x66012c8cf97bead5deae237070f9587f8e7a266d',
        '0x77012c8cf97bead5deae237070f9587f8e7a266d',
        '0x88012c8cf97bead5deae237070f9587f8e7a266d',
        '0x99012c8cf97bead5deae237070f9587f8e7a266d',
        '0xaa012c8cf97bead5deae237070f9587f8e7a266d',
    ],
    TEST_SAI_PRICE_ORACLE: '0x1928347120834128940721983472825823453223'
};
