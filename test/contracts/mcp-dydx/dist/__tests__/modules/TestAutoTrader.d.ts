import { TestContracts } from './TestContracts';
import { Amount, SendOptions, TxResult, Integer } from '../../src/types';
export declare class TestAutoTrader {
    private contracts;
    constructor(contracts: TestContracts);
    getAddress(): string;
    setData(tradeId: Integer, amount: Amount, options?: SendOptions): Promise<TxResult>;
    setRequireInputMarketId(market: Integer): Promise<TxResult>;
    setRequireOutputMarketId(market: Integer): Promise<TxResult>;
    setRequireMakerAccount(accountOwner: string, accountNumber: Integer): Promise<TxResult>;
    setRequireTakerAccount(accountOwner: string, accountNumber: Integer): Promise<TxResult>;
    setRequireOldInputPar(par: Integer): Promise<TxResult>;
    setRequireNewInputPar(par: Integer): Promise<TxResult>;
    setRequireInputWei(wei: Integer): Promise<TxResult>;
}
