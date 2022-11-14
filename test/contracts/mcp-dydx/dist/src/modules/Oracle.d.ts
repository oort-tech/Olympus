import { Contracts } from '../lib/Contracts';
import { MakerStablecoinPriceOracle } from './oracles/MakerStablecoinPriceOracle';
export declare class Oracle {
    private contracts;
    daiPriceOracle: MakerStablecoinPriceOracle;
    saiPriceOracle: MakerStablecoinPriceOracle;
    constructor(contracts: Contracts);
}
