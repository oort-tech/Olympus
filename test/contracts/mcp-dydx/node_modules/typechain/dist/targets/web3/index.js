"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ts_generator_1 = require("ts-generator");
const path_1 = require("path");
const abiParser_1 = require("../../parser/abiParser");
const shared_1 = require("../shared");
const generation_1 = require("./generation");
const DEFAULT_OUT_PATH = "./types/web3-contracts/";
class Web3 extends ts_generator_1.TsGeneratorPlugin {
    constructor(ctx) {
        super(ctx);
        this.name = "Web3";
        const { cwd, rawConfig } = ctx;
        this.outDirAbs = path_1.join(cwd, rawConfig.outDir || DEFAULT_OUT_PATH);
    }
    transformFile(file) {
        const abi = abiParser_1.extractAbi(file.contents);
        const isEmptyAbi = abi.length === 0;
        if (isEmptyAbi) {
            return;
        }
        const name = shared_1.getFilename(file.path);
        const contract = abiParser_1.parse(abi, name);
        return {
            path: path_1.join(this.outDirAbs, `${name}.d.ts`),
            contents: generation_1.codegen(contract),
        };
    }
}
exports.Web3 = Web3;
