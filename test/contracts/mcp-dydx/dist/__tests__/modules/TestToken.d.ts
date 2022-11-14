import { TestContracts } from './TestContracts';
import { Token } from '../../src/modules/Token';
import { TestToken as TestTokenContract } from '../../build/testing_wrappers/TestToken';
import { SendOptions, CallOptions, TxResult, address, Integer } from '../../src/types';
export declare class TestToken {
    private contracts;
    private token;
    private testTokenContract;
    constructor(contracts: TestContracts, token: Token, testTokenContract: TestTokenContract);
    getAddress(): string;
    issue(amount: Integer, from: address, options?: SendOptions): Promise<TxResult>;
    issueTo(amount: Integer, who: address, options?: SendOptions): Promise<TxResult>;
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
