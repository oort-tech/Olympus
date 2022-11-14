"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ts_generator_1 = require("ts-generator");
const legacy_1 = require("./targets/legacy");
const truffle_1 = require("./targets/truffle");
const web3_1 = require("./targets/web3");
const ethers_1 = require("./targets/ethers");
/**
 * Proxies calls to real implementation that is selected based on target parameter.
 */
class Typechain extends ts_generator_1.TsGeneratorPlugin {
    constructor(ctx) {
        super(ctx);
        this.name = "Typechain";
        this.realImpl = this.findRealImpl(ctx);
    }
    findRealImpl(ctx) {
        switch (ctx.rawConfig.target) {
            case "legacy":
                return new legacy_1.TypechainLegacy(ctx);
            case "truffle":
                return new truffle_1.Truffle(ctx);
            case "web3-1.0.0":
                return new web3_1.Web3(ctx);
            case "ethers":
                return new ethers_1.Ethers(ctx);
            case undefined:
                throw new Error(`Please provide --target parameter!`);
            default:
                throw new Error(`Unsupported target ${this.ctx.rawConfig.target}!`);
        }
    }
    beforeRun() {
        return this.realImpl.beforeRun();
    }
    transformFile(file) {
        return this.realImpl.transformFile(file);
    }
    afterRun() {
        return this.realImpl.afterRun();
    }
}
exports.Typechain = Typechain;
