"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const typeParser_1 = require("../../parser/typeParser");
function codegen(contract) {
    const template = `
  import { Contract, ContractTransaction, EventFilter } from 'ethers';
  import { Provider } from 'ethers/providers';
  import { BigNumber } from "ethers/utils";
  import { TransactionOverrides } from ".";

  export class ${contract.name} extends Contract {
    functions: {
      ${contract.constantFunctions.map(generateConstantFunction).join("\n")}
      ${contract.functions.map(generateFunction).join("\n")}
      ${contract.constants.map(generateConstant).join("\n")}
    };
    filters: {
      ${contract.events.map(generateEvents).join("\n")}
    }
}
  `;
    return template;
}
exports.codegen = codegen;
function generateConstantFunction(fn) {
    return `
  ${fn.name}(${generateInputTypes(fn.inputs)}): Promise<${generateOutputTypes(fn.outputs)}>;
`;
}
function generateFunction(fn) {
    return `
  ${fn.name}(${generateInputTypes(fn.inputs)}overrides?: TransactionOverrides): Promise<ContractTransaction>;
`;
}
function generateConstant(fn) {
    return `${fn.name}(): Promise<${generateOutputType(fn.output)}>;`;
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
        return `{ 
      ${outputs.map(t => t.name && `${t.name}: ${generateOutputType(t.type)}, `).join("")}
      ${outputs.map((t, i) => `${i}: ${generateOutputType(t.type)}`).join(", ")}
      }`;
    }
}
function generateEvents(event) {
    return `
  ${event.name}(${generateEventTypes(event.inputs)}): EventFilter;
`;
}
function generateEventTypes(eventArg) {
    if (eventArg.length === 0) {
        return "";
    }
    return (eventArg
        .map(arg => {
        const eventType = arg.isIndexed ? `${generateInputType(arg.type)} | null` : "null";
        return `${arg.name}: ${eventType}`;
    })
        .join(", ") + ", ");
}
function generateInputType(evmType) {
    switch (evmType.constructor) {
        case typeParser_1.IntegerType:
            return "number | string | BigNumber";
        case typeParser_1.UnsignedIntegerType:
            return "number | string | BigNumber";
        case typeParser_1.AddressType:
            return "string";
        case typeParser_1.BytesType:
            return "string";
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
            return "object[]";
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
