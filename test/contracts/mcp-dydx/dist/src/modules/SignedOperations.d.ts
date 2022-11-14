import Web3 from 'web3';
import { Signer } from './Signer';
import { Contracts } from '../lib/Contracts';
import { address, Action, AssetAmount, Operation, SignedOperation, SendOptions, CallOptions, SigningMethod } from '../../src/types';
export declare class SignedOperations extends Signer {
    private contracts;
    private networkId;
    constructor(contracts: Contracts, web3: Web3, networkId: number);
    /**
     * Sends an transaction to cancel an operation on-chain.
     */
    cancelOperation(operation: Operation, options?: SendOptions): Promise<any>;
    /**
     * Returns true if the contract can process operations.
     */
    isOperational(options?: CallOptions): Promise<boolean>;
    /**
     * Gets the status and the current filled amount (in makerAmount) of all given orders.
     */
    getOperationsAreInvalid(operations: Operation[], options?: CallOptions): Promise<boolean[]>;
    /**
     * Sends operation to current provider for signing. Can sign locally if the signing account is
     * loaded into web3 and SigningMethod.Hash is used.
     */
    signOperation(operation: Operation, signingMethod: SigningMethod): Promise<string>;
    /**
     * Sends operation to current provider for signing of a cancel message. Can sign locally if the
     * signing account is loaded into web3 and SigningMethod.Hash is used.
     */
    signCancelOperation(operation: Operation, signingMethod: SigningMethod): Promise<string>;
    /**
     * Sends operationHash to current provider for signing of a cancel message. Can sign locally if
     * the signing account is loaded into web3 and SigningMethod.Hash is used.
     */
    signCancelOperationByHash(operationHash: string, signer: string, signingMethod: SigningMethod): Promise<string>;
    /**
     * Uses web3.eth.sign to sign a cancel message for an operation. This signature is not used
     * on-chain,but allows dYdX backend services to verify that the cancel operation api call is from
     * the original maker of the operation.
     */
    ethSignCancelOperation(operation: Operation): Promise<string>;
    /**
     * Uses web3.eth.sign to sign a cancel message for an operation hash. This signature is not used
     * on-chain, but allows dYdX backend services to verify that the cancel operation api call is from
     * the original maker of the operation.
     */
    ethSignCancelOperationByHash(operationHash: string, signer: address): Promise<string>;
    /**
     * Returns true if the operation object has a non-null valid signature from the maker of the
     * operation.
     */
    operationHasValidSignature(signedOperation: SignedOperation): boolean;
    /**
     * Returns true if the operation hash has a non-null valid signature from a particular signer.
     */
    operationByHashHasValidSignature(operationHash: string, typedSignature: string, expectedSigner: address): boolean;
    /**
     * Returns true if the cancel operation message has a valid signature.
     */
    cancelOperationHasValidSignature(operation: Operation, typedSignature: string): boolean;
    /**
     * Returns true if the cancel operation message has a valid signature.
     */
    cancelOperationByHashHasValidSignature(operationHash: string, typedSignature: string, expectedSigner: address): boolean;
    /**
     * Returns the final signable EIP712 hash for approving an operation.
     */
    getOperationHash(operation: Operation): string;
    /**
     * Returns the EIP712 hash of the actions array.
     */
    getActionsHash(actions: Action[]): string;
    /**
     * Returns the EIP712 hash of a single Action struct.
     */
    getActionHash(action: Action): string;
    /**
     * Returns the EIP712 hash of an AssetAmount struct.
     */
    getAssetAmountHash(amount: AssetAmount): string;
    /**
     * Given some operation hash, returns the hash of a cancel-operation message.
     */
    operationHashToCancelOperationHash(operationHash: string): string;
    /**
     * Returns the EIP712 domain separator hash.
     */
    getDomainHash(): string;
    /**
     * Returns a signable EIP712 Hash of a struct
     */
    getEIP712Hash(structHash: string): string;
    private getDomainData;
    private ethSignTypedOperationInternal;
    private ethSignTypedCancelOperationInternal;
}
