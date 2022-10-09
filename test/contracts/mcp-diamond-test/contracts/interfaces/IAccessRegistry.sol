// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface IAccessRegistry {
    function hasRole(bytes32 role, address account) external view returns (bool);

    function addRole(bytes32 role, address account) external;

    function removeRole(bytes32 role, address account) external;

    function renounceRole(bytes32 role, address account) external;

    function transferRole(
        bytes32 role,
        address oldAccount,
        address newAccount
    ) external;

    function hasAdminRole(bytes32 role, address account) external view returns (bool);

    function addAdminRole(bytes32 role, address account) external;

    function removeAdminRole(bytes32 role, address account) external;

    function adminRoleTransfer(
        bytes32 role,
        address oldAccount,
        address newAccount
    ) external;

    function adminRoleRenounce(bytes32 role, address account) external;

    function pauseAccessRegistry() external;

    function unpauseAccessRegistry() external;

    function isPausedAccessRegistry() external view returns (bool);
}
