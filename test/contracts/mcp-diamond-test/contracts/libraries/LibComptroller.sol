// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./LibCommon.sol";

library LibComptroller {
    function _getAPR(bytes32 market, bytes32 _commitment) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return
            ds.indAPRRecords[market][_commitment].aprChanges[
                ds.indAPRRecords[market][_commitment].aprChanges.length - 1
            ];
    }

    function _getAPRInd(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indAPRRecords[market][_commitment].aprChanges[_index];
    }

    function _getAPY(bytes32 market, bytes32 _commitment) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return
            ds.indAPYRecords[market][_commitment].apyChanges[
                ds.indAPYRecords[market][_commitment].apyChanges.length - 1
            ];
    }

    function _getAPYInd(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indAPYRecords[market][_commitment].apyChanges[_index];
    }

    function _getApytime(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indAPYRecords[market][_commitment].time[_index];
    }

    function _getAprtime(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indAPRRecords[market][_commitment].time[_index];
    }

    function _getApyLastTime(bytes32 market, bytes32 _commitment) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indAPYRecords[market][_commitment].time[ds.indAPYRecords[market][_commitment].time.length - 1];
    }

    function _getAprLastTime(bytes32 market, bytes32 _commitment) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indAPRRecords[market][_commitment].time[ds.indAPRRecords[market][_commitment].time.length - 1];
    }

    function _setDepositCommitment(
        bytes32 _commitment, /*authContract(COMPTROLLER_ID)*/
        uint256 _days
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        ds.depositCommitment.push(_commitment);
        ds.commitmentDays[_commitment] = _days;
        for (uint256 i = ds.depositCommitment.length - 1; i > 0; i--) {
            if (ds.commitmentDays[ds.depositCommitment[i]] < ds.commitmentDays[ds.depositCommitment[i - 1]]) {
                bytes32 temp = ds.depositCommitment[i];
                ds.depositCommitment[i] = ds.depositCommitment[i - 1];
                ds.depositCommitment[i - 1] = temp;
            } else break;
        }
    }

    function _setBorrowCommitment(
        bytes32 _commitment, /*authContract(COMPTROLLER_ID)*/
        uint256 _days
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        ds.borrowCommitment.push(_commitment);
        ds.commitmentDays[_commitment] = _days;
        for (uint256 i = ds.borrowCommitment.length - 1; i > 0; i--) {
            if (ds.commitmentDays[ds.borrowCommitment[i]] < ds.commitmentDays[ds.borrowCommitment[i - 1]]) {
                bytes32 temp = ds.borrowCommitment[i];
                ds.borrowCommitment[i] = ds.borrowCommitment[i - 1];
                ds.borrowCommitment[i - 1] = temp;
            } else break;
        }
    }

    function _updateAPY(
        bytes32 market,
        bytes32 _commitment,
        uint256 _apy
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        APY storage apyUpdate = ds.indAPYRecords[market][_commitment];

        apyUpdate.time.push(block.timestamp);
        apyUpdate.apyChanges.push(_apy);
    }

    function _updateAPR(
        bytes32 market,
        bytes32 _commitment,
        uint256 _apr
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        APR storage aprUpdate = ds.indAPRRecords[market][_commitment];
        aprUpdate.time.push(block.timestamp);
        aprUpdate.aprChanges.push(_apr);
    }
}
