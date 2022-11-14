import { Contracts } from '../lib/Contracts';
import { address, SendOptions, Decimal, Integer, TxResult } from '../types';
export declare class Admin {
    private contracts;
    constructor(contracts: Contracts);
    withdrawExcessTokens(marketId: Integer, recipient: address, options?: SendOptions): Promise<TxResult>;
    withdrawUnsupportedTokens(token: address, recipient: address, options?: SendOptions): Promise<TxResult>;
    addMarket(token: address, priceOracle: address, interestSetter: address, marginPremium: Decimal, spreadPremium: Decimal, options?: SendOptions): Promise<TxResult>;
    setIsClosing(marketId: Integer, isClosing: boolean, options?: SendOptions): Promise<TxResult>;
    setMarginPremium(marketId: Integer, marginPremium: Decimal, options?: SendOptions): Promise<TxResult>;
    setSpreadPremium(marketId: Integer, spreadPremium: Decimal, options?: SendOptions): Promise<TxResult>;
    setPriceOracle(marketId: Integer, oracle: address, options?: SendOptions): Promise<TxResult>;
    setInterestSetter(marketId: Integer, interestSetter: address, options?: SendOptions): Promise<TxResult>;
    setMarginRatio(ratio: Decimal, options?: SendOptions): Promise<TxResult>;
    setLiquidationSpread(spread: Decimal, options?: SendOptions): Promise<TxResult>;
    setEarningsRate(rate: Decimal, options?: SendOptions): Promise<TxResult>;
    setMinBorrowedValue(minBorrowedValue: Integer, options?: SendOptions): Promise<TxResult>;
    setGlobalOperator(operator: address, approved: boolean, options?: SendOptions): Promise<TxResult>;
    setExpiryRampTime(newExpiryRampTime: Integer, options?: SendOptions): Promise<TxResult>;
}
