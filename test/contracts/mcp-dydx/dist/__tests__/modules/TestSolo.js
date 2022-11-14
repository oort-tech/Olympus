"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Solo_1 = require("../../src/Solo");
var TestContracts_1 = require("./TestContracts");
var Testing_1 = require("./Testing");
var TestSolo = /** @class */ (function (_super) {
    __extends(TestSolo, _super);
    function TestSolo(provider, networkId, options) {
        if (options === void 0) { options = {}; }
        var _this = _super.call(this, provider, networkId, options) || this;
        _this.testing = new Testing_1.Testing(provider, _this.contracts, _this.token);
        return _this;
    }
    TestSolo.prototype.setProvider = function (provider, networkId) {
        _super.prototype.setProvider.call(this, provider, networkId);
        this.testing.setProvider(provider);
    };
    TestSolo.prototype.createContractsModule = function (provider, networkId, web3, options) {
        return new TestContracts_1.TestContracts(provider, networkId, web3, options);
    };
    return TestSolo;
}(Solo_1.Solo));
exports.TestSolo = TestSolo;
//# sourceMappingURL=TestSolo.js.map