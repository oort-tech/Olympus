"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var web3_1 = __importDefault(require("web3"));
exports.provider = new web3_1.default.providers.HttpProvider("http://localhost:8545");
//# sourceMappingURL=Provider.js.map