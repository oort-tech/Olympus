import Web3 from 'web3';
import { Contracts } from '../lib/Contracts';
import { LogParsingOptions, TxResult } from '../types';
export declare class Logs {
    private contracts;
    private web3;
    constructor(contracts: Contracts, web3: Web3);
    parseLogs(receipt: TxResult, options?: LogParsingOptions): any;
    private logIsFrom;
    private parseAllLogs;
    private parseEvent;
    private parseLog;
    private parseLogWithContract;
    private parseArgs;
    private parseOrderFlags;
    private parseTuple;
    private parseAccountInfo;
    private parseIndex;
    private parseBalanceUpdate;
    private parseDecimalValue;
    private parseIntegerValue;
    private parseFillData;
}
