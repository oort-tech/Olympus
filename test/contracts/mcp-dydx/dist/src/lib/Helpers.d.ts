import { BigNumber } from 'bignumber.js';
import { Decimal, Integer } from '../types';
export declare function stringToDecimal(s: string): Decimal;
export declare function decimalToString(d: Decimal | string): string;
export declare function toString(input: number | string | BigNumber): string;
export declare function integerToValue(i: Integer): {
    sign: boolean;
    value: string;
};
export declare function valueToInteger({ value, sign }: {
    value: string;
    sign: boolean;
}): BigNumber;
export declare function coefficientsToString(coefficients: (number | string | Integer)[]): string;
export declare function toNumber(input: string | number | BigNumber): number;
export declare function getInterestPerSecond(maxAPR: Decimal, coefficients: number[], totals: {
    totalBorrowed: Integer;
    totalSupply: Integer;
}): Decimal;
export declare function getInterestPerSecondForPolynomial(maxAPR: Decimal, coefficients: number[], totals: {
    totalBorrowed: Integer;
    totalSupply: Integer;
}): Decimal;
export declare function getInterestPerSecondForDoubleExponent(maxAPR: Decimal, coefficients: number[], totals: {
    totalBorrowed: Integer;
    totalSupply: Integer;
}): Decimal;
