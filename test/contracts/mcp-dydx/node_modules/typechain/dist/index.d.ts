import { TsGeneratorPlugin, TFileDesc, TContext, TOutput } from "ts-generator";
export declare type TTypechainTarget = "truffle" | "web3-1.0.0" | "legacy" | "ethers";
export interface ITypechainCfg {
    target: TTypechainTarget;
    outDir?: string;
}
/**
 * Proxies calls to real implementation that is selected based on target parameter.
 */
export declare class Typechain extends TsGeneratorPlugin {
    name: string;
    private realImpl;
    constructor(ctx: TContext<ITypechainCfg>);
    private findRealImpl;
    beforeRun(): TOutput | Promise<TOutput>;
    transformFile(file: TFileDesc): TOutput | Promise<TOutput>;
    afterRun(): TOutput | Promise<TOutput>;
}
