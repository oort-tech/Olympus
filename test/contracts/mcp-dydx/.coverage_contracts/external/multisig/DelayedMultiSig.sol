/*

    Copyright 2019 dYdX Trading Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

pragma solidity 0.5.7;
pragma experimental ABIEncoderV2;

import { MultiSig } from "./MultiSig.sol";


/**
 * @title DelayedMultiSig
 * @author dYdX
 *
 * Multi-Signature Wallet with delay in execution.
 * Allows multiple parties to execute a transaction after a time lock has passed.
 * Adapted from Amir Bandeali's MultiSigWalletWithTimeLock contract.

 * Logic Changes:
 *  - Only owners can execute transactions
 *  - Require that each transaction succeeds
 *  - Added function to execute multiple transactions within the same Ethereum transaction
 */
contract DelayedMultiSig is
    MultiSig
{
function coverage_0x71f80266(bytes32 c__0x71f80266) public pure {}

    // ============ Events ============

    event ConfirmationTimeSet(uint256 indexed transactionId, uint256 confirmationTime);
    event TimeLockChange(uint32 secondsTimeLocked);

    // ============ Storage ============

    uint32 public secondsTimeLocked;
    mapping (uint256 => uint256) public confirmationTimes;

    // ============ Modifiers ============

    modifier notFullyConfirmed(
        uint256 transactionId
    ) {coverage_0x71f80266(0xb6d7d228879fa75ed1a530da3d07096c19c9f1d30c47077b36f4159c57116f94); /* function */ 

coverage_0x71f80266(0x55e5cd2f4ac950d1c1139ea5902c5bcc21529e0785bd042807c2795b575a3422); /* line */ 
        coverage_0x71f80266(0x0bc561c59112a436460200ea37dae861f2bdf82a8e6cfe7ca0ecc027c8c0bcdc); /* assertPre */ 
coverage_0x71f80266(0xce67db0c03cfeb4327be0e1cc1fc9cf83ccdc409d83e76e936f015f508a7bf4f); /* statement */ 
require(
            !isConfirmed(transactionId),
            "TX_FULLY_CONFIRMED"
        );coverage_0x71f80266(0xe3ac6d1ee0a4dce552cc9713cb16afc438800b1375f0cef54bde7d4aa34674ae); /* assertPost */ 

coverage_0x71f80266(0x7dd79901bfbab6e7a63dc24fa69ac3ec69eafef40d77f26b36f388ee71e995ac); /* line */ 
        _;
    }

    modifier fullyConfirmed(
        uint256 transactionId
    ) {coverage_0x71f80266(0x2977497d92bfc79b7314af81c4621747017e632819b10f96426132373dc05006); /* function */ 

coverage_0x71f80266(0x01e86790c539079b24cef5d9704ae2e360396c3167379661070c453ef28d21ec); /* line */ 
        coverage_0x71f80266(0xc55f712c6af11dc6fbd3c9d3c30314a612a21a6cc667551117041befd8a7d643); /* assertPre */ 
coverage_0x71f80266(0x58df030464689ccc8c8b315e5ee16936ad42522565b29b68ce3ab24e69e2ac63); /* statement */ 
require(
            isConfirmed(transactionId),
            "TX_NOT_FULLY_CONFIRMED"
        );coverage_0x71f80266(0x41573d00ce39cce3c8b8147cec2b19557dcfcbaee5c65a2f200d85c3542fef7d); /* assertPost */ 

coverage_0x71f80266(0x1f2db3a814a703020e3413458c4807d027befa73a812f753ea5cab6431820a7d); /* line */ 
        _;
    }

    modifier pastTimeLock(
        uint256 transactionId
    ) {coverage_0x71f80266(0xefde2634ecbb574d6de5526c0ab1e49562af4ede3f2b3ae6fa5cd23a91ec5d30); /* function */ 

coverage_0x71f80266(0xe76bf10f9b800b10c1de631dc818a4cbeda09d147a5ae35d7a7c81ee83a0d24c); /* line */ 
        coverage_0x71f80266(0xb67dddeb5a190478f3dde3a76b53f9807880017435e8c346322cdf1df40dcb30); /* assertPre */ 
coverage_0x71f80266(0x5e154322176fca55ee041a5864486b9d5e90c2bb4233620fd354c9dda4359a2c); /* statement */ 
require(
            block.timestamp >= confirmationTimes[transactionId] + secondsTimeLocked,
            "TIME_LOCK_INCOMPLETE"
        );coverage_0x71f80266(0x260bcb7bd6664e5cf5e836bb8a962d1019db77a5b84ae3623d69a626816cbb6f); /* assertPost */ 

coverage_0x71f80266(0x2deb1489f714c451539ea5f5d334a8952e37376d76aa68e4f713daf5fbe4326e); /* line */ 
        _;
    }

    // ============ Constructor ============

    /**
     * Contract constructor sets initial owners, required number of confirmations, and time lock.
     *
     * @param  _owners             List of initial owners.
     * @param  _required           Number of required confirmations.
     * @param  _secondsTimeLocked  Duration needed after a transaction is confirmed and before it
     *                             becomes executable, in seconds.
     */
    constructor (
        address[] memory _owners,
        uint256 _required,
        uint32 _secondsTimeLocked
    )
        public
        MultiSig(_owners, _required)
    {coverage_0x71f80266(0x7e8e2740c02eccc53af2c22bbb3e842cfc70f268540c90e82114cf4aade7eb12); /* function */ 

coverage_0x71f80266(0x1979f07dd69dd061f95a38f7d823d651bc6a1b585e7d145d29ec0214a5e66761); /* line */ 
        coverage_0x71f80266(0xc098466627a3c8285dff2cd3dfddccf22c0c90d6924145ea31e4cbb89028ff72); /* statement */ 
secondsTimeLocked = _secondsTimeLocked;
    }

    // ============ Wallet-Only Functions ============

    /**
     * Changes the duration of the time lock for transactions.
     *
     * @param  _secondsTimeLocked  Duration needed after a transaction is confirmed and before it
     *                             becomes executable, in seconds.
     */
    function changeTimeLock(
        uint32 _secondsTimeLocked
    )
        public
        onlyWallet
    {coverage_0x71f80266(0xadb4b1ecd6631846542c084655e953f372a36d423678ebe88e0dd975afb85913); /* function */ 

coverage_0x71f80266(0x82c0569cf73b3457a20f37942f6044d1a4b2714df3f72d599759566a67d1daf9); /* line */ 
        coverage_0x71f80266(0x848ec5015b82ba5606313c0c96651d47a2881bcc95aa77f022d049963ce85c12); /* statement */ 
secondsTimeLocked = _secondsTimeLocked;
coverage_0x71f80266(0x17b0bf9053cbc68dd4ebf7f6368104efa04319ca2c5fa7e82a5aa8b09e9b08d5); /* line */ 
        coverage_0x71f80266(0x2f598d34ef6f9f1cf43a7a2dece3e99eedbb9cdb5691554310e2699527b2323f); /* statement */ 
emit TimeLockChange(_secondsTimeLocked);
    }

    // ============ Admin Functions ============

    /**
     * Allows an owner to confirm a transaction.
     * Overrides the function in MultiSig.
     *
     * @param  transactionId  Transaction ID.
     */
    function confirmTransaction(
        uint256 transactionId
    )
        public
        ownerExists(msg.sender)
        transactionExists(transactionId)
        notConfirmed(transactionId, msg.sender)
        notFullyConfirmed(transactionId)
    {coverage_0x71f80266(0x2273212e35fc566d116835df317490e291f7fbae73007c3c57eca19965bcd012); /* function */ 

coverage_0x71f80266(0xb348120a6f118ca52a2a95a1ef645c38bc609d63dbc8595d5e3ee0bfd656cbbf); /* line */ 
        coverage_0x71f80266(0x76993eb564bb47f9aabdcc18ff971cdd0aecc4ff125335d0ac229d27a9c5ccb0); /* statement */ 
confirmations[transactionId][msg.sender] = true;
coverage_0x71f80266(0xdb77f5444e24b2037fd0ea416ce223046cef2974bef7289e07536282efa14663); /* line */ 
        coverage_0x71f80266(0xafaaf38a374046ff6a6bfe54ec0dc4f50f7d504eca0003f7d649db032c6d96ab); /* statement */ 
emit Confirmation(msg.sender, transactionId);
coverage_0x71f80266(0x512dd63fd7b64018268d5b3bb9896c6ad777cf5de108964287c818be16953b74); /* line */ 
        coverage_0x71f80266(0x773fc6629aa176286af1e2b54a7c925f343339ee49aad48edac929887ae0c109); /* statement */ 
if (isConfirmed(transactionId)) {coverage_0x71f80266(0xe172b66bf24598f1dce7cd711e2b522844ce51bcf3daff8b15264edb0e9a4299); /* branch */ 

coverage_0x71f80266(0x52471889f6cf11691cb64dff7d93dfe9696f8a822095761f4c528c1e0b9b1ef8); /* line */ 
            coverage_0x71f80266(0x22acba005c8d36dae239685de007e335611891f9b12f528f980bc4cfcab79e4a); /* statement */ 
setConfirmationTime(transactionId, block.timestamp);
        }else { coverage_0x71f80266(0x52e7ecc7e6ce4720bebd8db8ee5b9cda266f0c452c6f23788674702f5c330335); /* branch */ 
}
    }

    /**
     * Allows an owner to execute a confirmed transaction.
     * Overrides the function in MultiSig.
     *
     * @param  transactionId  Transaction ID.
     */
    function executeTransaction(
        uint256 transactionId
    )
        public
        ownerExists(msg.sender)
        notExecuted(transactionId)
        fullyConfirmed(transactionId)
        pastTimeLock(transactionId)
    {coverage_0x71f80266(0x94d73cc0ad152be0e8b066f8af5fd0ce72b7d0510b10ddea7284ba4dd4c47666); /* function */ 

coverage_0x71f80266(0x54f91b918c55da4a4b32a0ef93d17de18d6817ecd866d14f3e28758baf8c9a3e); /* line */ 
        coverage_0x71f80266(0x207f87d989c551238c4c96c43f32d65ce554de562c0454a6cc9b6c32b0d1aa80); /* statement */ 
Transaction storage txn = transactions[transactionId];
coverage_0x71f80266(0x9ec5ccb753441bc481869aff012978f7b6e44c122be3396371106b70d4b39617); /* line */ 
        coverage_0x71f80266(0x5cf6549fc92c16e162bec9f764f3d49e17a8949aa927707ced3a21abffe541df); /* statement */ 
txn.executed = true;
coverage_0x71f80266(0x9d03fa0a8c0b5c39f6664288dcc298380b231160d4fec780486bbb5c1bd1ad47); /* line */ 
        coverage_0x71f80266(0xe9ae850c1fd55c9bba1a227ac3817b7a052b2a86ea140390a40900b450bb6253); /* statement */ 
bool success = externalCall(
            txn.destination,
            txn.value,
            txn.data.length,
            txn.data
        );
coverage_0x71f80266(0x66f18355bdde99a4cf58f70c0959af36e15e170816f51b84db35697a47258935); /* line */ 
        coverage_0x71f80266(0xbd1c581bd839e91a83cdbd40820c270e628fa7d2a712585912fd28cf67d1a1de); /* assertPre */ 
coverage_0x71f80266(0xdf511ca5af08659af81293c9799267946ad3427279665fe0580d2edd4b179d61); /* statement */ 
require(
            success,
            "TX_REVERTED"
        );coverage_0x71f80266(0xe85e473e4f0bc4c369d20821674512e4601652dea7b71e2737f9ff27a0dfb6ec); /* assertPost */ 

coverage_0x71f80266(0xaed96e969bd506f009c23919870823e91d613709b1361a397dec68ef9038b3b5); /* line */ 
        coverage_0x71f80266(0x9fe3059d614e21eb0b1208f97e38c958c2f95cee5ed426a44ed683395e348c70); /* statement */ 
emit Execution(transactionId);
    }

    /**
     * Allows an owner to execute multiple confirmed transactions.
     *
     * @param  transactionIds  List of transaction IDs.
     */
    function executeMultipleTransactions(
        uint256[] memory transactionIds
    )
        public
        ownerExists(msg.sender)
    {coverage_0x71f80266(0xb9c5aa77a1c835d9ec6bc9a5a04ef24ef300d2251de5a062a8c8823a0711dc04); /* function */ 

coverage_0x71f80266(0x1672e6db1c5a5c94cb7e5acf4c099c92b73713d72a8d747de6e890d382303b54); /* line */ 
        coverage_0x71f80266(0xf40f4266fd32b9b7861aea707b4ce89bd364fbffd9841191413d7850295b5e9e); /* statement */ 
for (uint256 i = 0; i < transactionIds.length; i++) {
coverage_0x71f80266(0xc630adedfb45b29a27fb8f60ce2a3aea204aec5d2e11cb2464769a511da700b2); /* line */ 
            coverage_0x71f80266(0x5a525f899a844fc22dc6fb80075ddb6fec61e8c10f0a4f3e0b6772d390419248); /* statement */ 
executeTransaction(transactionIds[i]);
        }
    }

    // ============ Helper Functions ============

    /**
     * Sets the time of when a submission first passed.
     */
    function setConfirmationTime(
        uint256 transactionId,
        uint256 confirmationTime
    )
        internal
    {coverage_0x71f80266(0x8a4b8df3cd4e3b3bea6261011e93b66e1c62070493f0788c8301d0cc955e7123); /* function */ 

coverage_0x71f80266(0xdb778c9a7033efe5d149445ccc1f6d828e7562fd39dd6bb10a54782043d1081e); /* line */ 
        coverage_0x71f80266(0x1b70e2a7bb47ae728c0b7483208d1b277302c8e58dc6f86adcf26670e1bd8470); /* statement */ 
confirmationTimes[transactionId] = confirmationTime;
coverage_0x71f80266(0x2652a533029f16c62964c54112244d8c2f000a086d6784b7c4c5cb5c6d4b612b); /* line */ 
        coverage_0x71f80266(0xa53f4515ff1a6b2530ac85cdf868ea793cc9a93e8bd7c4e10df5cd6a2ba1d196); /* statement */ 
emit ConfirmationTimeSet(transactionId, confirmationTime);
    }
}
