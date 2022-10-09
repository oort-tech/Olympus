// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { AppStorageOpen, LibCommon, LibSwap, LibLiquidation } from "../libraries/LibLiquidation.sol";
import { Pausable } from "../util/Pausable.sol";
import { ILiquidator } from "../interfaces/ILiquidator.sol";
import { IAccessRegistry } from "../interfaces/IAccessRegistry.sol";

contract Liquidator is Pausable, ILiquidator {
    event Liquidation(
        address indexed account,
        bytes32 indexed market,
        bytes32 indexed commitment,
        uint256 amount,
        uint256 time
    );

    receive() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    fallback() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    function liquidation(
        address account,
        bytes32 _market,
        bytes32 _commitment
    ) external override nonReentrant returns (bool success) {
        uint256 amount = LibLiquidation._liquidation(msg.sender, account, _market, _commitment);
        emit Liquidation(account, _market, _commitment, amount, block.timestamp);
        return true;
    }

    function liquidableLoans(uint256 _indexFrom)
        external
        view
        override
        returns (
            address[] memory loanOwner,
            bytes32[] memory loanMarket,
            bytes32[] memory loanCommitment,
            uint256[] memory loanAmount,
            bytes32[] memory collateralMarket,
            uint256[] memory collateralAmount
        )
    {
        return LibLiquidation._liquidableLoans(_indexFrom);
    }

    function pauseLiquidator() external override authLiquidator {
        _pause();
    }

    function unpauseLiquidator() external override authLiquidator {
        _unpause();
    }

    function isPausedLiquidator() external view virtual override returns (bool) {
        return _paused();
    }

    modifier authLiquidator() {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        require(
            IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.superAdmin, msg.sender) ||
                IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.adminLiquidator, msg.sender),
            "ERROR: Not an admin"
        );
        _;
    }
}
