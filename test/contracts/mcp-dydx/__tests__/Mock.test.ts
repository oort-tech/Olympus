import { getSolo } from './helpers/Solo';
import { TestSolo } from './modules/TestSolo';
import { address } from '../src/types';
import { deployContract } from "./helpers/Deploy";
import TokenAJson from '../build/contracts/TokenA.json';

let tokenA: any;
let tokenAAddress: any;
//let SoloTest: any;
//let SoloMarginTest: any;

describe('EVM', () => {
  let solo: TestSolo;
  let accounts: address[];
  let user: address;

  beforeAll(async () => {
    const r = await getSolo();
    solo = r.solo;
    accounts = r.accounts;
    user = accounts[0];
    
    [tokenA] = await Promise.all([
      deployContract(solo, TokenAJson),
    ]);
    tokenAAddress = tokenA.options.address;
    console.log('deployed to..:', tokenAAddress);
  });
  console.log('deployed to:', tokenAAddress )
  console.log(user);
    
  it('Pass this thing', async () => {
        console.log('pass...');
    });
});
