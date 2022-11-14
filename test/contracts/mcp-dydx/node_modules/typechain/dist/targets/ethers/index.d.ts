import { TsGeneratorPlugin, TContext, TFileDesc } from "ts-generator";
export interface IEthersCfg {
    outDir?: string;
}
export declare class Ethers extends TsGeneratorPlugin {
    name: string;
    private readonly outDirAbs;
    constructor(ctx: TContext<IEthersCfg>);
    transformFile(file: TFileDesc): TFileDesc | void;
    afterRun(): TFileDesc[];
}
