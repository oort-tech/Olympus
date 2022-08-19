// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./LibCommon.sol";

library LibReserve {
    function _avblReservesDeposit(bytes32 _loanMarket) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.marketReservesDeposit[_loanMarket];
    }

    function _utilisedReservesDeposit(bytes32 _loanMarket) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.marketUtilisationDeposit[_loanMarket];
    }

    function _avblReservesLoan(bytes32 _loanMarket) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.marketReservesLoan[_loanMarket];
    }

    function _utilisedReservesLoan(bytes32 _loanMarket) internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.marketUtilisationLoan[_loanMarket];
    }

    function _marketReserves(bytes32 _market) internal view returns (uint256) {
        return _avblReservesDeposit(_market) + _avblReservesLoan(_market);
    }

    function _marketUtilisation(bytes32 _market) internal view returns (uint256) {
        return _utilisedReservesLoan(_market);
    }

    function _avblMarketReserves(bytes32 _market) internal view returns (uint256) {
        IBEP20 token = IBEP20(LibCommon._connectMarket(_market));
        uint256 balance = token.balanceOf(address(this));

        require((_marketReserves(_market) - _marketUtilisation(_market)) >= 0, "ERROR: Mathematical error");
        require(balance >= (_marketReserves(_market) - _marketUtilisation(_market)), "ERROR: Reserve imbalance");

        if (balance > (_marketReserves(_market) - _marketUtilisation(_market))) {
            return balance;
        }
        return (_marketReserves(_market) - _marketUtilisation(_market));
    }

    function _updateReservesLoan(
        bytes32 _loanMarket,
        uint256 _amount,
        uint256 _num
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        if (_num == 0) {
            ds.marketReservesLoan[_loanMarket] += _amount;
        } else if (_num == 1) {
            ds.marketReservesLoan[_loanMarket] -= _amount;
        }
    }

    function _updateUtilisationLoan(
        bytes32 _loanMarket,
        uint256 _amount,
        uint256 _num
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        if (_num == 0) {
            ds.marketUtilisationLoan[_loanMarket] += _amount;
        } else if (_num == 1) {
            // require(ds.marketUtilisationLoan[_loanMarket] >= _amount, "ERROR: Utilisation is less than amount");
            ds.marketUtilisationLoan[_loanMarket] -= _amount;
        }
    }

    function _getReserveFactor() internal view returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.reserveFactor;
    }

    function _updateReservesDeposit(
        bytes32 _loanMarket,
        uint256 _amount,
        uint256 _num /*authContract(DEPOSIT_ID)*/
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        if (_num == 0) {
            ds.marketReservesDeposit[_loanMarket] += _amount;
        } else if (_num == 1) {
            ds.marketReservesDeposit[_loanMarket] -= _amount;
        }
    }

    function _updateUtilisationDeposit(
        bytes32 _market,
        uint256 _amount,
        uint256 _num
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        if (_num == 0) {
            ds.marketUtilisationLoan[_market] += _amount;
        } else if (_num == 1) {
            // require(ds.marketUtilisationLoan[_loanMarket] >= _amount, "ERROR: Utilisation is less than amount");
            ds.marketUtilisationLoan[_market] -= _amount;
        }
    }
}
