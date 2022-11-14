"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const abiParser_1 = require("../../parser/abiParser");
const typeParser_1 = require("../../parser/typeParser");
const path_1 = require("path");
const fs_1 = require("fs");
function getRuntime() {
    const runtimePath = path_1.join(__dirname, "./runtime/typechain-runtime.ts");
    return fs_1.readFileSync(runtimePath, "utf8");
}
exports.getRuntime = getRuntime;
function codegen(abi, context) {
    const parsedContractAbi = abiParser_1.parse(abi, context.fileName);
    return codeGenForContract(abi, parsedContractAbi, context);
}
exports.codegen = codegen;
function codeGenForContract(abi, input, context) {
    const runtimeNamespace = "TC";
    const typeName = context.fileName;
    return `
    import { BigNumber } from "bignumber.js";
    import * as ${runtimeNamespace} from "${context.relativeRuntimePath}"

    export class ${typeName} extends ${runtimeNamespace}.TypeChainContract {
      public readonly rawWeb3Contract: any;

      public constructor(web3: any, address: string | BigNumber) {
        const abi = ${JSON.stringify(abi)};
        super(web3, address, abi);
      }

      static async createAndValidate(web3: any, address: string | BigNumber): Promise<${typeName}> {
        const contract = new ${typeName}(web3, address);
        const code = await ${runtimeNamespace}.promisify(web3.eth.getCode, [address]);

        // in case of missing smartcontract, code can be equal to "0x0" or "0x" depending on exact web3 implementation
        // to cover all these cases we just check against the source code length — there won't be any meaningful EVM program in less then 3 chars
        if (code.length < 4) {
          throw new Error(\`Contract at \${address} doesn't exist!\`);
        }
        return contract; 
      }
      
      ${codeGenForConstants(runtimeNamespace, input.constants)}
      
      ${codeGenForConstantsFunctions(runtimeNamespace, input.constantFunctions)}
      
      ${codeGenForFunctions(runtimeNamespace, input.functions)}
      
      ${codeGenForEvents(runtimeNamespace, input.events)}
    }
`;
}
function codeGenForConstants(runtimeNamespace, constants) {
    return constants
        .map(({ name, output }) => `
        public get ${name}(): Promise<${codeGenForOutput(output)}> { 
            return ${runtimeNamespace}.promisify(this.rawWeb3Contract.${name}, []); 
        }
    `)
        .join("\n");
}
function codeGenForConstantsFunctions(runtimeNamespace, constantFunctions) {
    return constantFunctions
        .map(({ inputs, name, outputs }) => `
        public ${name}(${inputs
        .map(codeGenForParams)
        .join(", ")}): Promise<${codeGenForOutputTypeList(outputs)}> { 
            return ${runtimeNamespace}.promisify(this.rawWeb3Contract.${name}, [${inputs
        .map(codeGenForArgs)
        .join(", ")}]); 
        }
   `)
        .join("\n");
}
function codeGenForFunctions(runtimeNamespace, functions) {
    return functions
        .map(({ payable, name, inputs }) => {
        const txParamsType = payable
            ? `${runtimeNamespace}.IPayableTxParams`
            : `${runtimeNamespace}.ITxParams`;
        return `public ${name}Tx(${inputs
            .map(codeGenForParams)
            .join(", ")}): ${runtimeNamespace}.DeferredTransactionWrapper<${txParamsType}> { return new ${runtimeNamespace}.DeferredTransactionWrapper<${txParamsType}>(this, "${name}", [${inputs
            .map(codeGenForArgs)
            .join(", ")}]);
                }`;
    })
        .join("\n");
}
function codeGenForEvents(runtimeNamespace, events) {
    return events
        .map(event => {
        const filterableEventParams = codeGenForEventArgs(event.inputs, true);
        const eventParams = codeGenForEventArgs(event.inputs, false);
        return `public ${event.name}Event(eventFilter: ${filterableEventParams}): ${runtimeNamespace}.DeferredEventWrapper<${eventParams}, ${filterableEventParams}> {
                return new ${runtimeNamespace}.DeferredEventWrapper<${eventParams}, ${filterableEventParams}>(this, '${event.name}', eventFilter);
              }`;
    })
        .join("\n");
}
function codeGenForParams(param, index) {
    return `${param.name || `arg${index}`}: ${codeGenForInput(param.type)}`;
}
function codeGenForArgs(param, index) {
    const paramName = param.name || `arg${index}`;
    if (param.type instanceof typeParser_1.ArrayType) {
        const elemParam = { name: `${paramName}Elem`, type: param.type.itemType };
        return `${paramName}.map(${elemParam.name} => ${codeGenForArgs(elemParam, 0)})`;
    }
    if (param.type instanceof typeParser_1.BooleanType)
        return paramName;
    return `${paramName}.toString()`;
}
function codeGenForOutputTypeList(output) {
    if (output.length === 1) {
        return codeGenForOutput(output[0].type);
    }
    else {
        return `[${output.map(x => codeGenForOutput(x.type)).join(", ")}]`;
    }
}
function codeGenForEventArgs(args, onlyIndexed) {
    return `{${args
        .filter(arg => arg.isIndexed || !onlyIndexed)
        .map(arg => {
        const inputCodegen = codeGenForInput(arg.type);
        // if we're specifying a filter, you can take a single value or an array of values to check for
        const argType = `${inputCodegen}${onlyIndexed ? ` | Array<${inputCodegen}>` : ""}`;
        return `${arg.name}${onlyIndexed ? "?" : ""}: ${argType}`;
    })
        .join(`, `)}}`;
}
function codeGenForInput(evmType) {
    switch (evmType.constructor) {
        case typeParser_1.IntegerType:
            return "BigNumber | number";
        case typeParser_1.UnsignedIntegerType:
            return "BigNumber | number";
        case typeParser_1.AddressType:
            return "BigNumber | string";
        case typeParser_1.TupleType:
            return generateTupleType(evmType, codeGenForInput);
        default:
            return codeGenForOutput(evmType);
    }
}
function codeGenForOutput(evmType) {
    switch (evmType.constructor) {
        case typeParser_1.BooleanType:
            return "boolean";
        case typeParser_1.IntegerType:
            return "BigNumber";
        case typeParser_1.UnsignedIntegerType:
            return "BigNumber";
        case typeParser_1.VoidType:
            return "void";
        case typeParser_1.StringType:
            return "string";
        case typeParser_1.BytesType:
            return "string";
        case typeParser_1.AddressType:
            return "string";
        case typeParser_1.ArrayType:
            return codeGenForOutput(evmType.itemType) + "[]";
        case typeParser_1.TupleType:
            return generateTupleType(evmType, codeGenForOutput);
        default:
            throw new Error(`Unrecognized ABI piece: ${evmType.constructor}`);
    }
}
function generateTupleType(tuple, generator) {
    return ("{" +
        tuple.components
            .map(component => `${component.name}: ${generator(component.type)}`)
            .join(", ") +
        "}");
}
