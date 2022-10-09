// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { IDiamondCut } from "../interfaces/IDiamondCut.sol";
import { LibDiamond } from "../libraries/LibDiamond.sol";

import "../util/Pausable.sol";

contract DiamondCutFacet is Pausable, IDiamondCut {
    function diamondCut(
        FacetCut[] calldata _diamondCut,
        address _init,
        bytes calldata _calldata
    ) external override nonReentrant {
        LibDiamond.enforceIsupgradeAdmin();
        LibDiamond.diamondCut(_diamondCut, _init, _calldata);
    }
}
