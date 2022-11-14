"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var TestExchangeWrapper = /** @class */ (function () {
    function TestExchangeWrapper(contracts) {
        this.contracts = contracts;
    }
    TestExchangeWrapper.prototype.getAddress = function () {
        return this.contracts.testExchangeWrapper.options.address;
    };
    TestExchangeWrapper.prototype.getExchangeAddress = function () {
        return '0x0000000000000000000000000000000000000001';
    };
    return TestExchangeWrapper;
}());
exports.TestExchangeWrapper = TestExchangeWrapper;
//# sourceMappingURL=TestExchangeWrapper.js.map