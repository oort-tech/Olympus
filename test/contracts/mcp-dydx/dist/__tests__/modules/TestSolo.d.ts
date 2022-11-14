import { Provider } from 'web3/providers';
import Web3 from 'web3';
import { Solo } from '../../src/Solo';
import { TestContracts } from './TestContracts';
import { Testing } from './Testing';
import { SoloOptions } from '../../src/types';
export declare class TestSolo extends Solo {
    contracts: TestContracts;
    testing: Testing;
    constructor(provider: Provider, networkId: number, options?: SoloOptions);
    setProvider(provider: Provider, networkId: number): void;
    protected createContractsModule(provider: Provider, networkId: number, web3: Web3, options: SoloOptions): any;
}
