import { SigningMethod, address } from '../../src/types';
import { SignOffChainAction } from './SignOffChainAction';
export declare class WalletLogin extends SignOffChainAction {
    signLogin(expiration: Date, signer: string, signingMethod: SigningMethod): Promise<string>;
    walletLoginIsValid(expiration: Date, typedSignature: string, expectedSigner: address): boolean;
    getWalletLoginHash(expiration: Date): string;
}
