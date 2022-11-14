import { address } from '../../src/types';
export declare enum SIGNATURE_TYPES {
    NO_PREPEND = 0,
    DECIMAL = 1,
    HEXADECIMAL = 2
}
export declare const PREPEND_DEC: string;
export declare const PREPEND_HEX: string;
export declare const EIP712_DOMAIN_STRING: string;
export declare const EIP712_DOMAIN_STRUCT: {
    name: string;
    type: string;
}[];
export declare const EIP712_DOMAIN_STRING_NO_CONTRACT: string;
export declare const EIP712_DOMAIN_STRUCT_NO_CONTRACT: {
    name: string;
    type: string;
}[];
export declare function isValidSigType(sigType: number): boolean;
export declare function ecRecoverTypedSignature(hash: string, typedSignature: string): address;
export declare function createTypedSignature(signature: string, sigType: number): string;
/**
 * Fixes any signatures that don't have a 'v' value of 27 or 28
 */
export declare function fixRawSignature(signature: string): string;
