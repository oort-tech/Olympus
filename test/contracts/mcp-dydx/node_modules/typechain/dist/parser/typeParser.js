"use strict";
/**
 * This will be refactored in the near future. Parser should produce discriminating union types.
 */
Object.defineProperty(exports, "__esModule", { value: true });
class EvmType {
}
exports.EvmType = EvmType;
class BooleanType extends EvmType {
}
exports.BooleanType = BooleanType;
class IntegerType extends EvmType {
    constructor(bits) {
        super();
        this.bits = bits;
    }
}
exports.IntegerType = IntegerType;
class UnsignedIntegerType extends EvmType {
    constructor(bits) {
        super();
        this.bits = bits;
    }
}
exports.UnsignedIntegerType = UnsignedIntegerType;
class VoidType extends EvmType {
}
exports.VoidType = VoidType;
class StringType extends EvmType {
}
exports.StringType = StringType;
class BytesType extends EvmType {
    constructor(size) {
        super();
        this.size = size;
    }
}
exports.BytesType = BytesType;
class AddressType extends EvmType {
}
exports.AddressType = AddressType;
class ArrayType extends EvmType {
    constructor(itemType, size) {
        super();
        this.itemType = itemType;
        this.size = size;
    }
}
exports.ArrayType = ArrayType;
class EvmTypeComponent {
    constructor(name, type) {
        this.name = name;
        this.type = type;
    }
}
exports.EvmTypeComponent = EvmTypeComponent;
class TupleType extends EvmType {
    constructor(components) {
        super();
        this.components = components;
    }
}
exports.TupleType = TupleType;
const isUIntTypeRegex = /^uint([0-9]*)$/;
const isIntTypeRegex = /^int([0-9]*)$/;
const isBytesTypeRegex = /^bytes([0-9]+)$/;
function parseEvmType(rawType, components) {
    const lastChar = rawType[rawType.length - 1];
    if (lastChar === "]") {
        // we parse array type
        let finishArrayTypeIndex = rawType.length - 2;
        while (rawType[finishArrayTypeIndex] !== "[") {
            finishArrayTypeIndex--;
        }
        const arraySizeRaw = rawType.slice(finishArrayTypeIndex + 1, rawType.length - 1);
        const arraySize = arraySizeRaw !== "" ? parseInt(arraySizeRaw) : undefined;
        const restOfTheType = rawType.slice(0, finishArrayTypeIndex);
        return new ArrayType(parseEvmType(restOfTheType, components), arraySize);
    }
    // this has to be primitive type
    //first deal with simple types
    switch (rawType) {
        case "bool":
            return new BooleanType();
        case "address":
            return new AddressType();
        case "string":
            return new StringType();
        case "byte":
            return new BytesType(1);
        case "bytes":
            return new ArrayType(new BytesType(1));
        case "tuple":
            if (!components)
                throw new Error("Tuple specified without components!");
            return new TupleType(components);
    }
    if (isUIntTypeRegex.test(rawType)) {
        const match = isUIntTypeRegex.exec(rawType);
        return new UnsignedIntegerType(parseInt(match[1] || "256"));
    }
    if (isIntTypeRegex.test(rawType)) {
        const match = isIntTypeRegex.exec(rawType);
        return new IntegerType(parseInt(match[1] || "256"));
    }
    if (isBytesTypeRegex.test(rawType)) {
        const match = isBytesTypeRegex.exec(rawType);
        return new BytesType(parseInt(match[1] || "1"));
    }
    throw new Error("Unknown type: " + rawType);
}
exports.parseEvmType = parseEvmType;
