// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;
import { Pausable } from "./util/Pausable.sol";
import { IAccessRegistry } from "./interfaces/IAccessRegistry.sol";

struct RoleData {
    mapping(address => bool) _members;
}
struct AdminRoleData {
    mapping(address => bool) _adminMembers;
}

contract AccessRegistry is Pausable, IAccessRegistry {
    mapping(bytes32 => RoleData) roles;
    mapping(bytes32 => AdminRoleData) adminRoles;
    bytes32 superAdmin;

    event AdminRoleDataGranted(bytes32 indexed role, address indexed account, address indexed sender);
    event AdminRoleDataRevoked(bytes32 indexed role, address indexed account, address indexed sender);
    event RoleGranted(bytes32 indexed role, address indexed account, address indexed sender);
    event RoleRevoked(bytes32 indexed role, address indexed account, address indexed sender);

    constructor(address upgradeAdmin) {
        superAdmin = 0x41636365737352656769737472792e61646d696e000000000000000000000000;
        adminRoles[superAdmin]._adminMembers[upgradeAdmin] = true;

        addAdminRole(superAdmin, address(this));
    }

    function hasRole(bytes32 role, address account) public view override returns (bool) {
        return roles[role]._members[account];
    }

    function addRole(bytes32 role, address account) public override onlyAdmin {
        require(!hasRole(role, account), "Role already exists. Please create a different role");
        roles[role]._members[account] = true;
        emit RoleGranted(role, account, msg.sender);
    }

    function removeRole(bytes32 role, address account) external override nonReentrant onlyAdmin {
        require(hasRole(role, account), "Role does not exist.");
        revokeRole(role, account);
    }

    function renounceRole(bytes32 role, address account) external override nonReentrant {
        require(hasRole(role, account), "Role does not exist.");
        require(_msgSender() == account, "Inadequate permissions");
        revokeRole(role, account);
    }

    function revokeRole(bytes32 role, address account) private {
        roles[role]._members[account] = false;
        emit RoleRevoked(role, account, msg.sender);
    }

    function transferRole(
        bytes32 role,
        address oldAccount,
        address newAccount
    ) external override nonReentrant {
        require(hasRole(role, oldAccount) && _msgSender() == oldAccount, "Role does not exist.");
        revokeRole(role, oldAccount);
        addRole(role, newAccount);
    }

    function hasAdminRole(bytes32 role, address account) public view override returns (bool) {
        return adminRoles[role]._adminMembers[account];
    }

    function addAdminRole(bytes32 role, address account) public override onlyAdmin {
        require(!hasAdminRole(role, account), "Role already exists. Please create a different role");
        adminRoles[role]._adminMembers[account] = true;
        emit AdminRoleDataGranted(role, account, msg.sender);
    }

    function removeAdminRole(bytes32 role, address account) external override nonReentrant onlyAdmin {
        require(hasAdminRole(role, account), "Role does not exist.");
        revokeAdmin(role, account);
    }

    function revokeAdmin(bytes32 role, address account) private {
        adminRoles[role]._adminMembers[account] = false;
        emit AdminRoleDataRevoked(role, account, msg.sender);
    }

    function adminRoleTransfer(
        bytes32 role,
        address oldAccount,
        address newAccount
    ) external override nonReentrant onlyAdmin {
        require(hasAdminRole(role, oldAccount), "Role already exists. Please create a different role");
        revokeAdmin(role, oldAccount);
        addAdminRole(role, newAccount);
    }

    function adminRoleRenounce(bytes32 role, address account) external override nonReentrant onlyAdmin {
        require(hasAdminRole(role, account), "Role does not exist.");
        require(_msgSender() == account, "Inadequate permissions");
        revokeAdmin(role, account);
    }

    modifier onlyAdmin() {
        require(hasAdminRole(superAdmin, msg.sender), "ERROR: Not an admin");
        _;
    }

    function pauseAccessRegistry() external override onlyAdmin {
        _pause();
    }

    function unpauseAccessRegistry() external override onlyAdmin {
        _unpause();
    }

    function isPausedAccessRegistry() external view virtual override returns (bool) {
        return _paused();
    }
}
