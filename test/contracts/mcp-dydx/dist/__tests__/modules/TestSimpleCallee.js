"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var TestSimpleCallee = /** @class */ (function () {
    function TestSimpleCallee(contracts) {
        this.contracts = contracts;
    }
    TestSimpleCallee.prototype.getAddress = function () {
        return this.contracts.testSimpleCallee.options.address;
    };
    return TestSimpleCallee;
}());
exports.TestSimpleCallee = TestSimpleCallee;
//# sourceMappingURL=TestSimpleCallee.js.map