import Web3 from 'web3';
import { Signer } from './Signer';
import { SigningMethod, address, OffChainAction } from '../types';
export declare class SignOffChainAction extends Signer {
    private domain;
    private version;
    private networkId;
    private EIP712_OFF_CHAIN_ACTION_ALL_STRUCT_STRING;
    constructor(web3: Web3, networkId: number, { domain, version, }?: {
        domain?: string;
        version?: string;
    });
    signOffChainAction(expiration: Date, signer: string, signingMethod: SigningMethod, action: OffChainAction): Promise<string>;
    signOffChainActionIsValid(expiration: Date, typedSignature: string, expectedSigner: address, action: OffChainAction): boolean;
    getDomainHash(): string;
    getOffChainActionHash(expiration: Date, action: OffChainAction): string;
    private getDomainData;
}
