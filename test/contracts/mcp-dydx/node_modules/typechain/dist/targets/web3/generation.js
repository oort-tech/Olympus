"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const typeParser_1 = require("../../parser/typeParser");
function codegen(contract) {
    const template = `
  import Contract, { CustomOptions, contractOptions } from "web3/eth/contract";
  import { TransactionObject, BlockType } from "web3/eth/types";
  import { Callback, EventLog } from "web3/types";
  import { EventEmitter } from "events";
  import { Provider } from "web3/providers";

  export class ${contract.name} {
    constructor(
        jsonInterface: any[],
        address?: string,
        options?: CustomOptions
    );
    _address: string;
    options: contractOptions;
    methods: {
      ${contract.constantFunctions.map(generateFunction).join("\n")}
      ${contract.functions.map(generateFunction).join("\n")}
      ${contract.constants.map(generateConstants).join("\n")}
    };
    deploy(options: {
        data: string;
        arguments: any[];
    }): TransactionObject<Contract>;
    events: {
      ${contract.events.map(generateEvents).join("\n")}
      allEvents: (
          options?: {
              filter?: object;
              fromBlock?: BlockType;
              topics?: (null|string)[];
          },
          cb?: Callback<EventLog>
      ) => EventEmitter;
    };
    getPastEvents(
        event: string,
        options?: {
            filter?: object;
            fromBlock?: BlockType;
            toBlock?: BlockType;
            topics?: (null|string)[];
        },
        cb?: Callback<EventLog[]>
    ): Promise<EventLog[]>;
    setProvider(provider: Provider): void;
    clone(): ${contract.name};
}
  `;
    return template;
}
exports.codegen = codegen;
function generateFunction(fn) {
    return `
  ${fn.name}(${generateInputTypes(fn.inputs)}): TransactionObject<${generateOutputTypes(fn.outputs)}>;
`;
}
function generateConstants(fn) {
    return `${fn.name}(): TransactionObject<${generateOutputType(fn.output)}>;`;
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
  ${event.name}(
    options?: {
        filter?: object;
        fromBlock?: BlockType;
        topics?: (null|string)[];
    },
    cb?: Callback<EventLog>): EventEmitter;
  `;
}
function generateInputType(evmType) {
    switch (evmType.constructor) {
        case typeParser_1.IntegerType:
            return "number | string";
        case typeParser_1.UnsignedIntegerType:
            return "number | string";
        case typeParser_1.AddressType:
            return "string";
        case typeParser_1.BytesType:
            return "string | number[]";
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
            return "string";
        case typeParser_1.UnsignedIntegerType:
            return "string";
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
