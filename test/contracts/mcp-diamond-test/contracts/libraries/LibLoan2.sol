// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./LibLoan1.sol";

library LibLoan2 {
    function _repayLoan(
        address _sender,
        bytes32 _loanMarket,
        bytes32 _commitment,
        uint256 _repayAmount
    )
        internal
        returns (
            uint256 /*authContract(LOANEXT_ID)*/
        )
    {
        require(LibCommon.diamondStorage().indLoanRecords[_sender][_loanMarket][_commitment].id != 0, "ERROR: No Loan");
        LibLoan._accruedInterest(_sender, _loanMarket, _commitment);

        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 remnantAmount;

        LoanAccount storage loanAccount = ds.loanPassbook[_sender];
        LoanState storage loanState = ds.indLoanState[_sender][_loanMarket][_commitment];
        LoanRecords storage loan = ds.indLoanRecords[_sender][_loanMarket][_commitment];
        CollateralRecords storage collateral = ds.indCollateralRecords[_sender][_loanMarket][_commitment];
        DeductibleInterest storage deductibleInterest = ds.indAccruedAPR[_sender][_loanMarket][_commitment];
        ActiveLoans storage activeLoans = ds.getActiveLoans[_sender];

        /// TRANSFER FUNDS TO PROTOCOL FROM USER
        if (_repayAmount != 0) {
            ds.loanToken = IBEP20(LibCommon._connectMarket(_loanMarket));
            ds.loanToken.transferFrom(_sender, address(this), _repayAmount);
        }

        _repayAmount = _repayAmount - (LibCommon.diamondStorage().loanClosureFees * _repayAmount) / 10000;
        /// CALCULATE REMNANT AMOUNT
        remnantAmount = _repaymentProcess(
            loan.id - 1,
            _repayAmount,
            loanAccount,
            loan,
            loanState,
            collateral,
            deductibleInterest,
            false
        );

        /// CONVERT remnantAmount into collateralAmount
        collateral.amount = LibSwap._swap(loan.market, collateral.market, remnantAmount, 2);
        LibReserve._updateReservesLoan(collateral.market, collateral.amount, 0);
        /// RESETTING STORAGE VALUES COMMON FOR commitment(0) & commitment(2)

        /// UPDATING LoanRecords
        delete loan.market;
        delete loan.commitment;
        delete loan.amount;

        /// UPDATING LoanState
        delete loanState.loanMarket;
        delete loanState.actualLoanAmount;
        delete loanState.currentMarket;
        delete loanState.currentAmount;

        /// UPDATING RECORDS IN LOANACCOUNT
        delete loanAccount.loans[loan.id - 1].market;
        delete loanAccount.loans[loan.id - 1].commitment;
        delete loanAccount.loans[loan.id - 1].amount;

        delete loanAccount.loanState[loan.id - 1].loanMarket;
        delete loanAccount.loanState[loan.id - 1].actualLoanAmount;
        delete loanAccount.loanState[loan.id - 1].currentMarket;
        delete loanAccount.loanState[loan.id - 1].currentAmount;

        /// ACTIVELOANS
        activeLoans.state[loan.id - 1] = STATE.REPAID;
        delete activeLoans.isSwapped[loan.id - 1];
        delete activeLoans.loanCurrentAmount[loan.id - 1];
        delete activeLoans.borrowInterest[loan.id - 1];

        if (_commitment != LibCommon._getCommitment(0, 1)) {
            /// UPDATING COLLATERAL AMOUNT IN STORAGE
            loanAccount.collaterals[loan.id - 1].amount = collateral.amount;

            collateral.isTimelockActivated = true;
            collateral.activationTime = block.timestamp;

            /// UPDATING LoanRecords
            loan.isSwapped = false;

            /// UPDATING LoanState
            loanState.state = STATE.REPAID;

            /// UPDATING RECORDS IN LOANACCOUNT
            loanAccount.loans[loan.id - 1].isSwapped = false;

            loanAccount.loanState[loan.id - 1].state = STATE.REPAID;
            loanAccount.collaterals[loan.id - 1].activationTime = block.timestamp;
            loanAccount.collaterals[loan.id - 1].isTimelockActivated = true;

            activeLoans.collateralAmount[loan.id - 1] = collateral.amount;

            LibReserve._updateUtilisationDeposit(loan.market, loanState.actualLoanAmount, 1);
            LibReserve._updateUtilisationLoan(loan.market, loanState.actualLoanAmount, 1);
        } else {
            /*/// Transfer remnant collateral to the user if _commitment != _getCommitment(2) */

            collateral.amount =
                collateral.amount -
                ((LibCommon.diamondStorage().collateralReleaseFees) * collateral.amount) /
                10000;

            ds.collateralToken = IBEP20(LibCommon._connectMarket(collateral.market));
            ds.collateralToken.transfer(_sender, collateral.amount);

            emit LibLoan.WithdrawCollateral(_sender, collateral.market, collateral.amount, loan.id, block.timestamp);
            LibReserve._updateReservesLoan(collateral.market, collateral.amount, 1);
            LibReserve._updateUtilisationLoan(loan.market, loanState.actualLoanAmount, 1);
            LibReserve._updateUtilisationDeposit(loan.market, loanState.actualLoanAmount, 1);

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

            // update loan id of the swapped record
            LoanRecords storage lastLoan = ds.indLoanRecords[lastLoanAccountLoan.owner][lastLoanAccountLoan.market][
                lastLoanAccountLoan.commitment
            ];
            lastLoan.id = loan.id;

            /// LOAN RECORDS
            delete loan.id;
            delete loan.isSwapped;
            delete loan.createdAt;
        }
        return _repayAmount;
    }

    function _repaymentProcess(
        uint256 num,
        uint256 _repayAmount,
        LoanAccount storage loanAccount,
        LoanRecords storage loan,
        LoanState storage loanState,
        CollateralRecords storage collateral,
        DeductibleInterest storage deductibleInterest,
        bool liquidationEvent
    ) internal returns (uint256) {
        bytes32 _commitment;
        uint256 _remnantAmount;
        uint256 _collateralAmount;

        _commitment = loan.commitment;
        _remnantAmount = 0;
        _collateralAmount = 0;

        /// convert collateral into loan market to add to the repayAmount
        _collateralAmount =
            collateral.amount -
            LibSwap._getAmountOutMin(
                LibSwap._getMarketAddress(loan.market),
                LibSwap._getMarketAddress(collateral.market),
                deductibleInterest.accruedInterest
            );
        /// COLLATERAL GETTING SWAPPED INTO LOAN
        _repayAmount += LibSwap._swap(collateral.market, loan.market, _collateralAmount, 2);
        LibReserve._updateReservesLoan(collateral.market, _collateralAmount, 1);
        
        if (_repayAmount >= loanState.actualLoanAmount) _remnantAmount = (_repayAmount - loanState.actualLoanAmount);
        else {
            if (loanState.currentMarket == loan.market) _repayAmount += loanState.currentAmount;
            else if (loanState.currentMarket != loanState.loanMarket)
                _repayAmount += LibSwap._swap(
                    loanState.currentMarket,
                    loanState.loanMarket,
                    loanState.currentAmount,
                    1
                );

            _remnantAmount = (_repayAmount - loanState.actualLoanAmount);
        }

        // / UPDATING RECORDS IN LOANACCOUNT
        delete loanAccount.accruedAPR[num];

        if (!liquidationEvent) {
            delete deductibleInterest.id;
            delete deductibleInterest.market;
            delete deductibleInterest.oldLengthAccruedInterest;
            delete deductibleInterest.oldTime;
            delete deductibleInterest.accruedInterest;
            loanAccount.collaterals[num].activationTime = block.timestamp;
            loanAccount.collaterals[num].isTimelockActivated = true;
        }

        return _remnantAmount;
    }
}
