"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const debug_1 = require("../utils/debug");
const typeParser_1 = require("./typeParser");
const errors_1 = require("../utils/errors");
const logger_1 = require("../utils/logger");
function parse(abi, name) {
    const constants = [];
    const constantFunctions = [];
    const functions = [];
    const events = [];
    let constructor = undefined;
    abi.forEach(abiPiece => {
        // @todo implement missing abi pieces
        // skip fallback functions
        if (abiPiece.type === "fallback") {
            return;
        }
        if (abiPiece.type === "constructor") {
            constructor = parseConstructor(abiPiece);
            return;
        }
        if (abiPiece.type === "function") {
            if (checkForOverloads(constants, constantFunctions, functions, abiPiece.name)) {
                logger_1.logger.log(`Detected overloaded constant function ${abiPiece.name} skipping...`);
                return;
            }
            if (abiPiece.constant && abiPiece.inputs.length === 0 && abiPiece.outputs.length === 1) {
                constants.push(parseConstant(abiPiece));
            }
            else if (abiPiece.constant) {
                constantFunctions.push(parseConstantFunction(abiPiece));
            }
            else {
                functions.push(parseFunctionDeclaration(abiPiece));
            }
            return;
        }
        if (abiPiece.type === "event") {
            const eventAbi = abiPiece;
            if (eventAbi.anonymous) {
                logger_1.logger.log("Skipping anonymous event...");
                return;
            }
            events.push(parseEvent(eventAbi));
            return;
        }
        throw new Error(`Unrecognized abi element: ${abiPiece.type}`);
    });
    return {
        name,
        constructor: constructor,
        constants,
        constantFunctions,
        functions,
        events,
    };
}
exports.parse = parse;
function checkForOverloads(constants, constantFunctions, functions, name) {
    return (constantFunctions.find(f => f.name === name) ||
        constants.find(f => f.name === name) ||
        functions.find(f => f.name === name));
}
function parseOutputs(outputs) {
    if (outputs.length === 0) {
        return [{ name: "", type: new typeParser_1.VoidType() }];
    }
    else {
        return outputs.map(parseRawAbiParameter);
    }
}
function parseConstant(abiPiece) {
    debug_1.default(`Parsing constant "${abiPiece.name}"`);
    return {
        name: abiPiece.name,
        output: parseRawAbiParameterType(abiPiece.outputs[0]),
    };
}
function parseEvent(abiPiece) {
    debug_1.default(`Parsing event "${abiPiece.name}"`);
    return {
        name: abiPiece.name,
        inputs: abiPiece.inputs.map(parseRawEventArg),
    };
}
exports.parseEvent = parseEvent;
function parseRawEventArg(eventArg) {
    return {
        name: eventArg.name,
        isIndexed: eventArg.indexed,
        type: parseRawAbiParameterType(eventArg),
    };
}
function parseConstantFunction(abiPiece) {
    debug_1.default(`Parsing constant function "${abiPiece.name}"`);
    return {
        name: abiPiece.name,
        inputs: abiPiece.inputs.map(parseRawAbiParameter),
        outputs: parseOutputs(abiPiece.outputs),
    };
}
function parseConstructor(abiPiece) {
    debug_1.default(`Parsing constructor declaration`);
    return {
        inputs: abiPiece.inputs.map(parseRawAbiParameter),
        payable: abiPiece.payable,
    };
}
function parseFunctionDeclaration(abiPiece) {
    debug_1.default(`Parsing function declaration "${abiPiece.name}"`);
    return {
        name: abiPiece.name,
        inputs: abiPiece.inputs.map(parseRawAbiParameter),
        outputs: parseOutputs(abiPiece.outputs),
        payable: abiPiece.payable,
    };
}
function parseRawAbiParameter(rawAbiParameter) {
    return {
        name: rawAbiParameter.name,
        type: parseRawAbiParameterType(rawAbiParameter),
    };
}
function parseRawAbiParameterType(rawAbiParameter) {
    const components = rawAbiParameter.components &&
        rawAbiParameter.components.map(component => new typeParser_1.EvmTypeComponent(component.name, parseRawAbiParameterType(component)));
    return typeParser_1.parseEvmType(rawAbiParameter.type, components);
}
function extractAbi(rawJson) {
    let json;
    try {
        json = JSON.parse(rawJson);
    }
    catch (_a) {
        throw new errors_1.MalformedAbiError("Not a json");
    }
    if (!json) {
        throw new errors_1.MalformedAbiError("Not a json");
    }
    if (Array.isArray(json)) {
        return json;
    }
    if (Array.isArray(json.abi)) {
        return json.abi;
    }
    throw new errors_1.MalformedAbiError("Not a valid ABI");
}
exports.extractAbi = extractAbi;
