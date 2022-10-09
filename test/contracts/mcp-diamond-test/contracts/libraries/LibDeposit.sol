// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./LibReserve.sol";

library LibDeposit {
    function _hasDeposit(
        address _account,
        bytes32 _loanMarket,
        bytes32 _commitment
    ) internal view returns (bool ret) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indDepositRecord[_account][_loanMarket][_commitment].id != 0;
    }

    function _hasAccount(address _account) internal view {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        require(ds.savingsPassbook[_account].accOpenTime != 0, "ERROR: No savings account");
    }

    function _hasYield(YieldLedger memory yield) internal pure {
        require(yield.id != 0, "ERROR: No Yield");
    }

    function _ensureSavingsAccount(address _account, SavingsAccount storage savingsAccount) internal {
        if (savingsAccount.accOpenTime == 0) {
            savingsAccount.accOpenTime = block.timestamp;
            savingsAccount.account = _account;
        }
    }

    function _getDepositInterest(
        bytes32 market,
        bytes32 _commitment,
        uint256 oldLengthAccruedYield,
        uint256 oldTime
    ) internal view returns (uint256 interestFactor) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        APY storage apy = ds.indAPYRecords[market][_commitment];

        uint256 index = oldLengthAccruedYield - 1;
        uint256 time = oldTime;
        uint256 aggregateYield;

        // 1. apr.time.length > oldLengthAccruedInterest => there is some change.
        if (apy.time.length > oldLengthAccruedYield) {
            if (apy.time[index] < time) {
                uint256 newIndex = index + 1;
                uint256 timeDiff = block.timestamp - apy.time[apy.time.length - 1];
                if (timeDiff > 600) aggregateYield = (timeDiff * apy.apyChanges[index]);

                for (uint256 i = newIndex; i < apy.apyChanges.length; i++) {
                    timeDiff = apy.time[i + 1] - apy.time[i];
                    if (timeDiff > 600) aggregateYield += (timeDiff * apy.apyChanges[i]);
                }
            } else if (apy.time[index] == time) {
                for (uint256 i = index; i < apy.apyChanges.length; i++) {
                    uint256 timeDiff = apy.time[i + 1] - apy.time[i];
                    if (timeDiff > 600) aggregateYield += (timeDiff * apy.apyChanges[i]);
                }
            }
        } else if (apy.time.length == oldLengthAccruedYield && block.timestamp > time) {
            if (apy.time[index] < time || apy.time[index] == time) {
                uint256 timeDiff = block.timestamp - time;
                if (timeDiff > 600) aggregateYield += (timeDiff * apy.apyChanges[index]);
            }
        }
        interestFactor = aggregateYield;
        return interestFactor;
    }

    function _calcAPY(
        bytes32 market,
        bytes32 _commitment,
        uint256 oldLengthAccruedYield,
        uint256 oldTime,
        uint256 aggregateYield
    )
        internal
        view
        returns (
            uint256,
            uint256,
            uint256
        )
    {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        APY storage apy = ds.indAPYRecords[market][_commitment];

        require(oldLengthAccruedYield > 0, "ERROR : oldLengthAccruedYield < 1");

        aggregateYield = _getDepositInterest(market, _commitment, oldLengthAccruedYield, oldTime);

        oldLengthAccruedYield = apy.time.length;
        oldTime = block.timestamp;

        return (oldLengthAccruedYield, oldTime, aggregateYield);
    }
}
