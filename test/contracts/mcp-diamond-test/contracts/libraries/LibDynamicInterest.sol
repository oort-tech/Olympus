// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./LibCommon.sol";
import "./LibReserve.sol";
import "./LibComptroller.sol";

library LibDynamicInterest {
    function _getDepositInterests(uint256 minOrMax) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.depositInterests[minOrMax];
    }

    function _getBorrowInterests(uint256 minOrMax) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.borrowInterests[minOrMax];
    }

    function _getInterestFactors(uint256 offsetOrFactor) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.interestFactors[offsetOrFactor];
    }

    function _setDepositInterests(uint256 minDepositInterest, uint256 maxDepositInterest) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        if (ds.depositInterests.length == 0) ds.depositInterests = new uint256[](2);
        ds.depositInterests[0] = minDepositInterest;
        ds.depositInterests[1] = maxDepositInterest;
    }

    function _setBorrowInterests(uint256 minBorrowInterest, uint256 maxBorrowInterest) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        if (ds.borrowInterests.length == 0) ds.borrowInterests = new uint256[](2);
        ds.borrowInterests[0] = minBorrowInterest;
        ds.borrowInterests[1] = maxBorrowInterest;
    }

    function _setInterestFactors(uint256 offset, uint256 correlationFactor) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        if (ds.interestFactors.length == 0) ds.interestFactors = new uint256[](2);
        ds.interestFactors[0] = offset;
        ds.interestFactors[1] = correlationFactor;
    }

    function _calculateDynamicInterest(bytes32 market, uint256 randomness) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        uint256 utilisationFactor = ((LibReserve._utilisedReservesLoan(market) * 10000) /
            LibReserve._avblReservesDeposit(market));

        // ==============code to round-off utilisationFactor===============
        if (utilisationFactor % 100 >= 50) utilisationFactor = utilisationFactor / 100 + 1;
        else utilisationFactor /= 100;
        // ================================================================

        uint256 correlationFactor = 100000;
        if (utilisationFactor <= 25) {
            for (uint256 i = ds.depositCommitment.length - 1; i >= 0; i--) {
                LibComptroller._updateAPY(market, ds.depositCommitment[i], 0);
                if (i == 0) break;
            }

            for (uint256 i = 0; i < ds.borrowCommitment.length; i++) {
                LibComptroller._updateAPR(
                    market,
                    ds.borrowCommitment[i],
                    ((_getBorrowInterests(0) * 100000) / correlationFactor)
                );
                correlationFactor = (correlationFactor * _getInterestFactors(1)) / 10;
            }
            return;
        }

        uint256 calculatedDepositInterest;
        uint256 calculatedBorrowInterest;

        if (utilisationFactor > 70) {
            calculatedDepositInterest = ((randomness * _getDepositInterests(1)) / 100);
            if (utilisationFactor >= 80) calculatedDepositInterest += 100;
        } else calculatedDepositInterest = ((randomness * (_getDepositInterests(1) - _getDepositInterests(0))) / 100);
        calculatedBorrowInterest = ((calculatedDepositInterest * 10000) /
            ((100 + _getInterestFactors(0)) * utilisationFactor));
        if (calculatedBorrowInterest > _getBorrowInterests(1)) {
            calculatedBorrowInterest = _getBorrowInterests(1);
            calculatedDepositInterest = ((((100 + _getInterestFactors(0)) * utilisationFactor) *
                calculatedBorrowInterest) / 10000);
        }

        correlationFactor = 100000;
        for (uint256 i = ds.depositCommitment.length - 1; i >= 0; i--) {
            LibComptroller._updateAPY(
                market,
                ds.depositCommitment[i],
                ((calculatedDepositInterest * 100000) / correlationFactor)
            );
            correlationFactor = (correlationFactor * _getInterestFactors(1)) / 10;
            if (i == 0) break;
        }

        correlationFactor = 100000;
        for (uint256 i = 0; i < ds.borrowCommitment.length; i++) {
            LibComptroller._updateAPR(
                market,
                ds.borrowCommitment[i],
                ((calculatedBorrowInterest * 100000) / correlationFactor)
            );
            correlationFactor = (correlationFactor * _getInterestFactors(1)) / 10;
        }
    }
}
