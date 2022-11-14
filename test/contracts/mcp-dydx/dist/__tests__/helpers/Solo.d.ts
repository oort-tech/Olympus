import { TestSolo } from '../modules/TestSolo';
export declare const solo: TestSolo;
export declare const getSolo: () => Promise<{
    solo: TestSolo;
    accounts: string[];
}>;
