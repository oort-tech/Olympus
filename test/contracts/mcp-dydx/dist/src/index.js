"use strict";
/*

    Copyright 2019 dYdX Trading Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var web3_1 = __importDefault(require("web3"));
exports.Web3 = web3_1.default;
var bignumber_js_1 = __importDefault(require("bignumber.js"));
exports.BigNumber = bignumber_js_1.default;
bignumber_js_1.default.config({
    EXPONENTIAL_AT: 1000,
    DECIMAL_PLACES: 80,
});
var Solo_1 = require("./Solo");
exports.Solo = Solo_1.Solo;
__export(require("./types"));
//# sourceMappingURL=index.js.map