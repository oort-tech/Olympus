// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { AppStorageOpen, LoanRecords, LibCommon, LibLoan2 } from "../libraries/LibLoan2.sol";
import { Pausable } from "../util/Pausable.sol";
import { IBEP20 } from "../util/IBEP20.sol";
import { ILoan2 } from "../interfaces/ILoan2.sol";
import { IAccessRegistry } from "../interfaces/IAccessRegistry.sol";

contract Loan2 is Pausable, ILoan2 {
    event LoanRepaid(
        address indexed account,
        uint256 indexed id,
        bytes32 indexed market,
        uint256 amount,
        uint256 timestamp
    );

    receive() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    fallback() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    function repayLoan(
        bytes32 _loanMarket,
        bytes32 _commitment,
        uint256 _repayAmount
    ) external override nonReentrant returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        LoanRecords storage loan = ds.indLoanRecords[msg.sender][_loanMarket][_commitment];
        uint256 repaymentAmount = LibLoan2._repayLoan(msg.sender, _loanMarket, _commitment, _repayAmount);
        emit LoanRepaid(msg.sender, loan.id, loan.market, repaymentAmount, block.timestamp);
        return true;
    }

    function pauseLoan2() external override authLoan2 {
        _pause();
    }

    function unpauseLoan2() external override authLoan2 {
        _unpause();
    }

    function isPausedLoan2() external view virtual override returns (bool) {
        return _paused();
    }

    modifier authLoan2() {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        require(
            IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.superAdmin, msg.sender) ||
                IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.adminLoan2, msg.sender),
            "ERROR: Not an admin"
        );

        _;
    }
}
