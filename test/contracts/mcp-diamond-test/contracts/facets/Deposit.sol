// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { SavingsAccount, DepositRecords, YieldLedger, ActiveDeposits, AppStorageOpen, LibCommon, LibReserve, LibDeposit } from "../libraries/LibDeposit.sol";
import { Pausable } from "../util/Pausable.sol";
import { IDeposit } from "../interfaces/IDeposit.sol";
import { IAccessRegistry } from "../interfaces/IAccessRegistry.sol";
import { IBEP20 } from "../util/IBEP20.sol";


import "hardhat/console.sol";

contract Deposit is Pausable, IDeposit {
    event NewDeposit(
        address indexed account,
        bytes32 indexed market,
        bytes32 commitment,
        uint256 indexed amount,
        uint256 depositId,
        uint256 time
    );

    event DepositAdded(
        address indexed account,
        bytes32 indexed market,
        bytes32 commitment,
        uint256 indexed amount,
        uint256 depositId,
        uint256 time
    );
    event DepositWithdrawal(address indexed account, uint256 depositId, uint256 indexed amount, uint256 timestamp);

    receive() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    fallback() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    function hasAccount(address _account) external view override returns (bool) {
        LibDeposit._hasAccount(_account);
        return true;
    }

    function getDeposits(address account)
        external
        view
        returns (
            bytes32[] memory market,
            bytes32[] memory commitment,
            uint256[] memory amount,
            uint256[] memory savingsInterest
        )
    {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        ActiveDeposits storage activeDeposits = ds.getActiveDeposits[account];

        return (
            activeDeposits.market,
            activeDeposits.commitment,
            activeDeposits.amount,
            activeDeposits.savingsInterest
        );
    }

    function getDepositInterest(address account, uint256 id) external view returns (uint256 interest) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        uint256 num = id - 1;

        ActiveDeposits storage activeDeposits = ds.getActiveDeposits[account];

        bytes32 market = activeDeposits.market[num];
        bytes32 commitment = activeDeposits.commitment[num];
        uint256 interestFactor = 0;
        uint256 depositInterest;

        DepositRecords storage deposit = ds.indDepositRecord[account][market][commitment];
        YieldLedger storage yield = ds.indYieldRecord[account][market][commitment];

        interestFactor = LibDeposit._getDepositInterest(market, commitment, yield.oldLengthAccruedYield, yield.oldTime);
        depositInterest = yield.accruedYield;
        depositInterest += ((interestFactor * deposit.amount) / (365 * 86400 * 10000));
        return depositInterest;
    }

    function hasDeposit(bytes32 _market, bytes32 _commitment) external view override returns (bool) {
        LibDeposit._hasDeposit(msg.sender, _market, _commitment);
        return true;
    }

    function hasYield(bytes32 _market, bytes32 _commitment) external view override returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        YieldLedger storage yield = ds.indYieldRecord[msg.sender][_market][_commitment];
        LibDeposit._hasYield(yield);
        return true;
    }

    /// CREATE DEPOSIT
    function depositRequest(
        bytes32 _market,
        bytes32 _commitment,
        uint256 _amount
    ) external override nonReentrant returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        preDepositProcess(_market, _amount);

        if (!LibDeposit._hasDeposit(msg.sender, _market, _commitment)) {
            _createNewDeposit(msg.sender, _market, _commitment, _amount);
            return true;
        }
        // ds.token.approveFrom(msg.sender, address(this), _amount);
        ds.token.transferFrom(msg.sender, address(this), _amount); // change the address(this) to the diamond address.
        _processDeposit(msg.sender, _market, _commitment, _amount);
        LibReserve._updateReservesDeposit(_market, _amount, 0);
        emit DepositAdded(
            msg.sender,
            _market,
            _commitment,
            _amount,
            ds.indDepositRecord[msg.sender][_market][_commitment].id,
            block.timestamp
        );
        return true;
    }

    /// WITHRAW DEPOSIT
    function withdrawDeposit(
        bytes32 _market,
        bytes32 _commitment,
        uint256 _amount
    ) external override nonReentrant returns (bool) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        LibDeposit._hasAccount(msg.sender); // checks if user has savings account
        LibCommon._isMarketSupported(_market);

        SavingsAccount storage savingsAccount = ds.savingsPassbook[msg.sender];
        DepositRecords storage deposit = ds.indDepositRecord[msg.sender][_market][_commitment];
        ActiveDeposits storage activeDeposits = ds.getActiveDeposits[msg.sender];

        uint256 fees;
        uint256 initialAmount;
        uint256 commitmentTimelock;

        commitmentTimelock = ds.commitmentDays[_commitment] * 86400;
        // Convert commitment days into seconds for block.timestamp
        initialAmount = _amount;
        // Converts yield
        _convertYield(msg.sender, _market, _commitment);
        require(deposit.amount >= _amount, "ERROR: Insufficient balance");
        /// CONNECTS THE MARKET BELOW
        ds.token = IBEP20(LibCommon._connectMarket(_market));
        require(_amount >= 0, "ERROR: You cannot transfer less than 0 amount");

        if (_commitment != LibCommon._getCommitment(0, 0)) {
            if (deposit.isTimelockActivated == false) {
                /// ACTIVATES TIMELOCK IF IT WASN'T ALREADY
                deposit.isTimelockActivated = true;
                // deposit.createdAt = block.timestamp;
                deposit.activationTime = block.timestamp;

                savingsAccount.deposits[deposit.id - 1].isTimelockActivated = true;
                savingsAccount.deposits[deposit.id - 1].activationTime = block.timestamp;
                savingsAccount.deposits[deposit.id - 1].createdAt = block.timestamp;
                return false;
            }
            // CHECKS FOR TIMELOCK + buffer time of 3 days
            if (deposit.createdAt + commitmentTimelock >= block.timestamp) {
                // console.log("deposit.createdAt : ",deposit.createdAt);
                // console.log("commitmentTimelock : ",commitmentTimelock);
                // console.log("deposit.createdAt + commitmentTimelock : ",deposit.createdAt + commitmentTimelock);
                // console.log("block.timestamp : ",block.timestamp);
                // require(
                //     deposit.activationTime + deposit.timelockValidity <= block.timestamp,
                //     "3 days timelock has not passed yet"
                // );
                /*  CHARGE PRECLOSURE FEES */
                uint256 PreClosurefees = ((LibCommon.diamondStorage().depositPreClosureFees) * _amount) / 10000;
                require(_amount > PreClosurefees, "PreClosurefees is greater than amount");
                /// DEDUCTS PRECLOSURE IF TIMELOCK IS GOING ON
                _amount = _amount - PreClosurefees;
                require(_amount > 0, "Amount Post pre Fees cannot be 0 ");
                // NEED NOT TRANSFER FEES TO PROTOCOL AS IT ALREADY STAYS HERE
            }
        }

        require(
                    deposit.activationTime + deposit.timelockValidity <= block.timestamp,
                    "3 days timelock has not passed yet"
                );
        /* NOW IT DEDUCTS DEPOSIT WITHDRAW FEE */
        fees = ((LibCommon.diamondStorage().depositWithdrawalFees) * _amount) / 10000;
        require(_amount > fees, "Fees is greater than amount");
        _amount = _amount - fees;
        require(_amount > 0, "Amount Post Fees cannot be 0 ");
        // console.log("Fees deducted");
        ds.token.transfer(msg.sender, _amount);
        /// NEED NOT TRANSFER FEES TO PROTOCOL AS IT ALREADY STAYS HERE
        deposit.amount -= initialAmount;
        savingsAccount.deposits[deposit.id - 1].amount -= initialAmount;

        activeDeposits.amount[deposit.id - 1] -= initialAmount;
        activeDeposits.savingsInterest[deposit.id - 1] = 0;

        LibReserve._updateReservesDeposit(_market, initialAmount, 1);
        emit DepositWithdrawal(msg.sender, deposit.id, _amount, block.timestamp);
        return true;
    }

    function _createNewDeposit(
        address _sender,
        bytes32 _market,
        bytes32 _commitment,
        uint256 _amount
    ) private {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        SavingsAccount storage savingsAccount = ds.savingsPassbook[_sender];
        DepositRecords storage deposit = ds.indDepositRecord[_sender][_market][_commitment];
        YieldLedger storage yield = ds.indYieldRecord[_sender][_market][_commitment];
        ActiveDeposits storage activeDeposits = ds.getActiveDeposits[_sender];

        LibDeposit._ensureSavingsAccount(_sender, savingsAccount);

        ds.token.transferFrom(_sender, address(this), _amount);
        _processNewDeposit(_market, _commitment, _amount, savingsAccount, deposit, yield, activeDeposits);
        LibReserve._updateReservesDeposit(_market, _amount, 0);
        emit NewDeposit(_sender, _market, _commitment, _amount, deposit.id, block.timestamp);
    }

    function _processNewDeposit(
        // address _account,
        bytes32 _market,
        bytes32 _commitment,
        uint256 _amount,
        SavingsAccount storage savingsAccount,
        DepositRecords storage deposit,
        YieldLedger storage yield,
        ActiveDeposits storage activeDeposits
    ) private {
        // SavingsAccount storage savingsAccount = savingsPassbook[_account];
        // DepositRecords storage deposit = indDepositRecord[_account][_market][_commitment];
        // YieldLedger storage yield = indYieldRecord[_account][_market][_commitment];
        uint256 id;

        if (savingsAccount.deposits.length == 0) {
            id = 1;
        } else {
            id = savingsAccount.deposits.length + 1;
        }
        deposit.id = id;
        deposit.market = _market;
        deposit.commitment = _commitment;
        deposit.amount = _amount;
        deposit.createdAt = block.timestamp;

        if (_commitment != LibCommon._getCommitment(0, 0)) {
            yield.id = id;
            yield.market = bytes32(_market);
            yield.oldLengthAccruedYield = LibCommon._getApyTimeLength(_market, _commitment);
            yield.oldTime = block.timestamp;
            yield.accruedYield = 0;
            deposit.isTimelockApplicable = true;
            deposit.isTimelockActivated = false;
            deposit.timelockValidity = LibCommon._getTimelockValidityDeposit();
            console.log("timelockValidity : ",deposit.timelockValidity);
            deposit.activationTime = 0;
        } else {
            yield.id = id;
            yield.market = _market;
            yield.oldLengthAccruedYield = LibCommon._getApyTimeLength(_market, _commitment);
            yield.oldTime = block.timestamp;
            yield.accruedYield = 0;
            deposit.isTimelockApplicable = false;
            deposit.isTimelockActivated = true;
            deposit.timelockValidity = 0;
            deposit.activationTime = 0;
        }

        savingsAccount.deposits.push(deposit);
        savingsAccount.yield.push(yield);

        activeDeposits.id.push(id - 1);
        activeDeposits.market.push(_market);
        activeDeposits.commitment.push(_commitment);
        activeDeposits.amount.push(_amount);
        activeDeposits.savingsInterest.push(yield.accruedYield);
    }

    function accruedYield(
        address _account,
        bytes32 _market,
        bytes32 _commitment
    ) private {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        LibDeposit._hasDeposit(_account, _market, _commitment);

        uint256 aggregateYield;

        SavingsAccount storage savingsAccount = ds.savingsPassbook[_account];
        DepositRecords storage deposit = ds.indDepositRecord[_account][_market][_commitment];
        YieldLedger storage yield = ds.indYieldRecord[_account][_market][_commitment];
        // console.log('_market', _market);
        // console.log('_commitment', _commitment);
        (yield.oldLengthAccruedYield, yield.oldTime, aggregateYield) = LibDeposit._calcAPY(
            _market,
            _commitment,
            yield.oldLengthAccruedYield,
            yield.oldTime,
            aggregateYield
        );

        aggregateYield = (aggregateYield * deposit.amount) / (365 * 86400 * 10000);

        yield.accruedYield += aggregateYield;
        savingsAccount.yield[deposit.id - 1].accruedYield += aggregateYield;
    }

    /// DELEGATED CALL - PROCESS DEPOSIT
    function _processDeposit(
        address _account,
        bytes32 _market,
        bytes32 _commitment,
        uint256 _amount
    ) private {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        SavingsAccount storage savingsAccount = ds.savingsPassbook[_account];
        DepositRecords storage deposit = ds.indDepositRecord[_account][_market][_commitment];
        YieldLedger storage yield = ds.indYieldRecord[_account][_market][_commitment];
        ActiveDeposits storage activeDeposits = ds.getActiveDeposits[_account];

        uint256 num = deposit.id - 1;

        accruedYield(_account, _market, _commitment);

        deposit.amount += _amount;
        deposit.createdAt = block.timestamp;
        deposit.activationTime = block.timestamp;
        deposit.isTimelockActivated = false;

        savingsAccount.deposits[num].amount += _amount;
        savingsAccount.deposits[num].createdAt = block.timestamp;

        savingsAccount.yield[num].oldLengthAccruedYield = yield.oldLengthAccruedYield;
        savingsAccount.yield[num].oldTime = yield.oldTime;
        savingsAccount.yield[num].accruedYield = yield.accruedYield;

        activeDeposits.market[num] = _market;
        activeDeposits.commitment[num] = _commitment;
        activeDeposits.amount[num] += _amount;
        activeDeposits.savingsInterest[num] = yield.accruedYield;
    }

    function preDepositProcess(bytes32 _market, uint256 _amount) private {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        LibCommon._isMarketSupported(_market);
        ds.token = IBEP20(LibCommon._connectMarket(_market));
        LibCommon._minAmountCheck(_market, _amount);
    }

    function _convertYield(
        address _account,
        bytes32 _market,
        bytes32 _commitment
    ) private {
        AppStorageOpen storage ds = LibCommon.diamondStorage();

        LibDeposit._hasAccount(_account);

        SavingsAccount storage savingsAccount = ds.savingsPassbook[_account];
        DepositRecords storage deposit = ds.indDepositRecord[_account][_market][_commitment];
        YieldLedger storage yield = ds.indYieldRecord[_account][_market][_commitment];
        LibDeposit._hasYield(yield);
        accruedYield(_account, _market, _commitment);

        deposit.amount += yield.accruedYield;


        /// RESETTING THE YIELD.
        yield.accruedYield = 0;

        savingsAccount.deposits[deposit.id - 1].amount = deposit.amount;
        savingsAccount.yield[deposit.id - 1].accruedYield = 0;
    }

    function pauseDeposit() external override authDeposit {
        _pause();
    }

    function unpauseDeposit() external override authDeposit {
        _unpause();
    }

    function isPausedDeposit() external view virtual override returns (bool) {
        return _paused();
    }

    modifier authDeposit() {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        require(
            IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.superAdmin, msg.sender) ||
                IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.adminDeposit, msg.sender),
            "ERROR: Not an admin"
        );
        _;
    }
}
