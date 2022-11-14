"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ts_generator_1 = require("ts-generator");
const path_1 = require("path");
const abiParser_1 = require("../../parser/abiParser");
const shared_1 = require("../shared");
const generation_1 = require("./generation");
const DEFAULT_OUT_PATH = "./types/ethers-contracts/";
class Ethers extends ts_generator_1.TsGeneratorPlugin {
    constructor(ctx) {
        super(ctx);
        this.name = "Ethers";
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
    afterRun() {
        return [
            {
                path: path_1.join(this.outDirAbs, "index.d.ts"),
                contents: `
        import { BigNumberish } from "ethers/utils";

        export class TransactionOverrides {
          nonce?: BigNumberish | Promise<BigNumberish>;
          gasLimit?: BigNumberish | Promise<BigNumberish>;
          gasPrice?: BigNumberish | Promise<BigNumberish>;
          value?: BigNumberish | Promise<BigNumberish>;
          chainId?: number | Promise<number>;
        }`,
            },
        ];
    }
}
exports.Ethers = Ethers;
