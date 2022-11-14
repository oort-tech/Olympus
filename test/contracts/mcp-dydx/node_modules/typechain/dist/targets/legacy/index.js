"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ts_generator_1 = require("ts-generator");
const path_1 = require("path");
const generation_1 = require("./generation");
const abiParser_1 = require("../../parser/abiParser");
const shared_1 = require("../shared");
class TypechainLegacy extends ts_generator_1.TsGeneratorPlugin {
    constructor(ctx) {
        super(ctx);
        this.name = "Typechain-legacy";
        const { cwd, rawConfig } = ctx;
        if (rawConfig.outDir) {
            this.outDirAbs = path_1.join(cwd, rawConfig.outDir);
            this.runtimePathAbs = buildOutputRuntimePath(this.outDirAbs);
        }
    }
    transformFile(file) {
        const fileDirPath = path_1.dirname(file.path);
        if (!this.runtimePathAbs) {
            this.runtimePathAbs = buildOutputRuntimePath(fileDirPath);
        }
        const outDir = this.outDirAbs || fileDirPath;
        const fileName = shared_1.getFilename(file.path);
        const outputFilePath = path_1.join(outDir, `${fileName}.ts`);
        const relativeRuntimePath = ts_generator_1.getRelativeModulePath(outputFilePath, this.runtimePathAbs);
        const abi = abiParser_1.extractAbi(file.contents);
        if (abi.length === 0) {
            return;
        }
        const wrapperContents = generation_1.codegen(abi, { fileName, relativeRuntimePath });
        return {
            path: outputFilePath,
            contents: wrapperContents,
        };
    }
    afterRun() {
        if (this.runtimePathAbs) {
            return {
                path: this.runtimePathAbs,
                contents: generation_1.getRuntime(),
            };
        }
    }
}
exports.TypechainLegacy = TypechainLegacy;
function buildOutputRuntimePath(dirPath) {
    return path_1.join(dirPath, "typechain-runtime.ts");
}
