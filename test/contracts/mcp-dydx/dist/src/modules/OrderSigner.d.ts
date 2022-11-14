import Web3 from 'web3';
import { Signer } from './Signer';
import { Contracts } from '../lib/Contracts';
import { address, SigningMethod, SignableOrder, SignedOrder, SendOptions, CallOptions } from '../../src/types';
export declare abstract class OrderSigner extends Signer {
    protected web3: Web3;
    protected contracts: Contracts;
    constructor(web3: Web3, contracts: Contracts);
    /**
     * Returns true if the contract can process orders.
     */
    isOperational(options?: CallOptions): Promise<boolean>;
    /**
     * Sends an transaction to pre-approve an order on-chain (so that no signature is required when
     * filling the order).
     */
    approveOrder(order: SignableOrder, options?: SendOptions): Promise<any>;
    /**
     * Sends an transaction to cancel an order on-chain.
     */
    cancelOrder(order: SignableOrder, options?: SendOptions): Promise<any>;
    /**
     * Sends order to current provider for signing. Can sign locally if the signing account is
     * loaded into web3 and SigningMethod.Hash is used.
     */
    signOrder(order: SignableOrder, signingMethod: SigningMethod): Promise<string>;
    /**
     * Sends order to current provider for signing of a cancel message. Can sign locally if the
     * signing account is loaded into web3 and SigningMethod.Hash is used.
     */
    signCancelOrder(order: SignableOrder, signingMethod: SigningMethod): Promise<string>;
    /**
     * Sends orderHash to current provider for signing of a cancel message. Can sign locally if
     * the signing account is loaded into web3 and SigningMethod.Hash is used.
     */
    signCancelOrderByHash(orderHash: string, signer: string, signingMethod: SigningMethod): Promise<string>;
    /**
     * Returns true if the order object has a non-null valid signature from the maker of the order.
     */
    orderHasValidSignature(order: SignedOrder): boolean;
    /**
     * Returns true if the order hash has a non-null valid signature from a particular signer.
     */
    orderByHashHasValidSignature(orderHash: string, typedSignature: string, expectedSigner: address): boolean;
    /**
     * Returns true if the cancel order message has a valid signature.
     */
    cancelOrderHasValidSignature(order: SignableOrder, typedSignature: string): boolean;
    /**
     * Returns true if the cancel order message has a valid signature.
     */
    cancelOrderByHashHasValidSignature(orderHash: string, typedSignature: string, expectedSigner: address): boolean;
    abstract getOrderHash(order: SignableOrder): string;
    protected abstract stringifyOrder(order: SignableOrder): string;
    protected abstract orderHashToCancelOrderHash(orderHash: string): string;
    protected abstract getContract(): any;
    protected abstract ethSignTypedOrderInternal(order: SignableOrder, signingMethod: SigningMethod): Promise<string>;
    protected abstract ethSignTypedCancelOrderInternal(orderHash: string, signer: address, signingMethod: SigningMethod): Promise<string>;
}
