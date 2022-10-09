// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { AppStorageOpen } from "../libraries/AppStorageOpen.sol";
import { LibDiamond } from "../libraries/LibDiamond.sol";
import { IDiamondLoupe } from "../interfaces/IDiamondLoupe.sol";
import { IDiamondCut } from "../interfaces/IDiamondCut.sol";

// It is exapected that this contract is customized if you want to deploy your diamond
// with data from a deployment script. Use the init function to initialize state variables
// of your diamond. Add parameters to the init funciton if you need to.

contract DiamondInit {
    AppStorageOpen internal s;

    // You can add parameters to this function in order to pass in
    // data to set your own state variables
    function init(
        address account,
        address reserveAddr,
        address accessRegistry
    ) external {
        // adding ERC165 data
        LibDiamond.DiamondStorage storage ds = LibDiamond.diamondStorage();
        ds.supportedInterfaces[type(IDiamondCut).interfaceId] = true;
        ds.supportedInterfaces[type(IDiamondLoupe).interfaceId] = true;

        s.superAdmin = 0x41636365737352656769737472792e61646d696e000000000000000000000000; //bytes32("AccessRegistry.admin");
        s.superAdminAddress = accessRegistry;

        s.adminComptroller = 0x61646d696e436f6d7074726f6c6c657200000000000000000000000000000000;
        s.adminDeposit = 0x61646d696e4465706f7369740000000000000000000000000000000000000000;
        s.adminLoan = 0x61646d696e4c6f616e0000000000000000000000000000000000000000000000;
        s.adminLoan1 = 0x61646d696e4c6f616e3100000000000000000000000000000000000000000000;
        s.adminLoan2 = 0x61646d696e4c6f616e3200000000000000000000000000000000000000000000;
        s.adminLiquidator = 0x61646d696e4c697175696461746f720000000000000000000000000000000000;
        s.adminOpenOracle = 0x61646d696e4f70656e4f7261636c650000000000000000000000000000000000;
        s.adminReserve = 0x61646d696e526573657276650000000000000000000000000000000000000000;
        s.adminTokenList = 0x61646d696e546f6b656e4c697374000000000000000000000000000000000000;
        s.protocolOwnedLiquidator = 0x70726f746f636f6c4f776e65644c697175696461746f72000000000000000000;
        s.adminDynamicInterest = 0x61646d696e44796e616d6963496e746572657374000000000000000000000000;

        s.reserveAddress = reserveAddr;
        s.upgradeAdmin = account;
        ds.upgradeAdmin = account;
    }
}
