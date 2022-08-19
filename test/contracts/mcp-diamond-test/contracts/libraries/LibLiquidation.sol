// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

// import "./LibSwap.sol";
import "./LibLoan2.sol";
import { IPancakeRouter01 } from "../interfaces/IPancakeRouter01.sol";
import { IAccessRegistry } from "../interfaces/IAccessRegistry.sol";
import "hardhat/console.sol";

library LibLiquidation {
    function _validateLiquidator(address liquidator) private view returns (bool validLiquidator) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        DepositRecords[] memory deposits = ds.savingsPassbook[liquidator].deposits;
        uint256 usdDeposited;
        for (uint256 i = 0; i < deposits.length; i++) {
            usdDeposited += (LibOracle._getQuote(deposits[i].market)) * deposits[i].amount;
            if (usdDeposited >= 2500) break;
        }

        require(usdDeposited >= 2500, "Liquidator criteria not met!");
        return true;
    }

    function _getDebtCategory(LoanRecords memory loan, CollateralRecords memory collateral)
        private
        view
        returns (uint8)
    {
        uint256 loanMarketPrice = LibOracle._getQuote(loan.market);
        uint256 collateralMarketPrice;
        if (loan.market == collateral.market) {
            collateralMarketPrice = loanMarketPrice;
        } else {
            collateralMarketPrice = LibOracle._getQuote(collateral.market);
        }

        if (((loanMarketPrice * loan.amount) / (collateralMarketPrice * collateral.initialAmount)) <= 1) {
            return uint8(1);
        } else if (((loanMarketPrice * loan.amount) / (collateralMarketPrice * collateral.initialAmount)) <= 2) {
            return uint8(2);
        } else {
            return uint8(3);
        }
    }

    function _validLoanLiquidation(
        LoanState memory loanState,
        CollateralRecords memory collateral,
        uint8 debtCategory
    ) private view returns (bool) {
        uint256 loanCurrentAmount = loanState.actualLoanAmount;
        uint256 collateralCurrentAmount;
        if (loanState.loanMarket != loanState.currentMarket) {
            loanCurrentAmount = LibSwap._getAmountOutMin(
                LibSwap._getMarket2Address(loanState.currentMarket),
                LibSwap._getMarketAddress(loanState.loanMarket),
                loanState.currentAmount
            );
        }

        collateralCurrentAmount = LibSwap._getAmountOutMin(
            LibSwap._getMarketAddress(collateral.market),
            LibSwap._getMarketAddress(loanState.loanMarket),
            collateral.amount
        );

        if (debtCategory == 1) {
            return ((106 * loanState.actualLoanAmount) / 100) >= (loanCurrentAmount + collateralCurrentAmount);
        } else if (debtCategory == 2) {
            return ((105 * loanState.actualLoanAmount) / 100) >= (loanCurrentAmount + collateralCurrentAmount);
        } else {
            return ((104 * loanState.actualLoanAmount) / 100) >= (loanCurrentAmount + collateralCurrentAmount);
        }
    }

    function _liquidation(
        address liquidator,
        address account,
        bytes32 _market,
        bytes32 _commitment
    ) internal returns (uint256) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        LoanAccount storage loanAccount = ds.loanPassbook[account];
        LoanState storage loanState = ds.indLoanState[account][_market][_commitment];
        LoanRecords storage loan = ds.indLoanRecords[account][_market][_commitment];
        CollateralRecords storage collateral = ds.indCollateralRecords[account][_market][_commitment];
        DeductibleInterest storage deductibleInterest = ds.indAccruedAPR[account][_market][_commitment];

        require(loan.id != 0, "ERROR: Loan does not exist");

        LibLoan._accruedInterest(account, loanState.currentMarket, loan.commitment);

        if (
            LibSwap._getAmountOutMin(
                LibSwap._getMarketAddress(loan.market),
                LibSwap._getMarketAddress(collateral.market),
                deductibleInterest.accruedInterest
            ) > collateral.amount
        ) {
            collateral.amount = 0;
        } else {
            collateral.amount =
                collateral.amount -
                LibSwap._getAmountOutMin(
                    LibSwap._getMarketAddress(loan.market),
                    LibSwap._getMarketAddress(collateral.market),
                    deductibleInterest.accruedInterest
                );
        }

        delete deductibleInterest.id;
        delete deductibleInterest.market;
        delete deductibleInterest.oldLengthAccruedInterest;
        delete deductibleInterest.oldTime;
        delete deductibleInterest.accruedInterest;
        require(
            _validLoanLiquidation(loanState, collateral, _getDebtCategory(loan, collateral)),
            "Liquidation price not hit"
        );

        uint256 loanAmount = loan.amount;

        LibReserve._updateReservesLoan(collateral.market, collateral.amount, 1);
        if (IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.protocolOwnedLiquidator, liquidator)) {
            // protocol initiated liquidation
            uint256 remnantAmount = LibLoan2._repaymentProcess(
                loan.id - 1,
                0,
                loanAccount,
                loan,
                loanState,
                collateral,
                deductibleInterest,
                true
            );
            LibReserve._updateReservesLoan(loan.market, remnantAmount, 0);
        } else if (_validateLiquidator(liquidator)) {
            // external liquidator with valid access
            IBEP20(LibCommon._connectMarket(loan.market)).transferFrom(liquidator, address(this), loan.amount);
            uint256 remnantAmount = LibLoan2._repaymentProcess(
                loan.id - 1,
                loan.amount,
                loanAccount,
                loan,
                loanState,
                collateral,
                deductibleInterest,
                true
            );

            collateral.amount = LibSwap._swap(loan.market, collateral.market, (70 * remnantAmount) / 100, 2);
            LibReserve._updateReservesDeposit(collateral.market, remnantAmount - ((70 * remnantAmount) / 100), 0);

            IBEP20(LibCommon._connectMarket(collateral.market)).transfer(
                liquidator,
                collateral.amount +
                    LibSwap._getAmountOutMin(
                        LibSwap._getMarketAddress(loan.market),
                        LibSwap._getMarketAddress(collateral.market),
                        loan.amount
                    )
            );
        }

        LibReserve._updateUtilisationLoan(loan.market, loanState.actualLoanAmount, 1);

        /// DELETING THE LOAN ENTRIES
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
        delete loanState.loanMarket;
        delete loanState.actualLoanAmount;
        delete loanState.currentMarket;
        delete loanState.currentAmount;
        delete loanState.state;

        _fillLoanArrayGap(account, loan);

        /// LOAN RECORDS
        delete loan.id;
        delete loan.market;
        delete loan.commitment;
        delete loan.amount;
        delete loan.isSwapped;
        delete loan.createdAt;

        return loanAmount;
    }

    function _fillLoanArrayGap(address account, LoanRecords memory loan) private {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        LoanAccount storage loanAccount = ds.loanPassbook[account];
        ActiveLoans storage activeLoans = ds.getActiveLoans[account];

        LoanRecords memory lastLoanAccountLoan = loanAccount.loans[loanAccount.loans.length - 1];
        loanAccount.loans[loan.id - 1] = lastLoanAccountLoan;
        loanAccount.collaterals[loan.id - 1] = loanAccount.collaterals[loanAccount.loans.length - 1];
        loanAccount.loanState[loan.id - 1] = loanAccount.loanState[loanAccount.loans.length - 1];
        loanAccount.accruedAPR[loan.id - 1] = loanAccount.accruedAPR[loanAccount.loans.length - 1];
        loanAccount.loans.pop();
        loanAccount.loanState.pop();
        loanAccount.accruedAPR.pop();
        loanAccount.collaterals.pop();

        activeLoans.loanMarket[loan.id - 1] = activeLoans.loanMarket[activeLoans.loanMarket.length - 1];
        activeLoans.loanCommitment[loan.id - 1] = activeLoans.loanCommitment[activeLoans.loanMarket.length - 1];
        activeLoans.loanAmount[loan.id - 1] = activeLoans.loanAmount[activeLoans.loanMarket.length - 1];
        activeLoans.collateralMarket[loan.id - 1] = activeLoans.collateralMarket[activeLoans.loanMarket.length - 1];
        activeLoans.collateralAmount[loan.id - 1] = activeLoans.collateralAmount[activeLoans.loanMarket.length - 1];
        activeLoans.isSwapped[loan.id - 1] = activeLoans.isSwapped[activeLoans.loanMarket.length - 1];
        activeLoans.loanCurrentMarket[loan.id - 1] = activeLoans.loanCurrentMarket[activeLoans.loanMarket.length - 1];
        activeLoans.loanCurrentAmount[loan.id - 1] = activeLoans.loanCurrentAmount[activeLoans.loanMarket.length - 1];
        activeLoans.borrowInterest[loan.id - 1] = activeLoans.borrowInterest[activeLoans.loanMarket.length - 1];
        activeLoans.state[loan.id - 1] = activeLoans.state[activeLoans.loanMarket.length - 1];
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
    }

    function _liquidableLoans(uint256 _indexFrom)
        internal
        view
        returns (
            address[] memory,
            bytes32[] memory,
            bytes32[] memory,
            uint256[] memory,
            bytes32[] memory,
            uint256[] memory
        )
    {
        // TODO: in frontend its showing up an empty data records
        address[] memory loanOwner = new address[](100);
        bytes32[] memory loanMarket = new bytes32[](100);
        bytes32[] memory loanCommitment = new bytes32[](100);
        uint256[] memory loanAmount = new uint256[](100);
        bytes32[] memory collateralMarket = new bytes32[](100);
        uint256[] memory collateralAmount = new uint256[](100);

        uint8 pointer;

        for (uint256 i = _indexFrom; i < _indexFrom + 10 && i < LibCommon.diamondStorage().borrowers.length; i++) {
            LoanState[] memory loanStates = LibCommon
                .diamondStorage()
                .loanPassbook[LibCommon.diamondStorage().borrowers[i]]
                .loanState;
            for (uint256 j = 0; j < loanStates.length; j++) {
                LoanRecords memory loan = LibCommon
                    .diamondStorage()
                    .loanPassbook[LibCommon.diamondStorage().borrowers[i]]
                    .loans[j];
                CollateralRecords memory collateral = LibCommon
                    .diamondStorage()
                    .loanPassbook[LibCommon.diamondStorage().borrowers[i]]
                    .collaterals[j];
                if (
                    loan.id != 0 && _validLoanLiquidation(loanStates[j], collateral, _getDebtCategory(loan, collateral))
                ) {
                    loanOwner[pointer] = loan.owner;
                    loanMarket[pointer] = loan.market;
                    loanCommitment[pointer] = loan.commitment;
                    loanAmount[pointer] = loan.amount;
                    collateralMarket[pointer] = collateral.market;
                    collateralAmount[pointer] = collateral.initialAmount;
                    pointer += 1;
                }
            }
        }

        return (loanOwner, loanMarket, loanCommitment, loanAmount, collateralMarket, collateralAmount);
    }
}
