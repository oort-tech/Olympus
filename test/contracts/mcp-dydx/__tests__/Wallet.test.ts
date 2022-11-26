//import {
//  SigningMethod,
//  address,
//  OffChainAction,
//} from '../src/types';
import { getSolo } from './helpers/Solo';
import { TestSolo } from './modules/TestSolo';
import { deployContract } from './helpers/Deploy';
import SoloMarginJson from '../build/contracts/SoloMargin.json';

//let accounts: address[];
//let signer: address;
let Solo: any;
let soloContract: any;

describe('signOffChainAction', () => {
  let solo: TestSolo;
  
  beforeAll(async () => {
    const r = await getSolo();
    solo = r.solo;
    //accounts = r.accounts;
    //account = accounts[1];
    //signer = accounts[0];

    [Solo] = await Promise.all([
      deployContract(solo, SoloMarginJson),
    ]);
    soloContract = Solo.options.address
    console.log('Solo for testing deployed to:', soloContract)
  });

  it('Successfully deploy testing solo', async () => {
     //pass
  });
});
