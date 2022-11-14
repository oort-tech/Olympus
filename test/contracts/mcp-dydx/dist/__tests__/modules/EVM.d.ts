import { Provider } from 'web3/providers';
export declare class EVM {
    private provider;
    constructor(provider: Provider);
    setProvider(provider: Provider): void;
    /**
     * Attempts to reset the EVM to its initial state. Useful for testing suites
     *
     * @param provider a valid web3 provider
     * @returns null
     */
    resetEVM(resetSnapshotId?: string): Promise<void>;
    reset(id: string): Promise<string>;
    snapshot(): Promise<string>;
    evmRevert(id: string): Promise<string>;
    stopMining(): Promise<string>;
    startMining(): Promise<string>;
    mineBlock(): Promise<string>;
    increaseTime(duration: number): Promise<string>;
    callJsonrpcMethod(method: string, params?: (any[])): Promise<string>;
    private send;
}
