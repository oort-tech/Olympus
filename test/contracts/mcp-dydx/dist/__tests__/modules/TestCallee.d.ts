import { TestContracts } from './TestContracts';
import { address, Integer, CallOptions } from '../../src/types';
export declare class TestCallee {
    private contracts;
    constructor(contracts: TestContracts);
    getAddress(): string;
    getAccountData(accountOwner: address, accountNumber: Integer, options?: CallOptions): Promise<string>;
    getSenderData(sender: address, options?: CallOptions): Promise<string>;
}
