import { TestContracts } from './TestContracts';
import { SendOptions, TxResult, address, Decimal } from '../../src/types';
export declare class TestInterestSetter {
    private contracts;
    constructor(contracts: TestContracts);
    getAddress(): string;
    setInterestRate(token: address, interestRate: Decimal, options?: SendOptions): Promise<TxResult>;
}
