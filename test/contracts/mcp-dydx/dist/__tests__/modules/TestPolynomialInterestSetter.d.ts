import { TestContracts } from './TestContracts';
import { SendOptions, TxResult, Integer } from '../../src/types';
export declare class TestPolynomialInterestSetter {
    private contracts;
    constructor(contracts: TestContracts);
    getAddress(): string;
    setParameters(maxAPR: Integer, coefficients: Integer, options?: SendOptions): Promise<TxResult>;
}
