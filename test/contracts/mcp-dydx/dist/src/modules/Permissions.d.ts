import { Contracts } from '../lib/Contracts';
import { SendOptions, TxResult, address } from '../types';
export declare class Permissions {
    private contracts;
    constructor(contracts: Contracts);
    setOperators(operatorArgs: ({
        operator: address;
        trusted: boolean;
    })[], options?: SendOptions): Promise<TxResult>;
    approveOperator(operator: address, options?: SendOptions): Promise<TxResult>;
    disapproveOperator(operator: address, options?: SendOptions): Promise<TxResult>;
}
