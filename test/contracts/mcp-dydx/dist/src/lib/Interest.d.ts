import { BigNumber } from 'bignumber.js';
import { Decimal, Integer } from '../types';
export declare class Interest {
    private networkId;
    constructor(networkId: number);
    setNetworkId(networkId: number): void;
    getEarningsRate(): Decimal;
    getInterestPerSecondByMarket(marketId: Integer, totals: {
        totalBorrowed: Integer;
        totalSupply: Integer;
    }): {
        borrowInterestRate: BigNumber;
        supplyInterestRate: BigNumber;
    };
    private getNetworkConstants;
    private getMarketConstants;
}
