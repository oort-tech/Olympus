import { BigNumber } from 'bignumber.js';
import { Operation } from './operate/Operation';
import { Contracts } from '../lib/Contracts';
import { TxResult, address, SendOptions } from '../types';
export declare class StandardActions {
    private operation;
    private contracts;
    constructor(operation: Operation, contracts: Contracts);
    deposit({ accountOwner, marketId, amount, accountNumber, options, }: {
        accountOwner: address;
        marketId: BigNumber | string;
        amount: BigNumber | string;
        accountNumber: BigNumber | string;
        options?: SendOptions;
    }): Promise<TxResult>;
    withdraw({ accountOwner, marketId, amount, accountNumber, options, }: {
        accountOwner: address;
        marketId: BigNumber | string;
        amount: BigNumber | string;
        accountNumber: BigNumber | string;
        options?: SendOptions;
    }): Promise<TxResult>;
    withdrawToZero({ accountOwner, marketId, accountNumber, options, }: {
        accountOwner: address;
        marketId: BigNumber | string;
        accountNumber: BigNumber | string;
        options?: SendOptions;
    }): Promise<TxResult>;
}
