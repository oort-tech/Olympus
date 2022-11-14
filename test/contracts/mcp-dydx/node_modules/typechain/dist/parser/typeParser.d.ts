/**
 * This will be refactored in the near future. Parser should produce discriminating union types.
 */
export declare abstract class EvmType {
}
export declare class BooleanType extends EvmType {
}
export declare class IntegerType extends EvmType {
    readonly bits: number;
    constructor(bits: number);
}
export declare class UnsignedIntegerType extends EvmType {
    readonly bits: number;
    constructor(bits: number);
}
export declare class VoidType extends EvmType {
}
export declare class StringType extends EvmType {
}
export declare class BytesType extends EvmType {
    readonly size: number;
    constructor(size: number);
}
export declare class AddressType extends EvmType {
}
export declare class ArrayType extends EvmType {
    readonly itemType: EvmType;
    readonly size?: number | undefined;
    constructor(itemType: EvmType, size?: number | undefined);
}
export declare class EvmTypeComponent {
    readonly name: string;
    readonly type: EvmType;
    constructor(name: string, type: EvmType);
}
export declare class TupleType extends EvmType {
    readonly components: EvmTypeComponent[];
    constructor(components: EvmTypeComponent[]);
}
export declare function parseEvmType(rawType: string, components?: EvmTypeComponent[]): EvmType;
