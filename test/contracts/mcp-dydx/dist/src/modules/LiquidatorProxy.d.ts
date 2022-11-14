import { Contracts } from '../lib/Contracts';
import { address, SendOptions, Decimal, Integer, TxResult } from '../types';
export declare class LiquidatorProxy {
    private contracts;
    constructor(contracts: Contracts);
    liquidate(accountOwner: address, accountNumber: Integer, liquidOwner: address, liquidNumber: Integer, minLiquidatorRatio: Decimal, minValueLiquidated: Integer, owedPreferences: Integer[], heldPreferences: Integer[], options?: SendOptions): Promise<TxResult>;
}
