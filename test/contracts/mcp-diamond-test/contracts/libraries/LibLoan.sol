// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./LibSwap.sol";
import "./LibReserve.sol";
import "./LibOracle.sol";
import "./LibDeposit.sol";

library LibLoan {
    event WithdrawCollateral(
        address indexed account,
        bytes32 indexed market,
        uint256 indexed amount,
        uint256 id,
        uint256 timestamp
    );

    function _hasLoanAccount(address _account) internal view returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        require(ds.loanPassbook[_account].accOpenTime != 0, "ERROR: No Loan Account");
        return true;
    }

    function _accruedInterest(
        address _account,
        bytes32 _loanMarket,
        bytes32 _commitment /*authContract(LOAN_ID)*/
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        uint256 aggregateYield;
        uint256 deductibleUSDValue;
        uint256 oldLengthAccruedInterest;
        uint256 oldTime;

        (oldLengthAccruedInterest, oldTime, aggregateYield) = _calcAPR(
            _loanMarket,
            ds.indLoanRecords[_account][_loanMarket][_commitment].commitment,
            ds.indAccruedAPR[_account][_loanMarket][_commitment].oldLengthAccruedInterest,
            ds.indAccruedAPR[_account][_loanMarket][_commitment].oldTime,
            aggregateYield
        );

        deductibleUSDValue =
            (((ds.indLoanRecords[_account][_loanMarket][_commitment].amount) * LibOracle._getQuote(_loanMarket)) *
                aggregateYield) /
            (365 * 86400 * 10000);
        ds.indAccruedAPR[_account][_loanMarket][_commitment].accruedInterest +=
            deductibleUSDValue /
            LibOracle._getQuote(ds.indCollateralRecords[_account][_loanMarket][_commitment].market);
        ds.indAccruedAPR[_account][_loanMarket][_commitment].oldLengthAccruedInterest = oldLengthAccruedInterest;
        ds.indAccruedAPR[_account][_loanMarket][_commitment].oldTime = oldTime;

        ds
            .loanPassbook[_account]
            .accruedAPR[ds.indLoanRecords[_account][_loanMarket][_commitment].id - 1]
            .accruedInterest = ds.indAccruedAPR[_account][_loanMarket][_commitment].accruedInterest;
        ds
            .loanPassbook[_account]
            .accruedAPR[ds.indLoanRecords[_account][_loanMarket][_commitment].id - 1]
            .oldLengthAccruedInterest = oldLengthAccruedInterest;
        ds
            .loanPassbook[_account]
            .accruedAPR[ds.indLoanRecords[_account][_loanMarket][_commitment].id - 1]
            .oldTime = oldTime;
    }

    function _swapToLoan(
        address _account,
        bytes32 _market,
        bytes32 _commitment /*authContract(LOAN_ID)*/
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 marketSwapFees;
        _hasLoanAccount(_account);
        LibCommon._isMarketSupported(_market);

        LoanRecords storage loan = ds.indLoanRecords[_account][_market][_commitment];
        LoanState storage loanState = ds.indLoanState[_account][_market][_commitment];
        ActiveLoans storage activeLoans = ds.getActiveLoans[_account];

        require(loan.id != 0, "ERROR: No loan");
        require(
            loan.isSwapped == true && loanState.currentMarket != loan.market,
            "ERROR: Swapped market does not exist"
        );

        LibCommon._isMarket2Supported(loanState.currentMarket);

        uint256 num = loan.id - 1;
        marketSwapFees = ((LibCommon.diamondStorage().marketSwapFees) * loanState.currentAmount) / 10000;
        loanState.currentAmount = loanState.currentAmount - (marketSwapFees);
        uint256 _swappedAmount = LibSwap._swap(loanState.currentMarket, loan.market, loanState.currentAmount, 1);
        /// Updating LoanRecord
        loan.isSwapped = false;

        /// updating the LoanState
        loanState.currentMarket = loan.market;
        loanState.currentAmount = _swappedAmount;

        /// Updating LoanAccount
        ds.loanPassbook[_account].loans[num].isSwapped = false;
        ds.loanPassbook[_account].loanState[num].currentMarket = loanState.currentMarket;
        ds.loanPassbook[_account].loanState[num].currentAmount = loanState.currentAmount;

        _accruedInterest(_account, _market, _commitment);

        /// UPDATING ACTIVELOANS
        activeLoans.isSwapped[num] = false;
        activeLoans.loanCurrentMarket[num] = loan.market;
        activeLoans.loanCurrentAmount[num] = _swappedAmount;
        activeLoans.borrowInterest[num] = ds.indAccruedAPR[_account][_market][_commitment].accruedInterest;
    }

    function _swapLoan(
        address _sender,
        bytes32 _loanMarket,
        bytes32 _commitment,
        bytes32 _swapMarket /*authContract(LOAN_ID)*/
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        _hasLoanAccount(_sender);
        uint256 marketSwapFees;
        LibCommon._isMarketSupported(_loanMarket);
        LibCommon._isMarket2Supported(_swapMarket);

        LoanAccount storage loanAccount = ds.loanPassbook[_sender];
        LoanRecords storage loan = ds.indLoanRecords[_sender][_loanMarket][_commitment];
        LoanState storage loanState = ds.indLoanState[_sender][_loanMarket][_commitment];
        ActiveLoans storage activeLoans = ds.getActiveLoans[_sender];

        require(loan.id != 0, "ERROR: No loan");
        require(loan.isSwapped == false && loanState.currentMarket == _loanMarket, "ERROR: Already swapped");

        uint256 _swappedAmount;
        uint256 num = loan.id - 1;
        marketSwapFees = ((LibCommon.diamondStorage().marketSwapFees) * loanState.currentAmount) / 10000;
        loanState.currentAmount = loanState.currentAmount - (marketSwapFees);
        _swappedAmount = LibSwap._swap(loan.market, _swapMarket, loanState.currentAmount, 0);

        /// Updating LoanRecord
        loan.isSwapped = true;


        /// Updating LoanState
        loanState.currentMarket = _swapMarket;
        loanState.currentAmount = _swappedAmount;

        /// Updating LoanAccount
        loanAccount.loans[num].isSwapped = true;
        loanAccount.loanState[num].currentMarket = loanState.currentMarket;
        loanAccount.loanState[num].currentAmount = loanState.currentAmount;

        _accruedInterest(_sender, _loanMarket, _commitment);

        /// UPDATING ACTIVELOANS
        activeLoans.isSwapped[num] = true;
        activeLoans.loanCurrentMarket[num] = _swapMarket;
        activeLoans.loanCurrentAmount[num] = _swappedAmount;
        activeLoans.borrowInterest[num] = ds.indAccruedAPR[_sender][_loanMarket][_commitment].accruedInterest;
    }

    function _withdrawCollateral(
        address _sender,
        bytes32 _market,
        bytes32 _commitment
    ) internal {
        _hasLoanAccount(_sender);
        LibCommon._isMarketSupported(_market);

        AppStorageOpen storage ds = LibCommon.diamondStorage();

        LoanAccount storage loanAccount = ds.loanPassbook[_sender];
        LoanRecords storage loan = ds.indLoanRecords[_sender][_market][_commitment];
        LoanState storage loanState = ds.indLoanState[_sender][_market][_commitment];
        CollateralRecords storage collateral = ds.indCollateralRecords[_sender][_market][_commitment];
        ActiveLoans storage activeLoans = ds.getActiveLoans[_sender];
        uint256 commitmentTimelock;

        commitmentTimelock = ds.commitmentDays[_commitment] * 86400;
        // Convert commitment days into seconds for block.timestamp

        /// REQUIRE STATEMENTS - CHECKING FOR LOAN, REPAYMENT & COLLATERAL TIMELOCK.
        require(loan.id != 0, "ERROR: Loan does not exist");
        require(loanState.state == STATE.REPAID, "ERROR: Active loan");
        // require((collateral.timelockValidity + commitmentTimelock) < block.timestamp, "ERROR: Active Timelock");

        console.log("loan.createdAt : ",loan.createdAt);
        console.log("commitmentTimelock : ",commitmentTimelock);
        console.log("collateral.activationTime : ",collateral.activationTime);
        
        if (loan.createdAt + commitmentTimelock >=  collateral.activationTime) {
            collateral.amount =
                collateral.amount -
                ((LibCommon.diamondStorage().collateralPreClosureFees) * collateral.amount) /
                10000;
        }
            require(
                collateral.activationTime + collateral.timelockValidity <= block.timestamp,
                "3 days timelock has not passed yet"
            );

        collateral.amount =
            collateral.amount -
            ((LibCommon.diamondStorage().collateralReleaseFees) * collateral.amount) /
            10000;

        ds.collateralToken = IBEP20(LibCommon._connectMarket(collateral.market));
        ds.collateralToken.transfer(_sender, collateral.amount);

        bytes32 collateralMarket = collateral.market;
        uint256 collateralAmount = collateral.amount;

        LibReserve._updateReservesLoan(collateralMarket, collateralAmount, 1);
        // emit WithdrawCollateral(_sender, collateralMarket, collateralAmount, loan.id, block.timestamp);

        /// UPDATING STORAGE RECORDS FOR LOAN
        /// COLLATERAL RECORDS
        delete collateral.id;
        delete collateral.market;
        delete collateral.commitment;
        delete collateral.amount;
        delete collateral.timelockValidity;
        delete collateral.isTimelockActivated;
        delete collateral.activationTime;
        delete collateral.initialAmount;

        /// LOAN STATE
        delete loanState.id;
        delete loanState.state;

        uint256 loanAccountCount = loanAccount.loans.length;
        LoanRecords memory lastLoanAccountLoan = loanAccount.loans[loanAccountCount - 1];
        loanAccount.loans[loan.id - 1] = lastLoanAccountLoan;
        loanAccount.collaterals[loan.id - 1] = loanAccount.collaterals[loanAccountCount - 1];
        loanAccount.loanState[loan.id - 1] = loanAccount.loanState[loanAccountCount - 1];
        loanAccount.accruedAPR[loan.id - 1] = loanAccount.accruedAPR[loanAccountCount - 1];
        loanAccount.loans.pop();
        loanAccount.loanState.pop();
        loanAccount.accruedAPR.pop();
        loanAccount.collaterals.pop();

        uint256 activeLoansCount = activeLoans.loanMarket.length;
        activeLoans.loanMarket[loan.id - 1] = activeLoans.loanMarket[activeLoansCount - 1];
        activeLoans.loanCommitment[loan.id - 1] = activeLoans.loanCommitment[activeLoansCount - 1];
        activeLoans.loanAmount[loan.id - 1] = activeLoans.loanAmount[activeLoansCount - 1];
        activeLoans.collateralMarket[loan.id - 1] = activeLoans.collateralMarket[activeLoansCount - 1];
        activeLoans.collateralAmount[loan.id - 1] = activeLoans.collateralAmount[activeLoansCount - 1];
        activeLoans.isSwapped[loan.id - 1] = activeLoans.isSwapped[activeLoansCount - 1];
        activeLoans.loanCurrentMarket[loan.id - 1] = activeLoans.loanCurrentMarket[activeLoansCount - 1];
        activeLoans.loanCurrentAmount[loan.id - 1] = activeLoans.loanCurrentAmount[activeLoansCount - 1];
        activeLoans.borrowInterest[loan.id - 1] = activeLoans.borrowInterest[activeLoansCount - 1];
        activeLoans.state[loan.id - 1] = activeLoans.state[activeLoansCount - 1];
        activeLoans.loanMarket.pop();
        activeLoans.loanCommitment.pop();
        activeLoans.loanAmount.pop();
        activeLoans.collateralMarket.pop();
        activeLoans.collateralAmount.pop();
        activeLoans.isSwapped.pop();
        activeLoans.loanCurrentMarket.pop();
        activeLoans.loanCurrentAmount.pop();
        activeLoans.borrowInterest.pop();
        activeLoans.state.pop();

        LoanRecords storage lastLoan = ds.indLoanRecords[lastLoanAccountLoan.owner][lastLoanAccountLoan.market][
            lastLoanAccountLoan.commitment
        ];
        lastLoan.id = loan.id;

        /// LOAN RECORDS
        delete loan.id;
        delete loan.isSwapped;
        delete loan.createdAt;
    }

    function _checkPermissibleWithdrawal(
        address account,
        uint256 amount,
        LoanRecords storage loan,
        LoanState storage loanState,
        CollateralRecords storage collateral
    ) internal {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        require(amount <= loanState.currentAmount, "ERROR: Amount > Loan value");
        require(loanState.currentMarket == loan.market, "ERROR: Can not withdraw secondary markets");

        _accruedInterest(account, loan.market, loan.commitment);

        uint256 collateralAvbl;
        uint256 usdCollateral;
        uint256 usdLoan;
        uint256 usdLoanCurrent;

        /// UPDATE collateralAvbl
        collateralAvbl = collateral.amount - ds.indAccruedAPR[account][loan.market][loan.commitment].accruedInterest;

        /// FETCH USDT PRICES
        usdCollateral = LibOracle._getQuote(collateral.market);
        usdLoan = LibOracle._getQuote(loan.market);
        usdLoanCurrent = LibOracle._getQuote(loanState.currentMarket);

        /// Permissible withdrawal amount calculation in the loanMarket.
        // permissibleAmount = ((usdCollateral*collateralAvbl - (30*usdCollateral*collateral.amount/100))/usdLoanCurrent);
        require(
            ((usdCollateral * collateralAvbl - ((30 * usdCollateral * collateral.amount) / 100)) / usdLoanCurrent) >=
                (amount),
            "ERROR: Request exceeds funds"
        );
        require(
            ((usdCollateral * collateralAvbl) +
                (usdLoanCurrent * loanState.currentAmount) -
                (amount * usdLoanCurrent)) >=
                ((109 * (usdLoan * ds.indLoanRecords[account][loan.market][loan.commitment].amount)) / 100),
            "ERROR: Liquidation risk"
        );
    }

    function _getLoanInterest(
        bytes32 market,
        bytes32 _commitment,
        uint256 oldLengthAccruedYield,
        uint256 oldTime
    ) internal view returns (uint256 interestFactor) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        APR storage apr = ds.indAPRRecords[market][_commitment];

        uint256 index = oldLengthAccruedYield - 1;
        uint256 time = oldTime;
        uint256 aggregateYield;

        // 1. apr.time.length > oldLengthAccruedInterest => there is some change.
        if (apr.time.length > oldLengthAccruedYield) {
            if (apr.time[index] < time) {
                uint256 newIndex = index + 1;
                uint256 timeDiff = block.timestamp - apr.time[apr.time.length - 1];
                if (timeDiff > 600) aggregateYield = (timeDiff * apr.aprChanges[index]);

                for (uint256 i = newIndex; i < apr.aprChanges.length; i++) {
                    timeDiff = apr.time[i + 1] - apr.time[i];
                    if (timeDiff > 600) aggregateYield += (timeDiff * apr.aprChanges[i]);
                }
            } else if (apr.time[index] == time) {
                for (uint256 i = index; i < apr.aprChanges.length; i++) {
                    uint256 timeDiff = apr.time[i + 1] - apr.time[i];
                    if (timeDiff > 600) aggregateYield += (timeDiff * apr.aprChanges[i]);
                }
            }
        } else if (apr.time.length == oldLengthAccruedYield && block.timestamp > time) {
            if (apr.time[index] < time || apr.time[index] == time) {
                uint256 timeDiff = (block.timestamp - time);
                if (timeDiff > 600) aggregateYield += (timeDiff * apr.aprChanges[index]);
            }
        }
        interestFactor = aggregateYield;
        return interestFactor;
    }

    function _calcAPR(
        bytes32 market,
        bytes32 _commitment,
        uint256 oldLengthAccruedInterest,
        uint256 oldTime,
        uint256 aggregateInterest
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

        APR storage apr = ds.indAPRRecords[market][_commitment];

        require(oldLengthAccruedInterest > 0, "oldLengthAccruedInterest is 0");

        aggregateInterest = _getLoanInterest(market, _commitment, oldLengthAccruedInterest, oldTime);
        oldLengthAccruedInterest = apr.time.length;
        oldTime = block.timestamp;
        return (oldLengthAccruedInterest, oldTime, aggregateInterest);
    }
}
