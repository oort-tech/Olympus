import { TsGeneratorPlugin, TFileDesc, TContext } from "ts-generator";
export interface ITypechainCfg {
    outDir?: string;
}
export declare class TypechainLegacy extends TsGeneratorPlugin {
    name: string;
    private readonly outDirAbs?;
    private runtimePathAbs?;
    constructor(ctx: TContext<ITypechainCfg>);
    transformFile(file: TFileDesc): TFileDesc | void;
    afterRun(): TFileDesc | void;
}
