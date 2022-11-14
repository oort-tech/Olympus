import { TestContracts } from './TestContracts';
import { CallOptions, SendOptions, TxResult, Integer } from '../../src/types';
export declare class TestDoubleExponentInterestSetter {
    private contracts;
    constructor(contracts: TestContracts);
    getAddress(): string;
    getInterestRate(borrowWei: Integer, supplyWei: Integer, options?: CallOptions): Promise<Integer>;
    setParameters(maxAPR: Integer, coefficients: Integer, options?: SendOptions): Promise<TxResult>;
}
