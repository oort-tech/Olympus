import { Contracts } from '../lib/Contracts';
import { address, SendOptions, CallOptions, Integer, TxResult } from '../../src/types';
export declare class ExpiryV2 {
    private contracts;
    constructor(contracts: Contracts);
    getAdmin(options?: CallOptions): Promise<address>;
    getExpiry(accountOwner: address, accountNumber: Integer, marketId: Integer, options?: CallOptions): Promise<Integer>;
    getApproval(approver: address, sender: address, options?: CallOptions): Promise<Integer>;
    getPrices(heldMarketId: Integer, owedMarketId: Integer, expiryTimestamp: Integer, options?: CallOptions): Promise<{
        heldPrice: Integer;
        owedPrice: Integer;
    }>;
    getRampTime(options?: CallOptions): Promise<Integer>;
    setApproval(sender: address, minTimeDelta: Integer, options?: SendOptions): Promise<TxResult>;
    setRampTime(newExpiryRampTime: Integer, options?: SendOptions): Promise<TxResult>;
}
