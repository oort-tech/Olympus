import BigNumber from 'bignumber.js';
import { getSolo } from './helpers/Solo';
import { TestSolo } from './modules/TestSolo';
import { address } from '../src/types';
import { resetEVM } from './helpers/EVM';
import TokenAJson from
'../build/contracts/TokenA.json';
import { deployContract } from "./helpers/Deploy";
//import { expect, jest, test } from 'jest';

let tokenA: any;
let tokenAAddress: any;

describe('EVM', () => {
  let solo: TestSolo;
  let accounts: address[];

  beforeAll(async () => {
    const r = await getSolo();
    solo = r.solo;
    accounts = r.accounts;
    
    [tokenA] = await Promise.all([
      deployContract(solo, TokenAJson),
    ]);
    tokenAAddress = tokenA.options.address;
    console.log("deployed to:", tokenAAddress);
  });
  console.log('deployed to:', tokenAAddress )

  beforeEach(async () => {
    await resetEVM();
  });

  it('Resets the state of the EVM successfully', async () => {
    const account = accounts[1];
    const receiver = accounts[2];
    const amount = new BigNumber(1);
    await tokenA.methods.issueTo(
      amount,
      receiver,
    );
    console.log(account);
    //const balance: BigNumber = await tokenA.methods.balanceOf(account);
    //console.log(balance);
    //expect(balance).toBe(amount);

    await resetEVM();

    //const newBalance: BigNumber = await solo.testing.tokenA.getBalance(account);
    //expect(newBalance).toBe(new BigNumber(0));
  });
});

