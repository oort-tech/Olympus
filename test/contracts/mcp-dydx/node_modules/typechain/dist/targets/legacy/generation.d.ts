import { RawAbiDefinition } from "../../parser/abiParser";
import { IContext } from "../shared";
export declare function getRuntime(): string;
export declare function codegen(abi: Array<RawAbiDefinition>, context: IContext): string;
