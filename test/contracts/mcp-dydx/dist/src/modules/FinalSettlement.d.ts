import { Contracts } from '../lib/Contracts';
import { SendOptions, CallOptions, Integer, TxResult } from '../../src/types';
export declare class FinalSettlement {
    private contracts;
    constructor(contracts: Contracts);
    getPrices(heldMarketId: Integer, owedMarketId: Integer, timestamp: Integer, options?: CallOptions): Promise<{
        heldPrice: Integer;
        owedPrice: Integer;
    }>;
    getRampTime(options?: CallOptions): Promise<Integer>;
    getStartTime(options?: CallOptions): Promise<Integer>;
    initialize(options?: SendOptions): Promise<TxResult>;
}
