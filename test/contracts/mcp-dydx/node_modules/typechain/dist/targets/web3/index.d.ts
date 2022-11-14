import { TsGeneratorPlugin, TContext, TFileDesc } from "ts-generator";
export interface IWeb3Cfg {
    outDir?: string;
}
export declare class Web3 extends TsGeneratorPlugin {
    name: string;
    private readonly outDirAbs;
    constructor(ctx: TContext<IWeb3Cfg>);
    transformFile(file: TFileDesc): TFileDesc | void;
}
