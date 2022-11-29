import BigNumber from 'bignumber.js';
import { getSolo } from './helpers/Solo';
import { TestSolo } from './modules/TestSolo';
import { address } from '../src/types';
//import { provider } from './helpers/Provider';

describe('EVM', () => {
    let solo: TestSolo;
    let accounts: address[];
    //let addr: any;

    beforeAll(async () => {
        const r = await getSolo();
        solo = r.solo;
        accounts = r.accounts;
        //console.log(accounts)
        //console.log(solo)
        //addr = solo.options.address;
        //console.log(addr);
        //console.log(solo)
        //console.log(solo.testing.evm.provider)
        console.log(await solo.testing.tokenA.issueTo(new BigNumber(1), accounts[0]))
    });

    it.only('should read accounts', async () => {
        const acc = accounts[0];
        console.log(acc);
    })

    it('should transfer between accounts', async () => {
        const acc = accounts[1];
        const amount = new BigNumber(1);
        const transfer = await solo.testing.tokenA.issueTo(amount, acc);
        console.log(transfer);
    })
});
