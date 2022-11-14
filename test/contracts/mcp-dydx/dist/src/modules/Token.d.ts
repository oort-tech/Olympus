import { EventEmitter } from 'web3/types';
import { Contracts } from '../lib/Contracts';
import { SendOptions, TxResult, address, Integer, CallOptions } from '../types';
export declare class Token {
    private contracts;
    private tokens;
    constructor(contracts: Contracts);
    getAllowance(tokenAddress: address, ownerAddress: address, spenderAddress: address, options?: CallOptions): Promise<Integer>;
    getBalance(tokenAddress: address, ownerAddress: address, options?: CallOptions): Promise<Integer>;
    getTotalSupply(tokenAddress: address, options?: CallOptions): Promise<Integer>;
    getName(tokenAddress: address, options?: CallOptions): Promise<string>;
    getSymbol(tokenAddress: address, options?: CallOptions): Promise<string>;
    getDecimals(tokenAddress: address, options?: CallOptions): Promise<Integer>;
    getSoloAllowance(tokenAddress: address, ownerAddress: address, options?: CallOptions): Promise<Integer>;
    setAllowance(tokenAddress: address, ownerAddress: address, spenderAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    setSolollowance(tokenAddress: address, ownerAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    setMaximumAllowance(tokenAddress: address, ownerAddress: address, spenderAddress: address, options?: SendOptions): Promise<TxResult>;
    setMaximumSoloAllowance(tokenAddress: address, ownerAddress: address, options?: SendOptions): Promise<TxResult>;
    unsetSoloAllowance(tokenAddress: address, ownerAddress: address, options?: SendOptions): Promise<TxResult>;
    transfer(tokenAddress: address, fromAddress: address, toAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    transferFrom(tokenAddress: address, fromAddress: address, toAddress: address, senderAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    subscribeToTransfers(tokenAddress: address, { from, to, fromBlock, }?: {
        from?: address;
        to?: address;
        fromBlock?: number;
    }): EventEmitter;
    subscribeToApprovals(tokenAddress: address, { owner, spender, fromBlock, }?: {
        owner?: address;
        spender?: address;
        fromBlock?: number;
    }): EventEmitter;
    private getToken;
}
