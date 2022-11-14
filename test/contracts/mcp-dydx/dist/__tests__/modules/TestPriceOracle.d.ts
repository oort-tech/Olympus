import { TestContracts } from './TestContracts';
import { SendOptions, TxResult, address, Integer } from '../../src/types';
export declare class TestPriceOracle {
    private contracts;
    constructor(contracts: TestContracts);
    getAddress(): string;
    setPrice(token: address, price: Integer, options?: SendOptions): Promise<TxResult>;
    getPrice(token: address): Promise<Integer>;
}
