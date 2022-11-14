import Web3 from 'web3';
import BigNumber from 'bignumber.js';
import { OrderSigner } from './OrderSigner';
import { Contracts } from '../lib/Contracts';
import { address, SendOptions, CallOptions, CanonicalOrder, SignedCanonicalOrder, CanonicalOrderState, LimitOrder, SigningMethod, Integer, Decimal } from '../../src/types';
export declare class CanonicalOrders extends OrderSigner {
    private networkId;
    constructor(contracts: Contracts, web3: Web3, networkId: number);
    protected stringifyOrder(order: CanonicalOrder): any;
    /**
     * Gets the status and the current filled amount (in makerAmount) of all given orders.
     */
    getOrderStates(orders: CanonicalOrder[], options?: CallOptions): Promise<CanonicalOrderState[]>;
    getTakerAddress(options?: CallOptions): Promise<address>;
    setTakerAddress(taker: address, options?: SendOptions): Promise<any>;
    getFeeForOrder(baseMarketBN: Integer, amount: Integer, isTaker?: boolean): Integer;
    /**
     * Returns the estimated account collateralization after making each of the orders provided.
     * The makerAccount of each order should be associated with the same account.
     * This function does not make any on-chain calls and so all information must be passed in
     * (including asset prices and remaining amounts on the orders).
     * - 150% collateralization will be returned as BigNumber(1.5).
     * - Accounts with zero borrow will be returned as BigNumber(infinity) regardless of supply.
     */
    getAccountCollateralizationAfterMakingOrders(weis: Integer[], prices: Integer[], orders: (LimitOrder | CanonicalOrder)[], remainingMakerAmounts: Integer[]): Decimal;
    toSolidity(price: BigNumber): BigNumber;
    /**
     * Returns the final signable EIP712 hash for approving an order.
     */
    getOrderHash(order: CanonicalOrder): string;
    /**
     * Given some order hash, returns the hash of a cancel-order message.
     */
    orderHashToCancelOrderHash(orderHash: string): string;
    /**
     * Returns the EIP712 domain separator hash.
     */
    getDomainHash(): string;
    orderToBytes(order: CanonicalOrder | SignedCanonicalOrder, price?: Decimal, fee?: Decimal): string;
    private getDomainData;
    protected ethSignTypedOrderInternal(order: CanonicalOrder, signingMethod: SigningMethod): Promise<string>;
    protected ethSignTypedCancelOrderInternal(orderHash: string, signer: string, signingMethod: SigningMethod): Promise<string>;
    private getCanonicalOrderFlags;
    protected getContract(): import("../../build/wrappers/CanonicalOrders").CanonicalOrders;
}
