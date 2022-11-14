"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const typeParser_1 = require("../../parser/typeParser");
function codegen(contracts) {
    const template = `
/// <reference types="truffle-typings" />
import { BigNumber } from "bignumber.js";

${contracts.map(generateContractInterface).join("\n")}

${contracts.map(generateContractInstanceInterface).join("\n")}
  `;
    return template;
}
exports.codegen = codegen;
function generateArtifactHeaders(contracts) {
    return `
  /// <reference types="truffle-typings" />

  import * as TruffleContracts from ".";
  
  declare global {
    namespace Truffle {
      interface Artifacts {
        ${contracts
        .map(f => `require(name: "${f.name}"): TruffleContracts.${f.name}Contract;`)
        .join("\n")}
      }
    }
  }  
  `;
}
exports.generateArtifactHeaders = generateArtifactHeaders;
function generateContractInterface(c) {
    return `
export interface ${c.name}Contract extends Truffle.Contract<${c.name}Instance> {
  ${c.constructor
        ? `"new"(${generateInputTypes(c.constructor.inputs)} meta?: Truffle.TransactionDetails): Promise<${c.name}Instance>;`
        : `"new"(meta?: Truffle.TransactionDetails): Promise<${c.name}Instance>;`}
}
`;
}
function generateContractInstanceInterface(c) {
    return `
export interface ${c.name}Instance extends Truffle.ContractInstance {
  ${c.constantFunctions.map(generateConstantFunction).join("\n")}
  ${c.functions.map(generateFunction).join("\n")}
  ${c.constants.map(generateConstants).join("\n")}
}
  `;
}
function generateFunction(fn) {
    return `
  ${fn.name}(${generateInputTypes(fn.inputs)} txDetails?: Truffle.TransactionDetails): Promise<Truffle.TransactionResponse>;
`;
}
function generateConstantFunction(fn) {
    return `
  ${fn.name}(${generateInputTypes(fn.inputs)} txDetails?: Truffle.TransactionDetails): Promise<${generateOutputTypes(fn.outputs)}>;
`;
}
function generateConstants(fn) {
    return `${fn.name}(txDetails?: Truffle.TransactionDetails): Promise<${generateOutputType(fn.output)}>;`;
}
function generateInputTypes(input) {
    if (input.length === 0) {
        return "";
    }
    return (input
        .map((input, index) => `${input.name || `arg${index}`}: ${generateInputType(input.type)}`)
        .join(", ") + ", ");
}
function generateOutputTypes(outputs) {
    if (outputs.length === 1) {
        return generateOutputType(outputs[0].type);
    }
    else {
        return `[${outputs.map(param => generateOutputType(param.type)).join(", ")}]`;
    }
}
function generateInputType(evmType) {
    switch (evmType.constructor) {
        case typeParser_1.IntegerType:
            return "number | BigNumber | string";
        case typeParser_1.UnsignedIntegerType:
            return "number | BigNumber | string";
        case typeParser_1.AddressType:
            return "string | BigNumber";
        case typeParser_1.BytesType:
            return "string | BigNumber";
        case typeParser_1.ArrayType:
            return `(${generateInputType(evmType.itemType)})[]`;
        case typeParser_1.BooleanType:
            return "boolean";
        case typeParser_1.StringType:
            return "string";
        case typeParser_1.TupleType:
            return generateTupleType(evmType, generateInputType);
        default:
            throw new Error(`Unrecognized type ${evmType}`);
    }
}
function generateOutputType(evmType) {
    switch (evmType.constructor) {
        case typeParser_1.IntegerType:
            return "BigNumber";
        case typeParser_1.UnsignedIntegerType:
            return "BigNumber";
        case typeParser_1.AddressType:
            return "string";
        case typeParser_1.VoidType:
            return "void";
        case typeParser_1.BytesType:
            return "string";
        case typeParser_1.ArrayType:
            return `(${generateOutputType(evmType.itemType)})[]`;
        case typeParser_1.BooleanType:
            return "boolean";
        case typeParser_1.StringType:
            return "string";
        case typeParser_1.TupleType:
            return generateTupleType(evmType, generateOutputType);
        default:
            throw new Error(`Unrecognized type ${evmType}`);
    }
}
function generateTupleType(tuple, generator) {
    return ("{" +
        tuple.components
            .map(component => `${component.name}: ${generator(component.type)}`)
            .join(", ") +
        "}");
}
