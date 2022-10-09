// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { AppStorageOpen, LibCommon, LibReserve } from "../libraries/LibReserve.sol";
import { Pausable } from "../util/Pausable.sol";
import { IReserve } from "../interfaces/IReserve.sol";
import { IBEP20 } from "../util/IBEP20.sol";
import { IAccessRegistry } from "../interfaces/IAccessRegistry.sol";

contract Reserve is Pausable, IReserve {
    receive() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    fallback() external payable {
        payable(LibCommon.upgradeAdmin()).transfer(msg.value);
    }

    function transferAnyBEP20(
        address _token,
        address _recipient,
        uint256 _value
    ) external override nonReentrant authReserve returns (bool) {
        IBEP20(_token).transfer(_recipient, _value);
        return true;
    }

    function avblMarketReserves(bytes32 _market) external view override returns (uint256) {
        return LibReserve._avblMarketReserves(_market);
    }

    function marketReserves(bytes32 _market) external view override returns (uint256) {
        return LibReserve._marketReserves(_market);
    }

    function marketUtilisation(bytes32 _market) external view override returns (uint256) {
        return LibReserve._marketUtilisation(_market);
    }

    function avblReservesDeposit(bytes32 _market) external view override returns (uint256) {
        return LibReserve._avblReservesDeposit(_market);
    }

    function utilisedReservesDeposit(bytes32 _market) external view override returns (uint256) {
        return LibReserve._utilisedReservesDeposit(_market);
    }

    modifier authReserve() {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        require(
            IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.superAdmin, msg.sender) ||
                IAccessRegistry(ds.superAdminAddress).hasAdminRole(ds.adminReserve, msg.sender),
            "ERROR: Not an admin"
        );
        _;
    }

    function pauseReserve() external override authReserve {
        _pause();
    }

    function unpauseReserve() external override authReserve {
        _unpause();
    }

    function isPausedReserve() external view virtual override returns (bool) {
        return _paused();
    }
}
