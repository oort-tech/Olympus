import { TestContracts } from './TestContracts';
export declare class TestExchangeWrapper {
    private contracts;
    constructor(contracts: TestContracts);
    getAddress(): string;
    getExchangeAddress(): string;
}
