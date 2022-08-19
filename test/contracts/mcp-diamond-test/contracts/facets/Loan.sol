// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { DeductibleInterest, LoanRecords, LoanState, CollateralRecords, AppStorageOpen, LoanAccount, ActiveLoans, STATE, YieldLedger, LibCommon, LibReserve, LibDeposit, LibLoan } from "../libraries/LibLoan.sol";
import { Pausable } from "../util/Pausable.sol";
import { IBEP20 } from "../util/IBEP20.sol";
import { ILoan } from "../interfaces/ILoan.sol";
import { IAccessRegistry } from "../interfaces/IAccessRegistry.sol";

contract Loan is Pausable, ILoan {
    event WithdrawCollateral(
        address indexed account,
        bytes32 indexed market,
        uint256 indexed amount,
        uint256 id,
        uint256 timestamp
    );
    event AddCollateral(address indexed account, uint256 indexed id, uint256 amount, uint256 timestamp);
    event WithdrawPartialLoan(address indexed account, uint256 indexed id, uint256 indexed amount, uint256 timestamp);
    event MarketSwapped(
        address indexed account,
        bytes32 loanMarket,
        bytes32 commmitment,
        bool isSwapped,
        bytes32 indexed currentMarket,
        uint256 indexed amount,
        uint256 timestamp
    );

    receive() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    fallback() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    /// Swap loan to a secondary market.
    function swapLoan(
        bytes32 _loanMarket,
        bytes32 _commitment,
        bytes32 _swapMarket
    ) external override nonReentrant returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        LoanRecords storage loan = ds.indLoanRecords[msg.sender][_loanMarket][_commitment];
        LoanState storage loanState = ds.indLoanState[msg.sender][_loanMarket][_commitment];

        LibLoan._swapLoan(msg.sender, _loanMarket, _commitment, _swapMarket);

        emit MarketSwapped(
            msg.sender,
            loanState.loanMarket,
            _commitment,
            loan.isSwapped,
            loanState.currentMarket,
            loanState.currentAmount,
            block.timestamp
        );
        return true;
    }

    /// SwapToLoan
    function swapToLoan(bytes32 _loanMarket, bytes32 _commitment)
        external
        override
        nonReentrant
        returns (bool success)
    {
        LibLoan._swapToLoan(msg.sender, _loanMarket, _commitment);

        AppStorageOpen storage ds = LibCommon.diamondStorage();
        LoanRecords storage loan = ds.indLoanRecords[msg.sender][_loanMarket][_commitment];
        LoanState storage loanState = ds.indLoanState[msg.sender][_loanMarket][_commitment];

        emit MarketSwapped(
            msg.sender,
            loanState.loanMarket,
            _commitment,
            loan.isSwapped,
            loanState.currentMarket,
            loanState.currentAmount,
            block.timestamp
        );
        return success = true;
    }

    function withdrawCollateral(bytes32 _market, bytes32 _commitment) external override nonReentrant returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        LoanRecords storage loan = ds.indLoanRecords[msg.sender][_market][_commitment];
        CollateralRecords storage collateral = ds.indCollateralRecords[msg.sender][_market][_commitment];
        bytes32 collateralMarket = collateral.market;
        uint256 collateralAmount = collateral.amount;
        LibLoan._withdrawCollateral(msg.sender, _market, _commitment);
        emit WithdrawCollateral(msg.sender, collateralMarket, collateralAmount, loan.id, block.timestamp);
        return true;
    }

    function _preAddCollateralProcess(
        bytes32 _collateralMarket,
        LoanRecords storage loan,
        LoanState storage loanState,
        CollateralRecords storage collateral
    )
        private
        view
    // Have removed collateral amount param from this as we stopped using min amount check in add collateral
    {
        require(loan.id != 0, "ERROR: No loan");
        require(loanState.state == STATE.ACTIVE, "ERROR: Inactive loan");
        require(collateral.market == _collateralMarket, "ERROR: Mismatch collateral market");

        LibCommon._isMarketSupported(_collateralMarket);
    }

    function addCollateral(
        bytes32 _loanMarket,
        bytes32 _commitment,
        uint256 _collateralAmount
    ) external override nonReentrant returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        LoanAccount storage loanAccount = ds.loanPassbook[msg.sender];
        LoanRecords storage loan = ds.indLoanRecords[msg.sender][_loanMarket][_commitment];
        LoanState storage loanState = ds.indLoanState[msg.sender][_loanMarket][_commitment];
        CollateralRecords storage collateral = ds.indCollateralRecords[msg.sender][_loanMarket][_commitment];
        ActiveLoans storage activeLoans = ds.getActiveLoans[msg.sender];

        _preAddCollateralProcess(collateral.market, loan, loanState, collateral);

        ds.collateralToken = IBEP20(LibCommon._connectMarket(collateral.market));

        /// TRIGGER: ds.collateralToken.approve() on the client.
        ds.collateralToken.transferFrom(msg.sender, address(this), _collateralAmount);
        LibReserve._updateReservesLoan(collateral.market, _collateralAmount, 0);

        /// UPDATE COLLATERAL IN STORAGE
        collateral.amount += _collateralAmount;
        loanAccount.collaterals[loan.id - 1].amount += _collateralAmount;
        activeLoans.collateralAmount[loan.id - 1] += _collateralAmount; // updating activeLoans

        LibLoan._accruedInterest(msg.sender, _loanMarket, _commitment);

        emit AddCollateral(msg.sender, loan.id, _collateralAmount, block.timestamp);
        return true;
    }

    function withdrawPartialLoan(
        bytes32 _loanMarket,
        bytes32 _commitment,
        uint256 _amount
    ) external override nonReentrant returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        LibLoan._hasLoanAccount(msg.sender);

        LoanRecords storage loan = ds.indLoanRecords[msg.sender][_loanMarket][_commitment];
        LoanState storage loanState = ds.indLoanState[msg.sender][_loanMarket][_commitment];
        CollateralRecords storage collateral = ds.indCollateralRecords[msg.sender][_loanMarket][_commitment];
        ActiveLoans storage activeLoans = ds.getActiveLoans[msg.sender];

        LibLoan._checkPermissibleWithdrawal(msg.sender, _amount, loan, loanState, collateral);

        loanState.currentAmount -= _amount;
        activeLoans.loanCurrentAmount[loan.id - 1] -= _amount;

        ds.loanToken = IBEP20(LibCommon._connectMarket(loan.market));
        ds.loanToken.transfer(msg.sender, _amount);

        emit WithdrawPartialLoan(msg.sender, loan.id, _amount, block.timestamp);
        // emit WithdrawPartialLoan(msg.sender, loan.id, _amount, loan.market, block.timestamp);
        return true;
    }

    function getLoanInterest(address account, uint256 id) external view returns (uint256 loanInterest) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        ActiveLoans memory activeLoans = ds.getActiveLoans[account];
        bytes32 market = activeLoans.loanMarket[id - 1];
        bytes32 commitment = activeLoans.loanCommitment[id - 1];
        uint256 interestFactor = 0;

        LoanState memory loanState = ds.indLoanState[account][market][commitment];
        DeductibleInterest memory yield = ds.indAccruedAPR[account][market][commitment];

        interestFactor = LibLoan._getLoanInterest(market, commitment, yield.oldLengthAccruedInterest, yield.oldTime);
        loanInterest = yield.accruedInterest;
        loanInterest += ((interestFactor * loanState.actualLoanAmount) / (365 * 86400 * 10000));
    }

    function pauseLoan() external override authLoan {
        _pause();
    }

    function unpauseLoan() external override authLoan {
        _unpause();
    }

    function isPausedLoan() external view virtual override returns (bool) {
        return _paused();
    }

    modifier authLoan() {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        require(
            IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.superAdmin, msg.sender) ||
                IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.adminLoan, msg.sender),
            "ERROR: Not an admin"
        );
        _;
    }
}
