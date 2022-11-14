import Web3 from 'web3';
import { SigningMethod } from '../../src/types';
export declare abstract class Signer {
    protected web3: Web3;
    constructor(web3: Web3);
    /**
     * Returns a signable EIP712 Hash of a struct
     */
    getEIP712Hash(structHash: string): string;
    /**
     * Returns the EIP712 domain separator hash.
     */
    abstract getDomainHash(): string;
    protected ethSignTypedDataInternal(signer: string, data: any, signingMethod: SigningMethod): Promise<string>;
}
