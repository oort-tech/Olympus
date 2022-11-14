import { TTypechainTarget } from "..";
export interface IOptions {
    files: string;
    target: TTypechainTarget;
    outDir?: string;
}
export declare function parseArgs(): IOptions;
