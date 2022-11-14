import Contract from 'web3/eth/contract';
import { Contracts } from '../../lib/Contracts';
import { address, Decimal, Integer, CallOptions, SendOptions, TxResult } from '../../types';
export declare class MakerStablecoinPriceOracle {
    private contracts;
    private oracleContract;
    constructor(contracts: Contracts, oracleContract: Contract);
    setPokerAddress(newPoker: address, options?: SendOptions): Promise<TxResult>;
    updatePrice(minimum?: Decimal, maximum?: Decimal, options?: SendOptions): Promise<TxResult>;
    getOwner(options?: CallOptions): Promise<address>;
    getPoker(options?: CallOptions): Promise<address>;
    getPrice(options?: CallOptions): Promise<Integer>;
    getPriceInfo(options?: CallOptions): Promise<{
        price: Decimal;
        lastUpdate: Integer;
    }>;
    getBoundedTargetPrice(options?: CallOptions): Promise<Integer>;
    getTargetPrice(options?: CallOptions): Promise<Integer>;
    getMedianizerPrice(options?: CallOptions): Promise<Integer>;
    getCurvePrice(options?: CallOptions): Promise<Integer>;
    getUniswapPrice(options?: CallOptions): Promise<Integer>;
    getDeviationParams(options?: CallOptions): Promise<{
        maximumPerSecond: Decimal;
        maximumAbsolute: Decimal;
    }>;
}
