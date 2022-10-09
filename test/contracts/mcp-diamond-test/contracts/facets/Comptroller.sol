// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { AppStorageOpen,DepositRecords, APR, APY, LibCommon, LibComptroller } from "../libraries/LibComptroller.sol";
import { LibReserve } from "../libraries/LibReserve.sol";
import { Pausable } from "../util/Pausable.sol";
import { IComptroller } from "../interfaces/IComptroller.sol";
import { IAccessRegistry } from "../interfaces/IAccessRegistry.sol";

contract Comptroller is Pausable, IComptroller {
    // using Address for address;

    event APRupdated(address indexed admin, uint256 indexed newAPR, uint256 indexed timestamp);
    event APYupdated(address indexed admin, uint256 indexed newAPY, uint256 indexed timestamp);

    event ReserveFactorUpdated(
        address indexed admin,
        uint256 oldReserveFactor,
        uint256 indexed newReserveFactor,
        uint256 indexed timestamp
    );
    event LoanIssuanceFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event TimelockValidityDeposit(
        address indexed admin,
        uint256 oldTime,
        uint256 indexed newTime,
        uint256 indexed timestamp
    );
    event LoanClosureFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event LoanPreClosureFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event CollateralPreClosureFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event DepositPreClosureFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event DepositWithdrawalFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event CollateralReleaseFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event YieldConversionFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event MarketSwapFeesUpdated(
        address indexed admin,
        uint256 oldFees,
        uint256 indexed newFees,
        uint256 indexed timestamp
    );
    event MaxWithdrawalUpdated(
        address indexed admin,
        uint256 indexed newFactor,
        uint256 indexed newBlockLimit,
        uint256 oldFactor,
        uint256 oldBlockLimit,
        uint256 timestamp
    );

    event CommitmentAdded(address indexed admin, bytes32 indexed _commitment, uint256 indexed _days, uint256 timestamp);

    receive() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    fallback() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    function getAPR(bytes32 market, bytes32 _commitment) external view override returns (uint256) {
        return LibComptroller._getAPR(market, _commitment);
    }

    function getAPRInd(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) external view override returns (uint256) {
        return LibComptroller._getAPRInd(market, _commitment, _index);
    }

    function getAPY(bytes32 market, bytes32 _commitment) external view override returns (uint256) {
        return LibComptroller._getAPY(market, _commitment);
    }

    function getAPYInd(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) external view override returns (uint256) {
        return LibComptroller._getAPYInd(market, _commitment, _index);
    }

    function getApytime(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) external view override returns (uint256) {
        return LibComptroller._getApytime(market, _commitment, _index);
    }

    function getAprtime(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) external view override returns (uint256) {
        return LibComptroller._getAprtime(market, _commitment, _index);
    }

    function getApyLastTime(bytes32 market, bytes32 _commitment) external view override returns (uint256) {
        return LibComptroller._getApyLastTime(market, _commitment);
    }

    function getAprLastTime(bytes32 market, bytes32 _commitment) external view override returns (uint256) {
        return LibComptroller._getAprLastTime(market, _commitment);
    }

    function getApyTimeLength(bytes32 market, bytes32 _commitment) external view override returns (uint256) {
        return LibCommon._getApyTimeLength(market, _commitment);
    }

    function getAprTimeLength(bytes32 market, bytes32 _commitment) external view override returns (uint256) {
        return LibCommon._getAprTimeLength(market, _commitment);
    }

    function getCommitment(uint256 _index, uint256 depositorborrow) external view override returns (bytes32) {
        return LibCommon._getCommitment(_index, depositorborrow);
    }

    // SETTERS
    function setDepositCommitment(bytes32 _commitment, uint256 _days) external override nonReentrant authComptroller {
        LibComptroller._setDepositCommitment(_commitment, _days);
        emit CommitmentAdded(msg.sender, _commitment, _days, block.timestamp);
    }

    function setBorrowCommitment(bytes32 _commitment, uint256 _days) external override nonReentrant authComptroller {
        LibComptroller._setBorrowCommitment(_commitment, _days);
        emit CommitmentAdded(msg.sender, _commitment, _days, block.timestamp);
    }
    function setTimelockValidityDeposit(uint256 time) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldTime = ds.timelockValidity;
        ds.timelockValidity = time;
        emit TimelockValidityDeposit(msg.sender, oldTime, ds.timelockValidity, block.timestamp);
        return true;
    }
    function getTimelockValidityDeposit() external override view returns (uint256) {
        return LibCommon.diamondStorage().timelockValidity;
    }

    function updateLoanIssuanceFees(uint256 fees) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.loanIssuanceFees;
        ds.loanIssuanceFees = fees;

        emit LoanIssuanceFeesUpdated(msg.sender, oldFees, ds.loanIssuanceFees, block.timestamp);
        return true;
    }

    function loanClosureFees() external view returns (uint256) {
        return LibCommon.diamondStorage().loanClosureFees;
    }

    function updateLoanClosureFees(uint256 fees) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.loanClosureFees;
        ds.loanClosureFees = fees;

        emit LoanClosureFeesUpdated(msg.sender, oldFees, ds.loanClosureFees, block.timestamp);
        return true;
    }

    function loanIssuanceFees() external view returns (uint256) {
        return LibCommon.diamondStorage().loanIssuanceFees;
    }

    function updateLoanPreClosureFees(uint256 fees) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.loanPreClosureFees;
        ds.loanPreClosureFees = fees;

        emit LoanPreClosureFeesUpdated(msg.sender, oldFees, ds.loanPreClosureFees, block.timestamp);
        return true;
    }

    function updateCollateralPreclosureFees(uint256 fees)
        external
        override
        nonReentrant
        authComptroller
        returns (bool)
    {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.collateralPreClosureFees;
        ds.collateralPreClosureFees = fees;

        emit CollateralPreClosureFeesUpdated(msg.sender, oldFees, ds.loanPreClosureFees, block.timestamp);
        return true;
    }

    function collateralPreClosureFees() external view override returns (uint256) {
        return LibCommon.diamondStorage().collateralPreClosureFees;
    }

    function depositPreClosureFees() external view override returns (uint256) {
        return LibCommon.diamondStorage().depositPreClosureFees;
    }

    function updateDepositPreclosureFees(uint256 fees) external override authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.depositPreClosureFees;
        ds.depositPreClosureFees = fees;

        emit DepositPreClosureFeesUpdated(msg.sender, oldFees, ds.depositPreClosureFees, block.timestamp);
        return true;
    }

    function depositWithdrawalFees() external view override returns (uint256) {
        return LibCommon.diamondStorage().depositWithdrawalFees;
    }

    function updateWithdrawalFees(uint256 fees) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.depositWithdrawalFees;
        ds.depositWithdrawalFees = fees;

        emit DepositWithdrawalFeesUpdated(msg.sender, oldFees, ds.depositWithdrawalFees, block.timestamp);
        return true;
    }

    function collateralReleaseFees() external view override returns (uint256) {
        return LibCommon.diamondStorage().collateralReleaseFees;
    }

    function updateCollateralReleaseFees(uint256 fees) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.collateralReleaseFees;
        ds.collateralReleaseFees = fees;

        emit CollateralReleaseFeesUpdated(msg.sender, oldFees, ds.collateralReleaseFees, block.timestamp);
        return true;
    }

    function updateYieldConversion(uint256 fees) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.yieldConversionFees;
        ds.yieldConversionFees = fees;

        emit YieldConversionFeesUpdated(msg.sender, oldFees, ds.yieldConversionFees, block.timestamp);
        return true;
    }

    function updateMarketSwapFees(uint256 fees) external override nonReentrant authComptroller returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFees = ds.marketSwapFees;
        ds.marketSwapFees = fees;

        emit MarketSwapFeesUpdated(msg.sender, oldFees, ds.marketSwapFees, block.timestamp);
        return true;
    }

    function marketSwapFees() external view returns (uint256) {
        return LibCommon.diamondStorage().marketSwapFees;
    }

    function updateReserveFactor(uint256 _reserveFactor) external override nonReentrant authComptroller returns (bool) {
        // implementing the barebones version for testnet.
        //  if cdr >= reserveFactor, 1:3 possible, else 1:2 possible.
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldReserveFactor = ds.reserveFactor;
        ds.reserveFactor = _reserveFactor;

        emit ReserveFactorUpdated(msg.sender, oldReserveFactor, ds.reserveFactor, block.timestamp);
        return true;
    }

    // this function sets a maximum permissible amount that can be moved in a single transaction without the admin permissions.
    function updateMaxWithdrawal(uint256 factor, uint256 blockLimit)
        external
        override
        nonReentrant
        authComptroller
        returns (bool)
    {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 oldFactor = ds.maxWithdrawalFactor;
        uint256 oldBlockLimit = blockLimit;

        ds.maxWithdrawalFactor = factor;
        ds.maxWithdrawalBlockLimit = blockLimit;

        emit MaxWithdrawalUpdated(
            msg.sender,
            ds.maxWithdrawalFactor,
            ds.maxWithdrawalBlockLimit,
            oldFactor,
            oldBlockLimit,
            block.timestamp
        );
        return true;
    }

    function getReserveFactor() external view override returns (uint256) {
        return LibReserve._getReserveFactor();
    }

    function updateReservesDeposit(bytes32 _market, uint256 _amount) external authComptroller {
        LibReserve._updateReservesDeposit(_market, _amount, 0);
    }

    function updateAPR(
        bytes32 _market,
        bytes32 _commitment,
        uint256 _apr
    ) external authComptroller {
        LibComptroller._updateAPR(_market, _commitment, _apr);
        emit APRupdated(msg.sender, _apr, block.timestamp);
    }

    function updateAPY(
        bytes32 _market,
        bytes32 _commitment,
        uint256 _apy
    ) external authComptroller {
        LibComptroller._updateAPY(_market, _commitment, _apy);
        emit APYupdated(msg.sender, _apy, block.timestamp);
    }

    modifier authComptroller() {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        require(
            IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.superAdmin, msg.sender) ||
                IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.adminComptroller, msg.sender),
            "ERROR: Not an admin"
        );
        _;
    }

    function pauseComptroller() external override authComptroller {
        _pause();
    }

    function unpauseComptroller() external override authComptroller {
        _unpause();
    }

    function isPausedComptroller() external view virtual override returns (bool) {
        return _paused();
    }
}
