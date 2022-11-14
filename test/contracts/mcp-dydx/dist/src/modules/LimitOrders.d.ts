import Web3 from 'web3';
import { OrderSigner } from './OrderSigner';
import { Contracts } from '../lib/Contracts';
import { CallOptions, Decimal, Integer, LimitOrder, SignedLimitOrder, LimitOrderState, SigningMethod } from '../../src/types';
export declare class LimitOrders extends OrderSigner {
    private networkId;
    constructor(contracts: Contracts, web3: Web3, networkId: number);
    /**
     * Gets the status and the current filled amount (in makerAmount) of all given orders.
     */
    getOrderStates(orders: LimitOrder[], options?: CallOptions): Promise<LimitOrderState[]>;
    /**
     * Returns the estimated account collateralization after making each of the orders provided.
     * The makerAccount of each order should be associated with the same account.
     * This function does not make any on-chain calls and so all information must be passed in
     * (including asset prices and remaining amounts on the orders).
     * - 150% collateralization will be returned as BigNumber(1.5).
     * - Accounts with zero borrow will be returned as BigNumber(infinity) regardless of supply.
     */
    getAccountCollateralizationAfterMakingOrders(weis: Integer[], prices: Integer[], orders: LimitOrder[], remainingMakerAmounts: Integer[]): Decimal;
    /**
     * Returns the final signable EIP712 hash for approving an order.
     */
    getOrderHash(order: LimitOrder): string;
    /**
     * Given some order hash, returns the hash of a cancel-order message.
     */
    orderHashToCancelOrderHash(orderHash: string): string;
    /**
     * Returns the EIP712 domain separator hash.
     */
    getDomainHash(): string;
    unsignedOrderToBytes(order: LimitOrder): string;
    signedOrderToBytes(order: SignedLimitOrder): string;
    private orderToByteArray;
    private getDomainData;
    protected ethSignTypedOrderInternal(order: LimitOrder, signingMethod: SigningMethod): Promise<string>;
    protected ethSignTypedCancelOrderInternal(orderHash: string, signer: string, signingMethod: SigningMethod): Promise<string>;
    protected stringifyOrder(order: LimitOrder): any;
    protected getContract(): import("../../build/wrappers/LimitOrders").LimitOrders;
}
