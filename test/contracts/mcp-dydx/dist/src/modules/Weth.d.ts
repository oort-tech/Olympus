import { Contracts } from '../lib/Contracts';
import { Token } from './Token';
import { SendOptions, CallOptions, TxResult, address, Integer } from '../types';
export declare class Weth {
    private contracts;
    private token;
    private weth;
    constructor(contracts: Contracts, token: Token);
    getAddress(): string;
    wrap(ownerAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    unwrap(ownerAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    getAllowance(ownerAddress: address, spenderAddress: address, options?: CallOptions): Promise<Integer>;
    getBalance(ownerAddress: address, options?: CallOptions): Promise<Integer>;
    getTotalSupply(options?: CallOptions): Promise<Integer>;
    getName(options?: CallOptions): Promise<string>;
    getSymbol(options?: CallOptions): Promise<string>;
    getDecimals(options?: CallOptions): Promise<Integer>;
    getSoloAllowance(ownerAddress: address, options?: CallOptions): Promise<Integer>;
    setAllowance(ownerAddress: address, spenderAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    setSolollowance(ownerAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    setMaximumAllowance(ownerAddress: address, spenderAddress: address, options?: SendOptions): Promise<TxResult>;
    setMaximumSoloAllowance(ownerAddress: address, options?: SendOptions): Promise<TxResult>;
    unsetSoloAllowance(ownerAddress: address, options?: SendOptions): Promise<TxResult>;
    transfer(fromAddress: address, toAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
    transferFrom(fromAddress: address, toAddress: address, senderAddress: address, amount: Integer, options?: SendOptions): Promise<TxResult>;
}
